#ifndef krnl_header_only_h
#define krnl_header_only_h
#include "../gnrc/types.h"

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

#endif

