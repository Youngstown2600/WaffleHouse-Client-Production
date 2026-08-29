#!/bin/sh
set -eu
ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

usage() {
  cat <<'USAGE'
WaffleHouse-Client 5.0r10 — all-platform builder

Usage:
  ./build.sh
  ./build.sh --platform linux [build options]
  ./build.sh --platform unix [build options]
  ./build.sh --platform macos [build options]
  ./build.sh --platform termux [build options]

Platforms:
  linux       Linux desktop
  unix        FreeBSD / Unix desktop
  macos       macOS desktop
  termux      Android / Termux CLI

With no --platform option, an interactive menu is shown.
All remaining options are forwarded to the selected platform builder.
USAGE
}

PLATFORM=
ARGS_FILE="${TMPDIR:-/tmp}/wafflehouse-all-platform-args-$$"
: > "$ARGS_FILE"
trap 'rm -f "$ARGS_FILE"' EXIT HUP INT TERM

while [ "$#" -gt 0 ]; do
  case "$1" in
    --platform)
      shift
      [ "$#" -gt 0 ] || { echo "--platform requires linux, unix, macos, or termux" >&2; exit 2; }
      PLATFORM=$(printf '%s' "$1" | tr '[:upper:]' '[:lower:]')
      ;;
    --platform=*)
      PLATFORM=$(printf '%s' "${1#--platform=}" | tr '[:upper:]' '[:lower:]')
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      printf '%s\n' "$1" | sed "s/'/'\\''/g; s/^/'/; s/$/'/" >> "$ARGS_FILE"
      ;;
  esac
  shift
done

case "$PLATFORM" in
  linux|unix|freebsd|macos|darwin|termux|'') : ;;
  *) echo "Unknown platform: $PLATFORM" >&2; usage >&2; exit 2 ;;
esac
[ "$PLATFORM" != freebsd ] || PLATFORM=unix
[ "$PLATFORM" != darwin ] || PLATFORM=macos

if [ -z "$PLATFORM" ]; then
  [ -t 0 ] || { echo "No interactive terminal. Use --platform." >&2; exit 2; }
  cat <<'PROMPT'
============================================================
          WAFFLEHOUSE-CLIENT 5.0r10 ALL PLATFORMS
============================================================
Choose the platform to build/install:

  1) Linux
  2) Unix / FreeBSD
  3) macOS
  4) Termux / Android
PROMPT
  printf 'Selection [1-4]: '
  IFS= read -r answer
  case "$answer" in
    1|linux|Linux) PLATFORM=linux ;;
    2|unix|Unix|freebsd|FreeBSD) PLATFORM=unix ;;
    3|macos|macOS|MacOS|darwin|Darwin) PLATFORM=macos ;;
    4|termux|Termux|android|Android) PLATFORM=termux ;;
    *) echo "Invalid selection." >&2; exit 2 ;;
  esac
fi

case "$PLATFORM" in
  linux)
    DIR="$ROOT_DIR/Linux"
    CMD="./build.sh --os linux"
    ;;
  unix)
    DIR="$ROOT_DIR/Unix-FreeBSD"
    CMD="./build.sh --os freebsd"
    ;;
  macos)
    DIR="$ROOT_DIR/macOS"
    CMD="./build.sh --os macos"
    ;;
  termux)
    DIR="$ROOT_DIR/Termux"
    CMD="./build.sh"
    ;;
esac

if [ -s "$ARGS_FILE" ]; then
  # shellcheck disable=SC2046,SC2086
  eval "set -- $(tr '\n' ' ' < "$ARGS_FILE")"
else
  set --
fi

cd "$DIR"
echo "Selected platform: $PLATFORM"
# CMD consists only of fixed strings chosen above.
eval "exec $CMD \"\$@\""
