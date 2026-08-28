# WaffleHouse-Client 5.0r4 — Unix/Linux

Dedicated source release for **Linux and FreeBSD**. This package contains both the Qt GUI and ncurses CLI frontends and the shared AIM/OSCAR, IRC, Telnet/BBS, SIP/PJSIP, media, secure-room, transfer, contacts, and history implementation.

## Build

```sh
./build.sh
```

The builder detects Linux vs. FreeBSD, checks/install dependencies where supported, prepares the managed PJSIP 2.17 dependency, and builds WaffleHouse-Client. See `BUILDING-Linux.md` or `BUILDING-FreeBSD.md` for platform notes.

## 5.0r4 highlights

- OSCAR typing status now stays in the normal AIM IM window; no extra typing-only window is opened.
- Redesigned Softphone **Phone** workspace.
- Dedicated **SIP Accounts** navigation tab.
- GUI Answer, Reject, Hang Up, Hold, Resume, Mute, Blind Transfer, Attended Transfer, DTMF, and Diagnostics controls.
- Conventional **File → Exit** with Ctrl+Q and the existing clean-shutdown path.
- GUI and CLI remain in the same desktop executable.

This archive intentionally contains no macOS or Termux builder/source bundle, no historical 3.x release audits, and no internal regression-test tree.
