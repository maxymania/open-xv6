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
#include <krnl/exec.h>
#include <gnrc/aptable.h>
#include <param.h>
#include <memlayout.h>
#include <mmu.h>
#include <proc.h>
#include <defs.h>
#include <x86.h>
#include <elf.h>

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uintp argc, sz, sp, ustack[3+MAXARG+1];
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pagetab_t *tab, *oldtab;

  if((ip = namei(path)) == 0)
    return -1;
  ilock(ip);
  tab = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) < sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  if((tab = setupkvm_v2()) == 0)
    goto bad;

  // Load program into memory.
  sz = 0;
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if((sz = allocuvm_v2(tab, sz, ph.vaddr + ph.memsz)) == 0)
      goto bad;
    if(loaduvm_v2(tab, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
      goto bad;
  }
  iunlockput(ip);
  ip = 0;

  // Allocate two pages at the next page boundary.
  // Make the first inaccessible.  Use the second as the user stack.
  sz = PGROUNDUP(sz);
  if((sz = allocuvm_v2(tab, sz, sz + 2*PGSIZE)) == 0)
    goto bad;
  clearpteu_v2(tab, (char*)(sz - 2*PGSIZE));
  sp = sz;

  // Push argument strings, prepare rest of stack in ustack.
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp = (sp - (strlen(argv[argc]) + 1)) & ~(sizeof(uintp)-1);
    if(copyout_v2(tab, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[3+argc] = sp;
  }
  ustack[3+argc] = 0;

  ustack[0] = 0xffffffff;  // fake return PC
  ustack[1] = argc;
  ustack[2] = sp - (argc+1)*sizeof(uintp);  // argv pointer

#if X64
  proc->tf->rdi = argc;
  proc->tf->rsi = sp - (argc+1)*sizeof(uintp);
#endif

  sp -= (3+argc+1) * sizeof(uintp);
  if(copyout_v2(tab, sp, ustack, (3+argc+1)*sizeof(uintp)) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(proc->name, last, sizeof(proc->name));

  // Commit to the user image.
  oldtab = proc->pagetable;
  proc->pagetable = tab;

  proc->sz = sz;
  proc->tf->eip = elf.entry;  // main
  proc->tf->esp = sp;
  flushtlb_all();
  freevm_v2(oldtab);
  return 0;

 bad:
  if(tab)
    freevm_v2(tab);
  if(ip)
    iunlockput(ip);
  return -1;
}


