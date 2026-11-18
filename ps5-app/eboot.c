/* ScrewU2Music standalone PS5 tile application: embeds local UI server. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 8787
int sceSystemServiceLaunchWebBrowser(const char *uri, void *unused);
int sceUserServiceInitialize(void *unused);
static const char page[] =
"<!doctype html><html lang='es'><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
"<title>ScrewU2Music</title><style>body{margin:0;background:#080c09;color:white;font:24px Arial;padding:7vw}"
"h1{font-size:9vw}b{color:#31ed67}button,a{display:inline-block;background:#2aee65;color:#05150a;padding:18px 25px;border-radius:25px;text-decoration:none;border:0}"
"nav{display:flex;gap:20px;margin:30px 0}.card{background:#19251d;border-radius:20px;padding:25px;margin:15px 0}</style>"
"<h1>ScrewU2<b>Music</b></h1><nav><button onclick=\"show('Inicio')\">Inicio</button>"
"<button onclick=\"show('Buscar')\">Buscar</button><button onclick=\"show('Biblioteca')\">Biblioteca</button></nav>"
"<div class='card'><h2 id='tab'>Inicio</h2><p>Bienvenido a ScrewU2Music</p>"
"<p>Aplicacion instalada en PS5. Reproduccion en desarrollo.</p>"
"<p style='color:#b2c4b7'>YouTube Music no abre directamente en el navegador PS5. La integracion de reproduccion sigue pendiente.</p></div>"
"<footer style='margin-top:8vh;text-align:center;color:#a2b3a7;font-size:16px'>Created by Gilmar</footer>"
"<script>function show(t){document.getElementById('tab').textContent=t}</script></html>";
int main(void) {
  int s=socket(AF_INET,SOCK_STREAM,0);
  if(s<0){fprintf(stderr,"socket failed\n");return 1;}
  int yes=1;setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes));
  struct sockaddr_in a;memset(&a,0,sizeof(a));a.sin_family=AF_INET;
  a.sin_addr.s_addr=htonl(INADDR_ANY);a.sin_port=htons(PORT);
  if(bind(s,(struct sockaddr*)&a,sizeof(a))<0){fprintf(stderr,"port busy\n");close(s);return 2;}
  if(listen(s,4)<0){fprintf(stderr,"listen failed\n");close(s);return 3;}
  sceUserServiceInitialize(NULL);
  int rc=sceSystemServiceLaunchWebBrowser("http://127.0.0.1:8787/",NULL);
  if(rc)fprintf(stderr,"browser launch failed: %d\n",rc);
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
