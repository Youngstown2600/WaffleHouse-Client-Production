# WaffleHouse-Client 5.0r7 — Linux build notes

WaffleHouse-Client 5.0r7 builds the same C++/Qt6 GUI + ncurses CLI on Debian-family Linux, Fedora, and Slackware. OSCAR voice adds the Qt 6 Multimedia development module to the existing Core/Gui/Widgets/Network requirements. The Media Center still uses `mpv` + `ffmpeg`.

## Debian / Ubuntu / Linux Mint and derivatives

`build.sh` can install `qt6-base-dev`, `qt6-multimedia-dev`, compiler/CMake/pkg-config, libsodium, ncurses, xkbcommon, PJSIP prerequisites, ALSA development files, mpv, and ffmpeg.

## Fedora / dnf systems

The builder uses the Qt 6 base and `qt6-qtmultimedia-devel` development packages plus the corresponding crypto/audio/network prerequisites.

## Slackware

The builder detects Slackware/slackpkg and uses available base packages; if `sbopkg` is available it can assist with Qt6/mpv packages. Package availability varies by Slackware installation, so the final dependency audit remains authoritative.

## Testing without system application installation

```sh
./build.sh --os linux --no-install
```

Use `--no-auto-deps` as well if you only want the dependency audit/build attempt and do not want the builder to install missing packages.
