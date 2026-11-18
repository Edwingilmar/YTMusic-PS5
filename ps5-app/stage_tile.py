#!/usr/bin/env python3
"""ScrewU2Music PS5 tile staging via the PS5 payload SDK FTP server.
Run from the extracted tile-kit directory on a PC on the same LAN.
Requires an already-running FTP server with MTRW support and a payload launcher.
Does not edit PS5 databases. Never runs the installer automatically.
"""
import argparse
from ftplib import FTP, error_perm
from pathlib import Path

TITLE = "SCUM00001"

def main():
    p = argparse.ArgumentParser(description="Stage ScrewU2Music files on a PS5")
    p.add_argument("--host", required=True, help="PS5 local IP")
    p.add_argument("--port", type=int, default=2121)
    p.add_argument("--finalize", action="store_true", help="Upload final system param.json after installer succeeds")
    args = p.parse_args()
    root = Path(__file__).resolve().parent / TITLE
    files = [
        (root / "eboot.bin", f"/system_ex/app/{TITLE}/eboot.bin"),
        (root / "sce_sys/param.json", f"/user/app/{TITLE}/sce_sys/param.json"),
        (root / "icon0.png", f"/user/app/{TITLE}/sce_sys/icon0.png"),
    ]
    if args.finalize:
        files = [(root / "sce_sys/param.json.system", f"/system_ex/app/{TITLE}/sce_sys/param.json")]
    for local, _ in files:
        if not local.is_file():
            raise SystemExit(f"Missing: {local}")
    print("This will stage app files. It will NOT register the app or alter app.db.")
    with FTP() as ftp:
        ftp.connect(args.host, args.port, timeout=15)
        ftp.login()
        # Required for write access in the upstream SDK install_app example.
        print("Requesting MTRW access...")
        ftp.sendcmd("MTRW")
        for directory in [
            f"/system_ex/app/{TITLE}", f"/system_ex/app/{TITLE}/sce_sys",
            f"/user/app/{TITLE}", f"/user/app/{TITLE}/sce_sys",
        ]:
            try:
                ftp.mkd(directory)
            except error_perm as exc:
                if not str(exc).startswith("550"):
                    raise
        for local, remote in files:
            with local.open("rb") as f:
                ftp.storbinary(f"STOR {remote}", f)
            print(f"Uploaded: {remote}")
        # Upstream SDK stages system param.json last, after executing installer.
    if args.finalize:
        print("Finalization upload complete. Check home-screen tile.")
        return
    print("Staging complete. Next, run ScrewU2Music-install.elf on the PS5.")
    print(f"After successful installer run: python3 stage_tile.py --host {args.host} --finalize")
    print("The home-screen tile still needs the UI server payload running.")

if __name__ == "__main__":
    main()
