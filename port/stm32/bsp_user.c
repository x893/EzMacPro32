#include "bsp.h"

void pinMode ( uint8_t pin, uint8_t mode )
{
	GPIO_TypeDef * port = PIN_PORT(pin);

	uint32_t pin_mask = PIN_MASK_MODE(0xF, pin);
	uint32_t pin_mode = PIN_MASK_MODE(mode, pin);;
	if (pin & 0x08)
		port->CRH = (port->CRH & ~pin_mask) | pin_mode;
	else
		port->CRL = (port->CRL & ~pin_mask) | pin_mode;

	if (mode == MODE_Input_PD)
		port->BRR = (((uint32_t)0x01) << (pin & 0x0F));
	else if (mode == GPIO_Mode_IPU)
		port->BSRR = (((uint32_t)0x01) << (pin & 0x0F));
}

void pinLow  ( uint8_t pin )
{
	GPIO_ResetBits(PIN_PORT(pin), PIN_MASK(pin));
}
void pinHigh ( uint8_t pin )
{
	GPIO_SetBits(PIN_PORT(pin), PIN_MASK(pin));
}


/*!
 * Init board.
 */
void BoardInit(void)
{
	SPI_InitTypeDef   SPI_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	RCC_APB2PeriphClockCmd( 0
		| RCC_APB2Periph_GPIOA
		| RCC_APB2Periph_GPIOB
		| RCC_APB2Periph_AFIO
		, ENABLE);

	DBGMCU_Config( 0
		| DBGMCU_TIM2_STOP
		| DBGMCU_TIM3_STOP
		| DBGMCU_SLEEP
		| DBGMCU_STOP
		| DBGMCU_STANDBY
		, ENABLE);
	MAC_TIMER_CLOCK();
	DELAY_TIMER_CLOCK();

	LED1_OFF();
	LED1_INIT();
	LED2_OFF();
	LED2_INIT();
	
	TimerInit( MAC_TIMER,   MAC_TIMER_PRESCALER,	MAC_TIMER_IRQn );
	TimerInit( DELAY_TIMER, DELAY_TIMER_PRESCALER,	0 );
	
	RF_SDN_HIGH();
	RF_SDN_INIT();
	RF_NSS_HIGH();
	RF_NSS_INIT();
	RF_SCLK_INIT();
	RF_MISO_INIT();
	RF_MOSI_INIT();
	RF_IRQ_INIT();

	DELAY_uS(10 * DELAY_1MS_TIMER2);
	RF_SDN_LOW();
	DELAY_uS(50 * DELAY_1MS_TIMER2);

	RF_SPI_CLOCK();

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(RF_SPI, &SPI_InitStructure);
	SPI_Cmd(RF_SPI, ENABLE);

	GPIO_EXTILineConfig(RF_IRQ_EXT_PORT, RF_IRQ_EXT_PIN);

	EXTI_InitStructure.EXTI_Line = RF_IRQ_EXT_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	DISABLE_MAC_EXT_INTERRUPT();
	CLEAR_MAC_EXT_INTERRUPT();

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = RF_IRQ_EXT_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

#ifdef RTC_ENABLED
    RTC_INIT();
#endif
#ifdef UART0_ENABLED
	Uart0Init();
#endif
}

