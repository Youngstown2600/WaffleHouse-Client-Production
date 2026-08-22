#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
python3 - "$ROOT/src/ircbackend.cpp" <<'PY2'
from pathlib import Path
import sys
s=Path(sys.argv[1]).read_text()
needle='''if (socket->state() != QAbstractSocket::UnconnectedState) {
            socket->disconnectFromHost();
            if (socket->state() != QAbstractSocket::UnconnectedState) {
                socket->waitForDisconnected(500);'''
if needle not in s:
    raise SystemExit('IRC disconnect state guard is missing')
# Ensure the old unconditional sequence is not present.
old='''socket->disconnectFromHost();
        socket->waitForDisconnected(500);'''
if old in s:
    raise SystemExit('unconditional IRC waitForDisconnected sequence still present')
print('irc_disconnect_state_guard_test: PASS')
PY2
