# Twisted AZ Mods

Three independent mods for the XDJ-AZ. Install them separately or use them together.

## Mods

### Change Deck On Load

Two independently selectable shortcuts:

- **Select Deck on Load:** selects the corresponding deck after loading a track. On by default.
- **Load Track on Deck Select:** loads the highlighted track when you press a deck's SELECT button in the track list. Off by default. Outside the track list, SELECT keeps its usual function.

### LED Blink On Air

Blinks the LOAD and SELECT buttons twice per second while their deck is ON AIR.

- **Blink LOAD LEDs:** independently enables LOAD blinking. On by default.
- **Blink SELECT LEDs:** independently enables SELECT blinking. On by default.

### 4 Deck Pro DJ Link

| Normal PRO DJ LINK setting | Behavior |
|---|---|
| Off | All four decks remain internal and send PRO DJ LINK data. |
| On | Original two internal decks plus two external players on decks 3/4. |

## Requirements

- XDJ-AZ with AZ-MODS installed.
- AZ-MODS 1.2.3 or newer.
- A USB drive for installation.

## Installation

### From the repository

1. Open **Community Mods > Repositories** and add `twistedmind-afk/Twisted-AZ-Mods`.
2. Install the desired mods from **Browse**.
3. Activate them and use **Reload** when requested.

### From a USB ZIP

1. Download the desired mod's `-usb.zip`, or the combined USB ZIP, from [Releases](https://github.com/twistedmind-afk/Twisted-AZ-Mods/releases).
2. Back up your existing `MODS` folder.
3. Merge the ZIP's `MODS/community-mods/` folders into the USB drive's `MODS/community-mods/` folder.
4. Approve and activate the mods in Community Mods, then Reload when requested.

## Repository and release files

- **This repository:** mod manifests, icons, complete source code, licenses and [build instructions](BUILD.md).
- **GitHub Releases:** ready-to-install `.azmod` packages, icons and USB ZIPs.
- **Individual USB ZIPs:** install one mod.
- **Combined USB ZIP:** contains all three mods.

The USB ZIPs contain the mod folders; they do not replace your global mod configuration or music library.
