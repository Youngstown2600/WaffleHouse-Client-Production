#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
fail(){ echo "Media Center 3.3 behavior regression failed: $*" >&2; exit 1; }
need(){ grep -Fq "$2" "$ROOT/$1" || fail "$1 missing: $2"; }
forbid(){ if grep -Fqi "$2" "$ROOT/$1"; then fail "$1 contains forbidden text: $2"; fi; }
need CMakeLists.txt 'project(WaffleHouseClient VERSION 3.3.1 LANGUAGES CXX)'
need CMakeLists.txt 'APP_VERSION_STRING="3.3r1"'
need src/mediacontroller.cpp 'QStringLiteral("stop"), QStringLiteral("keep-playlist")'
need src/mediacontroller.cpp 'QStringLiteral("play first queued item after stop")'
need src/mediawindow.cpp 'if (m_media->idle())'
need src/mediacontroller.cpp 'QStringLiteral("--ytdl=no")'
need src/mediawindow.cpp 'SHOUTcast Search'
need src/mediawindow.cpp 'directory.shoutcast.com/Search?query='
need src/terminalui.cpp 'QStringLiteral("/mshoutcast")'
need src/terminalui.cpp 'Usage: /mshoutcast SEARCH-TERMS'
need src/mediacontroller.cpp 'event == QStringLiteral("end-file")'
forbid src/main.cpp 'YouTube Audio'
forbid src/mediawindow.cpp 'YouTube Audio'
forbid src/terminalui.cpp '/myoutube'
forbid src/mediacontroller.cpp 'yt-dlp'
forbid build.sh 'yt-dlp'
forbid build.sh 'deno'
echo 'WaffleHouse-Client 3.3r1 Stop/stream/YouTube-removal regression: PASS'
