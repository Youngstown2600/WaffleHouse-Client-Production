# WaffleHouse-Client 5.0r18 — Desktop Bundle

## 5.0r18 AIM chat menu fix

- **Join AIM Chat…** is now enabled for a connected OSCAR account even when the BOS HostOnline list does not directly advertise CHAT_NAV/CHAT.
- ChatNav and Chat are requested as separate OSCAR services through OSERVICE redirects when a room is actually joined.
- Servers that truly do not support chat now return a visible runtime/protocol error instead of causing WaffleHouse to hide the feature in advance.


## 5.0r17 OSCAR audit viewer + /bin install prompt

- AIM/OSCAR Login Audit and Full Wire Trace now persist their already-redacted output to a per-account log under the platform application-data `logs/` directory.
- View it inside the GUI from **Tools → View OSCAR Audit Log…** or the AIM/OSCAR account context menu; the viewer shows the exact filesystem path.
- Full wire logs rotate at 5 MiB with one `.old` generation, and the internal viewer tails the last 512 KiB for large logs.
- Linux/FreeBSD builders now explicitly ask after a successful build whether to install system-wide and add `/bin/wafflehouse-client`. The default remains No. The actual application/assets remain in the normal prefix, with `/bin/wafflehouse-client` installed as the system launcher.
- Retains the 5.0r16 `BuildFeatures` compile fix, 5.0r15 IM composer-focus fix, and 5.0r14 native OSCAR presence/profile work.

## 5.0r15 IM composer focus fix

- New IM/chat windows now place keyboard focus in **Type a message…** after the window is shown.
- The read-only transcript remains mouse-selectable/copyable, but no longer wins initial keyboard focus.
- Focus is applied only on the first show so normal user-directed focus changes remain intact afterward.


## 5.0r14 OSCAR presence/profile hardening

WaffleHouse now decodes native OSCAR buddy away/status/idle fields instead of treating every connected buddy as simply Online. Saved AIM profiles are replayed after BOS login for servers that do not persist LOCATE data, and the regression suite audits the native presence/profile feature surface. Batched buddy updates are fully consumed, and standard host-side USERINFO, warning-level, and rejected-watch notifications are handled instead of falling through as unknown SNACs.

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

A normal successful build **does not automatically install WaffleHouse-Client**. Linux/FreeBSD explicitly ask whether to install it system-wide and add `/bin/wafflehouse-client`; `[y/N]` defaults to **No**.

- Linux / FreeBSD installed executable: `/usr/local/bin/wafflehouse-client`
- Linux / FreeBSD optional system launcher: `/bin/wafflehouse-client`
- macOS optional app install: `/Applications/WaffleHouse-Client.app`
- macOS optional command launcher: `/usr/local/bin/wafflehouse-client`

See `BUILDING-Linux.md`, `BUILDING-FreeBSD.md`, and `BUILDING-macOS.md` for platform notes.

## 5.0r14 highlights

- Added per-account **AIM/OSCAR debug / audit** modes: `Off`, `Login Audit`, and `Full Wire Trace`.
- Login Audit decodes the authentication and BOS bootstrap path, including FLAP greeting, BUCP challenge/login, response TLVs, redirect host, supported-family negotiation, rate negotiation, and legacy FLAP channel-4 signoff errors.
- Full Wire Trace records FLAP channel/sequence/length and SNAC family/subtype/flags/request IDs plus payload detail while redacting passwords, BUCP password hashes/challenge material, service cookies, and OSCAR Admin password fields.
- AIM/OSCAR idle is explicitly verified as **native OSCAR**: Generic Service SNAC `0x0001/0x0011` with a big-endian 32-bit idle-seconds value; `0` advertises Active/Back. Automatic idle only sends on state transitions.
- Added `--oscar-debug off|login|full` and matching GUI/CLI persisted account settings. Legacy OSCAR profiles using generic debug migrate to full wire tracing.
- Retains the 5.0r11 explicit Asterisk server selector (`Auto / generic SIP`, `Asterisk PJSIP / chan_pjsip`, `Asterisk legacy SIP / chan_sip`) and macOS Homebrew Qt deployment fixes.

See `OSCAR-DEBUG.md` for the audit fields and secret-redaction policy.

## 5.0r8 highlights

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

## 5.0r14 selectable protocol build

The top-level `./build.sh` asks which desktop OS is being built and, in interactive mode, which components to include: AIM/OSCAR, IRC, Telnet/BBS, SIP/VoIP, and Media Player/Radio. All are enabled by default. Non-interactive builds can use `--protocols aim,irc,telnet,sip,media` (or `--protocols all`). Omitted protocols are hidden/rejected by the GUI and CLI in that build.

SIP remains multi-account: multiple saved SIP registrations may be connected simultaneously. Each account keeps its own registrar, authentication, transport/local-port settings and Asterisk compatibility mode (`chan_pjsip` or legacy `chan_sip`), while the selected account supplies the outbound identity.

Telnet/BBS profiles can store the expected terminal columns/rows. GUI mode auto-fits a fixed-width font to preserve that exact character grid; CLI mode sends the configured NAWS size and, where supported, requests the terminal emulator resize its outer window so the BBS pane fits exactly.

AIM/OSCAR idle reporting uses the protocol's native Generic Service idle notification. WaffleHouse reports elapsed local idle seconds but does not create its own automatic Away message or threshold. Manual/server Away and AFK states remain distinct. OSCAR debug/audit modes (`off`, `login`, `full`) remain available; see `OSCAR-DEBUG.md`.
