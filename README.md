# WaffleHouse-Client 2.5.1

**WaffleHouse-Client 2.5.1** is an experimental single-executable C++ edition that combines the WaffleHouse Qt GUI and ncurses CLI into one program.

It is based on the stabilized WaffleHouse 1.9.1 Beta C++ code and includes AIM/OSCAR, IRC, Telnet/MUD/BBS, CPX secure messaging, capability negotiation, encrypted file transfer, IRC buddy/watch lists, nickname completion, themes, and the other 1.9.1 fixes.

> This is an **Alpha** development branch. WaffleHouse 1.9.1 Beta remains the current Beta family release.


## 2.5.1 GUI hotfix and account workflow

2.5.1 fixes the GUI SIP-account creation crash by deferring live PJSUA2 account insertion until the new backend is fully attached to WaffleHouse's state and signal model. It also reorganizes the GUI around saved connections:

- **Accounts** contains one submenu per saved connection. AIM/OSCAR and IRC accounts expose **IM / Chatroom…** and **Add / Remove Buddies…**; SIP accounts expose local **Buddies / Contacts** management for reusable dial targets.
- The main **IM / Chat** button opens one tabbed window containing both direct-message and chat-room/channel workflows for the selected AIM/IRC account.
- **Tools** now contains **Open Softphone**, **Show Connections Window**, **Change AIM Password**, and **Secure Identity Fingerprint**. The old top-level Phone, Buddies, and Conversation menus are removed.
- The Buddy List Softphone panel keeps account selection, per-PBX runtime prefix, quick dial, Register, and Disconnect; **Add SIP** and **Open Softphone** buttons are removed.
- SIP contacts are stored locally with each SIP connection and can be double-clicked in the Buddy List to populate quick dial.

## 2.5: native multi-account SIP / VoIP integration

SIP is now a first-class WaffleHouse connection protocol alongside AIM/OSCAR, IRC, and Telnet/BBS. Multiple SIP accounts can be saved and registered concurrently in one process. Both GUI and CLI add/edit SIP accounts through the normal WaffleHouse connection workflow (`Connection > Add...` or `/add`).

The Buddy List now includes a Softphone panel with SIP-account selection and quick dialing, and active SIP calls appear beneath their owning SIP account. The Softphone Profile tab edits the same saved WaffleHouse SIP connection instead of maintaining a second profile database. GUI and CLI share these saved accounts through the existing QSettings connection store.

The managed PJSIP 2.17 build is configured for up to 32 accounts, 64 compiled call slots, and WaffleHouse's 50-call runtime ceiling.

### S.I.P.H.E.R. r12-r14 synchronization

WaffleHouse-Client 2.5.1 carries forward the complete softphone-relevant update chain through **S.I.P.H.E.R. r14**:

- **r12:** live Linux/FreeBSD audio route and headset switching while calls remain active;
- **r13:** a runtime **per-SIP-account / per-PBX dial prefix**. The saved connection's dial prefix is the startup default, while the current prefix can be changed without editing/reloading the account. Explicit `sip:`/`sips:` URIs and `user@domain` destinations are never prefixed;
- **r14:** a conservative FreeBSD `snd_hda` builder compatibility check for the simple laptop topology of exactly one fixed Speaker plus one jack Headphones output and no analog Line-out. On that high-confidence topology only, a normal build can test a same-association headphone layout with sequence 15. Existing custom HDA hints cause an automatic skip, PulseAudio is stopped only when needed for reconfiguration, playback/capture PCM availability is validated, failures are rolled back immediately, and `/boot/device.hints` is changed only after a successful runtime validation with a timestamped backup. More complex hardware remains advisory-only.

Use `./build.sh --audio-diagnose` for a **read-only** audio/HDA diagnostic pass. Use `./build.sh --no-audio-fix` to retain diagnostics while disabling guarded automatic FreeBSD audio repair during a normal build.

## 2.4 Alpha: integrated SIP / VoIP softphone

WaffleHouse-Client now keeps **all existing AIM/OSCAR, IRC, Telnet/BBS, CPX secure messaging, file-transfer, theme, profile, and GUI/CLI features** and adds a full send/receive SIP softphone based on the S.I.P.H.E.R. r12-r14 PJSUA2 softphone line. SIP is an additional communications mode; it does not replace or remove any existing WaffleHouse protocol.

The integrated phone deliberately carries over only the normal softphone/diagnostic workflow:

- outbound and inbound SIP calls;
- answer, reject, hang up, hold, resume, microphone mute, DTMF, foreground-call selection;
- codec/media state and active-call status;
- SIP registration profiles with UDP/TCP/TLS, caller-ID/identity mode, dial prefix, ICE, SRTP, STUN, outbound proxy, and local SIP port;
- live Linux/FreeBSD audio-device following and headset hot-swap during an active call;
- **Main**, **Active Call**, **SIP Log**, **SIP Ladder**, **Profile**, and **Activity** views in the GUI;
- equivalent CLI commands/buffers (`/phone`, `/calls`, `/siplog`, `/ladder`, `/phoneprofile`, `/phoneactivity`, and call-control commands).

The S.I.P.H.E.R. PBX audit/security suite, queue/call-blast tooling, and SIP/RTP PCAP capture controls are **not part of WaffleHouse-Client**.

GUI access: use **Tools -> Open Softphone**. Incoming calls raise the softphone and offer Answer/Reject/Ignore.

CLI quick start:

```text
/add                         # choose SIP / VoIP and save an account
/connections
/select 2                    # select the SIP account you want to use
/connect                     # register the selected SIP account
/prefix 9                    # runtime prefix for the selected SIP/PBX account
/dialpreview 1001           # show the exact SIP Request-URI
/dial 1001                  # uses the current runtime prefix
/dialraw 1001               # bypass the runtime prefix for this call
/calls
/answer 0
/hangup 0
/siplog 0
/ladder 0
/phoneconfig                 # edits that same selected saved SIP account
/audio-devices
/audio-use 0 0
/audio-auto on
```

## One executable, two frontends

Build output:

```text
./build/wafflehouse-client
```

The executable decides which frontend to start **before creating the Qt application object**, so CLI mode does not need to initialize a graphical Qt session.

### Automatic mode selection

```text
Desktop / application-menu launch
        |
        +-- no interactive TTY attached
        |
        +--> GUI frontend

Interactive terminal launch
        |
        +-- stdin/stdout attached to a TTY
        |
        +--> CLI frontend
```

Examples:

```sh
# From an interactive terminal: automatically starts CLI
./build/wafflehouse-client

# Force the GUI even when launching from a terminal
./build/wafflehouse-client --gui

# Force CLI mode
./build/wafflehouse-client --cli
```

`--gui` and `--cli` are mutually exclusive.

If no graphical session exists, GUI mode exits with a useful error. If CLI mode is requested without an interactive TTY, it also exits cleanly instead of trying to initialize ncurses on a pipe/headless process.

## Shared application state

GUI and CLI mode use the same `WaffleHouseClient` application/settings identity. That means both frontends share:

- saved AIM/OSCAR, IRC, Telnet, and SIP/VoIP connection profiles;
- persistent IRC buddy/watch entries;
- security preferences;
- trusted CPX peer fingerprints;
- the same CPX master identity and therefore the same per-profile secure fingerprints;
- timestamps/member-pane preferences where those concepts apply to both frontends.

Frontend-specific appearance settings remain separate where necessary: the GUI keeps its Qt theme while CLI mode keeps its own ncurses palette.

### 2.4 theme expansion

WaffleHouse-Client 2.4 Alpha includes an expanded theme collection in both front ends. GUI themes receive full Qt widget styling; CLI themes use ncurses colors and automatically take advantage of 256-color terminals when available while retaining 8-color fallbacks. New themes include **Cyberpunk, Synthwave, Dracula, Vaporwave, Blood Moon, C64, DOS, Solarized Dark, Waffle Iron, Ghostline, Hot Dog Stand, and Neon Miami**.

See **THEMES.md** for the complete theme list, descriptions, and switching instructions.

On first use, WaffleHouse-Client can import compatible profiles from the existing `WaffleHouseGUI/WaffleHouseGUI` and `WaffleHouse-CLI/WaffleHouse-CLI` stores. Passwords/secrets are still intentionally not persisted.

## Protocols

- AIM / OSCAR
- IRC, including optional TLS
- Telnet / MUD / BBS
- SIP / VoIP softphone (PJSUA2 / PJSIP 2.17)

Discord is intentionally not included.

## Secure CPX features

- CPX3-compatible encrypted AIM/IRC private messages
- stable per-profile identities and fingerprints
- fingerprint trust/untrust
- changed-key rejection
- replay protection
- capability negotiation
- encrypted file transfer
- explicit accept/decline/cancel
- resumable `.cpxpart` receive files
- SHA-256 final verification

Current advertised optional capabilities include:

```text
secure-dm
file-transfer
file-resume
file-ack
file-direct-v1
```

## IRC features

- private messages and channels
- NAMES/member tracking
- topic setter/time handling
- nickname changes
- IRC formatting sanitization
- persistent local buddy/watch entries using standard `ISON`
- channel nickname Tab completion
- optional TLS
- raw IRC command support

## GUI mode

GUI mode contains the WaffleHouse desktop interface plus the integrated softphone, including:

- Buddy List main window
- explicit **Add Buddy** / **Remove Buddy** controls
- corrected AIM/IRC Join Chat dialog
- connection manager
- independent IM/channel/Telnet windows
- secure-session controls
- secure file-transfer UI
- transparency controls
- integrated Softphone window with Main, Active Call, SIP Log, SIP Ladder, Profile, and Activity tabs
- themes: System, Hacker, Matrix, Phosphor, Midnight, Amber, Ice, Classic Light, Cyberpunk, Synthwave, Dracula, Vaporwave, Blood Moon, C64, DOS, Solarized Dark, Waffle Iron, Ghostline, Hot Dog Stand, Neon Miami

## CLI mode

CLI mode contains the WaffleHouse 1.9.1 ncurses interface, including:

- `/add`, `/edit`, `/delete`, `/connect`, `/disconnect`
- `/query`, `/msg`, `/join`, `/part`, `/say`
- `/buddies`, `/addbuddy`, `/removebuddy`
- `/secure`, `/securestatus`, `/trust`, `/untrust`, `/secureoff`
- `/sendfile`, `/transfers`, `/accept`, `/decline`, `/canceltransfer`
- `/phone`, `/phoneconfig`, `/phonestart`, `/dial`, `/calls`, `/answer`, `/hangup`, `/siplog`, `/ladder`, `/audio-devices`, `/audio-use`
- `/options`, `/help`, `/env`
- reliable Esc cancellation in forms
- themes: System, Phosphor, Amber, Ice, Hacker, Matrix, Midnight, Classic Light, Cyberpunk, Synthwave, Dracula, Vaporwave, Blood Moon, C64, DOS, Solarized Dark, Waffle Iron, Ghostline, Hot Dog Stand, Neon Miami

### IRC `/raw` normalization

WaffleHouse-CLI accepts either wire-style IRC commands or familiar client-style slash commands after `/raw`. For example, `/raw PART #channel` and `/raw /part #channel` are equivalent. A bare `/raw /part` while an IRC channel buffer is active parts that current channel. For normal channel leaving, `/part` is now the preferred command.

## Build

The 2.5.1 builder is **self-preparing by default**. Run:

```sh
chmod +x build.sh
./build.sh
```

Before CMake runs, `build.sh` performs a complete prerequisite audit and ensures a managed PJSIP 2.17 build is available: CMake version, pkg-config/pkgconf, the platform compiler and C++17 link capability, GNU Make, Qt 6 Core/Gui/Widgets/Network development modules, libsodium, wide-character ncurses (`ncursesw`), xkbcommon, Git, the platform SIP audio/crypto prerequisites, and the local PJSIP/PJSUA2 2.17 toolchain. It also performs a small Qt 6 compile/link ABI probe with the selected C++ compiler. If anything is missing, the builder installs the platform package set automatically and then repeats the entire audit before configuring the real project.

Administrator privileges are requested only when package installation or the final system install requires them. The builder can use root directly, `sudo`, `doas`, or `su`. Use `--dry-run` to see the dependency/install commands without changing the system, or `--no-auto-deps` to disable automatic dependency installation.

### Linux

Automatic dependency installation supports these package-manager families when detected:

- `apt-get` (Debian, Ubuntu, Linux Mint)
- `dnf`
- `yum`
- `pacman`
- `zypper`

For Debian/Ubuntu/Linux Mint, the equivalent manual package command is:

```sh
sudo apt install \
  build-essential cmake pkg-config \
  qt6-base-dev qt6-qpa-plugins \
  libsodium-dev libncurses-dev libxkbcommon-dev ca-certificates \
  git curl libasound2-dev libssl-dev uuid-dev pulseaudio-utils
```

### FreeBSD

The builder uses FreeBSD `pkg` automatically. The equivalent manual package command is:

```sh
pkg install cmake pkgconf qt6-base libsodium ncurses libxkbcommon gcc gmake git curl portaudio opus bcg729 libuuid
```

**GCC/G++ and GNU Make (`gmake`) remain explicit FreeBSD prerequisites and the builder installs/verifies them.** However, WaffleHouse itself is compiled with FreeBSD's system Clang/C++ toolchain so it uses the same libc++ ABI as the packaged Qt 6 libraries. Forcing GCC/libstdc++ onto the packaged Qt build can compile successfully and then fail at the final link with unresolved Qt symbols involving `std::string`, `std::future`, or `std::optional`. The builder therefore verifies GCC/G++ as requested, uses `gmake`, installs `devel/ncurses`, and selects FreeBSD Clang/C++ for the Qt application.

The build script also detects a copied/moved source tree and automatically removes a stale generated CMake build directory when its `CMakeCache.txt` points to a different absolute source path. It now also removes the cache when the selected compiler or make program changes, which is especially important when moving an existing FreeBSD build from GCC to the Qt-compatible Clang toolchain.

## Desktop installation

CMake installs one executable:

```text
wafflehouse-client
```

and the desktop file:

```text
share/applications/wafflehouse-client.desktop
```

A desktop/menu launch naturally has no interactive controlling TTY, so the executable selects GUI mode automatically.

## Alpha goals

The 2.5.1 branch extends the **single executable / shared core / dual frontend** architecture so messaging, BBS/Telnet, and multi-account SIP calling all live in the same WaffleHouse connection model.

The intended direction is that future protocol and security work is implemented once in the shared core while GUI and CLI remain presentation layers over the same backend objects and persisted profiles.

## Verify the installed client

The unified 2.5.1 executable is `wafflehouse-client`. To confirm which build is actually being launched:

```sh
command -v wafflehouse-client
wafflehouse-client --version
```

The expected version output is `WaffleHouse-Client 2.5.1`. The old standalone commands `wafflehouse-gui` and `wafflehouse-cli` belong to the 1.x family and should not be used to test 2.5 features. `client-up.sh` removes those legacy launchers from the selected install prefix during an upgrade while preserving user settings.


## Version History

### 2.4 Alpha — 2026-08-15
- Rebranded the current unified C++ GUI/CLI release as **WaffleHouse-Client 2.4 Alpha**.
- Carries forward the validated 2.2 large-file direct-transfer path, transfer Cancel controls, resumable partial transfers, Resume/Clear controls, dedicated File Transfers window, and Linux/FreeBSD dependency/toolchain preflight.
- CLI mode is branded **WaffleHouse-CLI 2.4 Alpha** while remaining part of the same unified `wafflehouse-client` executable.
- Updated build/install/updater/version-reporting metadata and documentation to the 2.4 Alpha release line.


### 2.2 Alpha — 2026-08-15
- Advanced the unified C++ client to **WaffleHouse-Client 2.2 Alpha**.
- Added a dedicated **File Transfers** window in GUI mode with upload/download rows, live progress bars, status, and a timestamped transfer log.
- File-transfer status and progress no longer appear in IM/chat transcript windows.
- The File Transfers window opens automatically when a transfer starts or an incoming offer arrives and can also be reopened from **View → File Transfers**.
- Hardened CPX handshake restarts: capability frames that arrive before the replacement HELLO are deferred silently instead of being reported as secure-channel errors.
- Added CPX `file-direct-v1`: AIM/IRC remains the secure control channel, while accepted file payloads use a dedicated libsodium-encrypted TCP stream when a peer-advertised address is reachable.
- Direct transfers use 64 KiB encrypted stream chunks and do not consume AIM/IRC IM bandwidth; if a direct connection cannot be established or drops, the clients negotiate a resume offset and automatically fall back to the acknowledged secure relay mode.
- Added a **Cancel** action for every active GUI transfer. Cancelling stops only that transfer, not the AIM/IRC connection or conversation window; partial `.cpxpart` data is retained safely for a later resume.
- Retains the acknowledged `file-ack` relay mode, retransmission/resume handling, SHA-256 completion verification, and all Linux/FreeBSD dependency/build fixes from 2.1 Alpha.


### 2.1 Alpha — 2026-08-14
- Renamed the current unified development line from 2.0 Alpha to **WaffleHouse-Client 2.1 Alpha**.
- Added opt-in saved connection passwords shared by GUI and CLI profiles.
- Retains the portable Linux/FreeBSD build and install workflow introduced during the 2.0 Alpha milestone.
- FreeBSD build requirements explicitly include **GCC/G++** and **GNU Make (`gmake`)**; `build.sh` validates GCC/G++ as auxiliary prerequisites, uses `gmake`, and compiles the Qt application with FreeBSD Clang/libc++ for ABI compatibility.
- `build.sh` now checks all build dependencies first and automatically installs any missing packages on supported Linux package managers or FreeBSD `pkg`, then verifies the toolchain before CMake runs.
- Fixed FreeBSD curses detection: the builder now installs/verifies `ncurses`, and CMake links the packaged `ncursesw` through `pkgconf` instead of relying on base-system curses discovery.
- Fixed FreeBSD Qt linker ABI failures: packaged Qt 6 is now built against using FreeBSD Clang/libc++ instead of forcing GCC/libstdc++; GCC/G++ remain installed and verified as explicit prerequisites.
- Added a Qt ABI compile/link preflight and automatic CMake cache invalidation when the compiler or make program changes.
- Expanded dependency preflight to verify every module consumed directly by the build before project configuration.
- Added `--no-auto-deps` for users who intentionally want dependency installation disabled; automatic dependency installation remains the default.
- Keeps 2.0 Alpha in the history as the original unified GUI/CLI architecture milestone.

### 2.0 Alpha — 2026-08-14
- Created the first unified WaffleHouse C++ executable containing both Qt GUI and ncurses CLI frontends.
- Added automatic frontend detection: terminal launches select CLI; desktop launches select GUI.
- Added explicit `--gui` and `--cli` overrides.
- Unified saved connection profiles and CPX security state under one WaffleHouse-Client settings identity.
- Preserved frontend-specific GUI and CLI appearance preferences without forcing the same theme type on both UIs.
- Based the shared protocol core on the 1.9.1 Beta AIM/OSCAR, IRC, Telnet, CPX capability, secure file-transfer, IRC watch-list, and nickname-completion implementations.
- Included portable CMake-cache relocation protection in `build.sh`.

### 1.9.1 Beta lineage
- Source baseline used for the first 2.0 Alpha unified-client prototype.
- Added CPX capability negotiation and encrypted/resumable file transfer.
- Added IRC watch lists and channel nickname completion.
- Corrected Add Connection, Esc handling, Buddy List labels, and Join Chat UI defects.

> Naming note: This software project is not affiliated with or endorsed by Waffle House, Inc.

### 2.0 Alpha build/install update — 2026-08-14

The unified 2.0 Alpha builder now supports the same build/install style used by the 1.9.1 family while still producing only one executable.

```sh
./build.sh                         # build; interactively offers install on a TTY
./build.sh --clean                 # remove generated build tree and rebuild
./build.sh --dry-run               # show dependency/build/install commands without changing files
./build.sh --no-auto-deps           # fail instead of installing missing dependencies
./build.sh --install               # build and install
./build.sh --no-install            # build only
./build.sh --install --prefix /usr/local
./build.sh --install --prefix "$HOME/.local"
./build.sh --jobs 12
./build.sh --build-type Debug
```

On both Linux and FreeBSD the default prefix is `/usr/local`, giving these default installation paths:

```text
/usr/local/bin/wafflehouse-client
/usr/local/share/applications/wafflehouse-client.desktop
```

A custom prefix follows the same standard layout. For example, `--prefix "$HOME/.local"` installs the executable to `$HOME/.local/bin/wafflehouse-client` without requiring root when that tree is writable.

The build remains portable between source locations. If a copied source tree contains a CMake cache pointing at another user's or another directory's absolute path, the builder discards only that generated `build/` tree and reconfigures it.

Until project-specific 2.3 artwork is added, GUI tray setup uses the desktop theme's `internet-chat` icon and falls back to a Qt-provided icon when necessary, preventing the null-icon tray warning.

### Saved connection passwords (2.1 Alpha)

AIM/OSCAR and IRC password prompts now offer an explicit **Save password on this computer** option. The CLI exposes the same choice as a yes/no prompt and in the connection form. Password saving is **off by default**.

When enabled, the password is stored with the shared WaffleHouse-Client profile through Qt `QSettings`, so a credential saved in GUI mode is available in CLI mode and vice versa. This storage is local to the user account but **is not encrypted at rest**. Users who do not want a password written to disk should leave the option unchecked; the credential then remains session-only.



## Large secure file transfers
WaffleHouse-Client 2.4 Alpha advertises both `file-ack` and `file-direct-v1`. When both peers support direct transfer, AIM/IRC is used only to authenticate and negotiate the offer/accept/cancel/fallback control messages. The file payload then moves over a dedicated TCP connection encrypted with a transfer-specific key derived from the existing CPX secure session and libsodium `secretstream` authentication.

The receiving client advertises reachable local interface addresses and a temporary random TCP port. This works directly on the same LAN and on hosts with mutually routable addresses. NAT or host firewalls may prevent the direct socket from connecting; that is expected and does **not** fail the chat session. After the direct connection attempts time out, WaffleHouse exchanges the receiver's current `.cpxpart` byte offset and resumes through the acknowledged secure relay automatically.

Direct transfer uses 64 KiB plaintext chunks before authenticated encryption, which is far more appropriate for tens/hundreds of MiB files than relaying thousands of instant messages. Final SHA-256 verification still occurs before the `.cpxpart` file is renamed to the requested destination. Older `file-transfer` peers remain supported in legacy/reliable relay mode according to their advertised capabilities.

## Upgrading with `client-up.sh`

WaffleHouse-Client 2.5.1 includes a POSIX `client-up.sh` upgrade helper for Linux and FreeBSD.
It is designed to remove the previously installed WaffleHouse-Client executable and desktop entry, preserve all per-user configuration, and then perform a clean build/install of the copy contained in the current source tree.

```sh
chmod +x client-up.sh
./client-up.sh
```

The updater removes only the installed application files under the selected prefix:

```text
PREFIX/bin/wafflehouse-client
PREFIX/share/applications/wafflehouse-client.desktop
```

It **does not remove** saved profiles, saved passwords, IRC buddy/watch data, CPX identity/trust information, or other Qt user settings.

When possible, `client-up.sh` detects the prefix of the currently installed `wafflehouse-client` command automatically. If no installed command can be detected, the default prefix is `/usr/local`. A custom or per-user installation can be selected explicitly:

```sh
./client-up.sh --prefix /usr/local
./client-up.sh --prefix "$HOME/.local"
```

Useful options include:

```sh
./client-up.sh --yes                 # upgrade without the confirmation question
./client-up.sh --dry-run             # show removal/build/install actions only
./client-up.sh --remove-only         # uninstall application files, preserve settings
./client-up.sh --no-auto-deps        # do not let build.sh install missing dependencies
./client-up.sh --jobs 12
./client-up.sh --build-type Debug
```

The normal upgrade path invokes the existing `build.sh --clean --install`, so the full Linux/FreeBSD dependency audit, automatic dependency installation, Qt ABI checks, stale CMake-cache protection, and platform-specific compiler/make selection remain in effect.
Build portability note: the 2.4 Alpha File Transfers progress code uses a C++17 type-safe clamp and builds consistently with GCC/Linux and Clang/FreeBSD Qt 6 toolchains.



### CLI branding and terminal keys

When the unified executable selects terminal mode, the interface is branded **WaffleHouse-CLI**.
All interactive CLI text-entry paths (main input, password/text prompts, and `/add`/edit forms) accept the common Backspace encodings used by Linux and FreeBSD terminals: ncurses `KEY_BACKSPACE`, ASCII BS (`0x08`), DEL (`0x7F`), and the terminal-configured erase character. Interactive popup windows also enable ncurses keypad/meta decoding so keypad Enter/Esc, arrows, Delete, Home/End, and Backspace are handled consistently instead of leaking raw control sequences such as `^?` into fields.


### Resume / Clear behavior

The GUI File Transfers window keeps active transfers separate from chat. Active rows provide **Cancel**. Cancelled or interrupted transfers provide **Resume** and **Clear**. Resume continues from the existing `.cpxpart` byte offset and renegotiates direct encrypted transfer when possible. Clear discards an incomplete `.cpxpart` download and removes the transfer entry; it never deletes a successfully completed download or the sender's original file. Successful transfers automatically leave the active list shortly after verification while their log entry remains. CLI mode provides `/resume ID` and `/cleartransfer ID`.
