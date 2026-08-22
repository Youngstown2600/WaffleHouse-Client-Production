# WaffleHouse-Client 3.1 branding

This 3.1 release integrates the WaffleHouse-Client badge artwork while preserving the established protocol/core feature set.

## GUI / desktop integration

- The generated WaffleHouse-Client badge is embedded in the Qt executable.
- GUI mode sets it as the application/window icon.
- The GUI startup splash displays the badge instead of the terminal ASCII logo.
- The system tray uses the bundled multi-resolution WaffleHouse-Client icon first, with the existing window/theme/Qt fallbacks retained only for damaged or unusual runtime environments.
- The desktop entry now uses `Icon=wafflehouse-client`.
- Linux/FreeBSD installs place 16, 22, 24, 32, 48, 64, 128, 256, and 512 pixel icons into the hicolor icon theme.
- The installer refreshes the desktop and icon caches when the corresponding host tools are available.
- Uninstall removes the installed WaffleHouse-Client icon files while preserving user configuration.

## CLI behavior

The unified CLI build is unchanged functionally. Its existing terminal-native ASCII WaffleHouse branding remains in place because a raster image is not portable across normal ncurses terminals.

## Verification

`tests/run_all.sh` passes 25/25 source/regression tests, including `logo_tray_300r2_test.sh`.
