# Known issues

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
