#ifndef krnl_log_h
#define krnl_log_h

/*
 * depends on bio.h
 * struct buf;
 */

#include "bio.h"

// log.c
void            initlog(void);
void            log_write(struct buf*);
void            begin_trans();
void            commit_trans();

#endif

