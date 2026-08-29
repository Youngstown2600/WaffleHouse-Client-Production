#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$ROOT_DIR"

SELECTED_OS=
PROTOCOL_SELECTION=
PROTOCOLS_EXPLICIT=0

usage() {
  cat <<'USAGE'
WaffleHouse-Client 5.0r18 — combined desktop builder

Usage: ./build.sh [--os linux|freebsd|macos] [--protocols LIST] [platform build options]

With no --os option the builder asks which operating system you are installing on:
  1) Linux
  2) FreeBSD / Unix
  3) macOS

The selected platform is checked against the host before any dependency installation
or build action begins. Termux/Android is intentionally not part of this desktop bundle.

The builder also asks which features to bake into this binary:
  AIM/OSCAR, IRC, Telnet/BBS, SIP/VoIP, Media/Radio
All five default to Yes. Disabled features are compile-time gated from the GUI/CLI.
For unattended builds use --protocols aim,irc,telnet,sip,media (or --protocols all).

Application installation is NOT automatic. After a successful normal Linux/FreeBSD
build, the platform builder explicitly asks whether to install WaffleHouse-Client
system-wide and add /bin/wafflehouse-client. Pressing Enter means No.
macOS keeps its normal /Applications + /usr/local/bin launcher flow.

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
  ./build.sh --os linux --protocols aim,irc,telnet
  ./build.sh --os macos --protocols aim,irc,telnet,sip,media
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
    --protocols)
      shift
      [ "$#" -gt 0 ] || { echo "--protocols requires a comma-separated list or all" >&2; exit 2; }
      PROTOCOL_SELECTION=$(printf '%s' "$1" | tr '[:upper:]' '[:lower:]')
      PROTOCOLS_EXPLICIT=1
      ;;
    --protocols=*)
      PROTOCOL_SELECTION=$(printf '%s' "${1#--protocols=}" | tr '[:upper:]' '[:lower:]')
      PROTOCOLS_EXPLICIT=1
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
             WAFFLEHOUSE-CLIENT 5.0r18 DESKTOP
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
    echo "You selected FreeBSD / Unix, but this host reports '$HOST_OS'. The validated Unix target in 5.0r18 is FreeBSD." >&2
    exit 2
    ;;
  *)
    echo "OS selection/host mismatch: selected '$SELECTED_OS', host reports '$HOST_OS'." >&2
    echo "Re-run and choose the operating system you are actually building on." >&2
    exit 2
    ;;
esac

# Compile-time feature selection.  Keep this in the top-level dispatcher so
# Linux, FreeBSD and macOS all produce the same feature matrix.
FEATURE_OSCAR=OFF
FEATURE_IRC=OFF
FEATURE_TELNET=OFF
FEATURE_SIP=OFF
FEATURE_MEDIA=OFF

feature_on() {
  case "$1" in
    aim|oscar) FEATURE_OSCAR=ON ;;
    irc) FEATURE_IRC=ON ;;
    telnet|bbs) FEATURE_TELNET=ON ;;
    sip|voip|pjsip|chan_sip|chansip) FEATURE_SIP=ON ;;
    media|radio|player) FEATURE_MEDIA=ON ;;
    '') ;;
    *) echo "Unknown protocol/feature in --protocols: $1" >&2; exit 2 ;;
  esac
}

if [ "$PROTOCOLS_EXPLICIT" -eq 1 ]; then
  case ",$PROTOCOL_SELECTION," in
    *,all,*) FEATURE_OSCAR=ON; FEATURE_IRC=ON; FEATURE_TELNET=ON; FEATURE_SIP=ON; FEATURE_MEDIA=ON ;;
    *)
      oldIFS=$IFS
      IFS=,
      # shellcheck disable=SC2086
      set -- $PROTOCOL_SELECTION
      IFS=$oldIFS
      for feature in "$@"; do feature_on "$feature"; done
      ;;
  esac
else
  if [ ! -t 0 ]; then
    FEATURE_OSCAR=ON; FEATURE_IRC=ON; FEATURE_TELNET=ON; FEATURE_SIP=ON; FEATURE_MEDIA=ON
  else
    echo
    echo "What should be baked into this WaffleHouse-Client build?"
    echo "Press Enter for Yes.  Answer n to leave a feature out of the binary UI."
    ask_feature() {
      _label=$1
      printf '  Include %s? [Y/n]: ' "$_label"
      IFS= read -r _answer
      case "$_answer" in n|N|no|NO|No) return 1 ;; *) return 0 ;; esac
    }
    ask_feature "AIM / OSCAR" && FEATURE_OSCAR=ON || true
    ask_feature "IRC" && FEATURE_IRC=ON || true
    ask_feature "Telnet / BBS" && FEATURE_TELNET=ON || true
    ask_feature "SIP / VoIP" && FEATURE_SIP=ON || true
    ask_feature "Media Player / Radio" && FEATURE_MEDIA=ON || true
  fi
fi

if [ "$FEATURE_OSCAR$FEATURE_IRC$FEATURE_TELNET$FEATURE_SIP$FEATURE_MEDIA" = "OFFOFFOFFOFFOFF" ]; then
  echo "At least one protocol or the media player must be selected." >&2
  exit 2
fi

WAFFLEHOUSE_FEATURE_CMAKE_ARGS="-DWAFFLEHOUSE_ENABLE_OSCAR=$FEATURE_OSCAR -DWAFFLEHOUSE_ENABLE_IRC=$FEATURE_IRC -DWAFFLEHOUSE_ENABLE_TELNET=$FEATURE_TELNET -DWAFFLEHOUSE_ENABLE_SIP=$FEATURE_SIP -DWAFFLEHOUSE_ENABLE_MEDIA=$FEATURE_MEDIA"
export WAFFLEHOUSE_FEATURE_CMAKE_ARGS

printf '\nSelected platform: %s (host: %s)\n' "$SELECTED_OS" "$HOST_OS"
printf 'Build features: AIM/OSCAR=%s  IRC=%s  TELNET/BBS=%s  SIP/VoIP=%s  MEDIA=%s\n\n' \
  "$FEATURE_OSCAR" "$FEATURE_IRC" "$FEATURE_TELNET" "$FEATURE_SIP" "$FEATURE_MEDIA"

# Restore forwarded arguments exactly enough for normal CLI paths/options.
# shellcheck disable=SC2046,SC2086
if [ -s "$forward_file" ]; then
  eval "set -- $(tr '\n' ' ' < "$forward_file")"
else
  set --
fi
exec "$PLATFORM_SCRIPT" "$@"
