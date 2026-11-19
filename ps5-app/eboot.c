/* ScrewU2Music standalone PS5 tile application: embeds local UI server. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 8787
int sceSystemServiceLaunchWebBrowser(const char *uri, void *unused);
int sceUserServiceInitialize(void *unused);
#include "ui_page.h"
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
