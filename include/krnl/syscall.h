#ifndef krnl_syscall_h
#define krnl_syscall_h
#include "../gnrc/types.h"

// syscall.c
int             argint(int, int*);
int             argptr(int, char**, int);
int             argstr(int, char**);
int             arguintp(int, uintp*);
int             fetchuintp(uintp, uintp*);
int             fetchstr(uintp, char**);
void            syscall(void);

#endif

