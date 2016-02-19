#ifndef aptables_h
#define aptables_h

#include <gnrc/types.h>
#include <gnrc/aptable.h>

struct pagetab {
	pde_t* pgdir;
	pde_t* pml4;
};

#endif


