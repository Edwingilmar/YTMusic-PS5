# YouTube Music PS5 launcher v0.1 (experimental)

Source code only until GitHub Actions successfully compiles an ELF.

This experimental PS5 payload attempts to launch https://music.youtube.com/ via sceSystemServiceLaunchWebBrowser. It does not provide a native player. Audio, navigation, browser support, and firmware 13.60 compatibility are untested.

## Build
Open Actions > Build PS5 YouTube Music launcher > Run workflow. If successful, download artifact YTMusic_PS5_elf, extract the ELF and test using your payload loader.

Based on ps5-payload-dev/sdk browser sample. Not affiliated with Google, YouTube, or Sony.
