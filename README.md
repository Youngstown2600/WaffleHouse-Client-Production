# WaffleHouse-Client 5.0r10 — All-Platform Bundle

This release was rebuilt from the user-supplied 5.0r8 desktop bundle as the desktop source-of-truth, then versioned forward to 5.0r10 after applying the SIP compatibility changes.

This archive contains:

- **Linux** desktop GUI + CLI — WaffleHouse-Client 5.0r10
- **FreeBSD / Unix** desktop GUI + CLI — WaffleHouse-Client 5.0r10
- **macOS** desktop GUI + CLI — WaffleHouse-Client 5.0r10
- **Termux / Android** CLI — WaffleHouse-Client-Termux Build 1.2

Run `./build.sh` and choose the target OS, or pass `--os linux`, `--os freebsd`, `--os macos`, or `--os termux`.

## macOS 5.0r10 linker fix

The desktop branch now normalizes PJSIP pkg-config framework pairs before CMake links the application. macOS frameworks such as `CoreServices` and `AudioToolbox` are emitted as `-framework <Name>` instead of being misread as normal `-l<Name>` libraries. This fixes the `ld: library 'CoreServices' not found` failure seen at the final 100% link step.

## Asterisk chan_sip support

All four builds retain PJSIP 2.17 as the **client-side SIP stack** and can connect to standards-based SIP servers regardless of the server's internal channel driver. SIP account settings now expose:

- `auto`
- `standard`
- `asterisk-chan_sip`

Select `asterisk-chan_sip` for older Asterisk servers using `chan_sip`. That mode uses legacy-safe registration behavior, including legacy Contact rewriting and disabling RFC 5626 SIP-Outbound, while retaining Digest authentication and normal SIP/RTP calling.

The account AOR/From identity now correctly honors the configured Caller-ID domain when it differs from the registrar/SIP domain.

## Installation behavior

Building does **not** silently add WaffleHouse-Client to a bin directory. The selected builder asks first. Press Enter at the prompt to leave the application uninstalled and run it from the build output instead.
