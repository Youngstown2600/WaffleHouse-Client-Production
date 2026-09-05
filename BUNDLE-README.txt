NINA NETWORK: WaffleHouse-Client 5.1r4 includes the known-good NinaIM/NINAPatcher-compatible OSCAR handshake in the shared GUI/CLI backend.
PLATFORMS: Linux/Unix, FreeBSD, macOS, Windows, Termux/Android.

WaffleHouse-Client 5.1r4 — All-Platform Source Bundle
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


5.1r4 lifecycle: Linux/FreeBSD/macOS interactive builders offer Build/Install or Uninstall/Remove before protocol/dependency work. Use --uninstall/--remove-only for scripted desktop removal. Termux has the same removal option in scripts/build-termux.sh. Windows is portable and build-windows.ps1 -Uninstall removes the builder-managed dist/windows package. User configuration is preserved on all removal paths.

5.1r4 UI/account update: AIM/OSCAR and IRC profiles have optional local account labels; the desktop dial pad enforces uniform key geometry; Active Call Controls use a compact two-row primary control bank.

5.1r4 media update: Media playlists/streams persist in the per-user WaffleHouse media library; local PLS/M3U/M3U8/XSPF definitions are cached internally and the expanded queue is mirrored to queue.m3u8. Historical validation/merge artifacts were removed from the shipping bundle.
