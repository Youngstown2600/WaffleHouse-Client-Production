#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
fail(){ echo "WaffleHouse-Client 3.3r1 OSCAR capability/profile regression failed: $*" >&2; exit 1; }
need(){ grep -Fq "$2" "$ROOT/$1" || fail "$1 missing: $2"; }
need src/oscarprotocol.h 'constexpr quint16 FAM_LOCATE = 0x0002;'
need src/oscarprotocol.h 'constexpr quint16 LOCATE_RIGHTS_QUERY = 0x0002;'
need src/oscarprotocol.h 'constexpr quint16 LOCATE_SET_INFO = 0x0004;'
need src/oscarprotocol.h 'constexpr quint16 LOCATE_USER_INFO_QUERY = 0x0005;'
need src/oscarbackend.h 'void setProfile(const QString &profile);'
need src/oscarbackend.h 'void serverCapabilitiesChanged(const QStringList &features,'
need src/oscarbackend.cpp 'const bool profileSupported = families.contains(FAM_LOCATE);'
need src/oscarbackend.cpp 'request(*m_bos, FAM_LOCATE, LOCATE_RIGHTS_QUERY'
need src/oscarbackend.cpp 'request(*m_bos, FAM_LOCATE, LOCATE_USER_INFO_QUERY'
need src/oscarbackend.cpp 'm_bos->sendSnac(FAM_LOCATE, LOCATE_SET_INFO, body);'
need src/mainwindow.cpp 'QStringLiteral("Edit AIM Profile…")'
need src/mainwindow.cpp 'QStringLiteral("Server Capabilities…")'
need src/mainwindow.cpp 'void MainWindow::editAimProfile(BackendState *state)'
need src/mainwindow.cpp 'state->aimProfileMaxLength'
echo 'WaffleHouse-Client 3.3r1 OSCAR capability/profile regression: PASS'
