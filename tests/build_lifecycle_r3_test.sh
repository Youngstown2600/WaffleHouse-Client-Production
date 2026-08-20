#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD="$ROOT/build.sh"
UP="$ROOT/client-up.sh"

sh -n "$BUILD"
sh -n "$UP"

grep -F -- '--upgrade)' "$BUILD" >/dev/null
grep -F -- '--uninstall|--remove-only)' "$BUILD" >/dev/null
grep -F 'cleanup_legacy_installation' "$BUILD" >/dev/null
grep -F 'cmake --install "$ROOT_DIR/build" --prefix "$INSTALL_PREFIX"' "$BUILD" >/dev/null
# Build command must occur before install/legacy cleanup in the execution tail.
build_line=$(grep -n 'run_cmd cmake --build build --parallel "$JOBS"' "$BUILD" | tail -1 | cut -d: -f1)
cleanup_line=$(grep -n '  cleanup_legacy_installation' "$BUILD" | tail -1 | cut -d: -f1)
install_line=$(grep -n 'run_privileged cmake --install "$ROOT_DIR/build" --prefix "$INSTALL_PREFIX"' "$BUILD" | tail -1 | cut -d: -f1)
[ "$build_line" -lt "$cleanup_line" ]
[ "$build_line" -lt "$install_line" ]
grep -F 'exec "$ROOT_DIR/build.sh" --upgrade "$@"' "$UP" >/dev/null

echo 'build lifecycle r3 regression: PASS'
