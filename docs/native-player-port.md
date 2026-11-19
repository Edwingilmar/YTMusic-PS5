# ScrewU2Music: native playback port (work in progress)

This branch deliberately does **not** change the working installer, PS5 tile, browser launcher, or release catalog. The current web UI and iframe are not a native music player.

## Verified upstreams

- [ProsperoPlayer](https://github.com/KINGDKAK/ProsperoPlayer), GPL-3.0: native PS5 player, SDL2 + FFmpeg (libavformat, libavcodec, libswresample, libavutil), SceAudioOut, ScePad. Its Makefile targets **PS5_PAYLOAD_SDK**, unlike ScrewU2Music's current **ps5-payload-dev/sdk** sample installer build. Porting requires a compatible toolchain and a separately compiled ELF. The upstream `playback/` directory currently contains empty placeholders; actual player sources are under `pp/src/` and `main.c`.
- [Limusic](https://github.com/SimoHypers/limusic), GPL-3.0: Rust/Tauri/InnerTube and libmpv. Not a PS5-native drop-in. Its documented stream retrieval relies on a real webview for JS cipher and BotGuard, so copying it does not remove every browser-related dependency.

## Integration gates (no fake green checks)

1. Build a **separate** native audio test ELF using the same PS5 SDK and FFmpeg/SDL2 libs that ProsperoPlayer requires. Keep the stable ScrewU2Music installer and media tile untouched.
2. Test local, authorized AAC/MP3 audio through SceAudioOut with DualSense play/pause. Confirm on PS5 firmware 13.60.
3. Test HTTPS audio streaming from an authorized endpoint with certificate validation, buffering, stop/pause and error reporting. No PSN sign-in or Sony browser.
4. Evaluate account login and catalog APIs independently. YouTube Music does not offer a general public third-party music streaming API. Do not solicit Google passwords, hardcode cookies, or promise Premium playback.
5. Only after the native test works, wire it into ScrewU2Music and update Payload Manager. Preserve previous installer, logo, tabs and rollback artifact.

## License

Both upstream repositories advertise GPL-3.0. Any code copied or adapted from them requires preserving copyright notices, source availability and GPL-3.0 compliance. No upstream source has been copied into this branch yet.

## Current status

Research and port plan completed. **No native playback implementation or PS5 audio test has been compiled.** The existing YouTube iframe still fails with “This video is unavailable” on the user's PS5. Do not publish this branch as a functional player.
