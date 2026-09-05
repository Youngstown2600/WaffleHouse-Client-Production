#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR=$(cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT_DIR"

echo "WaffleHouse-Client 5.1r4 - Windows/MSYS2 UCRT64 GUI + CLI"
if [[ "${MSYSTEM:-}" != UCRT64 ]]; then
  echo "This script requires an MSYS2 UCRT64 shell. Run build-windows.ps1." >&2; exit 2
fi

PREFIX_NAME=${MINGW_PACKAGE_PREFIX:-mingw-w64-ucrt-x86_64}
pacman -S --needed --noconfirm \
  base-devel git curl \
  "$PREFIX_NAME-toolchain" "$PREFIX_NAME-cmake" "$PREFIX_NAME-ninja" "$PREFIX_NAME-pkgconf" "$PREFIX_NAME-qt6-base" "$PREFIX_NAME-qt6-multimedia" \
  "$PREFIX_NAME-libsodium" "$PREFIX_NAME-ncurses" "$PREFIX_NAME-openssl" || {
    echo "Dependency installation failed. Update MSYS2 (pacman -Syu) and rerun." >&2; exit 1;
  }

PJSIP_PREFIX=${WAFFLEHOUSE_PJSIP_PREFIX:-$HOME/.local/wafflehouse-pjsip}
PJSIP_SRC="$ROOT_DIR/third_party/pjproject"
if [[ ! -f "$PJSIP_SRC/pjsip/include/pjsip.h" ]]; then
  mkdir -p "$ROOT_DIR/third_party"
  rm -rf "$PJSIP_SRC"
  git clone --depth 1 --branch 2.17 https://github.com/pjsip/pjproject.git "$PJSIP_SRC"
fi
if [[ ! -f "$PJSIP_PREFIX/.wafflehouse-pjsip-build" ]] || ! grep -q 'windows' "$PJSIP_PREFIX/.wafflehouse-pjsip-build"; then
  ./scripts/build-pjsip.sh "$PJSIP_SRC" "$PJSIP_PREFIX"
fi
export PKG_CONFIG_PATH="$PJSIP_PREFIX/lib/pkgconfig:${MINGW_PREFIX:-/ucrt64}/lib/pkgconfig:${PKG_CONFIG_PATH:-}"
export CMAKE_PREFIX_PATH="${MINGW_PREFIX:-/ucrt64}:${CMAKE_PREFIX_PATH:-}"

cmake -S . -B build-windows -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PWD/dist/windows" \
  -DWAFFLEHOUSE_ENABLE_OSCAR=ON -DWAFFLEHOUSE_ENABLE_IRC=ON -DWAFFLEHOUSE_ENABLE_TELNET=ON \
  -DWAFFLEHOUSE_ENABLE_SIP=ON -DWAFFLEHOUSE_ENABLE_MEDIA=ON
cmake --build build-windows --parallel
cmake --install build-windows

mkdir -p dist/windows
EXE=$(find build-windows -maxdepth 3 -type f -iname 'wafflehouse-client.exe' | head -1)
[[ -n "$EXE" ]] || { echo "wafflehouse-client.exe was not produced" >&2; exit 1; }
cp -f "$EXE" dist/windows/wafflehouse-client.exe
DEPLOY=$(command -v windeployqt6 || command -v windeployqt || true)
if [[ -n "$DEPLOY" ]]; then "$DEPLOY" dist/windows/wafflehouse-client.exe; fi
cat > dist/windows/wafflehouse-client-gui.cmd <<'EOF'
@echo off
"%~dp0wafflehouse-client.exe" --gui %*
EOF
cat > dist/windows/wafflehouse-client-cli.cmd <<'EOF'
@echo off
"%~dp0wafflehouse-client.exe" --cli %*
EOF
echo "Windows package: $ROOT_DIR/dist/windows"
