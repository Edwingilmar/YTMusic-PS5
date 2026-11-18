/* ScrewU2Music tile launcher: opens the local UI served by the payload.
 * Requires ScrewU2Music UI server running on the console.
 */
#include <unistd.h>
int sceUserServiceInitialize(void *);
int sceSystemServiceLaunchWebBrowser(const char *, void *);
int main(void) {
  sceUserServiceInitialize(0);
  sleep(1);
  return sceSystemServiceLaunchWebBrowser("http://127.0.0.1:8787/", 0);
}
