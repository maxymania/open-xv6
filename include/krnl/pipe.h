#ifndef krnl_pipe_h
#define krnl_pipe_h

#include "file_structs.h"

struct pipe;
// pipe.c
int             pipealloc(struct file**, struct file**);
void            pipeclose(struct pipe*, int);
int             piperead(struct pipe*, char*, int);
int             pipewrite(struct pipe*, char*, int);

#endif

