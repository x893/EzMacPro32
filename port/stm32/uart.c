#include "bsp.h"

#include <stdio.h>

RingBuffer_t RingBuffer_TX;
RingBuffer_t RingBuffer_RX;

uint8_t RingBuffer_Put(RingBuffer_t *rb, uint8_t data)
{
	uint8_t next = rb->PutIndex + 1;
	if (next == rb->Size)
		next = 0;
	if (rb->GetIndex != next)
	{
		rb->Buffer[rb->PutIndex] = data;
		rb->PutIndex = next;
		return 1;
	}
	return 0;
}

uint8_t RingBuffer_Get(RingBuffer_t *rb, uint8_t * data)
{
	if (rb->GetIndex != rb->PutIndex)
	{
		*data = rb->Buffer[rb->GetIndex];
		rb->GetIndex++;
		if (rb->GetIndex == rb->Size)
			rb->GetIndex = 0;
		return 1;
	}
	else
		*data = 0;
	return 0;
}

void UsartPut(char ch)
{
	if (ch == '\n')
		UsartPut('\r');
	while(!(RingBuffer_Put(&RingBuffer_TX, (uint8_t)ch)));
	USART_ITConfig(USART_PORT, USART_IT_TXE, ENABLE);
}

PUTCHAR_PROTOTYPE
{
	UsartPut(ch);
	return ch;
}

char Usart0Get(void)
{
	char c;
	if (RingBuffer_Get(&RingBuffer_RX, (uint8_t *)&c))
		return c;
	return 0;
}

INTERRUPT(USART_IRQHandler, 0)
{
	uint8_t data;
	if(USART_GetITStatus(USART_PORT, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(USART_PORT);
		if (USART_GetFlagStatus(USART_PORT,	USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE) == SET)
			data = USART_ReceiveData(USART_PORT);
		else
			RingBuffer_Put(&RingBuffer_RX, data);
	}
	if(USART_GetITStatus(USART_PORT, USART_IT_TXE) != RESET)
	{
		if (RingBuffer_Get(&RingBuffer_TX, &data))
			USART_SendData(USART_PORT, data);
		else
			USART_ITConfig(USART_PORT, USART_IT_TXE, DISABLE);
	}
}

void Uart0Init(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	memset(&RingBuffer_TX, 0, sizeof(RingBuffer_TX));
	memset(&RingBuffer_RX, 0, sizeof(RingBuffer_RX));
	RingBuffer_TX.Size = USART_TX_SIZE;
	RingBuffer_RX.Size = USART_RX_SIZE;
	
	USART_CLOCK();
	USART_RX_INIT();
	USART_TX_INIT();

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_InitStructure.USART_BaudRate = UART0_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART_PORT, &USART_InitStructure);
	USART_Cmd(USART_PORT, ENABLE);
	USART_ITConfig(USART_PORT, USART_IT_RXNE, ENABLE);
}
