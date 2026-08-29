#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT_DIR"

[ "$(uname -s)" = Darwin ] || { echo "build-macos.sh must run on macOS." >&2; exit 2; }

CLEAN=0
FORCE_PJSIP=0
AUTO_DEPS=1
MAKE_DMG=0
WITH_MEDIA_DEPS=0
BUILD_TYPE=Release
JOBS=$(sysctl -n hw.ncpu 2>/dev/null || echo 2)
PJSIP_PREFIX=${PJSIP_PREFIX:-${HOME:-$ROOT_DIR}/.local/wafflehouse-pjsip}
BUILD_DIR=${BUILD_DIR:-$ROOT_DIR/build-macos}
INSTALL_MODE=ask
INSTALL_PREFIX=/usr/local
APP_INSTALL_DIR=${WAFFLEHOUSE_APP_INSTALL_DIR:-/Applications}
ASSUME_YES=0

usage() {
  cat <<USAGE
Usage: ./build.sh --os macos [options]

Build WaffleHouse-Client 5.0r13 for macOS. The .app contains the GUI and the same
binary also supports --cli for the terminal interface.

  --clean          remove the macOS build directory first
  --pjsip          force rebuild managed PJSIP 2.17
  --dmg            also create a local DMG with macdeployqt
  --with-media-deps
                   optionally try to install mpv/ffmpeg with Homebrew;
                   failures do not abort the WaffleHouse build
  --no-auto-deps   do not install missing Homebrew dependencies
  --install        install after a successful build without the final prompt
  --no-install     build only and suppress the install prompt
  --prefix PATH    command launcher prefix (default /usr/local)
  --yes, -y        answer yes to explicit install confirmation paths
  --jobs N         parallel build jobs
  --build-type T   CMake build type (default Release)
  -h, --help       show this help

Default optional installation targets:
  app:      $APP_INSTALL_DIR/WaffleHouse-Client.app
  launcher: $INSTALL_PREFIX/bin/wafflehouse-client

A normal build asks before installing either target. Enter/default is No.
USAGE
}

while [ "$#" -gt 0 ]; do
  case "$1" in
    --clean) CLEAN=1 ;;
    --pjsip) FORCE_PJSIP=1 ;;
    --dmg) MAKE_DMG=1 ;;
    --with-media-deps) WITH_MEDIA_DEPS=1 ;;
    --no-auto-deps) AUTO_DEPS=0 ;;
    --install) INSTALL_MODE=yes ;;
    --no-install) INSTALL_MODE=no ;;
    --yes|-y) ASSUME_YES=1 ;;
    --prefix) shift; [ "$#" -gt 0 ] || { echo "--prefix requires a path" >&2; exit 2; }; INSTALL_PREFIX=$1 ;;
    --jobs) shift; JOBS=${1:?--jobs requires a value} ;;
    --build-type) shift; BUILD_TYPE=${1:?--build-type requires a value} ;;
    --dry-run) echo "--dry-run is currently a Linux/FreeBSD builder option." >&2; exit 2 ;;
    --upgrade|--uninstall|--remove-only|--audio-diagnose|--no-audio-fix)
      echo "$1 is currently a Linux/FreeBSD builder option." >&2; exit 2 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown macOS build option: $1" >&2; usage >&2; exit 2 ;;
  esac
  shift
done

case "$JOBS" in ''|*[!0-9]*|0) echo "--jobs requires a positive integer" >&2; exit 2 ;; esac
[ "$INSTALL_PREFIX" = / ] || INSTALL_PREFIX=${INSTALL_PREFIX%/}
INSTALL_BIN="$INSTALL_PREFIX/bin/wafflehouse-client"
INSTALL_APP="$APP_INSTALL_DIR/WaffleHouse-Client.app"

command -v brew >/dev/null 2>&1 || {
  echo "Homebrew is required for the macOS dependency preflight: https://brew.sh" >&2
  exit 1
}
BREW_PREFIX=$(brew --prefix)

need_formula() {
  formula=$1
  brew list --versions "$formula" >/dev/null 2>&1 && return 0
  if [ "$AUTO_DEPS" -eq 0 ]; then
    echo "Missing Homebrew dependency: $formula" >&2
    return 1
  fi
  echo "==> Installing macOS dependency: $formula"
  brew install "$formula"
}

# Only packages required to compile/link WaffleHouse belong in the fatal
# dependency preflight. mpv and ffmpeg are external runtime helpers and are
# deliberately optional; an older/unsupported macOS Homebrew installation may
# have no bottle for mpv, and that must never prevent the client itself from
# building. yt-dlp is not used by the 5.0r13 media implementation.
for formula in cmake pkg-config qt libsodium ncurses portaudio opus; do
  need_formula "$formula"
done
command -v git >/dev/null 2>&1 || { echo "git is required (install Xcode Command Line Tools)." >&2; exit 1; }

optional_formula() {
  formula=$1
  purpose=$2
  if brew list --versions "$formula" >/dev/null 2>&1; then
    echo "Optional runtime dependency present: $formula ($purpose)"
    return 0
  fi

  if [ "$WITH_MEDIA_DEPS" -eq 0 ]; then
    echo "NOTE: optional runtime dependency '$formula' is not installed ($purpose)."
    echo "      WaffleHouse-Client will still build; only the related media feature is unavailable."
    echo "      Re-run with --with-media-deps if you want the builder to try Homebrew."
    return 0
  fi

  echo "==> Trying optional macOS runtime dependency: $formula"
  if brew install "$formula"; then
    echo "Installed optional dependency: $formula"
  else
    echo "WARNING: Homebrew could not install optional dependency '$formula'." >&2
    echo "         Continuing the WaffleHouse-Client build without it." >&2
  fi
}

optional_formula mpv "local/radio media playback"
optional_formula ffmpeg "SSH/remote media audio helper"

if [ "$FORCE_PJSIP" -eq 1 ]; then rm -f "$PJSIP_PREFIX/.wafflehouse-pjsip-build"; fi
if ! PKG_CONFIG_PATH="$PJSIP_PREFIX/lib/pkgconfig:${PKG_CONFIG_PATH:-}" pkg-config --exact-version=2.17 libpjproject >/dev/null 2>&1 || \
   [ ! -f "$PJSIP_PREFIX/.wafflehouse-pjsip-build" ]; then
  echo "==> Preparing managed PJSIP 2.17 for macOS"
  PJSIP_PREFIX="$PJSIP_PREFIX" "$ROOT_DIR/scripts/bootstrap-pjsip.sh"
fi

[ "$CLEAN" -eq 0 ] || rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

QT_PREFIX=$(brew --prefix qt)
SODIUM_PREFIX=$(brew --prefix libsodium)
NCURSES_PREFIX=$(brew --prefix ncurses)
PORTAUDIO_PREFIX=$(brew --prefix portaudio)
OPUS_PREFIX=$(brew --prefix opus)
export PKG_CONFIG_PATH="$PJSIP_PREFIX/lib/pkgconfig:$SODIUM_PREFIX/lib/pkgconfig:$NCURSES_PREFIX/lib/pkgconfig:$PORTAUDIO_PREFIX/lib/pkgconfig:$OPUS_PREFIX/lib/pkgconfig:$BREW_PREFIX/lib/pkgconfig:${PKG_CONFIG_PATH:-}"
export CMAKE_PREFIX_PATH="$QT_PREFIX${CMAKE_PREFIX_PATH:+:$CMAKE_PREFIX_PATH}"

printf '%s\n' "============================================================" \
  "                    WAFFLEHOUSE-CLIENT 5.0r13" \
  "============================================================" \
  "Platform:       macOS $(sw_vers -productVersion 2>/dev/null || true)" \
  "Architecture:   $(uname -m)" \
  "Build type:     $BUILD_TYPE" \
  "PJSIP:          $PJSIP_PREFIX" \
  "Install app:    $INSTALL_APP" \
  "CLI launcher:   $INSTALL_BIN"

# Feature flags are fixed -DNAME=ON/OFF tokens supplied by the top-level builder.
# shellcheck disable=SC2086
cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
  -DCMAKE_PREFIX_PATH="$QT_PREFIX" ${WAFFLEHOUSE_FEATURE_CMAKE_ARGS:-}
cmake --build "$BUILD_DIR" -j "$JOBS"

APP="$BUILD_DIR/wafflehouse-client.app"
[ -d "$APP" ] || { echo "macOS app bundle not produced at $APP" >&2; exit 1; }
MACDEPLOYQT="$QT_PREFIX/bin/macdeployqt"
[ -x "$MACDEPLOYQT" ] || { echo "macdeployqt not found: $MACDEPLOYQT" >&2; exit 1; }

# Make notification sounds self-contained inside the .app rather than relying on
# the extracted source directory remaining in place after installation.
mkdir -p "$APP/Contents/Resources/sounds"
cp -f "$ROOT_DIR"/sounds/*.wav "$APP/Contents/Resources/sounds/"

# Homebrew Qt 6 is split across module kegs. macdeployqt knows the qtbase and
# qtmultimedia paths it was built with, but dependencies referenced through
# plugins can live in other kegs (notably QtSvg and QtPdf in qtwebengine).
# Feed those module library directories to macdeployqt explicitly.
set -- "$APP"
# Seed the modules implicated by current Homebrew Qt deployments, then append
# every installed Qt dependency of the `qt` meta-formula so future split kegs
# are discoverable without another hard-coded macdeployqt fix.
QT_DEPLOY_FORMULAS="qtbase qtmultimedia qtsvg qtdeclarative qtwebengine qttools $(brew deps --installed --formula qt 2>/dev/null || true)"
for qt_formula in $QT_DEPLOY_FORMULAS; do
  case "$qt_formula" in qt*) ;; *) continue ;; esac
  qt_module_prefix=$(brew --prefix "$qt_formula" 2>/dev/null || true)
  if [ -n "$qt_module_prefix" ] && [ -d "$qt_module_prefix/lib" ]; then
    candidate="-libpath=$qt_module_prefix/lib"
    case " $* " in *" $candidate "*) ;; *) set -- "$@" "$candidate" ;; esac
  fi
done
if [ "$MAKE_DMG" -eq 1 ]; then set -- "$@" -dmg; fi
"$MACDEPLOYQT" "$@"

ask_install() {
  [ "$INSTALL_MODE" = ask ] || return 0
  if [ "$ASSUME_YES" -eq 1 ]; then INSTALL_MODE=yes; return 0; fi
  if [ ! -t 0 ]; then INSTALL_MODE=no; return 0; fi
  echo
  echo "Build succeeded."
  echo "Optional installation will copy the app to:"
  echo "  $INSTALL_APP"
  echo "and add the command launcher:"
  echo "  $INSTALL_BIN"
  printf 'Install WaffleHouse-Client now? [y/N]: '
  IFS= read -r answer
  case "$answer" in y|Y|yes|YES|Yes) INSTALL_MODE=yes ;; *) INSTALL_MODE=no ;; esac
}

run_admin() {
  if [ "$(id -u)" -eq 0 ]; then "$@"; else sudo "$@"; fi
}

ask_install
if [ "$INSTALL_MODE" = yes ]; then
  echo
  echo "==> Installing WaffleHouse-Client 5.0r13 for macOS"
  run_admin mkdir -p "$APP_INSTALL_DIR" "$INSTALL_PREFIX/bin"
  if [ -e "$INSTALL_APP" ]; then run_admin rm -rf "$INSTALL_APP"; fi
  run_admin ditto "$APP" "$INSTALL_APP"
  if [ -e "$INSTALL_BIN" ] || [ -L "$INSTALL_BIN" ]; then run_admin rm -f "$INSTALL_BIN"; fi
  run_admin ln -s "$INSTALL_APP/Contents/MacOS/wafflehouse-client" "$INSTALL_BIN"
  echo "Installed app:      $INSTALL_APP"
  echo "Installed launcher: $INSTALL_BIN"
else
  echo
  echo "System installation skipped."
  echo "Built app: $APP"
fi

echo
echo "Optional media runtime status:"
if brew list --versions mpv >/dev/null 2>&1 || command -v mpv >/dev/null 2>&1; then
  echo "  mpv:    available"
else
  echo "  mpv:    not installed (client works; Media playback is disabled)"
fi
if brew list --versions ffmpeg >/dev/null 2>&1 || command -v ffmpeg >/dev/null 2>&1; then
  echo "  ffmpeg: available"
else
  echo "  ffmpeg: not installed (only SSH/remote media helper is unavailable)"
fi

cat <<DONE

WaffleHouse-Client 5.0r13 macOS build complete.
GUI: open "$APP"
CLI: "$APP/Contents/MacOS/wafflehouse-client" --cli
$(if [ "$MAKE_DMG" -eq 1 ]; then echo "DMG: $BUILD_DIR/wafflehouse-client.dmg"; fi)

Code signing/notarization are intentionally left to the distributor's Apple
Developer identity; this builder does not invent or embed signing credentials.
DONE
