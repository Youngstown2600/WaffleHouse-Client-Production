#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT"
fail(){ echo "FAIL: $*" >&2; exit 1; }
need(){ grep -F "$2" "$1" >/dev/null || fail "$1 missing: $2"; }
forbid(){ if grep -F "$2" "$1" >/dev/null; then fail "$1 contains forbidden text: $2"; fi; }

need CMakeLists.txt 'project(WaffleHouseClient VERSION 3.3.1 LANGUAGES CXX)'
need CMakeLists.txt 'src/modernstyle.cpp'
need CMakeLists.txt 'APP_VERSION_STRING="3.3r1"'

# 3.3 compact main shell: no redundant navigation rail; identity + accounts + command bar.
forbid src/mainwindow.cpp 'setObjectName(QStringLiteral("Sidebar"))'
forbid src/mainwindow.cpp 'QStringLiteral("Communications Hub")'
forbid src/mainwindow.cpp 'QStringLiteral("Accounts & Buddies")'
forbid src/mainwindow.cpp 'auto *navSoftphone = makeNav'
forbid src/mainwindow.cpp 'auto *navSettings = new QPushButton'
forbid src/mainwindow.cpp 'auto *addConnection = new QPushButton(QStringLiteral("+ New Connection"), sidebar);'
need src/mainwindow.cpp 'QStringLiteral("WAFFLEHOUSE-CLIENT")'
need src/mainwindow.cpp 'QStringLiteral("VERSION %1").arg(appVersionString().toUpper())'
need src/mainwindow.cpp 'auto *accountTitle = new QLabel(QStringLiteral("Accounts"), accountCard);'
need src/mainwindow.cpp 'm_commandInput = new QLineEdit(accountCard);'
need src/mainwindow.cpp 'void MainWindow::executeGuiCommand(const QString &input)'
need src/mainwindow.cpp 'QMenu *management = m_accountsMenu->addMenu(QStringLiteral("Account Management"));'
need src/mainwindow.cpp 'toolsMenu->addAction(m_importBbsAction);'
need src/mainwindow.cpp 'QStringLiteral("File Transfer &Log / Activity…")'
need src/mainwindow.cpp 'QStringLiteral("Show Media Center")'
if grep -F 'SoftphoneCard' src/mainwindow.cpp >/dev/null; then fail 'main dashboard must not contain a Softphone card'; fi
need src/mainwindow.cpp 'Open &Softphone…'
need src/mainwindow.cpp 'ModernStyle::styleSheet(m_options.theme)'

# Shared styling remains available for dedicated windows such as Softphone.
need src/modernstyle.cpp 'QPushButton[nav="true"]'
need src/modernstyle.cpp 'QFrame#Card'

# CLI/TUI modern shell remains intact.
need src/terminalui.cpp 'WAFFLEHOUSE-CLIENT %1 CLI%2'
need src/terminalui.cpp 'QStringLiteral("❯ ")'
need src/terminalui.cpp 'Tab completes /commands | Ctrl-N/P buffers | Alt-1..9/F1..F9 jump | PgUp/PgDn scroll |'
need src/terminalui.cpp 'safeAdd(height - 4, 0, QString(width, QChar(0x2500))'
need src/terminalui.cpp 'drawShortcutHint(height - 3, width);'
need src/terminalui.cpp 'drawStatusBar(height - 2, width);'
need src/terminalui.cpp 'drawInputLine(height - 1, width);'
need src/terminalui.cpp 'COLOR_PAIR(PairFooter)'
need src/terminalui.cpp 'footer = unread;'

echo 'WaffleHouse-Client 3.3r1 modern GUI/TUI shell regression: PASS'
