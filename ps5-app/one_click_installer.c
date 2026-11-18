/* ScrewU2Music one-shot home-screen tile installer.
 * Bundles all title files inside one ELF. No FTP staging or app.db edits.
 * Based on ps5-payload-dev/ftpsrv install-ps5.c (GPL-3.0-or-later).
 * Experimental: requires a jailbroken PS5 with app-install privileges.
 */
#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ps5/kernel.h>
#include <stdlib.h>
#include <string.h>

#define TITLE_ID "SCUM00001"
typedef struct app_launch_ctx {
  uint32_t structsize;
  uint32_t user_id;
  uint32_t app_opt;
  uint64_t crash_report;
  uint32_t check_flag;
} app_launch_ctx_t;
int sceUserServiceInitialize(void *);
int sceUserServiceGetForegroundUser(uint32_t *);
int sceSystemServiceLaunchApp(const char *, char **, app_launch_ctx_t *);
int sceAppInstUtilInitialize(void);
int sceAppInstUtilTerminate(void);
int sceAppInstUtilAppInstallAll(void *);

static int ensure_dir(const char *p) {
  if (mkdir(p, 0755) == 0 || errno == EEXIST) return 0;
  perror(p);
  return -1;
}
/* PS5 native HTTPS: fetch each release asset, never overwrite a working file
 * until its replacement has downloaded successfully. */
extern int sceNetInit(void);
extern int sceNetPoolCreate(const char *, int, int);
extern int sceSslInit(size_t);
extern int sceHttpInit(int, int, size_t);
extern int sceHttpCreateTemplate(int, const char *, int, int);
extern int sceHttpSetAutoRedirect(int, int);
extern int sceHttpCreateConnectionWithURL(int, const char *, int);
extern int sceHttpCreateRequestWithURL(int, int, const char *, uint64_t);
extern int sceHttpSendRequest(int, const void *, size_t);
extern int sceHttpGetStatusCode(int, int *);
extern int sceHttpReadData(int, void *, size_t);
extern int sceHttpDeleteRequest(int);
extern int sceHttpDeleteConnection(int);
extern int sceHttpDeleteTemplate(int);
extern int sceHttpTerm(int);
extern int sceSslTerm(int);
extern int sceNetPoolDestroy(int);

#define BASE "https://github.com/Edwingilmar/YTMusic-PS5/releases/download/auto-latest/"
static int http_ctx=-1, ssl_ctx=-1, net_pool=-1, tmpl=-1;
static int network_init(void) {
  sceNetInit();
  net_pool=sceNetPoolCreate("screwu2", 512*1024, 0);
  if(net_pool<0)return net_pool;
  ssl_ctx=sceSslInit(512*1024);
  if(ssl_ctx<0)return ssl_ctx;
  http_ctx=sceHttpInit(net_pool,ssl_ctx,512*1024);
  if(http_ctx<0)return http_ctx;
  tmpl=sceHttpCreateTemplate(http_ctx,"ScrewU2MusicInstaller/1.0",1,1);
  if(tmpl<0)return tmpl;
  sceHttpSetAutoRedirect(tmpl,1);
  return 0;
}
static void network_close(void) {
  if(tmpl>=0)sceHttpDeleteTemplate(tmpl);
  if(http_ctx>=0)sceHttpTerm(http_ctx);
  if(ssl_ctx>=0)sceSslTerm(ssl_ctx);
  if(net_pool>=0)sceNetPoolDestroy(net_pool);
}
static int download(const char *asset,const char *dest) {
  char url[256],tmp[256],buf[16384];
  snprintf(url,sizeof(url),"%s%s",BASE,asset);
  snprintf(tmp,sizeof(tmp),"%s.tmp",dest);
  int conn=sceHttpCreateConnectionWithURL(tmpl,url,0);
  if(conn<0)return conn;
  int req=sceHttpCreateRequestWithURL(conn,0,url,0);
  if(req<0){sceHttpDeleteConnection(conn);return req;}
  int err=sceHttpSendRequest(req,0,0),status=0;
  if(err>=0)err=sceHttpGetStatusCode(req,&status);
  if(err>=0 && status!=200)err=-status;
  FILE *out=NULL;
  size_t total=0;
  if(err>=0)out=fopen(tmp,"wb");
  if(err>=0 && !out)err=-1;
  while(err>=0) {
    int n=sceHttpReadData(req,buf,sizeof(buf));
    if(n<0){err=n;break;}
    if(n==0)break;
    if(fwrite(buf,1,(size_t)n,out)!=(size_t)n){err=-2;break;}
    total+=(size_t)n;
    if(total>32*1024*1024){err=-3;break;}
  }
  if(out && fclose(out))err=-4;
  sceHttpDeleteRequest(req);
  sceHttpDeleteConnection(conn);
  if(err>=0 && total>0 && rename(tmp,dest)==0) {
    printf("Downloaded %s (%zu bytes)\\n",asset,total);
    return 0;
  }
  remove(tmp);
  return err<0?err:-5;
}
static int register_app(void) {
  int (*install_title)(const char*, const char*, void*) = 0;
  uint32_t handle = 0;
  if (!kernel_dynlib_handle(-1, "libSceAppInstUtil.sprx", &handle))
    install_title = (void*)kernel_dynlib_resolve(-1, handle, "Wudg3Xe3heE");
  if (install_title) return install_title(TITLE_ID, "/user/app/", 0);
  return sceAppInstUtilAppInstallAll(0);
}
int main(void) {
  int err=network_init();
  if(err<0){printf("Network init failed: 0x%08x\\n",err);network_close();return 1;}
  if(ensure_dir("/user/app/" TITLE_ID) ||
     ensure_dir("/user/app/" TITLE_ID "/sce_sys")) {
    network_close();return 2;
  }
  /* Release assets are generated together by GitHub Actions. */
  err=download("ScrewU2Music-eboot.bin","/user/app/" TITLE_ID "/eboot.bin");
  if(!err)err=download("ScrewU2Music-icon0.png","/user/app/" TITLE_ID "/sce_sys/icon0.png");
  if(!err)err=download("ScrewU2Music-param.json","/user/app/" TITLE_ID "/sce_sys/param.json");
  network_close();
  if(err){printf("Download failed: 0x%08x\\n",err);return 3;}
  err=sceAppInstUtilInitialize();
  if(err){printf("AppInstUtil init failed: 0x%08x\\n",err);return 4;}
  err=register_app();
  sceAppInstUtilTerminate();
  if(err){printf("Registration failed: 0x%08x\\n",err);return 5;}
  app_launch_ctx_t ctx={0};
  err=sceUserServiceInitialize(0);
  if(err)return 6;
  err=sceUserServiceGetForegroundUser(&ctx.user_id);
  if(err)return 7;
  char *args[]={0};
  err=sceSystemServiceLaunchApp(TITLE_ID,args,&ctx);
  printf("Launch result: 0x%08x\\n",err);
  return err<0?8:0;
}
