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

// efficient constants!
enum {
	HIGHMEMOFF = HIGHMEMBASE - MAX_REGMEM,
};

#if 1
static inline uintp v2p(void *a) {
	uintp b = ((uintp) (a));
	return b - ((uintp)(b<KERNBASE?HIGHMEMOFF:KERNBASE));
}
static inline void *p2v(uintp a) {
	return (void *) ((a) + ((uintp)(a>MAX_REGMEM?HIGHMEMOFF:KERNBASE)));
}
#endif

#if 0
static inline uintp v2p(void *a) { return ((uintp) (a)) - ((uintp)KERNBASE); }
static inline void *p2v(uintp a) { return (void *) ((a) + ((uintp)KERNBASE)); }
#endif

#define V2P(a) (((uintp) (a)) - KERNBASE)
#define P2V(a) (((void *) (a)) + KERNBASE)
#define IO2V(a) (((void *) (a)) + DEVBASE - DEVSPACE)

#endif

#define V2P_WO(x) ((x) - KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + KERNBASE)    // same as V2P, but without casts

