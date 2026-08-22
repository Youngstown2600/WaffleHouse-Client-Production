#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
fail(){ echo "Media Center 3.3 regression failed: $*" >&2; exit 1; }
need(){ grep -Fq -- "$2" "$ROOT/$1" || fail "$1 missing: $2"; }
forbid(){ if grep -Fqi -- "$2" "$ROOT/$1"; then fail "$1 contains forbidden text: $2"; fi; }
need_file(){ [ -f "$ROOT/$1" ] || fail "missing $1"; }

need CMakeLists.txt 'project(WaffleHouseClient VERSION 3.3.1 LANGUAGES CXX)'
need CMakeLists.txt 'APP_VERSION_STRING="3.3r1"'
need src/appbranding.h '#define APP_VERSION_STRING "3.3r1"'
need include/trunkmonkey/Version.h '#define WAFFLEHOUSE_SOFTPHONE_VERSION "3.3r1"'
need_file src/mediacontroller.h
need_file src/mediacontroller.cpp
need_file src/mediawindow.h
need_file src/mediawindow.cpp

# Private mpv IPC, safe startup, and explicit disabling of mpv's ytdl hook.
need src/mediacontroller.cpp 'QStringLiteral("--no-config")'
need src/mediacontroller.cpp 'QStringLiteral("--ytdl=no")'
need src/mediacontroller.cpp 'QStringLiteral("--load-unsafe-playlists=no")'
need src/mediacontroller.cpp 'QStandardPaths::RuntimeLocation'
need src/mediacontroller.cpp '::socket(AF_UNIX, SOCK_STREAM, 0)'
need src/mediacontroller.cpp 'QSocketNotifier::Read'
need src/mediacontroller.cpp 'm_pendingRequests.insert'
forbid src/mediacontroller.cpp 'QLocalSocket'

# Queue and transport behavior.
need src/mediacontroller.cpp 'QStringLiteral("append-play")'
need src/mediacontroller.cpp 'QStringLiteral("playlist-play-index")'
need src/mediacontroller.cpp 'QStringLiteral("playlist-remove")'
need src/mediacontroller.cpp 'QStringLiteral("playlist-clear")'
need src/mediacontroller.cpp 'QStringLiteral("stop"), QStringLiteral("keep-playlist")'
need src/mediacontroller.cpp 'QStringLiteral("play first queued item after stop")'
need src/mediawindow.cpp 'if (m_media->idle())'
need src/mediawindow.cpp 'syncPlaylist'

# Supported media/radio surfaces remain.
need src/mediawindow.cpp 'SHOUTcast/Icecast'
need src/mediawindow.cpp 'HTTP/HLS streams'
need src/mediawindow.cpp 'SHOUTcast / Icecast / HTTP(S) / HLS media URL:'
need src/mediawindow.cpp 'M3U / PLS playlist URL'
need src/mediacontroller.cpp 'QStringLiteral("loadlist")'
need src/main.cpp 'Open Stream / Radio URL…'
need src/main.cpp 'Open Internet Playlist URL…'
for command in media mstatus mplay mstream mshoutcast menqueue mplaylist mpause mresume mstop mnext mprev mseek mvolume mmute mshuffle mrepeat meq; do
    need src/terminalui.cpp "QStringLiteral(\"/$command\")"
done

# Removed feature must not survive in executable source or builder.
for file in src/mediacontroller.cpp src/mediacontroller.h src/mediawindow.cpp src/mediawindow.h src/main.cpp src/terminalui.cpp build.sh; do
  forbid "$file" 'yt-dlp'
  forbid "$file" '/myoutube'
  forbid "$file" 'YouTube Audio'
done
forbid build.sh 'deno'

need build.sh 'check_command_dep "WaffleHouse-Client media backend" mpv mpv'
need build.sh 'check_command_dep "WaffleHouse-Client FFmpeg tools" ffmpeg ffmpeg'
need build.sh 'Testing-safe mode: build first; system installation will be offered only after a successful build (default: No).'
need src/chatwindow.cpp 'commands << IrcBackend::slashCommands();'
need src/terminalui.cpp 'if (irc->handleSlashCommand(roomContext, line)) return;'
need src/mainwindow.cpp 'QStringLiteral("WAFFLEHOUSE-CLIENT")'
need src/mainwindow.cpp 'QStringLiteral("Show Media Center")'
need src/mainwindow.cpp 'm_mediaWindow->executeCommand(command, rest, &mediaMessage)'
need src/secureroom.cpp 'CPXROOM1'

echo 'WaffleHouse-Client 3.3r1 media/stream/removal regression: PASS'
