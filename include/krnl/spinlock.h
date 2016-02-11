#ifndef krnl_spinlock_h
#define krnl_spinlock_h
#include "../gnrc/types.h"

// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?
  
  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
  uintp pcs[10];      // The call stack (an array of program counters)
                     // that locked the lock.
};


// spinlock.c
void            acquire(struct spinlock*);
void            getcallerpcs(void*, uintp*);
void            getstackpcs(uintp*, uintp*);
int             holding(struct spinlock*);
void            initlock(struct spinlock*, char*);
void            release(struct spinlock*);
void            pushcli(void);
void            popcli(void);

#endif

