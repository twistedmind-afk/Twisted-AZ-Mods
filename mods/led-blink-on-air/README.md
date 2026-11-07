# LED Blink On Air

Blinks LOAD and SELECT buttons twice per second while a deck is ON AIR.

The menu has two independent switches, both On by default:

- **Blink LOAD LEDs**
- **Blink SELECT LEDs**

Enable either group or both. Settings take effect immediately, without Reload.

| Deck state | SELECT indication |
|---|---|
| Selected, ON AIR | Bright / dim blinking |
| Selected, not ON AIR | Bright |
| Not selected, ON AIR | Dim / off blinking |
| Not selected, not ON AIR | Dim |

LOAD buttons blink bright/off while ON AIR and otherwise retain their usual indication. Disabled groups retain their usual behavior. The mod follows ON AIR status, not simply whether a track is playing, and does not block loading.

## Requirements

XDJ-AZ with AZ-MODS 1.2.3 or newer.

## Installation

Add `twistedmind-afk/Twisted-AZ-Mods` under **Community Mods > Repositories**, install **LED Blink On Air** from Browse, activate it and Reload when requested. Alternatively, merge its USB ZIP's mod folder onto your USB drive.

Disable older LED mods and the combined Twisted Load Link first. Reload interrupts playback. Works alongside Change Deck On Load and 4 Deck Pro DJ Link.
