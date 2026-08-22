#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
fail(){ echo "WaffleHouse-Client 3.3r1 version regression failed: $*" >&2; exit 1; }
need(){ grep -Fq "$2" "$ROOT/$1" || fail "$1 missing: $2"; }
need CMakeLists.txt 'project(WaffleHouseClient VERSION 3.3.1 LANGUAGES CXX)'
need CMakeLists.txt 'APP_VERSION_STRING="3.3r1"'
need src/appbranding.h '#define APP_VERSION_STRING "3.3r1"'
need include/trunkmonkey/Version.h '#define WAFFLEHOUSE_SOFTPHONE_VERSION "3.3r1"'
need include/trunkmonkey/Version.h 'WaffleHouse-Client/3.3r1'
need build.sh 'Build WaffleHouse-Client 3.3r1, the unified C++ GUI/CLI executable.'
echo 'WaffleHouse-Client 3.3r1 version regression: PASS'
