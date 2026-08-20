#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
BUILD_SCRIPT="$ROOT_DIR/build.sh"

PREFIX=/usr/local
PREFIX_EXPLICIT=0
REMOVE_ONLY=0
ASSUME_YES=0
DRY_RUN=0
NO_AUTO_DEPS=0
JOBS=
BUILD_TYPE=
PRIV_METHOD=

HOST_OS=$(uname -s)
case "$HOST_OS" in
  Linux|FreeBSD) ;;
  *)
    echo "Unsupported OS: $HOST_OS (supported: Linux, FreeBSD)" >&2
    exit 2
    ;;
esac

usage() {
  cat <<'EOF2'
Usage: ./client-up.sh [options]

Upgrade helper for WaffleHouse-Client 2.5.2.

By default this script:
  1. Locates the currently installed WaffleHouse-Client prefix when possible.
  2. Removes the previous unified executable/desktop entry plus legacy
     wafflehouse-gui / wafflehouse-cli launchers in the selected prefix.
  3. Preserves all user profiles, saved settings, passwords, CPX identity/trust
     data, buddy/watch lists, and other per-user configuration.
  4. Runs the current build.sh with --clean --install to install this release.

Options:
  --prefix PATH       Upgrade the installation under PATH (default: auto-detect,
                      otherwise /usr/local)
  --remove-only       Remove the installed client without building/installing 2.5.2
  --yes, -y           Do not ask for confirmation
  --dry-run           Show what would be removed/built/installed; change nothing
  --no-auto-deps      Pass --no-auto-deps to build.sh
  --jobs N            Pass parallel job count to build.sh
  --build-type TYPE   Pass CMake build type to build.sh
  -h, --help          Show this help

Examples:
  ./client-up.sh
  ./client-up.sh --yes
  ./client-up.sh --prefix /usr/local
  ./client-up.sh --prefix "$HOME/.local"
  ./client-up.sh --remove-only
EOF2
}

while [ "$#" -gt 0 ]; do
  case "$1" in
    --prefix)
      shift
      [ "$#" -gt 0 ] || { echo "--prefix requires a path" >&2; exit 2; }
      PREFIX=$1
      PREFIX_EXPLICIT=1
      ;;
    --remove-only) REMOVE_ONLY=1 ;;
    --yes|-y) ASSUME_YES=1 ;;
    --dry-run) DRY_RUN=1 ;;
    --no-auto-deps) NO_AUTO_DEPS=1 ;;
    --jobs)
      shift
      [ "$#" -gt 0 ] || { echo "--jobs requires a positive integer" >&2; exit 2; }
      case "$1" in ''|*[!0-9]*|0) echo "--jobs requires a positive integer" >&2; exit 2 ;; esac
      JOBS=$1
      ;;
    --build-type)
      shift
      [ "$#" -gt 0 ] || { echo "--build-type requires a value" >&2; exit 2; }
      BUILD_TYPE=$1
      ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown option: $1" >&2; usage >&2; exit 2 ;;
  esac
  shift
done

# If the user did not specify a prefix, prefer the prefix of the currently
# resolved wafflehouse-client executable when it has the standard PREFIX/bin
# layout. This makes upgrades work for /usr/local as well as ~/.local and other
# custom prefixes without guessing.
if [ "$PREFIX_EXPLICIT" -eq 0 ] && command -v wafflehouse-client >/dev/null 2>&1; then
  CURRENT_BIN=$(command -v wafflehouse-client)
  case "$CURRENT_BIN" in
    */bin/wafflehouse-client)
      DETECTED_PREFIX=${CURRENT_BIN%/bin/wafflehouse-client}
      [ -n "$DETECTED_PREFIX" ] || DETECTED_PREFIX=/
      PREFIX=$DETECTED_PREFIX
      ;;
  esac
fi

[ "$PREFIX" = "/" ] || PREFIX=${PREFIX%/}
BIN_PATH="$PREFIX/bin/wafflehouse-client"
DESKTOP_PATH="$PREFIX/share/applications/wafflehouse-client.desktop"
LEGACY_GUI_PATH="$PREFIX/bin/wafflehouse-gui"
LEGACY_CLI_PATH="$PREFIX/bin/wafflehouse-cli"
LEGACY_GUI_DESKTOP_PATH="$PREFIX/share/applications/wafflehouse-gui.desktop"

printf '%s\n' "============================================================"
printf '%s\n' "             WAFFLEHOUSE CLIENT-UP 2.5.2"
printf '%s\n' "============================================================"
printf 'Host OS:        %s\n' "$HOST_OS"
printf 'Upgrade prefix: %s\n' "$PREFIX"
printf 'Current binary: %s\n' "$BIN_PATH"
printf 'Desktop entry:  %s\n' "$DESKTOP_PATH"
printf 'Legacy GUI:     %s\n' "$LEGACY_GUI_PATH"
printf 'Legacy CLI:     %s\n' "$LEGACY_CLI_PATH"
echo
printf '%s\n' "User configuration will NOT be removed."
printf '%s\n' "Saved profiles, credentials, CPX identity/trust state, and settings are preserved."
echo

if [ "$ASSUME_YES" -ne 1 ] && [ "$DRY_RUN" -ne 1 ] && [ -t 0 ]; then
  if [ "$REMOVE_ONLY" -eq 1 ]; then
    printf 'Remove the installed WaffleHouse-Client from this prefix? [y/N] '
  else
    printf 'Remove the previous client and install WaffleHouse-Client 2.5.2? [y/N] '
  fi
  IFS= read -r answer
  case "$answer" in y|Y|yes|YES|Yes) ;; *) echo "Upgrade cancelled."; exit 0 ;; esac
fi

quote_arg() {
  printf "'%s'" "$(printf '%s' "$1" | sed "s/'/'\\\\''/g")"
}

choose_privilege_method() {
  # Nothing to remove means no cleanup privilege is needed.
  any_target=0
  for target in "$BIN_PATH" "$DESKTOP_PATH" "$LEGACY_GUI_PATH" "$LEGACY_CLI_PATH" "$LEGACY_GUI_DESKTOP_PATH"; do
    if [ -e "$target" ] || [ -L "$target" ]; then any_target=1; break; fi
  done
  if [ "$any_target" -eq 0 ]; then
    PRIV_METHOD=none
    return 0
  fi

  # If every existing target is directly removable by this user, stay unprivileged.
  removable=1
  for target in "$BIN_PATH" "$DESKTOP_PATH" "$LEGACY_GUI_PATH" "$LEGACY_CLI_PATH" "$LEGACY_GUI_DESKTOP_PATH"; do
    if [ -e "$target" ] || [ -L "$target" ]; then
      parent=$(dirname "$target")
      [ -w "$parent" ] || removable=0
    fi
  done
  if [ "$removable" -eq 1 ]; then PRIV_METHOD=none; return 0; fi

  if [ "$DRY_RUN" -eq 1 ]; then PRIV_METHOD=dry-run; return 0; fi
  if [ "$(id -u)" -eq 0 ]; then PRIV_METHOD=root; return 0; fi
  if command -v sudo >/dev/null 2>&1; then PRIV_METHOD=sudo; sudo -v; return 0; fi
  if command -v doas >/dev/null 2>&1; then PRIV_METHOD=doas; doas true; return 0; fi
  if command -v su >/dev/null 2>&1; then PRIV_METHOD=su; return 0; fi
  echo "Cannot remove the previous installation: root privileges are required and no sudo/doas/su helper is available." >&2
  exit 1
}

run_privileged() {
  if [ "$DRY_RUN" -eq 1 ]; then
    printf '  [dry-run]'
    for arg in "$@"; do printf ' %s' "$arg"; done
    echo
    return 0
  fi

  case "$PRIV_METHOD" in
    none|root) "$@" ;;
    sudo) sudo "$@" ;;
    doas) doas "$@" ;;
    su)
      cmd=
      for arg in "$@"; do cmd="$cmd $(quote_arg "$arg")"; done
      if [ "$HOST_OS" = FreeBSD ]; then su -m root -c "$cmd"; else su root -c "$cmd"; fi
      ;;
    *) echo "Privilege method was not initialized." >&2; exit 1 ;;
  esac
}

choose_privilege_method

echo "==> Removing previous installed WaffleHouse clients"
for target in "$BIN_PATH" "$DESKTOP_PATH" "$LEGACY_GUI_PATH" "$LEGACY_CLI_PATH" "$LEGACY_GUI_DESKTOP_PATH"; do
  if [ -e "$target" ] || [ -L "$target" ]; then
    run_privileged rm -f "$target"
    echo "Removed: $target"
  else
    echo "Not installed: $target"
  fi
done

# Update desktop caches when available. Failure here should never abort an
# otherwise valid upgrade/remove operation.
if [ "$DRY_RUN" -eq 0 ] && command -v update-desktop-database >/dev/null 2>&1 && [ -d "$PREFIX/share/applications" ]; then
  update-desktop-database "$PREFIX/share/applications" >/dev/null 2>&1 || true
fi

if [ "$REMOVE_ONLY" -eq 1 ]; then
  echo
  echo "Previous WaffleHouse-Client installation removed. User configuration was preserved."
  exit 0
fi

[ -x "$BUILD_SCRIPT" ] || chmod +x "$BUILD_SCRIPT" 2>/dev/null || true
[ -f "$BUILD_SCRIPT" ] || { echo "Cannot continue: $BUILD_SCRIPT was not found." >&2; exit 1; }

echo
echo "==> Building and installing WaffleHouse-Client 2.5.2"
set -- "$BUILD_SCRIPT" --clean --install --prefix "$PREFIX"
[ "$DRY_RUN" -eq 1 ] && set -- "$@" --dry-run
[ "$NO_AUTO_DEPS" -eq 1 ] && set -- "$@" --no-auto-deps
[ -n "$JOBS" ] && set -- "$@" --jobs "$JOBS"
[ -n "$BUILD_TYPE" ] && set -- "$@" --build-type "$BUILD_TYPE"
"$@"

echo
echo "Client upgrade complete."
echo "Installed: $BIN_PATH"
echo "Configuration preserved: yes"
