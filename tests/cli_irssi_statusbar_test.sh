#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
TERM="$ROOT/src/terminalui.cpp"
HDR="$ROOT/src/terminalui.h"

fail() { echo "FAIL: $*" >&2; exit 1; }

# The CLI should reserve the bottom two rows for an Irssi-style status bar
# followed immediately by the input prompt.
grep -F 'void TerminalUi::drawStatusBar(int row, int width)' "$TERM" >/dev/null \
    || fail 'drawStatusBar implementation missing'
grep -F 'void drawStatusBar(int row, int width);' "$HDR" >/dev/null \
    || fail 'drawStatusBar declaration missing'
grep -F 'drawStatusBar(height - 2, width);' "$TERM" >/dev/null \
    || fail 'status bar is not anchored one row above input'
grep -F 'drawInputLine(height - 1, width);' "$TERM" >/dev/null \
    || fail 'input line is not on the terminal bottom row'
! grep -Fq 'drawFooter(height - 1, width);' "$TERM" \
    || fail 'old footer still occupies the bottom row'

grep -F 'QDateTime::currentDateTime().toString(QStringLiteral("HH:mm"))' "$TERM" >/dev/null \
    || fail 'status bar clock missing'
grep -F 'connectionLabel(entry)' "$TERM" >/dev/null \
    || fail 'status bar connection context missing'
grep -F 'buffer->number' "$TERM" >/dev/null \
    || fail 'status bar buffer number missing'
grep -F 'A_REVERSE | A_BOLD' "$TERM" >/dev/null \
    || fail 'status bar does not use full-width highlighted styling'

echo 'PASS: Irssi-style CLI status bar is anchored directly above the prompt'
