#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT"
fail(){ echo "WaffleHouse 3.1 presence/version regression failed: $*" >&2; exit 1; }
need(){ grep -Fq "$2" "$1" || fail "$1 missing: $2"; }

# Release branding.
need CMakeLists.txt 'project(WaffleHouseClient VERSION 3.1.0 LANGUAGES CXX)'
need CMakeLists.txt 'APP_VERSION_STRING="3.1"'
need src/appbranding.h '#define APP_VERSION_STRING "3.1"'
need include/trunkmonkey/Version.h '#define WAFFLEHOUSE_SOFTPHONE_VERSION "3.1"'
need include/trunkmonkey/Version.h 'WaffleHouse-Client/3.1'

# IRC uses standard CTCP VERSION and keeps the control traffic out of chat.
need src/ircbackend.h 'void requestClientVersion(const QString &target);'
need src/ircbackend.cpp 'PRIVMSG %1 :\x01VERSION\x01'
need src/ircbackend.cpp 'QStringLiteral("version-request")'
need src/ircbackend.cpp 'QStringLiteral("version")'
need src/mainwindow.cpp 'VERSION WaffleHouse-Client %1'
need src/terminalui.cpp 'VERSION WaffleHouse-Client %1'

# AIM uses WaffleHouse-private hidden version frames and legacy CPX hints.
need src/oscarbackend.h 'void requestClientVersion(const QString &target);'
need src/oscarbackend.cpp 'QStringLiteral("[[WHVER:Q]]")'
need src/oscarbackend.cpp 'QStringLiteral("[[WHVER:R:%1]]")'
need src/oscarbackend.cpp 'Legacy CPX3-compatible client detected; exact WaffleHouse-Client version unavailable'
need src/oscarbackend.cpp 'doSendIm(command.a, command.b, !command.flag);'

# /version is available in both frontends and has a legacy/no-response result.
need src/chatwindow.cpp 'QStringLiteral("/version")'
need src/mainwindow.cpp 'command.startsWith(QStringLiteral("/version "))'
need src/terminalui.cpp 'QStringLiteral("/version")'
need src/terminalui.cpp 'command == QStringLiteral("version")'
need src/mainwindow.cpp 'no 3.1 reply; peer may be an older WaffleHouse/CPX client'
need src/terminalui.cpp 'no 3.1 reply; peer may be an older WaffleHouse/CPX client'

# Automatic OSCAR Idle -> Away -> Online uses shared GUI/CLI settings.
need src/mainwindow.h 'bool autoPresenceEnabled = true;'
need src/mainwindow.h 'int autoIdleMinutes = 5;'
need src/mainwindow.h 'int autoAwayMinutes = 15;'
need src/mainwindow.cpp 'presence/autoEnabled'
need src/mainwindow.cpp 'presence/idleMinutes'
need src/mainwindow.cpp 'presence/awayMinutes'
need src/mainwindow.cpp 'qApp->installEventFilter(this);'
need CMakeLists.txt 'src/useractivity.cpp'
need src/useractivity.cpp 'xprintidle'
need src/mainwindow.cpp 'UserActivity::idleMilliseconds'
need src/terminalui.cpp 'UserActivity::idleMilliseconds'
need src/mainwindow.cpp '&MainWindow::updateAutoPresence'
need src/mainwindow.cpp 'oscar->setIdleSeconds'
need src/mainwindow.cpp 'oscar->setAwayMessage'
need src/mainwindow.cpp 'oscar->setBack();'
need src/mainwindow.cpp 'Respect manual Away/AFK/Idle.'
need src/terminalui.cpp 'settings.beginGroup(QStringLiteral("presence"));'
need src/terminalui.cpp 'updateAutoPresence();'
need src/terminalui.cpp 'markUserActivity();'
need src/terminalui.cpp 'QStringLiteral("/autopresence")'
need src/terminalui.cpp 'Auto OSCAR presence: idle after %1 min, away after %2 min.'

echo 'WaffleHouse 3.1 automatic OSCAR presence + peer version query regression: PASS'
