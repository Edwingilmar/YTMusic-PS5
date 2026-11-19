/* Native PS5 boot probe, based on the SDK install_app eboot entry convention.
 * Deliberately avoids dynamic linking and fake ELF header patching.
 * First establish that the home-screen tile can start a valid SELF.
 */
long __syscall(long n, ...) {
  long a1=0,a2=0,a3=0,a4=0,a5=0,a6=0;
  __builtin_va_list ap;
  unsigned long ret;
  char error;
  __builtin_va_start(ap,n);
  a1=__builtin_va_arg(ap,long);a2=__builtin_va_arg(ap,long);
  a3=__builtin_va_arg(ap,long);a4=__builtin_va_arg(ap,long);
  a5=__builtin_va_arg(ap,long);a6=__builtin_va_arg(ap,long);
  __builtin_va_end(ap);
  register long r10 __asm__("r10")=a4;
  register long r8 __asm__("r8")=a5;
  register long r9 __asm__("r9")=a6;
  __asm__ __volatile__("syscall": "=a"(ret),"=@ccc"(error),"+r"(r10),"+r"(r8),"+r"(r9):"a"(n),"D"(a1),"S"(a2),"d"(a3):"rcx","r11","memory");
  return error?-ret:ret;
}
int main(void) {
  static const char message[]="ScrewU2Music native PS5 eboot started successfully\n";
  __syscall(0x259,7,message,0);
  /* Stay alive so a successful start can be distinguished from an immediate exit. */
  for (;;) __asm__ __volatile__("pause");
  return 0;
}
void _start(void) { __syscall(1,main()); __builtin_trap(); }
