#ifndef krnl_kalloc_h
#define krnl_kalloc_h
#include "../gnrc/types.h"

// kalloc.c
char*           kalloc(void);
void            kfree(char*);
void            kinit1(void*, void*);
void            kinit2(void*, void*);

#endif

