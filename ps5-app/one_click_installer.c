/* ScrewU2Music one-shot home-screen tile installer.
 * Bundles all title files inside one ELF. No FTP staging or app.db edits.
 * Based on ps5-payload-dev/ftpsrv install-ps5.c (GPL-3.0-or-later).
 * Experimental: requires a jailbroken PS5 with app-install privileges.
 */
#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ui_page.h"
#include "logo_asset.h"
#include <sys/stat.h>
#include <ps5/kernel.h>

#define TITLE_ID "SCUM00002"
#ifndef SCREWU2_BUILD
#define SCREWU2_BUILD "local"
#endif
static const char screwu2_build_id[] __attribute__((used)) = "ScrewU2Music-build:" SCREWU2_BUILD;
#define INCASSET(name, file) \
  __asm__(".section .rodata\n" \
          ".global " #name "\n" \
          ".global " #name "_end\n" \
          ".global " #name "_size\n" \
          ".align 16\n" \
          #name ":\n" \
          ".incbin \"" file "\"\n" \
          #name "_end:\n" \
          #name "_size:\n" \
          ".quad " #name "_end - " #name "\n" \
          ".previous\n"); \
  extern const uint8_t name[]; \
  extern const size_t name##_size;

INCASSET(eboot, "SCUM00002/eboot.bin")
INCASSET(icon0, "SCUM00002/icon0.png")
INCASSET(param, "SCUM00002/sce_sys/param.json")

int sceAppInstUtilInitialize(void);
int sceAppInstUtilTerminate(void);
int sceAppInstUtilAppInstallAll(void *);

/* The media tile opens the deeplink, never the freestanding eboot.
 * Keep this payload alive as the local HTTP service for the approved UI.
 * A restart of the PS5 requires reloading the payload. */
static int serve_ui(void) {
  int fd=socket(AF_INET,SOCK_STREAM,0);
  if(fd<0){perror("ScrewU2Music socket");return 10;}
  int yes=1;
  setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));
  struct sockaddr_in a;
  memset(&a,0,sizeof(a));
  a.sin_family=AF_INET;
  a.sin_port=htons(8787);
  a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
  if(bind(fd,(struct sockaddr*)&a,sizeof(a))<0){perror("ScrewU2Music bind");close(fd);return 11;}
  if(listen(fd,8)<0){perror("ScrewU2Music listen");close(fd);return 12;}
  printf("ScrewU2Music UI ready at http://127.0.0.1:8787/\\n");
  for(;;) {
    int c=accept(fd,0,0);
    if(c<0)continue;
    char req[2048];
    ssize_t n=recv(c,req,sizeof(req)-1,0);
    if(n>0) {
      req[n]=0;
      int logo=!strncmp(req,"GET /assets/logo.svg ",21)||!strncmp(req,"HEAD /assets/logo.svg ",22);
      int head=!strncmp(req,"HEAD ",5);
      int ok=logo||!strncmp(req,"GET / ",6)||!strncmp(req,"GET /index.html ",16)||
             !strncmp(req,"HEAD / ",7)||!strncmp(req,"HEAD /index.html ",17);
      const char *body=logo?logo_svg:(ok?page:"Not found\\n");
      size_t len=logo?sizeof(logo_svg)-1:(ok?sizeof(page)-1:10);
      char header[256];
      int count=snprintf(header,sizeof(header),
          "HTTP/1.1 %s\\r\\nContent-Type: %s\\r\\nContent-Length: %zu\\r\\nCache-Control: no-store\\r\\nConnection: close\\r\\n\\r\\n",
          ok?"200 OK":"404 Not Found",logo?"image/svg+xml":(ok?"text/html; charset=utf-8":"text/plain"),len);
      if(count>0 && (size_t)count<sizeof(header)) {
        size_t off=0;
        while(off<(size_t)count){ssize_t w=send(c,header+off,(size_t)count-off,0);if(w<=0)break;off+=(size_t)w;}
        if(!head){off=0;while(off<len){ssize_t w=send(c,body+off,len-off,0);if(w<=0)break;off+=(size_t)w;}}
      }
    }
    close(c);
  }
}

static int ensure_dir(const char *p) {
  if (mkdir(p, 0755) == 0 || errno == EEXIST) return 0;
  perror(p);
  return -1;
}
static int write_asset(const char *p, const uint8_t *data, size_t size) {
  FILE *f = fopen(p, "wb");
  if (!f) { perror(p); return -1; }
  size_t n = fwrite(data, 1, size, f);
  int e = fclose(f);
  if (n != size || e) { printf("Write failed: %s\n", p); return -1; }
  return 0;
}
static int register_app(void) {
  /* The exported AppInstallTitleDir entry uses an NID in the PS5 SDK.
   * Resolve it exactly as the working ps5-browser-app installer does. */
  int (*install_title)(const char*,const char*,void*)=0;
  uint32_t handle=0;
  if(!kernel_dynlib_handle(-1,"libSceAppInstUtil.sprx",&handle))
    install_title=(void*)kernel_dynlib_resolve(-1,handle,"Wudg3Xe3heE");
  if(install_title)return install_title(TITLE_ID,"/user/app/",0);
  return sceAppInstUtilAppInstallAll(0);
}
int main(void) {
  printf("ScrewU2Music installer build: %s\n", screwu2_build_id);
  int err = sceAppInstUtilInitialize();
  if (err) { printf("AppInstUtil init failed: 0x%08x\n", err); return 1; }
  if (ensure_dir("/user/app/" TITLE_ID) ||
      ensure_dir("/user/app/" TITLE_ID "/sce_sys") ||
      ensure_dir("/system_ex/app/" TITLE_ID) ||
      ensure_dir("/system_ex/app/" TITLE_ID "/sce_sys") ||
      write_asset("/system_ex/app/" TITLE_ID "/eboot.bin", eboot, eboot_size) ||
      write_asset("/user/app/" TITLE_ID "/sce_sys/icon0.png", icon0, icon0_size) ||
      write_asset("/user/app/" TITLE_ID "/sce_sys/param.json", param, param_size)) {
    sceAppInstUtilTerminate();
    return 2;
  }
  err = register_app();
  if (err) printf("ScrewU2Music register failed: 0x%08x\n", err);
  else printf("ScrewU2Music tile registered.\n");
  if (!err && write_asset("/system_ex/app/" TITLE_ID "/sce_sys/param.json", param, param_size)) err = -1;
  sceAppInstUtilTerminate();
  if (err) return 3;
  printf("ScrewU2Music media tile installed. Start the tile while this payload is running.\\n");
  return serve_ui();
}
