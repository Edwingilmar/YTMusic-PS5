#!/usr/bin/env python3
"""Generate one PS5 installer ELF source embedding the complete Max'd native app.
GPL-3.0-or-later: upstream HandshakeHackin/youtube-maxd and ps5-payload-dev.
"""
import json, pathlib, sys, zipfile
archive=pathlib.Path(sys.argv[1])
output=pathlib.Path(sys.argv[2])
stage=pathlib.Path(sys.argv[3])
title="PPSA99991"
stage.mkdir(parents=True,exist_ok=True)
with zipfile.ZipFile(archive) as z:
    entries=[i for i in z.infolist() if not i.is_dir()]
    if not entries or not any(i.filename==title+"/eboot.bin" for i in entries):
        raise SystemExit("Native app eboot missing")
    for i in entries:
        p=pathlib.PurePosixPath(i.filename)
        if p.is_absolute() or p.parts[0]!=title or any(part in ("", ".", "..") or any(c in part for c in ('"', "\\", "\\n", "\\r", "\\0")) for part in p.parts):
            raise SystemExit("Unsafe asset path: "+i.filename)
        dest=stage.joinpath(*p.parts)
        dest.parent.mkdir(parents=True,exist_ok=True)
        data=z.read(i)
        if i.filename==title+"/sce_sys/param.json":
            meta=json.loads(data)
            meta["localizedParameters"]={k:{**v,"titleName":"ScrewU2Music"} for k,v in meta.get("localizedParameters",{}).items()}
            if not meta.get("localizedParameters"): meta["titleName"]="ScrewU2Music"
            data=(json.dumps(meta,ensure_ascii=False,indent=2)+"\n").encode()
        dest.write_bytes(data)
lines=['/* Generated GPL-3.0-or-later native installer; all assets in one ELF. */',
'#include <stdio.h>','#include <stdint.h>','#include <stddef.h>','#include <errno.h>','#include <sys/stat.h>','#include <ps5/kernel.h>',
'#ifndef TITLE_ID','#define TITLE_ID "PPSA99991"','#endif',
'int sceAppInstUtilInitialize(void);','int sceAppInstUtilTerminate(void);','int sceAppInstUtilAppInstallAll(void *);',
'static int mk(const char *p){if(mkdir(p,0755)==0||errno==EEXIST)return 0;perror(p);return -1;}',
'static int put(const char *p,const unsigned char *d,size_t n){FILE *f=fopen(p,"wb");if(!f){perror(p);return -1;}int ok=fwrite(d,1,n,f)==n; if(fclose(f))ok=0;return ok?0:-1;}',
'static int reg(void){int (*fn)(const char*,const char*,void*)=0;uint32_t h=0;if(!kernel_dynlib_handle(-1,"libSceAppInstUtil.sprx",&h))fn=(void*)kernel_dynlib_resolve(-1,h,"Wudg3Xe3heE");return fn?fn(TITLE_ID,"/user/app/",0):sceAppInstUtilAppInstallAll(0);}',
]
for n,i in enumerate(entries):
    rel=i.filename
    lines += [f'__asm__(".section .rodata\\n.global asset_{n}\\nasset_{n}:\\n.incbin \\"{rel}\\"\\n.global asset_{n}_end\\nasset_{n}_end:\\n.previous\\n");',
              f'extern const unsigned char asset_{n}[],asset_{n}_end[];']
lines+=['int main(void){int rc=sceAppInstUtilInitialize();if(rc){printf("AppInst init: %d\\n",rc);return 1;}']
dirs=set()
for i in entries:
    rel=pathlib.PurePosixPath(i.filename)
    root="/system_ex/app" if rel.name=="eboot.bin" else "/user/app"
    dest=pathlib.PurePosixPath(root,*rel.parts)
    for parent in list(dest.parents)[:-1]:
        if str(parent).startswith(("/user/app/","/system_ex/app/")): dirs.add(str(parent))
for d in sorted(dirs,key=lambda x:(x.count("/"),x)):
    lines.append(f'if(mk({json.dumps(d)}))return 2;')
for n,i in enumerate(entries):
    rel=pathlib.PurePosixPath(i.filename)
    root="/system_ex/app" if rel.name=="eboot.bin" else "/user/app"
    dest=str(pathlib.PurePosixPath(root,*rel.parts))
    lines.append(f'if(put({json.dumps(dest)},asset_{n},(size_t)(asset_{n}_end-asset_{n})))return 3;')
lines+=['rc=reg();if(rc){printf("Register failed: %d\\n",rc);return 4;}',
'puts("ScrewU2Music native app installed. Launch it from the PS5 home screen.");',
'sceAppInstUtilTerminate();return 0;}']
output.write_text("\n".join(lines)+"\n")
print(f"Embedded {len(entries)} files, {sum(i.file_size for i in entries)} bytes")
