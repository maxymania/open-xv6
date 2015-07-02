#ifndef GNRC_TYPE_H
#define GNRC_TYPE_H

typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

typedef unsigned int  uint32;
typedef unsigned long uint64;

#if X64
typedef unsigned long uintp;
#else
typedef unsigned int  uintp;
#endif

typedef uintp pde_t;

#endif

