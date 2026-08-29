# WaffleHouse-Client 5.0r10 — Corrected All-Platforms Bundle

This packaging correction contains four explicit platform trees:

- `Linux/` — Linux desktop source and builder
- `Unix-FreeBSD/` — FreeBSD/Unix desktop source and builder
- `macOS/` — macOS desktop source and builder, including the corrected framework-link handling
- `Termux/` — Android/Termux CLI source and builder (Build 1.2)

Run `./build.sh` at the bundle root and choose a platform, or use `--platform linux`, `--platform unix`, `--platform macos`, or `--platform termux`.

The desktop source remains WaffleHouse-Client 5.0r10. This is a packaging correction, not a source-code revision.
