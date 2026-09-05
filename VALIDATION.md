# WaffleHouse-Client 5.1r4 — Validation

Date: 2026-09-02

## Scope

5.1r4 adds a persistent Media Library and cleans the all-platform release bundle while retaining the complete inherited 5.1r3 + 5.0r20 regression surface.

The Media Center now:

- restores its queue from per-user application data without autoplay;
- persists queue entries/titles/order/current index plus volume, mute, shuffle, and repeat state in `media/library.json`;
- mirrors the saved queue to `media/queue.m3u8` for lazy mpv rehydration;
- caches local PLS/M3U/M3U8/XSPF playlist definitions under `media/playlists/`;
- preserves expanded stream entries independently of the original imported playlist file;
- exposes a **Library Folder** shortcut in the GUI;
- shares the same persistence implementation between GUI and CLI `MediaController` usage.

The shipping bundle cleanup removes old `VALIDATION-*.txt` receipts, the one-off r20 merge audit, and the obsolete historical platform-comparison artifact. `VALIDATION.md` is now the single current validation report; regression scripts remain under `tests/`.

## Regression results

19/19 regression files PASS:

- BuildFeatures GUI declaration scope
- IM composer initial focus
- macOS framework linkage
- macOS Qt deployment
- OSCAR login/full debug + native idle
- OSCAR native feature audit
- selectable protocols + multi-SIP + BBS geometry + OSCAR
- persistent OSCAR log + `/bin` install prompt
- OSCAR ChatNav/Chat redirect/menu gating
- r20 OSCAR LOCATE Away/Idle hydration
- r20 printable unsecured-transfer envelope + legacy receive compatibility
- 5.1 NINA compatibility
- 5.1r4 platform/version propagation
- 5.1r1 responsive softphone layout
- 5.1r2 uninstall/remove lifecycle
- 5.1r3 AIM/IRC labels + compact phone controls
- 5.1r4 persistent Media Library + bundle cleanup
- chan_sip compatibility
- SIP server-type UI

Additional validation:

- 25 shell files PASS `sh -n`.
- Python regression files PASS `py_compile`.
- No `build/`, `CMakeFiles/`, `CMakeCache.txt`, `.git`, `__pycache__`, `.pyc`, object files, or editor backup artifacts are present in the release source tree.

## Native-build limitation in this execution environment

A fresh CMake configure was attempted. Compiler detection and pthread checks succeed, then configuration stops at `find_package(Qt6)` because this execution environment does not contain `Qt6Config.cmake` / `qt6-config.cmake`. Therefore this validation does not claim a fresh native Qt compile/link inside the packaging environment. The source-level and packaging regressions above all pass.

## Packaged-release verification

The final ZIP was unpacked into a clean directory. All 19 regression files passed again from the packaged tree, the r4 media-persistence/bundle-cleanup gate passed, and the archive contains no historical `VALIDATION-*.txt`, r20 merge receipt, obsolete historical platform comparison, build/cache/object artifacts, or generated Python bytecode.
