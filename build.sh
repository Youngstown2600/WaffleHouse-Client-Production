#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
SELECTED_OS=
ARGS_FILE="${TMPDIR:-/tmp}/wafflehouse-all-platform-args-$$"
: > "$ARGS_FILE"
trap 'rm -f "$ARGS_FILE"' EXIT HUP INT TERM

usage(){
cat <<'EOF'
WaffleHouse-Client 5.0r10 — all-platform builder

Usage: ./build.sh [--os linux|freebsd|macos|termux] [platform options]

With no --os option, choose:
  1) Linux
  2) FreeBSD / Unix
  3) macOS
  4) Termux / Android

The desktop branch builds WaffleHouse-Client 5.0r10.
The Termux branch builds WaffleHouse-Client-Termux Build 1.2.

Application installation is not automatic. Desktop builders and the Termux
builder ask before adding WaffleHouse-Client to their normal bin locations.
Pressing Enter at an install prompt means No.

Examples:
  ./build.sh
  ./build.sh --os linux --clean
  ./build.sh --os freebsd --pjsip
  ./build.sh --os macos --clean --dmg
  ./build.sh --os termux --clean
  ./build.sh --os termux --install
EOF
}

while [ "$#" -gt 0 ]; do
  case "$1" in
    --os)
      shift
      [ "$#" -gt 0 ] || { echo "--os requires linux, freebsd, macos, or termux" >&2; exit 2; }
      SELECTED_OS=$(printf '%s' "$1" | tr '[:upper:]' '[:lower:]')
      ;;
    --os=*) SELECTED_OS=$(printf '%s' "${1#--os=}" | tr '[:upper:]' '[:lower:]') ;;
    -h|--help) usage; exit 0 ;;
    *) printf '%s\n' "$1" | sed "s/'/'\\''/g; s/^/'/; s/$/'/" >> "$ARGS_FILE" ;;
  esac
  shift
done

case "$SELECTED_OS" in
  linux|freebsd|macos|darwin|termux|android|'') : ;;
  unix) SELECTED_OS=freebsd ;;
  *) echo "Unknown --os value: $SELECTED_OS" >&2; usage >&2; exit 2 ;;
esac
[ "$SELECTED_OS" != darwin ] || SELECTED_OS=macos
[ "$SELECTED_OS" != android ] || SELECTED_OS=termux

if [ -z "$SELECTED_OS" ]; then
  if [ ! -t 0 ]; then
    echo "No interactive terminal is available. Re-run with --os linux, freebsd, macos, or termux." >&2
    exit 2
  fi
  cat <<'EOF'
============================================================
            WAFFLEHOUSE-CLIENT ALL PLATFORMS
============================================================
What operating system are you installing on?

  1) Linux
  2) FreeBSD / Unix
  3) macOS
  4) Termux / Android
EOF
  printf 'Selection [1-4]: '
  IFS= read -r answer
  case "$answer" in
    1|linux|Linux) SELECTED_OS=linux ;;
    2|freebsd|FreeBSD|unix|Unix) SELECTED_OS=freebsd ;;
    3|macos|macOS|MacOS|darwin|Darwin) SELECTED_OS=macos ;;
    4|termux|Termux|android|Android) SELECTED_OS=termux ;;
    *) echo "Invalid selection." >&2; exit 2 ;;
  esac
fi

HOST_OS=$(uname -s 2>/dev/null || echo unknown)
case "$SELECTED_OS" in
  linux)
    [ "$HOST_OS" = Linux ] || { echo "Linux selected, but host reports $HOST_OS." >&2; exit 2; }
    TARGET="$ROOT/desktop/build.sh --os linux"
    ;;
  freebsd)
    [ "$HOST_OS" = FreeBSD ] || { echo "FreeBSD / Unix selected, but validated Unix target requires a FreeBSD host (host reports $HOST_OS)." >&2; exit 2; }
    TARGET="$ROOT/desktop/build.sh --os freebsd"
    ;;
  macos)
    [ "$HOST_OS" = Darwin ] || { echo "macOS selected, but host reports $HOST_OS." >&2; exit 2; }
    TARGET="$ROOT/desktop/build.sh --os macos"
    ;;
  termux)
    [ "$HOST_OS" = Linux ] || { echo "Termux selected, but host reports $HOST_OS." >&2; exit 2; }
    case "${PREFIX:-}" in
      /data/data/com.termux/files/usr) : ;;
      *) echo "Termux selected, but this is not a native Termux environment (PREFIX=${PREFIX:-unset})." >&2; exit 2 ;;
    esac
    TARGET="$ROOT/termux/build.sh"
    ;;
esac

if [ -s "$ARGS_FILE" ]; then
  eval "set -- $(tr '\n' ' ' < "$ARGS_FILE")"
else
  set --
fi

printf '\nSelected platform: %s (host: %s)\n\n' "$SELECTED_OS" "$HOST_OS"
case "$SELECTED_OS" in
  linux) exec "$ROOT/desktop/build.sh" --os linux "$@" ;;
  freebsd) exec "$ROOT/desktop/build.sh" --os freebsd "$@" ;;
  macos) exec "$ROOT/desktop/build.sh" --os macos "$@" ;;
  termux) exec "$ROOT/termux/build.sh" "$@" ;;
esac
