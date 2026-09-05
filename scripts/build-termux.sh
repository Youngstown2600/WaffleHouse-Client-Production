#!/data/data/com.termux/files/usr/bin/bash
set -euo pipefail
ROOT_DIR=$(cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT_DIR"

ACTION=build
ASSUME_YES=0
ORIGINAL_ARGC=$#
usage() {
  cat <<'USAGE'
Usage: ./scripts/build-termux.sh [--uninstall|--remove-only] [--yes]

With no options in an interactive Termux session the builder asks whether to
build/install or uninstall/remove WaffleHouse-Client. Uninstall preserves all
per-user WaffleHouse configuration.
USAGE
}
while [[ $# -gt 0 ]]; do
  case "$1" in
    --uninstall|--remove-only) ACTION=uninstall ;;
    --yes|-y) ASSUME_YES=1 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown Termux builder option: $1" >&2; usage >&2; exit 2 ;;
  esac
  shift
done

echo "WaffleHouse-Client 5.1r4 - Termux/Android GUI + CLI"
if [[ -z "${TERMUX_VERSION:-}" && "${PREFIX:-}" != *com.termux* ]]; then
  echo "This builder must be run inside Termux." >&2; exit 2
fi

if [[ "$ORIGINAL_ARGC" -eq 0 && -t 0 ]]; then
  cat <<'ACTION_PROMPT'

What do you want to do?
  1) Build / Install WaffleHouse-Client
  2) Uninstall / Remove WaffleHouse-Client
ACTION_PROMPT
  read -r -p 'Selection [1]: ' answer
  case "$answer" in
    ''|1) ACTION=build ;;
    2) ACTION=uninstall ;;
    *) echo "Invalid selection." >&2; exit 2 ;;
  esac
fi

if [[ "$ACTION" == uninstall ]]; then
  if [[ "$ASSUME_YES" -ne 1 && -t 0 ]]; then
    read -r -p 'Remove installed WaffleHouse-Client files and preserve user configuration? [y/N]: ' answer
    case "$answer" in y|Y|yes|YES|Yes) ;; *) echo "Uninstall cancelled."; exit 0 ;; esac
  fi
  echo "==> Removing WaffleHouse-Client from Termux"
  rm -f "$PREFIX/bin/wafflehouse-client" \
        "$PREFIX/bin/wafflehouse-client-gui" \
        "$PREFIX/bin/wafflehouse-client-cli" \
        "$PREFIX/bin/wafflehouse-shell" \
        "$PREFIX/share/applications/wafflehouse-client.desktop"
  for size in 16 22 24 32 48 64 128 256 512; do
    rm -f "$PREFIX/share/icons/hicolor/${size}x${size}/apps/wafflehouse-client.png"
  done
  rm -rf "$PREFIX/share/wafflehouse-client"
  echo "WaffleHouse-Client application files removed. User configuration was preserved."
  exit 0
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
