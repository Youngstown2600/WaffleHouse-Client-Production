# Building WaffleHouse-Client 5.0r7 on macOS

From the source root:

```sh
chmod +x build.sh
./build.sh
```

Choose **3) macOS** when prompted. For scripted builds you can use:

```sh
./build.sh --os macos --clean
```

Homebrew is required. Missing Homebrew formulae are installed unless `--no-auto-deps` is supplied. The builder prepares managed PJSIP 2.17, configures Qt 6/Cocoa, builds the `.app`, copies the built-in notification sounds into the application resources, and runs `macdeployqt`.

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

The 5.0r7 macOS path uses the same application source as Linux/FreeBSD and includes the 5.0r4–5.0r6 OSCAR fixes. It also explicitly keeps the WaffleHouse status/menu-bar icon as a full-color non-template icon and removes the status item synchronously during quit to prevent the historical white-on-white icon state.

Code signing and notarization require the distributor's own Apple Developer identity and are intentionally not fabricated by the builder.
