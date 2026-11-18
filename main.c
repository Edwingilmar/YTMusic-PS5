/* ScrewU2Music v0.2 - PS5 browser compatibility diagnostic.
 * This is NOT a native YouTube Music player.
 */
#include <stdio.h>
#include <stdlib.h>

int sceUserServiceInitialize(void *);
int sceUserServiceTerminate(void);
int sceSystemServiceLaunchWebBrowser(const char *uri, void *reserved);

int main(void) {
    const char *url = "https://music.youtube.com/";
    int init = sceUserServiceInitialize(NULL);
    printf("[ScrewU2Music v0.2] UserService init: 0x%08X\n", (unsigned int)init);
    if (init != 0) return EXIT_FAILURE;

    printf("[ScrewU2Music v0.2] Requesting system browser: %s\n", url);
    int result = sceSystemServiceLaunchWebBrowser(url, NULL);
    printf("[ScrewU2Music v0.2] Browser API result: 0x%08X\n", (unsigned int)result);
    if (result != 0) {
        fprintf(stderr, "[ScrewU2Music] Browser launch rejected by system service.\n");
    } else {
        printf("[ScrewU2Music] Launch request accepted; website loading is NOT verified.\n");
    }
    sceUserServiceTerminate();
    return result == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
