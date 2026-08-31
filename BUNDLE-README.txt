NINA NETWORK: WaffleHouse-Client 5.1 includes the known-good NinaIM/NINAPatcher-compatible OSCAR handshake in the shared GUI/CLI backend.
PLATFORMS: Linux/Unix, FreeBSD, macOS, Windows, Termux/Android.

WaffleHouse-Client 5.1 — All-Platform Source Bundle
Platforms: Linux, FreeBSD/Unix, macOS
Build: ./build.sh

The builder asks which OS is being installed on, verifies that selection against
uname, and dispatches to the matching platform build path.

A successful normal build asks before installing WaffleHouse-Client into the
platform's system bin location. The default answer is No.

macOS 5.0r8 fixes the Homebrew mpv failure seen on 5.0r7: mpv and ffmpeg are
optional runtime helpers and no longer block compilation. The builder does not
try to install them unless --with-media-deps is explicitly supplied; even then,
a Homebrew install failure is a warning rather than a fatal build error.
