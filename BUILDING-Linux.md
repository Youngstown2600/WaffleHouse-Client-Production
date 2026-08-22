# WaffleHouse-Client 3.1r6 — Linux build notes

The Media Center uses the same C++/Qt6 source on supported Linux distributions. The runtime media stack is `mpv` + `ffmpeg` for local media, direct HTTP/HLS streams, internet radio, and playlists.

## Debian / Ubuntu / Linux Mint and derivatives

`build.sh` detects `apt-get` and can install the Qt6, compiler, libsodium/ncurses/xkbcommon, SIP/audio, mpv, and ffmpeg packages automatically.

## Fedora / dnf systems

`build.sh` detects `dnf` (and `yum` as a compatibility path) and installs the corresponding development/runtime packages before rebuilding the dependency audit.

## Slackware

`build.sh` detects `/etc/slackware-version` or `slackpkg`. It uses `slackpkg` for available base packages. If `sbopkg` is installed it can also attempt the SlackBuilds packages `qt6` and `mpv`.

Slackware package sources vary by installation, so the final dependency audit remains authoritative.

## Testing without system application installation

```sh
./build.sh --no-install
```

This still allows dependency setup unless `--no-auto-deps` is also supplied. WaffleHouse itself is not copied to `/usr/local/bin` when `--no-install` is used.
