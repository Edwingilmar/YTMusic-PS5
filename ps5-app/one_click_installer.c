#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SCREWU2_BUILD "local"

int sceUserServiceInitialize(void *);
int sceAppInstUtil_installPkg(const char *, void *);
int sceSystemServiceLaunchWebBrowser(const char *, void *);

int main(void) {
    sceUserServiceInitialize(NULL);
    fprintf(stderr, "[ScrewU2Music installer] build=" SCREWU2_BUILD "\n");
    sceSystemServiceLaunchWebBrowser("http://127.0.0.1:8080/", NULL);
    return 0;
}
