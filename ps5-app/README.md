# ScrewU2Music PS5 tile package scaffold

Target title ID: `SCUM00001` (unverified availability; check for collisions on the console).

The `sce_sys/param.json` describes a **deeplink tile**, not a standalone native music app. It points to `http://127.0.0.1:8787/`; the launcher must run a local web server on the console before the tile can open.

**Not yet installable**: this directory is a packaging scaffold only. A real tile package requires an icon0.png, a PS5-compatible PKG packaging toolchain, and a tested ELF installer. No installation script should modify app.db/appinfo.db directly; reference implementations warn this can lock games and trigger database recovery.

Reference: https://github.com/XavyProd-Git/pkg-mutant-shop/blob/master/ps5-app/tile/register_tile.py

YouTube Music playback and authentication remain unimplemented.
