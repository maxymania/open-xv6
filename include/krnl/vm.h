#ifndef krnl_vm_h
#define krnl_vm_h
#include "../gnrc/types.h"
#include "../gnrc/aptable.h"

#include "proc.h"
#include "file_structs.h"

// vm.c
void            seginit(void);
void            kvmalloc(void);
void            vmenable(void);
void            switchuvm(struct proc*);
void            switchkvm(void);
int             copyout(pde_t*, uint, void*, uint);


pagetab_t*      setupkvm_v2(void);
void            inituvm_v2(pagetab_t*, char*, uint);
int             loaduvm_v2(pagetab_t*, char*, struct inode*, uint, uint);
int             allocuvm_v2(pagetab_t*, uint, uint);
int             deallocuvm_v2(pagetab_t*, uintp, uintp);
pagetab_t*      copyuvm_v2(pagetab_t*, uint);
void            freevm_v2(pagetab_t*);
char*           uva2ka_v2(pagetab_t*, char*);
void            clearpteu_v2(pagetab_t *tab, char *uva);
int             copyout_v2(pagetab_t*, uint, void*, uint);

//OLD
pde_t*          setupkvm(void);
char*           uva2ka(pde_t*, char*);
int             allocuvm(pde_t*, uint, uint);
int             deallocuvm(pde_t*, uintp, uintp);
void            freevm(pde_t*);
void            inituvm(pde_t*, char*, uint);
int             loaduvm(pde_t*, char*, struct inode*, uint, uint);
pde_t*          copyuvm(pde_t*, uint);
void            clearpteu(pde_t *pgdir, char *uva);
//END OLD

#endif

