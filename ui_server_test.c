/* ScrewU2Music v0.5: local UI launch experiment, PS5 payload SDK.
 * Serves a bundled HTML interface at 127.0.0.1:8787, then attempts to
 * open it with the PS5 browser. Browser support is firmware dependent.
 * This is NOT an installer, native app, or music playback implementation.
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 8787
#define SCE_NOTIFICATION_LOCAL_USER_ID_SYSTEM 0xFE
int sceNotificationSend(int userId, bool isLogged, const char *payload);
int sceSystemServiceLaunchWebBrowser(const char *uri, void *unused);
int sceUserServiceInitialize(void *unused);
static void notify(const char *msg) {
  char json[1400];
  snprintf(json,sizeof(json),
    "{\"rawData\":{\"viewTemplateType\":\"InteractiveToastTemplateB\","
    "\"channelType\":\"Downloads\",\"useCaseId\":\"IDC\","
    "\"toastOverwriteType\":\"No\",\"isImmediate\":true,\"priority\":100,"
    "\"viewData\":{\"message\":{\"body\":\"ScrewU2Music v0.5\"},"
    "\"subMessage\":{\"body\":\"%s\"},\"actions\":[]},"
    "\"platformViews\":{\"previewDisabled\":{\"viewData\":{\"message\":{\"body\":\"ScrewU2Music\"}}}}},"
    "\"localNotificationId\":\"588193129\"}",msg);
  sceNotificationSend(SCE_NOTIFICATION_LOCAL_USER_ID_SYSTEM,true,json);
}
static const char page[] =
"<!doctype html><html lang='es'><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
"<title>ScrewU2Music</title><style>body{margin:0;background:#080c09;color:white;font:24px Arial;padding:7vw}"
"h1{font-size:9vw}b{color:#31ed67}button,a{display:inline-block;background:#2aee65;color:#05150a;padding:18px 25px;border-radius:25px;text-decoration:none;border:0}"
"nav{display:flex;gap:20px;margin:30px 0}.card{background:#19251d;border-radius:20px;padding:25px;margin:15px 0}</style>"
"<h1>ScrewU2<b>Music</b></h1><nav><button onclick=\"show('Inicio')\">Inicio</button>"
"<button onclick=\"show('Buscar')\">Buscar</button><button onclick=\"show('Biblioteca')\">Biblioteca</button></nav>"
"<div class='card'><h2 id='tab'>Inicio</h2><p>Bienvenido a ScrewU2Music v0.5</p>"
"<p>Interfaz experimental para PS5. Reproduccion aun no disponible.</p>"
"<a href='https://music.youtube.com/'>YouTube Music</a></div>"
"<script>function show(t){document.getElementById('tab').textContent=t}</script></html>";
int main(void) {
  int s=socket(AF_INET,SOCK_STREAM,0);
  if(s<0){notify("ERROR: no se pudo abrir socket");return 1;}
  int yes=1;setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));
  struct sockaddr_in a;memset(&a,0,sizeof(a));a.sin_family=AF_INET;
  a.sin_addr.s_addr=htonl(INADDR_ANY);a.sin_port=htons(PORT);
  if(bind(s,(struct sockaddr*)&a,sizeof(a))<0){notify("ERROR: puerto 8787 ocupado");close(s);return 2;}
  if(listen(s,4)<0){notify("ERROR: listen fallo");close(s);return 3;}
  notify("Servidor listo: PS5 IP puerto 8787");
  sceUserServiceInitialize(NULL);
  int rc=sceSystemServiceLaunchWebBrowser("http://127.0.0.1:8787/",NULL);
  if(rc)notify("Browser fallo; abrir IP de PS5 puerto 8787 en celular");
  for(;;){
    int c=accept(s,NULL,NULL);if(c<0)continue;
    char req[1024];ssize_t n=recv(c,req,sizeof(req)-1,0);
    if(n>0){
      char head[256];int len=(int)strlen(page);
      snprintf(head,sizeof(head),"HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: %d\r\nConnection: close\r\n\r\n",len);
      send(c,head,strlen(head),0);send(c,page,len,0);
    }close(c);
  }
  return 0;
}
