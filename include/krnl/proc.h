#ifndef krnl_proc_h
#define krnl_proc_h
#include "../gnrc/types.h"

#include "spinlock.h"

struct proc;

// proc.c
struct proc*    copyproc(struct proc*);
void            exit(void);
int             fork(void);
int             growproc(int);
int             kill(int);
void            pinit(void);
void            procdump(void);
void            scheduler(void) __attribute__((noreturn));
void            userinit(void);
int             wait(void);
void            yield(void);

void            sleep_v2(struct proc**, struct spinlock*);
void            wakeup_v2(struct proc**);

#endif

