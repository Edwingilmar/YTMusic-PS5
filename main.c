/* YouTube Music PS5 launcher v0.1, experimental */
#include <stdio.h>
#include <stdlib.h>
int sceUserServiceInitialize(void *);
int sceUserServiceTerminate(void);
int sceSystemServiceLaunchWebBrowser(const char *uri, void *reserved);
__attribute__((constructor)) static void initialize(void) {
    if (sceUserServiceInitialize(NULL) != 0)
        fprintf(stderr, "UserService initialization failed\n");
}
int main(void) {
    int result = sceSystemServiceLaunchWebBrowser("https://music.youtube.com/", NULL);
    if (result != 0) {
        fprintf(stderr, "Browser launch failed: %d\n", result);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
__attribute__((destructor)) static void cleanup(void) {
    sceUserServiceTerminate();
}
