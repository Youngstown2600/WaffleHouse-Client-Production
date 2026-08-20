#include "mainwindow.h"
#include "platforminfo.h"

#include "chatwindow.h"
#include "appbranding.h"
#include "ircbackend.h"
#include "oscarbackend.h"
#include "telnetbackend.h"
#include "bbsdirectory.h"
#include "transferwindow.h"
#include "sipcontroller.h"
#include "sipbackend.h"
#include "softphonewindow.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDir>
#include <QCryptographicHash>
#include <QIcon>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QTabWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QSettings>
#include <QSignalBlocker>
#include <QStatusBar>
#include <QStyle>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QUuid>
#include <QVariant>

#include <algorithm>

namespace {

class ConnectionDialog final : public QDialog {
public:
    explicit ConnectionDialog(const ConnectionSettings &defaults,
                              bool editing = false,
                              QWidget *parent = nullptr)
        : QDialog(parent),
          m_editing(editing),
          m_original(defaults)
    {
        setWindowTitle(m_editing
                           ? QStringLiteral("Edit Connection — %1").arg(appDisplayName())
                           : QStringLiteral("Add Connection — %1").arg(appDisplayName()));
        setModal(true);
        setMinimumWidth(440);

        auto *outer = new QVBoxLayout(this);
        auto *form = new QFormLayout;
        outer->addLayout(form);

        m_protocol = new QComboBox(this);
        m_protocol->addItem(QStringLiteral("Select protocol…"),
                            static_cast<int>(ConnectionSettings::Protocol::Unknown));
        m_protocol->addItem(QStringLiteral("AIM / OSCAR"),
                            static_cast<int>(ConnectionSettings::Protocol::Oscar));
        m_protocol->addItem(QStringLiteral("IRC"),
                            static_cast<int>(ConnectionSettings::Protocol::Irc));
        m_protocol->addItem(QStringLiteral("Telnet / MUD / BBS"),
                            static_cast<int>(ConnectionSettings::Protocol::Telnet));
        m_protocol->addItem(QStringLiteral("SIP / VoIP"),
                            static_cast<int>(ConnectionSettings::Protocol::Sip));

        const int wanted = m_protocol->findData(static_cast<int>(defaults.protocol));
        if (wanted >= 0 && (m_editing || defaults.protocol != ConnectionSettings::Protocol::Unknown)) {
            m_protocol->setCurrentIndex(wanted);
        } else {
            m_protocol->setCurrentIndex(0);
        }
        form->addRow(QStringLiteral("Protocol:"), m_protocol);
        m_protocol->setEnabled(!m_editing);

        m_serverLabel = new QLabel(QStringLiteral("Server:"), this);
        m_server = new QLineEdit(defaults.server, this);
        form->addRow(m_serverLabel, m_server);

        m_portLabel = new QLabel(QStringLiteral("Port:"), this);
        m_port = new QSpinBox(this);
        m_port->setRange(1, 65535);
        m_port->setValue(defaults.port ? defaults.port : 5190);
        form->addRow(m_portLabel, m_port);

        m_userLabel = new QLabel(QStringLiteral("Screen name:"), this);
        m_user = new QLineEdit(defaults.username, this);
        form->addRow(m_userLabel, m_user);

        m_passwordLabel = new QLabel(QStringLiteral("Password:"), this);
        m_password = new QLineEdit(this);
        m_password->setEchoMode(QLineEdit::Password);
        m_password->setText(defaults.password);
        form->addRow(m_passwordLabel, m_password);

        m_savePassword = new QCheckBox(QStringLiteral("Save password on this computer"), this);
        m_savePassword->setChecked(defaults.savePassword);
        m_savePassword->setToolTip(
            QStringLiteral("Stores this password in the local WaffleHouse-Client settings file. "
                           "The saved value is not encrypted at rest."));
        m_savePassword->setEnabled(!m_password->text().isEmpty());
        connect(m_password, &QLineEdit::textChanged, m_savePassword,
                [this](const QString &text) {
                    m_savePassword->setEnabled(!text.isEmpty());
                    if (text.isEmpty()) m_savePassword->setChecked(false);
                });
        form->addRow(QString(), m_savePassword);

        m_secretNote = new QLabel(
            QStringLiteral(
                "Passwords are session-only unless Save password is selected. "
                "Saved passwords are stored in the local application settings and are not "
                "encrypted at rest. Leave the password blank to be prompted when you connect."),
            this);
        m_secretNote->setWordWrap(true);
        form->addRow(QString(), m_secretNote);

        m_realNameLabel = new QLabel(QStringLiteral("Real name:"), this);
        m_realName = new QLineEdit(defaults.realName, this);
        form->addRow(m_realNameLabel, m_realName);

        m_tls = new QCheckBox(QStringLiteral("Use TLS"), this);
        m_tls->setChecked(defaults.tls);
        form->addRow(QString(), m_tls);

        m_redirectHostLabel = new QLabel(QStringLiteral("Redirect host:"), this);
        m_redirectHost = new QLineEdit(defaults.redirectHost, this);
        m_redirectHost->setPlaceholderText(QStringLiteral("optional"));
        form->addRow(m_redirectHostLabel, m_redirectHost);

        m_redirectPortLabel = new QLabel(QStringLiteral("Redirect port:"), this);
        m_redirectPort = new QSpinBox(this);
        m_redirectPort->setRange(0, 65535);
        m_redirectPort->setSpecialValueText(QStringLiteral("server default"));
        m_redirectPort->setValue(defaults.redirectPort);
        form->addRow(m_redirectPortLabel, m_redirectPort);

        m_telnetTerminalLabel = new QLabel(QStringLiteral("Terminal type:"), this);
        m_telnetTerminal = new QLineEdit(
            defaults.telnetTerminalType.isEmpty()
                ? QStringLiteral("ANSI")
                : defaults.telnetTerminalType,
            this);
        form->addRow(m_telnetTerminalLabel, m_telnetTerminal);

        m_sipProfileNameLabel = new QLabel(QStringLiteral("Account label:"), this);
        m_sipProfileName = new QLineEdit(defaults.sipProfileName, this);
        m_sipProfileName->setPlaceholderText(QStringLiteral("Office SIP / PBX / trunk"));
        form->addRow(m_sipProfileNameLabel, m_sipProfileName);

        m_sipDomainLabel = new QLabel(QStringLiteral("SIP domain:"), this);
        m_sipDomain = new QLineEdit(defaults.sipDomain.isEmpty() ? defaults.server : defaults.sipDomain, this);
        form->addRow(m_sipDomainLabel, m_sipDomain);

        m_sipRegistrarLabel = new QLabel(QStringLiteral("Registrar:"), this);
        m_sipRegistrar = new QLineEdit(defaults.sipRegistrar, this);
        m_sipRegistrar->setPlaceholderText(QStringLiteral("blank = sip:<domain>"));
        form->addRow(m_sipRegistrarLabel, m_sipRegistrar);

        m_sipAuthLabel = new QLabel(QStringLiteral("Auth username:"), this);
        m_sipAuth = new QLineEdit(defaults.sipAuthUsername, this);
        m_sipAuth->setPlaceholderText(QStringLiteral("blank = SIP username"));
        form->addRow(m_sipAuthLabel, m_sipAuth);

        m_sipDisplayNameLabel = new QLabel(QStringLiteral("Display name:"), this);
        m_sipDisplayName = new QLineEdit(defaults.sipDisplayName, this);
        form->addRow(m_sipDisplayNameLabel, m_sipDisplayName);

        m_sipOutboundProxyLabel = new QLabel(QStringLiteral("Outbound proxy:"), this);
        m_sipOutboundProxy = new QLineEdit(defaults.sipOutboundProxy, this);
        m_sipOutboundProxy->setPlaceholderText(QStringLiteral("optional"));
        form->addRow(m_sipOutboundProxyLabel, m_sipOutboundProxy);

        m_sipCallerIdDomainLabel = new QLabel(QStringLiteral("Caller-ID domain:"), this);
        m_sipCallerIdDomain = new QLineEdit(defaults.sipCallerIdDomain, this);
        m_sipCallerIdDomain->setPlaceholderText(QStringLiteral("optional"));
        form->addRow(m_sipCallerIdDomainLabel, m_sipCallerIdDomain);

        m_sipDialPrefixLabel = new QLabel(QStringLiteral("Dial prefix:"), this);
        m_sipDialPrefix = new QLineEdit(defaults.sipDialPrefix, this);
        m_sipDialPrefix->setPlaceholderText(QStringLiteral("optional"));
        form->addRow(m_sipDialPrefixLabel, m_sipDialPrefix);

        m_sipStunLabel = new QLabel(QStringLiteral("STUN server:"), this);
        m_sipStun = new QLineEdit(defaults.sipStunServer, this);
        m_sipStun->setPlaceholderText(QStringLiteral("optional"));
        form->addRow(m_sipStunLabel, m_sipStun);

        m_sipTransportLabel = new QLabel(QStringLiteral("Transport:"), this);
        m_sipTransport = new QComboBox(this);
        m_sipTransport->addItems({QStringLiteral("udp"), QStringLiteral("tcp"), QStringLiteral("tls")});
        m_sipTransport->setCurrentText(defaults.sipTransport.isEmpty() ? QStringLiteral("udp") : defaults.sipTransport.toCaseFolded());
        form->addRow(m_sipTransportLabel, m_sipTransport);

        m_sipIdentityLabel = new QLabel(QStringLiteral("Caller-ID identity:"), this);
        m_sipIdentity = new QComboBox(this);
        m_sipIdentity->addItems({QStringLiteral("from"), QStringLiteral("pai"), QStringLiteral("rpid"), QStringLiteral("from+pai")});
        m_sipIdentity->setCurrentText(defaults.sipIdentityMode.isEmpty() ? QStringLiteral("from") : defaults.sipIdentityMode.toCaseFolded());
        form->addRow(m_sipIdentityLabel, m_sipIdentity);

        m_sipLocalPortLabel = new QLabel(QStringLiteral("Local SIP port:"), this);
        m_sipLocalPort = new QSpinBox(this);
        m_sipLocalPort->setRange(1, 65535);
        m_sipLocalPort->setValue(defaults.sipLocalPort ? defaults.sipLocalPort : 5060);
        form->addRow(m_sipLocalPortLabel, m_sipLocalPort);

        m_sipExpiresLabel = new QLabel(QStringLiteral("Registration expiry:"), this);
        m_sipExpires = new QSpinBox(this);
        m_sipExpires->setRange(30, 86400);
        m_sipExpires->setSuffix(QStringLiteral(" sec"));
        m_sipExpires->setValue(defaults.sipRegistrationExpires ? static_cast<int>(defaults.sipRegistrationExpires) : 300);
        form->addRow(m_sipExpiresLabel, m_sipExpires);

        m_sipIce = new QCheckBox(QStringLiteral("Enable ICE"), this);
        m_sipIce->setChecked(defaults.sipUseIce);
        form->addRow(QString(), m_sipIce);
        m_sipSrtp = new QCheckBox(QStringLiteral("Enable SRTP"), this);
        m_sipSrtp->setChecked(defaults.sipEnableSrtp);
        form->addRow(QString(), m_sipSrtp);

        m_debug = new QCheckBox(QStringLiteral("Protocol debug logging"), this);
        m_debug->setChecked(defaults.debug);
        form->addRow(QString(), m_debug);

        auto *buttons = new QDialogButtonBox(
            QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);
        buttons->button(QDialogButtonBox::Ok)->setText(
            m_editing ? QStringLiteral("Save") : QStringLiteral("Add & Connect"));
        outer->addWidget(buttons);

        connect(buttons, &QDialogButtonBox::accepted, this, [this] {
            const ConnectionSettings candidate = settings();
            if (candidate.protocol == ConnectionSettings::Protocol::Unknown) {
                QMessageBox::warning(this,
                                     QStringLiteral("Select a protocol"),
                                     QStringLiteral("Choose AIM/OSCAR, IRC, Telnet, or SIP/VoIP."));
                return;
            }
            if (candidate.protocol == ConnectionSettings::Protocol::Telnet) {
                if (candidate.server.trimmed().isEmpty()) {
                    QMessageBox::warning(this, QStringLiteral("Missing information"), QStringLiteral("A Telnet server is required."));
                    return;
                }
            } else if (candidate.protocol == ConnectionSettings::Protocol::Sip) {
                if (candidate.sipDomain.trimmed().isEmpty() || candidate.username.trimmed().isEmpty()) {
                    QMessageBox::warning(this, QStringLiteral("Missing information"), QStringLiteral("SIP domain and SIP username/extension are required."));
                    return;
                }
                try {
                    trunkmonkey::ProfileStore::validate(sipProfileFromConnectionSettings(candidate));
                } catch (const std::exception &e) {
                    QMessageBox::warning(this, QStringLiteral("Invalid SIP Account"), QString::fromLocal8Bit(e.what()));
                    return;
                }
            } else if (candidate.server.trimmed().isEmpty() || candidate.username.trimmed().isEmpty()) {
                QMessageBox::warning(this, QStringLiteral("Missing information"), QStringLiteral("Server and account/nickname are required."));
                return;
            }
            accept();
        });
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
        connect(m_protocol, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this] { updateFields(); });
        connect(m_tls, &QCheckBox::toggled, this, [this](bool enabled) {
            if (currentProtocol() == ConnectionSettings::Protocol::Irc) {
                if (enabled && m_port->value() == 6667) {
                    m_port->setValue(6697);
                } else if (!enabled && m_port->value() == 6697) {
                    m_port->setValue(6667);
                }
            }
        });

        updateFields();
    }

    ConnectionSettings settings() const
    {
        ConnectionSettings value;
        value.protocol = currentProtocol();
        value.password = m_password->text();
        value.savePassword = m_savePassword->isChecked() && !value.password.isEmpty();
        value.debug = m_debug->isChecked();

        value.server = m_server->text().trimmed();
        value.port = static_cast<quint16>(m_port->value());
        value.username = m_user->text().trimmed();
        value.realName = m_realName->text().trimmed();
        value.tls = m_tls->isChecked();
        // Buddy/contact lists belong to the saved connection and are managed
        // by their dedicated Accounts window; editing transport/login fields
        // must not silently erase them.
        value.ircBuddies = m_original.ircBuddies;
        value.sipContacts = m_original.sipContacts;
        value.redirectHost = m_redirectHost->text().trimmed();
        value.redirectPort = static_cast<quint16>(m_redirectPort->value());
        value.telnetTerminalType = m_telnetTerminal->text().trimmed();
        value.sipProfileName = m_sipProfileName->text().trimmed();
        value.sipDomain = m_sipDomain->text().trimmed();
        value.sipRegistrar = m_sipRegistrar->text().trimmed();
        value.sipAuthUsername = m_sipAuth->text().trimmed();
        value.sipDisplayName = m_sipDisplayName->text().trimmed();
        value.sipOutboundProxy = m_sipOutboundProxy->text().trimmed();
        value.sipCallerIdDomain = m_sipCallerIdDomain->text().trimmed();
        value.sipDialPrefix = m_sipDialPrefix->text().trimmed();
        value.sipStunServer = m_sipStun->text().trimmed();
        value.sipTransport = m_sipTransport->currentText();
        value.sipIdentityMode = m_sipIdentity->currentText();
        value.sipLocalPort = static_cast<quint16>(m_sipLocalPort->value());
        value.sipRegistrationExpires = static_cast<quint32>(m_sipExpires->value());
        value.sipUseIce = m_sipIce->isChecked();
        value.sipEnableSrtp = m_sipSrtp->isChecked();
        if (value.protocol == ConnectionSettings::Protocol::Sip) {
            value.server = value.sipDomain;
            value.port = value.sipLocalPort;
        }
        return value;
    }

private:
    ConnectionSettings::Protocol currentProtocol() const
    {
        return static_cast<ConnectionSettings::Protocol>(m_protocol->currentData().toInt());
    }

    void updateFields()
    {
        const auto protocol = currentProtocol();
        const bool unknown = protocol == ConnectionSettings::Protocol::Unknown;
        const bool irc = protocol == ConnectionSettings::Protocol::Irc;
        const bool oscar = protocol == ConnectionSettings::Protocol::Oscar;
        const bool telnet = protocol == ConnectionSettings::Protocol::Telnet;
        const bool sip = protocol == ConnectionSettings::Protocol::Sip;

        m_serverLabel->setVisible(!unknown && !sip);
        m_server->setVisible(!unknown && !sip);
        m_portLabel->setVisible(!unknown && !sip);
        m_port->setVisible(!unknown && !sip);
        m_userLabel->setVisible(!unknown);
        m_user->setVisible(!unknown);

        m_passwordLabel->setVisible(!telnet && !unknown);
        m_password->setVisible(!telnet && !unknown);
        m_savePassword->setVisible(!telnet && !unknown);
        m_secretNote->setVisible(!telnet && !unknown);

        m_realNameLabel->setVisible(irc);
        m_realName->setVisible(irc);
        m_tls->setVisible(irc);

        m_redirectHostLabel->setVisible(oscar);
        m_redirectHost->setVisible(oscar);
        m_redirectPortLabel->setVisible(oscar);
        m_redirectPort->setVisible(oscar);

        m_telnetTerminalLabel->setVisible(telnet);
        m_telnetTerminal->setVisible(telnet);

        const QList<QWidget *> sipWidgets = {
            m_sipProfileNameLabel, m_sipProfileName, m_sipDomainLabel, m_sipDomain,
            m_sipRegistrarLabel, m_sipRegistrar, m_sipAuthLabel, m_sipAuth,
            m_sipDisplayNameLabel, m_sipDisplayName, m_sipOutboundProxyLabel, m_sipOutboundProxy,
            m_sipCallerIdDomainLabel, m_sipCallerIdDomain, m_sipDialPrefixLabel, m_sipDialPrefix,
            m_sipStunLabel, m_sipStun, m_sipTransportLabel, m_sipTransport,
            m_sipIdentityLabel, m_sipIdentity, m_sipLocalPortLabel, m_sipLocalPort,
            m_sipExpiresLabel, m_sipExpires, m_sipIce, m_sipSrtp};
        for (QWidget *widget : sipWidgets) widget->setVisible(sip);

        if (irc) {
            m_userLabel->setText(QStringLiteral("Nickname:"));
            m_passwordLabel->setText(QStringLiteral("Server password:"));
            m_password->setPlaceholderText(QStringLiteral("optional"));
            if (m_port->value() == 5190 || m_port->value() == 23) {
                m_port->setValue(m_tls->isChecked() ? 6697 : 6667);
            }
        } else if (telnet) {
            m_userLabel->setText(QStringLiteral("Session label:"));
            m_user->setPlaceholderText(QStringLiteral("optional"));
            if (m_port->value() == 5190 || m_port->value() == 6667
                || m_port->value() == 6697) {
                m_port->setValue(23);
            }
        } else if (oscar) {
            m_userLabel->setText(QStringLiteral("Screen name:"));
            m_passwordLabel->setText(QStringLiteral("Password:"));
            m_password->setPlaceholderText(QString());
            m_user->setPlaceholderText(QString());
            if (m_port->value() == 6667 || m_port->value() == 6697 || m_port->value() == 23) m_port->setValue(5190);
        } else if (sip) {
            m_userLabel->setText(QStringLiteral("SIP username / extension:"));
            m_passwordLabel->setText(QStringLiteral("SIP password:"));
            m_user->setPlaceholderText(QStringLiteral("1001 / user / auth identity"));
            m_password->setPlaceholderText(QString());
            if (m_sipProfileName->text().trimmed().isEmpty() && !m_user->text().trimmed().isEmpty())
                m_sipProfileName->setPlaceholderText(QStringLiteral("SIP — %1").arg(m_user->text().trimmed()));
        }
    }

    bool m_editing = false;
    ConnectionSettings m_original;
    QComboBox *m_protocol = nullptr;
    QLabel *m_serverLabel = nullptr;
    QLineEdit *m_server = nullptr;
    QLabel *m_portLabel = nullptr;
    QSpinBox *m_port = nullptr;
    QLabel *m_userLabel = nullptr;
    QLineEdit *m_user = nullptr;
    QLabel *m_passwordLabel = nullptr;
    QLineEdit *m_password = nullptr;
    QCheckBox *m_savePassword = nullptr;
    QLabel *m_secretNote = nullptr;
    QLabel *m_realNameLabel = nullptr;
    QLineEdit *m_realName = nullptr;
    QCheckBox *m_tls = nullptr;
    QLabel *m_redirectHostLabel = nullptr;
    QLineEdit *m_redirectHost = nullptr;
    QLabel *m_redirectPortLabel = nullptr;
    QSpinBox *m_redirectPort = nullptr;
    QLabel *m_telnetTerminalLabel = nullptr;
    QLineEdit *m_telnetTerminal = nullptr;
    QLabel *m_sipProfileNameLabel = nullptr; QLineEdit *m_sipProfileName = nullptr;
    QLabel *m_sipDomainLabel = nullptr; QLineEdit *m_sipDomain = nullptr;
    QLabel *m_sipRegistrarLabel = nullptr; QLineEdit *m_sipRegistrar = nullptr;
    QLabel *m_sipAuthLabel = nullptr; QLineEdit *m_sipAuth = nullptr;
    QLabel *m_sipDisplayNameLabel = nullptr; QLineEdit *m_sipDisplayName = nullptr;
    QLabel *m_sipOutboundProxyLabel = nullptr; QLineEdit *m_sipOutboundProxy = nullptr;
    QLabel *m_sipCallerIdDomainLabel = nullptr; QLineEdit *m_sipCallerIdDomain = nullptr;
    QLabel *m_sipDialPrefixLabel = nullptr; QLineEdit *m_sipDialPrefix = nullptr;
    QLabel *m_sipStunLabel = nullptr; QLineEdit *m_sipStun = nullptr;
    QLabel *m_sipTransportLabel = nullptr; QComboBox *m_sipTransport = nullptr;
    QLabel *m_sipIdentityLabel = nullptr; QComboBox *m_sipIdentity = nullptr;
    QLabel *m_sipLocalPortLabel = nullptr; QSpinBox *m_sipLocalPort = nullptr;
    QLabel *m_sipExpiresLabel = nullptr; QSpinBox *m_sipExpires = nullptr;
    QCheckBox *m_sipIce = nullptr; QCheckBox *m_sipSrtp = nullptr;
    QCheckBox *m_debug = nullptr;
};

QString statusWord(bool connected)
{
    return connected ? QStringLiteral("Online") : QStringLiteral("Offline");
}

} // namespace

MainWindow::MainWindow(const ConnectionSettings &defaults, QWidget *parent)
    : QMainWindow(parent),
      m_defaults(defaults)
{
    setWindowTitle(QStringLiteral("%1 %2 — Buddy List").arg(appDisplayName(), appVersionString()));
    // Start wide enough to expose the complete menu bar on typical Linux/FreeBSD
    // desktops. The window can still be resized narrower; the Options button below
    // remains available even when the right-most menu titles no longer fit.
    resize(520, 500);
    setMinimumSize(320, 360);

    m_sipController = new SipController(this);
    m_softphoneWindow = new SoftphoneWindow(m_sipController, nullptr);
    connect(m_softphoneWindow, &SoftphoneWindow::profileSaveRequested, this,
            [this](const QString &accountId, const trunkmonkey::SipProfile &profile, bool savePassword) {
                BackendState *state = stateById(accountId);
                if (!state || !state->backend
                    || state->backend->settings().protocol != ConnectionSettings::Protocol::Sip) {
                    return;
                }
                ConnectionSettings updated = state->backend->settings();
                applySipProfileToConnectionSettings(profile, updated);
                updated.savePassword = savePassword && !updated.password.isEmpty();
                state->backend->setConnectionSettings(updated);
                const ConnectionSettings applied = state->backend->settings();
                state->secretRequired = true;
                state->hasSessionSecret = !applied.password.isEmpty();
                updateConnectionItem(state);
                saveConnections();
                refreshBuddyList();
                appendActivity(state->backend, QStringLiteral("SIP account updated from Softphone Profile."));
            });

    buildUi();
    buildMenus();
    buildConnectionsWindow();
    m_transferWindow = new TransferWindow(nullptr);
    connect(m_transferWindow, &TransferWindow::cancelRequested,
            this, &MainWindow::cancelFileTransfer);
    connect(m_transferWindow, &TransferWindow::resumeRequested,
            this, &MainWindow::resumeFileTransfer);
    connect(m_transferWindow, &TransferWindow::clearRequested,
            this, &MainWindow::clearFileTransfer);
    connect(&m_directTransfers, &CpxDirectTransferManager::progress,
            this, &MainWindow::handleDirectProgress);
    connect(&m_directTransfers, &CpxDirectTransferManager::incomingFinished,
            this, &MainWindow::handleDirectIncomingFinished);
    connect(&m_directTransfers, &CpxDirectTransferManager::outgoingFinished,
            this, &MainWindow::handleDirectOutgoingFinished);
    connect(&m_directTransfers, &CpxDirectTransferManager::failed,
            this, &MainWindow::handleDirectFailure);
    buildTrayIcon();
    loadUiSettings();
    loadOptions();

    m_secureReady = m_secure.initialize(&m_secureError);
    m_fileTransferTimer = new QTimer(this);
    m_fileTransferTimer->setInterval(100);
    connect(m_fileTransferTimer, &QTimer::timeout, this, &MainWindow::pumpFileTransfers);
    m_fileTransferTimer->start();
    loadConnections();
    m_sipController->initialize();
    connect(m_sipController, &SipController::accountsChanged, this, &MainWindow::refreshSoftphoneControls);
    connect(m_sipController, &SipController::accountStateChanged, this, [this](const QString &) { refreshBuddyList(); refreshSoftphoneControls(); });
    connect(m_sipController, &SipController::callsChanged, this, [this] { refreshBuddyList(); refreshSoftphoneControls(); });
    connect(m_sipController, &SipController::incomingCall, this, [this](const QString &accountId, int, const QString &) {
        m_sipController->setSelectedAccountId(accountId);
        refreshBuddyList(); refreshSoftphoneControls();
    });
    refreshSoftphoneControls();
    applyTheme();

    setWindowOpacity(m_buddyOpacity);
    if (m_connectionsWindow) {
        m_connectionsWindow->setWindowOpacity(m_connectionsOpacity);
    }

    updateActions();
    refreshBuddyList();

    if (m_connectionList && m_connectionList->count() == 0) {
        statusBar()->showMessage(
            QStringLiteral("No saved connections. Use Connection → Add to create one."));
    } else if (m_connectionList) {
        statusBar()->showMessage(
            QStringLiteral("%1 saved connection(s) restored.")
                .arg(m_connectionList->count()));
    }

    if (!m_secureReady && !m_secureError.isEmpty()) {
        if (m_activity) {
            m_activity->appendPlainText(
                QStringLiteral("[security] Encrypted DMs unavailable: %1").arg(m_secureError));
        }
    }
}

MainWindow::~MainWindow()
{
    m_quitting = true;

    const auto windows = m_windows.values();
    for (ChatWindow *window : windows) {
        if (window) {
            window->close();
        }
    }

    const auto states = m_states.values();
    for (BackendState *state : states) {
        if (state && state->backend) {
            QObject::disconnect(state->backend, nullptr, this, nullptr);
            state->backend->stop();
        }
    }

    for (BackendState *state : states) {
        delete state;
    }
    m_states.clear();

    delete m_transferWindow;
    m_transferWindow = nullptr;

    delete m_softphoneWindow;
    m_softphoneWindow = nullptr;

    delete m_connectionsWindow;
    m_connectionsWindow = nullptr;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveConnections();
    saveUiSettings();

    if (!m_quitting && m_trayIcon && m_trayIcon->isVisible()) {
        hide();
        event->ignore();
        if (!m_trayHintShown) {
            m_trayHintShown = true;
            m_trayIcon->showMessage(
                QStringLiteral("%1 is still running").arg(appDisplayName()),
                QStringLiteral("Use the tray icon to reopen the Buddy List or quit %1.").arg(appDisplayName()),
                QSystemTrayIcon::Information,
                3500);
        }
        return;
    }

    if (!m_quitting) {
        m_quitting = true;
        QApplication::quit();
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::buildUi()
{
    auto *central = new QWidget(this);
    auto *outer = new QVBoxLayout(central);
    outer->setContentsMargins(6, 6, 6, 6);
    outer->setSpacing(5);

    auto *buddyGroup = new QGroupBox(QStringLiteral("Accounts / Buddies"), central);
    auto *buddyLayout = new QVBoxLayout(buddyGroup);
    buddyLayout->setContentsMargins(6, 8, 6, 6);

    m_buddyTree = new QTreeWidget(buddyGroup);
    m_buddyTree->setHeaderLabels({QStringLiteral("Buddy / Account"),
                                  QStringLiteral("Status")});
    m_buddyTree->setRootIsDecorated(true);
    m_buddyTree->setAlternatingRowColors(true);
    m_buddyTree->setUniformRowHeights(true);
    buddyLayout->addWidget(m_buddyTree, 1);
    outer->addWidget(buddyGroup, 1);

    auto *phoneGroup = new QGroupBox(QStringLiteral("Softphone"), central);
    auto *phoneLayout = new QGridLayout(phoneGroup);
    phoneLayout->setContentsMargins(6, 8, 6, 6);
    m_buddySipAccount = new QComboBox(phoneGroup);
    m_buddySipAccount->setToolTip(QStringLiteral("SIP account used for outbound calls"));
    m_buddyDialPrefix = new QLineEdit(phoneGroup);
    m_buddyDialPrefix->setPlaceholderText(QStringLiteral("PBX prefix (optional)"));
    m_buddyDialPrefix->setToolTip(QStringLiteral("Runtime prefix for the selected SIP account. Change per PBX without editing the saved account."));
    m_buddyDial = new QLineEdit(phoneGroup);
    m_buddyDial->setPlaceholderText(QStringLiteral("Number or SIP URI"));
    m_buddyDialButton = new QPushButton(QStringLiteral("Dial"), phoneGroup);
    m_buddySipConnectButton = new QPushButton(QStringLiteral("Register"), phoneGroup);
    m_buddySipDisconnectButton = new QPushButton(QStringLiteral("Disconnect"), phoneGroup);
    phoneLayout->addWidget(new QLabel(QStringLiteral("Account:"), phoneGroup), 0, 0);
    phoneLayout->addWidget(m_buddySipAccount, 0, 1, 1, 2);
    phoneLayout->addWidget(new QLabel(QStringLiteral("Prefix:"), phoneGroup), 1, 0);
    phoneLayout->addWidget(m_buddyDialPrefix, 1, 1, 1, 2);
    phoneLayout->addWidget(m_buddyDial, 2, 0, 1, 2);
    phoneLayout->addWidget(m_buddyDialButton, 2, 2);
    phoneLayout->addWidget(m_buddySipConnectButton, 3, 0, 1, 2);
    phoneLayout->addWidget(m_buddySipDisconnectButton, 3, 2);
    phoneLayout->setColumnStretch(1, 1);
    outer->addWidget(phoneGroup);

    // Keep the Buddy List controls usable at narrow window widths. 1.9 originally
    // put every control on one horizontal row, which could clip the right-hand
    // buttons. A two-row grid keeps all controls visible and gives Options a
    // permanent, menu-independent entry point.
    auto *buttons = new QGridLayout;
    buttons->setHorizontalSpacing(4);
    buttons->setVerticalSpacing(4);

    m_newImButton = new QPushButton(QStringLiteral("IM / Chat"), central);
    m_optionsButton = new QPushButton(QStringLiteral("Options"), central);
    m_buddyConnectButton = new QPushButton(QStringLiteral("Connect"), central);
    m_buddyDisconnectButton = new QPushButton(QStringLiteral("Disconnect"), central);

    m_newImButton->setToolTip(QStringLiteral("Open a direct message or chat room for the selected AIM/IRC account"));
    m_optionsButton->setToolTip(
        QStringLiteral("Open application options, themes, and security settings"));

    buttons->addWidget(m_newImButton, 0, 0);
    buttons->addWidget(m_buddyConnectButton, 0, 1);
    buttons->addWidget(m_buddyDisconnectButton, 0, 2);
    buttons->addWidget(m_optionsButton, 1, 0, 1, 3);
    for (int column = 0; column < 3; ++column) {
        buttons->setColumnStretch(column, 1);
    }
    outer->addLayout(buttons);

    setCentralWidget(central);
    statusBar()->showMessage(QStringLiteral("%1 ready").arg(appDisplayName()));

    connect(m_newImButton, &QPushButton::clicked,
            this, &MainWindow::newIm);
    connect(m_buddyConnectButton, &QPushButton::clicked,
            this, &MainWindow::connectSelected);
    connect(m_buddyDisconnectButton, &QPushButton::clicked,
            this, &MainWindow::disconnectSelected);
    connect(m_optionsButton, &QPushButton::clicked,
            this, &MainWindow::showOptionsDialog);
    connect(m_buddySipConnectButton, &QPushButton::clicked, this, [this] {
        const QString id = m_buddySipAccount ? m_buddySipAccount->currentData().toString() : QString();
        if (BackendState *state = stateById(id)) { selectState(state); connectSelected(); }
    });
    connect(m_buddySipDisconnectButton, &QPushButton::clicked, this, [this] {
        const QString id = m_buddySipAccount ? m_buddySipAccount->currentData().toString() : QString();
        if (BackendState *state = stateById(id)) { selectState(state); disconnectSelected(); }
    });
    connect(m_buddySipAccount, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        const QString backendId = m_buddySipAccount ? m_buddySipAccount->currentData().toString() : QString();
        if (!backendId.isEmpty()) {
            m_sipController->setSelectedAccountId(backendId);
            if (BackendState *state = stateById(backendId)) selectState(state);
        }
    });
    connect(m_buddyDialPrefix, &QLineEdit::editingFinished, this, [this] {
        const QString backendId=m_buddySipAccount?m_buddySipAccount->currentData().toString():QString();
        if(backendId.isEmpty()) return;
        QString error;
        if(!m_sipController->setDialPrefix(backendId,m_buddyDialPrefix->text().trimmed(),&error)){
            QMessageBox::warning(this,QStringLiteral("SIP Dial Prefix"),error);
            const QSignalBlocker blocker(m_buddyDialPrefix);
            m_buddyDialPrefix->setText(m_sipController->dialPrefix(backendId));
        }
    });
    auto quickDial = [this] {
        const QString backendId = m_buddySipAccount ? m_buddySipAccount->currentData().toString() : QString();
        const QString destination = m_buddyDial ? m_buddyDial->text().trimmed() : QString();
        if (backendId.isEmpty() || destination.isEmpty()) return;
        QString error;
        const int callId = m_sipController->dial(backendId, destination, QString(), &error);
        if (callId < 0) {
            QMessageBox::warning(this, QStringLiteral("SIP Call Failed"), error);
            return;
        }
        m_buddyDial->clear();
        m_softphoneWindow->showAndRaise();
        statusBar()->showMessage(QStringLiteral("Dialing %1 (call %2)").arg(destination).arg(callId), 4000);
    };
    connect(m_buddyDialButton, &QPushButton::clicked, this, quickDial);
    connect(m_buddyDial, &QLineEdit::returnPressed, this, quickDial);

    connect(m_buddyTree, &QTreeWidget::currentItemChanged,
            this, [this](QTreeWidgetItem *current) {
                if (BackendState *state = stateFromBuddyItem(current)) {
                    if (m_connectionList && state->connectionItem
                        && m_connectionList->currentItem() != state->connectionItem) {
                        m_connectionList->setCurrentItem(state->connectionItem);
                    }
                }
                updateActions();
            });

    connect(m_buddyTree, &QTreeWidget::itemDoubleClicked,
            this, [this](QTreeWidgetItem *item, int) {
                BackendState *state = stateFromBuddyItem(item);
                if (!state || !state->backend) {
                    return;
                }
                selectState(state);
                if (state->backend->settings().protocol == ConnectionSettings::Protocol::Sip) {
                    m_sipController->setSelectedAccountId(state->backend->id());
                    if (item && item->parent()) {
                        const QString contact = item->data(0, Qt::UserRole + 1).toString().trimmed();
                        const QVariant callData = item->data(0, Qt::UserRole + 2);
                        if (!contact.isEmpty()) {
                            if (m_buddySipAccount) {
                                const int idx = m_buddySipAccount->findData(state->backend->id());
                                if (idx >= 0) m_buddySipAccount->setCurrentIndex(idx);
                            }
                            if (m_buddyDial) {
                                m_buddyDial->setText(contact);
                                m_buddyDial->setFocus();
                            }
                            return;
                        }
                        if (callData.isValid()) {
                            m_softphoneWindow->showAndRaise();
                            return;
                        }
                    }
                    return;
                }
                if (item && item->parent() && state->connected) {
                    const QString buddy = item->data(0, Qt::UserRole + 1).toString();
                    if (!buddy.isEmpty()) {
                        ensureConversationWindow(
                            state->backend, QStringLiteral("im"), buddy, true);
                    }
                }
            });
}

void MainWindow::buildMenus()
{
    // Keep the menu bar owned by the WaffleHouse window instead of allowing a
    // desktop/global-menu integration to export it elsewhere. This also makes
    // menu behavior consistent between X11, Wayland, Linux, and FreeBSD.
    QMenuBar *bar = menuBar();
    bar->setNativeMenuBar(false);
    bar->setVisible(true);

    QMenu *connectionMenu = bar->addMenu(QStringLiteral("&Connection"));
    m_addConnectionAction = connectionMenu->addAction(QStringLiteral("&Add…"));
    m_importBbsAction = connectionMenu->addAction(QStringLiteral("Import &BBS List…"));
    m_editConnectionAction = connectionMenu->addAction(QStringLiteral("&Edit Selected…"));
    m_deleteConnectionAction = connectionMenu->addAction(QStringLiteral("&Delete Selected"));
    connectionMenu->addSeparator();
    m_connectAction = connectionMenu->addAction(QStringLiteral("&Connect Selected"));
    m_disconnectAction = connectionMenu->addAction(QStringLiteral("&Disconnect Selected"));
    connectionMenu->addSeparator();
    QAction *quitAction = connectionMenu->addAction(QStringLiteral("&Quit"));

    // Accounts is rebuilt from the saved WaffleHouse connection model. Each
    // account gets its own submenu so buddy/contact and conversation actions
    // cannot accidentally target whichever account happened to be selected in
    // another window.
    m_accountsMenu = bar->addMenu(QStringLiteral("&Accounts"));
    connect(m_accountsMenu, &QMenu::aboutToShow, this, &MainWindow::rebuildAccountsMenu);

    QMenu *viewMenu = bar->addMenu(QStringLiteral("&View"));

    // Themes are available directly from View -> Theme as well as from the full
    // Options dialog. The direct menu makes theme switching quick and obvious.
    QMenu *themeMenu = viewMenu->addMenu(QStringLiteral("&Theme"));
    auto *themeGroup = new QActionGroup(themeMenu);
    themeGroup->setExclusive(true);

    struct ThemeEntry {
        const char *label;
        const char *id;
    };
    static constexpr ThemeEntry themeEntries[] = {
        {"System", "system"},
        {"Hacker", "hacker"},
        {"Matrix", "matrix"},
        {"Phosphor", "phosphor"},
        {"Midnight", "midnight"},
        {"Amber", "amber"},
        {"Ice", "ice"},
        {"Classic Light", "classic-light"},
        {"Cyberpunk", "cyberpunk"},
        {"Synthwave", "synthwave"},
        {"Dracula", "dracula"},
        {"Vaporwave", "vaporwave"},
        {"Blood Moon", "blood-moon"},
        {"C64", "c64"},
        {"DOS", "dos"},
        {"Solarized Dark", "solarized-dark"},
        {"Waffle Iron", "waffle-iron"},
        {"Ghostline", "ghostline"},
        {"Hot Dog Stand", "hot-dog-stand"},
        {"Neon Miami", "neon-miami"},
    };

    for (const ThemeEntry &entry : themeEntries) {
        QAction *action = themeMenu->addAction(QString::fromLatin1(entry.label));
        action->setCheckable(true);
        action->setData(QString::fromLatin1(entry.id));
        themeGroup->addAction(action);
        connect(action, &QAction::triggered, this, [this, action] {
            m_options.theme = action->data().toString();
            saveOptions();
            applyTheme();
            statusBar()->showMessage(
                QStringLiteral("Theme changed to %1.").arg(action->text()), 2500);
        });
    }

    // loadOptions() runs after the menu is constructed, so refresh the check mark
    // immediately before the submenu opens instead of assuming the startup value.
    connect(themeMenu, &QMenu::aboutToShow, this, [this, themeMenu] {
        for (QAction *action : themeMenu->actions()) {
            action->setChecked(action->data().toString() == m_options.theme);
        }
    });

    viewMenu->addSeparator();
    m_transferWindowAction = viewMenu->addAction(QStringLiteral("&File Transfers…"));
    viewMenu->addSeparator();
    m_buddyTransparencyAction =
        viewMenu->addAction(QStringLiteral("Buddy List &Transparency…"));
    m_connectionsTransparencyAction =
        viewMenu->addAction(QStringLiteral("Connections Window T&ransparency…"));

    QMenu *toolsMenu = bar->addMenu(QStringLiteral("&Tools"));
    m_phoneAction = toolsMenu->addAction(QStringLiteral("Open &Softphone…"));
    m_showConnectionsAction = toolsMenu->addAction(QStringLiteral("Show &Connections Window"));
    toolsMenu->addSeparator();
    m_changePasswordAction = toolsMenu->addAction(QStringLiteral("Change AIM &Password…"));
    m_fingerprintAction = toolsMenu->addAction(QStringLiteral("Secure Identity &Fingerprint…"));
    toolsMenu->addSeparator();
    m_rawAction = toolsMenu->addAction(QStringLiteral("Send &Raw Protocol Command…"));
    toolsMenu->addSeparator();
    m_optionsAction = toolsMenu->addAction(QStringLiteral("&Options…"));
    m_optionsAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+,")));
    m_optionsAction->setStatusTip(
        QStringLiteral("Open application options, themes, and security settings"));

    QMenu *helpMenu = bar->addMenu(QStringLiteral("&Help"));
    QAction *helpAction = helpMenu->addAction(QStringLiteral("%1 &Help…").arg(appDisplayName()));
    QAction *aboutAction = helpMenu->addAction(QStringLiteral("&About %1").arg(appDisplayName()));

    connect(m_addConnectionAction, &QAction::triggered,
            this, [this] { openConnectionDialog(m_defaults, nullptr); });
    connect(m_importBbsAction, &QAction::triggered,
            this, &MainWindow::importBbsList);
    connect(m_editConnectionAction, &QAction::triggered,
            this, &MainWindow::editSelected);
    connect(m_deleteConnectionAction, &QAction::triggered,
            this, &MainWindow::deleteSelected);
    connect(m_connectAction, &QAction::triggered,
            this, &MainWindow::connectSelected);
    connect(m_disconnectAction, &QAction::triggered,
            this, &MainWindow::disconnectSelected);
    connect(m_showConnectionsAction, &QAction::triggered,
            this, &MainWindow::showConnectionsWindow);
    connect(m_phoneAction, &QAction::triggered,
            m_softphoneWindow, &SoftphoneWindow::showAndRaise);
    connect(quitAction, &QAction::triggered,
            this, &MainWindow::quitApplication);

    connect(m_changePasswordAction, &QAction::triggered,
            this, &MainWindow::changePassword);
    connect(m_fingerprintAction, &QAction::triggered,
            this, &MainWindow::showSelectedFingerprint);
    connect(m_transferWindowAction, &QAction::triggered,
            this, &MainWindow::showTransferWindow);
    connect(m_buddyTransparencyAction, &QAction::triggered,
            this, &MainWindow::setBuddyTransparency);
    connect(m_connectionsTransparencyAction, &QAction::triggered,
            this, &MainWindow::setConnectionsTransparency);
    connect(m_rawAction, &QAction::triggered,
            this, &MainWindow::rawProtocolCommand);
    connect(m_optionsAction, &QAction::triggered,
            this, &MainWindow::showOptionsDialog);
    connect(helpAction, &QAction::triggered,
            this, &MainWindow::showHelpDialog);

    connect(aboutAction, &QAction::triggered, this, [this] {
        const QString aboutHtml = QStringLiteral(
                "<pre>%1</pre>"
                "<b>%2 — Version %3</b><br><br>"
                "A native C++/Qt communications client for AIM/OSCAR, IRC, Telnet/MUD/BBS, and SIP/VoIP calls.<br><br>"
                "%2 supports optional CPX3-compatible encrypted private messages with fingerprint verification.")
                .arg(appAsciiLogo().toHtmlEscaped(), appDisplayName(), appVersionString());
        QMessageBox::about(this, QStringLiteral("About %1").arg(appDisplayName()), aboutHtml);
    });
}

QString MainWindow::accountMenuLabel(BackendState *state) const
{
    if (!state || !state->backend) return QStringLiteral("Unknown account");
    const ConnectionSettings &cfg = state->backend->settings();
    QString name;
    if (cfg.protocol == ConnectionSettings::Protocol::Sip) {
        name = cfg.sipProfileName.trimmed();
        if (name.isEmpty() && !cfg.username.trimmed().isEmpty()) {
            const QString domain = cfg.sipDomain.trimmed().isEmpty() ? cfg.server.trimmed() : cfg.sipDomain.trimmed();
            name = domain.isEmpty() ? cfg.username.trimmed()
                                    : QStringLiteral("%1@%2").arg(cfg.username.trimmed(), domain);
        }
    } else {
        name = state->identity.trimmed();
        if (name.isEmpty()) name = cfg.username.trimmed();
        if (name.isEmpty()) name = cfg.server.trimmed();
    }
    if (name.isEmpty()) name = state->backend->protocolName();
    return QStringLiteral("%1 / %2").arg(name, state->backend->protocolName());
}

void MainWindow::rebuildAccountsMenu()
{
    if (!m_accountsMenu) return;
    m_accountsMenu->clear();

    QList<BackendState *> states = m_states.values();
    std::sort(states.begin(), states.end(), [this](BackendState *a, BackendState *b) {
        return accountMenuLabel(a).compare(accountMenuLabel(b), Qt::CaseInsensitive) < 0;
    });

    if (states.isEmpty()) {
        QAction *empty = m_accountsMenu->addAction(QStringLiteral("No saved accounts"));
        empty->setEnabled(false);
        return;
    }

    for (BackendState *state : states) {
        if (!state || !state->backend) continue;
        const QString backendId = state->backend->id();
        const auto protocol = state->backend->settings().protocol;
        QMenu *account = m_accountsMenu->addMenu(accountMenuLabel(state));

        QAction *select = account->addAction(QStringLiteral("Select Account"));
        connect(select, &QAction::triggered, this, [this, backendId] {
            if (BackendState *target = stateById(backendId)) selectState(target);
        });

        QAction *edit = account->addAction(QStringLiteral("Edit Connection…"));
        edit->setEnabled(!state->connected && !state->connecting);
        connect(edit, &QAction::triggered, this, [this, backendId] {
            if (BackendState *target = stateById(backendId)) {
                selectState(target);
                openConnectionDialog(target->backend->settings(), target);
            }
        });

        QAction *toggle = account->addAction(
            state->connected || state->connecting
                ? QStringLiteral("Disconnect")
                : QStringLiteral("Connect"));
        connect(toggle, &QAction::triggered, this, [this, backendId] {
            if (BackendState *target = stateById(backendId)) {
                selectState(target);
                if (target->connected || target->connecting) disconnectSelected();
                else connectSelected();
            }
        });

        if (protocol == ConnectionSettings::Protocol::Oscar
            || protocol == ConnectionSettings::Protocol::Irc) {
            account->addSeparator();
            QAction *conversation = account->addAction(QStringLiteral("IM / Chatroom…"));
            conversation->setEnabled(state->connected);
            connect(conversation, &QAction::triggered, this, [this, backendId] {
                if (BackendState *target = stateById(backendId)) {
                    selectState(target);
                    openMessagingDialog(target);
                }
            });

            QAction *buddies = account->addAction(QStringLiteral("Add / Remove Buddies…"));
            connect(buddies, &QAction::triggered, this, [this, backendId] {
                if (BackendState *target = stateById(backendId)) {
                    selectState(target);
                    openBuddyManager(target);
                }
            });

            if (protocol == ConnectionSettings::Protocol::Irc) {
                QAction *nick = account->addAction(QStringLiteral("Change IRC Nickname…"));
                nick->setEnabled(state->connected);
                connect(nick, &QAction::triggered, this, [this, backendId] {
                    if (BackendState *target = stateById(backendId)) {
                        selectState(target);
                        changeIrcNick();
                    }
                });
            }
        } else if (protocol == ConnectionSettings::Protocol::Sip) {
            account->addSeparator();
            QAction *contacts = account->addAction(QStringLiteral("Add / Remove Buddies / Contacts…"));
            connect(contacts, &QAction::triggered, this, [this, backendId] {
                if (BackendState *target = stateById(backendId)) {
                    selectState(target);
                    openBuddyManager(target);
                }
            });
        }
    }
}

void MainWindow::openMessagingDialog(BackendState *state,
                                     const QString &presetTarget,
                                     bool startRoomTab)
{
    if (!state || !state->backend) return;
    const auto protocol = state->backend->settings().protocol;
    if (protocol != ConnectionSettings::Protocol::Oscar
        && protocol != ConnectionSettings::Protocol::Irc) {
        QMessageBox::information(this, QStringLiteral("IM / Chatroom"),
                                 QStringLiteral("This connection type does not provide WaffleHouse IM/chat rooms."));
        return;
    }
    if (!state->connected) {
        QMessageBox::information(this, QStringLiteral("IM / Chatroom"),
                                 QStringLiteral("Connect %1 first.").arg(accountMenuLabel(state)));
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("%1 — IM / Chatroom").arg(accountMenuLabel(state)));
    dialog.setMinimumWidth(480);
    auto *outer = new QVBoxLayout(&dialog);
    auto *tabs = new QTabWidget(&dialog);

    auto *imTab = new QWidget(tabs);
    auto *imForm = new QFormLayout(imTab);
    auto *target = new QComboBox(imTab);
    target->setEditable(true);
    target->setInsertPolicy(QComboBox::NoInsert);
    QStringList buddies = state->buddies.values();
    buddies.sort(Qt::CaseInsensitive);
    target->addItems(buddies);
    if (!presetTarget.trimmed().isEmpty()) target->setCurrentText(presetTarget.trimmed());
    else target->setCurrentIndex(-1);
    target->setPlaceholderText(QStringLiteral("screen name or nickname"));
    imForm->addRow(QStringLiteral("Buddy / user:"), target);
    auto *imHint = new QLabel(QStringLiteral("Open a private message using this account."), imTab);
    imHint->setWordWrap(true);
    imForm->addRow(QString(), imHint);
    tabs->addTab(imTab, QStringLiteral("Instant Message"));

    auto *roomTab = new QWidget(tabs);
    auto *roomForm = new QFormLayout(roomTab);
    auto *room = new QComboBox(roomTab);
    room->setEditable(true);
    room->setInsertPolicy(QComboBox::NoInsert);
    QStringList rooms = state->discoveredRooms.values();
    rooms.sort(Qt::CaseInsensitive);
    room->addItems(rooms);
    room->setCurrentIndex(-1);
    room->setPlaceholderText(protocol == ConnectionSettings::Protocol::Irc
                                 ? QStringLiteral("#channel")
                                 : QStringLiteral("room name"));
    roomForm->addRow(protocol == ConnectionSettings::Protocol::Irc
                         ? QStringLiteral("Channel:")
                         : QStringLiteral("Room:"), room);
    QCheckBox *privateRoom = nullptr;
    if (protocol == ConnectionSettings::Protocol::Oscar) {
        privateRoom = new QCheckBox(QStringLiteral("Private AIM exchange (create if needed)"), roomTab);
        roomForm->addRow(QString(), privateRoom);
    }
    auto *roomHint = new QLabel(
        protocol == ConnectionSettings::Protocol::Irc
            ? QStringLiteral("Join an IRC channel and open its conversation window.")
            : QStringLiteral("Join or create an AIM chat room and open its conversation window."),
        roomTab);
    roomHint->setWordWrap(true);
    roomForm->addRow(QString(), roomHint);
    tabs->addTab(roomTab, QStringLiteral("Chat Room"));

    tabs->setCurrentIndex(startRoomTab ? 1 : 0);
    outer->addWidget(tabs);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, &dialog);
    if (auto *ok = buttons->button(QDialogButtonBox::Ok)) ok->setText(QStringLiteral("Open"));
    outer->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    if (tabs->currentIndex() == 0) {
        const QString buddy = target->currentText().trimmed();
        if (!buddy.isEmpty()) ensureConversationWindow(state->backend, QStringLiteral("im"), buddy, true);
        return;
    }

    QString roomName = room->currentText().trimmed();
    if (roomName.isEmpty()) return;
    if (protocol == ConnectionSettings::Protocol::Irc
        && !QStringLiteral("#&+!").contains(roomName.front())) {
        roomName.prepend(QLatin1Char('#'));
    }
    m_closedRoomKeys.remove(conversationKey(state->backend, QStringLiteral("chat"), roomName));
    state->backend->joinRoom(roomName, privateRoom && privateRoom->isChecked());
    ensureConversationWindow(state->backend, QStringLiteral("chat"), roomName, true);
}

void MainWindow::openBuddyManager(BackendState *state)
{
    if (!state || !state->backend) return;
    const auto protocol = state->backend->settings().protocol;
    if (protocol != ConnectionSettings::Protocol::Oscar
        && protocol != ConnectionSettings::Protocol::Irc
        && protocol != ConnectionSettings::Protocol::Sip) {
        QMessageBox::information(this, QStringLiteral("Buddies / Contacts"),
                                 QStringLiteral("This connection type does not maintain a buddy/contact list."));
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("%1 — Buddies / Contacts").arg(accountMenuLabel(state)));
    dialog.resize(480, 430);
    auto *outer = new QVBoxLayout(&dialog);

    auto *description = new QLabel(&dialog);
    if (protocol == ConnectionSettings::Protocol::Sip) {
        description->setText(QStringLiteral(
            "SIP contacts are local WaffleHouse dial targets for this SIP account. "
            "They are not uploaded to the PBX."));
    } else if (!state->connected) {
        description->setText(QStringLiteral(
            "This account is offline. Connect it before changing its buddy list."));
    } else if (protocol == ConnectionSettings::Protocol::Irc) {
        description->setText(QStringLiteral(
            "IRC buddies are local nickname watches for this saved IRC account."));
    } else {
        description->setText(QStringLiteral(
            "AIM buddy changes are sent through the selected AIM/OSCAR account."));
    }
    description->setWordWrap(true);
    outer->addWidget(description);

    auto *list = new QListWidget(&dialog);
    outer->addWidget(list, 1);

    auto refill = [list](const QStringList &provided) {
        QStringList names = provided;
        names.removeDuplicates();
        names.sort(Qt::CaseInsensitive);
        const QString selected = list->currentItem() ? list->currentItem()->text() : QString();
        list->clear();
        list->addItems(names);
        if (!selected.isEmpty()) {
            const auto matches = list->findItems(selected, Qt::MatchFixedString);
            if (!matches.isEmpty()) list->setCurrentItem(matches.first());
        }
    };
    refill(state->buddies.values());

    auto *addRow = new QHBoxLayout;
    auto *entry = new QLineEdit(&dialog);
    entry->setPlaceholderText(protocol == ConnectionSettings::Protocol::Sip
                                  ? QStringLiteral("extension, number, user@domain, or SIP URI")
                                  : protocol == ConnectionSettings::Protocol::Irc
                                        ? QStringLiteral("nickname to watch")
                                        : QStringLiteral("AIM screen name"));
    auto *add = new QPushButton(QStringLiteral("Add"), &dialog);
    auto *remove = new QPushButton(QStringLiteral("Remove Selected"), &dialog);
    addRow->addWidget(entry, 1);
    addRow->addWidget(add);
    outer->addLayout(addRow);
    outer->addWidget(remove);

    const bool mutableNow = protocol == ConnectionSettings::Protocol::Sip || state->connected;
    entry->setEnabled(mutableNow);
    add->setEnabled(mutableNow);
    remove->setEnabled(mutableNow && list->currentItem());
    connect(list, &QListWidget::currentItemChanged, &dialog,
            [remove, mutableNow](QListWidgetItem *current) {
                remove->setEnabled(mutableNow && current);
            });

    connect(state->backend, &ChatBackend::buddyListChanged, &dialog,
            [refill](const QStringList &names) mutable { refill(names); });

    auto addBuddyNow = [this, state, entry] {
        const QString name = entry->text().trimmed();
        if (name.isEmpty()) return;
        state->backend->addBuddy(name);
        saveConnections();
        entry->clear();
    };
    connect(add, &QPushButton::clicked, &dialog, addBuddyNow);
    connect(entry, &QLineEdit::returnPressed, &dialog, addBuddyNow);
    connect(remove, &QPushButton::clicked, &dialog, [this, state, list] {
        QListWidgetItem *item = list->currentItem();
        if (!item) return;
        state->backend->removeBuddy(item->text());
        saveConnections();
    });

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    outer->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    dialog.exec();
}

void MainWindow::showTransferWindow()
{
    if (m_transferWindow) m_transferWindow->showAndRaise();
}

void MainWindow::buildConnectionsWindow()
{
    m_connectionsWindow = new QMainWindow(nullptr);
    m_connectionsWindow->setWindowTitle(QStringLiteral("%1 — Connections").arg(appDisplayName()));
    m_connectionsWindow->resize(560, 390);
    m_connectionsWindow->setMinimumSize(450, 300);
    m_connectionsWindow->setAttribute(Qt::WA_QuitOnClose, false);

    auto *central = new QWidget(m_connectionsWindow);
    auto *outer = new QVBoxLayout(central);
    outer->setContentsMargins(7, 7, 7, 7);
    outer->setSpacing(6);

    m_connectionList = new QListWidget(central);
    m_connectionList->setMinimumHeight(105);
    outer->addWidget(m_connectionList, 1);

    auto *buttonRow = new QHBoxLayout;
    buttonRow->setSpacing(4);
    m_addConnectionButton = new QPushButton(QStringLiteral("Add"), central);
    m_editConnectionButton = new QPushButton(QStringLiteral("Edit"), central);
    m_deleteConnectionButton = new QPushButton(QStringLiteral("Delete"), central);
    m_connectButton = new QPushButton(QStringLiteral("Connect"), central);
    m_disconnectButton = new QPushButton(QStringLiteral("Disconnect"), central);
    buttonRow->addWidget(m_addConnectionButton);
    buttonRow->addWidget(m_editConnectionButton);
    buttonRow->addWidget(m_deleteConnectionButton);
    buttonRow->addStretch(1);
    buttonRow->addWidget(m_connectButton);
    buttonRow->addWidget(m_disconnectButton);
    outer->addLayout(buttonRow);

    auto *activityLabel = new QLabel(QStringLiteral("Activity"), central);
    QFont labelFont = activityLabel->font();
    labelFont.setBold(true);
    activityLabel->setFont(labelFont);
    outer->addWidget(activityLabel);

    m_activity = new QPlainTextEdit(central);
    m_activity->setReadOnly(true);
    m_activity->setMaximumBlockCount(2500);
    m_activity->setMinimumHeight(105);
    outer->addWidget(m_activity, 1);

    m_connectionsWindow->setCentralWidget(central);

    connect(m_addConnectionButton, &QPushButton::clicked,
            this, [this] { openConnectionDialog(m_defaults, nullptr); });
    connect(m_editConnectionButton, &QPushButton::clicked,
            this, &MainWindow::editSelected);
    connect(m_deleteConnectionButton, &QPushButton::clicked,
            this, &MainWindow::deleteSelected);
    connect(m_connectButton, &QPushButton::clicked,
            this, &MainWindow::connectSelected);
    connect(m_disconnectButton, &QPushButton::clicked,
            this, &MainWindow::disconnectSelected);

    connect(m_connectionList, &QListWidget::currentItemChanged,
            this, [this](QListWidgetItem *current) {
                BackendState *state = current
                    ? stateById(current->data(Qt::UserRole).toString())
                    : nullptr;
                if (state) {
                    selectState(state);
                }
                updateActions();
            });
}

void MainWindow::buildTrayIcon()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        statusBar()->showMessage(
            QStringLiteral("System tray is unavailable; closing Buddy List will quit %1.").arg(appDisplayName()),
            6000);
        return;
    }

    m_trayIcon = new QSystemTrayIcon(this);
    QIcon icon = QIcon::fromTheme(QStringLiteral("internet-chat"));
    if (icon.isNull()) {
        icon = windowIcon();
    }
    // Alpha builds do not yet ship a project-specific artwork resource.  Give
    // the tray icon a guaranteed Qt fallback so QSystemTrayIcon is never shown
    // with a null icon on minimal Linux/FreeBSD desktop themes.
    if (icon.isNull() && QApplication::style()) {
        icon = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
    }
    if (icon.isNull()) {
        delete m_trayIcon;
        m_trayIcon = nullptr;
        statusBar()->showMessage(
            QStringLiteral("No usable tray icon is available; closing Buddy List will quit %1.")
                .arg(appDisplayName()),
            6000);
        return;
    }
    m_trayIcon->setIcon(icon);
    m_trayIcon->setToolTip(appDisplayName());

    m_trayMenu = new QMenu(this);
    m_trayShowBuddyAction = m_trayMenu->addAction(QStringLiteral("Show Buddy List"));
    m_trayShowConnectionsAction = m_trayMenu->addAction(QStringLiteral("Show Connections"));
    m_trayShowPhoneAction = m_trayMenu->addAction(QStringLiteral("Show Softphone"));
    m_trayMenu->addSeparator();
    m_trayConnectAction = m_trayMenu->addAction(QStringLiteral("Connect Selected"));
    m_trayDisconnectAction = m_trayMenu->addAction(QStringLiteral("Disconnect Selected"));
    m_trayMenu->addSeparator();
    QAction *quitAction = m_trayMenu->addAction(QStringLiteral("Quit %1").arg(appDisplayName()));

    connect(m_trayShowBuddyAction, &QAction::triggered,
            this, &MainWindow::showBuddyWindow);
    connect(m_trayShowConnectionsAction, &QAction::triggered,
            this, &MainWindow::showConnectionsWindow);
    connect(m_trayShowPhoneAction, &QAction::triggered,
            m_softphoneWindow, &SoftphoneWindow::showAndRaise);
    connect(m_trayConnectAction, &QAction::triggered,
            this, &MainWindow::connectSelected);
    connect(m_trayDisconnectAction, &QAction::triggered,
            this, &MainWindow::disconnectSelected);
    connect(quitAction, &QAction::triggered,
            this, &MainWindow::quitApplication);

    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, [this](QSystemTrayIcon::ActivationReason reason) {
                if (reason == QSystemTrayIcon::Trigger
                    || reason == QSystemTrayIcon::DoubleClick) {
                    showBuddyWindow();
                }
            });

    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->show();
}

void MainWindow::openConnectionDialog(const ConnectionSettings &defaults,
                                      BackendState *editingState)
{
    if (editingState && (editingState->connected || editingState->connecting)) {
        QMessageBox::information(
            this,
            QStringLiteral("Edit Connection"),
            QStringLiteral("Disconnect this connection before editing it."));
        return;
    }

    if (editingState && editingState->backend
        && editingState->backend->settings().protocol == ConnectionSettings::Protocol::Sip) {
        for (const auto &call : m_sipController->calls()) {
            if (!call.disconnected
                && QString::fromStdString(call.accountId) == editingState->backend->id()) {
                QMessageBox::information(
                    this,
                    QStringLiteral("SIP Account In Use"),
                    QStringLiteral("Hang up active calls on this SIP account before editing it."));
                return;
            }
        }
    }

    ConnectionDialog dialog(defaults, editingState != nullptr, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    ConnectionSettings settings = dialog.settings();

    if (editingState && editingState->backend) {
        const ConnectionSettings oldSettings = editingState->backend->settings();
        if (settings.password.isEmpty()) {
            settings.password = oldSettings.password;
            // An unchecked Save password box explicitly disables persistence even when
            // the existing session password remains usable until the application exits.
            if (!settings.savePassword) {
                settings.savePassword = false;
            }
        }

        editingState->backend->setConnectionSettings(settings);
        editingState->identity.clear();
        editingState->endpoint.clear();
        editingState->buddies.clear();
        editingState->onlineBuddies.clear();
        if (settings.protocol == ConnectionSettings::Protocol::Irc) {
            for (const QString &buddy : settings.ircBuddies)
                if (!buddy.trimmed().isEmpty()) editingState->buddies.insert(buddy.trimmed());
        } else if (settings.protocol == ConnectionSettings::Protocol::Sip) {
            for (const QString &contact : settings.sipContacts)
                if (!contact.trimmed().isEmpty()) editingState->buddies.insert(contact.trimmed());
        }
        editingState->targetNames.clear();
        editingState->discoveredRooms.clear();

        if (settings.protocol == ConnectionSettings::Protocol::Irc) {
            editingState->secretRequired =
                editingState->secretRequired || !settings.password.isEmpty();
        } else if (settings.protocol == ConnectionSettings::Protocol::Telnet) {
            editingState->secretRequired = false;
        } else {
            editingState->secretRequired = true;
        }
        editingState->hasSessionSecret = !settings.password.isEmpty();

        updateConnectionItem(editingState);
        refreshBuddyList();
        updateActions();
        saveConnections();
        appendActivity(editingState->backend,
                       QStringLiteral("Saved connection updated."));
        return;
    }

    ChatBackend *backend = createBackend(settings);
    if (!backend) {
        return;
    }

    const bool secretRequired =
        settings.protocol == ConnectionSettings::Protocol::Oscar
        || settings.protocol == ConnectionSettings::Protocol::Sip
        || (settings.protocol == ConnectionSettings::Protocol::Irc
            && !settings.password.isEmpty());

    attachBackend(backend,
                  true,
                  secretRequired,
                  !settings.password.isEmpty(),
                  false);
}


void MainWindow::importBbsList()
{
    const QString path = QFileDialog::getOpenFileName(
        this, QStringLiteral("Import BBS List"), QString(),
        QStringLiteral("BBS Lists (*.csv *.tsv *.json *.txt);;All Files (*)"));
    if (path.isEmpty()) return;

    QString error;
    const auto entries = BbsDirectory::loadFile(path, &error);
    if (entries.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("BBS Import"),
                             error.isEmpty() ? QStringLiteral("No BBS entries were found in the selected file.") : error);
        return;
    }

    int added = 0, skipped = 0;
    for (const auto &bbs : entries) {
        bool duplicate = false;
        for (BackendState *state : m_states) {
            if (!state || !state->backend) continue;
            const auto &cfg = state->backend->settings();
            if (cfg.protocol == ConnectionSettings::Protocol::Telnet
                && cfg.server.compare(bbs.host, Qt::CaseInsensitive) == 0
                && cfg.port == bbs.port) { duplicate = true; break; }
        }
        if (duplicate) { ++skipped; continue; }
        ConnectionSettings cfg;
        cfg.protocol = ConnectionSettings::Protocol::Telnet;
        cfg.server = bbs.host;
        cfg.port = bbs.port;
        cfg.username = bbs.name;
        cfg.telnetTerminalType = bbs.terminalType.isEmpty() ? QStringLiteral("ANSI") : bbs.terminalType;
        if (ChatBackend *backend = createBackend(cfg)) {
            attachBackend(backend, true, false, false, false);
            ++added;
        }
    }
    QMessageBox::information(this, QStringLiteral("BBS Import"),
        QStringLiteral("Imported %1 BBS entries. %2 duplicates skipped.\n\nImported BBSes are saved offline; use Connect when you want to open one.")
            .arg(added).arg(skipped));
}

void MainWindow::loadUiSettings()
{
    QSettings settings;
    m_buddyOpacity = std::clamp(
        settings.value(QStringLiteral("ui/buddyOpacity"), 1.0).toDouble(),
        0.30,
        1.0);
    m_connectionsOpacity = std::clamp(
        settings.value(QStringLiteral("ui/connectionsOpacity"), 1.0).toDouble(),
        0.30,
        1.0);
}

void MainWindow::saveUiSettings() const
{
    QSettings settings;
    settings.setValue(QStringLiteral("ui/buddyOpacity"), m_buddyOpacity);
    settings.setValue(QStringLiteral("ui/connectionsOpacity"), m_connectionsOpacity);
}

void MainWindow::loadOptions()
{
    QSettings settings;
    m_options.theme = settings.value(QStringLiteral("ui/theme"), QStringLiteral("system"))
                          .toString().toCaseFolded();
    const QSet<QString> validThemes{QStringLiteral("system"), QStringLiteral("hacker"), QStringLiteral("matrix"),
                                    QStringLiteral("phosphor"), QStringLiteral("midnight"),
                                    QStringLiteral("amber"), QStringLiteral("ice"),
                                    QStringLiteral("classic-light"), QStringLiteral("cyberpunk"),
                                    QStringLiteral("synthwave"), QStringLiteral("dracula"),
                                    QStringLiteral("vaporwave"), QStringLiteral("blood-moon"),
                                    QStringLiteral("c64"), QStringLiteral("dos"),
                                    QStringLiteral("solarized-dark"), QStringLiteral("waffle-iron"),
                                    QStringLiteral("ghostline"), QStringLiteral("hot-dog-stand"),
                                    QStringLiteral("neon-miami")};
    if (!validThemes.contains(m_options.theme)) {
        m_options.theme = QStringLiteral("system");
    }
    m_options.showTimestamps = settings.value(QStringLiteral("ui/showTimestamps"), true).toBool();
    m_options.showSidePanes = settings.value(QStringLiteral("ui/showSidePanes"), true).toBool();
    m_options.encryptedDmEnabled = settings.value(QStringLiteral("security/encryptedDmEnabled"), true).toBool();
    m_options.autoReplySecure = settings.value(QStringLiteral("security/autoReplySecure"), true).toBool();
    m_options.showSecureFingerprints = settings.value(QStringLiteral("security/showSecureFingerprints"), true).toBool();
}

void MainWindow::saveOptions() const
{
    QSettings settings;
    settings.setValue(QStringLiteral("ui/theme"), m_options.theme);
    settings.setValue(QStringLiteral("ui/showTimestamps"), m_options.showTimestamps);
    settings.setValue(QStringLiteral("ui/showSidePanes"), m_options.showSidePanes);
    settings.setValue(QStringLiteral("security/encryptedDmEnabled"), m_options.encryptedDmEnabled);
    settings.setValue(QStringLiteral("security/autoReplySecure"), m_options.autoReplySecure);
    settings.setValue(QStringLiteral("security/showSecureFingerprints"), m_options.showSecureFingerprints);
    settings.sync();
}

void MainWindow::applyTheme()
{
    QString sheet;

    if (m_options.theme == QStringLiteral("hacker")) {
        sheet = QStringLiteral(
            "QWidget { background-color: #020402; color: #39ff14; font-family: 'Monospace'; }"
            "QMainWindow,QDialog { background-color: #020402; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { "
            " background-color: #000000; color: #55ff33; border: 1px solid #168a0f; "
            " selection-background-color: #124d0d; selection-color: #b7ff9f; }"
            "QLineEdit:focus,QPlainTextEdit:focus,QTextEdit:focus,QListWidget:focus,QTreeWidget:focus,QComboBox:focus { "
            " border: 1px solid #39ff14; }"
            "QPushButton { background-color: #071007; color: #39ff14; border: 1px solid #1dbb13; "
            " padding: 5px 10px; min-height: 18px; }"
            "QPushButton:hover { background-color: #0d250b; border-color: #55ff33; }"
            "QPushButton:pressed { background-color: #12360e; }"
            "QPushButton:disabled { color: #286326; border-color: #173c17; background-color: #050905; }"
            "QMenuBar,QMenu,QStatusBar { background-color: #040904; color: #39ff14; }"
            "QMenu::item:selected,QMenuBar::item:selected { background-color: #12360e; }"
            "QHeaderView::section { background-color: #071207; color: #55ff33; border: 1px solid #168a0f; padding: 4px; }"
            "QGroupBox { border: 1px solid #168a0f; margin-top: 8px; padding-top: 6px; }"
            "QGroupBox::title { subcontrol-origin: margin; left: 8px; padding: 0 4px; color: #55ff33; }"
            "QToolTip { background-color: #000000; color: #55ff33; border: 1px solid #39ff14; }"
            "QScrollBar:vertical { background: #020602; width: 12px; margin: 0; }"
            "QScrollBar::handle:vertical { background: #176510; min-height: 24px; border: 1px solid #39ff14; }"
            "QScrollBar::handle:vertical:hover { background: #209116; }"
            "QScrollBar:add-line:vertical,QScrollBar:sub-line:vertical { height: 0; }"
            "QScrollBar:horizontal { background: #020602; height: 12px; margin: 0; }"
            "QScrollBar::handle:horizontal { background: #176510; min-width: 24px; border: 1px solid #39ff14; }"
            "QScrollBar:add-line:horizontal,QScrollBar:sub-line:horizontal { width: 0; }"
            "QSlider::groove:horizontal { height: 6px; background: #0b2609; border: 1px solid #176510; }"
            "QSlider::handle:horizontal { width: 14px; margin: -5px 0; background: #39ff14; border: 1px solid #88ff70; }"
            "QSplitter::handle { background: #168a0f; }"
            "QCheckBox::indicator { width: 14px; height: 14px; border: 1px solid #39ff14; background: #000000; }"
            "QCheckBox::indicator:checked { background: #39ff14; }");
    } else if (m_options.theme == QStringLiteral("matrix")) {
        sheet = QStringLiteral(
            "QWidget { background-color: #000000; color: #00ff41; font-family: 'Monospace'; }"
            "QMainWindow,QDialog { background-color: #000000; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { "
            " background-color: #000000; color: #00ff41; border: 1px solid #008f11; "
            " selection-background-color: #003b0a; selection-color: #b6ffbf; }"
            "QLineEdit:focus,QPlainTextEdit:focus,QTextEdit:focus,QListWidget:focus,QTreeWidget:focus,QComboBox:focus { border: 1px solid #00ff41; }"
            "QPushButton,QMenuBar,QMenu,QStatusBar { background-color: #001600; color: #00ff41; }"
            "QPushButton { border: 1px solid #008f11; padding: 5px 9px; }"
            "QPushButton:hover { background-color: #003b0a; border-color: #00ff41; }"
            "QPushButton:pressed { background-color: #005b12; }"
            "QHeaderView::section { background-color: #001d05; color: #00ff41; border: 1px solid #008f11; padding: 4px; }"
            "QGroupBox { border: 1px solid #008f11; margin-top: 8px; padding-top: 6px; }"
            "QGroupBox::title { subcontrol-origin: margin; left: 8px; padding: 0 4px; color: #00ff41; }"
            "QScrollBar:vertical { background: #000000; width: 12px; }"
            "QScrollBar::handle:vertical { background: #006b10; min-height: 24px; border: 1px solid #00ff41; }"
            "QScrollBar:horizontal { background: #000000; height: 12px; }"
            "QScrollBar::handle:horizontal { background: #006b10; min-width: 24px; border: 1px solid #00ff41; }"
            "QScrollBar:add-line,QScrollBar:sub-line { width: 0; height: 0; }"
            "QSlider::groove:horizontal { height: 6px; background: #003b0a; border: 1px solid #008f11; }"
            "QSlider::handle:horizontal { width: 14px; margin: -5px 0; background: #00ff41; border: 1px solid #b6ffbf; }"
            "QSplitter::handle { background: #008f11; }");
    } else if (m_options.theme == QStringLiteral("phosphor")) {
        sheet = QStringLiteral(
            "QWidget { background: #071007; color: #75ff83; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { "
            " background: #020702; color: #89ff95; border: 1px solid #2b7a34; selection-background-color: #215f29; }"
            "QPushButton,QMenuBar,QMenu,QStatusBar { background: #0b180c; color: #89ff95; }"
            "QPushButton { border: 1px solid #2b7a34; padding: 4px 8px; }"
            "QPushButton:hover { background: #17391b; }"
            "QHeaderView::section { background: #102512; color: #89ff95; border: 1px solid #2b7a34; }"
            "QGroupBox { border: 1px solid #2b7a34; margin-top: 8px; }"
            "QScrollBar::handle { background: #2b7a34; }"
            "QSplitter::handle { background: #2b7a34; }");
    } else if (m_options.theme == QStringLiteral("midnight")) {
        sheet = QStringLiteral(
            "QWidget { background-color: #11141b; color: #e1e7f0; }"
            "QMainWindow,QDialog { background-color: #11141b; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { "
            " background-color: #0a0d12; color: #e7edf7; border: 1px solid #394150; "
            " selection-background-color: #31476d; selection-color: #ffffff; }"
            "QPushButton { background-color: #202631; color: #eef3fb; border: 1px solid #4b5668; padding: 5px 9px; }"
            "QPushButton:hover { background-color: #2a3342; border-color: #72809a; }"
            "QMenuBar,QMenu,QStatusBar { background-color: #171c25; color: #e7edf7; }"
            "QMenu::item:selected,QMenuBar::item:selected { background-color: #2a3b5c; }"
            "QHeaderView::section { background-color: #202631; color: #eef3fb; border: 1px solid #3f4958; padding: 4px; }"
            "QGroupBox { border: 1px solid #3f4958; margin-top: 8px; }"
            "QScrollBar::handle { background: #4b5668; }"
            "QSplitter::handle { background: #3f4958; }");
    } else if (m_options.theme == QStringLiteral("amber")) {
        sheet = QStringLiteral(
            "QWidget { background: #130d03; color: #ffbf47; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { "
            " background: #070401; color: #ffc85c; border: 1px solid #8f5e15; selection-background-color: #6a4510; }"
            "QPushButton,QMenuBar,QMenu,QStatusBar { background: #211604; color: #ffc85c; }"
            "QPushButton { border: 1px solid #8f5e15; padding: 4px 8px; }"
            "QPushButton:hover { background: #3a2709; }"
            "QHeaderView::section { background: #2a1c06; color: #ffc85c; border: 1px solid #8f5e15; }"
            "QGroupBox { border: 1px solid #8f5e15; margin-top: 8px; }"
            "QScrollBar::handle { background: #8f5e15; }"
            "QSplitter::handle { background: #8f5e15; }");
    } else if (m_options.theme == QStringLiteral("ice")) {
        sheet = QStringLiteral(
            "QWidget { background: #071018; color: #bde8ff; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { "
            " background: #02070b; color: #c9edff; border: 1px solid #2e6d91; selection-background-color: #1f526f; }"
            "QPushButton,QMenuBar,QMenu,QStatusBar { background: #0c1b25; color: #c9edff; }"
            "QPushButton { border: 1px solid #2e6d91; padding: 4px 8px; }"
            "QPushButton:hover { background: #153447; }"
            "QHeaderView::section { background: #102b3a; color: #c9edff; border: 1px solid #2e6d91; }"
            "QGroupBox { border: 1px solid #2e6d91; margin-top: 8px; }"
            "QScrollBar::handle { background: #2e6d91; }"
            "QSplitter::handle { background: #2e6d91; }");
    } else if (m_options.theme == QStringLiteral("cyberpunk")) {
        sheet = QStringLiteral(
            "QWidget { background: #070714; color: #dffcff; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { background: #03030a; color: #e7fdff; border: 1px solid #00d9ff; selection-background-color: #8a087b; selection-color: #ffffff; }"
            "QPushButton { background: #17102e; color: #00e5ff; border: 1px solid #ff2bd6; padding: 5px 9px; }"
            "QPushButton:hover { background: #2d1550; color: #ffffff; border-color: #00e5ff; }"
            "QMenuBar,QMenu,QStatusBar { background: #100b22; color: #ff62df; }"
            "QMenu::item:selected,QMenuBar::item:selected { background: #31114b; color: #00e5ff; }"
            "QHeaderView::section { background: #16102b; color: #00e5ff; border: 1px solid #9d1d91; padding: 4px; }"
            "QGroupBox { border: 1px solid #9d1d91; margin-top: 8px; }"
            "QScrollBar::handle,QSplitter::handle { background: #00a9c6; }");
    } else if (m_options.theme == QStringLiteral("synthwave")) {
        sheet = QStringLiteral(
            "QWidget { background: #160b2d; color: #f7ddff; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { background: #0b0618; color: #fff0ff; border: 1px solid #a447d1; selection-background-color: #ff4ecd; selection-color: #160b2d; }"
            "QPushButton { background: #28124a; color: #ff69d8; border: 1px solid #30e7ff; padding: 5px 9px; }"
            "QPushButton:hover { background: #3b1764; border-color: #ff4ecd; color: #ffffff; }"
            "QMenuBar,QMenu,QStatusBar { background: #210e40; color: #30e7ff; }"
            "QMenu::item:selected,QMenuBar::item:selected { background: #4a1765; color: #ff8be5; }"
            "QHeaderView::section { background: #301350; color: #30e7ff; border: 1px solid #a447d1; padding: 4px; }"
            "QGroupBox { border: 1px solid #a447d1; margin-top: 8px; }"
            "QScrollBar::handle,QSplitter::handle { background: #ff4ecd; }");
    } else if (m_options.theme == QStringLiteral("dracula")) {
        sheet = QStringLiteral(
            "QWidget { background: #282a36; color: #f8f8f2; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { background: #1e1f29; color: #f8f8f2; border: 1px solid #6272a4; selection-background-color: #44475a; selection-color: #f8f8f2; }"
            "QPushButton { background: #343746; color: #bd93f9; border: 1px solid #6272a4; padding: 5px 9px; }"
            "QPushButton:hover { background: #44475a; color: #ff79c6; border-color: #bd93f9; }"
            "QMenuBar,QMenu,QStatusBar { background: #21222c; color: #f8f8f2; }"
            "QMenu::item:selected,QMenuBar::item:selected { background: #44475a; color: #50fa7b; }"
            "QHeaderView::section { background: #343746; color: #8be9fd; border: 1px solid #6272a4; padding: 4px; }"
            "QGroupBox { border: 1px solid #6272a4; margin-top: 8px; }"
            "QScrollBar::handle,QSplitter::handle { background: #6272a4; }");
    } else if (m_options.theme == QStringLiteral("vaporwave")) {
        sheet = QStringLiteral(
            "QWidget { background: #241b3a; color: #ffe5f8; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { background: #171127; color: #ffd6f6; border: 1px solid #b967ff; selection-background-color: #ff71ce; selection-color: #27163e; }"
            "QPushButton { background: #382455; color: #01cdfe; border: 1px solid #ff71ce; padding: 5px 9px; }"
            "QPushButton:hover { background: #533074; color: #ffffff; border-color: #01cdfe; }"
            "QMenuBar,QMenu,QStatusBar { background: #302047; color: #ff71ce; }"
            "QMenu::item:selected,QMenuBar::item:selected { background: #5c3377; color: #01cdfe; }"
            "QHeaderView::section { background: #40275e; color: #05ffa1; border: 1px solid #b967ff; padding: 4px; }"
            "QGroupBox { border: 1px solid #b967ff; margin-top: 8px; }"
            "QScrollBar::handle,QSplitter::handle { background: #ff71ce; }");
    } else if (m_options.theme == QStringLiteral("blood-moon")) {
        sheet = QStringLiteral(
            "QWidget { background: #120205; color: #ffd8d8; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { background: #080102; color: #ffe8e8; border: 1px solid #8f1427; selection-background-color: #6c0b1c; selection-color: #ffffff; }"
            "QPushButton { background: #27060c; color: #ff5269; border: 1px solid #b01d34; padding: 5px 9px; }"
            "QPushButton:hover { background: #420812; color: #ffffff; border-color: #ff334f; }"
            "QMenuBar,QMenu,QStatusBar { background: #200409; color: #ff6b7f; }"
            "QMenu::item:selected,QMenuBar::item:selected { background: #500b18; color: #ffffff; }"
            "QHeaderView::section { background: #310710; color: #ff8595; border: 1px solid #8f1427; padding: 4px; }"
            "QGroupBox { border: 1px solid #8f1427; margin-top: 8px; }"
            "QScrollBar::handle,QSplitter::handle { background: #8f1427; }");
    } else if (m_options.theme == QStringLiteral("c64")) {
        sheet = QStringLiteral(
            "QWidget { background: #40318d; color: #b8c3ff; font-family: 'Monospace'; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { background: #352879; color: #d3d9ff; border: 2px solid #7869c4; selection-background-color: #7869c4; selection-color: #ffffff; }"
            "QPushButton { background: #5040a0; color: #d3d9ff; border: 2px solid #7869c4; padding: 5px 9px; }"
            "QPushButton:hover { background: #6655b5; color: #ffffff; }"
            "QMenuBar,QMenu,QStatusBar { background: #352879; color: #b8c3ff; }"
            "QMenu::item:selected,QMenuBar::item:selected { background: #7869c4; color: #ffffff; }"
            "QHeaderView::section { background: #5040a0; color: #ffffff; border: 1px solid #7869c4; padding: 4px; }"
            "QGroupBox { border: 2px solid #7869c4; margin-top: 8px; }"
            "QScrollBar::handle,QSplitter::handle { background: #7869c4; }");
    } else if (m_options.theme == QStringLiteral("dos")) {
        sheet = QStringLiteral(
            "QWidget { background: #000080; color: #ffffff; font-family: 'Monospace'; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { background: #000050; color: #ffffff; border: 1px solid #00ffff; selection-background-color: #00aaaa; selection-color: #000000; }"
            "QPushButton { background: #0000aa; color: #ffff55; border: 1px solid #ffffff; padding: 4px 8px; }"
            "QPushButton:hover { background: #0000cc; color: #ffffff; border-color: #00ffff; }"
            "QMenuBar,QMenu,QStatusBar { background: #000050; color: #ffff55; }"
            "QMenu::item:selected,QMenuBar::item:selected { background: #00aaaa; color: #000000; }"
            "QHeaderView::section { background: #0000aa; color: #ffffff; border: 1px solid #00ffff; padding: 4px; }"
            "QGroupBox { border: 1px solid #00ffff; margin-top: 8px; }"
            "QScrollBar::handle,QSplitter::handle { background: #00aaaa; }");
    } else if (m_options.theme == QStringLiteral("solarized-dark")) {
        sheet = QStringLiteral(
            "QWidget { background: #002b36; color: #839496; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { background: #073642; color: #93a1a1; border: 1px solid #586e75; selection-background-color: #2aa198; selection-color: #002b36; }"
            "QPushButton { background: #073642; color: #b58900; border: 1px solid #586e75; padding: 5px 9px; }"
            "QPushButton:hover { background: #0b4652; color: #2aa198; border-color: #2aa198; }"
            "QMenuBar,QMenu,QStatusBar { background: #073642; color: #93a1a1; }"
            "QMenu::item:selected,QMenuBar::item:selected { background: #2aa198; color: #002b36; }"
            "QHeaderView::section { background: #073642; color: #268bd2; border: 1px solid #586e75; padding: 4px; }"
            "QGroupBox { border: 1px solid #586e75; margin-top: 8px; }"
            "QScrollBar::handle,QSplitter::handle { background: #586e75; }");
    } else if (m_options.theme == QStringLiteral("waffle-iron")) {
        sheet = QStringLiteral(
            "QWidget { background: #1c1206; color: #ffe7a3; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { background: #0f0903; color: #fff0bd; border: 1px solid #9b6726; selection-background-color: #b87823; selection-color: #1c1206; }"
            "QPushButton { background: #3a240b; color: #f4b942; border: 1px solid #b87823; padding: 5px 9px; }"
            "QPushButton:hover { background: #563810; color: #fff1b8; border-color: #ffd36a; }"
            "QMenuBar,QMenu,QStatusBar { background: #2b1b08; color: #f4b942; }"
            "QMenu::item:selected,QMenuBar::item:selected { background: #6f4519; color: #fff1b8; }"
            "QHeaderView::section { background: #42280c; color: #ffd36a; border: 1px solid #9b6726; padding: 4px; }"
            "QGroupBox { border: 1px solid #9b6726; margin-top: 8px; }"
            "QScrollBar::handle,QSplitter::handle { background: #9b6726; }");
    } else if (m_options.theme == QStringLiteral("ghostline")) {
        sheet = QStringLiteral(
            "QWidget { background: #07131b; color: #c6efff; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { background: #030b10; color: #d7f4ff; border: 1px solid #35799a; selection-background-color: #4d5aa8; selection-color: #ffffff; }"
            "QPushButton { background: #0e2431; color: #5fd7ff; border: 1px solid #5576b8; padding: 5px 9px; }"
            "QPushButton:hover { background: #17394b; color: #ffffff; border-color: #9b7bff; }"
            "QMenuBar,QMenu,QStatusBar { background: #0a1d28; color: #7be3ff; }"
            "QMenu::item:selected,QMenuBar::item:selected { background: #38447f; color: #ffffff; }"
            "QHeaderView::section { background: #102b3a; color: #8ddfff; border: 1px solid #5576b8; padding: 4px; }"
            "QGroupBox { border: 1px solid #5576b8; margin-top: 8px; }"
            "QScrollBar::handle,QSplitter::handle { background: #5576b8; }");
    } else if (m_options.theme == QStringLiteral("hot-dog-stand")) {
        sheet = QStringLiteral(
            "QWidget { background: #ff0000; color: #ffff00; font-weight: bold; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { background: #ffff00; color: #000000; border: 2px solid #000000; selection-background-color: #000000; selection-color: #ffff00; }"
            "QPushButton { background: #ffff00; color: #ff0000; border: 2px solid #000000; padding: 5px 9px; }"
            "QPushButton:hover { background: #000000; color: #ffff00; }"
            "QMenuBar,QMenu,QStatusBar { background: #ffff00; color: #ff0000; }"
            "QMenu::item:selected,QMenuBar::item:selected { background: #000000; color: #ffff00; }"
            "QHeaderView::section { background: #000000; color: #ffff00; border: 1px solid #ff0000; padding: 4px; }"
            "QGroupBox { border: 2px solid #ffff00; margin-top: 8px; }"
            "QScrollBar::handle,QSplitter::handle { background: #ffff00; }");
    } else if (m_options.theme == QStringLiteral("neon-miami")) {
        sheet = QStringLiteral(
            "QWidget { background: #071c24; color: #d8fffb; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { background: #041116; color: #e5fffc; border: 1px solid #20e3d2; selection-background-color: #ff4fa3; selection-color: #ffffff; }"
            "QPushButton { background: #0d3038; color: #20e3d2; border: 1px solid #ff4fa3; padding: 5px 9px; }"
            "QPushButton:hover { background: #134650; color: #ffffff; border-color: #20e3d2; }"
            "QMenuBar,QMenu,QStatusBar { background: #092830; color: #ff70b7; }"
            "QMenu::item:selected,QMenuBar::item:selected { background: #5a1f4b; color: #20e3d2; }"
            "QHeaderView::section { background: #0d3640; color: #20e3d2; border: 1px solid #ff4fa3; padding: 4px; }"
            "QGroupBox { border: 1px solid #ff4fa3; margin-top: 8px; }"
            "QScrollBar::handle,QSplitter::handle { background: #20e3d2; }");
    } else if (m_options.theme == QStringLiteral("classic-light")) {
        sheet = QStringLiteral(
            "QWidget { background-color: #f2f2f2; color: #202020; }"
            "QMainWindow,QDialog { background-color: #f2f2f2; }"
            "QLineEdit,QPlainTextEdit,QTextEdit,QListWidget,QTreeWidget,QComboBox,QSpinBox { "
            " background-color: #ffffff; color: #202020; border: 1px solid #9a9a9a; "
            " selection-background-color: #2f6fa7; selection-color: #ffffff; }"
            "QPushButton { background-color: #e5e5e5; color: #202020; border: 1px solid #8b8b8b; padding: 5px 9px; }"
            "QPushButton:hover { background-color: #d8e6f3; }"
            "QMenuBar,QMenu,QStatusBar { background-color: #e8e8e8; color: #202020; }"
            "QMenu::item:selected,QMenuBar::item:selected { background-color: #d2e5f5; color: #101010; }"
            "QHeaderView::section { background-color: #dddddd; color: #202020; border: 1px solid #a0a0a0; padding: 4px; }"
            "QGroupBox { border: 1px solid #a0a0a0; margin-top: 8px; }"
            "QScrollBar::handle { background: #b1b1b1; }"
            "QSplitter::handle { background: #a0a0a0; }");
    }

    qApp->setStyleSheet(sheet);
    applyConversationOptions();
}

void MainWindow::applyConversationOptions()
{
    for (ChatWindow *window : m_windows) {
        if (!window) continue;
        window->setShowTimestamps(m_options.showTimestamps);
        window->setShowSidePane(m_options.showSidePanes);
        updateConversationSecurity(window);
    }
}

void MainWindow::showOptionsDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Options — %1").arg(appDisplayName()));
    dialog.setMinimumWidth(430);
    auto *outer = new QVBoxLayout(&dialog);
    auto *form = new QFormLayout;

    QComboBox theme(&dialog);
    theme.addItem(QStringLiteral("System"), QStringLiteral("system"));
    theme.addItem(QStringLiteral("Hacker"), QStringLiteral("hacker"));
    theme.addItem(QStringLiteral("Matrix"), QStringLiteral("matrix"));
    theme.addItem(QStringLiteral("Phosphor"), QStringLiteral("phosphor"));
    theme.addItem(QStringLiteral("Midnight"), QStringLiteral("midnight"));
    theme.addItem(QStringLiteral("Amber"), QStringLiteral("amber"));
    theme.addItem(QStringLiteral("Ice"), QStringLiteral("ice"));
    theme.addItem(QStringLiteral("Classic Light"), QStringLiteral("classic-light"));
    theme.addItem(QStringLiteral("Cyberpunk"), QStringLiteral("cyberpunk"));
    theme.addItem(QStringLiteral("Synthwave"), QStringLiteral("synthwave"));
    theme.addItem(QStringLiteral("Dracula"), QStringLiteral("dracula"));
    theme.addItem(QStringLiteral("Vaporwave"), QStringLiteral("vaporwave"));
    theme.addItem(QStringLiteral("Blood Moon"), QStringLiteral("blood-moon"));
    theme.addItem(QStringLiteral("C64"), QStringLiteral("c64"));
    theme.addItem(QStringLiteral("DOS"), QStringLiteral("dos"));
    theme.addItem(QStringLiteral("Solarized Dark"), QStringLiteral("solarized-dark"));
    theme.addItem(QStringLiteral("Waffle Iron"), QStringLiteral("waffle-iron"));
    theme.addItem(QStringLiteral("Ghostline"), QStringLiteral("ghostline"));
    theme.addItem(QStringLiteral("Hot Dog Stand"), QStringLiteral("hot-dog-stand"));
    theme.addItem(QStringLiteral("Neon Miami"), QStringLiteral("neon-miami"));
    const int themeIndex = theme.findData(m_options.theme);
    if (themeIndex >= 0) theme.setCurrentIndex(themeIndex);
    form->addRow(QStringLiteral("Theme:"), &theme);

    QCheckBox timestamps(QStringLiteral("Show timestamps in conversations"), &dialog);
    timestamps.setChecked(m_options.showTimestamps);
    form->addRow(QString(), &timestamps);

    QCheckBox sidePanes(QStringLiteral("Show room member side panes"), &dialog);
    sidePanes.setChecked(m_options.showSidePanes);
    form->addRow(QString(), &sidePanes);

    QCheckBox encrypted(QStringLiteral("Enable CPX3-compatible encrypted DMs"), &dialog);
    encrypted.setChecked(m_options.encryptedDmEnabled);
    form->addRow(QString(), &encrypted);

    QCheckBox autoReply(QStringLiteral("Automatically reply to secure handshakes"), &dialog);
    autoReply.setChecked(m_options.autoReplySecure);
    form->addRow(QString(), &autoReply);

    QCheckBox fingerprints(QStringLiteral("Show secure fingerprint notices"), &dialog);
    fingerprints.setChecked(m_options.showSecureFingerprints);
    form->addRow(QString(), &fingerprints);

    QLabel identity(&dialog);
    if (BackendState *state = selectedState(); m_secureReady && state) {
        identity.setText(QStringLiteral("Selected profile fingerprint:\n%1")
                             .arg(m_secure.localFingerprint(state->profileId)));
    } else if (!m_secureReady) {
        identity.setText(QStringLiteral("Encrypted DMs unavailable: %1").arg(m_secureError));
    } else {
        identity.setText(QStringLiteral("Select a connection to view its secure fingerprint."));
    }
    identity.setTextInteractionFlags(Qt::TextSelectableByMouse);
    identity.setWordWrap(true);
    outer->addLayout(form);
    outer->addWidget(&identity);

    QDialogButtonBox buttons(QDialogButtonBox::Cancel | QDialogButtonBox::Save, &dialog);
    outer->addWidget(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    m_options.theme = theme.currentData().toString();
    m_options.showTimestamps = timestamps.isChecked();
    m_options.showSidePanes = sidePanes.isChecked();
    m_options.encryptedDmEnabled = encrypted.isChecked();
    m_options.autoReplySecure = autoReply.isChecked();
    m_options.showSecureFingerprints = fingerprints.isChecked();
    saveOptions();
    applyTheme();
}

void MainWindow::showHelpDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("%1 Help").arg(appDisplayName()));
    dialog.resize(720, 600);
    auto *outer = new QVBoxLayout(&dialog);
    auto *help = new QPlainTextEdit(&dialog);
    help->setReadOnly(true);
    help->setPlainText(QStringLiteral(
        "%1\nVersion %3\n\n"
        "%2 HELP\n\n"
        "CONNECTIONS\n"
        "  Connection > Add... creates AIM/OSCAR, IRC, Telnet/BBS, or SIP/VoIP profiles.\n"
        "  New profiles start with no protocol selected. Secrets are not saved unless you opt in.\n"
        "  Accounts contains one submenu per saved connection for connection-specific actions.\n"
        "  AIM/IRC account menus provide IM / Chatroom and Add / Remove Buddies windows.\n"
        "  Tools contains Show Connections Window, Open Softphone, AIM password, fingerprint, and Options.\n\n"
        "SIP / VOIP SOFTPHONE\n"
        "  SIP accounts are normal saved WaffleHouse connections and multiple accounts may register at once.\n"
        "  The Buddy List Softphone panel selects the outbound SIP identity and provides quick dialing.\n"
        "  Active calls appear under their SIP account; double-click an active call to open Softphone.\n"
        "  SIP contacts are local dial targets; double-click one to load it into quick dial.\n"
        "  Tools > Open Softphone opens the full phone workspace.\n"
        "  Softphone > Profile edits the same saved SIP account as Connection > Edit.\n"
        "  Softphone tabs: Main, Active Call, SIP Log, SIP Ladder, Profile, and Activity.\n\n"
        "TELNET / MUD / BBS\n"
        "  Add a Telnet profile, choose host/port and terminal type, then connect.\n"
        "  A terminal session window opens automatically. Closing it disconnects that Telnet profile.\n"
        "  Telnet is plaintext; credentials and traffic are not encrypted by the Telnet protocol.\n\n"
        "SECURE PRIVATE MESSAGES\n"
        "  Secure DMs interoperate with WaffleHouse-Client, WaffleHouse 1.9.1 family clients, GhostPulse, CrossPoint, and legacy CPX3-compatible clients.\n"
        "  Encryption applies to AIM/OSCAR and IRC private messages only.\n"
        "  It does not encrypt rooms/channels, Telnet traffic, routing metadata, or server-visible endpoints.\n\n"
        "  1. Open an IM with another compatible WaffleHouse/CPX3-compatible user.\n"
        "  2. Choose Security > Start Secure Session.\n"
        "  3. Choose Security > Secure Session Status to view both fingerprints.\n"
        "  4. Compare the peer fingerprint through a separate trusted channel (voice, phone, in person, etc.).\n"
        "  5. Choose Security > Trust Peer Fingerprint after it matches.\n"
        "  6. Type normally. Messages are encrypted automatically while the secure session is active.\n\n"
        "  An unverified secure session is encrypted but not identity-verified.\n"
        "  If a trusted peer later presents a different key, the client rejects that secure session.\n"
        "  Security > Forget Trusted Fingerprint removes saved trust.\n"
        "  Security > Close Secure Session returns that conversation to plaintext.\n\n"
        "SECURE FILE TRANSFER\n"
        "  After a secure session is active and the peer advertises file-transfer capability, choose Security > Send File.\n"
        "  Transfers are encrypted inside CPX, chunked, resumable, and SHA-256 verified before finalizing.\n"
        "  Incoming transfers require explicit acceptance and a destination path.\n\n"
        "SLASH ALIASES INSIDE CONVERSATION WINDOWS\n"
        "  Tab          complete/cycle matching slash commands\n"
        "  Shift+Tab    cycle matching commands backward\n"
        "  /options     open Options\n"
        "  /help        open this Help window\n"
        "  /fingerprint show this connection profile's local secure fingerprint\n"
        "  /secure      start a secure session in an IM\n"
        "  /securestatus show fingerprints/trust state\n"
        "  /trust       trust the active peer fingerprint\n"
        "  /untrust     forget the trusted peer fingerprint\n"
        "  /secureoff   close the secure session\n\n"
        "RUNTIME ENVIRONMENT\n"
        "  %4\n"
        "  Graphical-terminal sessions are distinguished from desktop launches and console-only TTYs.\n\n"
        "THEMES\n"
        "  Use the Buddy List Options button, Tools > Options, or Ctrl+, for full settings.\n"
        "  View > Theme provides quick access to System, Hacker, Matrix, Phosphor, Midnight, Amber, Ice, and Classic Light.\n")
        .arg(appAsciiLogo(), appDisplayName(), appVersionString(), RuntimeEnvironment::detect().summary()));
    outer->addWidget(help, 1);
    QDialogButtonBox close(QDialogButtonBox::Close, &dialog);
    outer->addWidget(&close);
    connect(&close, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(&close, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    dialog.exec();
}

void MainWindow::loadConnections()
{
    if (!m_connectionList) return;

    QSettings settings;
    const int count = settings.beginReadArray(QStringLiteral("connections"));
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        ConnectionSettings value;
        const int protocolValue = settings.value(
            QStringLiteral("protocol"),
            static_cast<int>(ConnectionSettings::Protocol::Unknown)).toInt();
        if (protocolValue == static_cast<int>(ConnectionSettings::Protocol::Oscar))
            value.protocol = ConnectionSettings::Protocol::Oscar;
        else if (protocolValue == static_cast<int>(ConnectionSettings::Protocol::Irc))
            value.protocol = ConnectionSettings::Protocol::Irc;
        else if (protocolValue == static_cast<int>(ConnectionSettings::Protocol::Telnet))
            value.protocol = ConnectionSettings::Protocol::Telnet;
        else if (protocolValue == static_cast<int>(ConnectionSettings::Protocol::Sip))
            value.protocol = ConnectionSettings::Protocol::Sip;
        else
            continue;

        value.server = settings.value(QStringLiteral("server")).toString();
        const int defaultPort = value.protocol == ConnectionSettings::Protocol::Irc ? 6667
            : value.protocol == ConnectionSettings::Protocol::Telnet ? 23
            : value.protocol == ConnectionSettings::Protocol::Sip ? 5060 : 5190;
        value.port = static_cast<quint16>(settings.value(QStringLiteral("port"), defaultPort).toUInt());
        value.username = settings.value(QStringLiteral("username")).toString();
        value.redirectHost = settings.value(QStringLiteral("redirectHost")).toString();
        value.redirectPort = static_cast<quint16>(settings.value(QStringLiteral("redirectPort"), 0).toUInt());
        value.realName = settings.value(QStringLiteral("realName"), appDefaultRealName()).toString();
        value.tls = settings.value(QStringLiteral("tls"), false).toBool();
        value.ircBuddies = settings.value(QStringLiteral("ircBuddies")).toStringList();
        value.sipContacts = settings.value(QStringLiteral("sipContacts")).toStringList();
        value.telnetTerminalType = settings.value(QStringLiteral("telnetTerminalType"), QStringLiteral("ANSI")).toString();
        value.sipProfileName = settings.value(QStringLiteral("sipProfileName")).toString();
        value.sipDomain = settings.value(QStringLiteral("sipDomain"), value.protocol == ConnectionSettings::Protocol::Sip ? value.server : QString()).toString();
        value.sipRegistrar = settings.value(QStringLiteral("sipRegistrar")).toString();
        value.sipAuthUsername = settings.value(QStringLiteral("sipAuthUsername")).toString();
        value.sipDisplayName = settings.value(QStringLiteral("sipDisplayName")).toString();
        value.sipOutboundProxy = settings.value(QStringLiteral("sipOutboundProxy")).toString();
        value.sipCallerIdDomain = settings.value(QStringLiteral("sipCallerIdDomain")).toString();
        value.sipDialPrefix = settings.value(QStringLiteral("sipDialPrefix")).toString();
        value.sipStunServer = settings.value(QStringLiteral("sipStunServer")).toString();
        value.sipTransport = settings.value(QStringLiteral("sipTransport"), QStringLiteral("udp")).toString();
        value.sipIdentityMode = settings.value(QStringLiteral("sipIdentityMode"), QStringLiteral("from")).toString();
        value.sipLocalPort = static_cast<quint16>(settings.value(QStringLiteral("sipLocalPort"), value.protocol == ConnectionSettings::Protocol::Sip ? value.port : 5060).toUInt());
        value.sipRegistrationExpires = settings.value(QStringLiteral("sipRegistrationExpires"), 300).toUInt();
        value.sipUseIce = settings.value(QStringLiteral("sipUseIce"), false).toBool();
        value.sipEnableSrtp = settings.value(QStringLiteral("sipEnableSrtp"), false).toBool();
        value.debug = settings.value(QStringLiteral("debug"), false).toBool();
        value.savePassword = settings.value(QStringLiteral("savePassword"), false).toBool();
        value.password = value.savePassword
            ? settings.value(QStringLiteral("password")).toString()
            : QString();

        const bool defaultSecretRequired = value.protocol == ConnectionSettings::Protocol::Oscar
            || value.protocol == ConnectionSettings::Protocol::Sip;
        const bool secretRequired = settings.value(QStringLiteral("secretRequired"), defaultSecretRequired).toBool();

        QString profileId = settings.value(QStringLiteral("id")).toString().trimmed();
        if (profileId.isEmpty()) {
            const QString seed = QStringLiteral("%1|%2|%3|%4|%5")
                .arg(static_cast<int>(value.protocol))
                .arg(value.server.toCaseFolded())
                .arg(value.port)
                .arg(value.username.toCaseFolded())
                .arg(i);
            profileId = QStringLiteral("migrated-%1").arg(QString::fromLatin1(
                QCryptographicHash::hash(seed.toUtf8(), QCryptographicHash::Sha256)
                    .left(16).toHex()));
        }

        ChatBackend *backend = createBackend(value);
        if (backend) {
            attachBackend(backend, false, secretRequired, !value.password.isEmpty(), false, profileId);
        }
    }
    settings.endArray();

    if (m_connectionList->count() > 0) {
        m_connectionList->setCurrentRow(0);
        saveConnections();
    }
}

void MainWindow::saveConnections() const
{
    if (!m_connectionList) return;
    QSettings settings;
    settings.remove(QStringLiteral("connections"));
    settings.beginWriteArray(QStringLiteral("connections"));

    int outputIndex = 0;
    for (int row = 0; row < m_connectionList->count(); ++row) {
        QListWidgetItem *item = m_connectionList->item(row);
        if (!item) continue;
        BackendState *state = stateById(item->data(Qt::UserRole).toString());
        if (!state || !state->backend) continue;

        const ConnectionSettings &value = state->backend->settings();
        settings.setArrayIndex(outputIndex++);
        settings.setValue(QStringLiteral("id"), state->profileId);
        settings.setValue(QStringLiteral("protocol"), static_cast<int>(value.protocol));
        settings.setValue(QStringLiteral("server"), value.server);
        settings.setValue(QStringLiteral("port"), value.port);
        settings.setValue(QStringLiteral("username"), value.username);
        settings.setValue(QStringLiteral("redirectHost"), value.redirectHost);
        settings.setValue(QStringLiteral("redirectPort"), value.redirectPort);
        settings.setValue(QStringLiteral("realName"), value.realName);
        settings.setValue(QStringLiteral("tls"), value.tls);
        settings.setValue(QStringLiteral("ircBuddies"), value.ircBuddies);
        settings.setValue(QStringLiteral("sipContacts"), value.sipContacts);
        settings.setValue(QStringLiteral("telnetTerminalType"), value.telnetTerminalType);
        settings.setValue(QStringLiteral("sipProfileName"), value.sipProfileName);
        settings.setValue(QStringLiteral("sipDomain"), value.sipDomain);
        settings.setValue(QStringLiteral("sipRegistrar"), value.sipRegistrar);
        settings.setValue(QStringLiteral("sipAuthUsername"), value.sipAuthUsername);
        settings.setValue(QStringLiteral("sipDisplayName"), value.sipDisplayName);
        settings.setValue(QStringLiteral("sipOutboundProxy"), value.sipOutboundProxy);
        settings.setValue(QStringLiteral("sipCallerIdDomain"), value.sipCallerIdDomain);
        settings.setValue(QStringLiteral("sipDialPrefix"), value.sipDialPrefix);
        settings.setValue(QStringLiteral("sipStunServer"), value.sipStunServer);
        settings.setValue(QStringLiteral("sipTransport"), value.sipTransport);
        settings.setValue(QStringLiteral("sipIdentityMode"), value.sipIdentityMode);
        settings.setValue(QStringLiteral("sipLocalPort"), value.sipLocalPort);
        settings.setValue(QStringLiteral("sipRegistrationExpires"), value.sipRegistrationExpires);
        settings.setValue(QStringLiteral("sipUseIce"), value.sipUseIce);
        settings.setValue(QStringLiteral("sipEnableSrtp"), value.sipEnableSrtp);
        settings.setValue(QStringLiteral("debug"), value.debug);
        settings.setValue(QStringLiteral("secretRequired"), state->secretRequired);
        settings.setValue(QStringLiteral("savePassword"), value.savePassword);
        if (value.savePassword && !value.password.isEmpty()) {
            settings.setValue(QStringLiteral("password"), value.password);
        }
    }
    settings.endArray();
    settings.sync();
}

bool MainWindow::ensureConnectionSecret(BackendState *state)
{
    if (!state || !state->backend) {
        return false;
    }
    if (!state->secretRequired || state->hasSessionSecret) {
        return true;
    }

    const auto protocol = state->backend->settings().protocol;
    QString title;
    QString label;
    bool allowEmpty = false;

    switch (protocol) {
    case ConnectionSettings::Protocol::Oscar:
        title = QStringLiteral("AIM / OSCAR Password");
        label = QStringLiteral("Password:");
        break;
    case ConnectionSettings::Protocol::Irc:
        title = QStringLiteral("IRC Server Password");
        label = QStringLiteral("Server password (leave blank if none):");
        allowEmpty = true;
        break;
    case ConnectionSettings::Protocol::Telnet:
        return true;
    case ConnectionSettings::Protocol::Sip:
        title = QStringLiteral("SIP Account Password");
        label = QStringLiteral("SIP password:");
        break;
    case ConnectionSettings::Protocol::Unknown:
        return false;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setModal(true);

    auto *outer = new QVBoxLayout(&dialog);
    outer->setContentsMargins(16, 16, 16, 16);
    outer->setSpacing(10);

    auto *promptLabel = new QLabel(label, &dialog);
    auto *secretEdit = new QLineEdit(&dialog);
    secretEdit->setEchoMode(QLineEdit::Password);
    secretEdit->setMinimumWidth(320);
    auto *savePassword = new QCheckBox(QStringLiteral("Save password on this computer"), &dialog);
    savePassword->setChecked(false);
    savePassword->setToolTip(
        QStringLiteral("Stores this password in the local WaffleHouse-Client settings file. "
                       "The saved value is not encrypted at rest."));

    auto *note = new QLabel(
        QStringLiteral("Saved passwords are stored in your local application settings and "
                       "are not encrypted at rest."),
        &dialog);
    note->setWordWrap(true);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Cancel | QDialogButtonBox::Ok, &dialog);
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    QObject::connect(secretEdit, &QLineEdit::textChanged, savePassword,
                     [savePassword](const QString &text) {
                         savePassword->setEnabled(!text.isEmpty());
                         if (text.isEmpty()) savePassword->setChecked(false);
                     });
    savePassword->setEnabled(false);

    outer->addWidget(promptLabel);
    outer->addWidget(secretEdit);
    outer->addWidget(savePassword);
    outer->addWidget(note);
    outer->addWidget(buttons);
    secretEdit->setFocus();

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    const QString secret = secretEdit->text();
    if (!allowEmpty && secret.isEmpty()) {
        QMessageBox::warning(
            this, title, QStringLiteral("A password is required to connect."));
        return false;
    }

    ConnectionSettings updated = state->backend->settings();
    updated.password = secret;
    updated.savePassword = savePassword->isChecked() && !secret.isEmpty();
    state->backend->setConnectionSettings(updated);
    state->hasSessionSecret = true;
    saveConnections();
    return true;
}

ChatBackend *MainWindow::createBackend(const ConnectionSettings &settings)
{
    switch (settings.protocol) {
    case ConnectionSettings::Protocol::Oscar:
        return new OscarBackend(settings, this);
    case ConnectionSettings::Protocol::Irc:
        return new IrcBackend(settings, this);
    case ConnectionSettings::Protocol::Telnet:
        return new TelnetBackend(settings, this);
    case ConnectionSettings::Protocol::Sip:
        return new SipBackend(settings, m_sipController, this);
    case ConnectionSettings::Protocol::Unknown:
        break;
    }
    return nullptr;
}

void MainWindow::wireBackend(ChatBackend *backend)
{
    if (!backend) {
        return;
    }

    connect(backend, &ChatBackend::connected, this,
            [this, backend](const QString &identity, const QString &endpoint) {
                handleConnected(backend, identity, endpoint);
            }, Qt::QueuedConnection);
    connect(backend, &ChatBackend::disconnected, this,
            [this, backend](const QString &reason) {
                handleDisconnected(backend, reason);
            }, Qt::QueuedConnection);
    connect(backend, &ChatBackend::eventReceived, this,
            [this, backend](const QString &kind, const QString &target, const QString &text) {
                handleEvent(backend, kind, target, text);
            }, Qt::QueuedConnection);
    connect(backend, &ChatBackend::membersChanged, this,
            [this, backend](const QString &room, const QString &action,
                            const QStringList &names) {
                handleMembers(backend, room, action, names);
            }, Qt::QueuedConnection);
    connect(backend, &ChatBackend::targetNamed, this,
            [this, backend](const QString &kind, const QString &target,
                            const QString &name) {
                handleTargetNamed(backend, kind, target, name);
            }, Qt::QueuedConnection);
    connect(backend, &ChatBackend::roomDiscovered, this,
            [this, backend](const QString &roomId, const QString &name) {
                handleRoomDiscovered(backend, roomId, name);
            }, Qt::QueuedConnection);
    connect(backend, &ChatBackend::buddyListChanged, this,
            [this, backend](const QStringList &names) {
                handleBuddyList(backend, names);
            }, Qt::QueuedConnection);
    connect(backend, &ChatBackend::buddyPresenceChanged, this,
            [this, backend](const QString &name, bool online) {
                handleBuddyPresence(backend, name, online);
            }, Qt::QueuedConnection);
    connect(backend, &ChatBackend::backendError, this,
            [this, backend](const QString &context, const QString &message) {
                handleBackendError(backend, context, message);
            }, Qt::QueuedConnection);
}

void MainWindow::attachBackend(ChatBackend *backend,
                               bool persist,
                               bool secretRequired,
                               bool hasSessionSecret,
                               bool autoConnect,
                               const QString &profileId)
{
    if (!backend || !m_connectionList) {
        return;
    }

    auto *state = new BackendState;
    state->backend = backend;
    state->profileId = profileId.trimmed().isEmpty()
        ? QUuid::createUuid().toString(QUuid::WithoutBraces)
        : profileId.trimmed();
    state->secretRequired = secretRequired;
    state->hasSessionSecret = hasSessionSecret;
    state->connectionItem = new QListWidgetItem(m_connectionList);
    state->connectionItem->setData(Qt::UserRole, backend->id());
    m_states.insert(backend->id(), state);
    if (backend->settings().protocol == ConnectionSettings::Protocol::Irc) {
        for (const QString &buddy : backend->settings().ircBuddies) {
            if (!buddy.trimmed().isEmpty()) state->buddies.insert(buddy.trimmed());
        }
    }

    // Wire and attach the backend to WaffleHouse state before a SIP account is
    // inserted into the live PJSUA2 endpoint. This prevents controller signals
    // from re-entering GUI refresh code while a new SipBackend is still under
    // construction (the 2.5 Add-SIP crash).
    wireBackend(backend);
    bool backendReady = true;
    if (auto *sip = qobject_cast<SipBackend *>(backend)) {
        for (const QString &contact : backend->settings().sipContacts) {
            if (!contact.trimmed().isEmpty()) state->buddies.insert(contact.trimmed());
        }
        QString sipError;
        backendReady = sip->initializeAccount(&sipError);
        if (!backendReady) {
            appendActivity(backend, QStringLiteral("SIP account initialization failed: %1").arg(sipError));
            QMessageBox::warning(this, QStringLiteral("SIP Account"),
                                 QStringLiteral("The SIP account was saved, but the softphone endpoint could not add it:\n\n%1").arg(sipError));
        }
    }
    updateConnectionItem(state);
    selectState(state);

    appendActivity(
        backend,
        persist
            ? QStringLiteral("Connection added.")
            : QStringLiteral("Saved connection restored."));

    refreshBuddyList();
    updateActions();

    if (persist) {
        saveConnections();
    }

    if (autoConnect && backendReady) {
        connectState(state);
    }
}

MainWindow::BackendState *MainWindow::stateFor(ChatBackend *backend) const
{
    return backend ? m_states.value(backend->id(), nullptr) : nullptr;
}

MainWindow::BackendState *MainWindow::stateById(const QString &id) const
{
    return m_states.value(id, nullptr);
}

MainWindow::BackendState *MainWindow::stateFromBuddyItem(QTreeWidgetItem *item) const
{
    if (!item) {
        return nullptr;
    }
    return stateById(item->data(0, Qt::UserRole).toString());
}

MainWindow::BackendState *MainWindow::selectedState() const
{
    if (m_buddyTree) {
        if (BackendState *state = stateFromBuddyItem(m_buddyTree->currentItem())) {
            return state;
        }
    }
    QListWidgetItem *item = m_connectionList ? m_connectionList->currentItem() : nullptr;
    return item ? stateById(item->data(Qt::UserRole).toString()) : nullptr;
}

void MainWindow::selectState(BackendState *state)
{
    if (!state || !state->backend) {
        return;
    }

    if (m_connectionList && state->connectionItem
        && m_connectionList->currentItem() != state->connectionItem) {
        m_connectionList->setCurrentItem(state->connectionItem);
    }

    if (m_buddyTree) {
        for (int i = 0; i < m_buddyTree->topLevelItemCount(); ++i) {
            QTreeWidgetItem *root = m_buddyTree->topLevelItem(i);
            if (root && root->data(0, Qt::UserRole).toString() == state->backend->id()) {
                if (!m_buddyTree->currentItem()
                    || stateFromBuddyItem(m_buddyTree->currentItem()) != state) {
                    m_buddyTree->setCurrentItem(root);
                }
                break;
            }
        }
    }
}

void MainWindow::updateConnectionItem(BackendState *state)
{
    if (!state || !state->backend || !state->connectionItem) {
        return;
    }

    const QString identity = state->identity.isEmpty()
        ? state->backend->settings().username
        : state->identity;
    const QString stateWord = state->connecting
        ? QStringLiteral("Connecting")
        : statusWord(state->connected);

    QString text = QStringLiteral("%1 — %2").arg(stateWord, state->backend->protocolName());
    if (!identity.isEmpty()) {
        text += QStringLiteral(" — %1").arg(identity);
    }
    state->connectionItem->setText(text);
    state->connectionItem->setToolTip(state->endpoint);
}

void MainWindow::updateActions()
{
    BackendState *state = selectedState();
    const bool exists = state && state->backend;
    const bool online = exists && state->connected;
    const bool connecting = exists && state->connecting;
    const bool editable = exists && !online && !connecting;
    const bool oscar = online
        && state->backend->settings().protocol == ConnectionSettings::Protocol::Oscar;
    const bool isTelnet = exists
        && state->backend->settings().protocol == ConnectionSettings::Protocol::Telnet;
    const bool isSip = exists
        && state->backend->settings().protocol == ConnectionSettings::Protocol::Sip;
    const bool privateMessaging = online && !isTelnet && !isSip;

    if (m_editConnectionAction) m_editConnectionAction->setEnabled(editable);
    if (m_deleteConnectionAction) m_deleteConnectionAction->setEnabled(exists);
    if (m_connectAction) m_connectAction->setEnabled(exists && !online && !connecting);
    if (m_disconnectAction) m_disconnectAction->setEnabled(exists && (online || connecting));

    if (m_editConnectionButton) m_editConnectionButton->setEnabled(editable);
    if (m_deleteConnectionButton) m_deleteConnectionButton->setEnabled(exists);
    if (m_connectButton) m_connectButton->setEnabled(exists && !online && !connecting);
    if (m_disconnectButton) m_disconnectButton->setEnabled(exists && (online || connecting));
    if (m_buddyConnectButton) m_buddyConnectButton->setEnabled(exists && !online && !connecting);
    if (m_buddyDisconnectButton) m_buddyDisconnectButton->setEnabled(exists && (online || connecting));

    if (m_newImButton) m_newImButton->setEnabled(privateMessaging);
    if (m_rawAction) m_rawAction->setEnabled(online && !isSip);
    if (m_changePasswordAction) m_changePasswordAction->setEnabled(oscar);
    if (m_fingerprintAction) m_fingerprintAction->setEnabled(exists && m_secureReady && !isTelnet && !isSip);


    if (m_trayConnectAction) {
        m_trayConnectAction->setEnabled(exists && !online && !connecting);
    }
    if (m_trayDisconnectAction) {
        m_trayDisconnectAction->setEnabled(exists && (online || connecting));
    }

    rebuildTrayMenu();
}

void MainWindow::refreshBuddyList()
{
    if (!m_buddyTree) {
        return;
    }

    QString selectedBackendId;
    QString selectedBuddy;
    if (QTreeWidgetItem *current = m_buddyTree->currentItem()) {
        selectedBackendId = current->data(0, Qt::UserRole).toString();
        selectedBuddy = current->data(0, Qt::UserRole + 1).toString();
    }

    m_buddyTree->clear();

    QList<BackendState *> states = m_states.values();
    std::sort(states.begin(), states.end(), [](BackendState *a, BackendState *b) {
        if (!a || !a->backend) return false;
        if (!b || !b->backend) return true;
        const int protocolCompare = a->backend->protocolName().compare(
            b->backend->protocolName(), Qt::CaseInsensitive);
        if (protocolCompare != 0) {
            return protocolCompare < 0;
        }
        const QString aName = a->identity.isEmpty() ? a->backend->settings().username : a->identity;
        const QString bName = b->identity.isEmpty() ? b->backend->settings().username : b->identity;
        return aName.compare(bName, Qt::CaseInsensitive) < 0;
    });

    QTreeWidgetItem *restoreItem = nullptr;

    for (BackendState *state : states) {
        if (!state || !state->backend) {
            continue;
        }

        QString accountName = state->identity.isEmpty()
            ? state->backend->settings().username
            : state->identity;
        if (accountName.isEmpty()) {
            accountName = state->backend->settings().protocol == ConnectionSettings::Protocol::Telnet
                ? state->backend->settings().server
                : state->backend->protocolName();
        }

        auto *root = new QTreeWidgetItem(m_buddyTree);
        root->setText(0, accountName);
        root->setText(
            1,
            QStringLiteral("%1 — %2")
                .arg(state->backend->protocolName(),
                     state->connecting ? QStringLiteral("Connecting")
                                       : statusWord(state->connected)));
        root->setData(0, Qt::UserRole, state->backend->id());
        QFont rootFont = root->font(0);
        rootFont.setBold(true);
        root->setFont(0, rootFont);

        if (state->backend->settings().protocol == ConnectionSettings::Protocol::Oscar
            || state->backend->settings().protocol == ConnectionSettings::Protocol::Irc
            || state->backend->settings().protocol == ConnectionSettings::Protocol::Sip) {
            const bool sipContacts =
                state->backend->settings().protocol == ConnectionSettings::Protocol::Sip;
            QStringList buddies = state->buddies.values();
            std::sort(buddies.begin(), buddies.end(), [state](const QString &a, const QString &b) {
                const bool aOnline = state->onlineBuddies.contains(a.toCaseFolded());
                const bool bOnline = state->onlineBuddies.contains(b.toCaseFolded());
                if (aOnline != bOnline) {
                    return aOnline;
                }
                return a.compare(b, Qt::CaseInsensitive) < 0;
            });

            for (const QString &buddy : buddies) {
                auto *item = new QTreeWidgetItem(root);
                item->setText(0, buddy);
                const bool online = state->onlineBuddies.contains(buddy.toCaseFolded());
                item->setText(1, sipContacts
                                     ? QStringLiteral("Contact")
                                     : (online ? QStringLiteral("Online") : QStringLiteral("Offline")));
                item->setData(0, Qt::UserRole, state->backend->id());
                item->setData(0, Qt::UserRole + 1, buddy);
                if (!sipContacts && online) {
                    QFont font = item->font(0);
                    font.setBold(true);
                    item->setFont(0, font);
                }

                if (state->backend->id() == selectedBackendId
                    && buddy == selectedBuddy) {
                    restoreItem = item;
                }
            }
        }

        if (state->backend->settings().protocol == ConnectionSettings::Protocol::Sip) {
            const auto calls = m_sipController->calls();
            for (const auto &call : calls) {
                if (QString::fromStdString(call.accountId) != state->backend->id() || call.disconnected) {
                    continue;
                }
                auto *item = new QTreeWidgetItem(root);
                item->setText(0, QStringLiteral("Call #%1 — %2")
                                     .arg(call.id)
                                     .arg(QString::fromStdString(call.remoteUri)));
                item->setText(1, QString::fromStdString(call.state));
                item->setData(0, Qt::UserRole, state->backend->id());
                item->setData(0, Qt::UserRole + 2, call.id);
                QFont font = item->font(0);
                font.setBold(true);
                item->setFont(0, font);
            }
        }

        root->setExpanded(true);
        if (!restoreItem && state->backend->id() == selectedBackendId
            && selectedBuddy.isEmpty()) {
            restoreItem = root;
        }
    }

    m_buddyTree->resizeColumnToContents(0);
    if (restoreItem) {
        m_buddyTree->setCurrentItem(restoreItem);
    } else if (m_buddyTree->topLevelItemCount() > 0 && !m_buddyTree->currentItem()) {
        m_buddyTree->setCurrentItem(m_buddyTree->topLevelItem(0));
    }
    refreshSoftphoneControls();
}

void MainWindow::refreshSoftphoneControls()
{
    if (!m_buddySipAccount) return;
    const QString previous = m_buddySipAccount->currentData().toString().isEmpty()
        ? m_sipController->selectedAccountId() : m_buddySipAccount->currentData().toString();
    const QSignalBlocker blocker(m_buddySipAccount);
    m_buddySipAccount->clear();
    QList<BackendState *> states = m_states.values();
    std::sort(states.begin(), states.end(), [](BackendState *a, BackendState *b) {
        if (!a || !a->backend) {
            return false;
        }
        if (!b || !b->backend) {
            return true;
        }
        return a->backend->settings().sipProfileName.compare(b->backend->settings().sipProfileName, Qt::CaseInsensitive) < 0;
    });
    for (BackendState *state : states) {
        if (!state || !state->backend || state->backend->settings().protocol != ConnectionSettings::Protocol::Sip) continue;
        const auto &cfg = state->backend->settings();
        const QString label = cfg.sipProfileName.trimmed().isEmpty()
            ? QStringLiteral("%1@%2").arg(cfg.username, cfg.sipDomain.isEmpty() ? cfg.server : cfg.sipDomain)
            : cfg.sipProfileName;
        const QString reg = m_sipController->registrationText(state->backend->id());
        m_buddySipAccount->addItem(QStringLiteral("%1 — %2").arg(label, reg), state->backend->id());
    }
    int idx = m_buddySipAccount->findData(previous);
    if (idx < 0 && m_buddySipAccount->count() > 0) idx = 0;
    if (idx >= 0) {
        m_buddySipAccount->setCurrentIndex(idx);
        m_sipController->setSelectedAccountId(m_buddySipAccount->currentData().toString());
    }
    const bool haveSip = m_buddySipAccount->count() > 0;
    const QString selectedSipId=haveSip?m_buddySipAccount->currentData().toString():QString();
    if(m_buddyDialPrefix){
        const QSignalBlocker prefixBlocker(m_buddyDialPrefix);
        m_buddyDialPrefix->setEnabled(haveSip);
        m_buddyDialPrefix->setText(haveSip?m_sipController->dialPrefix(selectedSipId):QString());
    }
    BackendState *selectedSip = haveSip ? stateById(selectedSipId) : nullptr;
    const bool sipOnline = selectedSip && selectedSip->connected;
    const bool sipConnecting = selectedSip && selectedSip->connecting;
    if (m_buddyDial) m_buddyDial->setEnabled(haveSip);
    if (m_buddyDialButton) m_buddyDialButton->setEnabled(haveSip);
    if (m_buddySipConnectButton) m_buddySipConnectButton->setEnabled(selectedSip && !sipOnline && !sipConnecting);
    if (m_buddySipDisconnectButton) m_buddySipDisconnectButton->setEnabled(selectedSip && (sipOnline || sipConnecting));
}

void MainWindow::appendActivity(ChatBackend *backend, const QString &text)
{
    if (!m_activity) {
        return;
    }
    const QString timestamp = QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss"));
    const QString label = backend ? backend->protocolName() : appDisplayName();
    m_activity->appendPlainText(
        QStringLiteral("[%1] [%2] %3").arg(timestamp, label, text));
}

void MainWindow::rebuildTrayMenu()
{
    if (!m_trayIcon) {
        return;
    }

    int onlineCount = 0;
    int totalCount = 0;
    for (BackendState *state : m_states) {
        if (!state || !state->backend) {
            continue;
        }
        ++totalCount;
        if (state->connected) {
            ++onlineCount;
        }
    }

    m_trayIcon->setToolTip(
        QStringLiteral("%1 — %2/%3 connection(s) online")
            .arg(appDisplayName())
            .arg(onlineCount)
            .arg(totalCount));
}

QString MainWindow::conversationKey(ChatBackend *backend,
                                    const QString &kind,
                                    const QString &target) const
{
    return QStringLiteral("%1|%2|%3")
        .arg(backend ? backend->id() : QStringLiteral("none"),
             kind,
             target.toCaseFolded());
}

QString MainWindow::targetDisplayName(BackendState *state,
                                      const QString &kind,
                                      const QString &target) const
{
    if (!state) {
        return target;
    }
    return state->targetNames.value(QStringLiteral("%1|%2").arg(kind, target), target);
}

QString MainWindow::conversationOpacityKey(ChatBackend *backend,
                                           const QString &kind,
                                           const QString &target) const
{
    if (!backend) {
        return QStringLiteral("ui/conversations/default/opacity");
    }
    const ConnectionSettings &s = backend->settings();
    const QString identity = QStringLiteral("%1|%2|%3|%4|%5|%6")
        .arg(static_cast<int>(s.protocol))
        .arg(s.server)
        .arg(s.port)
        .arg(s.username)
        .arg(kind)
        .arg(target.toCaseFolded());
    const QByteArray hash = QCryptographicHash::hash(
        identity.toUtf8(), QCryptographicHash::Sha1).toHex();
    return QStringLiteral("ui/conversations/%1/opacity")
        .arg(QString::fromLatin1(hash));
}

ChatWindow *MainWindow::ensureConversationWindow(ChatBackend *backend,
                                                 const QString &kind,
                                                 const QString &target,
                                                 bool showWindow)
{
    if (!backend || target.isEmpty()) {
        return nullptr;
    }

    BackendState *state = stateFor(backend);
    if (!state || !state->connected) {
        return nullptr;
    }

    const QString key = conversationKey(backend, kind, target);
    if (ChatWindow *existing = m_windows.value(key, nullptr)) {
        if (showWindow) {
            existing->show();
            existing->raise();
            existing->activateWindow();
        }
        return existing;
    }

    auto *window = new ChatWindow(
        backend,
        kind,
        target,
        targetDisplayName(state, kind, target),
        conversationOpacityKey(backend, kind, target));
    window->setBackendOnline(true);
    window->setShowTimestamps(m_options.showTimestamps);
    window->setShowSidePane(m_options.showSidePanes);
    m_windows.insert(key, window);

    connect(window, &ChatWindow::conversationClosing,
            this, &MainWindow::handleConversationClosing);
    connect(window, &ChatWindow::messageSubmitted,
            this, &MainWindow::handleConversationMessage);
    connect(window, &ChatWindow::terminalBytesSubmitted, this,
            [this](ChatWindow *w, const QByteArray &bytes) {
                if (!w) return;
                BackendState *state = stateById(w->backendId());
                if (state && state->backend && state->connected
                    && state->backend->settings().protocol == ConnectionSettings::Protocol::Telnet) {
                    state->backend->sendTerminalInput(bytes);
                }
            });
    connect(window, &ChatWindow::secureRequested,
            this, &MainWindow::startSecureSession);
    connect(window, &ChatWindow::secureStatusRequested,
            this, &MainWindow::showSecureStatus);
    connect(window, &ChatWindow::trustRequested,
            this, &MainWindow::trustSecurePeer);
    connect(window, &ChatWindow::untrustRequested,
            this, &MainWindow::untrustSecurePeer);
    connect(window, &ChatWindow::secureOffRequested,
            this, &MainWindow::closeSecureSession);
    connect(window, &ChatWindow::fileSendRequested,
            this, &MainWindow::sendFile);
    connect(window, &QObject::destroyed, this, [this, key] {
        m_windows.remove(key);
    });
    updateConversationSecurity(window);

    if (showWindow) {
        window->show();
    }
    return window;
}

void MainWindow::closeBackendWindows(ChatBackend *backend)
{
    if (!backend) {
        return;
    }
    const QString backendId = backend->id();
    const auto windows = m_windows.values();
    for (ChatWindow *window : windows) {
        if (window && window->backendId() == backendId) {
            window->setBackendOnline(false);
            window->close();
        }
    }
}

void MainWindow::connectState(BackendState *state)
{
    if (!state || !state->backend || state->connected || state->connecting) {
        return;
    }
    if (!ensureConnectionSecret(state)) {
        return;
    }

    state->connecting = true;
    updateConnectionItem(state);
    refreshBuddyList();
    updateActions();
    appendActivity(state->backend, QStringLiteral("Connecting…"));
    state->backend->start();
}

void MainWindow::connectSelected()
{
    connectState(selectedState());
}

void MainWindow::disconnectSelected()
{
    BackendState *state = selectedState();
    if (!state || !state->backend || (!state->connected && !state->connecting)) {
        return;
    }

    appendActivity(state->backend, QStringLiteral("Disconnecting…"));
    state->connected = false;
    state->connecting = false;
    state->onlineBuddies.clear();
    m_secure.closeConnection(state->profileId);
    closeBackendWindows(state->backend);
    updateConnectionItem(state);
    refreshBuddyList();
    updateActions();
    state->backend->stop();
}

void MainWindow::deleteSelected()
{
    BackendState *state = selectedState();
    if (!state || !state->backend) {
        return;
    }
    if (state->backend->settings().protocol == ConnectionSettings::Protocol::Sip) {
        for (const auto &call : m_sipController->calls()) {
            if (!call.disconnected && QString::fromStdString(call.accountId) == state->backend->id()) {
                QMessageBox::information(this, QStringLiteral("SIP Account In Use"),
                                         QStringLiteral("Hang up active calls on this SIP account before deleting it."));
                return;
            }
        }
    }

    const QString identity = state->identity.isEmpty()
        ? state->backend->settings().username
        : state->identity;
    const QString description = identity.isEmpty()
        ? state->backend->protocolName()
        : QStringLiteral("%1 — %2").arg(state->backend->protocolName(), identity);

    if (QMessageBox::question(
            this,
            QStringLiteral("Delete Connection"),
            QStringLiteral("Delete the saved connection for %1?")
                .arg(description)) != QMessageBox::Yes) {
        return;
    }

    ChatBackend *backend = state->backend;
    const QString backendId = backend->id();
    state->connected = false;
    state->connecting = false;
    m_secure.closeConnection(state->profileId);
    closeBackendWindows(backend);
    QObject::disconnect(backend, nullptr, this, nullptr);
    backend->stop();

    if (m_connectionList && state->connectionItem) {
        const int row = m_connectionList->row(state->connectionItem);
        if (row >= 0) {
            delete m_connectionList->takeItem(row);
        }
    }

    m_states.remove(backendId);
    state->connectionItem = nullptr;
    delete state;
    backend->deleteLater();

    refreshBuddyList();
    updateActions();
    saveConnections();
    statusBar()->showMessage(QStringLiteral("Connection deleted"), 3000);
}

void MainWindow::editSelected()
{
    BackendState *state = selectedState();
    if (!state || !state->backend) {
        return;
    }
    openConnectionDialog(state->backend->settings(), state);
}

void MainWindow::newIm()
{
    BackendState *state = selectedState();
    if (!state || !state->backend) return;

    QString preset;
    if (QTreeWidgetItem *item = m_buddyTree ? m_buddyTree->currentItem() : nullptr) {
        if (item->parent()) preset = item->data(0, Qt::UserRole + 1).toString().trimmed();
    }
    openMessagingDialog(state, preset, false);
}

void MainWindow::joinRoom()
{
    BackendState *state = selectedState();
    if (!state || !state->backend) return;
    openMessagingDialog(state, QString(), true);
}

void MainWindow::addBuddy()
{
    BackendState *state = selectedState();
    if (!state || !state->connected || !state->backend) return;
    const auto protocol = state->backend->settings().protocol;
    if (protocol != ConnectionSettings::Protocol::Oscar
        && protocol != ConnectionSettings::Protocol::Irc) return;

    const bool ircBuddy = protocol == ConnectionSettings::Protocol::Irc;
    bool ok = false;
    const QString buddy = QInputDialog::getText(
        this,
        ircBuddy ? QStringLiteral("Add IRC Buddy / Watch") : QStringLiteral("Add AIM Buddy"),
        ircBuddy ? QStringLiteral("Nickname to watch:") : QStringLiteral("Screen name:"),
        QLineEdit::Normal,
        QString(),
        &ok).trimmed();
    if (ok && !buddy.isEmpty()) {
        state->backend->addBuddy(buddy);
    }
}

void MainWindow::removeBuddy()
{
    QTreeWidgetItem *item = m_buddyTree ? m_buddyTree->currentItem() : nullptr;
    if (!item || !item->parent()) {
        return;
    }

    BackendState *state = stateFromBuddyItem(item);
    const QString buddy = item->data(0, Qt::UserRole + 1).toString();
    if (!state || !state->connected || !state->backend || buddy.isEmpty()) {
        return;
    }

    const bool ircBuddy = state->backend->settings().protocol == ConnectionSettings::Protocol::Irc;
    if (QMessageBox::question(
            this,
            QStringLiteral("Remove Buddy"),
            ircBuddy
                ? QStringLiteral("Remove %1 from this IRC profile's local buddy/watch list?").arg(buddy)
                : QStringLiteral("Remove %1 from the AIM buddy list?").arg(buddy))
        == QMessageBox::Yes) {
        state->backend->removeBuddy(buddy);
    }
}

void MainWindow::changePassword()
{
    BackendState *state = selectedState();
    if (!state || !state->connected || !state->backend
        || state->backend->settings().protocol != ConnectionSettings::Protocol::Oscar) {
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Change AIM Password"));
    auto *outer = new QVBoxLayout(&dialog);
    auto *form = new QFormLayout;
    QLineEdit current(&dialog);
    QLineEdit next(&dialog);
    QLineEdit confirm(&dialog);
    current.setEchoMode(QLineEdit::Password);
    next.setEchoMode(QLineEdit::Password);
    confirm.setEchoMode(QLineEdit::Password);
    form->addRow(QStringLiteral("Current password:"), &current);
    form->addRow(QStringLiteral("New password:"), &next);
    form->addRow(QStringLiteral("Confirm new password:"), &confirm);
    outer->addLayout(form);
    QDialogButtonBox buttons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, &dialog);
    outer->addWidget(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        if (next.text().isEmpty() || next.text() != confirm.text()) {
            QMessageBox::warning(
                this,
                QStringLiteral("Password change"),
                QStringLiteral("The new passwords are empty or do not match."));
            return;
        }
        state->backend->changePassword(current.text(), next.text());
    }
}

void MainWindow::changeIrcNick()
{
    BackendState *state = selectedState();
    if (!state || !state->connected || !state->backend
        || state->backend->settings().protocol != ConnectionSettings::Protocol::Irc) {
        return;
    }

    bool ok = false;
    const QString nick = QInputDialog::getText(
        this,
        QStringLiteral("Change IRC Nickname"),
        QStringLiteral("New nickname:"),
        QLineEdit::Normal,
        state->identity,
        &ok).trimmed();
    if (ok && !nick.isEmpty()) {
        state->backend->changeNickname(nick);
    }
}

void MainWindow::rawProtocolCommand()
{
    BackendState *state = selectedState();
    if (!state || !state->connected || !state->backend) {
        return;
    }

    const auto protocol = state->backend->settings().protocol;
    if (protocol == ConnectionSettings::Protocol::Irc) {
        bool ok = false;
        const QString line = QInputDialog::getText(
            this,
            QStringLiteral("Raw IRC Command"),
            QStringLiteral("IRC line:"),
            QLineEdit::Normal,
            QString(),
            &ok).trimmed();
        if (ok && !line.isEmpty()) {
            state->backend->sendRaw(line);
        }
        return;
    }

    if (protocol == ConnectionSettings::Protocol::Telnet) {
        bool ok = false;
        const QString line = QInputDialog::getText(
            this,
            QStringLiteral("Raw Telnet Line"),
            QStringLiteral("Line to send:"),
            QLineEdit::Normal,
            QString(),
            &ok);
        if (ok) {
            state->backend->sendRaw(line);
        }
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(QStringLiteral("Raw OSCAR SNAC"));
    auto *outer = new QVBoxLayout(&dialog);
    auto *form = new QFormLayout;
    QLineEdit family(QStringLiteral("0x01"), &dialog);
    QLineEdit subtype(QStringLiteral("0x16"), &dialog);
    QLineEdit body(&dialog);
    body.setPlaceholderText(QStringLiteral("hex body, optional"));
    form->addRow(QStringLiteral("Family:"), &family);
    form->addRow(QStringLiteral("Subtype:"), &subtype);
    form->addRow(QStringLiteral("Hex body:"), &body);
    outer->addLayout(form);
    QDialogButtonBox buttons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, &dialog);
    outer->addWidget(&buttons);
    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        state->backend->sendRaw(family.text(), subtype.text(), body.text());
    }
}

void MainWindow::setBuddyTransparency()
{
    bool ok = false;
    const int current = static_cast<int>(m_buddyOpacity * 100.0 + 0.5);
    const int percent = QInputDialog::getInt(
        this,
        QStringLiteral("Buddy List Transparency"),
        QStringLiteral("Window opacity (%):"),
        current,
        30,
        100,
        5,
        &ok);
    if (!ok) {
        return;
    }
    m_buddyOpacity = static_cast<double>(percent) / 100.0;
    setWindowOpacity(m_buddyOpacity);
    saveUiSettings();
}

void MainWindow::setConnectionsTransparency()
{
    if (!m_connectionsWindow) {
        return;
    }
    bool ok = false;
    const int current = static_cast<int>(m_connectionsOpacity * 100.0 + 0.5);
    const int percent = QInputDialog::getInt(
        this,
        QStringLiteral("Connections Window Transparency"),
        QStringLiteral("Window opacity (%):"),
        current,
        30,
        100,
        5,
        &ok);
    if (!ok) {
        return;
    }
    m_connectionsOpacity = static_cast<double>(percent) / 100.0;
    m_connectionsWindow->setWindowOpacity(m_connectionsOpacity);
    saveUiSettings();
}

void MainWindow::showConnectionsWindow()
{
    if (!m_connectionsWindow) {
        return;
    }
    m_connectionsWindow->show();
    m_connectionsWindow->raise();
    m_connectionsWindow->activateWindow();
}

void MainWindow::showBuddyWindow()
{
    show();
    raise();
    activateWindow();
}

void MainWindow::quitApplication()
{
    if (m_quitting) {
        return;
    }
    m_quitting = true;
    saveConnections();
    saveUiSettings();

    const auto windows = m_windows.values();
    for (ChatWindow *window : windows) {
        if (window) {
            window->close();
        }
    }

    if (m_connectionsWindow) {
        m_connectionsWindow->close();
    }

    for (BackendState *state : m_states) {
        if (state && state->backend) {
            state->connected = false;
            state->connecting = false;
            QObject::disconnect(state->backend, nullptr, this, nullptr);
            state->backend->stop();
        }
    }

    if (m_trayIcon) {
        m_trayIcon->hide();
    }
    close();
    QApplication::quit();
}

QString MainWindow::imPayload(const QString &text) const
{
    if (text.startsWith(QLatin1Char('<'))) {
        const int end = text.indexOf(QStringLiteral("> "));
        if (end >= 0) return text.mid(end + 2);
    }
    return text;
}

QString MainWindow::imSpeakerPrefix(const QString &text) const
{
    if (text.startsWith(QLatin1Char('<'))) {
        const int end = text.indexOf(QStringLiteral("> "));
        if (end >= 0) return text.left(end + 2);
    }
    return {};
}

QString MainWindow::secureTrustKey(BackendState *state, const QString &target) const
{
    if (!state || state->profileId.isEmpty()) return {};
    const QString source = state->profileId + QChar(0x1f) + target.toCaseFolded();
    const QByteArray digest = QCryptographicHash::hash(source.toUtf8(), QCryptographicHash::Sha256).toHex();
    return QStringLiteral("security/trustedPeers/%1").arg(QString::fromLatin1(digest));
}

QString MainWindow::trustedFingerprint(BackendState *state, const QString &target) const
{
    const QString key = secureTrustKey(state, target);
    if (key.isEmpty()) return {};
    QSettings settings;
    return settings.value(key).toString();
}

void MainWindow::setTrustedFingerprint(BackendState *state,
                                       const QString &target,
                                       const QString &fingerprint)
{
    const QString key = secureTrustKey(state, target);
    if (key.isEmpty() || fingerprint.isEmpty()) return;
    QSettings settings;
    settings.setValue(key, fingerprint);
    settings.sync();
}

void MainWindow::clearTrustedFingerprint(BackendState *state, const QString &target)
{
    const QString key = secureTrustKey(state, target);
    if (key.isEmpty()) return;
    QSettings settings;
    settings.remove(key);
    settings.sync();
}

void MainWindow::startSecureSession(ChatWindow *window)
{
    if (!window || window->kind() != QStringLiteral("im")) return;
    BackendState *state = stateById(window->backendId());
    if (!state || !state->connected || !state->backend) return;
    if (!m_options.encryptedDmEnabled || !m_secureReady) {
        QMessageBox::warning(this, QStringLiteral("Encrypted DMs"),
                             m_secureReady
                                 ? QStringLiteral("Encrypted DMs are disabled in Tools > Options.")
                                 : QStringLiteral("Encrypted DMs are unavailable: %1").arg(m_secureError));
        return;
    }
    if (state->backend->settings().protocol == ConnectionSettings::Protocol::Telnet) return;

    QString notice;
    const QString frame = m_secure.beginHandshake(state->profileId, window->target(), &notice);
    if (frame.isEmpty()) {
        window->appendMessage(QStringLiteral("[error] [secure] Could not start secure handshake."));
        return;
    }
    m_outgoingSecureFrames.insert(state->profileId + QChar(0x1f)
        + window->target().toCaseFolded() + QChar(0x1f) + frame);
    state->backend->sendPrivateMessage(window->target(), frame);
    window->appendMessage(QStringLiteral("[secure] %1").arg(notice));
    updateConversationSecurity(window);
}

void MainWindow::showSecureStatus(ChatWindow *window)
{
    if (!window || window->kind() != QStringLiteral("im")) return;
    BackendState *state = stateById(window->backendId());
    if (!state || !m_secureReady) return;

    const QString peer = m_secure.peerFingerprint(state->profileId, window->target());
    const QString local = m_secure.localFingerprint(state->profileId);
    const QString trusted = trustedFingerprint(state, window->target());
    QString trustState = QStringLiteral("unverified");
    if (!peer.isEmpty() && trusted == peer) trustState = QStringLiteral("trusted / verified");
    else if (!trusted.isEmpty() && trusted != peer) trustState = QStringLiteral("TRUST MISMATCH");

    const QString body = peer.isEmpty()
        ? QStringLiteral("No secure session is active with %1.\n\nLocal fingerprint:\n%2")
              .arg(window->displayName(), local)
        : QStringLiteral("Secure session with %1\n\nPeer fingerprint:\n%2\n\nLocal fingerprint:\n%3\n\nTrust: %4\n\nCompare the peer fingerprint using a separate trusted channel before trusting it.")
              .arg(window->displayName(), peer, local, trustState);
    QMessageBox::information(this, QStringLiteral("Secure Session Status"), body);
}

void MainWindow::trustSecurePeer(ChatWindow *window)
{
    if (!window || window->kind() != QStringLiteral("im")) return;
    BackendState *state = stateById(window->backendId());
    if (!state || !m_secureReady) return;
    const QString peer = m_secure.peerFingerprint(state->profileId, window->target());
    if (peer.isEmpty()) {
        QMessageBox::information(this, QStringLiteral("Trust Peer"),
                                 QStringLiteral("Start a secure session first so the client can receive the peer fingerprint."));
        return;
    }

    const auto answer = QMessageBox::question(
        this,
        QStringLiteral("Trust Peer Fingerprint"),
        QStringLiteral("Peer: %1\n\nFingerprint:\n%2\n\nHave you compared this fingerprint through a separate trusted channel and confirmed it matches?")
            .arg(window->displayName(), peer),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (answer != QMessageBox::Yes) return;

    setTrustedFingerprint(state, window->target(), peer);
    window->appendMessage(QStringLiteral("[secure] Peer fingerprint trusted: %1").arg(peer));
    updateConversationSecurity(window);
}

void MainWindow::untrustSecurePeer(ChatWindow *window)
{
    if (!window || window->kind() != QStringLiteral("im")) return;
    BackendState *state = stateById(window->backendId());
    if (!state) return;
    clearTrustedFingerprint(state, window->target());
    window->appendMessage(QStringLiteral("[secure] Saved trusted fingerprint cleared."));
    updateConversationSecurity(window);
}

void MainWindow::closeSecureSession(ChatWindow *window)
{
    if (!window || window->kind() != QStringLiteral("im")) return;
    BackendState *state = stateById(window->backendId());
    if (!state) return;
    m_secure.closeSession(state->profileId, window->target());
    window->appendMessage(QStringLiteral("[secure] Secure session closed; messages are plaintext until a new secure session is started."));
    updateConversationSecurity(window);
}

void MainWindow::showSelectedFingerprint()
{
    BackendState *state = selectedState();
    if (!state || !m_secureReady) {
        QMessageBox::warning(this, QStringLiteral("Secure Identity"),
                             m_secureReady ? QStringLiteral("Select a connection first.")
                                           : QStringLiteral("Encrypted DMs are unavailable: %1").arg(m_secureError));
        return;
    }
    QMessageBox::information(
        this,
        QStringLiteral("Secure Identity Fingerprint"),
        QStringLiteral("This saved connection profile has its own stable secure identity.\n\n%1\n\nFingerprint:\n%2")
            .arg(state->backend ? state->backend->protocolName() : QStringLiteral("Connection"),
                 m_secure.localFingerprint(state->profileId)));
}

void MainWindow::sendFile(ChatWindow *window)
{
    if (!window || window->kind() != QStringLiteral("im")) return;
    BackendState *state = stateById(window->backendId());
    if (!state || !state->connected || !state->backend || !m_secureReady
        || !m_secure.hasSession(state->profileId, window->target())) {
        QMessageBox::information(this, QStringLiteral("Secure File Transfer"),
                                 QStringLiteral("Start a secure CPX3 session with this peer before sending a file."));
        return;
    }
    if (!m_secure.peerSupports(state->profileId, window->target(), QStringLiteral("file-transfer"))) {
        QMessageBox::information(this, QStringLiteral("Secure File Transfer"),
                                 QStringLiteral("This peer has not advertised CPX file-transfer support. The peer may be running an older client."));
        return;
    }

    const QString path = QFileDialog::getOpenFileName(this, QStringLiteral("Send Secure File"));
    if (path.isEmpty()) return;

    QString transferId;
    QString offer;
    QString error;
    const bool reliableTransfer = m_secure.peerSupports(
        state->profileId, window->target(), QStringLiteral("file-ack"));
    const bool directPreferred = reliableTransfer && m_secure.peerSupports(
        state->profileId, window->target(), QStringLiteral("file-direct-v1"));
    if (!m_fileTransfers.createOffer(window->target(), path, transferId, offer, &error,
                                     reliableTransfer)) {
        QMessageBox::warning(this, QStringLiteral("Secure File Transfer"), error);
        return;
    }
    m_fileTransferProfiles.insert(transferId, state->profileId);
    m_fileTransferProgressShown.insert(transferId, -10);

    const QString peer = window->displayName();
    refreshTransferWindow(transferId, QStringLiteral("Upload"), peer, QStringLiteral("Offering"));
    logTransfer(QStringLiteral("Offering %1 to %2 [%3]%4")
                    .arg(QFileInfo(path).fileName(), peer, transferId,
                         directPreferred ? QStringLiteral(" — direct encrypted transport preferred")
                         : (reliableTransfer ? QStringLiteral(" — reliable ACK relay mode")
                                             : QStringLiteral(" — legacy relay mode"))));

    if (!sendSecureControlPayload(state, window->target(), offer)) {
        logTransfer(QStringLiteral("Failed to send file offer for %1 [%2]")
                        .arg(QFileInfo(path).fileName(), transferId));
        return;
    }
}

bool MainWindow::sendSecureControlPayload(BackendState *state,
                                          const QString &target,
                                          const QString &plaintext)
{
    if (!state || !state->backend || !state->connected || !m_secureReady
        || !m_secure.hasSession(state->profileId, target)) return false;
    QString error;
    const QString frame = m_secure.encrypt(state->profileId, target, plaintext, &error);
    if (frame.isEmpty()) {
        logTransfer(QStringLiteral("ERROR: %1").arg(error));
        return false;
    }
    if (state->backend->settings().protocol == ConnectionSettings::Protocol::Irc
        && frame.toUtf8().size() > 400) {
        const QString message = QStringLiteral("Internal IRC transfer frame exceeded the safe message size.");
        logTransfer(QStringLiteral("ERROR: %1").arg(message));
        return false;
    }
    m_outgoingSecureFrames.insert(state->profileId + QChar(0x1f)
        + target.toCaseFolded() + QChar(0x1f) + frame);
    state->backend->sendPrivateMessage(target, frame);
    return true;
}

void MainWindow::logTransfer(const QString &message, bool showWindow)
{
    if (!m_transferWindow || message.trimmed().isEmpty()) return;
    m_transferWindow->appendLog(message);
    if (showWindow) m_transferWindow->showAndRaise();
}

void MainWindow::refreshTransferWindow(const QString &transferId,
                                       const QString &direction,
                                       const QString &peer,
                                       const QString &status)
{
    if (!m_transferWindow || transferId.isEmpty()) return;
    const auto info = m_fileTransfers.transfer(transferId);
    if (info.id.isEmpty()) return;
    m_transferWindow->updateTransfer(
        transferId,
        direction,
        peer,
        info.fileName,
        info.transferred,
        info.total,
        status.isEmpty() ? info.status : status,
        info.resumable);
}

void MainWindow::appendTransferProgress(const CpxFileTransferManager::Event &event,
                                        const QString &direction,
                                        const QString &peer)
{
    if (event.id.isEmpty()) return;
    const auto info = m_fileTransfers.transfer(event.id);
    const qint64 transferred = event.transferred > 0 ? event.transferred : info.transferred;
    const qint64 total = event.total > 0 ? event.total : info.total;
    const int percent = total > 0 ? static_cast<int>((transferred * 100) / total) : event.percent;
    const int bucket = percent >= 100 ? 100 : (qMax(0, percent) / 5) * 5;
    const int previous = m_fileTransferProgressShown.value(event.id, -5);

    refreshTransferWindow(event.id, direction, peer,
                          info.status.isEmpty() ? QStringLiteral("Transferring") : info.status);
    if (bucket <= previous && bucket < 100) return;
    m_fileTransferProgressShown[event.id] = bucket;
    logTransfer(QStringLiteral("%1 %2: %3% (%4 / %5 bytes) [%6]")
                    .arg(direction, info.fileName)
                    .arg(qMax(0, percent))
                    .arg(transferred)
                    .arg(total)
                    .arg(event.id), false);
}

bool MainWindow::handleFileTransferPayload(BackendState *state,
                                           const QString &target,
                                           const QString &plaintext,
                                           ChatWindow *window)
{
    Q_UNUSED(window);
    if (!CpxFileTransferManager::looksLikeMessage(plaintext)) return false;
    if (!state) return true;

    const QString peer = targetDisplayName(state, QStringLiteral("im"), target);
    const auto event = m_fileTransfers.processIncoming(target, plaintext);
    if (!event.id.isEmpty()) m_fileTransferProfiles.insert(event.id, state->profileId);
    if (!event.replyPayload.isEmpty()) {
        sendSecureControlPayload(state, target, event.replyPayload);
    }

    using Kind = CpxFileTransferManager::EventKind;
    switch (event.kind) {
    case Kind::OfferReceived: {
        refreshTransferWindow(event.id, QStringLiteral("Download"), peer, QStringLiteral("Waiting for acceptance"));
        logTransfer(QStringLiteral("%1 wants to send %2 (%3 bytes) [%4]")
                        .arg(peer, event.fileName).arg(event.total).arg(event.id));
        const auto answer = QMessageBox::question(
            m_transferWindow ? static_cast<QWidget *>(m_transferWindow) : static_cast<QWidget *>(this),
            QStringLiteral("Secure File Transfer"),
            QStringLiteral("%1 wants to send:\n\n%2\n%3 bytes\n\nAccept this encrypted transfer?")
                .arg(peer, event.fileName).arg(event.total),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (answer != QMessageBox::Yes) {
            const QString reply = m_fileTransfers.declineIncoming(event.id, QStringLiteral("declined by user"));
            sendSecureControlPayload(state, target, reply);
            refreshTransferWindow(event.id, QStringLiteral("Download"), peer, QStringLiteral("Declined"));
            logTransfer(QStringLiteral("Declined incoming transfer %1 [%2]").arg(event.fileName, event.id));
            return true;
        }
        QString downloadDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        if (downloadDir.isEmpty()) downloadDir = QDir::homePath();
        const QString suggested = QDir(downloadDir).filePath(event.fileName);
        const QString destination = QFileDialog::getSaveFileName(
            m_transferWindow ? static_cast<QWidget *>(m_transferWindow) : static_cast<QWidget *>(this),
            QStringLiteral("Save Secure File"), suggested);
        if (destination.isEmpty()) {
            const QString reply = m_fileTransfers.declineIncoming(event.id, QStringLiteral("save cancelled"));
            sendSecureControlPayload(state, target, reply);
            refreshTransferWindow(event.id, QStringLiteral("Download"), peer, QStringLiteral("Cancelled"));
            logTransfer(QStringLiteral("Save cancelled for %1 [%2]").arg(event.fileName, event.id));
            return true;
        }
        QString error;
        QString reply = m_fileTransfers.acceptIncoming(event.id, destination, &error);
        if (reply.isEmpty()) {
            logTransfer(QStringLiteral("ERROR accepting %1: %2 [%3]").arg(event.fileName, error, event.id));
            const QString cancel = m_fileTransfers.declineIncoming(event.id, error);
            sendSecureControlPayload(state, target, cancel);
            refreshTransferWindow(event.id, QStringLiteral("Download"), peer, QStringLiteral("Error"));
            return true;
        }

        // Prefer the dedicated encrypted TCP data path whenever both peers
        // advertise it. AIM/IRC remains the authenticated control channel;
        // if the direct socket cannot be established we automatically resume
        // at the receiver's current byte offset through the reliable relay.
        const QString relayReply = reply;
        bool directReady = false;
        if (m_secure.peerSupports(state->profileId, target, QStringLiteral("file-direct-v1"))
            && m_secure.peerSupports(state->profileId, target, QStringLiteral("file-ack"))) {
            QString keyError;
            const QByteArray transferKey = m_secure.fileTransferKey(
                state->profileId, target, event.id, &keyError);
            const auto acceptedInfo = m_fileTransfers.transfer(event.id);
            CpxDirectTransferManager::ListenResult listener;
            if (!transferKey.isEmpty()
                && m_directTransfers.prepareIncoming(
                    event.id,
                    acceptedInfo.path + QStringLiteral(".cpxpart"),
                    acceptedInfo.total,
                    acceptedInfo.transferred,
                    transferKey,
                    listener,
                    &error)) {
                const QString directReply = m_fileTransfers.acceptIncoming(
                    event.id, destination, &error, listener.port, listener.hosts);
                directReady = !directReply.isEmpty();
                if (directReady) {
                    reply = directReply;
                } else {
                    reply = relayReply;
                    m_directTransfers.cancel(event.id);
                }
            } else if (!keyError.isEmpty()) {
                error = keyError;
            }
            if (!directReady && !error.isEmpty()) {
                logTransfer(QStringLiteral("Direct transfer unavailable for %1; using secure relay fallback: %2 [%3]")
                                .arg(event.fileName, error, event.id), false);
                error.clear();
            }
        }

        m_fileTransferProgressShown.insert(event.id, -5);
        if (!sendSecureControlPayload(state, target, reply) && directReady) {
            m_directTransfers.cancel(event.id);
            directReady = false;
            reply = m_fileTransfers.fallbackIncomingToRelay(event.id);
            if (!reply.isEmpty()) sendSecureControlPayload(state, target, reply);
            logTransfer(QStringLiteral("Direct endpoint advertisement could not be relayed; using secure relay fallback [%1]")
                            .arg(event.id), false);
        }
        const auto info = m_fileTransfers.transfer(event.id);
        refreshTransferWindow(event.id, QStringLiteral("Download"), peer,
                              directReady ? QStringLiteral("Waiting for direct connection")
                                          : (info.transferred > 0 ? QStringLiteral("Resuming relay")
                                                                : QStringLiteral("Receiving by relay")));
        if (directReady) {
            logTransfer(QStringLiteral("Accepted %1 → %2 [%3] — encrypted direct transport prepared")
                            .arg(event.fileName, destination, event.id));
        } else {
            logTransfer(info.transferred > 0
                ? QStringLiteral("Resuming %1 at byte %2 → %3 [%4] by secure relay")
                      .arg(event.fileName).arg(info.transferred).arg(destination, event.id)
                : QStringLiteral("Receiving %1 → %2 [%3] by secure relay")
                      .arg(event.fileName, destination, event.id));
        }
        return true;
    }
    case Kind::Accepted:
        if (event.direct) {
            refreshTransferWindow(event.id, QStringLiteral("Upload"), peer,
                                  QStringLiteral("Connecting direct"));
            logTransfer(QStringLiteral("%1 accepted %2; establishing encrypted direct data connection [%3]")
                            .arg(peer, event.fileName, event.id));
            startDirectOutgoing(event, state);
        } else {
            refreshTransferWindow(event.id, QStringLiteral("Upload"), peer, QStringLiteral("Sending by relay"));
            logTransfer(QStringLiteral("%1 accepted %2; secure relay upload started [%3]")
                            .arg(peer, event.fileName, event.id));
            appendTransferProgress(event, QStringLiteral("Upload"), peer);
        }
        return true;
    case Kind::Fallback:
        m_directTransfers.cancel(event.id);
        refreshTransferWindow(event.id, QStringLiteral("Download"), peer,
                              QStringLiteral("Relay fallback"));
        logTransfer(QStringLiteral("Direct transport fallback requested%1; resuming through secure relay at byte %2 [%3]")
                        .arg(event.reason.isEmpty() ? QString() : QStringLiteral(": %1").arg(event.reason))
                        .arg(event.transferred).arg(event.id));
        return true;
    case Kind::Declined:
        refreshTransferWindow(event.id, QStringLiteral("Upload"), peer, QStringLiteral("Declined"));
        logTransfer(QStringLiteral("Transfer declined%1 [%2]")
                        .arg(event.reason.isEmpty() ? QString() : QStringLiteral(": %1").arg(event.reason), event.id));
        return true;
    case Kind::ResumeRequested:
        logTransfer(QStringLiteral("%1 requested resume of %2 [%3]")
                        .arg(peer, event.fileName, event.id));
        resumeIncomingFileTransfer(event.id, state, peer);
        return true;
    case Kind::Progress:
        appendTransferProgress(event, event.outgoing ? QStringLiteral("Upload") : QStringLiteral("Download"), peer);
        return true;
    case Kind::Completed:
        appendTransferProgress(event, event.outgoing ? QStringLiteral("Upload") : QStringLiteral("Download"), peer);
        refreshTransferWindow(event.id,
                              event.outgoing ? QStringLiteral("Upload") : QStringLiteral("Download"),
                              peer, QStringLiteral("Complete"));
        if (event.outgoing) {
            logTransfer(QStringLiteral("Receiver confirmed SHA-256 verification; upload complete: %1 [%2]")
                            .arg(event.fileName, event.id));
        } else {
            logTransfer(QStringLiteral("Download complete and SHA-256 verified: %1 [%2]")
                            .arg(event.path, event.id));
        }
        m_fileTransferProgressShown.remove(event.id);
        QTimer::singleShot(1800, this, [this, id = event.id]() {
            clearFileTransfer(id);
        });
        return true;
    case Kind::Cancelled: {
        m_directTransfers.cancel(event.id);
        const auto info = m_fileTransfers.transfer(event.id);
        refreshTransferWindow(event.id,
                              info.outgoing ? QStringLiteral("Upload") : QStringLiteral("Download"),
                              peer, QStringLiteral("Cancelled"));
        logTransfer(QStringLiteral("Transfer cancelled%1 [%2]")
                        .arg(event.reason.isEmpty() ? QString() : QStringLiteral(": %1").arg(event.reason), event.id));
        m_fileTransferProgressShown.remove(event.id);
        return true;
    }
    case Kind::Error: {
        const auto info = m_fileTransfers.transfer(event.id);
        if (!event.id.isEmpty()) {
            m_directTransfers.cancel(event.id);
            const QString cancel = m_fileTransfers.cancel(event.id, event.reason);
            sendSecureControlPayload(state, target, cancel);
        }
        refreshTransferWindow(event.id,
                              info.outgoing ? QStringLiteral("Upload") : QStringLiteral("Download"),
                              peer, QStringLiteral("Error"));
        logTransfer(QStringLiteral("ERROR: %1%2")
                        .arg(event.reason,
                             event.id.isEmpty() ? QString() : QStringLiteral(" [%1]").arg(event.id)));
        return true;
    }
    case Kind::None:
        return true;
    }
    return true;
}

void MainWindow::cancelFileTransfer(const QString &transferId)
{
    const auto info = m_fileTransfers.transfer(transferId);
    if (info.id.isEmpty() || info.complete) return;

    m_directTransfers.cancel(transferId);
    const QString payload = m_fileTransfers.cancel(
        transferId, QStringLiteral("cancelled by user"));
    BackendState *state = stateById(m_fileTransferProfiles.value(transferId));
    if (state && state->connected && state->backend && !info.target.isEmpty()) {
        sendSecureControlPayload(state, info.target, payload);
    }
    const QString peer = state
        ? targetDisplayName(state, QStringLiteral("im"), info.target)
        : info.target;
    refreshTransferWindow(transferId,
                          info.outgoing ? QStringLiteral("Upload") : QStringLiteral("Download"),
                          peer, QStringLiteral("Cancelled"));
    logTransfer(QStringLiteral("Cancelled %1 transfer of %2 [%3]; chat connection remains open")
                    .arg(info.outgoing ? QStringLiteral("upload") : QStringLiteral("download"),
                         info.fileName, transferId));
    m_fileTransferProgressShown.remove(transferId);
}

bool MainWindow::resumeIncomingFileTransfer(const QString &transferId,
                                            BackendState *state,
                                            const QString &peer)
{
    const auto info = m_fileTransfers.transfer(transferId);
    if (info.id.isEmpty() || info.outgoing || !state || !state->connected || !state->backend) return false;
    if (!m_fileTransfers.canResume(transferId)) {
        QMessageBox::information(this, QStringLiteral("Resume File Transfer"),
                                 QStringLiteral("This transfer no longer has resumable partial data."));
        refreshTransferWindow(transferId, QStringLiteral("Download"), peer, QStringLiteral("Cancelled"));
        return false;
    }
    if (!m_secure.hasSession(state->profileId, info.target)) {
        QMessageBox::information(this, QStringLiteral("Resume File Transfer"),
                                 QStringLiteral("Re-establish the secure CPX session with this peer before resuming the transfer."));
        return false;
    }

    m_directTransfers.cancel(transferId);
    QString error;
    QString payload;
    bool directReady = false;
    if (m_secure.peerSupports(state->profileId, info.target, QStringLiteral("file-direct-v1"))
        && m_secure.peerSupports(state->profileId, info.target, QStringLiteral("file-ack"))) {
        QString keyError;
        const QByteArray transferKey = m_secure.fileTransferKey(
            state->profileId, info.target, transferId, &keyError);
        const QString partPath = info.path + QStringLiteral(".cpxpart");
        qint64 resumeOffset = 0;
        const QFileInfo partInfo(partPath);
        if (partInfo.exists() && partInfo.isFile() && partInfo.size() <= info.total) {
            resumeOffset = partInfo.size();
        }
        CpxDirectTransferManager::ListenResult listener;
        if (!transferKey.isEmpty()
            && m_directTransfers.prepareIncoming(
                transferId, partPath, info.total, resumeOffset,
                transferKey, listener, &error)) {
            payload = m_fileTransfers.resumeIncoming(
                transferId, &error, listener.port, listener.hosts);
            directReady = !payload.isEmpty();
            if (!directReady) m_directTransfers.cancel(transferId);
        } else if (!keyError.isEmpty()) {
            error = keyError;
        }
    }
    if (!directReady) {
        error.clear();
        payload = m_fileTransfers.resumeIncoming(transferId, &error);
    }
    if (payload.isEmpty()) {
        logTransfer(QStringLiteral("ERROR resuming %1: %2 [%3]")
                        .arg(info.fileName, error, transferId));
        return false;
    }
    if (!sendSecureControlPayload(state, info.target, payload)) {
        if (directReady) m_directTransfers.cancel(transferId);
        logTransfer(QStringLiteral("ERROR: could not send resume acceptance for %1 [%2]")
                        .arg(info.fileName, transferId));
        return false;
    }

    const auto resumed = m_fileTransfers.transfer(transferId);
    m_fileTransferProgressShown.insert(transferId, -5);
    refreshTransferWindow(transferId, QStringLiteral("Download"), peer,
                          directReady ? QStringLiteral("Waiting for direct connection")
                                      : (resumed.transferred > 0 ? QStringLiteral("Resuming relay")
                                                                 : QStringLiteral("Receiving by relay")));
    logTransfer(QStringLiteral("Resuming download %1 at byte %2 [%3]%4")
                    .arg(resumed.fileName)
                    .arg(resumed.transferred)
                    .arg(transferId)
                    .arg(directReady ? QStringLiteral(" — encrypted direct transport prepared")
                                     : QStringLiteral(" — secure relay")));
    return true;
}

void MainWindow::resumeFileTransfer(const QString &transferId)
{
    const auto info = m_fileTransfers.transfer(transferId);
    if (info.id.isEmpty() || !m_fileTransfers.canResume(transferId)) {
        QMessageBox::information(this, QStringLiteral("Resume File Transfer"),
                                 QStringLiteral("This transfer is no longer resumable."));
        return;
    }
    BackendState *state = stateById(m_fileTransferProfiles.value(transferId));
    if (!state || !state->connected || !state->backend
        || !m_secure.hasSession(state->profileId, info.target)) {
        QMessageBox::information(this, QStringLiteral("Resume File Transfer"),
                                 QStringLiteral("Re-establish the connection and secure CPX session with this peer before resuming."));
        return;
    }
    const QString peer = targetDisplayName(state, QStringLiteral("im"), info.target);
    if (!info.outgoing) {
        resumeIncomingFileTransfer(transferId, state, peer);
        return;
    }

    QString error;
    const QString payload = m_fileTransfers.requestResume(transferId, &error);
    if (payload.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Resume File Transfer"), error);
        return;
    }
    if (!sendSecureControlPayload(state, info.target, payload)) {
        m_fileTransfers.cancel(transferId, QStringLiteral("resume request could not be sent"));
        refreshTransferWindow(transferId, QStringLiteral("Upload"), peer, QStringLiteral("Cancelled"));
        return;
    }
    refreshTransferWindow(transferId, QStringLiteral("Upload"), peer, QStringLiteral("Resume requested"));
    logTransfer(QStringLiteral("Requested resume of upload %1 [%2]").arg(info.fileName, transferId));
}

void MainWindow::clearFileTransfer(const QString &transferId)
{
    const auto info = m_fileTransfers.transfer(transferId);
    if (info.id.isEmpty()) {
        if (m_transferWindow) m_transferWindow->removeTransfer(transferId);
        return;
    }
    if (!info.complete) return;

    m_directTransfers.cancel(transferId);
    QString error;
    if (!m_fileTransfers.clearTransfer(transferId, &error)) {
        if (!error.isEmpty()) logTransfer(QStringLiteral("ERROR clearing transfer: %1 [%2]").arg(error, transferId));
        return;
    }
    m_fileTransferProfiles.remove(transferId);
    m_fileTransferProgressShown.remove(transferId);
    if (m_transferWindow) m_transferWindow->removeTransfer(transferId);
}

void MainWindow::startDirectOutgoing(const CpxFileTransferManager::Event &event,
                                     BackendState *state)
{
    if (!state || event.id.isEmpty()) return;
    const auto info = m_fileTransfers.transfer(event.id);
    QString error;
    const QByteArray key = m_secure.fileTransferKey(
        state->profileId, info.target, event.id, &error);
    if (key.isEmpty()
        || !m_directTransfers.startOutgoing(event.id, info.path, info.total,
                                            info.transferred, event.directHosts,
                                            event.directPort, key, &error)) {
        handleDirectFailure(event.id,
                            error.isEmpty()
                                ? QStringLiteral("direct transfer could not be started")
                                : error,
                            true);
        return;
    }
    const QString peer = targetDisplayName(state, QStringLiteral("im"), info.target);
    refreshTransferWindow(event.id, QStringLiteral("Upload"), peer,
                          QStringLiteral("Direct encrypted transfer"));
    logTransfer(QStringLiteral("Direct encrypted upload connected/preparing for %1 [%2]")
                    .arg(info.fileName, event.id), false);
}

void MainWindow::handleDirectProgress(const QString &transferId,
                                      qint64 transferred,
                                      qint64 total,
                                      bool outgoing)
{
    m_fileTransfers.updateDirectProgress(transferId, transferred, outgoing);
    const auto info = m_fileTransfers.transfer(transferId);
    BackendState *state = stateById(m_fileTransferProfiles.value(transferId));
    const QString peer = state
        ? targetDisplayName(state, QStringLiteral("im"), info.target)
        : info.target;
    CpxFileTransferManager::Event event;
    event.id = transferId;
    event.fileName = info.fileName;
    event.transferred = transferred;
    event.total = total;
    event.outgoing = outgoing;
    appendTransferProgress(event,
                           outgoing ? QStringLiteral("Upload") : QStringLiteral("Download"),
                           peer);
    refreshTransferWindow(transferId,
                          outgoing ? QStringLiteral("Upload") : QStringLiteral("Download"),
                          peer,
                          outgoing ? QStringLiteral("Sending direct")
                                   : QStringLiteral("Receiving direct"));
}

void MainWindow::handleDirectIncomingFinished(const QString &transferId)
{
    const auto before = m_fileTransfers.transfer(transferId);
    BackendState *state = stateById(m_fileTransferProfiles.value(transferId));
    if (before.id.isEmpty() || !state) return;

    QString error;
    if (!m_fileTransfers.finalizeIncomingDirect(transferId, &error)) {
        const QString cancel = m_fileTransfers.cancel(transferId, error);
        refreshTransferWindow(transferId, QStringLiteral("Download"),
                              targetDisplayName(state, QStringLiteral("im"), before.target),
                              QStringLiteral("Error"));
        logTransfer(QStringLiteral("ERROR verifying direct download %1: %2 [%3]")
                        .arg(before.fileName, error, transferId));
        sendSecureControlPayload(state, before.target, cancel);
        return;
    }

    sendSecureControlPayload(state, before.target,
                             m_fileTransfers.completionPayload(transferId));
    const auto info = m_fileTransfers.transfer(transferId);
    const QString peer = targetDisplayName(state, QStringLiteral("im"), info.target);
    refreshTransferWindow(transferId, QStringLiteral("Download"), peer,
                          QStringLiteral("Complete"));
    logTransfer(QStringLiteral("Direct download complete and SHA-256 verified: %1 [%2]")
                    .arg(info.path, transferId));
    m_fileTransferProgressShown.remove(transferId);
    QTimer::singleShot(1800, this, [this, transferId]() {
        clearFileTransfer(transferId);
    });
}

void MainWindow::handleDirectOutgoingFinished(const QString &transferId)
{
    m_fileTransfers.markOutgoingDirectSent(transferId);
    const auto info = m_fileTransfers.transfer(transferId);
    BackendState *state = stateById(m_fileTransferProfiles.value(transferId));
    const QString peer = state
        ? targetDisplayName(state, QStringLiteral("im"), info.target)
        : info.target;
    refreshTransferWindow(transferId, QStringLiteral("Upload"), peer,
                          QStringLiteral("Verifying"));
    logTransfer(QStringLiteral("Direct upload transmitted: %1; waiting for receiver SHA-256 confirmation [%2]")
                    .arg(info.fileName, transferId));
}

void MainWindow::handleDirectFailure(const QString &transferId,
                                     const QString &reason,
                                     bool outgoing)
{
    const auto info = m_fileTransfers.transfer(transferId);
    BackendState *state = stateById(m_fileTransferProfiles.value(transferId));
    if (info.id.isEmpty() || !state || !state->connected || !state->backend) return;

    m_directTransfers.cancel(transferId);
    const QString peer = targetDisplayName(state, QStringLiteral("im"), info.target);
    QString payload;
    if (outgoing) {
        payload = m_fileTransfers.requestOutgoingRelayFallback(transferId, reason);
        refreshTransferWindow(transferId, QStringLiteral("Upload"), peer,
                              QStringLiteral("Switching to relay"));
        logTransfer(QStringLiteral("Direct upload unavailable (%1); requesting secure relay resume [%2]")
                        .arg(reason, transferId));
    } else {
        payload = m_fileTransfers.fallbackIncomingToRelay(transferId);
        refreshTransferWindow(transferId, QStringLiteral("Download"), peer,
                              QStringLiteral("Switching to relay"));
        logTransfer(QStringLiteral("Direct download interrupted (%1); resuming through secure relay at byte %2 [%3]")
                        .arg(reason).arg(info.transferred).arg(transferId));
    }
    if (!payload.isEmpty()) sendSecureControlPayload(state, info.target, payload);
}

void MainWindow::pumpFileTransfers()
{
    const QStringList ids = m_fileTransfers.activeOutgoingIds();
    for (const QString &id : ids) {
        const QString profileId = m_fileTransferProfiles.value(id);
        BackendState *state = nullptr;
        for (BackendState *candidate : m_states) {
            if (candidate && candidate->profileId == profileId) { state = candidate; break; }
        }
        const auto before = m_fileTransfers.transfer(id);
        if (!state || !state->connected || !state->backend
            || !m_secure.hasSession(profileId, before.target)) continue;

        const QString peer = targetDisplayName(state, QStringLiteral("im"), before.target);
        const bool irc = state->backend->settings().protocol == ConnectionSettings::Protocol::Irc;
        const int rawChunk = irc ? 120 : 768;
        const int minimumSendIntervalMs = irc ? 1000 : 500;
        bool finished = false;
        QString error;
        const QString payload = m_fileTransfers.nextOutgoingPayload(
            id, rawChunk, &finished, &error, minimumSendIntervalMs);
        if (payload.isEmpty()) {
            if (!error.isEmpty()) {
                refreshTransferWindow(id, QStringLiteral("Upload"), peer, QStringLiteral("Error"));
                logTransfer(QStringLiteral("ERROR: %1 [%2]").arg(error, id));
            }
            continue;
        }
        if (!sendSecureControlPayload(state, before.target, payload)) continue;

        const auto after = m_fileTransfers.transfer(id);
        CpxFileTransferManager::Event progress;
        progress.id = id; progress.fileName = after.fileName;
        progress.transferred = after.transferred; progress.total = after.total;
        progress.percent = after.total > 0 ? static_cast<int>((after.transferred * 100) / after.total) : 100;
        appendTransferProgress(progress, QStringLiteral("Upload"), peer);
        if (finished) {
            refreshTransferWindow(id, QStringLiteral("Upload"), peer, QStringLiteral("Verifying"));
            logTransfer(QStringLiteral("Finished transmitting %1; waiting for receiver SHA-256 confirmation [%2]")
                            .arg(after.fileName, id));
        }
    }
}

bool MainWindow::sendPrivateText(BackendState *state,
                                 const QString &target,
                                 const QString &text,
                                 ChatWindow *window)
{
    if (!state || !state->backend || !state->connected) return false;

    if (m_options.encryptedDmEnabled && m_secureReady
        && m_secure.hasSession(state->profileId, target)) {
        QString error;
        const QString frame = m_secure.encrypt(state->profileId, target, text, &error);
        if (frame.isEmpty()) {
            if (window) window->appendMessage(QStringLiteral("[error] [secure] %1").arg(error));
            return false;
        }
        if (state->backend->settings().protocol == ConnectionSettings::Protocol::Irc
            && frame.toUtf8().size() > 400) {
            if (window) window->appendMessage(QStringLiteral(
                "[error] [secure] Encrypted IRC message is too long; split it into shorter messages."));
            return false;
        }

        m_outgoingSecureFrames.insert(state->profileId + QChar(0x1f)
            + target.toCaseFolded() + QChar(0x1f) + frame);
        state->backend->sendPrivateMessage(target, frame);
        if (window) {
            const QString me = state->identity.isEmpty()
                ? (state->backend->settings().username.isEmpty()
                       ? QStringLiteral("me")
                       : state->backend->settings().username)
                : state->identity;
            window->appendMessage(QStringLiteral("<%1> [secure] %2").arg(me, text));
        }
        return true;
    }

    state->backend->sendPrivateMessage(target, text);
    return true;
}

void MainWindow::handleConversationMessage(ChatWindow *window, const QString &text)
{
    if (!window) return;
    BackendState *state = stateById(window->backendId());
    if (!state || !state->backend || !state->connected) return;

    const QString command = text.trimmed().toCaseFolded();
    if (window->kind() != QStringLiteral("terminal")) {
        if (command == QStringLiteral("/options")) { showOptionsDialog(); return; }
        if (command == QStringLiteral("/help")) { showHelpDialog(); return; }
        if (command == QStringLiteral("/fingerprint")) { selectState(state); showSelectedFingerprint(); return; }
        if (window->kind() == QStringLiteral("im")) {
            if (command == QStringLiteral("/secure")) { startSecureSession(window); return; }
            if (command == QStringLiteral("/securestatus")) { showSecureStatus(window); return; }
            if (command == QStringLiteral("/trust")) { trustSecurePeer(window); return; }
            if (command == QStringLiteral("/untrust")) { untrustSecurePeer(window); return; }
            if (command == QStringLiteral("/secureoff")) { closeSecureSession(window); return; }
        }
    }

    if (window->kind() == QStringLiteral("chat")) {
        state->backend->sendRoomMessage(window->target(), text);
    } else if (window->kind() == QStringLiteral("terminal")) {
        state->backend->sendPrivateMessage(window->target(), text);
    } else {
        sendPrivateText(state, window->target(), text, window);
    }
}

void MainWindow::updateConversationSecurity(ChatWindow *window)
{
    if (!window || window->kind() != QStringLiteral("im") || !m_secureReady) {
        if (window) window->setSecurityState(false, false);
        return;
    }
    BackendState *state = stateById(window->backendId());
    if (!state || !m_secure.hasSession(state->profileId, window->target())) {
        window->setSecurityState(false, false);
        return;
    }
    const QString peer = m_secure.peerFingerprint(state->profileId, window->target());
    const QString trusted = trustedFingerprint(state, window->target());
    window->setSecurityState(true, !peer.isEmpty() && trusted == peer,
                             peer, m_secure.localFingerprint(state->profileId));
}

void MainWindow::handleConnected(ChatBackend *backend,
                                 const QString &identity,
                                 const QString &endpoint)
{
    BackendState *state = stateFor(backend);
    if (!state) {
        return;
    }

    state->connecting = false;
    state->connected = true;
    state->identity = identity;
    state->endpoint = endpoint;
    updateConnectionItem(state);
    appendActivity(backend,
                   QStringLiteral("Connected as %1 — %2").arg(identity, endpoint));
    refreshBuddyList();
    selectState(state);
    updateActions();
    statusBar()->showMessage(
        QStringLiteral("%1 connected").arg(backend->protocolName()), 4000);

    if (backend->settings().protocol == ConnectionSettings::Protocol::Telnet) {
        ChatWindow *terminal = ensureConversationWindow(
            backend, QStringLiteral("terminal"), backend->settings().server, true);
        if (terminal) terminal->setBackendOnline(true);
    }

    // Successful login always brings the Buddy List to the front.
    showBuddyWindow();
}

void MainWindow::handleDisconnected(ChatBackend *backend, const QString &reason)
{
    BackendState *state = stateFor(backend);
    if (!state) {
        return;
    }

    const bool failedWhileConnecting = state->connecting;
    state->connecting = false;
    state->connected = false;
    state->onlineBuddies.clear();

    if (failedWhileConnecting && state->secretRequired) {
        ConnectionSettings cleared = state->backend->settings();
        if (!cleared.savePassword) {
            cleared.password.clear();
            if (auto *sip = qobject_cast<SipBackend *>(state->backend)) {
                // Clearing a failed session secret must not reconfigure PJSUA2
                // from inside a disconnect/error callback.
                sip->clearSessionPassword();
            } else {
                state->backend->setConnectionSettings(cleared);
            }
        }
        state->hasSessionSecret = cleared.savePassword && !cleared.password.isEmpty();
    }

    if (!state->profileId.isEmpty()) {
        m_secure.closeConnection(state->profileId);
    }
    const QString roomPrefix = backend->id() + QStringLiteral("|chat|");
    for (auto it = m_closedRoomKeys.begin(); it != m_closedRoomKeys.end();) {
        if (it->startsWith(roomPrefix)) it = m_closedRoomKeys.erase(it);
        else ++it;
    }
    closeBackendWindows(backend);
    updateConnectionItem(state);
    appendActivity(backend, QStringLiteral("Disconnected: %1").arg(reason));
    refreshBuddyList();
    updateActions();
}

void MainWindow::handleEvent(ChatBackend *backend,
                             const QString &kind,
                             const QString &target,
                             const QString &text)
{
    BackendState *state = stateFor(backend);
    if (!state) return;

    if (kind == QStringLiteral("status") || target.isEmpty()) {
        appendActivity(backend, text);
        return;
    }
    if (!state->connected) return;
    if (kind == QStringLiteral("chat")
        && m_closedRoomKeys.contains(conversationKey(backend, kind, target))) {
        return;
    }

    if (kind == QStringLiteral("im") && m_secureReady) {
        const QString payload = imPayload(text);
        const QString outgoingToken = state->profileId + QChar(0x1f)
            + target.toCaseFolded() + QChar(0x1f) + payload;
        if (m_outgoingSecureFrames.remove(outgoingToken)) {
            return;
        }

        if (SecureChannelManager::looksLikeFrame(payload)) {
            ChatWindow *window = ensureConversationWindow(backend, kind, target, true);
            if (!window) return;

            if (!m_options.encryptedDmEnabled) {
                window->appendMessage(QStringLiteral(
                    "[secure] Encrypted DM frame ignored because encrypted DMs are disabled."));
                return;
            }

            const auto result = m_secure.processIncoming(
                state->profileId, target, payload, m_options.autoReplySecure);

            if (!result.replyFrame.isEmpty()) {
                m_outgoingSecureFrames.insert(state->profileId + QChar(0x1f)
                    + target.toCaseFolded() + QChar(0x1f) + result.replyFrame);
                backend->sendPrivateMessage(target, result.replyFrame);
            }

            const QString capsFrame = m_secure.capabilitiesFrame(state->profileId, target);
            if (!capsFrame.isEmpty()) {
                m_outgoingSecureFrames.insert(state->profileId + QChar(0x1f)
                    + target.toCaseFolded() + QChar(0x1f) + capsFrame);
                backend->sendPrivateMessage(target, capsFrame);
            }

            if (result.kind == SecureChannelManager::IncomingKind::Decrypted) {
                if (handleFileTransferPayload(state, target, result.plaintext, window)) {
                    updateConversationSecurity(window);
                    return;
                }
                QString prefix = imSpeakerPrefix(text);
                if (prefix.isEmpty()) {
                    prefix = QStringLiteral("<%1> ").arg(targetDisplayName(state, kind, target));
                }
                window->appendMessage(prefix + QStringLiteral("[secure] ") + result.plaintext);
                updateConversationSecurity(window);
                return;
            }

            if (result.kind == SecureChannelManager::IncomingKind::Error) {
                window->appendMessage(QStringLiteral("[error] [secure] %1").arg(result.notice));
                updateConversationSecurity(window);
                return;
            }

            if (result.kind == SecureChannelManager::IncomingKind::Control) {
                QString notice = result.notice;
                if (!result.peerFingerprint.isEmpty()) {
                    const QString trusted = trustedFingerprint(state, target);
                    if (!trusted.isEmpty() && trusted != result.peerFingerprint) {
                        window->appendMessage(
                            QStringLiteral("[error] [secure] TRUST WARNING: fingerprint changed. Trusted %1; received %2")
                                .arg(trusted, result.peerFingerprint));
                        m_secure.closeSession(state->profileId, target);
                        updateConversationSecurity(window);
                        return;
                    }
                    if (trusted == result.peerFingerprint) {
                        notice += QStringLiteral(" [trusted]");
                    } else {
                        notice += QStringLiteral(" [UNVERIFIED — compare fingerprints before trusting]");
                    }
                }
                if (!notice.isEmpty()) {
                    if (m_options.showSecureFingerprints || result.peerFingerprint.isEmpty()) {
                        window->appendMessage(QStringLiteral("[secure] %1").arg(notice));
                    } else {
                        window->appendMessage(QStringLiteral("[secure] Secure session established."));
                    }
                }
                updateConversationSecurity(window);
                return;
            }
        }
    }

    ChatWindow *window = ensureConversationWindow(backend, kind, target, true);
    if (window) window->appendMessage(text);
}

void MainWindow::handleMembers(ChatBackend *backend,
                               const QString &room,
                               const QString &action,
                               const QStringList &names)
{
    BackendState *state = stateFor(backend);
    if (!state || !state->connected) {
        return;
    }
    if (m_closedRoomKeys.contains(conversationKey(backend, QStringLiteral("chat"), room))) {
        return;
    }

    ChatWindow *window = ensureConversationWindow(
        backend, QStringLiteral("chat"), room, false);
    if (window) {
        window->updateMembers(action, names);
    }
}

void MainWindow::handleTargetNamed(ChatBackend *backend,
                                   const QString &kind,
                                   const QString &target,
                                   const QString &displayName)
{
    BackendState *state = stateFor(backend);
    if (!state) {
        return;
    }
    state->targetNames.insert(QStringLiteral("%1|%2").arg(kind, target), displayName);
    const QString key = conversationKey(backend, kind, target);
    if (ChatWindow *window = m_windows.value(key, nullptr)) {
        window->setDisplayName(displayName);
    }
}

void MainWindow::handleRoomDiscovered(ChatBackend *backend,
                                      const QString &roomId,
                                      const QString &displayName)
{
    BackendState *state = stateFor(backend);
    if (!state) {
        return;
    }
    state->discoveredRooms.insert(roomId, displayName);
    state->targetNames.insert(QStringLiteral("chat|%1").arg(roomId), displayName);
}

void MainWindow::handleBuddyList(ChatBackend *backend, const QStringList &names)
{
    BackendState *state = stateFor(backend);
    if (!state || !state->backend) {
        return;
    }
    const bool localSipContacts =
        state->backend->settings().protocol == ConnectionSettings::Protocol::Sip;
    if (!state->connected && !localSipContacts) {
        return;
    }
    state->buddies.clear();
    for (const QString &name : names) {
        if (!name.trimmed().isEmpty()) {
            state->buddies.insert(name.trimmed());
        }
    }
    if (state->backend
        && (state->backend->settings().protocol == ConnectionSettings::Protocol::Irc
            || state->backend->settings().protocol == ConnectionSettings::Protocol::Sip)) {
        saveConnections();
    }
    refreshBuddyList();
}

void MainWindow::handleBuddyPresence(ChatBackend *backend,
                                     const QString &name,
                                     bool online)
{
    BackendState *state = stateFor(backend);
    if (!state || !state->connected || name.trimmed().isEmpty()) {
        return;
    }

    const QString key = name.toCaseFolded();
    if (online) {
        state->onlineBuddies.insert(key);
        state->buddies.insert(name);
    } else {
        state->onlineBuddies.remove(key);
    }
    refreshBuddyList();
}

void MainWindow::handleBackendError(ChatBackend *backend,
                                    const QString &context,
                                    const QString &message)
{
    appendActivity(backend, QStringLiteral("[error] %1: %2").arg(context, message));
    statusBar()->showMessage(QStringLiteral("%1: %2").arg(context, message), 8000);
}

void MainWindow::handleConversationClosing(ChatWindow *window)
{
    if (!window) return;
    BackendState *state = stateById(window->backendId());
    if (!state || !state->backend) return;

    if (window->kind() == QStringLiteral("im") && !state->profileId.isEmpty()) {
        m_secure.closeSession(state->profileId, window->target());
    }

    if (state->connected && window->kind() == QStringLiteral("chat")) {
        m_closedRoomKeys.insert(conversationKey(state->backend, QStringLiteral("chat"), window->target()));
        state->backend->leaveRoom(window->target());
    } else if (state->connected && window->kind() == QStringLiteral("terminal")) {
        state->connected = false;
        state->connecting = false;
        updateConnectionItem(state);
        refreshBuddyList();
        updateActions();
        state->backend->stop();
    }
}

