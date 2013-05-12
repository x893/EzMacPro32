#include "bsp.h"

SEGMENT_VARIABLE(EZMacProTimerMSB, U16, EZMAC_PRO_GLOBAL_MSPACE);

/*!
 * Initialise Timer.
 */
void TimerInit(TIM_TypeDef* TIMx, uint16_t prescaler, uint8_t irq)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	TIM_Cmd(TIMx, DISABLE);

	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
#ifdef STM32F10X_MD
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
#endif
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);

	TIM_SelectOnePulseMode(TIMx, TIM_OPMode_Single);

	if (irq != 0)
	{
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		NVIC_InitStructure.NVIC_IRQChannel = irq;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure); 
	}
}

/*!
 * This function is used to busy wait for timeout msec.
 * Note: longest wait interval is ~32 msec.
 */
void Timer2BusyWait_ms(U16 timeout)
{
	CLEAR_DELAY_COMPLETE();
	SET_DELAY_TIMER_COUNT(65535 - timeout);
	START_DELAY_TIMER();
	WAIT_DELAY_COMPLETE();
	STOP_DELAY_TIMER();
}

//================================================================================================
//
// Timer Functions for externalInt.c module
//
//================================================================================================
//------------------------------------------------------------------------------------------------
// Function Name
//    extIntTimeout()
//
// Return Value : None
// Parameters   : U32 longTime
//
// Notes:
//
// This function is called when a interrupt event must initiate a timeout event.
// A 32-bit union is used to provide word and byte access. The upper word is stored in
// EZMacProTimerMSB. The The lower word is first negated then written to the TL0 and TH0 sfrs.
//
//-----------------------------------------------------------------------------------------------
void extIntTimeout (U32 longTime)
{
	UU32 time;

	DISABLE_MAC_TIMER_INTERRUPT();
	STOP_MAC_TIMER();
	CLEAR_MAC_TIMER_INTERRUPT();

	time.U32 = longTime;
	EZMacProTimerMSB = time.U16[MSB];
	SET_MAC_TIMER_COUNT( - time.U16[LSB]);

	START_MAC_TIMER();
}

//================================================================================================
//
// Timer Functions for timerInt.c module
//
//================================================================================================
//------------------------------------------------------------------------------------------------
// Function Name
//    timerIntTimeout()
//
// Return Value : None
// Parameters   : U32 longTime
//
// Notes:
//
// This function is called when a timeout event must initiate a subsequent timeout event.
// A 32-bit union is used to provide word and byte access. The upper word is stored in
// EZMacProTimerMSB. The The lower word is first negated then written to the TL0 and TH0 sfrs.
//
// This function is not included for the Transmitter only configuration.
//
//-----------------------------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY_OPERATION
void timerIntTimeout (U32 longTime) __attribute__((alias("extIntTimeout")));
#endif

//================================================================================================
// Timer Functions for EZMacPro.c module
//
// Parameters   : U32 longTime
// Notes:
// This function is called when a interrupt event must initiate a timeout event.
// A 32-bit union is used to provide word and byte access. The upper word is stored in
// EZMacProTimerMSB. The The lower word is first negated then written to the TL0 and TH0 sfrs.
//================================================================================================
void macTimeout (U32 longTime)
{
	U8 restoreInts = GET_MAC_EXT_INTERRUPT();
	DISABLE_MAC_INTERRUPTS();
	extIntTimeout(longTime);
	SET_MAC_EXT_INTERRUPT(restoreInts);
}
//================================================================================================
