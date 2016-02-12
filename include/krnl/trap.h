#ifndef krnl_trap_h
#define krnl_trap_h
#include <gnrc/types.h>

#include <krnl/spinlock.h>

struct proc;

// trap.c
void            idtinit(void);
extern uint     ticks;
void            tvinit(void);
extern struct spinlock tickslock;
extern struct proc* ticksqueue;

#endif

