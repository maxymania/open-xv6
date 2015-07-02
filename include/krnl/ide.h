#ifndef krnl_ide_h
#define krnl_ide_h

/*
 * depends on bio.h
 * struct buf;
 */

#include "bio.h"

// ide.c
void            ideinit(void);
void            ideintr(void);
void            iderw(struct buf*);

#endif

