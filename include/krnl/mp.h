#ifndef krnl_mp_h
#define krnl_mp_h
#include "../gnrc/types.h"

// mp.c
extern int      ismp;
int             mpbcpu(void);
void            mpinit(void);
void            mpstartthem(void);

#endif

