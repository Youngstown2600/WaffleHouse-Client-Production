# WaffleHouse-Client 5.1 — Termux / Android

Run `./scripts/build-termux.sh` inside Termux. It enables the Termux X11 repository, installs the native Termux toolchain/Qt packages, builds the shared WaffleHouse source, and installs `wafflehouse-client`, `wafflehouse-client-cli`, and `wafflehouse-client-gui` under `$PREFIX/bin`. CLI runs in the normal terminal. GUI requires a Termux:X11 session. Both frontends share the exact same AIM/OSCAR backend; NINA is auto-detected from `*.nina.chat` or can be selected explicitly as the `nina` network profile.
