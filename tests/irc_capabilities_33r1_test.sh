#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
fail(){ echo "WaffleHouse-Client 3.3r1 IRC capability regression failed: $*" >&2; exit 1; }
need(){ grep -Fq "$2" "$ROOT/$1" || fail "$1 missing: $2"; }
need src/ircbackend.h 'void serverCapabilitiesChanged(const QStringList &ircv3Capabilities,'
need src/ircbackend.cpp 'QStringLiteral("CAP LS 302")'
need src/ircbackend.cpp 'QStringLiteral("CAP END")'
need src/ircbackend.cpp 'parsed.command == QStringLiteral("CAP")'
need src/ircbackend.cpp 'parsed.command == QStringLiteral("005")'
need src/ircbackend.cpp 'm_ircv3Capabilities'
need src/ircbackend.cpp 'm_isupportTokens'
need src/mainwindow.cpp 'QStringLiteral("IRC Server Capabilities")'
need src/mainwindow.cpp '005 / ISUPPORT tokens:'
echo 'WaffleHouse-Client 3.3r1 IRC capability regression: PASS'
