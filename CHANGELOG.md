## 5.0r18

- Fix AIM/OSCAR account context-menu **Join AIM Chat…** being incorrectly disabled when BOS does not advertise CHAT_NAV (0x000D) and CHAT (0x000E) directly.
- Treat OSCAR ChatNav/Chat correctly as on-demand redirect services obtained through OSERVICE SERVICE_REQUEST (0x0001/0x0004); the backend already performs those redirects when joining a room.
- Keep runtime failure honest: if a server truly does not provide ChatNav/Chat, the join attempt now reaches the backend and reports the OSCAR service error instead of being preemptively grayed out.
- Clarify the OSCAR Feature Center text so BOS-advertised foodgroups are not confused with separately redirected services.
- Retains the 5.0r17 persistent OSCAR audit viewer and /bin launcher prompt.

## 5.0r17

- Added persistent per-account OSCAR Login Audit / Full Wire Trace files using the same credential-redacted diagnostic stream already shown in Activity.
- Added **Tools → View OSCAR Audit Log…** and an OSCAR account context-menu entry; multiple AIM accounts can be selected and the viewer displays the exact log path.
- Full OSCAR logs rotate at 5 MiB with one `.old` generation; the GUI tails the last 512 KiB for large logs.
- Linux/FreeBSD builders explicitly ask after a successful build whether to install WaffleHouse-Client system-wide and add `/bin/wafflehouse-client`; Enter/default remains No.
- `/bin/wafflehouse-client` is a launcher for the normal prefix-installed executable, keeping shared desktop/icon/resource installation in the correct prefix instead of scattering assets under `/share`.
- Retains the 5.0r16 BuildFeatures compile fix, 5.0r15 IM composer-focus fix, and 5.0r14 native OSCAR presence/profile hardening.

## 5.0r15

- Fixed new IM/chat windows opening with keyboard focus in the read-only transcript instead of the message composer.
- Transcript uses mouse-only focus for selection/copying; first-show focus is queued onto the message input after window activation.
- Added a regression gate for IM composer focus behavior.

## 5.0r14

- Fixed native AIM/OSCAR buddy presence decoding: BUDDY arrival/departure packets now retain away/unavailable, DND, N/A, busy, free-for-chat, invisible, and idle state instead of collapsing every connected buddy to plain Online.
- Buddy-list GUI and CLI now display native OSCAR presence and idle information; Locate user-info refreshes can also supply the buddy away message.
- Buddy capability UUIDs received in native arrival packets are retained using OSCAR's present/absent TLV semantics.
- Added Buddy rights negotiation for initial departure notifications without falsely advertising unsupported BART/icon support.
- Locally saved AIM profiles are persisted in the account and replayed after BOS login, working around revival/private servers that keep LOCATE profile data only for a session.
- Native AIM chat capability is now advertised because WaffleHouse implements OSCAR chat rooms; unsupported legacy Talk/Direct-IM/File/BART capabilities remain detection-only and are not falsely advertised.
- Added a native OSCAR feature regression audit covering idle, away/back, profile TLVs/replay, Query2 user info, rich buddy presence, SSI/auth, privacy, chat, typing/stored messages, account admin, and capability gating.
- BUDDY arrival/departure notifications now consume every batched UserInfo record instead of stopping after the first buddy.
- Added native host-event handling for OSERVICE own-user-info updates, OSCAR warning/evil-level changes, and BUDDY rejected-watch notifications.

## 5.0r13

- Builder feature selection: AIM/OSCAR, IRC, Telnet/BBS, SIP/VoIP, and Media can be selected per build.
- GUI/CLI hide and reject protocols omitted by the selected build profile.
- Telnet/BBS profiles store exact rows/columns and auto-fit the GUI font; CLI requests a matching outer terminal size and advertises exact NAWS dimensions.
- SIP multi-account behavior is retained: multiple registrations can stay active concurrently against different PBXs, including Asterisk chan_pjsip and legacy chan_sip compatibility profiles.
- AIM/OSCAR idle handling now reports real idle seconds through native OSCAR OSERVICE idle signaling; WaffleHouse no longer invents client-side auto-away thresholds/messages.
- OSCAR login/full wire debug and credential redaction from 5.0r12 are preserved.

## 5.0r12

- Added AIM/OSCAR diagnostic levels: **Off**, **Login Audit**, and **Full Wire Trace**.
- Login Audit traces TCP/FLAP/BUCP/BOS bootstrap, server families, version/rate negotiation, redirects, and decoded authorization failures.
- Full Wire Trace records every OSCAR FLAP/SNAC header and payload while redacting passwords, password hashes, challenge material, and auth/service cookies.
- Legacy FLAP channel-4 authorization failures are decoded instead of collapsing to a generic `server signed off` message.
- Confirmed and hardened native OSCAR idle signaling via `OSERVICE__IDLE_NOTIFICATION` SNAC `0x0001/0x0011` with a 32-bit idle-seconds payload; zero advertises active.
- Added explicit native-idle audit lines and OSCAR debug settings to GUI and CLI account editors.

## 5.0r11

- macOS: fixed post-link `macdeployqt` deployment with Homebrew's split Qt 6 module kegs by supplying module `-libpath` directories, including QtSvg and the QtPdf-containing qtwebengine keg.
- SIP account setup now visibly asks which remote server type is in use: Auto/generic SIP, Asterisk PJSIP (`chan_pjsip`), or legacy Asterisk `chan_sip`.
- CLI accepts the friendly aliases `pjsip`, `chan_pjsip`, and `chan_sip`. WaffleHouse continues to use PJSIP 2.17 internally in every mode.

# WaffleHouse-Client 5.x Changelog

## 5.0r11

- Fixed macOS PJSIP static-link handling so pkg-config framework pairs such as `-framework CoreServices` are passed to AppleClang as frameworks instead of being misinterpreted as `-lCoreServices`.
- Framework normalization is generic for every framework advertised by PJSIP, avoiding the same failure if another macOS framework appears in the static link metadata.
- Removed the unused `this` capture in the Search History clear-button lambda reported by AppleClang.
- Retains all 5.0r9 Asterisk `chan_sip` compatibility behavior unchanged.

## 5.0r9

- Rebuilt from the user-supplied 5.0r8 desktop bundle as the source-of-truth baseline.
- Added legacy Asterisk `chan_sip` server compatibility mode while retaining PJSIP 2.17 as WaffleHouse-Client's client-side SIP stack.
- Added `auto`, `standard`, and `asterisk-chan_sip` compatibility choices to GUI and CLI SIP account editors.
- `asterisk-chan_sip` mode disables RFC 5626 SIP-Outbound behavior and enables legacy-safe Contact/Via rewriting for older Asterisk registrars.
- SIP account identity now honors the configured Caller-ID domain independently of the registrar/domain, useful for older PBXs and trunks.
- Digest authentication continues to follow the registrar-provided realm using a wildcard realm credential.
- Added regression coverage for the chan_sip compatibility path.

## 5.0r8
- Fixed the macOS builder so optional media tools cannot block compilation of the client. `mpv` and `ffmpeg` are now runtime-only optional dependencies; failure to install either one is non-fatal.
- Removed the stale macOS `yt-dlp` build dependency; the 5.0r8 media path does not use the removed YouTube resolver.
- Added `--with-media-deps` for users who explicitly want the builder to attempt Homebrew installation of `mpv`/`ffmpeg`. A Homebrew “no bottle available” error now produces a warning and the WaffleHouse build continues.
- macOS runtime discovery now checks Homebrew, MacPorts, Fink, and `/Applications/mpv.app` locations so Finder-launched `.app` bundles can find `mpv`/`ffmpeg` even when those directories are absent from the GUI process PATH.

## 5.0r7
- Combined the Linux/FreeBSD and macOS desktop releases into one shared source bundle. The top-level builder now asks the user to select Linux, FreeBSD/Unix, or macOS before any platform build/dependency work begins.
- Normal builds on every desktop platform ask before installing WaffleHouse-Client into a system bin directory; Enter/default remains No.
- Synchronized macOS from the 5.0r3 platform package to the complete 5.0r6 application source, bringing over the 5.0r4 typing-window fix, 5.0r5 OSCAR transfer/window/rate fixes, and 5.0r6 secure-direct finalization/profile-ID fixes.
- Fixed the macOS status/menu-bar icon white-on-white regression by explicitly keeping the WaffleHouse icon non-template/full-color and removing the QSystemTrayIcon synchronously during application quit.
- Bundled built-in notification sounds inside the macOS `.app` so an installed/moved app does not depend on the extracted source directory.

## 5.0r6
- Fixed secure CPX direct downloads that reached 100% but remained as `*.cpxpart` while the receiver stayed on “Receiving direct” and the sender stayed on “Verifying”.
- Corrected GUI file-transfer ownership lookup: transfer records store the stable connection profile ID, but several completion/cancel/resume/direct callbacks incorrectly looked it up as a transient backend ID.
- Direct receive completion now always performs local SHA-256 verification and final rename even if the account/control-channel object becomes temporarily unavailable; peer completion confirmation is sent when the owning account is available.
- Hardened the encrypted direct socket sender so a queued final frame can drain and close proactively instead of depending on the receiver to close first.
- The same profile-ID lookup correction also restores GUI cancel, resume, decline, direct-progress peer naming, and direct-fallback handling for file transfers.

## 5.0r5
- Fixed AIM/OSCAR Send File from the buddy list so launching a transfer does not create an IM window.
- File-transfer OFFER/ACCEPT/DATA/ACK/DONE/COMPLETE control traffic is now consumed as transport traffic and does not create GUI conversations.
- Secure file-transfer payloads are recognized before an IM window is created, preventing the same pop-up behavior on encrypted transfers.
- Corrected OSCAR rate-class reply parsing (supports both 30-byte and 35-byte layouts) and tuned relay transfers to use larger payloads at a roughly two-second cadence, reducing ICBM rate-limit pressure that could stall downloads after they started.
- Extended reliable-transfer ACK retry tolerance and kept Send File available from an online IM even without an active secure session.

## 5.0r4
- Fixed OSCAR typing notifications so they stay in the normal AIM IM window instead of opening a separate transient "typing" conversation window.
- The existing IM header now remains the single typing-state indicator (typing, paused, or cleared).

## 5.0r3
- Pruned the release archive to platform-required source, assets, builders, and current documentation only.
- Removed other-platform adapters/builders, historical 3.x release artifacts, examples, companion payloads, and internal regression-test trees from the customer-facing archive.
- Tailored CMake configuration specifically to the Unix/Linux bundle so it no longer exposes dead build branches for other platforms.

## 5.0r2
- Added conventional GUI application exit: File → Exit on Linux/FreeBSD with Ctrl+Q.
- Uses the existing clean shutdown path; tray-close behavior remains unchanged.

## 5.0r1
- Redesigned the Softphone around a combined Phone workspace.
- Added a dedicated SIP Accounts left-navigation tab.
- Exposed Answer, Reject, Hang Up, Hold, Resume, Mute, Blind Transfer, Attended Transfer, DTMF, and Diagnostics in the GUI.
- Widened the Softphone navigation rail to prevent clipped labels.
- Cleaned GCC OSCAR initializer and misleading-indentation warnings.

## 5.0
- Introduced the 5.x unified desktop application/core design, unified contacts/history/capabilities, expanded SIP diagnostics and transfer controls, and command-palette support while preserving AIM/OSCAR, IRC, Telnet/BBS, media, secure rooms, encrypted transfers, themes, and CLI/GUI operation.
