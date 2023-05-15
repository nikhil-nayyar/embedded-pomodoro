#ifndef STUB_H
#define STUB_H

#define nop()  __asm__("nop")
void _exit(int a); 
int _sbrk();
int _close();
int _read() ;
int _fstat(); 
int _isatty();
int _lseek(); 
int _write(int file, char* ptr, int len);

int __bss_start__;
int __bss_end__;
int _kill;
int _getpid;

#endif