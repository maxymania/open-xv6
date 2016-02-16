/* 
 *
 * Copyright (c) 2016 Simon Schmidt
 * Copyright (c) 2013 Brian Swetland
 * Copyright (c) 2006-2009 Frans Kaashoek, Robert Morris, Russ Cox,
 *                         Massachusetts Institute of Technology
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*
 * TODO: clean up or throw out.
 */

#include <param.h>
#include <types.h>
#include <defs.h>
#include <x86.h>
#include <memlayout.h>
#include <mmu.h>
#include <proc.h>
#include <elf.h>

#include <gnrc/aptable.h>
#include <gnrc/aptable_s.h>

extern char data[];  // defined by kernel.ld
pde_t *kpgdir;  // for use in scheduler()
struct segdesc gdt[NSEGS];

#ifndef X64
#error "Only AMD64 is supported!"
#endif

// Return the address of the PTE in page table pgdir
// that corresponds to virtual address va.  If alloc!=0,
// create any required page table pages.
static pte_t *
walkpgdir(pde_t *pgdir, const void *va, int alloc)
{
  pde_t *pde;
  pte_t *pgtab;

  pde = &pgdir[PDX(va)];
  if(*pde & PTE_P){
    pgtab = (pte_t*)p2v(PTE_ADDR(*pde));
  } else {
    if(!alloc || (pgtab = (pte_t*)kalloc()) == 0)
      return 0;
    // Make sure all those PTE_P bits are zero.
    memset(pgtab, 0, PGSIZE);
    // The permissions here are overly generous, but they can
    // be further restricted by the permissions in the page table 
    // entries, if necessary.
    *pde = v2p(pgtab) | PTE_P | PTE_W | PTE_U;
  }
  return &pgtab[PTX(va)];
}
static pte_t *
walkpgdir_v2(pagetab_t* tab, const void *va, int alloc){
  return walkpgdir(tab->pgdir,va,alloc);
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not
// be page-aligned.
static int
mappages_v2(pagetab_t* tab, void *va, uintp size, uintp pa, int perm)
{
  char *a, *last;
  pte_t *pte;
  
  a = (char*)PGROUNDDOWN((uintp)va);
  last = (char*)PGROUNDDOWN(((uintp)va) + size - 1);
  for(;;){
    if((pte = walkpgdir_v2(tab, a, 1)) == 0)
      return -1;
    if(*pte & PTE_P)
      panic("remap");
    *pte = pa | perm | PTE_P;
    if(a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  return 0;
}


/******************************************************************************

// There is one page table per process, plus one that's used when
// a CPU is not running any process (kpgdir). The kernel uses the
// current process's page table during system calls and interrupts;
// page protection bits prevent user code from using the kernel's
// mappings.
// 
// setupkvm() and exec() set up every page table like this:
//
//   0..KERNBASE: user memory (text+data+stack+heap), mapped to
//                phys memory allocated by the kernel
//   KERNBASE..KERNBASE+EXTMEM: mapped to 0..EXTMEM (for I/O space)
//   KERNBASE+EXTMEM..data: mapped to EXTMEM..V2P(data)
//                for the kernel's instructions and r/o data
//   data..KERNBASE+PHYSTOP: mapped to V2P(data)..PHYSTOP, 
//                                  rw data + free physical memory
//   0xfe000000..0: mapped direct (devices such as ioapic)
//
// The kernel allocates physical memory for its heap and for user memory
// between V2P(end) and the end of physical memory (PHYSTOP)
// (directly addressable from end..P2V(PHYSTOP)).

// This table defines the kernel's mappings, which are present in
// every process's page table.

******************************************************************************/



// Load the initcode into address 0 of pgdir.
// sz must be less than a page.
void
inituvm_v2(pagetab_t* tab, char *init, uint sz){
  char *mem;
  
  if(sz >= PGSIZE)
    panic("inituvm: more than a page");
  mem = kalloc();
  memset(mem, 0, PGSIZE);
  mappages_v2(tab, 0, PGSIZE, v2p(mem), PTE_W|PTE_U);
  memmove(mem, init, sz);
}

// Load a program segment into pgdir.  addr must be page-aligned
// and the pages from addr to addr+sz must already be mapped.
int
loaduvm_v2(pagetab_t* tab, char *addr, struct inode *ip, uint offset, uint sz)
{
  uint i, pa, n;
  pte_t *pte;

  if((uintp) addr % PGSIZE != 0)
    panic("loaduvm: addr must be page aligned");
  for(i = 0; i < sz; i += PGSIZE){
    if((pte = walkpgdir_v2(tab, addr+i, 0)) == 0)
      panic("loaduvm: address should exist");
    pa = PTE_ADDR(*pte);
    if(sz - i < PGSIZE)
      n = sz - i;
    else
      n = PGSIZE;
    if(readi(ip, p2v(pa), offset+i, n) != n)
      return -1;
  }
  return 0;
}

// Allocate page tables and physical memory to grow process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
int
allocuvm_v2(pagetab_t *tab, uint oldsz, uint newsz){
  char *mem;
  uintp a;

  if(newsz >= KERNBASE)
    return 0;
  if(newsz < oldsz)
    return oldsz;

  a = PGROUNDUP(oldsz);
  for(; a < newsz; a += PGSIZE){
    mem = kalloc();
    if(mem == 0){
      cprintf("allocuvm out of memory\n");
      deallocuvm_v2(tab, newsz, oldsz);
      return 0;
    }
    memset(mem, 0, PGSIZE);
    mappages_v2(tab, (char*)a, PGSIZE, v2p(mem), PTE_W|PTE_U);
  }
  return newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
int
deallocuvm_v2(pagetab_t* tab, uintp oldsz, uintp newsz){
  pte_t *pte;
  uintp a, pa;

  if(newsz >= oldsz)
    return oldsz;

  a = PGROUNDUP(newsz);
  for(; a  < oldsz; a += PGSIZE){
    pte = walkpgdir_v2(tab, (char*)a, 0);
    if(!pte)
      a += (NPTENTRIES - 1) * PGSIZE;
    else if((*pte & PTE_P) != 0){
      pa = PTE_ADDR(*pte);
      // Free the memory if, and only if, it is non-null
      if(pa != 0){
        char *v = p2v(pa);
        kfree(v);
      }
      *pte = 0;
    }
  }
  return newsz;
//	return deallocuvm(tab->pgdir,oldsz,newsz);
}


// Free a page table and all the physical memory pages
// in the user part.
void
freevm_v2(pagetab_t* tab){
  pde_t *pgdir = tab->pgdir;
  uint i;
  if(pgdir == 0)
    panic("freevm: no pgdir");
  deallocuvm_v2(tab, 0x3fa00000, 0);
  for(i = 0; i < NPDENTRIES-2; i++){
    if(pgdir[i] & PTE_P){
      char * v = p2v(PTE_ADDR(pgdir[i]));
      kfree(v);
    }
  }
  kfree((char*)pgdir);
  kfree(tab);
}

// Clear PTE_U on a page. Used to create an inaccessible
// page beneath the user stack.
void
clearpteu_v2(pagetab_t *tab, char *uva)
{
  pte_t *pte;

  pte = walkpgdir_v2(tab, uva, 0);
  if(pte == 0)
    panic("clearpteu_v2");
  *pte &= ~PTE_U;
}

// Given a parent process's page table, create a copy
// of it for a child.
pagetab_t*
copyuvm_v2(pagetab_t* tab, uint sz){
  pagetab_t *dtab;
  pte_t *pte;
  uintp pa, i, flags;
  char *mem;

  if((dtab = setupkvm_v2()) == 0)
    return 0;
  for(i = 0; i < sz; i += PGSIZE){
    // there is no reason to panic().
    // Implementations should be fault-tolerant.
    if((pte = walkpgdir_v2(tab, (void *) i, 0)) == 0)
      goto bad; // pte should exist
    if(!(*pte & PTE_P))
      continue; // page not present, skip to next
    pa = PTE_ADDR(*pte);
    flags = PTE_FLAGS(*pte);
    if((mem = kalloc()) == 0)
      goto bad;
    memmove(mem, (char*)p2v(pa), PGSIZE);
    if(mappages_v2(dtab, (void*)i, PGSIZE, v2p(mem), flags) < 0)
      goto bad;
  }
  return dtab;

bad:
  freevm_v2(dtab);
  return 0;
}

//PAGEBREAK!
// Map user virtual address to kernel address.
char*
uva2ka_v2(pagetab_t* tab, char* uva){
  pte_t *pte;

  pte = walkpgdir_v2(tab, uva, 0);
  if((*pte & PTE_P) == 0)
    return 0;
  if((*pte & PTE_U) == 0)
    return 0;
  return (char*)p2v(PTE_ADDR(*pte));
}

// Copy len bytes from p to user address va in page table pgdir.
// Most useful when pgdir is not the current page table.
// uva2ka ensures this only works for PTE_U pages.
int
copyout_v2(pagetab_t* tab, uint va, void* p, uint len)
{
  char *buf, *pa0;
  uintp n, va0;

  buf = (char*)p;
  while(len > 0){
    va0 = (uint)PGROUNDDOWN(va);
    pa0 = uva2ka_v2(tab, (char*)va0);
    if(pa0 == 0)
      return -1;
    n = PGSIZE - (va - va0);
    if(n > len)
      n = len;
    memmove(pa0 + (va - va0), buf, n);
    len -= n;
    buf += n;
    va = va0 + PGSIZE;
  }
  return 0;
}

void
flushtlb_all(void){
	// This damn hack is scattered all over the kernel to flush the TLB.
	switchuvm(proc);
}

void
flushtlb_range(uintp beg, uintp end){
	beg &= ~0xfff;
	end &= ~0xfff;
	for(;beg<end;beg+=0x1000)
		invlpg((void*)beg);
}

void
flushtlb_page(uintp pos){
	invlpg((void*)pos);
}

