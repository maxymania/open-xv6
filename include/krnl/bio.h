#ifndef krnl_bio_h
#define krnl_bio_h

struct buf;

// bio.c
void            binit(void);
struct buf*     bread(uint, uint);
void            brelse(struct buf*);
void            bwrite(struct buf*);

#endif

