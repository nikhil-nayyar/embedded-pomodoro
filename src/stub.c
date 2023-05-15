#include "stub.h"

#define nop()  __asm__("nop")
void _exit(int a) { for(;;); }
int _sbrk() { return -1; }
int _close() { return -1; }
int _read() { return -1; }
int _fstat() { return -1; }
int _isatty() { return -1; }
int _lseek() { return -1; }

int _write(int file, char* ptr, int len)
{
   nop();
}
void foo()
{
   nop();
}