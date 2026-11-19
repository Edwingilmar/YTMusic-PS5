/* ScrewU2Music: local HTML server for PS5 browser.
 * Built as the installed title's eboot, not the installer payload.
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define REQUEST_SIZE 2048
int sceUserServiceInitialize(void *);
int sceSystemServiceLaunchWebBrowser(const char *, void *);
#include "ui_page.h"

static int send_all(int fd, const void *data, size_t size) {
    const char *p = (const char *)data;
    while (size) {
        ssize_t n = send(fd, p, size, 0);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) return -1;
        p += n;
        size -= (size_t)n;
    }
    return 0;
}

static void respond(int client) {
    char req[REQUEST_SIZE];
    ssize_t n = recv(client, req, sizeof(req) - 1, 0);
    if (n <= 0) return;
    req[n] = 0;
    const int is_get = !strncmp(req, "GET / ", 6) ||
                       !strncmp(req, "GET /index.html ", 16);
    const int is_head = !strncmp(req, "HEAD / ", 7) ||
                        !strncmp(req, "HEAD /index.html ", 17);
    const char *body = is_get || is_head ? page : "Not found\n";
    size_t length = is_get || is_head ? sizeof(page) - 1 : 10;
    char header[320];
    int h = snprintf(header, sizeof(header),
        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Cache-Control: no-store\r\n"
        "Connection: close\r\n\r\n",
        is_get || is_head ? "200 OK" : "404 Not Found",
        is_get || is_head ? "text/html; charset=utf-8" : "text/plain",
        length);
    if (h <= 0 || (size_t)h >= sizeof(header)) return;
    if (send_all(client, header, (size_t)h)) return;
    if (!is_head) send_all(client, body, length);
}

int main(void) {
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) { perror("socket"); return 1; }
    int reuse = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(server, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind"); close(server); return 2;
    }
    if (listen(server, 8) < 0) {
        perror("listen"); close(server); return 3;
    }
    fprintf(stderr, "[ScrewU2Music] HTTP listening on 127.0.0.1:%d\n", PORT);
    int user_rc = sceUserServiceInitialize(NULL);
    fprintf(stderr, "[ScrewU2Music] UserService: 0x%x\n", user_rc);
    int browser_rc = sceSystemServiceLaunchWebBrowser("http://127.0.0.1:8080/", NULL);
    fprintf(stderr, "[ScrewU2Music] Browser launch: 0x%x\n", browser_rc);
    /* Keep the app alive even if the browser request is rejected.
       The loop exits when the PS5 terminates this title. */
    for (;;) {
        int client = accept(server, NULL, NULL);
        if (client < 0) {
            if (errno == EINTR) continue;
            perror("accept");
            break;
        }
        respond(client);
        close(client);
    }
    close(server);
    return 4;
}
