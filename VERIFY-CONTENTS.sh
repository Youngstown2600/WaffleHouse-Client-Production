#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
for d in Linux Unix-FreeBSD macOS Termux; do
  if [ ! -d "$ROOT/$d" ]; then
    echo "MISSING: $d" >&2
    exit 1
  fi
  echo "OK: $d"
done
echo "All four platform trees are present."
