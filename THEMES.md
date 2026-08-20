# WaffleHouse-Client 2.5 Alpha — Theme Guide

WaffleHouse-Client keeps GUI and CLI appearance preferences separately. GUI mode uses full Qt widget styling. CLI mode uses ncurses palettes, taking advantage of 256-color terminals when available and falling back to standard terminal colors when necessary.

| Theme | Style |
|---|---|
| System | Native desktop / terminal defaults |
| Hacker | Bright terminal green on near-black |
| Matrix | Pure black and Matrix green |
| Phosphor | Softer old-school green CRT |
| Midnight | Cool blue-gray late-night dark theme |
| Amber | Vintage amber monochrome terminal |
| Ice | Cold blue/cyan terminal palette |
| Classic Light | Traditional light desktop palette |
| Cyberpunk | Black, electric cyan, and hot magenta |
| Synthwave | Deep purple, neon pink, and cyan |
| Dracula | Charcoal with purple, pink, cyan, and green accents |
| Vaporwave | Pastel pink, violet, aqua, and dark purple |
| Blood Moon | Black and deep crimson with bright red accents |
| C64 | Commodore-inspired blue and light-blue retro palette |
| DOS | Blue-screen DOS-inspired white/cyan/yellow palette |
| Solarized Dark | Solarized-inspired blue-green dark palette |
| Waffle Iron | Charcoal-brown with toasted gold and syrup-like accents |
| Ghostline | Dark steel blue, cyan, and violet tech palette |
| Hot Dog Stand | Deliberately ridiculous red/yellow/black high-contrast palette |
| Neon Miami | Dark teal with neon aqua and pink |

## Switching themes

### GUI

Use **View → Theme**, or open **Tools → Options** and choose a theme from the Theme list.

### CLI

Run `/options`, move to **Theme**, then use **Left/Right** to cycle through the palettes. Save with **F10** or **Ctrl-S**.

Theme selections persist independently for GUI and CLI mode.
