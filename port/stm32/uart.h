#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>

#define USART_RX_SIZE	16
#define USART_TX_SIZE	128

#ifdef __CC_ARM
#pragma pack(1)
#endif
typedef struct RingBuffer_s {
	uint8_t GetIndex;
	uint8_t PutIndex;
	uint8_t Size;
	uint8_t Buffer[USART_RX_SIZE];
} RingBuffer_t;
#ifdef __CC_ARM
#pragma pack(8)
#endif

#ifdef __GNUC__
	/*	With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
		set to 'Yes') calls __io_putchar() */
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#ifdef __CC_ARM
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
#endif

uint8_t RingBuffer_Put(RingBuffer_t *rb, uint8_t data);
uint8_t RingBuffer_Get(RingBuffer_t * rb, uint8_t * data);
void Uart0Init(void);

#endif
