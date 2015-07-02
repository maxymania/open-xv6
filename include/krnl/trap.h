#ifndef krnl_trap_h
#define krnl_trap_h
#include "../gnrc/types.h"

#include "spinlock.h"
// trap.c
void            idtinit(void);
extern uint     ticks;
void            tvinit(void);
extern struct spinlock tickslock;

#endif

