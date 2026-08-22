#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
fail(){ echo "WaffleHouse-Client 3.3r1 media-key regression failed: $*" >&2; exit 1; }
need(){ grep -Fq "$2" "$ROOT/$1" || fail "$1 missing: $2"; }
need src/mainwindow.h 'void installMediaKeyShortcuts();'
need src/mainwindow.cpp 'shortcut->setContext(Qt::ApplicationShortcut);'
need src/mainwindow.cpp 'Qt::Key_MediaPlay'
need src/mainwindow.cpp 'Qt::Key_MediaPause'
need src/mainwindow.cpp 'Qt::Key_MediaTogglePlayPause'
need src/mainwindow.cpp 'Qt::Key_MediaStop'
need src/mainwindow.cpp 'Qt::Key_MediaNext'
need src/mainwindow.cpp 'Qt::Key_MediaPrevious'
need src/mainwindow.cpp 'Qt::Key_VolumeUp'
need src/mainwindow.cpp 'Qt::Key_VolumeDown'
need src/mainwindow.cpp 'Qt::Key_VolumeMute'
need src/mediawindow.cpp 'QStringLiteral("mvolup")'
need src/mediawindow.cpp 'QStringLiteral("mvoldown")'
need src/mediawindow.cpp 'QStringLiteral("mtoggle")'
echo 'WaffleHouse-Client 3.3r1 media-key regression: PASS'
