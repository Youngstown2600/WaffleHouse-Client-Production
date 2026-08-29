#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$ROOT_DIR"

SELECTED_OS=
PASSTHRU=

usage() {
  cat <<'USAGE'
WaffleHouse-Client 5.0r8 — combined desktop builder

Usage: ./build.sh [--os linux|freebsd|macos] [platform build options]

With no --os option the builder asks which operating system you are installing on:
  1) Linux
  2) FreeBSD / Unix
  3) macOS

The selected platform is checked against the host before any dependency installation
or build action begins. Termux/Android is intentionally not part of this desktop bundle.

Application installation is NOT automatic. After a successful normal build, the
platform builder asks before adding WaffleHouse-Client to a system bin directory.
Pressing Enter at that prompt means No.

Common options forwarded to the selected platform builder include:
  --clean, --pjsip, --no-auto-deps, --jobs N, --build-type TYPE
  --install, --no-install, --prefix PATH

Linux/FreeBSD also support:
  --audio-diagnose, --no-audio-fix, --dry-run, --upgrade, --uninstall, --yes

macOS also supports:
  --dmg, --with-media-deps, --yes

Examples:
  ./build.sh
  ./build.sh --os linux --clean
  ./build.sh --os freebsd --pjsip
  ./build.sh --os macos --clean --dmg
USAGE
}

# Pull --os out of the argument list while preserving all other options.
set -- "$@"
forward_file="${TMPDIR:-/tmp}/wafflehouse-build-args-$$"
: > "$forward_file"
trap 'rm -f "$forward_file"' EXIT HUP INT TERM
while [ "$#" -gt 0 ]; do
  case "$1" in
    --os)
      shift
      [ "$#" -gt 0 ] || { echo "--os requires linux, freebsd, or macos" >&2; exit 2; }
      SELECTED_OS=$(printf '%s' "$1" | tr '[:upper:]' '[:lower:]')
      ;;
    --os=*)
      SELECTED_OS=$(printf '%s' "${1#--os=}" | tr '[:upper:]' '[:lower:]')
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      # POSIX sh has no safe array; store one shell-quoted argument per line.
      printf '%s\n' "$1" | sed "s/'/'\\''/g; s/^/'/; s/$/'/" >> "$forward_file"
      ;;
  esac
  shift
done

case "$SELECTED_OS" in
  linux|freebsd|macos|darwin|'') : ;;
  *) echo "Unknown --os value: $SELECTED_OS" >&2; usage >&2; exit 2 ;;
esac
[ "$SELECTED_OS" != darwin ] || SELECTED_OS=macos

if [ -z "$SELECTED_OS" ]; then
  if [ ! -t 0 ]; then
    echo "No interactive terminal is available. Re-run with --os linux, --os freebsd, or --os macos." >&2
    exit 2
  fi
  cat <<'PROMPT'
============================================================
             WAFFLEHOUSE-CLIENT 5.0r8 DESKTOP
============================================================
What operating system are you installing on?

  1) Linux
  2) FreeBSD / Unix
  3) macOS
PROMPT
  printf 'Selection [1-3]: '
  IFS= read -r answer
  case "$answer" in
    1|linux|Linux) SELECTED_OS=linux ;;
    2|freebsd|FreeBSD|unix|Unix) SELECTED_OS=freebsd ;;
    3|macos|macOS|MacOS|darwin|Darwin) SELECTED_OS=macos ;;
    *) echo "Invalid selection." >&2; exit 2 ;;
  esac
fi

HOST_OS=$(uname -s)
case "$SELECTED_OS:$HOST_OS" in
  linux:Linux) PLATFORM_SCRIPT="$ROOT_DIR/scripts/build-unix.sh" ;;
  freebsd:FreeBSD) PLATFORM_SCRIPT="$ROOT_DIR/scripts/build-unix.sh" ;;
  macos:Darwin) PLATFORM_SCRIPT="$ROOT_DIR/scripts/build-macos.sh" ;;
  freebsd:*)
    echo "You selected FreeBSD / Unix, but this host reports '$HOST_OS'. The validated Unix target in 5.0r8 is FreeBSD." >&2
    exit 2
    ;;
  *)
    echo "OS selection/host mismatch: selected '$SELECTED_OS', host reports '$HOST_OS'." >&2
    echo "Re-run and choose the operating system you are actually building on." >&2
    exit 2
    ;;
esac

printf '\nSelected platform: %s (host: %s)\n\n' "$SELECTED_OS" "$HOST_OS"

# Restore forwarded arguments exactly enough for normal CLI paths/options.
# shellcheck disable=SC2046,SC2086
if [ -s "$forward_file" ]; then
  eval "set -- $(tr '\n' ' ' < "$forward_file")"
else
  set --
fi
exec "$PLATFORM_SCRIPT" "$@"
