#ifndef krnl_uart_h
#define krnl_uart_h

// uart.c
void		uartearlyinit(void);
void            uartinit(void);
void            uartintr(void);
void            uartputc(int);

#endif

