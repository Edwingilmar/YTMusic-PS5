# PS5 home-screen installation plan

## Goal
Run `ScrewU2Music.elf` once on an already-jailbroken PS5 13.60, create a dedicated **Media** home-screen tile (without replacing PlayStation Store), and open the ScrewU2Music interface from that tile.

## Verified reference implementations
- [deox1111/ps5-library](https://github.com/deox1111/ps5-library): ELF registers a home-screen tile; the payload must remain running for the tile to work.
- [XavyProd-Git/pkg-mutant-shop](https://github.com/XavyProd-Git/pkg-mutant-shop): ELF writes its UI and adds a Media tile.
- [RemiPelloux/piou-autoloader](https://github.com/RemiPelloux/piou-autoloader): installer ELF creates a home-screen shortcut; Relapse is used for firmware 13.60.

## Implementation stages
1. Extract and verify the *actual* home-screen tile registration and embedded web UI mechanisms in reference source code. Check licenses before reusing code.
2. Implement an ELF that serves bundled `prototype/index.html` and `assets/logo.svg` over local HTTP while it remains running.
3. Register a unique, non-conflicting Media tile that opens the local interface. Add an uninstall path and avoid editing unrelated tiles.
4. Add persistent payload autoload support only after a successful on-console smoke test.
5. Implement native input and YouTube Music-compatible playback only after verifying authentication, service permissions, network and codec feasibility.

## Safety and status
**NOT IMPLEMENTED:** home-screen registration, HTTP serving in ELF, autoload, YouTube Music account integration, audio playback, PKG installation. Current GitHub Actions `ScrewU2Music.elf` is a notification-only diagnostic and **must not** be described as an installer.

Firmware target: PS5 13.60 with existing jailbreak. Avoid changes to DNS or replacing system apps. Preserve user-installed homebrew and existing title IDs. No claims of successful install without a PS5 test.
