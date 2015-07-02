#ifndef krnl_ioapic_h
#define krnl_ioapic_h

// ioapic.c
void            ioapicenable(int irq, int cpu);
extern uchar    ioapicid;
void            ioapicinit(void);

#endif

