#ifndef krnl_swtch_h
#define krnl_swtch_h
#include "../gnrc/types.h"

struct context;

// swtch.S
void            swtch(struct context**, struct context*);

#endif

