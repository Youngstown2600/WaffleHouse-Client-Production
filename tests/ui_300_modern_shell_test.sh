#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT"
fail(){ echo "FAIL: $*" >&2; exit 1; }
need(){ grep -F "$2" "$1" >/dev/null || fail "$1 missing: $2"; }

need CMakeLists.txt 'project(WaffleHouseClient VERSION 3.1.0 LANGUAGES CXX)'
need CMakeLists.txt 'src/modernstyle.cpp'
need CMakeLists.txt 'APP_VERSION_STRING="3.1"'
need src/mainwindow.cpp 'setObjectName(QStringLiteral("Sidebar"))'
need src/mainwindow.cpp 'QStringLiteral("Communications Hub")'
need src/mainwindow.cpp 'auto *addConnection = new QPushButton(QStringLiteral("+ New Connection"), sidebar);'
need src/mainwindow.cpp 'auto *navSettings = new QPushButton(QStringLiteral("Settings"), sidebar);'
if grep -F 'UNIFIED CLIENT' src/mainwindow.cpp >/dev/null; then fail 'Unified Client pill must be removed'; fi
if grep -F 'navSettings->setProperty("nav", true)' src/mainwindow.cpp >/dev/null; then fail 'Settings must use normal button styling'; fi
need src/mainwindow.cpp 'QStringLiteral("Accounts & Buddies")'
if grep -F 'QStringLiteral("Quick Actions")' src/mainwindow.cpp >/dev/null; then fail 'redundant Quick Actions card must be removed'; fi
need src/mainwindow.cpp 'QStringLiteral("  Communications")'
need src/mainwindow.cpp 'auto *navSoftphone = makeNav(QStringLiteral("  Softphone"));'
need src/mainwindow.cpp 'connect(navSoftphone, &QPushButton::clicked'
if grep -F 'SoftphoneCard' src/mainwindow.cpp >/dev/null; then fail 'main dashboard must not contain a Softphone card'; fi
need src/mainwindow.cpp 'Open &Softphone…' 
need src/mainwindow.cpp 'ModernStyle::styleSheet(m_options.theme)'
need src/modernstyle.cpp 'QPushButton[nav="true"]'
need src/modernstyle.cpp 'QFrame#Card'
need src/terminalui.cpp 'WAFFLEHOUSE-CLI %1%2'
need src/terminalui.cpp 'QStringLiteral("❯ ")'
need src/terminalui.cpp 'Tab completes /commands | Ctrl-N/P buffers | Alt-1..9/F1..F9 jump | PgUp/PgDn scroll |'
need src/terminalui.cpp 'safeAdd(height - 4, 0, QString(width, QChar(0x2500))'
need src/terminalui.cpp 'drawShortcutHint(height - 3, width);'
need src/terminalui.cpp 'drawStatusBar(height - 2, width);'
need src/terminalui.cpp 'drawInputLine(height - 1, width);'
need src/terminalui.cpp 'COLOR_PAIR(PairFooter)'
need src/terminalui.cpp 'footer = unread;'

echo 'WaffleHouse 3.1 modern GUI/TUI shell regression: PASS'
