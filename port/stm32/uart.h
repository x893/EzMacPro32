#ifndef _UART_H_
#define _UART_H_
#define ENABLE_UART0_INTERRUPT()        ES0 = 1
#define SET_UART0_INTERRUPT_FLAG()      TI0 = 1
#define CLEAR_UART0_INTERRUPT_FLAG()    TI0 = 0

void Uart0Init(void);

#endif