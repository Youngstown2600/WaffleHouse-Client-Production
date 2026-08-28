# WaffleHouse-Client 5.0r7 — Combined Desktop Bundle

One source bundle for **Linux, FreeBSD/Unix, and macOS**. The Qt GUI and ncurses CLI use the same shared AIM/OSCAR, IRC, Telnet/BBS, SIP/PJSIP, media, secure-room, transfer, contacts, and history implementation.

## Build

```sh
chmod +x build.sh
./build.sh
```

The top-level builder asks which operating system you are installing on:

1. Linux
2. FreeBSD / Unix
3. macOS

It verifies the selection against the current host and then dispatches to the appropriate platform builder. You can also use `--os linux`, `--os freebsd`, or `--os macos` for scripted builds.

A normal successful build **does not automatically install WaffleHouse-Client into a system bin directory**. The selected platform builder asks first, and `[y/N]` defaults to **No**.

- Linux / FreeBSD default launcher: `/usr/local/bin/wafflehouse-client`
- macOS optional app install: `/Applications/WaffleHouse-Client.app`
- macOS optional command launcher: `/usr/local/bin/wafflehouse-client`

See `BUILDING-Linux.md`, `BUILDING-FreeBSD.md`, and `BUILDING-macOS.md` for platform notes.

## 5.0r7 highlights

- Re-unifies the Linux/FreeBSD and macOS release trees into one desktop source bundle with an explicit OS-selection builder.
- macOS is synchronized to the 5.0r6 Unix/Linux source baseline, including all 5.0r4–5.0r6 AIM/OSCAR typing and file-transfer fixes.
- Fixes the macOS menu/status icon regression where the tray/status artwork could become white-on-white after closing the last visible application window. The WaffleHouse icon is explicitly kept as a full-color non-template icon on macOS, and the status item is synchronously removed during application quit.
- macOS notification sounds are copied into the `.app` bundle so moving the application out of the extracted source tree does not break built-in sounds.
- Secure CPX direct file transfers finalize correctly: after the final encrypted byte arrives, the receiver SHA-256 verifies the `.cpxpart`, renames it to the chosen filename, reports Complete, and returns confirmation so the sender leaves Verifying.
- AIM/OSCAR file-transfer control traffic no longer opens chat windows.
- OSCAR typing status stays in the normal AIM IM window.
- OSCAR rate-class parsing and relay pacing improvements from 5.0r5 are included on all desktop platforms.
- GUI/CLI softphone, media, secure rooms, themes, notifications, contacts/history, and the 5.0r2 conventional GUI exit path remain shared.

Termux/Android remains a separate target and is intentionally not included in this desktop bundle.
