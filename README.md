# WaffleHouse-Client 3.3r1

WaffleHouse-Client is the unified C++ GUI/CLI communications client with AIM/OSCAR, IRC, Telnet/BBS, SIP/VoIP, CPX encrypted DMs, Secure Rooms, secure/unsecured file transfer, notifications, themes, and integrated media playback.

The built-in Media Center supports local audio/video, direct HTTP/HTTPS media, SHOUTcast/Icecast radio, HLS streams, and M3U/M3U8/PLS playlists through an external `mpv` backend. Supported targets remain Debian-family Linux, Fedora, Slackware, and FreeBSD.

**Version 3.3r1 does not include YouTube streaming support.** There is no YouTube button or menu action in the GUI, no `/myoutube` CLI command, and no yt-dlp/Deno dependency or resolver code. mpv is launched with `--ytdl=no` so the removed feature is not reintroduced implicitly.

Stop preserves the queue. Pressing Play/Resume while stopped automatically begins with the first queued item.

The 3.3r1 GUI uses a compact main Accounts window without the old left navigation rail. Account lifecycle actions are under **Accounts → Account Management**; Tools contains BBS import, the file-transfer activity log, and Softphone access. AIM/IRC accounts and buddies expose protocol-aware right-click actions. The main window also includes a `/command` box that maps CLI-style commands to GUI/backend actions.

### 3.3r1 account/media additions

- Hardware multimedia keys control the integrated Media Center across the GUI: Play, Pause, Play/Pause, Stop, Previous, Next, Volume Up, Volume Down, and Mute.
- Right-click a connected AIM/OSCAR account for **Edit AIM Profile…** or **Server Capabilities…**. Profile availability is based on the server-advertised OSCAR Locate family, with Locate rights used to discover the maximum profile size when the server reports it.
- Right-click a connected IRC account for **Server Capabilities…**. WaffleHouse-Client records IRCv3 `CAP LS 302` results and the server's numeric 005/ISUPPORT feature tokens.

See `MEDIA.md`, `BUILDING-Linux.md`, `BUILDING-FreeBSD.md`, and `CHANGELOG.md` for details.
