#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
failures=0
total=0
for test_script in "$ROOT"/tests/*.sh; do
    [ "$(basename "$test_script")" = "run_all.sh" ] && continue
    total=$((total + 1))
    echo "===== $(basename "$test_script") ====="
    if TERM=${TERM:-xterm} sh "$test_script"; then
        :
    else
        failures=$((failures + 1))
    fi
done
echo "Regression summary: $((total - failures))/$total passed"
[ "$failures" -eq 0 ]
