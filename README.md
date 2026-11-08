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

### [4 Deck Pro DJ Link Plus](mods/4-deck-pro-dj-link-plus)

| Normal PRO DJ LINK | Mod settings | Behavior |
|---|---|---|
| Off | Any | All four decks remain internal and send PRO DJ LINK data. |
| On | 3+1 Off | Original two internal decks plus external players on decks 3/4. |
| On | 3+1 On, External Deck 4 Off | Internal decks 1/2/4 and external Deck 3. |
| On | 3+1 On, External Deck 4 On | Internal decks 1/2/3 and external Deck 4. |

Select the 3+1 layout under **MOD MENU > 4 Deck Pro DJ Link Plus** and apply it with **Reload**. Set the external CDJ's Player No. to match the chosen slot. The saved layout takes effect after Reload.

## Known Core interaction: Compact Track Info

Some AZ-MODS 1.2.3 Core builds can show a thick, centered waveform playhead even though
**Waveform Current Position** is set to **Left**, particularly after startup or loading tracks.
Empty decks can be affected too, and the marker may no longer line up with the waveform.

The reproduced issue is in the shared Core layout for **Compact Track Info**, which can
restore a cached marker position and width while resizing the waveform lane. It can affect
both **4 Deck Pro DJ Link** and **4 Deck Pro DJ Link Plus**. Changing LINK variants alone
is not a reliable workaround.

Temporary workaround, in this order:

1. Open the AZ's native **SHORTCUT** settings and set **Waveform Current Position** to **Center**.
2. Leave SHORTCUT and open **BROWSE** once.
3. Return to SHORTCUT and set **Waveform Current Position** back to **Left**.
4. Return to the waveform view and check the marker alignment.

**Do not skip the BROWSE step between Center and Left.** The workaround may need repeating
after a restart or layout change. Alternatively, turn **Compact Track Info** off in MOD MENU.

Plus 1.0.1 includes a build-specific compatibility hook for the identified affected Core.
It does not replace the Core file. Unknown Core builds or hook conflicts require the workaround above.
The integrated helper is awaiting hardware validation; the underlying layout correction was checked separately.

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

Use only one LINK role mod at a time. If the original **4 Deck Pro DJ Link** is installed, disable it before activating **Plus**.
