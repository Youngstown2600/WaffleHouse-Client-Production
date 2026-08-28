# WaffleHouse-Client 5.x Changelog

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
