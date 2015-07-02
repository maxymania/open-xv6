#ifndef krnl_console_h
#define krnl_console_h
#include "../gnrc/types.h"

// console.c
void            consoleinit(void);
void            cprintf(char*, ...);
void            consoleintr(int(*)(void));
void            panic(char*) __attribute__((noreturn));

#endif

