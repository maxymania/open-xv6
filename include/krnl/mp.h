#ifndef krnl_mp_h
#define krnl_mp_h

// mp.c
extern int      ismp;
int             mpbcpu(void);
void            mpinit(void);
void            mpstartthem(void);

#endif

