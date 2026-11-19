/* ScrewU2Music: standalone PS5 fake-SELF HTTP server.
 * install_app/eboot.x is a static, freestanding entry point: no libc or
 * imported Sce libraries are available in this eboot.
 */
#include <stddef.h>
#include <stdint.h>
#include "ui_page.h"

#define PORT 8080
#define AF_INET 2
#define SOCK_STREAM 1
#define SOL_SOCKET 0xffff
#define SO_REUSEADDR 4

static long syscall6(long n,long a,long b,long c,long d,long e,long f) {
    register long r10 __asm__("r10")=d;
    register long r8 __asm__("r8")=e;
    register long r9 __asm__("r9")=f;
    unsigned long ret;
    unsigned char carry;
    __asm__ volatile("syscall" : "=a"(ret),"=@ccc"(carry),"+r"(r10),"+r"(r8),"+r"(r9)
      : "a"(n),"D"(a),"S"(b),"d"(c) : "rcx","r11","memory");
    return carry ? -(long)ret : (long)ret;
}
static long sc(long n,long a,long b,long c,long d,long e,long f) {
    return syscall6(n,a,b,c,d,e,f);
}
static size_t str_len(const char *s) {size_t n=0;while(s[n])n++;return n;}
static int starts(const char *s,const char *p) {
    while(*p)if(*s++!=*p++)return 0;
    return 1;
}
static void send_all(int fd,const char *s,size_t n) {
    while(n) {
        long sent=sc(133,fd,(long)s,n,0,0,0); /* sendto */
        if(sent<=0)return;
        s+=sent;n-=(size_t)sent;
    }
}
static void respond(int fd) {
    char req[2048];
    long n=sc(29,fd,(long)req,sizeof(req)-1,0,0,0); /* recvfrom */
    if(n<=0)return;
    req[n]=0;
    int head=starts(req,"HEAD / ")||starts(req,"HEAD /index.html ");
    int ok=head||starts(req,"GET / ")||starts(req,"GET /index.html ");
    const char *body=ok?page:"Not found\n";
    size_t len=ok?sizeof(page)-1:10;
    /* Content-Length is computed without libc. */
    char digits[32],reversed[32];size_t nd=0;
    size_t v=len;
    do {reversed[nd++]=(char)('0'+v%10);v/=10;}while(v);
    for(size_t i=0;i<nd;i++)digits[i]=reversed[nd-i-1];
    const char *status=ok?"HTTP/1.1 200 OK\r\n":"HTTP/1.1 404 Not Found\r\n";
    send_all(fd,status,str_len(status));
    const char *type=ok?"Content-Type: text/html; charset=utf-8\r\n":"Content-Type: text/plain\r\n";
    send_all(fd,type,str_len(type));
    send_all(fd,"Content-Length: ",16);
    send_all(fd,digits,nd);
    const char *tail="\r\nCache-Control: no-store\r\nConnection: close\r\n\r\n";
    send_all(fd,tail,str_len(tail));
    if(!head)send_all(fd,body,len);
}
int main(void) {
    /* FreeBSD/PS5 sockaddr_in layout: length, family, port, IPv4. */
    struct {uint8_t len,family;uint16_t port;uint32_t ip;uint8_t zero[8];} addr=
      {16,AF_INET,(uint16_t)((PORT>>8)|(PORT<<8)),0x0100007f,{0}};
    long server=sc(97,AF_INET,SOCK_STREAM,0,0,0,0); /* socket */
    if(server<0)return 1;
    int yes=1;
    sc(105,server,SOL_SOCKET,SO_REUSEADDR,(long)&yes,sizeof(yes),0);
    if(sc(104,server,(long)&addr,sizeof(addr),0,0,0)<0)return 2; /* bind */
    if(sc(106,server,8,0,0,0,0)<0)return 3; /* listen */
    for(;;) {
        long client=sc(30,server,0,0,0,0,0); /* accept */
        if(client<0)continue;
        respond((int)client);
        sc(6,client,0,0,0,0,0); /* close */
    }
}
void _start(void) {
    long rc=main();
    sc(1,rc,0,0,0,0,0);
    __builtin_trap();
}
