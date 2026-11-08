# 4 Deck Pro DJ Link Plus

Version **1.0.1** for XDJ-AZ firmware `ep147-9e9454e357389ece` and AZ-MODS 1.2.3 or newer.

| Normal PRO DJ LINK | 3 Internal + 1 External | External Deck 4 | Internal decks | External players |
|---|---|---|---|---|
| Off | Either | Either | 1, 2, 3, 4 | None |
| On | Off | Either | 1, 2 | 3 and 4 |
| On | On | Off | 1, 2, 4 | 3 |
| On | On | On | 1, 2, 3 | 4 |

With normal LINK Off, all four internal decks broadcast their native PRO DJ LINK data. With LINK On, use the original 2+2 layout or choose 3+1. The additional internal deck remains available for loading and playback, while the selected external slot shows LINK PLAYER.

## Settings

Open **MOD MENU > 4 Deck Pro DJ Link Plus**:

- **3 Internal + 1 External:** Off keeps the original 2+2 layout. On enables 3+1.
- **External Deck 4 (off: Deck 3):** Off reserves Deck 3 for an external player; On reserves Deck 4. Available when 3+1 is enabled.

Choose the matching **Player No.** on the external CDJ. Use **Reload** after changing these mod settings. They are saved immediately, but the active layout changes only after Reload; there is no live reassignment. Reload interrupts playback.

The normal PRO DJ LINK setting still chooses between four internal decks (Off) and the selected external-player layout (On).

## Installation

Add `twistedmind-afk/Twisted-AZ-Mods` under **Community Mods > Repositories**, then install and activate **4 Deck Pro DJ Link Plus**. Alternatively, merge the USB ZIP's `MODS/community-mods/` folder into the same folder on your USB drive. Approve the mod and Reload when requested. Wait for the USB library to become ready before browsing tracks.

Compatible with **Change Deck On Load** and **LED Blink On Air**. Enable only one mod that changes native LINK deck roles at a time.

## Source and compatibility

Complete GPL-3.0-only source, icon, license and Makefile are included here. See the repository [build instructions](../../BUILD.md). The mod uses normal SDK hooks, firmware fingerprint checks, Community Mods approval and crash recovery.

Both 3+1 layouts were checked with stock CDJ-2000NXS2 firmware 1.87 in an isolated emulator. Loading a track from the AZ's USB library over LINK displayed its title, BPM, key, waveforms and cues on the CDJ. This does not establish real-hardware audio, SYNC or long-session performance.

## Integrated playhead compatibility fix

Plus **1.0.1** includes a compatibility hook for the identified affected Core build
(GNU build ID `0a4e88389d15266420766f0cf1064b27ef45659e`, SHA-256
`0e2b9c2f384a80cf645d6fe7769fbee2b00a0e18422f296e67f980f18c10b92b`).
It preserves the live native playhead position and width when that Core's compact layout
changes marker height. Native Left/Center choices, cues, grid and the LINK role logic stay native.
The mod does not replace or patch the Core file and does not change saved waveform preferences.

The compatibility hook is installed only through the SDK on the exact tested firmware and
identified Core build. A different/already corrected Core skips this helper. A claimed hook
conflict is logged and leaves LINK available with the manual workaround below. The log reports
whether compatibility is active; future unrecognized Core builds are not modified speculatively.

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

Plus 1.0.1 includes the build-specific compatibility fix above. It does not include or install
a modified Core. Use the workaround if your Core is not covered or the compatibility hook is unavailable.

Validation: the integrated 1.0.1 helper was built and package-validated; no emulator playback test was performed for this release. Hardware feedback is pending. The earlier 3+1 checks above apply to the unchanged role implementation.
