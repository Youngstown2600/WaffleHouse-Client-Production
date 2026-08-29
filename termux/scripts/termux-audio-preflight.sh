#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail

PREFIX=${PREFIX:-/data/data/com.termux/files/usr}
TMPDIR=${TMPDIR:-$PREFIX/tmp}
MODE=manual
case "${1:-}" in
  "") ;;
  --startup) MODE=startup ;;
  --help|-h)
    cat <<'HELP'
Usage: wafflehouse-audio-preflight [--startup]

Without options, run the interactive/manual WaffleHouse microphone preflight.
--startup is used internally by WaffleHouse-Client-Termux: successful checks are
silent, while missing Android microphone permission/add-on errors are explained.
HELP
    exit 0
    ;;
  *)
    echo "Unknown option: $1" >&2
    exit 64
    ;;
esac

mkdir -p "$TMPDIR"
probe=$(mktemp "$TMPDIR/wafflehouse-mic-probe.XXXXXX.m4a")
cleanup(){ rm -f "$probe" 2>/dev/null || true; }
trap cleanup EXIT

if ! command -v termux-microphone-record >/dev/null 2>&1; then
  echo "WaffleHouse microphone check: the termux-api command package is missing." >&2
  echo "Run: pkg install termux-api" >&2
  echo "Then install the matching Termux:API Android add-on and grant Microphone permission." >&2
  exit 2
fi

if [[ "$MODE" == manual ]]; then
  cat <<'MSG'
WaffleHouse-Client-Termux audio preflight
----------------------------------------
This test asks Android for one second of microphone access. If Android displays
an audio permission dialog, choose Allow while using the app.

IMPORTANT: The Termux:API Android add-on must be installed from the SAME source
and signing family as the main Termux app (for example F-Droid + F-Droid).
MSG
fi

# Starting a one-second recording is the reliable permission/capability probe.
# When permission has never been granted Android/Termux:API will surface its
# normal permission dialog here. In startup mode output stays quiet on success.
set +e
out=$(termux-microphone-record -f "$probe" -l 1 -e aac -r 16000 -c 1 2>&1)
rc=$?
set -e

settle=${WAFFLEHOUSE_PREFLIGHT_SETTLE_SECONDS:-2}
if [[ $rc -eq 0 ]]; then
  sleep "$settle"
fi

if [[ $rc -eq 0 && -s "$probe" ]]; then
  if [[ "$MODE" == manual ]]; then
    printf '%s\n' "$out"
    bytes=$(wc -c < "$probe" | tr -d ' ')
    echo "Microphone preflight PASS: Android capture produced $bytes bytes."
  fi
  exit 0
fi

# Startup failures are intentionally visible: this is the point where the user
# needs to grant Android permission or install the matching Termux:API add-on.
[[ -n "$out" ]] && printf '%s\n' "$out" >&2
echo >&2
echo "WaffleHouse microphone permission/capture check failed." >&2
echo "If Android displays a Microphone permission dialog, choose Allow while using the app." >&2
echo "If no dialog appears, verify the Termux:API Android add-on is installed from the SAME source/signing family as Termux." >&2
echo "You can also grant it manually: Android Settings -> Apps -> Termux:API -> Permissions -> Microphone." >&2
if [[ "$MODE" == startup ]]; then
  echo "WaffleHouse will continue; SIP remains usable and will fall back to receive-only/null audio if capture is unavailable." >&2
  echo "Manual retest: wafflehouse-audio-preflight" >&2
else
  echo "Then rerun: wafflehouse-audio-preflight" >&2
fi
exit $(( rc == 0 ? 1 : rc ))
