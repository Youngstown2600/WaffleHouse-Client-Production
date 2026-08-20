#include "telnetbackend.h"

#include <QAbstractSocket>
#include <QHostAddress>
#include <QHostInfo>
#include <QMutexLocker>
#include <QRegularExpression>
#include <QTcpSocket>

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>

namespace {
constexpr quint8 IAC  = 255;
constexpr quint8 DONT = 254;
constexpr quint8 DO   = 253;
constexpr quint8 WONT = 252;
constexpr quint8 WILL = 251;
constexpr quint8 SB   = 250;
constexpr quint8 SE   = 240;

constexpr quint8 OPT_BINARY = 0;
constexpr quint8 OPT_ECHO = 1;
constexpr quint8 OPT_SGA = 3;
constexpr quint8 OPT_TTYPE = 24;
constexpr quint8 OPT_NAWS = 31;
constexpr quint8 OPT_LINEMODE = 34;

constexpr quint8 TTYPE_IS = 0;
constexpr quint8 TTYPE_SEND = 1;

QByteArray escapeIac(QByteArray data)
{
    data.replace(QByteArray(1, static_cast<char>(IAC)),
                 QByteArray(2, static_cast<char>(IAC)));
    return data;
}
} // namespace

TelnetBackend::TelnetBackend(ConnectionSettings settings, QObject *parent)
    : ChatBackend(std::move(settings), parent)
{
}

TelnetBackend::~TelnetBackend()
{
    m_stopRequested = true;
    if (m_thread && m_thread != QThread::currentThread()) {
        m_thread->wait();
    }
}

void TelnetBackend::setConnectionSettings(const ConnectionSettings &settings)
{
    ChatBackend::setConnectionSettings(settings);
}

void TelnetBackend::start()
{
    if (m_thread) {
        return;
    }
    m_stopRequested = false;
    m_telnetState = TelnetState::Data;
    m_subnegData.clear();
    m_serverEcho = false;
    m_thread = QThread::create([this] { run(); });
    connect(m_thread, &QThread::finished, m_thread, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, this,
            [this] { m_thread = nullptr; }, Qt::QueuedConnection);
    m_thread->start();
}

void TelnetBackend::stop()
{
    m_stopRequested = true;
    QThread *thread = m_thread;
    if (thread && thread != QThread::currentThread()) {
        thread->wait(12000);
    }
}

void TelnetBackend::enqueue(Command command)
{
    QMutexLocker locker(&m_commandMutex);
    m_commands.enqueue(std::move(command));
}

QList<TelnetBackend::Command> TelnetBackend::takeCommands()
{
    QList<Command> result;
    QMutexLocker locker(&m_commandMutex);
    while (!m_commands.isEmpty()) {
        result.push_back(m_commands.dequeue());
    }
    return result;
}

void TelnetBackend::sendPrivateMessage(const QString &, const QString &message)
{
    enqueue({CommandType::SendLine, message});
}

void TelnetBackend::joinRoom(const QString &, bool)
{
    emit backendError(QStringLiteral("Telnet"),
                      QStringLiteral("Telnet sessions do not use rooms/channels."));
}

void TelnetBackend::sendRoomMessage(const QString &, const QString &message)
{
    enqueue({CommandType::SendLine, message});
}

void TelnetBackend::leaveRoom(const QString &)
{
    stop();
}

void TelnetBackend::sendRaw(const QString &line, const QString &, const QString &)
{
    enqueue({CommandType::RawLine, line});
}

void TelnetBackend::setTerminalSize(int columns, int rows)
{
    m_columns = std::clamp(columns, 20, 65535);
    m_rows = std::clamp(rows, 5, 65535);
}

void TelnetBackend::writeAll(QTcpSocket &socket, const QByteArray &data)
{
    qint64 offset = 0;
    while (offset < data.size()) {
        const qint64 written = socket.write(data.constData() + offset, data.size() - offset);
        if (written < 0) {
            throw std::runtime_error(socket.errorString().toStdString());
        }
        offset += written;
        if (!socket.waitForBytesWritten(3000) && socket.bytesToWrite() > 0) {
            throw std::runtime_error(socket.errorString().toStdString());
        }
    }
}

void TelnetBackend::sendLine(QTcpSocket &socket, const QString &text)
{
    QByteArray bytes = escapeIac(text.toUtf8());
    bytes.append("\r\n");
    writeAll(socket, bytes);

    // If the remote side is not doing Telnet ECHO, provide local echo so line-mode
    // MUD/BBS sessions remain readable.
    if (!m_serverEcho.load()) {
        emit eventReceived(QStringLiteral("terminal"), m_settings.server,
                           QStringLiteral("> %1\n").arg(text));
    }
}

void TelnetBackend::sendIac(QTcpSocket &socket, quint8 command, quint8 option)
{
    QByteArray out;
    out.append(static_cast<char>(IAC));
    out.append(static_cast<char>(command));
    out.append(static_cast<char>(option));
    writeAll(socket, out);
}

void TelnetBackend::sendSubneg(QTcpSocket &socket, quint8 option, const QByteArray &payload)
{
    QByteArray out;
    out.append(static_cast<char>(IAC));
    out.append(static_cast<char>(SB));
    out.append(static_cast<char>(option));
    out.append(escapeIac(payload));
    out.append(static_cast<char>(IAC));
    out.append(static_cast<char>(SE));
    writeAll(socket, out);
}

void TelnetBackend::sendWindowSize(QTcpSocket &socket)
{
    const quint16 cols = static_cast<quint16>(m_columns.load());
    const quint16 rows = static_cast<quint16>(m_rows.load());
    QByteArray payload;
    payload.append(static_cast<char>((cols >> 8) & 0xff));
    payload.append(static_cast<char>(cols & 0xff));
    payload.append(static_cast<char>((rows >> 8) & 0xff));
    payload.append(static_cast<char>(rows & 0xff));
    sendSubneg(socket, OPT_NAWS, payload);
}

void TelnetBackend::handleNegotiation(QTcpSocket &socket, quint8 command, quint8 option)
{
    if (m_settings.debug) {
        emit eventReceived(QStringLiteral("status"), QString(),
                           QStringLiteral("[TELNET] IAC %1 option %2")
                               .arg(command).arg(option));
    }

    if (command == WILL) {
        switch (option) {
        case OPT_BINARY:
        case OPT_SGA:
            sendIac(socket, DO, option);
            break;
        case OPT_ECHO:
            m_serverEcho = true;
            sendIac(socket, DO, option);
            break;
        default:
            sendIac(socket, DONT, option);
            break;
        }
        return;
    }

    if (command == WONT) {
        if (option == OPT_ECHO) {
            m_serverEcho = false;
        }
        sendIac(socket, DONT, option);
        return;
    }

    if (command == DO) {
        switch (option) {
        case OPT_BINARY:
        case OPT_SGA:
            sendIac(socket, WILL, option);
            break;
        case OPT_TTYPE:
            sendIac(socket, WILL, option);
            break;
        case OPT_NAWS:
            sendIac(socket, WILL, option);
            sendWindowSize(socket);
            break;
        case OPT_ECHO:
        case OPT_LINEMODE:
        default:
            sendIac(socket, WONT, option);
            break;
        }
        return;
    }

    if (command == DONT) {
        sendIac(socket, WONT, option);
    }
}

void TelnetBackend::handleSubnegotiation(QTcpSocket &socket,
                                         quint8 option,
                                         const QByteArray &payload)
{
    if (option == OPT_TTYPE && !payload.isEmpty()
        && static_cast<quint8>(payload.front()) == TTYPE_SEND) {
        QByteArray response;
        response.append(static_cast<char>(TTYPE_IS));
        QString terminal = m_settings.telnetTerminalType.trimmed();
        if (terminal.isEmpty()) {
            terminal = QStringLiteral("xterm-256color");
        }
        response.append(terminal.toLatin1());
        sendSubneg(socket, OPT_TTYPE, response);
    }
}

QString TelnetBackend::sanitizeTerminalText(const QByteArray &bytes) const
{
    QString text = QString::fromUtf8(bytes.constData(), bytes.size());

    // The client is currently a readable text terminal rather than a complete
    // VT100 emulator. Strip ANSI/VT control sequences so BBS/MUD output remains
    // readable in the GUI transcript instead of exposing raw escape sequences.
    static const QRegularExpression csi(
        QStringLiteral("\\x1B\\[[0-?]*[ -/]*[@-~]"));
    static const QRegularExpression osc(
        QStringLiteral("\\x1B\\][^\\x07]*(?:\\x07|\\x1B\\\\)"));
    text.remove(osc);
    text.remove(csi);

    // Normalize Telnet CR/LF conventions before handing text to the scrollback
    // buffer. A bare CR from an old BBS is treated as a newline rather than a
    // cursor-return control character that could produce confusing GUI transcript output.
    text.replace(QStringLiteral("\r\n"), QStringLiteral("\n"));
    text.replace(QLatin1Char('\r'), QLatin1Char('\n'));

    QString clean;
    clean.reserve(text.size());
    for (const QChar ch : text) {
        const ushort u = ch.unicode();
        if (ch == QLatin1Char('\b')) {
            if (!clean.isEmpty() && clean.back() != QLatin1Char('\n')) {
                clean.chop(1);
            }
        } else if (ch == QLatin1Char('\n') || ch == QLatin1Char('\t')) {
            clean += ch;
        } else if (u >= 0x20 && u != 0x7f) {
            clean += ch;
        }
    }
    return clean;
}

void TelnetBackend::processBytes(QTcpSocket &socket, const QByteArray &bytes)
{
    QByteArray display;

    for (const char raw : bytes) {
        const quint8 b = static_cast<quint8>(raw);

        switch (m_telnetState) {
        case TelnetState::Data:
            if (b == IAC) {
                m_telnetState = TelnetState::Iac;
            } else {
                display.append(raw);
            }
            break;

        case TelnetState::Iac:
            if (b == IAC) {
                display.append(static_cast<char>(IAC));
                m_telnetState = TelnetState::Data;
            } else if (b == DO || b == DONT || b == WILL || b == WONT) {
                m_pendingCommand = b;
                m_telnetState = TelnetState::Negotiation;
            } else if (b == SB) {
                m_telnetState = TelnetState::SubnegOption;
            } else {
                m_telnetState = TelnetState::Data;
            }
            break;

        case TelnetState::Negotiation:
            handleNegotiation(socket, m_pendingCommand, b);
            m_telnetState = TelnetState::Data;
            break;

        case TelnetState::SubnegOption:
            m_subnegOption = b;
            m_subnegData.clear();
            m_telnetState = TelnetState::SubnegData;
            break;

        case TelnetState::SubnegData:
            if (b == IAC) {
                m_telnetState = TelnetState::SubnegIac;
            } else {
                m_subnegData.append(raw);
            }
            break;

        case TelnetState::SubnegIac:
            if (b == SE) {
                handleSubnegotiation(socket, m_subnegOption, m_subnegData);
                m_subnegData.clear();
                m_telnetState = TelnetState::Data;
            } else if (b == IAC) {
                m_subnegData.append(static_cast<char>(IAC));
                m_telnetState = TelnetState::SubnegData;
            } else {
                m_telnetState = TelnetState::SubnegData;
            }
            break;
        }
    }

    if (!display.isEmpty()) {
        const QString text = sanitizeTerminalText(display);
        if (!text.isEmpty()) {
            emit eventReceived(QStringLiteral("terminal"), m_settings.server, text);
        }
    }
}

void TelnetBackend::run()
{
    QString disconnectReason = QStringLiteral("signed off");

    try {
        emit eventReceived(QStringLiteral("status"), QString(),
                           QStringLiteral("[TELNET] Resolving %1…")
                               .arg(m_settings.server));

        QList<QHostAddress> addresses;
        QHostAddress literal;
        if (literal.setAddress(m_settings.server)) {
            addresses.append(literal);
        } else {
            const QHostInfo info = QHostInfo::fromName(m_settings.server);
            if (info.error() != QHostInfo::NoError) {
                throw std::runtime_error(
                    QStringLiteral("Telnet DNS lookup failed for %1: %2")
                        .arg(m_settings.server, info.errorString()).toStdString());
            }
            addresses = info.addresses();
        }

        if (addresses.isEmpty()) {
            throw std::runtime_error(
                QStringLiteral("Telnet DNS lookup returned no addresses for %1")
                    .arg(m_settings.server).toStdString());
        }

        std::stable_sort(addresses.begin(), addresses.end(),
                         [](const QHostAddress &a, const QHostAddress &b) {
            const bool aV4 = a.protocol() == QAbstractSocket::IPv4Protocol;
            const bool bV4 = b.protocol() == QAbstractSocket::IPv4Protocol;
            return aV4 && !bV4;
        });

        std::unique_ptr<QTcpSocket> socket;
        QStringList errors;
        for (const QHostAddress &address : addresses) {
            if (m_stopRequested) {
                break;
            }

            emit eventReceived(QStringLiteral("status"), QString(),
                               QStringLiteral("[TELNET] Trying %1:%2…")
                                   .arg(address.toString()).arg(m_settings.port));

            auto candidate = std::make_unique<QTcpSocket>();
            candidate->connectToHost(address, m_settings.port);
            if (!candidate->waitForConnected(6000)) {
                errors.append(QStringLiteral("%1: %2")
                                  .arg(address.toString(), candidate->errorString()));
                candidate->abort();
                continue;
            }
            socket = std::move(candidate);
            break;
        }

        if (!socket) {
            if (m_stopRequested) {
                throw std::runtime_error("Telnet connection cancelled");
            }
            throw std::runtime_error(
                QStringLiteral("Telnet could not connect to any resolved address: %1")
                    .arg(errors.join(QStringLiteral("; "))).toStdString());
        }

        const QString identity = m_settings.username.trimmed().isEmpty()
            ? m_settings.server
            : m_settings.username.trimmed();
        const QString endpoint = QStringLiteral("%1:%2")
            .arg(m_settings.server).arg(m_settings.port);

        emit eventReceived(QStringLiteral("status"), QString(),
                           QStringLiteral("[online] Telnet connected to %1")
                               .arg(endpoint));
        emit connected(identity, endpoint);

        while (!m_stopRequested) {
            for (const Command &command : takeCommands()) {
                switch (command.type) {
                case CommandType::SendLine:
                    sendLine(*socket, command.text);
                    break;
                case CommandType::RawLine:
                    sendLine(*socket, command.text);
                    break;
                }
            }

            if (socket->bytesAvailable() <= 0 && !socket->waitForReadyRead(50)) {
                if (socket->state() != QAbstractSocket::ConnectedState) {
                    throw std::runtime_error("Telnet server closed the connection");
                }
                continue;
            }

            const QByteArray incoming = socket->readAll();
            if (!incoming.isEmpty()) {
                processBytes(*socket, incoming);
            }
        }

        socket->disconnectFromHost();
        socket->waitForDisconnected(500);
    } catch (const std::exception &e) {
        disconnectReason = QString::fromUtf8(e.what());
        if (!m_stopRequested || disconnectReason != QStringLiteral("Telnet connection cancelled")) {
            emit backendError(QStringLiteral("Telnet connection"), disconnectReason);
        }
    }

    emit eventReceived(QStringLiteral("status"), QString(),
                       QStringLiteral("[offline] Telnet signed off"));
    emit disconnected(disconnectReason);
}
