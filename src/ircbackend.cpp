#include "ircbackend.h"
#include "appbranding.h"

#include <QAbstractSocket>
#include <QDateTime>
#include <QHostAddress>
#include <QHostInfo>
#include <QMutexLocker>
#include <QSslSocket>
#include <QTcpSocket>

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>

IrcBackend::IrcBackend(ConnectionSettings settings, QObject *parent)
    : ChatBackend(std::move(settings), parent),
      m_nickname(m_settings.username)
{
    for (const QString &name : m_settings.ircBuddies) {
        if (!name.trimmed().isEmpty()) m_watchBuddies.insert(name.trimmed());
    }
}

IrcBackend::~IrcBackend()
{
    m_stopRequested = true;
    if (m_thread && m_thread != QThread::currentThread()) {
        m_thread->wait();
    }
}

void IrcBackend::setConnectionSettings(const ConnectionSettings &settings)
{
    ChatBackend::setConnectionSettings(settings);
    m_nickname = m_settings.username;
    m_watchBuddies.clear();
    m_onlineWatchBuddies.clear();
    for (const QString &name : m_settings.ircBuddies) {
        if (!name.trimmed().isEmpty()) m_watchBuddies.insert(name.trimmed());
    }
}

void IrcBackend::start()
{
    if (m_thread) {
        return;
    }
    m_stopRequested = false;
    m_thread = QThread::create([this] { run(); });
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, this, [this] { m_thread = nullptr; }, Qt::QueuedConnection);
    m_thread->start();
}

void IrcBackend::stop()
{
    m_stopRequested = true;
    QThread *thread = m_thread;
    if (thread && thread != QThread::currentThread()) {
        thread->wait(15000);
    }
}

void IrcBackend::enqueue(Command command)
{
    QMutexLocker locker(&m_commandMutex);
    m_commands.enqueue(std::move(command));
}

QList<IrcBackend::Command> IrcBackend::takeCommands()
{
    QList<Command> result;
    QMutexLocker locker(&m_commandMutex);
    while (!m_commands.isEmpty()) {
        result.push_back(m_commands.dequeue());
    }
    return result;
}

void IrcBackend::sendPrivateMessage(const QString &target, const QString &message)
{
    enqueue({CommandType::SendIm, target, message});
}

void IrcBackend::joinRoom(const QString &room, bool)
{
    enqueue({CommandType::Join, room, {}});
}

void IrcBackend::sendRoomMessage(const QString &room, const QString &message)
{
    enqueue({CommandType::SendRoom, room, message});
}

void IrcBackend::leaveRoom(const QString &room)
{
    enqueue({CommandType::Part, room, {}});
}

void IrcBackend::sendRaw(const QString &line, const QString &, const QString &)
{
    // IRC's wire protocol does not use the leading slash that interactive
    // clients use for commands.  Accept either spelling so GUI/CLI users can
    // type "/PART #channel" or "PART #channel" without accidentally
    // transmitting an invalid literal slash to the server.
    QString clean = line.trimmed();
    if (clean.startsWith(QLatin1Char('/'))) {
        clean.remove(0, 1);
        clean = clean.trimmed();
    }

    // IRC commands are conventionally uppercase.  Only normalize the command
    // token; preserve all arguments/trailing text exactly as entered.
    int commandEnd = 0;
    while (commandEnd < clean.size() && !clean.at(commandEnd).isSpace()) {
        ++commandEnd;
    }
    if (commandEnd > 0) {
        clean.replace(0, commandEnd, clean.left(commandEnd).toUpper());
    }

    enqueue({CommandType::Raw, clean, {}});
}

void IrcBackend::changeNickname(const QString &newNick)
{
    enqueue({CommandType::Nick, newNick, {}});
}

void IrcBackend::addBuddy(const QString &name)
{
    const QString clean = name.trimmed();
    if (clean.isEmpty()) return;
    bool found = false;
    for (const QString &existing : m_settings.ircBuddies) {
        if (existing.compare(clean, Qt::CaseInsensitive) == 0) { found = true; break; }
    }
    if (!found) m_settings.ircBuddies.append(clean);
    m_settings.ircBuddies.sort(Qt::CaseInsensitive);
    emit buddyListChanged(m_settings.ircBuddies);
    enqueue({CommandType::WatchAdd, clean, {}});
}

void IrcBackend::removeBuddy(const QString &name)
{
    const QString clean = name.trimmed();
    if (clean.isEmpty()) return;
    for (auto it = m_settings.ircBuddies.begin(); it != m_settings.ircBuddies.end();) {
        if (it->compare(clean, Qt::CaseInsensitive) == 0) it = m_settings.ircBuddies.erase(it);
        else ++it;
    }
    emit buddyListChanged(m_settings.ircBuddies);
    enqueue({CommandType::WatchRemove, clean, {}});
}

bool IrcBackend::isChannel(const QString &target)
{
    if (target.isEmpty()) {
        return false;
    }
    return QStringLiteral("#&+!").contains(target.front());
}

QString IrcBackend::canonicalChannel(QString room)
{
    room = room.trimmed();
    if (!room.isEmpty() && !isChannel(room)) {
        room.prepend(QLatin1Char('#'));
    }
    return room;
}

QString IrcBackend::nickFromPrefix(const QString &prefix)
{
    return prefix.section(QLatin1Char('!'), 0, 0);
}

QString IrcBackend::stripMemberPrefix(QString nick)
{
    while (!nick.isEmpty() && QStringLiteral("~&@%+").contains(nick.front())) {
        nick.remove(0, 1);
    }
    return nick;
}


QString IrcBackend::stripFormatting(const QString &text)
{
    // IRC formatting controls are transport-level decoration.  The TUI and
    // GUI share this backend, so normalize them here instead of allowing raw
    // C0 bytes to appear as ^B/^O/etc. in topics or conversation text.
    QString out;
    out.reserve(text.size());

    for (qsizetype i = 0; i < text.size(); ++i) {
        const QChar ch = text.at(i);
        const ushort u = ch.unicode();

        // Common IRC toggles: bold, reset, monospace, reverse, italic,
        // strikethrough and underline.
        if (u == 0x02 || u == 0x0F || u == 0x11 || u == 0x16
            || u == 0x1D || u == 0x1E || u == 0x1F) {
            continue;
        }

        // mIRC numeric colors: ^C[fg][,bg], with one or two decimal digits.
        if (u == 0x03) {
            int digits = 0;
            while (i + 1 < text.size() && digits < 2 && text.at(i + 1).isDigit()) {
                ++i;
                ++digits;
            }
            if (i + 1 < text.size() && text.at(i + 1) == QLatin1Char(',')) {
                const qsizetype comma = i + 1;
                qsizetype j = comma + 1;
                int bgDigits = 0;
                while (j < text.size() && bgDigits < 2 && text.at(j).isDigit()) {
                    ++j;
                    ++bgDigits;
                }
                if (bgDigits > 0) i = j - 1;
            }
            continue;
        }

        // IRCv3 hex colors: ^Drrggbb[,rrggbb].  Only consume a color when a
        // complete six-digit hex value follows so ordinary text is preserved.
        if (u == 0x04) {
            auto isHex = [](QChar c) {
                const ushort v = c.unicode();
                return (v >= '0' && v <= '9') || (v >= 'a' && v <= 'f')
                    || (v >= 'A' && v <= 'F');
            };
            qsizetype j = i + 1;
            bool foreground = j + 6 <= text.size();
            for (qsizetype k = 0; foreground && k < 6; ++k) foreground = isHex(text.at(j + k));
            if (foreground) {
                i = j + 5;
                if (i + 1 < text.size() && text.at(i + 1) == QLatin1Char(',')) {
                    j = i + 2;
                    bool background = j + 6 <= text.size();
                    for (qsizetype k = 0; background && k < 6; ++k) background = isHex(text.at(j + k));
                    if (background) i = j + 5;
                }
            }
            continue;
        }

        // Do not allow embedded CR/LF or other C0 controls to leak into the
        // curses/Qt renderer. Preserve TAB for readable server text.
        if ((u < 0x20 && u != 0x09) || u == 0x7F) {
            continue;
        }

        out.append(ch);
    }
    return out;
}

IrcBackend::ParsedLine IrcBackend::parseLine(const QString &line)
{
    ParsedLine parsed;
    QString rest = line;

    if (rest.startsWith(QLatin1Char(':'))) {
        const int space = rest.indexOf(QLatin1Char(' '));
        if (space < 0) {
            return parsed;
        }
        parsed.prefix = rest.mid(1, space - 1);
        rest = rest.mid(space + 1);
    }

    QString trailing;
    const int trailingPos = rest.indexOf(QStringLiteral(" :"));
    if (trailingPos >= 0) {
        trailing = rest.mid(trailingPos + 2);
        rest = rest.left(trailingPos);
    }

    QStringList parts = rest.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    if (parts.isEmpty()) {
        return parsed;
    }

    parsed.command = parts.takeFirst().toUpper();
    parsed.params = parts;
    if (trailingPos >= 0) {
        parsed.params.push_back(trailing);
    }
    return parsed;
}

void IrcBackend::sendLine(QAbstractSocket &socket, const QString &line)
{
    QString clean = line;
    clean.remove(QLatin1Char('\r'));
    clean.remove(QLatin1Char('\n'));

    if (m_settings.debug) {
        emit eventReceived(QStringLiteral("status"), QString(),
                           QStringLiteral("[IRC =>] %1").arg(clean));
    }

    const QByteArray bytes = clean.toUtf8() + "\r\n";
    qint64 offset = 0;
    while (offset < bytes.size()) {
        const qint64 written = socket.write(bytes.constData() + offset, bytes.size() - offset);
        if (written < 0) {
            throw std::runtime_error(
                QStringLiteral("IRC write failed: %1").arg(socket.errorString()).toStdString());
        }
        offset += written;
        if (socket.bytesToWrite() > 0 && !socket.waitForBytesWritten(5000)) {
            throw std::runtime_error(
                QStringLiteral("IRC write timed out: %1").arg(socket.errorString()).toStdString());
        }
    }
}

QString IrcBackend::readLine(QAbstractSocket &socket, int timeoutMs)
{
    while (!socket.canReadLine()) {
        if (!socket.waitForReadyRead(timeoutMs)) {
            if (socket.state() != QAbstractSocket::ConnectedState) {
                throw std::runtime_error("IRC server closed the connection");
            }
            return {};
        }
    }
    QString line = QString::fromUtf8(socket.readLine());
    while (line.endsWith(QLatin1Char('\n')) || line.endsWith(QLatin1Char('\r'))) {
        line.chop(1);
    }
    return line;
}

void IrcBackend::addMembers(const QString &room, const QStringList &names)
{
    const QString key = room.toCaseFolded();
    m_roomNames[key] = room;
    auto &members = m_members[key];
    for (const QString &name : names) {
        if (!name.isEmpty()) {
            members.insert(name);
        }
    }
    emit membersChanged(room, QStringLiteral("add"), names);
}

void IrcBackend::removeMembers(const QString &room, const QStringList &names)
{
    const QString key = room.toCaseFolded();
    auto it = m_members.find(key);
    if (it != m_members.end()) {
        for (const QString &name : names) {
            for (auto memberIt = it->begin(); memberIt != it->end();) {
                if (memberIt->compare(name, Qt::CaseInsensitive) == 0) {
                    memberIt = it.value().erase(memberIt);
                } else {
                    ++memberIt;
                }
            }
        }
    }
    emit membersChanged(m_roomNames.value(key, room), QStringLiteral("remove"), names);
}

void IrcBackend::replaceMembers(const QString &room, const QStringList &names)
{
    const QString key = room.toCaseFolded();
    m_roomNames[key] = room;
    QSet<QString> replacement;
    for (const QString &name : names) {
        if (!name.isEmpty()) {
            replacement.insert(name);
        }
    }
    m_members[key] = replacement;
    emit membersChanged(room, QStringLiteral("replace"), names);
}

void IrcBackend::processLine(const QString &line)
{
    if (m_settings.debug) {
        emit eventReceived(QStringLiteral("status"), QString(),
                           QStringLiteral("[IRC <=] %1").arg(line));
    }

    const ParsedLine parsed = parseLine(line);
    const QString nick = nickFromPrefix(parsed.prefix);

    if (parsed.command == QStringLiteral("PRIVMSG") && parsed.params.size() >= 2) {
        const QString target = parsed.params[0];
        const QString text = stripFormatting(parsed.params[1]);
        if (isChannel(target)) {
            emit eventReceived(QStringLiteral("chat"), target,
                               QStringLiteral("<%1> %2").arg(nick, text));
        } else {
            emit eventReceived(QStringLiteral("im"), nick,
                               QStringLiteral("<%1> %2").arg(nick, text));
        }
        return;
    }

    if (parsed.command == QStringLiteral("NOTICE") && parsed.params.size() >= 2) {
        const QString target = parsed.params[0];
        const QString sender = nick.isEmpty() ? parsed.prefix : nick;
        if (isChannel(target)) {
            emit eventReceived(QStringLiteral("chat"), target,
                               QStringLiteral("-%1- %2").arg(sender, stripFormatting(parsed.params[1])));
        } else {
            emit eventReceived(QStringLiteral("status"), QString(),
                               QStringLiteral("-%1- %2").arg(sender, stripFormatting(parsed.params[1])));
        }
        return;
    }

    if (parsed.command == QStringLiteral("JOIN") && !parsed.params.isEmpty()) {
        const QString room = parsed.params.back();
        addMembers(room, {nick});
        emit eventReceived(QStringLiteral("chat"), room,
                           QStringLiteral("*** %1 joined").arg(nick));
        return;
    }

    if (parsed.command == QStringLiteral("PART") && !parsed.params.isEmpty()) {
        const QString room = parsed.params[0];
        removeMembers(room, {nick});
        const QString reason = parsed.params.size() >= 2 ? stripFormatting(parsed.params[1]) : QString();
        emit eventReceived(QStringLiteral("chat"), room,
                           QStringLiteral("*** %1 left%2")
                               .arg(nick,
                                    reason.isEmpty() ? QString()
                                                     : QStringLiteral(" (%1)").arg(reason)));
        return;
    }

    if (parsed.command == QStringLiteral("QUIT")) {
        const QString reason = parsed.params.isEmpty() ? QString() : stripFormatting(parsed.params[0]);
        for (auto it = m_members.begin(); it != m_members.end(); ++it) {
            QString actual;
            for (const QString &member : it.value()) {
                if (member.compare(nick, Qt::CaseInsensitive) == 0) {
                    actual = member;
                    break;
                }
            }
            if (!actual.isEmpty()) {
                const QString room = m_roomNames.value(it.key(), it.key());
                removeMembers(room, {actual});
                emit eventReceived(QStringLiteral("chat"), room,
                                   QStringLiteral("*** %1 quit%2")
                                       .arg(actual,
                                            reason.isEmpty() ? QString()
                                                             : QStringLiteral(" (%1)").arg(reason)));
            }
        }
        return;
    }

    if (parsed.command == QStringLiteral("NICK") && !parsed.params.isEmpty()) {
        const QString newNick = parsed.params.back();
        const QString oldNick = nick;
        if (oldNick.compare(m_nickname, Qt::CaseInsensitive) == 0) {
            m_nickname = newNick;
        }

        for (auto it = m_members.begin(); it != m_members.end(); ++it) {
            QString actual;
            for (const QString &member : it.value()) {
                if (member.compare(oldNick, Qt::CaseInsensitive) == 0) {
                    actual = member;
                    break;
                }
            }
            if (!actual.isEmpty()) {
                it.value().remove(actual);
                it.value().insert(newNick);
                const QString room = m_roomNames.value(it.key(), it.key());
                emit membersChanged(room, QStringLiteral("remove"), {actual});
                emit membersChanged(room, QStringLiteral("add"), {newNick});
                emit eventReceived(QStringLiteral("chat"), room,
                                   QStringLiteral("*** %1 is now known as %2")
                                       .arg(actual, newNick));
            }
        }
        return;
    }

    // RPL_NAMREPLY: <me> <symbol> <channel> :names...
    if (parsed.command == QStringLiteral("353") && parsed.params.size() >= 4) {
        const QString room = parsed.params[2];
        const QString key = room.toCaseFolded();
        m_roomNames[key] = room;
        auto &pending = m_pendingNames[key];
        for (QString member : parsed.params[3].split(QLatin1Char(' '), Qt::SkipEmptyParts)) {
            member = stripMemberPrefix(member);
            if (!member.isEmpty()) {
                pending.insert(member);
            }
        }
        return;
    }

    // RPL_ENDOFNAMES
    if (parsed.command == QStringLiteral("366") && parsed.params.size() >= 2) {
        const QString room = parsed.params[1];
        const QString key = room.toCaseFolded();
        const QSet<QString> pending = m_pendingNames.take(key);
        QStringList names = pending.values();
        names.sort(Qt::CaseInsensitive);
        replaceMembers(room, names);
        return;
    }

    if (parsed.command == QStringLiteral("KICK") && parsed.params.size() >= 2) {
        const QString room = parsed.params[0];
        const QString victim = parsed.params[1];
        const QString reason = parsed.params.size() >= 3 ? stripFormatting(parsed.params[2]) : QString();
        removeMembers(room, {victim});
        emit eventReceived(QStringLiteral("chat"), room,
                           QStringLiteral("*** %1 was kicked by %2%3")
                               .arg(victim,
                                    nick,
                                    reason.isEmpty() ? QString()
                                                     : QStringLiteral(" (%1)").arg(reason)));
        return;
    }

    if (parsed.command == QStringLiteral("TOPIC") && parsed.params.size() >= 2) {
        emit eventReceived(QStringLiteral("chat"), parsed.params[0],
                           QStringLiteral("*** topic set by %1: %2")
                               .arg(nick, stripFormatting(parsed.params[1])));
        return;
    }

    if (parsed.command == QStringLiteral("332") && parsed.params.size() >= 3) {
        emit eventReceived(QStringLiteral("chat"), parsed.params[1],
                           QStringLiteral("*** topic: %1").arg(stripFormatting(parsed.params[2])));
        return;
    }

    // RPL_TOPICWHOTIME: <me> <channel> <setter> <unix-time>
    if (parsed.command == QStringLiteral("333") && parsed.params.size() >= 4) {
        bool ok = false;
        const qint64 epoch = parsed.params[3].toLongLong(&ok);
        QString when = parsed.params[3];
        if (ok) {
            when = QDateTime::fromSecsSinceEpoch(epoch).toLocalTime()
                       .toString(QStringLiteral("ddd MMM d HH:mm:ss yyyy"));
        }
        emit eventReceived(QStringLiteral("chat"), parsed.params[1],
                           QStringLiteral("*** topic set by %1 [%2]")
                               .arg(stripFormatting(parsed.params[2]), when));
        return;
    }

    // RPL_ISON: <me> :nick1 nick2 ...
    // The list is used for the local per-profile IRC buddy/watch feature.
    if (parsed.command == QStringLiteral("303") && parsed.params.size() >= 2) {
        QSet<QString> online;
        for (const QString &name : parsed.params.back().split(QLatin1Char(' '), Qt::SkipEmptyParts))
            online.insert(name.toCaseFolded());
        for (const QString &watched : m_watchBuddies) {
            const QString folded = watched.toCaseFolded();
            const bool isOnline = online.contains(folded);
            const bool wasOnline = m_onlineWatchBuddies.contains(folded);
            if (isOnline != wasOnline) emit buddyPresenceChanged(watched, isOnline);
        }
        m_onlineWatchBuddies = online;
        return;
    }

    static const QSet<QString> interestingErrors = {
        QStringLiteral("401"), QStringLiteral("403"), QStringLiteral("404"),
        QStringLiteral("405"), QStringLiteral("442"), QStringLiteral("443"),
        QStringLiteral("471"), QStringLiteral("473"), QStringLiteral("474"),
        QStringLiteral("475")
    };
    if (interestingErrors.contains(parsed.command)) {
        emit eventReceived(QStringLiteral("status"), QString(),
                           QStringLiteral("[IRC %1] %2")
                               .arg(parsed.command, parsed.params.join(QLatin1Char(' '))));
    }
}

void IrcBackend::run()
{
    QString disconnectReason = QStringLiteral("signed off");

    try {
        emit eventReceived(QStringLiteral("status"), QString(),
                           QStringLiteral("[IRC] Resolving %1…").arg(m_settings.server));

        QList<QHostAddress> addresses;
        QHostAddress literalAddress;
        if (literalAddress.setAddress(m_settings.server)) {
            addresses.append(literalAddress);
        } else {
            const QHostInfo info = QHostInfo::fromName(m_settings.server);
            if (info.error() != QHostInfo::NoError) {
                throw std::runtime_error(
                    QStringLiteral("IRC DNS lookup failed for %1: %2")
                        .arg(m_settings.server, info.errorString()).toStdString());
            }
            addresses = info.addresses();
        }

        if (addresses.isEmpty()) {
            throw std::runtime_error(
                QStringLiteral("IRC DNS lookup returned no addresses for %1")
                    .arg(m_settings.server).toStdString());
        }

        // Prefer IPv4 first. A surprisingly common failure mode is a hostname
        // advertising IPv6 while the local network has no working IPv6 route.
        std::stable_sort(addresses.begin(), addresses.end(), [](const QHostAddress &a,
                                                                 const QHostAddress &b) {
            const bool aV4 = a.protocol() == QAbstractSocket::IPv4Protocol;
            const bool bV4 = b.protocol() == QAbstractSocket::IPv4Protocol;
            return aV4 && !bV4;
        });

        QStringList addressText;
        for (const QHostAddress &address : addresses) {
            if (!addressText.contains(address.toString())) {
                addressText.append(address.toString());
            }
        }
        emit eventReceived(QStringLiteral("status"), QString(),
                           QStringLiteral("[IRC] Resolved %1 → %2")
                               .arg(m_settings.server, addressText.join(QStringLiteral(", "))));

        std::unique_ptr<QAbstractSocket> socket;
        QStringList connectionErrors;

        for (const QHostAddress &address : addresses) {
            if (m_stopRequested) {
                break;
            }

            const QString mode = m_settings.tls ? QStringLiteral("TLS")
                                                : QStringLiteral("plain TCP");
            emit eventReceived(
                QStringLiteral("status"), QString(),
                QStringLiteral("[IRC] Trying %1:%2 using %3…")
                    .arg(address.toString())
                    .arg(m_settings.port)
                    .arg(mode));

            if (m_settings.tls) {
                auto ssl = std::make_unique<QSslSocket>();
                ssl->setPeerVerifyName(m_settings.server);
                ssl->connectToHost(address, m_settings.port);

                if (!ssl->waitForConnected(6000)) {
                    connectionErrors.append(
                        QStringLiteral("%1: TCP %2")
                            .arg(address.toString(), ssl->errorString()));
                    ssl->abort();
                    continue;
                }

                emit eventReceived(QStringLiteral("status"), QString(),
                                   QStringLiteral("[IRC] TCP connected to %1; starting TLS…")
                                       .arg(address.toString()));

                ssl->startClientEncryption();
                if (!ssl->waitForEncrypted(8000)) {
                    connectionErrors.append(
                        QStringLiteral("%1: TLS %2")
                            .arg(address.toString(), ssl->errorString()));
                    ssl->abort();
                    continue;
                }

                emit eventReceived(QStringLiteral("status"), QString(),
                                   QStringLiteral("[IRC] TLS established with %1.")
                                       .arg(address.toString()));
                socket = std::move(ssl);
                break;
            }

            auto tcp = std::make_unique<QTcpSocket>();
            tcp->connectToHost(address, m_settings.port);
            if (!tcp->waitForConnected(6000)) {
                connectionErrors.append(
                    QStringLiteral("%1: %2")
                        .arg(address.toString(), tcp->errorString()));
                tcp->abort();
                continue;
            }

            emit eventReceived(QStringLiteral("status"), QString(),
                               QStringLiteral("[IRC] TCP connected to %1.")
                                   .arg(address.toString()));
            socket = std::move(tcp);
            break;
        }

        if (!socket) {
            if (m_stopRequested) {
                throw std::runtime_error("IRC connection cancelled");
            }
            throw std::runtime_error(
                QStringLiteral("IRC could not connect to any resolved address: %1")
                    .arg(connectionErrors.join(QStringLiteral("; "))).toStdString());
        }

        emit eventReceived(QStringLiteral("status"), QString(),
                           QStringLiteral("[IRC] Registering nickname %1…").arg(m_nickname));

        if (!m_settings.password.isEmpty()) {
            sendLine(*socket, QStringLiteral("PASS %1").arg(m_settings.password));
        }
        sendLine(*socket, QStringLiteral("NICK %1").arg(m_nickname));
        sendLine(*socket,
                 QStringLiteral("USER %1 0 * :%2")
                     .arg(m_nickname,
                          m_settings.realName.isEmpty()
                              ? appDefaultRealName()
                              : m_settings.realName));

        bool registered = false;
        while (!registered && !m_stopRequested) {
            const QString line = readLine(*socket, 12000);
            if (line.isEmpty()) {
                continue;
            }

            if (m_settings.debug) {
                emit eventReceived(QStringLiteral("status"), QString(),
                                   QStringLiteral("[IRC <=] %1").arg(line));
            }
            const ParsedLine parsed = parseLine(line);

            if (parsed.command == QStringLiteral("PING")) {
                const QString token = parsed.params.isEmpty() ? QString() : parsed.params.back();
                sendLine(*socket, QStringLiteral("PONG :%1").arg(token));
                continue;
            }
            if (parsed.command == QStringLiteral("001")) {
                registered = true;
                break;
            }
            if (QSet<QString>{QStringLiteral("431"), QStringLiteral("432"),
                              QStringLiteral("433"), QStringLiteral("436"),
                              QStringLiteral("464"), QStringLiteral("465")}
                    .contains(parsed.command)) {
                throw std::runtime_error(
                    QStringLiteral("IRC login failed (%1): %2")
                        .arg(parsed.command,
                             parsed.params.isEmpty() ? QString() : parsed.params.back())
                        .toStdString());
            }
            if (parsed.command == QStringLiteral("ERROR")) {
                throw std::runtime_error(
                    (parsed.params.isEmpty() ? QStringLiteral("IRC server error")
                                             : parsed.params.back()).toStdString());
            }
            processLine(line);
        }

        if (!registered) {
            throw std::runtime_error("IRC registration cancelled");
        }

        emit eventReceived(
            QStringLiteral("status"), QString(),
            QStringLiteral("[online] IRC signed on as %1 via %2:%3%4")
                .arg(m_nickname, m_settings.server)
                .arg(m_settings.port)
                .arg(m_settings.tls ? QStringLiteral(" (TLS)") : QString()));
        emit connected(m_nickname,
                       QStringLiteral("%1:%2").arg(m_settings.server).arg(m_settings.port));
        {
            QStringList watched = m_watchBuddies.values();
            watched.sort(Qt::CaseInsensitive);
            emit buddyListChanged(watched);
            if (!watched.isEmpty())
                emit eventReceived(QStringLiteral("status"), QString(),
                                   QStringLiteral("[IRC] Local buddy/watch list loaded (%1 name(s)); using ISON presence checks.").arg(watched.size()));
        }
        qint64 nextWatchPoll = 0;

        while (!m_stopRequested) {
            for (const Command &command : takeCommands()) {
                switch (command.type) {
                case CommandType::SendIm:
                    sendLine(*socket,
                             QStringLiteral("PRIVMSG %1 :%2").arg(command.a, command.b));
                    emit eventReceived(QStringLiteral("im"), command.a,
                                       QStringLiteral("<%1> %2").arg(m_nickname, command.b));
                    break;

                case CommandType::Join: {
                    const QString room = canonicalChannel(command.a);
                    if (!room.isEmpty()) {
                        m_roomNames[room.toCaseFolded()] = room;
                        sendLine(*socket, QStringLiteral("JOIN %1").arg(room));
                    }
                    break;
                }

                case CommandType::SendRoom:
                    sendLine(*socket,
                             QStringLiteral("PRIVMSG %1 :%2").arg(command.a, command.b));
                    emit eventReceived(QStringLiteral("chat"), command.a,
                                       QStringLiteral("<%1> %2").arg(m_nickname, command.b));
                    break;

                case CommandType::Part:
                    sendLine(*socket, QStringLiteral("PART %1 :Leaving").arg(command.a));
                    m_members.remove(command.a.toCaseFolded());
                    m_roomNames.remove(command.a.toCaseFolded());
                    emit membersChanged(command.a, QStringLiteral("replace"), {});
                    break;

                case CommandType::Raw:
                    if (!command.a.trimmed().isEmpty()) {
                        sendLine(*socket, command.a);
                    }
                    break;

                case CommandType::Nick:
                    if (!command.a.trimmed().isEmpty() && !command.a.contains(QLatin1Char(' '))) {
                        sendLine(*socket, QStringLiteral("NICK %1").arg(command.a.trimmed()));
                    }
                    break;

                case CommandType::WatchAdd: {
                    const QString clean = command.a.trimmed();
                    if (!clean.isEmpty()) {
                        bool exists = false;
                        for (const QString &watched : m_watchBuddies)
                            if (watched.compare(clean, Qt::CaseInsensitive) == 0) { exists = true; break; }
                        if (!exists) m_watchBuddies.insert(clean);
                        nextWatchPoll = 0;
                    }
                    break;
                }

                case CommandType::WatchRemove: {
                    const QString folded = command.a.trimmed().toCaseFolded();
                    for (auto it = m_watchBuddies.begin(); it != m_watchBuddies.end();) {
                        if (it->toCaseFolded() == folded) it = m_watchBuddies.erase(it);
                        else ++it;
                    }
                    if (m_onlineWatchBuddies.remove(folded)) emit buddyPresenceChanged(command.a.trimmed(), false);
                    nextWatchPoll = 0;
                    break;
                }
                }
            }

            // Standard IRC ISON presence polling for local buddy/watch names.
            const qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
            if (!m_watchBuddies.isEmpty() && nowMs >= nextWatchPoll) {
                QStringList names = m_watchBuddies.values();
                names.sort(Qt::CaseInsensitive);
                QString batch;
                for (const QString &name : names) {
                    if (!batch.isEmpty() && batch.size() + name.size() + 1 > 350) {
                        sendLine(*socket, QStringLiteral("ISON %1").arg(batch));
                        batch.clear();
                    }
                    if (!batch.isEmpty()) batch += QLatin1Char(' ');
                    batch += name;
                }
                if (!batch.isEmpty()) sendLine(*socket, QStringLiteral("ISON %1").arg(batch));
                nextWatchPoll = nowMs + 20000;
            }

            if (socket->bytesAvailable() <= 0 && !socket->waitForReadyRead(75)) {
                if (socket->state() != QAbstractSocket::ConnectedState) {
                    throw std::runtime_error("IRC server closed the connection");
                }
                continue;
            }

            while (socket->canReadLine()) {
                QString line = QString::fromUtf8(socket->readLine());
                while (line.endsWith(QLatin1Char('\n')) || line.endsWith(QLatin1Char('\r'))) {
                    line.chop(1);
                }
                const ParsedLine parsed = parseLine(line);
                if (parsed.command == QStringLiteral("PING")) {
                    const QString token = parsed.params.isEmpty() ? QString() : parsed.params.back();
                    sendLine(*socket, QStringLiteral("PONG :%1").arg(token));
                } else {
                    processLine(line);
                }
            }
        }

        try {
            sendLine(*socket, QStringLiteral("QUIT :%1 signing off").arg(appDisplayName()));
        } catch (...) {
        }
        socket->disconnectFromHost();
        socket->waitForDisconnected(500);
    } catch (const std::exception &e) {
        disconnectReason = QString::fromUtf8(e.what());
        emit backendError(QStringLiteral("IRC connection"), disconnectReason);
    }

    emit eventReceived(QStringLiteral("status"), QString(), QStringLiteral("[offline] IRC signed off"));
    emit disconnected(disconnectReason);
}

