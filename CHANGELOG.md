# WaffleHouse-Client 5.x Changelog

## 5.0r7
- Combined the Linux/FreeBSD and macOS desktop releases into one shared source bundle. The top-level builder now asks the user to select Linux, FreeBSD/Unix, or macOS before any platform build/dependency work begins.
- Normal builds on every desktop platform ask before installing WaffleHouse-Client into a system bin directory; Enter/default remains No.
- Synchronized macOS from the 5.0r3 platform package to the complete 5.0r6 application source, bringing over the 5.0r4 typing-window fix, 5.0r5 OSCAR transfer/window/rate fixes, and 5.0r6 secure-direct finalization/profile-ID fixes.
- Fixed the macOS status/menu-bar icon white-on-white regression by explicitly keeping the WaffleHouse icon non-template/full-color and removing the QSystemTrayIcon synchronously during application quit.
- Bundled built-in notification sounds inside the macOS `.app` so an installed/moved app does not depend on the extracted source directory.

## 5.0r6
- Fixed secure CPX direct downloads that reached 100% but remained as `*.cpxpart` while the receiver stayed on “Receiving direct” and the sender stayed on “Verifying”.
- Corrected GUI file-transfer ownership lookup: transfer records store the stable connection profile ID, but several completion/cancel/resume/direct callbacks incorrectly looked it up as a transient backend ID.
- Direct receive completion now always performs local SHA-256 verification and final rename even if the account/control-channel object becomes temporarily unavailable; peer completion confirmation is sent when the owning account is available.
- Hardened the encrypted direct socket sender so a queued final frame can drain and close proactively instead of depending on the receiver to close first.
- The same profile-ID lookup correction also restores GUI cancel, resume, decline, direct-progress peer naming, and direct-fallback handling for file transfers.

## 5.0r5
- Fixed AIM/OSCAR Send File from the buddy list so launching a transfer does not create an IM window.
- File-transfer OFFER/ACCEPT/DATA/ACK/DONE/COMPLETE control traffic is now consumed as transport traffic and does not create GUI conversations.
- Secure file-transfer payloads are recognized before an IM window is created, preventing the same pop-up behavior on encrypted transfers.
- Corrected OSCAR rate-class reply parsing (supports both 30-byte and 35-byte layouts) and tuned relay transfers to use larger payloads at a roughly two-second cadence, reducing ICBM rate-limit pressure that could stall downloads after they started.
- Extended reliable-transfer ACK retry tolerance and kept Send File available from an online IM even without an active secure session.

## 5.0r4
- Fixed OSCAR typing notifications so they stay in the normal AIM IM window instead of opening a separate transient "typing" conversation window.
- The existing IM header now remains the single typing-state indicator (typing, paused, or cleared).

## 5.0r3
- Pruned the release archive to platform-required source, assets, builders, and current documentation only.
- Removed other-platform adapters/builders, historical 3.x release artifacts, examples, companion payloads, and internal regression-test trees from the customer-facing archive.
- Tailored CMake configuration specifically to the Unix/Linux bundle so it no longer exposes dead build branches for other platforms.

## 5.0r2
- Added conventional GUI application exit: File → Exit on Linux/FreeBSD with Ctrl+Q.
- Uses the existing clean shutdown path; tray-close behavior remains unchanged.

## 5.0r1
- Redesigned the Softphone around a combined Phone workspace.
- Added a dedicated SIP Accounts left-navigation tab.
- Exposed Answer, Reject, Hang Up, Hold, Resume, Mute, Blind Transfer, Attended Transfer, DTMF, and Diagnostics in the GUI.
- Widened the Softphone navigation rail to prevent clipped labels.
- Cleaned GCC OSCAR initializer and misleading-indentation warnings.

## 5.0
- Introduced the 5.x unified desktop application/core design, unified contacts/history/capabilities, expanded SIP diagnostics and transfer controls, and command-palette support while preserving AIM/OSCAR, IRC, Telnet/BBS, media, secure rooms, encrypted transfers, themes, and CLI/GUI operation.
