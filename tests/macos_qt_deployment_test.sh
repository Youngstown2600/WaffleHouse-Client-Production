#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
B="$ROOT/scripts/build-macos.sh"
grep -q 'QT_DEPLOY_FORMULAS="qtbase qtmultimedia qtsvg qtdeclarative qtwebengine qttools' "$B"
grep -q 'brew deps --installed --formula qt' "$B"
grep -q '"-libpath=\$qt_module_prefix/lib"' "$B"
grep -q '"\$MACDEPLOYQT" "\$@"' "$B"
echo "macOS Homebrew Qt deployment regression: PASS"
