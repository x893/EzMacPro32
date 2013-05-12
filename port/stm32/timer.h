#ifndef _TIMER_H_
#define _TIMER_H_

/*!
 * Timer macros.
 */

#define START_DELAY_TIMER()				DELAY_TIMER->CR1 |= TIM_CR1_CEN
#define START_MAC_TIMER()				MAC_TIMER->CR1   |= TIM_CR1_CEN

#define STOP_DELAY_TIMER()				DELAY_TIMER->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN))
#define STOP_MAC_TIMER()				MAC_TIMER->CR1   &= (uint16_t)(~((uint16_t)TIM_CR1_CEN))

#define WAIT_DELAY_COMPLETE()			while (!(DELAY_TIMER->SR & TIM_FLAG_Update))
	
#define CLEAR_DELAY_COMPLETE()			DELAY_TIMER->SR = (uint16_t)~TIM_FLAG_Update

#define SET_DELAY_TIMER_COUNT(count)	DELAY_TIMER->CNT = count
#define SET_MAC_TIMER_COUNT(count)		MAC_TIMER->CNT   = count

#define DELAY_uS(delay)					Timer2BusyWait_ms(delay)

#define ENABLE_MAC_TIMER_INTERRUPT()		MAC_TIMER->DIER |=  TIM_IT_Update
#define CLEAR_MAC_TIMER_INTERRUPT()			MAC_TIMER->SR = (uint16_t)~TIM_IT_Update
#define GET_MAC_TIMER_INTERRUPT()			((MAC_TIMER->DIER &  TIM_IT_Update) ? 1 : 0)
#define DISABLE_MAC_TIMER_INTERRUPT_INT()	\
	do {									\
		MAC_TIMER->DIER &= ~TIM_IT_Update;	\
		CLEAR_MAC_TIMER_INTERRUPT();		\
	} while (0)
#define DISABLE_MAC_TIMER_INTERRUPT()		\
	do {									\
		DISABLE_GLOBAL_INTERRUPTS();		\
		DISABLE_MAC_TIMER_INTERRUPT_INT();	\
		ENABLE_GLOBAL_INTERRUPTS();			\
	} while (0)
#define SET_MAC_TIMER_INTERRUPT(enable)		\
	do {									\
		if (enable)	{						\
			ENABLE_MAC_TIMER_INTERRUPT();	\
		}									\
	} while (0)

#define TIMEOUT_US(n)                   ((U32)(n) * (SYSCLK_HZ / MAC_TIMER_PRESCALER / 1000000L))
// n = transmission speed
#define BYTE_TIME(n)                    ((SYSCLK_HZ / n) * 20 / MAC_TIMER_PRESCALER)

#define DELAY_1MS_TIMER2                (U16)((SYSCLK_HZ / DELAY_TIMER_PRESCALER) / 1012)
#define DELAY_2MS_TIMER2                (U16)((SYSCLK_HZ / DELAY_TIMER_PRESCALER) /  506)
#define DELAY_5MS_TIMER2                (U16)((SYSCLK_HZ / DELAY_TIMER_PRESCALER) /  202)
#define DELAY_15MS_TIMER2               (U16)((SYSCLK_HZ / DELAY_TIMER_PRESCALER) /   67)

extern SEGMENT_VARIABLE(EZMacProTimerMSB, U16, EZMAC_PRO_GLOBAL_MSPACE);

void TimerInit(TIM_TypeDef* TIMx, uint16_t prescaler, uint8_t irq);
void TimersInit(void);

void Timer2BusyWait_ms(U16 timeout);

void macTimeout (U32);
void extIntTimeout (U32);
void timerIntTimeout (U32);

#endif //_TIMER_H_
