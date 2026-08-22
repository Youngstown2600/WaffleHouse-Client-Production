#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
fail() { echo "notification sounds 3.0 regression failed: $*" >&2; exit 1; }

[ -f "$ROOT/src/notificationmanager.cpp" ] || fail "shared notification manager missing"
[ -f "$ROOT/src/notificationmanager.h" ] || fail "notification manager header missing"
for sound in mention.wav message.wav aim-im.wav aim-chat.wav complete.wav warning.wav; do
  [ -s "$ROOT/sounds/$sound" ] || fail "built-in sound missing: $sound"
done

grep -q 'notifications/events/%1/' "$ROOT/src/notificationmanager.cpp" || fail "shared QSettings event store missing"
grep -q 'classifyIncoming' "$ROOT/src/mainwindow.cpp" || fail "GUI incoming notification classification missing"
grep -q 'classifyIncoming' "$ROOT/src/terminalui.cpp" || fail "CLI incoming notification classification missing"
grep -q 'IRC channel mention' "$ROOT/src/notificationmanager.cpp" || fail "IRC mention event missing"
grep -q 'AIM chat message' "$ROOT/src/notificationmanager.cpp" || fail "AIM chat event missing"
grep -q 'Notification Sounds' "$ROOT/src/mainwindow.cpp" || fail "GUI notification settings missing"
grep -q 'Choose Notification Sound' "$ROOT/src/mainwindow.cpp" || fail "GUI custom sound picker missing"
grep -q 'QStringLiteral("/notifications")' "$ROOT/src/terminalui.cpp" || fail "CLI /notifications missing"
grep -q 'QStringLiteral("/soundtest")' "$ROOT/src/terminalui.cpp" || fail "CLI /soundtest missing"
grep -q 'install(DIRECTORY sounds/' "$ROOT/CMakeLists.txt" || fail "sound asset install rule missing"
grep -q 'Notification sound player.*paplay' "$ROOT/build.sh" || fail "paplay builder preflight missing"

echo "notification sounds 3.0 regression passed"
