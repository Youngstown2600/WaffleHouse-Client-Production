#!/bin/sh
set -eu
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
fail(){ echo "Media Center 3.3 Linux/FreeBSD portability regression failed: $*" >&2; exit 1; }
need(){ grep -Fq "$2" "$ROOT/$1" || fail "$1 missing: $2"; }
forbid(){ if grep -Fqi "$2" "$ROOT/$1"; then fail "$1 contains forbidden text: $2"; fi; }
# Media backend stays distro-neutral and never invokes a shell.
need src/mediacontroller.cpp 'm_process = new QProcess(this)'
need src/mediacontroller.cpp 'QStringLiteral("--ytdl=no")'
forbid src/mediacontroller.cpp 'system('
forbid src/mediacontroller.cpp 'popen('
forbid src/mediacontroller.cpp 'sh -c'
# Builder coverage for requested OS families.
need build.sh 'command -v apt-get'
need build.sh 'command -v dnf'
need build.sh '/etc/slackware-version'
need build.sh 'LINUX_PKG_MANAGER=slackware'
need build.sh 'command -v sbopkg'
need build.sh 'FreeBSD pkg(8) was not found.'
need build.sh 'pkg install -y'
need build.sh 'mpv ffmpeg'
forbid build.sh 'yt-dlp'
forbid build.sh 'deno'
echo 'WaffleHouse-Client 3.3r1 Debian/Fedora/Slackware/FreeBSD media portability: PASS'
