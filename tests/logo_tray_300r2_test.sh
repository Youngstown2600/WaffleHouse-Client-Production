#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

need_file() {
    [ -f "$ROOT/$1" ] || { echo "FAIL: missing $1" >&2; exit 1; }
}
need_grep() {
    pattern=$1 file=$2
    grep -q -- "$pattern" "$ROOT/$file" || { echo "FAIL: $file missing pattern: $pattern" >&2; exit 1; }
}

need_file assets/branding/wafflehouse-client-logo.png
for size in 16 22 24 32 48 64 128 256 512; do
    need_file "assets/icons/${size}x${size}/wafflehouse-client.png"
done
need_grep 'qt_add_resources(${APP_EXECUTABLE} wafflehouse_branding' CMakeLists.txt
need_grep 'qt_add_resources(${APP_EXECUTABLE} wafflehouse_icons' CMakeLists.txt
need_grep 'Icon=wafflehouse-client' data/wafflehouse-client.desktop
need_grep 'QApplication::setWindowIcon(appIcon())' src/main.cpp
need_grep 'const QPixmap logoPixmap = appLogoPixmap()' src/main.cpp
need_grep 'QIcon icon = appIcon()' src/mainwindow.cpp
need_grep 'INSTALL_ICON_THEME_DIR=' build.sh
need_grep 'refresh_icon_cache' build.sh

echo "WaffleHouse 3.1 application/logo/tray integration: PASS"
