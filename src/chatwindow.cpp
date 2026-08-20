#include "chatwindow.h"

#include "appbranding.h"
#include "backend.h"

#include <QAction>
#include <QCloseEvent>
#include <QDateTime>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QFrame>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QSettings>
#include <QSlider>
#include <QSplitter>
#include <QTextOption>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <utility>

ChatWindow::ChatWindow(ChatBackend *backend,
                       QString kind,
                       QString target,
                       QString displayName,
                       QString opacitySettingsKey,
                       QWidget *parent)
    : QMainWindow(parent),
      m_backend(backend),
      m_kind(std::move(kind)),
      m_target(std::move(target)),
      m_displayName(std::move(displayName)),
      m_opacitySettingsKey(std::move(opacitySettingsKey))
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    resize(m_kind == QStringLiteral("chat") ? 700 : 590, 440);
    setMinimumSize(m_kind == QStringLiteral("chat") ? QSize(520, 330)
                                                     : QSize(430, 300));
    buildMenus();
    buildUi();
    loadOpacity();
    refreshTitle();
}

QString ChatWindow::backendId() const
{
    return m_backend ? m_backend->id() : QString();
}

void ChatWindow::buildMenus()
{
    QMenu *viewMenu = menuBar()->addMenu(QStringLiteral("&View"));
    QAction *resetOpacity =
        viewMenu->addAction(QStringLiteral("Reset Window &Opacity"));
    connect(resetOpacity, &QAction::triggered,
            this, &ChatWindow::setTransparency);

    if (m_kind == QStringLiteral("im")) {
        QMenu *securityMenu = menuBar()->addMenu(QStringLiteral("&Security"));
        QAction *start = securityMenu->addAction(QStringLiteral("Start &Secure Session"));
        QAction *status = securityMenu->addAction(QStringLiteral("Secure Session &Status"));
        securityMenu->addSeparator();
        QAction *trust = securityMenu->addAction(QStringLiteral("&Trust Peer Fingerprint"));
        QAction *untrust = securityMenu->addAction(QStringLiteral("&Forget Trusted Fingerprint"));
        securityMenu->addSeparator();
        QAction *off = securityMenu->addAction(QStringLiteral("&Close Secure Session"));
        securityMenu->addSeparator();
        QAction *sendFile = securityMenu->addAction(QStringLiteral("Send &File…"));

        connect(start, &QAction::triggered, this, [this] { emit secureRequested(this); });
        connect(status, &QAction::triggered, this, [this] { emit secureStatusRequested(this); });
        connect(trust, &QAction::triggered, this, [this] { emit trustRequested(this); });
        connect(untrust, &QAction::triggered, this, [this] { emit untrustRequested(this); });
        connect(off, &QAction::triggered, this, [this] { emit secureOffRequested(this); });
        connect(sendFile, &QAction::triggered, this, [this] { emit fileSendRequested(this); });
    }
}

void ChatWindow::buildUi()
{
    auto *central = new QWidget(this);
    auto *outer = new QVBoxLayout(central);
    outer->setContentsMargins(6, 6, 6, 6);
    outer->setSpacing(4);

    auto *topLine = new QHBoxLayout;
    m_heading = new QLabel(central);
    QFont headingFont = m_heading->font();
    headingFont.setBold(true);
    m_heading->setFont(headingFont);
    topLine->addWidget(m_heading, 1);

    m_securityLabel = new QLabel(central);
    m_securityLabel->setTextFormat(Qt::PlainText);
    m_securityLabel->setVisible(false);
    topLine->addWidget(m_securityLabel);

    m_connectionLabel = new QLabel(central);
    m_connectionLabel->setTextFormat(Qt::PlainText);
    topLine->addWidget(m_connectionLabel);
    outer->addLayout(topLine);

    m_transcript = new QPlainTextEdit(central);
    m_transcript->setReadOnly(true);
    m_transcript->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_transcript->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    if (m_kind == QStringLiteral("chat")) {
        auto *splitter = new QSplitter(Qt::Horizontal, central);
        splitter->addWidget(m_transcript);

        auto *memberFrame = new QFrame(splitter);
        memberFrame->setFrameShape(QFrame::StyledPanel);
        memberFrame->setFrameShadow(QFrame::Sunken);
        m_memberPane = memberFrame;
        auto *memberLayout = new QVBoxLayout(memberFrame);
        memberLayout->setContentsMargins(7, 7, 7, 7);
        memberLayout->setSpacing(4);

        m_membersTitle = new QLabel(QStringLiteral("Members (0)"), memberFrame);
        QFont memberTitleFont = m_membersTitle->font();
        memberTitleFont.setBold(true);
        m_membersTitle->setFont(memberTitleFont);

        m_members = new QListWidget(memberFrame);
        m_members->setMinimumWidth(125);
        m_members->setMaximumWidth(220);
        memberLayout->addWidget(m_membersTitle);
        memberLayout->addWidget(m_members, 1);

        splitter->addWidget(memberFrame);
        splitter->setStretchFactor(0, 1);
        splitter->setStretchFactor(1, 0);
        splitter->setSizes({545, 145});
        outer->addWidget(splitter, 1);
    } else {
        outer->addWidget(m_transcript, 1);
    }

    if (m_kind == QStringLiteral("im")) {
        auto *securityRow = new QHBoxLayout;
        securityRow->setSpacing(4);
        m_secureButton = new QPushButton(QStringLiteral("Secure"), central);
        m_secureStatusButton = new QPushButton(QStringLiteral("Status"), central);
        m_secureCloseButton = new QPushButton(QStringLiteral("Close Secure"), central);
        m_sendFileButton = new QPushButton(QStringLiteral("Send File…"), central);
        securityRow->addWidget(m_secureButton);
        securityRow->addWidget(m_secureStatusButton);
        securityRow->addWidget(m_secureCloseButton);
        securityRow->addWidget(m_sendFileButton);
        securityRow->addStretch(1);
        outer->addLayout(securityRow);

        connect(m_secureButton, &QPushButton::clicked,
                this, [this] { emit secureRequested(this); });
        connect(m_secureStatusButton, &QPushButton::clicked,
                this, [this] { emit secureStatusRequested(this); });
        connect(m_secureCloseButton, &QPushButton::clicked,
                this, [this] { emit secureOffRequested(this); });
        connect(m_sendFileButton, &QPushButton::clicked,
                this, [this] { emit fileSendRequested(this); });
    }

    auto *opacityRow = new QHBoxLayout;
    opacityRow->setSpacing(6);
    auto *opacityLabel = new QLabel(QStringLiteral("Opacity"), central);
    m_opacitySlider = new QSlider(Qt::Horizontal, central);
    m_opacitySlider->setRange(30, 100);
    m_opacitySlider->setSingleStep(1);
    m_opacitySlider->setPageStep(5);
    m_opacityValueLabel = new QLabel(QStringLiteral("100%"), central);
    m_opacityValueLabel->setMinimumWidth(44);
    opacityRow->addWidget(opacityLabel);
    opacityRow->addWidget(m_opacitySlider, 1);
    opacityRow->addWidget(m_opacityValueLabel);
    outer->addLayout(opacityRow);
    connect(m_opacitySlider, &QSlider::valueChanged,
            this, &ChatWindow::opacitySliderChanged);

    auto *inputRow = new QHBoxLayout;
    inputRow->setSpacing(4);
    m_messageEdit = new QLineEdit(central);
    m_messageEdit->setPlaceholderText(
        m_kind == QStringLiteral("terminal")
            ? QStringLiteral("Type a Telnet command/line…")
            : QStringLiteral("Type a message…"));
    m_sendButton = new QPushButton(QStringLiteral("Send"), central);
    inputRow->addWidget(m_messageEdit, 1);
    inputRow->addWidget(m_sendButton);
    outer->addLayout(inputRow);

    m_messageEdit->installEventFilter(this);
    connect(m_messageEdit, &QLineEdit::textEdited, this, [this] {
        resetCommandCompletion();
        resetMemberCompletion();
    });
    connect(m_messageEdit, &QLineEdit::returnPressed,
            this, &ChatWindow::sendMessage);
    connect(m_sendButton, &QPushButton::clicked,
            this, &ChatWindow::sendMessage);

    setCentralWidget(central);
}

QStringList ChatWindow::availableSlashCommands() const
{
    if (m_kind == QStringLiteral("terminal")) return {};

    QStringList commands = {
        QStringLiteral("/fingerprint"),
        QStringLiteral("/help"),
        QStringLiteral("/options")
    };
    if (m_kind == QStringLiteral("im")) {
        commands << QStringLiteral("/secure")
                 << QStringLiteral("/secureoff")
                 << QStringLiteral("/securestatus")
                 << QStringLiteral("/trust")
                 << QStringLiteral("/untrust");
    }
    commands.sort(Qt::CaseInsensitive);
    return commands;
}

void ChatWindow::resetCommandCompletion()
{
    m_commandCompletionMatches.clear();
    m_commandCompletionIndex = -1;
}

void ChatWindow::resetMemberCompletion()
{
    m_memberCompletionMatches.clear();
    m_memberCompletionIndex = -1;
    m_memberCompletionStart = -1;
}

bool ChatWindow::completeSlashCommand(int direction)
{
    if (!m_messageEdit) return false;
    const int cursor = m_messageEdit->cursorPosition();
    const QString text = m_messageEdit->text();
    if (cursor <= 0 || !text.startsWith(QLatin1Char('/'))) {
        resetCommandCompletion();
        return false;
    }

    const QString prefix = text.left(cursor);
    if (prefix.contains(QLatin1Char(' ')) || prefix.contains(QLatin1Char('\t'))) {
        resetCommandCompletion();
        return false;
    }

    if (!m_commandCompletionMatches.isEmpty()
        && m_commandCompletionIndex >= 0
        && m_commandCompletionIndex < m_commandCompletionMatches.size()
        && prefix.compare(m_commandCompletionMatches[m_commandCompletionIndex],
                          Qt::CaseInsensitive) == 0) {
        const int count = m_commandCompletionMatches.size();
        m_commandCompletionIndex = (m_commandCompletionIndex + direction + count) % count;
    } else {
        m_commandCompletionMatches.clear();
        for (const QString &command : availableSlashCommands()) {
            if (command.startsWith(prefix, Qt::CaseInsensitive)) {
                m_commandCompletionMatches.append(command);
            }
        }
        if (m_commandCompletionMatches.isEmpty()) {
            m_commandCompletionIndex = -1;
            return true;
        }
        m_commandCompletionIndex = direction < 0
            ? m_commandCompletionMatches.size() - 1 : 0;
    }

    const QString replacement = m_commandCompletionMatches[m_commandCompletionIndex];
    QString updated = text;
    updated.replace(0, cursor, replacement);
    m_messageEdit->setText(updated);
    m_messageEdit->setCursorPosition(replacement.size());
    return true;
}

bool ChatWindow::completeMemberName(int direction)
{
    if (!m_messageEdit || !m_backend || m_kind != QStringLiteral("chat")
        || m_backend->protocolName().compare(QStringLiteral("IRC"), Qt::CaseInsensitive) != 0) {
        resetMemberCompletion();
        return false;
    }

    const QString text = m_messageEdit->text();
    const int cursor = m_messageEdit->cursorPosition();
    if (cursor < 0 || text.startsWith(QLatin1Char('/'))) {
        resetMemberCompletion();
        return false;
    }

    int tokenStart = cursor;
    while (tokenStart > 0 && !text.at(tokenStart - 1).isSpace()) --tokenStart;
    const QString token = text.mid(tokenStart, cursor - tokenStart);

    auto decorated = [tokenStart](const QString &nick) {
        return tokenStart == 0 ? nick + QStringLiteral(": ") : nick + QLatin1Char(' ');
    };

    const bool cycling = !m_memberCompletionMatches.isEmpty()
        && m_memberCompletionIndex >= 0
        && m_memberCompletionIndex < m_memberCompletionMatches.size()
        && m_memberCompletionStart == tokenStart
        && token.compare(decorated(m_memberCompletionMatches[m_memberCompletionIndex]),
                         Qt::CaseInsensitive) == 0;

    if (cycling) {
        const int count = m_memberCompletionMatches.size();
        m_memberCompletionIndex = (m_memberCompletionIndex + direction + count) % count;
    } else {
        QString seed = token.trimmed();
        if (seed.endsWith(QLatin1Char(':'))) seed.chop(1);
        if (seed.isEmpty()) {
            resetMemberCompletion();
            return false;
        }
        m_memberCompletionMatches.clear();
        for (const QString &nick : m_memberSet) {
            if (nick.startsWith(seed, Qt::CaseInsensitive)) {
                m_memberCompletionMatches.append(nick);
            }
        }
        m_memberCompletionMatches.sort(Qt::CaseInsensitive);
        if (m_memberCompletionMatches.isEmpty()) {
            resetMemberCompletion();
            return true;
        }
        m_memberCompletionIndex = direction < 0
            ? m_memberCompletionMatches.size() - 1 : 0;
        m_memberCompletionStart = tokenStart;
    }

    const QString replacement = decorated(m_memberCompletionMatches[m_memberCompletionIndex]);
    QString updated = text;
    updated.replace(tokenStart, cursor - tokenStart, replacement);
    m_messageEdit->setText(updated);
    m_messageEdit->setCursorPosition(tokenStart + replacement.size());
    return true;
}

bool ChatWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_messageEdit && event && event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab) {
            if (completeSlashCommand(+1)) return true;
            if (completeMemberName(+1)) return true;
        }
        if (keyEvent->key() == Qt::Key_Backtab) {
            if (completeSlashCommand(-1)) return true;
            if (completeMemberName(-1)) return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void ChatWindow::refreshTitle()
{
    const QString protocol = m_backend
        ? m_backend->protocolName()
        : QStringLiteral("Disconnected");
    m_heading->setText(m_displayName);
    m_connectionLabel->setText(
        QStringLiteral("%1 · %2")
            .arg(protocol,
                 m_online ? QStringLiteral("online") : QStringLiteral("offline")));
    setWindowTitle(QStringLiteral("%1 — %2 — %3 %4")
                       .arg(m_displayName, protocol, appDisplayName(), appVersionString()));
}

void ChatWindow::appendMessage(const QString &text)
{
    if (!m_transcript) {
        return;
    }

    if (!m_showTimestamps) {
        m_transcript->appendPlainText(text);
        return;
    }

    const QString timestamp =
        QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss"));
    const QStringList lines = text.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        m_transcript->appendPlainText(
            QStringLiteral("[%1] %2").arg(timestamp, line));
    }
}

void ChatWindow::updateMembers(const QString &action, const QStringList &names)
{
    if (!m_members) {
        return;
    }

    if (action == QStringLiteral("replace")) {
        m_memberSet.clear();
        for (const QString &name : names) {
            if (!name.trimmed().isEmpty()) {
                m_memberSet.insert(name.trimmed());
            }
        }
    } else if (action == QStringLiteral("add")) {
        for (const QString &name : names) {
            if (!name.trimmed().isEmpty()) {
                m_memberSet.insert(name.trimmed());
            }
        }
    } else if (action == QStringLiteral("remove")) {
        for (const QString &name : names) {
            for (auto it = m_memberSet.begin(); it != m_memberSet.end();) {
                if (it->compare(name, Qt::CaseInsensitive) == 0) {
                    it = m_memberSet.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    refreshMembers();
}

void ChatWindow::refreshMembers()
{
    if (!m_members || !m_membersTitle) {
        return;
    }

    QStringList names = m_memberSet.values();
    std::sort(names.begin(), names.end(), [](const QString &a, const QString &b) {
        return a.compare(b, Qt::CaseInsensitive) < 0;
    });

    m_members->clear();
    m_members->addItems(names);
    m_membersTitle->setText(QStringLiteral("Members (%1)").arg(names.size()));
}

void ChatWindow::setDisplayName(const QString &displayName)
{
    if (displayName.trimmed().isEmpty()) {
        return;
    }
    m_displayName = displayName.trimmed();
    refreshTitle();
}

void ChatWindow::setBackendOnline(bool online)
{
    m_online = online;
    if (m_messageEdit) {
        m_messageEdit->setEnabled(online);
    }
    if (m_sendButton) {
        m_sendButton->setEnabled(online);
    }
    if (m_secureButton) m_secureButton->setEnabled(online && !m_secureActive);
    if (m_secureStatusButton) m_secureStatusButton->setEnabled(online);
    if (m_secureCloseButton) m_secureCloseButton->setEnabled(online && m_secureActive);
    if (m_sendFileButton) m_sendFileButton->setEnabled(online && m_secureActive);
    refreshTitle();
}

void ChatWindow::setShowSidePane(bool enabled)
{
    m_showSidePane = enabled;
    if (m_memberPane) {
        m_memberPane->setVisible(enabled);
    }
}

void ChatWindow::setSecurityState(bool active,
                                  bool trusted,
                                  const QString &peerFingerprint,
                                  const QString &localFingerprint)
{
    if (!m_securityLabel) {
        return;
    }

    const bool isIm = m_kind == QStringLiteral("im");
    m_secureActive = isIm && active;
    if (m_secureButton) m_secureButton->setEnabled(isIm && !m_secureActive && m_online);
    if (m_secureStatusButton) m_secureStatusButton->setEnabled(isIm && m_online);
    if (m_secureCloseButton) m_secureCloseButton->setEnabled(isIm && m_secureActive && m_online);
    if (m_sendFileButton) m_sendFileButton->setEnabled(isIm && m_secureActive && m_online);

    if (!active || !isIm) {
        m_securityLabel->clear();
        m_securityLabel->setToolTip(QString());
        m_securityLabel->setVisible(false);
        return;
    }

    m_securityLabel->setText(trusted
        ? QStringLiteral("🔒 secure · trusted")
        : QStringLiteral("🔒 secure · unverified"));
    m_securityLabel->setToolTip(
        QStringLiteral("Peer fingerprint: %1\nLocal fingerprint: %2")
            .arg(peerFingerprint.isEmpty() ? QStringLiteral("unknown") : peerFingerprint,
                 localFingerprint.isEmpty() ? QStringLiteral("unknown") : localFingerprint));
    m_securityLabel->setVisible(true);
}

void ChatWindow::sendMessage()
{
    if (!m_backend || !m_online) {
        return;
    }

    const QString message = m_messageEdit->text();
    if (message.trimmed().isEmpty()) {
        return;
    }

    m_messageEdit->clear();
    emit messageSubmitted(this, message);
}

void ChatWindow::loadOpacity()
{
    if (m_opacitySettingsKey.isEmpty()) {
        return;
    }
    QSettings settings;
    m_opacity = std::clamp(
        settings.value(m_opacitySettingsKey, 1.0).toDouble(),
        0.30,
        1.0);
    setWindowOpacity(m_opacity);
    const int percent = static_cast<int>(m_opacity * 100.0 + 0.5);
    if (m_opacitySlider) m_opacitySlider->setValue(percent);
    if (m_opacityValueLabel) m_opacityValueLabel->setText(QStringLiteral("%1%").arg(percent));
}

void ChatWindow::saveOpacity() const
{
    if (m_opacitySettingsKey.isEmpty()) {
        return;
    }
    QSettings settings;
    settings.setValue(m_opacitySettingsKey, m_opacity);
}

void ChatWindow::setTransparency()
{
    m_opacity = 1.0;
    setWindowOpacity(m_opacity);
    if (m_opacitySlider) m_opacitySlider->setValue(100);
    if (m_opacityValueLabel) m_opacityValueLabel->setText(QStringLiteral("100%"));
    saveOpacity();
}

void ChatWindow::opacitySliderChanged(int percent)
{
    percent = std::clamp(percent, 30, 100);
    m_opacity = static_cast<double>(percent) / 100.0;
    setWindowOpacity(m_opacity);
    if (m_opacityValueLabel) {
        m_opacityValueLabel->setText(QStringLiteral("%1%").arg(percent));
    }
    saveOpacity();
}

void ChatWindow::updateTerminalSize()
{
    if (!m_backend || m_kind != QStringLiteral("terminal") || !m_transcript) {
        return;
    }
    const QFontMetrics metrics(m_transcript->font());
    const int charWidth = std::max(1, metrics.horizontalAdvance(QLatin1Char('M')));
    const int lineHeight = std::max(1, metrics.lineSpacing());
    const QSize area = m_transcript->viewport()->size();
    m_backend->setTerminalSize(std::max(20, area.width() / charWidth),
                               std::max(5, area.height() / lineHeight));
}

void ChatWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateTerminalSize();
}

void ChatWindow::closeEvent(QCloseEvent *event)
{
    saveOpacity();
    if (!m_closeSignalSent) {
        m_closeSignalSent = true;
        emit conversationClosing(this);
    }
    QMainWindow::closeEvent(event);
}
