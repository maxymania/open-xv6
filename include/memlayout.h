// Memory layout

#define EXTMEM  0x100000            // Start of extended memory
#define PHYSTOP 0xE000000           // Top physical memory
#define DEVSPACE 0xFE000000         // Other devices are at high addresses

// first canonical kernel virtual address on AMD64
#define HIGHMEMBASE 0xFFFF800000000000

// Key addresses for address space layout (see kmap in vm.c for layout)
#define KERNBASE 0xFFFFFFFF80000000 // First kernel virtual address
#define DEVBASE  0xFFFFFFFF40000000 // First device virtual address

#define KERNLINK (KERNBASE+EXTMEM)  // Address where kernel is linked

#define MAX_REGMEM 0x80000000 // maximum regular memory (above KERNBASE)
#define MAX_EXTMEM (DEVBASE-HIGHMEMBASE) // maximum extended memory (stored below KERNBASE)

#ifndef __ASSEMBLER__

static inline uintp v2p(void *a) {
	uintp b = ((uintp) (a));
	if((b<DEVBASE)&&(b>=HIGHMEMBASE))
		return b - ((uintp)HIGHMEMBASE) + ((uintp)MAX_REGMEM);
	return b - ((uintp)KERNBASE);
}
static inline void *p2v(uintp a) {
	if(a<MAX_REGMEM)
		return (void *) ((a) + ((uintp)KERNBASE));
	return (void *) ((a) - ((uintp)MAX_REGMEM) + ((uintp)HIGHMEMBASE));
}

#define V2P(a) (((uintp) (a)) - KERNBASE)
#define P2V(a) (((void *) (a)) + KERNBASE)
#define IO2V(a) (((void *) (a)) + DEVBASE - DEVSPACE)

#endif

#define V2P_WO(x) ((x) - KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE)    // same as V2P, but without casts

