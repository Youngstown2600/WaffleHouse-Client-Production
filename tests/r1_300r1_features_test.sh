#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
fail(){ echo "WaffleHouse 3.1 feature regression failed: $*" >&2; exit 1; }
need(){ grep -F "$2" "$ROOT/$1" >/dev/null || fail "$1 missing: $2"; }
forbid(){ if grep -F "$2" "$ROOT/$1" >/dev/null; then fail "$1 still contains forbidden text: $2"; fi; }

# Release branding must be generated from the current version rather than a stale splash literal.
need CMakeLists.txt 'project(WaffleHouseClient VERSION 3.3.1 LANGUAGES CXX)'
need CMakeLists.txt 'APP_VERSION_STRING="3.3r1"'
need src/appbranding.h '#define APP_VERSION_STRING "3.3r1"'
need include/trunkmonkey/Version.h '#define WAFFLEHOUSE_SOFTPHONE_VERSION "3.3r1"'
need src/main.cpp 'QStringLiteral("WAFFLEHOUSE-CLIENT — VERSION %1").arg(appVersionString().toUpper())'
need build.sh 'Build WaffleHouse-Client 3.3r1, the unified C++ GUI/CLI executable.'
need build.sh 'WAFFLEHOUSE-CLIENT 3.3r1'
forbid src/main.cpp 'VERSION 2.3 ALPHA'

# Secure / unsecured transfer selection and dedicated non-CPX wire framing.
[ -s "$ROOT/src/filetransport.h" ] || fail 'unsecured transport wrapper missing'
need src/filetransport.h 'WHFILE1|'
need src/filetransport.h 'wrapUnsecured'
need src/filetransport.h 'unwrapUnsecured'
need src/mainwindow.cpp 'Secure — CPX encrypted and authenticated'
need src/mainwindow.cpp 'Unsecured — ordinary AIM/IRC private-message transport'
need src/mainwindow.cpp 'Nothing will be sent until the secure session is established.'
need src/mainwindow.cpp 'm_fileTransferSecure.insert(transferId, secureTransfer);'
need src/mainwindow.cpp 'WaffleFileTransport::wrapUnsecured(plaintext)'
need src/mainwindow.cpp 'WaffleFileTransport::unwrapUnsecured(payload, filePayload)'
need src/mainwindow.cpp 'SHA-256 verification enabled'

# CLI /sendfile must be guided, with a real file browser and a security toggle.
need src/terminalui.cpp 'promptFileTransfer(entry, target, path, secureTransfer)'
need src/terminalui.cpp 'QString TerminalUi::browseFile'
need src/terminalui.cpp 'KEY_F(2)'
need src/terminalui.cpp 'Secure transfer'
need src/terminalui.cpp 'Space toggles secure'
need src/terminalui.cpp 'WaffleFileTransport::wrapUnsecured(plaintext)'
need src/terminalui.cpp 'WaffleFileTransport::unwrapUnsecured(payload, filePayload)'
need src/terminalui.cpp '/sendfile                    open guided recipient/file/security dialog'

# Footer is outside the main pane and theme-aware for every palette.
need src/terminalui.cpp 'constexpr int PairFooter = 7;'
need src/terminalui.cpp 'footer = unread;'
need src/terminalui.cpp 'init_pair(PairFooter, footer, background);'
need src/terminalui.cpp 'safeAdd(height - 4, 0, QString(width, QChar(0x2500))'
need src/terminalui.cpp 'drawShortcutHint(height - 3, width);'
need src/terminalui.cpp 'COLOR_PAIR(PairFooter)'

# Softphone uses a left rail and a phone-like main page while retaining existing workspaces.
need src/softphonewindow.cpp 'sidebar->setObjectName(QStringLiteral("Sidebar"))'
need src/softphonewindow.cpp 'm_tabs->tabBar()->hide();'
need src/softphonewindow.cpp 'QStringLiteral("  Phone")'
need src/softphonewindow.cpp 'QStringLiteral("  Active Calls")'
need src/softphonewindow.cpp 'QStringLiteral("  SIP Log")'
need src/softphonewindow.cpp 'QStringLiteral("  SIP Ladder")'
need src/softphonewindow.cpp 'QStringLiteral("  Profile")'
need src/softphonewindow.cpp 'QStringLiteral("  Activity")'
need src/softphonewindow.cpp 'm_phoneStatus = new QLabel'
need src/softphonewindow.cpp 'dialGrid->addWidget(new QLabel(QStringLiteral("Caller ID:")'
need src/softphonewindow.cpp 'dialGrid->addWidget(m_callerId, 1, 1, 1, 3);'
need src/softphonewindow.cpp 'dialGrid->addWidget(new QLabel(QStringLiteral("Prefix:")'
need src/softphonewindow.cpp 'dialGrid->addWidget(m_runtimeDialPrefix, 2, 1);'
need src/softphonewindow.cpp 'dialGrid->addWidget(new QLabel(QStringLiteral("Destination:")'
need src/softphonewindow.cpp 'dialGrid->addWidget(m_destination, 2, 3);'
need src/softphonewindow.cpp 'const QStringList digits'
need src/softphonewindow.cpp 'const QStringList dialLabels'
need src/softphonewindow.cpp 'QStringLiteral("2\nABC")'
need src/softphonewindow.cpp 'key->setProperty("dialKey", true)'
need src/softphonewindow.cpp 'key->setFixedSize(64, 64)'
need src/softphonewindow.cpp 'QStringLiteral("CALL")'
need src/softphonewindow.cpp 'QStringLiteral("HANG UP")'
need src/softphonewindow.cpp 'm_controller->sendDtmf(liveId, digit, &error)'
need src/modernstyle.cpp 'QPushButton[dialKey="true"]'
need src/softphonewindow.cpp 'resize(740, 550);'
need src/mainwindow.cpp 'resize(680, 520);'
need src/mainwindow.cpp 'setMinimumSize(560, 420);'
need src/mainwindow.cpp 'm_connectionsWindow->resize(620, 430);'
need src/chatwindow.cpp 'QStringLiteral("chat") ? 560 : 480, 360'
need src/transferwindow.cpp 'resize(600, 380);'
need src/modernstyle.cpp 'border-radius: 32px;'
need src/modernstyle.cpp 'QPushButton[phoneAction="call"]'
need src/modernstyle.cpp 'QPushButton[phoneAction="hangup"]'

# Previously-added notification customization must survive the r1 work.
need src/mainwindow.cpp 'Choose Notification Sound'
need src/terminalui.cpp 'QStringLiteral("/soundtest")'

# Unsecured mode must not mutate the proven chunk/hash engine itself.
sha_line=$(grep '  src/filetransfer.cpp$' "$ROOT/tests/core-2.5.4-r6.sha256" || true)
[ -n "$sha_line" ] || fail 'filetransfer.cpp preservation hash missing'
expected=$(printf '%s\n' "$sha_line" | awk '{print $1}')
if command -v sha256sum >/dev/null 2>&1; then actual=$(sha256sum "$ROOT/src/filetransfer.cpp" | awk '{print $1}'); else actual=$(sha256 -q "$ROOT/src/filetransfer.cpp"); fi
[ "$actual" = "$expected" ] || fail 'underlying file-transfer engine changed unexpectedly'

echo 'WaffleHouse 3.1 secure/unsecured transfer + footer + softphone regression: PASS'
