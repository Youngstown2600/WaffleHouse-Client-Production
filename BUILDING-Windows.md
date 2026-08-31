# WaffleHouse-Client 5.1 — Windows

Run `./build-windows.ps1` from PowerShell. The builder uses MSYS2 UCRT64 so the existing Qt, pkg-config, libsodium, ncurses, and managed PJSIP 2.17 code paths stay consistent with Unix builds. The resulting `dist/windows` folder contains `wafflehouse-client.exe` plus GUI/CLI launchers. Both frontends use the same OSCAR backend and therefore the same NINA compatibility path.
