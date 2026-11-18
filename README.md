# ScrewU2Music (PS5 homebrew prototype)

![ScrewU2Music brand logo](assets/logo.svg)

## Current status
This repository currently builds **PS5 ELF payload diagnostics**, not an installable PS5 application. A successful ELF launch does not open a music-player interface or install an icon on the home screen.

The current v0.4 ELF is a notification experiment. It is **not** a Spotify/YouTube Music player, and the PS5 firmware 13.60 UI and audio paths have not been implemented or validated.

## Target application
- Standalone dark interface with green accents
- Controller navigation, home, search, library and playback controls
- Local audio playback first; online service integration only after network, authentication, and decoding are validated
- PS5 application packaging and installable home-screen icon, subject to a verified PS5 homebrew app toolchain

## Branding
The editable vector logo is at [assets/logo.svg](assets/logo.svg). This is a newly drawn SVG approximation of the approved visual concept, **not** the original rendered PNG. A native PS5 package would require platform-specific icon/splash assets and integration during packaging.

## Builds
The GitHub Actions workflow currently publishes a diagnostic `ScrewU2Music.elf`. Do not mistake it for an installable `.pkg` or a completed player.
