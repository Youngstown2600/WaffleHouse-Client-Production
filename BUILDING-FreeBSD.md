# WaffleHouse-Client 2.5 Alpha — FreeBSD build notes

WaffleHouse-Client 2.5 fully integrates a multi-account PJSIP softphone while retaining the existing Qt GUI, ncurses CLI, AIM/OSCAR, IRC, Telnet/BBS, CPX secure messaging, themes, and file-transfer features.

Run:

```sh
chmod +x build.sh
./build.sh
```

The builder audits and, when necessary, installs the complete dependency set. The equivalent manual package command is:

```sh
pkg install cmake pkgconf qt6-base libsodium ncurses libxkbcommon \
  gcc gmake git curl portaudio opus bcg729 libuuid
```

The builder then creates a managed **PJSIP/PJSUA2 2.17** installation under `~/.local/wafflehouse-pjsip`, configured for 32 SIP accounts and 64 call slots (50 active-call runtime ceiling). Use `./build.sh --pjsip` to force that dependency to be rebuilt.

FreeBSD normally supplies Clang/C++ in the base system. The WaffleHouse executable and PJSUA2 are pinned to the FreeBSD base Clang/libc++ ABI because the packaged Qt 6 libraries use that ABI. GCC/G++ remain installed and verified as auxiliary project prerequisites, while GNU Make (`gmake`) is used for generated Makefiles and PJSIP.

For the SIP media stack, the managed FreeBSD PJSIP build uses external PortAudio and explicitly requires Opus, bcg729, and libuuid. The optional legacy WebRTC AEC, UPnP, AMR, SILK, and video helpers are disabled for a deterministic static link. The r12 PJSUA2 call-lifetime guard and live FreeBSD audio-route/headset hot-swap behavior are retained, together with r13 runtime per-account PBX dial prefixes and the r14 guarded `snd_hda` compatibility layer.

The preflight verifies Qt6 Core, Gui, Widgets, and Network; libsodium; ncursesw; xkbcommon; PJSIP 2.17; C++17 compilation; and a real Qt compile/link ABI probe. CMake is not allowed to configure the WaffleHouse project unless the audit passes.

If an older build tree was configured with another compiler, the builder detects the mismatch in `build/CMakeCache.txt`, removes only the generated build tree, and reconfigures using the Qt/PJSIP-compatible compiler automatically.

## r14 FreeBSD Speaker / Headphones compatibility

A normal FreeBSD build now performs the S.I.P.H.E.R. r14-style `snd_hda` topology inspection before compilation. Automatic retasking is considered **only** when the codec exposes the conservative laptop signature of exactly one fixed Speaker and one jack Headphones output with no analog Line-out. For that signature, WaffleHouse can test the firmware Speaker association with the Headphones pin moved to sequence 15.

Safety rules are deliberately strict:

- existing user HDA pin hints cause the automatic repair to be skipped;
- more complex output topologies are reported but never retasked automatically;
- PulseAudio is stopped only when a live HDA rebuild requires it and restarted afterward;
- playback and capture PCM availability are checked after the test;
- any failed validation immediately restores the previous runtime pin configuration;
- persistence occurs only after successful validation; the generated headphone hint is written to `/boot/device.hints` after a timestamped backup;
- a S.I.P.H.E.R. r14-managed hint is recognized as managed state so WaffleHouse does not try to compete with it.

For diagnosis only:

```sh
./build.sh --audio-diagnose
```

That mode is read-only. To run a normal build while explicitly disabling recognized automatic audio repair:

```sh
./build.sh --no-audio-fix
```
