#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
fail(){ echo "Media Center 3.3 YouTube-removal regression failed: $*" >&2; exit 1; }
for file in src/mediacontroller.cpp src/mediacontroller.h src/mediawindow.cpp src/mediawindow.h src/main.cpp src/terminalui.cpp build.sh; do
  for term in 'yt-dlp' 'yt_dlp' 'queueYouTubeResolve' 'startNextYouTubeResolve' 'openYouTubeDialog' '/myoutube' 'YouTube Audio'; do
    if grep -Fqi "$term" "$ROOT/$file"; then fail "$file still contains removed YouTube feature text: $term"; fi
  done
done
grep -Fq 'QStringLiteral("--ytdl=no")' "$ROOT/src/mediacontroller.cpp" || fail 'mpv ytdl hook is not explicitly disabled'
echo 'WaffleHouse-Client 3.3r1 YouTube feature removal: PASS'
