# WaffleHouse-Client-Termux Build 1.2

A dedicated Android/Termux CLI build of WaffleHouse-Client. This is **not** the Linux desktop binary with `--cli` forced on: the Termux target has its own entry point and compiles no Qt Widgets/GUI frontend code.

## Asterisk chan_sip compatibility (Build 1.2)

SIP accounts now support `auto`, `standard`, and `asterisk-chan_sip` compatibility modes. The legacy mode keeps WaffleHouse's PJSIP 2.17 client stack but disables SIP-Outbound and uses legacy-safe Contact rewriting for older Asterisk `chan_sip` peers/registrars. Use `asterisk-chan_sip` in the SIP account form when the remote Asterisk box is using `chan_sip`. `auto` keeps the general-purpose client behavior and does not guess a hidden server-side Asterisk driver.

## Automatic Android microphone permission check (Build 1.0+, retained in Build 1.1)

The client now runs the Termux microphone preflight automatically before the TUI starts. When Android capture permission is already working, the check is silent and WaffleHouse opens normally. If permission has not been granted, the probe calls `termux-microphone-record`, which allows Termux:API/Android to present the normal Microphone permission dialog. Grant **Allow while using the app**.

If the check still fails (for example, the Termux:API Android add-on is missing or signed from a different source), WaffleHouse prints recovery instructions and continues starting. SIP remains usable with the Build 0.8+ speaker-only/null-audio fallback instead of blocking AIM/IRC/BBS or SIP signaling. The manual command remains available:

```bash
wafflehouse-audio-preflight
```

Set `WAFFLEHOUSE_SKIP_AUDIO_PREFLIGHT=1` only when deliberately bypassing the startup check for troubleshooting.

## Termux call-control convenience (Build 1.1)

`/hangup` now hangs up the current foreground call without requiring a call ID. `/hangup CALL-ID` remains available when explicitly controlling one of several calls. If multiple calls are live and WaffleHouse cannot determine a foreground call, it asks for the ID instead of guessing. This behavior is specific to the Termux client.

## Preserved features

- AIM/OSCAR accounts, buddy presence, IM and private rooms
- IRC accounts, channels and IRC `/commands`
- Telnet/BBS sessions with ANSI terminal emulation
- SIP/PJSIP 2.17 softphone, multiple accounts, calls, DTMF, hold/mute and SIP diagnostics
- Secure rooms and encrypted direct messaging
- Secure/unsecured file transfer and direct transfer
- Saved profiles/accounts and CLI themes
- Media playback, streams and playlists retained from the supplied 3.1r8 baseline

## Termux-native differences

- CLI only; no desktop Qt Widgets or X11 window is compiled into WaffleHouse
- Qt Core + Network remain because the existing protocol engines are built on them
- `pkg` is used for Termux packages
- no CPAN or Perl command is used by the WaffleHouse builder
- Termux's own `xdg-open`/`termux-open-url` is reused
- shared Downloads storage is used when `termux-setup-storage` has been granted
- PJSIP is built specifically for Termux using the Termux PortAudio/OpenSL ES and libopus packages

## Build

```sh
pkg install unzip
unzip WaffleHouse-Client-Termux-Build-1.2.zip
cd WaffleHouse-Client-Termux-Build-1.2
chmod +x build.sh
./build.sh --clean
```

Then run:

```sh
wafflehouse-client
```

For Android shared storage:

```sh
termux-setup-storage
```

## Builder options

```text
./build.sh --clean       clean application build
./build.sh --pjsip       force rebuild of managed PJSIP 2.17
./build.sh --test        run source/parity tests only
./build.sh --uninstall   uninstall WaffleHouse-Client files
```

## Termux runtime paths

WaffleHouse-Client uses Termux's writable `$TMPDIR` (normally `$PREFIX/tmp`) for SIP logs, sockets, and temporary runtime data. It does not assume Android exposes a writable `/tmp`. If Termux temp variables are unavailable, Android falls back to `$HOME/.cache/wafflehouse-client/tmp`.

## Termux SIP audio / RTP notes (Build 0.8+, retained in Build 1.1)

Build 0.8 introduced the SIP/RTP decoupling from Android hardware audio; Build 1.1 retains it unchanged. PJSIP starts
with a null sound device, so a missing microphone permission can no longer prevent
an INVITE from being sent or RTP/RTCP sockets from being created. Once call media
is negotiated, WaffleHouse attempts full-duplex audio. If microphone capture cannot
open, the call remains alive and WaffleHouse attempts playback-only audio.

For microphone capture, install the **Termux:API Android add-on** from the same
source/signing family as the main Termux app, then grant its Microphone permission.
The `pkg install termux-api` package supplies only the command-line client; it does
not install the Android add-on APK.

After installation, test the Android microphone permission path with:

```sh
wafflehouse-audio-preflight
```

Then use `/audio-devices` and `/audio-reopen` in WaffleHouse. RTP diagnostics remain
available through the Softphone call report/media views and include local/remote
RTP/RTCP addresses, codec, packet counts, loss, jitter, discard, RTT and jitter
buffer delay.

## Responsive Termux interface (Build 0.9+, retained in Build 1.1)

The CLI is now sized from the live Termux PTY rather than an 80x24 assumption. It automatically reflows after phone rotation, soft-keyboard open/close, Android split-screen resizing, and tablet/external-display changes. On short screens optional chrome is removed first; the status/input rows and active conversation remain visible. Telnet/BBS sessions also receive updated NAWS dimensions when the server negotiated NAWS.
