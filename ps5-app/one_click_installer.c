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

#define TITLE_ID "SCUM00001"
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

INCASSET(eboot, "SCUM00001/eboot.bin")
INCASSET(icon0, "SCUM00001/icon0.png")
INCASSET(param, "SCUM00001/sce_sys/param.json")

typedef struct app_launch_ctx {\n  uint32_t structsize;\n  uint32_t user_id;\n  uint32_t app_opt;\n  uint64_t crash_report;\n  uint32_t check_flag;\n} app_launch_ctx_t;\nint sceUserServiceInitialize(void *);\nint sceUserServiceGetForegroundUser(uint32_t *);\nint sceSystemServiceLaunchApp(const char *, char **, app_launch_ctx_t *);\nint sceAppInstUtilInitialize(void);
int sceAppInstUtilTerminate(void);
int sceAppInstUtilAppInstallAll(void *);

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
  int (*install_title)(const char*, const char*, void*) = 0;
  uint32_t handle = 0;
  if (!kernel_dynlib_handle(-1, "libSceAppInstUtil.sprx", &handle))
    install_title = (void*)kernel_dynlib_resolve(-1, handle, "Wudg3Xe3heE");
  if (install_title) return install_title(TITLE_ID, "/user/app/", 0);
  return sceAppInstUtilAppInstallAll(0);
}
int main(void) {
  int err = sceAppInstUtilInitialize();
  if (err) { printf("AppInstUtil init failed: 0x%08x\n", err); return 1; }
  if (ensure_dir("/user/app/" TITLE_ID) ||
      ensure_dir("/user/app/" TITLE_ID "/sce_sys") ||
      write_asset("/user/app/" TITLE_ID "/eboot.bin", eboot, eboot_size) ||
      write_asset("/user/app/" TITLE_ID "/sce_sys/icon0.png", icon0, icon0_size) ||
      write_asset("/user/app/" TITLE_ID "/sce_sys/param.json", param, param_size)) {
    sceAppInstUtilTerminate();
    return 2;
  }
  err = register_app();
  if (err) printf("ScrewU2Music register failed: 0x%08x\n", err);
  else printf("ScrewU2Music tile registered.\n");
  sceAppInstUtilTerminate();
  if (err) return 3;
  app_launch_ctx_t ctx = {0};
  err = sceUserServiceInitialize(0);
  if (err) { printf("UserService init failed: 0x%08x\\n", err); return 4; }
  err = sceUserServiceGetForegroundUser(&ctx.user_id);
  if (err) { printf("Foreground user failed: 0x%08x\\n", err); return 5; }
  char *args[] = {0};
  err = sceSystemServiceLaunchApp(TITLE_ID, args, &ctx);
  if (err < 0) { printf("Auto launch failed: 0x%08x\\n", err); return 6; }
  printf("ScrewU2Music launch requested.\\n");
  return 0;
}
