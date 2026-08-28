WaffleHouse-Client 5.0r7 — Combined Desktop Bundle
Platforms: Linux, FreeBSD/Unix, macOS
Build: ./build.sh

The builder asks which OS is being installed on, verifies that selection against
uname, and dispatches to the matching platform build path.

A successful normal build asks before installing WaffleHouse-Client into the
platform's system bin location. The default answer is No.

macOS source is synchronized to the 5.0r6 Unix/Linux fixes and includes the
5.0r7 macOS status/menu-bar icon teardown/non-template fix.
