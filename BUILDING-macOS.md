# Building WaffleHouse-Client 5.1r3 on macOS

From the source root:

```sh
chmod +x build.sh
./build.sh
```

Choose **3) macOS** when prompted. For scripted builds you can use:

```sh
./build.sh --os macos --clean
```

Homebrew is required for the build toolchain. Only compile/link dependencies are mandatory and auto-installed unless `--no-auto-deps` is supplied. `mpv` and `ffmpeg` are optional runtime helpers and **never block the client build**; `yt-dlp` is not required. The builder prepares managed PJSIP 2.17, configures Qt 6/Cocoa, builds the `.app`, copies the built-in notification sounds into the application resources, and runs `macdeployqt`.

If you want the builder to *try* installing the optional media helpers, use:

```sh
./build.sh --os macos --with-media-deps
```

If Homebrew reports that `mpv` has no bottle for that Mac/macOS release, the builder prints a warning and continues. The rest of WaffleHouse-Client remains fully buildable; only local/radio media playback is unavailable until an `mpv` runtime is installed.

Create a DMG as well:

```sh
./build.sh --os macos --clean --dmg
```

The resulting app is normally `build-macos/wafflehouse-client.app`. Its executable supports both modes:

```sh
open build-macos/wafflehouse-client.app
build-macos/wafflehouse-client.app/Contents/MacOS/wafflehouse-client --cli
```

After a successful normal build, the builder asks before installing. Pressing Enter means **No**. If accepted, the defaults are:

- `/Applications/WaffleHouse-Client.app`
- `/usr/local/bin/wafflehouse-client` → the executable inside the installed `.app`

The 5.0r13 macOS path uses the same application source as Linux/FreeBSD and includes the 5.0r4–5.0r6 OSCAR fixes. It also explicitly keeps the WaffleHouse status/menu-bar icon as a full-color non-template icon and removes the status item synchronously during quit to prevent the historical white-on-white icon state.

Code signing and notarization require the distributor's own Apple Developer identity and are intentionally not fabricated by the builder.


## Uninstall / remove

The macOS builder now supports the same lifecycle choice. Interactive `./build.sh` offers **Uninstall / Remove**, or run:

```sh
./build.sh --os macos --uninstall
```

It removes `/Applications/WaffleHouse-Client.app` and the `/usr/local/bin/wafflehouse-client` launcher (or custom app/prefix targets) without running Homebrew dependency checks. Per-user WaffleHouse configuration, history, accounts, and logs are preserved. `--remove-only` is an alias.
