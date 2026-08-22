#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT"
fail(){ echo "WaffleHouse 3.3 compact GUI cleanup failed: $*" >&2; exit 1; }
need(){ grep -F "$2" "$1" >/dev/null || fail "$1 missing: $2"; }
forbid(){ if grep -F "$2" "$1" >/dev/null; then fail "$1 still contains: $2"; fi; }
need src/mainwindow.cpp 'resize(680, 520);'
need src/mainwindow.cpp 'setMinimumSize(560, 420);'
need src/mainwindow.cpp 'm_connectionsWindow->resize(620, 430);'
need src/softphonewindow.cpp 'resize(740, 550);'
need src/chatwindow.cpp 'QStringLiteral("chat") ? 560 : 480, 360'
need src/transferwindow.cpp 'resize(600, 380);'
forbid src/mainwindow.cpp 'QStringLiteral("Quick Actions")'
forbid src/mainwindow.cpp 'setObjectName(QStringLiteral("Sidebar"))'
forbid src/mainwindow.cpp 'auto *navDashboard'
forbid src/mainwindow.cpp 'auto *navMessages'
forbid src/mainwindow.cpp 'auto *navSoftphone'
forbid src/mainwindow.cpp 'auto *navConnections'
forbid src/mainwindow.cpp 'auto *navTransfers'
forbid src/mainwindow.cpp 'auto *navSettings'
need src/mainwindow.cpp 'auto *title = new QLabel(QStringLiteral("WAFFLEHOUSE-CLIENT"), topBar);'
need src/mainwindow.cpp 'QStringLiteral("VERSION %1").arg(appVersionString().toUpper())'
need src/mainwindow.cpp 'auto *accountTitle = new QLabel(QStringLiteral("Accounts"), accountCard);'
need src/mainwindow.cpp 'm_commandInput = new QLineEdit(accountCard);'
need src/mainwindow.cpp 'qOverload<>(&MainWindow::executeGuiCommand)'
need src/mainwindow.cpp 'showAccountContextMenu'
need src/mainwindow.cpp 'showBuddyContextMenu'
echo 'WaffleHouse 3.3 compact GUI + command-bar regression: PASS'
