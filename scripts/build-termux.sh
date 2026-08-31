#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail
ROOT_DIR=$(cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT_DIR"

echo "WaffleHouse-Client 5.1 - Termux/Android GUI + CLI"
if [[ -z "${TERMUX_VERSION:-}" && "${PREFIX:-}" != *com.termux* ]]; then
  echo "This builder must be run inside Termux." >&2; exit 2
fi
pkg update -y
pkg install -y x11-repo
pkg update -y
pkg install -y clang cmake ninja pkg-config make git curl perl python \
  qt6-qtbase qt6-qtmultimedia libsodium ncurses openssl

PJSIP_PREFIX=${WAFFLEHOUSE_PJSIP_PREFIX:-$HOME/.local/wafflehouse-pjsip}
PJSIP_SRC="$ROOT_DIR/third_party/pjproject"
if [[ ! -f "$PJSIP_SRC/pjsip/include/pjsip.h" ]]; then
  mkdir -p "$ROOT_DIR/third_party"
  rm -rf "$PJSIP_SRC"
  git clone --depth 1 --branch 2.17 https://github.com/pjsip/pjproject.git "$PJSIP_SRC"
fi
if [[ ! -f "$PJSIP_PREFIX/.wafflehouse-pjsip-build" ]]; then
  CC=clang CXX=clang++ ./scripts/build-pjsip.sh "$PJSIP_SRC" "$PJSIP_PREFIX"
fi
export PKG_CONFIG_PATH="$PJSIP_PREFIX/lib/pkgconfig:$PREFIX/lib/pkgconfig:${PKG_CONFIG_PATH:-}"
export CMAKE_PREFIX_PATH="$PREFIX:${CMAKE_PREFIX_PATH:-}"
cmake -S . -B build-termux -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PREFIX" \
  -DWAFFLEHOUSE_ENABLE_OSCAR=ON -DWAFFLEHOUSE_ENABLE_IRC=ON -DWAFFLEHOUSE_ENABLE_TELNET=ON \
  -DWAFFLEHOUSE_ENABLE_SIP=ON -DWAFFLEHOUSE_ENABLE_MEDIA=ON
cmake --build build-termux --parallel
cmake --install build-termux
cat > "$PREFIX/bin/wafflehouse-client-gui" <<'EOF'
#!/data/data/com.termux/files/usr/bin/bash
exec wafflehouse-client --gui "$@"
EOF
cat > "$PREFIX/bin/wafflehouse-client-cli" <<'EOF'
#!/data/data/com.termux/files/usr/bin/bash
exec wafflehouse-client --cli "$@"
EOF
chmod +x "$PREFIX/bin/wafflehouse-client-gui" "$PREFIX/bin/wafflehouse-client-cli"
echo
echo "Installed: $PREFIX/bin/wafflehouse-client"
echo "CLI:       wafflehouse-client-cli"
echo "GUI:       wafflehouse-client-gui  (requires a Termux:X11 graphical session)"
