#ifndef krnl_file_h
#define krnl_file_h
#include "../gnrc/types.h"

#include "file_structs.h"

// file.c
struct file*    filealloc(void);
void            fileclose(struct file*);
struct file*    filedup(struct file*);
void            fileinit(void);
int             fileread(struct file*, char*, int n);
int             filestat(struct file*, struct stat*);
int             filewrite(struct file*, char*, int n);


#endif

