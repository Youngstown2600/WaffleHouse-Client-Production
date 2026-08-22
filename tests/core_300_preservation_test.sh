#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
MANIFEST="$ROOT/tests/core-2.5.4-r6.sha256"

hash_file() {
    if command -v sha256sum >/dev/null 2>&1; then
        sha256sum "$1" | awk '{print $1}'
    elif command -v sha256 >/dev/null 2>&1; then
        sha256 -q "$1"
    else
        echo "No SHA-256 utility available" >&2
        exit 2
    fi
}

# 3.0r1 intentionally changed the presentation/controller files below.
# 3.0r2 additionally changes only the AIM/IRC backend surfaces needed for the
# peer-version query/response protocol. This secure-room rebuild also changes the
# chat-window surface and CPX capability advertisement while adding a separate
# secure-room crypto module. These documented deltas are covered by
# tests/secure_room_31_test.sh; file-transfer, Telnet/BBS, ANSI terminal, SIP,
# and all remaining protocol/service files stay byte-for-byte pinned. Version 3.3
# intentionally changed only the SIP fallback display label in sipbackend.cpp.
# Version 3.3r1 additionally extends oscarprotocol.h with documented LOCATE
# profile/capability constants; the behavior is guarded by the dedicated 3.3r1 tests.
is_r1_frontend_file() {
    case "$1" in
        src/mainwindow.h|src/softphonewindow.cpp|src/softphonewindow.h|src/terminalui.h|src/chatwindow.cpp|src/chatwindow.h|src/securechannel.cpp|src/transferwindow.cpp|src/ircbackend.cpp|src/ircbackend.h|src/oscarbackend.cpp|src/oscarbackend.h|src/oscarprotocol.h) return 0 ;;
        *) return 1 ;;
    esac
}

checked=0
skipped=0
while IFS='  ' read -r expected rel; do
    [ -n "$expected" ] || continue
    rel=${rel# }
    if is_r1_frontend_file "$rel"; then
        skipped=$((skipped + 1))
        continue
    fi
    actual=$(hash_file "$ROOT/$rel")
    if [ "$actual" != "$expected" ]; then
        echo "FAIL: preserved core file changed: $rel" >&2
        exit 1
    fi
    checked=$((checked + 1))
done < "$MANIFEST"

[ "$checked" -ge 32 ] || { echo "FAIL: preservation gate unexpectedly small ($checked files)" >&2; exit 1; }
[ "$skipped" -eq 13 ] || { echo "FAIL: expected thirteen documented frontend/protocol exceptions through 3.3r1; got $skipped" >&2; exit 1; }
echo "WaffleHouse-Client 3.3r1 preserved protocol/file-transfer/SIP/BBS core: PASS ($checked pinned, $skipped documented exceptions)"
