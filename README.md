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


## PS5 launcher architecture (research validated)
The requested one-ELF home-screen launcher is technically plausible. The open-source [deox1111/ps5-library](https://github.com/deox1111/ps5-library) demonstrates a payload that adds a home-screen tile and keeps its server running. The [ps5-payload-dev/websrv](https://github.com/ps5-payload-dev/websrv) project demonstrates HTTP serving on jailbroken PS5s.

**Do not confuse the existing v0.4 diagnostic release with a launcher.** It cannot install ScrewU2Music or play YouTube Music. The web UI prototype is at [prototype/index.html](prototype/index.html) and the brand SVG at [assets/logo.svg](assets/logo.svg).

### Remaining engineering and acceptance tests
- Integrate an embedded HTTP server with the HTML/CSS/JS and logo bundled in the ELF.
- Implement home-screen tile registration with an app-specific unique identifier and safe cleanup, based on source review and license compliance.
- Ensure persistent server lifetime, boot autoload instructions, and firmware 13.60 compatibility.
- Validate the launcher on a real PS5 before publishing a release as installable.
- Determine a supported method for YouTube Music authentication and playback. No complete public official playback API is provided for arbitrary PS5 clients; do not embed account credentials or claim audio support without tests.
