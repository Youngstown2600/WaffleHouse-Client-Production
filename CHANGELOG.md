# WaffleHouse-Client 3.1 — Secure AIM/IRC rooms + Communications Hub cleanup — 2026-08-20

- Added CPX secure-room mode to AIM/OSCAR chat rooms and IRC channels. Room traffic is encrypted with XChaCha20-Poly1305 and appears on the public room transport as `[[CPXROOM1:...]]` ciphertext.
- Added `secure-room-v1` capability negotiation. Shared room keys are never posted in the room; they are distributed individually inside already-established CPX encrypted private-message sessions.
- Added GUI room security controls and CLI `/secure`, `/securestatus`, and `/secureoff` room behavior. Secure-room plaintext is tagged `[secure-room]`; ordinary traffic received while a room key is active is tagged `[plaintext]` for easy verification.
- Added automatic room-key rotation by the key owner when membership changes, with redistribution to current secure peers.
- Renamed the main sidebar **Messages** item to **Communications**.
- Removed Softphone navigation, quick-dial controls, SIP contacts/calls, and Softphone actions from the main GUI/Buddy List. Full SIP/VoIP functionality remains available through **Tools → Open Softphone**, the tray menu, Accounts/Connections, and the CLI phone commands.
- Added `src/secureroom.*` and `tests/secure_room_31_test.sh`; complete source regression gate passes 26/26 tests.

# WaffleHouse-Client 3.0r2 — Branded application + tray artwork — 2026-08-20

- Embedded the WaffleHouse-Client badge artwork as the GUI application/window icon and splash logo.
- Replaced the generic `internet-chat` tray icon with the bundled multi-resolution WaffleHouse-Client icon, retaining safe Qt/theme fallbacks.
- Installed hicolor desktop icons from 16x16 through 512x512 on Linux/FreeBSD and updated the desktop entry to `Icon=wafflehouse-client`.
- Kept the CLI terminal-native ASCII branding unchanged; the unified executable and all existing 3.0r2 protocol/SIP/file-transfer behavior remain intact.

# WaffleHouse-Client 3.0r2 — Automatic OSCAR presence + peer version discovery — 2026-08-20

- Added automatic AIM/OSCAR presence management with default Idle-after-5-minutes and Away-after-15-minutes thresholds, automatic return to Online on resumed activity, and protection for manually selected Away/AFK/Idle states.
- Added shared GUI/CLI persistence for automatic-presence enable/disable and Idle/Away thresholds.
- Added optional X11 workstation-idle detection through `xprintidle`, with a portable WaffleHouse-input fallback when global idle time is unavailable.
- Added `/version [USER]` peer discovery: standard CTCP VERSION on IRC and an invisible WaffleHouse control exchange on AIM.
- Added exact 3.0r2 reporting, legacy CPX3-compatible AIM detection, and explicit timeout messaging when an older/non-WaffleHouse peer cannot report an exact version.
- Added dedicated 3.0r2 regression coverage while preserving all unaffected CPX, file-transfer, Telnet/BBS, SIP/PJSIP, notification, and modern GUI/CLI behavior.

# WaffleHouse-Client 3.0r1 — Guided transfers + softphone/CLI polish — 2026-08-20

- Tightened default GUI window dimensions and shared spacing so Main, Softphone, Connections, chat/IM, and Transfers use substantially less desktop space while remaining resizable.
- Removed the redundant Main-screen Quick Actions card and Options button; account actions now live in the right-click account menu and Settings remains in the left rail.

- Added right-click account context menus on the Main Accounts & Buddies tree and Connections list for protocol-aware connect/disconnect, IM/chat, buddy/contact, Softphone, and edit actions.

- Refined the Softphone Phone page with a centered real telephone-style keypad: equal 72x72 keys, phone letter legends, circular theme-aware styling, Caller ID above routing, and Prefix before Destination.
- Reduced default/minimum sizes across the main window, Connections/Buddy List, Softphone, chat/IM, transfer monitor, and large utility dialogs while keeping all windows resizable.

- Added a guided **Secure / Unsecured** file-transfer choice to the existing GUI Send File action. Secure mode retains CPX encryption/authentication and explains session setup when needed; unsecured mode uses ordinary AIM/IRC PM transport while preserving chunking/resume and SHA-256 completion verification.
- Reworked CLI `/sendfile` into a curses transfer form with recipient, file path, **Secure transfer** toggle, and an **F2 file browser**.
- Added mode-aware transfer routing/accept/resume/cancel handling so replies remain on the transfer's original secure or unsecured transport.
- Moved the CLI keyboard shortcut rail below the main-screen separator and gave it a theme-derived secondary accent on every theme.
- Modernized the Softphone window with a WaffleHouse-style left navigation rail and a phone-like Main page with Prefix, Destination, Caller ID, a 12-key dial pad, Call/Hang Up, and live call status.
- Rebranded current release/version surfaces to **3.0r1** and replaced the stale GUI splash version literal with the live application version string.
- Preserved the 2.5.4-r6 AIM/IRC/Telnet/BBS/CPX/SIP/file-transfer engines; 3.0r1 changes are limited to presentation/controllers plus the new unsecured wire wrapper.


## WaffleHouse-Client 3.0 — shared notification sounds

- Adds shared GUI/CLI notification audio for IRC channel mentions, IRC private messages, AIM instant messages, and AIM chat messages.
- Ships original WaffleHouse notification WAVs and lets each event use Built-in, Custom file, or None.
- GUI Options includes per-event enable/source/browse/test controls.
- CLI adds `/notifications`, `/notify on|off`, `/sound EVENT builtin|off|PATH`, and `/soundtest EVENT`.
- Notification settings live in the existing WaffleHouseClient QSettings store, so GUI and CLI automatically share them.
- Incoming-message classification suppresses local outgoing echoes and only rings IRC channels when the current nickname is mentioned.
# WaffleHouse-Client 3.0 — GUI sidebar polish — 2026-08-20

- CLI startup polish: suppresses PJSIP/pjlib bootstrap console chatter emitted before `EpConfig::logConfig` takes effect, eliminating raw `sip_endpoint`/`pjlib` lines between the splash screen and Main. PJSIP file logging and SIP diagnostics remain enabled.

- Moved **+ New Connection** out of the Communications Hub top bar and into the lower left sidebar directly above Settings.
- Restyled **Settings** as a normal application button instead of a transparent navigation item.
- Removed the **UNIFIED CLIENT** status pill from the dashboard header.
- No protocol, SIP, security, file-transfer, BBS, or CLI behavior changed.

# WaffleHouse-Client 3.0 — 2026-08-20

- Rebuilt the main Qt interface as a modern communications dashboard with persistent navigation and card-based Accounts/Buddies, Softphone, and Quick Actions areas.
- Rebuilt the Connections window with modern card-based profile/activity sections.
- Added a shared `ModernStyle` Qt design system so all GUI windows inherit consistent modern controls, spacing, rounded surfaces, status treatments, and the existing theme family.
- Modernized the ncurses CLI chrome, active-session strip, status indicator, and command prompt while preserving the established shortcut/status/input footer contract.
- Kept the 2.5.4-r6 protocol/security/transfer/SIP/BBS core unchanged during the 3.0 interface rebuild.
- Retained all 15 pre-3.0 regression tests and added 3.0 modern-shell/core-preservation checks.

# WaffleHouse-Client 2.5.4-r6 — 2026-08-20

- Added native AIM/OSCAR Away and Idle presence transmission.
- Added custom AFK status/message, carried over classic OSCAR as an Away message prefixed with `[AFK]` for compatibility.
- Added CLI `/away`, `/afk`, `/idle`, `/back`, and `/status`.
- Added GUI Accounts -> AIM account -> Set AIM Status / AFK.
- AIM status bars/account rows now show Away/AFK/Idle state.

# WaffleHouse-Client 2.5.4-r5 — 2026-08-20

- AIM/OSCAR `/join ROOM` and `/j ROOM` now invoke the private-room exchange path used by `/joinprivate`.
- IRC `/join` and `/j` retain normal public-channel behavior.
- `/j` remains a single-dispatch alias of `/join`; help text now documents protocol-aware behavior.

# WaffleHouse-Client 2.5.4-r4 — 2026-08-20

- Fixed CLI `/j` shorthand so it is actually registered in slash-command dispatch, tab completion, and help.
- `/j ROOM` now executes the exact same AIM chatroom / IRC channel join path as `/join ROOM`.

# WaffleHouse-Client 2.5.4-r3 — 2026-08-20

- Folded upgrade/uninstall lifecycle into `build.sh`: `--upgrade`, `--uninstall`, `--remove-only`, `--yes`, prefix auto-detection, build-before-replace, legacy launcher cleanup, and preserved user configuration.
- Reduced `client-up.sh` to a compatibility wrapper around `build.sh --upgrade`.
- Simplified the CLI status bar to `[time] [screen#:buffer] [state]`, removing the redundant unnumbered selected-connection label.
- The global Status screen now remains `IDLE` instead of borrowing the selected account's offline/online state.

# WaffleHouse-Client 2.5.4-r2 — 2026-08-20

- Fixed restored QSettings profiles being incorrectly marked transient in 2.5.4-r1.
- `/connections` and `/accounts` again enumerate all saved profiles after startup.
- `saveConnections()` once again retains restored profiles instead of omitting them from the persistent array.
- Added a saved-profile persistence regression test covering the load/list/save contract.

# WaffleHouse-Client 2.5.4-r1 — 2026-08-19

- Fixed Telnet `/disconnect` calling `waitForDisconnected()` after an immediate socket close, which caused Qt to print `QAbstractSocket::waitForDisconnected() is not allowed in UnconnectedState` directly into the ncurses screen.
- Added a forced ncurses physical-screen repaint after backend disconnect so the CLI immediately repairs itself if any library writes outside curses.

# WaffleHouse-Client 2.5.4 — 2026-08-19

- Added `/active` to list only connecting/online sessions with their CLI screen numbers.
- Added `/accounts` as an alias of `/connections`.
- Changed `/telnet HOST[:PORT]` to a true ephemeral quick-connect; it is never persisted as a saved account.
- Preserved Telnet/BBS terminal screens after disconnect until the user explicitly runs `/close`.
- Offline preserved BBS buffers return to normal slash-command editing so `/close` remains usable.
- Added password/passphrase prompt detection so the CLI input mirror masks sensitive BBS input with `*`.
- Removed backend raw-byte local echo to avoid duplicate BBS input and accidental password disclosure.
- Fixed duplicate insertion of CLI connection entries and excluded ephemeral sessions from persistent settings.
- Synchronized stale build/version labels to 2.5.4.

# WaffleHouse-Client 2.5.3 — 2026-08-19

- Fixed classic BBS ANSI text that could disappear in the CLI when a board used SGR bold/bright-black (`1;30`) for gray labels: ncurses now uses explicit bright color indexes when the terminal supports them instead of relying only on `A_BOLD + COLOR_BLACK`.
- Kept bright/normal ANSI color pairs distinct so later cell rendering cannot collapse gray/bright text back into normal black.
- Hardened the ANSI model with correct ED (`CSI J`) cursor semantics plus HPA/HPR/VPA/VPR, ECH, ICH, DCH, IND, NEL, and reverse-index handling.
- Kept Telnet/BBS NAWS fixed at 80x24 from the CLI even when the surrounding terminal window is resized.
- Raw BBS keystrokes are still sent immediately, but printable text/backspace is mirrored in the CLI input row so the operator can see the command being typed; Enter clears the local mirror for the next command.
- Merged the complete S.I.P.H.E.R. GUI/CLI theme collection into WaffleHouse while retaining WaffleHouse-only themes. Added Solarized, Nord, Ocean, Retro Blue, Monochrome, Blue Box, Red Box, Beige Box, 2600, WarGames, CRT Green, VT220, Cobalt, and Stealth.
- Added CLI `/themes` and `/theme NAME`; selections persist through the existing options store.

# WaffleHouse-Client 2.5.2-r1 — 2026-08-19

- Removed the unused Telnet terminal buffer local in `TerminalUi::onConnected()`.
- No functional behavior changed; this only eliminates the `-Wunused-variable` warning during CLI compilation.

## 2.5.2 ANSI/BBS terminal + explicit connection workflow — 2026-08-19

- Replaced Telnet ANSI-stripping transcript behavior with a shared ANSI screen model for GUI and ncurses CLI.
- Added CP437 box/block character decoding, cursor movement, screen/line erase, SGR color/attribute handling, CR/LF/backspace/tab semantics, save/restore cursor, and fixed-screen rendering.
- Added raw BBS keyboard input (Enter, Backspace, Escape, arrows, Home/End, Delete, PageUp/PageDown in GUI; raw printable/control input in CLI while the BBS buffer is active).
- Added multi-BBS list import from GUI and CLI `/bbsimport` with CSV/TSV/JSON/pipe/simple text formats and duplicate suppression.
- Added CLI `/telnet HOST PORT` and `/telnet HOST:PORT` ad-hoc sessions.
- Added `/connect PROTOCOL:name` matching such as `/connect AIM:nexus`; connecting explicitly opens/populates the saved profile status buffer.
- New connections and imported BBS entries are save-only and no longer auto-connect.
- CLI live connection strip now hides inactive saved profiles; startup no longer creates status buffers for every offline profile.

## 2.5.1 CLI status-line polish — 2026-08-19

- Reworked the ncurses bottom layout to use an Irssi-style full-width status bar directly above the input prompt.
- The status bar shows the current HH:mm clock, selected connection, active buffer number/name, connection/registration state, and aggregate unread count.
- Removed the old bottom help footer so the input prompt occupies the final terminal row, matching the classic Irssi layout.
- The status bar is theme-aware and refreshes with the normal TUI redraw loop.
- Added `tests/cli_irssi_statusbar_test.sh` to lock the bottom-row layout and status-line content in place.

## 2.5.1 SIP account hotfix — 2026-08-19

- Fixed a CLI Add-SIP stack-overflow/segmentation fault caused by recursive SIP account initialization from the synchronous backend error handler.
- SIP session-password clearing no longer re-enters PJSUA2 account creation or account reconfiguration.
- PJSUA2 `pj::Error` exceptions are surfaced with their native operation/reason text instead of the generic "Unknown SIP/PJSUA2 error" message.
- Optional SIP registrar and outbound-proxy hostnames are normalized to SIP URIs automatically.
- Added `tests/sip_251_error_recovery_test.sh` to guard the failed-account recovery path.

# WaffleHouse-Client Changelog

- SIP hotfix: never call PJSIP runtime STUN update with an empty server vector; PJSIP 2.17 asserts on that input.
- CLI hotfix: restored/new SIP connections are inserted into SipController immediately, so the Softphone account count is correct before /connect.

## 2.5.1 — 2026-08-19

- Fixed the GUI crash path when adding a SIP account by deferring SIP/PJSUA2 account insertion until the new backend is fully attached to WaffleHouse state and signal handlers.
- Hardened SIP account add/start/register/update/disconnect error boundaries so unexpected PJSUA2 exceptions are converted into GUI-visible errors instead of escaping through Qt event handling; failed account insertion also rolls back controller state.
- Removed **Add SIP** and **Open Softphone** from the main Buddy List. Softphone remains available from **Tools → Open Softphone**.
- Removed the top-level **Phone**, **Buddies**, and **Conversation** menus.
- Renamed **Account** to **Accounts** and rebuilt it dynamically with one submenu per saved connection (for example `nexus / AIM/OSCAR`).
- Moved **Show Connections Window**, **Change AIM Password**, and **Secure Identity Fingerprint** to **Tools**.
- Added per-account **IM / Chatroom…** launchers for AIM/OSCAR and IRC. The new unified window contains both private-message and room/channel options in tabs.
- Added per-account **Add / Remove Buddies…** management windows for AIM/OSCAR and IRC, plus local **Buddies / Contacts** management for SIP dial targets.
- SIP contacts are saved with the SIP connection, shown below that account in the Buddy List, and can be double-clicked to populate the quick-dial field.
- Preserved IRC buddy/watch and SIP contact lists when editing connection settings and across GUI/CLI settings round-trips.
- Retains the complete S.I.P.H.E.R. r14 softphone-relevant synchronization, including r12 live headset switching, r13 per-PBX dial prefixes, and the guarded r14 FreeBSD HDA compatibility builder.

## 2.5 Alpha — 2026-08-19

- Promoted SIP/VoIP to a first-class WaffleHouse connection protocol in both GUI and CLI.
- Added multiple concurrent saved SIP accounts and independent registration state inside one shared PJSIP endpoint.
- Added SIP/VoIP to the normal GUI Add Connection dialog and CLI `/add` form, including registrar, authentication, caller-ID, transport, STUN, ICE, SRTP, dial-prefix, and registration settings.
- Reworked the Buddy List with a Softphone account selector, quick-dial field, Softphone launcher, and active calls displayed beneath the owning SIP account.
- Unified the Softphone Profile tab with WaffleHouse's saved connection store; there is no separate softphone profile database.
- Reworked CLI phone commands around the selected WaffleHouse SIP connection: `/select`, `/connect`, `/disconnect`, `/dial`, `/phoneprofile`, and `/phoneconfig`.
- Added account identity to call snapshots and call tables so inbound/outbound calls remain associated with the correct SIP account.
- Configured managed PJSIP 2.17 with `PJSUA_MAX_ACC=32` and bumped the managed-build stamp to force one safe dependency rebuild.
- Retained all existing AIM/OSCAR, IRC, Telnet/BBS, CPX secure messaging/file-transfer, themes, and dual GUI/CLI behavior.
- Continued to exclude S.I.P.H.E.R. audit/security testing, queue/blast tooling, and SIP/RTP PCAP capture.
- Synced the integrated softphone through **S.I.P.H.E.R. r14** while retaining WaffleHouse's multi-account architecture.
- Retained r12 Linux/FreeBSD live headset/audio-route switching.
- Added r13-style runtime **per-account PBX dial prefixes** to the Buddy List, Softphone Main view, and CLI; saved connection prefixes remain startup defaults. Added `/prefix`, `/dialraw`, and `/dialpreview`.
- Added the r14 conservative FreeBSD `snd_hda` Speaker/Headphones compatibility detector and guarded runtime test/persistence workflow to `build.sh`. Automatic repair is limited to one fixed Speaker + one jack Headphones + no analog Line-out, uses headphone sequence 15, skips existing custom HDA policy, validates playback/capture PCMs, rolls failures back immediately, and persists only after a successful test with a timestamped `/boot/device.hints` backup.
- Added read-only `./build.sh --audio-diagnose` and automatic-repair opt-out `--no-audio-fix`.

## 2.4 Alpha — 2026-08-19

- Added a shared PJSIP 2.17 softphone engine to the unified WaffleHouse-Client without removing AIM/OSCAR, IRC, Telnet/BBS, CPX secure messaging, file transfer, themes, saved connections, or existing GUI/CLI behavior.
- Added inbound and outbound SIP calling, answer/reject/hangup, hold/resume, DTMF, mute, caller-ID selection, registration state, and shared SIP profiles.
- Added GUI softphone tabs: Main, Active Call, SIP Log, SIP Ladder, Profile, and Activity.
- Added matching CLI softphone commands and status views, including `/phone`, `/dial`, `/calls`, `/answer`, `/reject`, `/hangup`, `/hold`, `/callresume`, `/dtmf`, `/siplog`, `/ladder`, and audio-device controls.
- Preserved the r12 Linux/FreeBSD live audio-device route/hot-swap behavior for headset plug/unplug changes during an active call.
- Deliberately excluded the S.I.P.H.E.R. PBX audit/security suite, queue/call-blast tooling, and SIP/RTP PCAP capture subsystem.

## 2.3 Alpha — 2026-08-15
- Fixed IRC `/raw` command normalization: client-style inputs such as `/raw /part #channel` no longer send an invalid leading slash to the server.
- Added `/part [#channel]` to WaffleHouse-CLI; bare `/part` and `/raw /part` now leave the active IRC channel cleanly.
- Expanded GUI and CLI theme library with Cyberpunk, Synthwave, Dracula, Vaporwave, Blood Moon, C64, DOS, Solarized Dark, Waffle Iron, Ghostline, Hot Dog Stand, and Neon Miami.
- CLI palettes use 256-color accents when supported and automatically fall back to standard terminal colors.
- Promoted the unified client release line from 2.2 Alpha to **WaffleHouse-Client 2.3 Alpha**.
- Updated GUI, CLI, build, install, updater, version output, CMake metadata, and documentation branding to 2.3 Alpha.
- Carries forward the proven 2.2 secure direct-transfer implementation, Resume/Clear/Cancel controls, dedicated transfer window, terminal-input fixes, and Linux/FreeBSD dependency/toolchain checks.
- CLI frontend now identifies itself consistently as **WaffleHouse-CLI 2.3 Alpha**.

## 2.2 Alpha — 2026-08-15
- Added File Transfers window resume/clear controls: cancelled or interrupted uploads/downloads now expose **Resume** and **Clear** actions.
- Successful transfers automatically clear from the active transfer list shortly after SHA-256 completion while remaining in the transfer log.
- Resume reuses the saved `.cpxpart` offset and renegotiates direct encrypted transport when available, falling back to secure relay if needed.
- Clearing a cancelled download removes only its `.cpxpart` partial; successfully downloaded files and original upload files are never deleted.
- Added CLI `/resume ID` and `/cleartransfer ID` equivalents for unified GUI/CLI parity.
- Audited all ncurses input paths for Linux/FreeBSD terminal-key variations. Text-entry fields now consistently treat `KEY_BACKSPACE`, ASCII BS (`0x08`), DEL (`0x7F`), and the terminal-configured erase character as Backspace.
- Enabled keypad/meta decoding on every interactive popup/form window and normalized keypad Enter/Esc handling, preventing raw escape/control sequences from leaking into modal input.
- Fixed cross-platform compilation of the File Transfers progress calculation on Qt 6/GCC and Qt 6/Clang.
- Added CPX `file-direct-v1` encrypted TCP payload transport for large files while keeping AIM/IRC as the secure control channel.
- Added automatic direct-to-relay fallback with receiver-authoritative resume offsets when direct connectivity is unavailable or interrupted.
- Added per-transfer **Cancel** buttons in the GUI File Transfers window; cancelling a transfer leaves the chat/server connection online and preserves safe partial data for resume.

- Bumped the unified C++ client from 2.1 Alpha to **2.2 Alpha**.
- Added a standalone GUI **File Transfers** window with per-transfer upload/download progress, peer/file/status columns, and a timestamped log.
- Removed GUI file-transfer progress/status messages from IM/chat transcripts.
- Added **View → File Transfers** and automatic display when a transfer begins or an incoming offer arrives.
- Made pre-key-exchange CPX CAPS races silent and deferred until HELLO completes, preventing benign stale capability frames from surfacing as secure-channel errors.
- Carries forward reliable ACK/retry/resume/SHA-256 transfer behavior and the Linux/FreeBSD dependency and ABI preflight builder.
- Added `client-up.sh`, a Linux/FreeBSD upgrade helper that removes the prior installed binary/desktop entry, preserves user configuration, and clean-builds/installs the current release.

## 2.1 Alpha — 2026-08-14

- Current unified WaffleHouse-Client Alpha release.
- Version branding and build metadata advanced from 2.0 Alpha to 2.1 Alpha.
- Added explicit opt-in saved connection passwords shared between GUI and CLI.
- Retains Linux/FreeBSD portable build, install, and stale-CMake-cache handling.
- FreeBSD builds explicitly require GCC/G++ and GNU Make (`gmake`); `build.sh` validates GCC/G++ as auxiliary prerequisites, uses `gmake`, and compiles the Qt application with FreeBSD Clang/libc++ for ABI compatibility.
- `build.sh` now performs a dependency preflight and automatically installs missing build packages on supported Linux package managers and FreeBSD `pkg`.
- Fixed FreeBSD CMake curses detection by adding `ncurses` to automatic dependencies and linking the packaged wide-character `ncursesw` interface through `pkgconf`.
- Added post-install dependency verification and the `--no-auto-deps` opt-out; automatic dependency installation is enabled by default.

## 2.0 Alpha — 2026-08-14

- First unified C++ WaffleHouse executable.
- Runtime auto-selection between Qt GUI and ncurses CLI.
- `--gui` and `--cli` force-mode switches.
- Shared saved connection profiles and CPX identity/trust state between frontends.
- Imported 1.9.1 Beta AIM/OSCAR, IRC, Telnet, secure-DM and secure-file-transfer features.
- Included IRC buddy/watch list and nickname Tab completion.
- Included relocated-source CMake cache protection.

### 2.0 Alpha build/install update — 2026-08-14

- Expanded `build.sh` to match the 1.9.1-style build/install workflow for the unified executable.
- Added `--clean`, `--dry-run`, `--install`, `--no-install`, `--prefix`, `--jobs`, and `--build-type` options.
- Kept Linux and FreeBSD host detection and portable stale-CMake-cache cleanup.
- Added CMake `GNUInstallDirs` support. The default `/usr/local` prefix installs the executable as `/usr/local/bin/wafflehouse-client` on Linux and FreeBSD.
- Installs the desktop entry under `/usr/local/share/applications` by default.
- Supports unprivileged per-user installs such as `--prefix "$HOME/.local"`.
- Added sudo/doas/su handling only for the final system-install step when required.
- Added a built-in Qt tray-icon fallback so Alpha builds do not emit `QSystemTrayIcon::setVisible: No Icon set` when no theme icon is available.

### 2.1 Alpha saved-password opt-in — 2026-08-14

- Added an explicit **Save password on this computer** option to GUI connection/password prompts.
- Added the equivalent save-password choice to the CLI connection form and runtime password prompt.
- Saved credentials are shared between GUI and CLI because both frontends use the same profile store.
- Password saving remains opt-in and defaults to off.
- The UI documents that QSettings storage is local but not encrypted at rest; unchecking the option stops persistence.


### 2.1 Alpha reliability update — 2026-08-15
- Added `file-ack` CPX capability and reliable stop-and-wait chunk acknowledgments for new peers.
- Large relay file transfers now retransmit lost chunks instead of cancelling on the next offset.
- Duplicate/out-of-order reliable chunks are resynchronized with cumulative ACKs rather than tearing down the transfer.
- Added completion confirmation after receiver SHA-256 verification.
- Paced AIM/OSCAR and IRC file-transfer relay traffic to avoid flooding the chat transport.
- File-transfer failures are contained to the transfer and no longer intentionally close the secure/chat session.
- Duplicate secure HELLO frames no longer reset replay-nonce history.
- Capability frames arriving just before HELLO are deferred instead of reported as a secure-channel error.


- Fixed Linux/GCC and FreeBSD/Clang compilation of direct-transfer key cleanup by keeping the temporary libsodium secret buffer mutable before `sodium_memzero()`.
