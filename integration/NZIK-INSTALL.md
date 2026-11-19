# ScrewU2Music + N-Zik companion (experimental)

This kit includes two **separate** PS5 ELF payloads:
- `ScrewU2Music-install.elf`: existing ScrewU2Music tile installer (unchanged).
- `N-Zik-companion/n-zik-ps5.elf`: upstream N-Zik PS5 v0.2.0, by DevVeda (GPL-3.0-or-later).

The N-Zik ELF is **not** embedded in ScrewU2Music and does not provide direct YouTube account login in the tile. It is a working upstream architecture for YouTube Music playback via the N-Zik Android phone app. Firmware 13.60 has **not** been tested by this project.

## Set up
1. On Android install the N-Zik app from https://github.com/N-Zik-Group/N-Zik .
2. On PS5 install and start Payload Manager and Kabuki (https://github.com/stupiditytries/Kabuki).
3. Add the upstream Payload Manager source: `https://github.com/DevVeda/n-zik-ps5/releases/latest/download/repo.json` and install N-Zik PS5. Alternatively, copy the bundled `n-zik-ps5.elf` to `/data/pldmgr/payloads/local/n-zik-ps5/n-zik-ps5.elf` over FTP.
4. Start Kabuki, then N-Zik PS5 from Payload Manager.
5. Press L3 + R3 for the QR code. In the Android N-Zik app choose avatar > PC Server > Start server > Pair, then scan QR.
6. Start playback on your phone. Audio should play from PS5 if compatible.

Upstream source: https://github.com/DevVeda/n-zik-ps5 .
This separate distribution preserves upstream copyright and GPL licensing. Review upstream THIRD_PARTY.md for bundled dependencies.
