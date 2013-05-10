
#ifndef _TIMER_H_
#define _TIMER_H_


/*!
 * Timer macros.
 */
#define TIMER0_LOW_BYTE                 TL0
#define TIMER0_HIGH_BYTE                TH0
#define TIMER3_LOW_BYTE                 TMR3L
#define TIMER3_HIGH_BYTE                TMR3H

#define ENABLE_TIMER0_INTERRUPT()       ET0=1
#define DISABLE_TIMER0_INTERRUPT()      ET0=0
#define CLEAR_TIMER0_INTERRUPT()        TF0=0
#define START_TIMER0()                  TR0=1
#define STOP_TIMER0()                   TR0=0

#define ENABLE_TIMER2_INTERRUPT()       ET2=1
#define DISABLE_TIMER2_INTERRUPT()      ET2=0
#define CLEAR_TIMER2_INTERRUPT()        TMR2CN &= ~0x80
#define START_TIMER2()                  TMR2CN |= 0x04
#define STOP_TIMER2()                   TMR2CN &= ~0x04

#define ENABLE_TIMER3_INTERRUPT()       EIE1 |= 0x80
#define DISABLE_TIMER3_INTERRUPT()      EIE1 &= ~0x80
#define CLEAR_TIMER3_INTERRUPT()        TMR3CN &= ~0x80
#define START_TIMER3()                  TMR3CN |= 0x04
#define STOP_TIMER3()                   TMR3CN &= ~0x04

#define TIMER0_PRESCALER                (12)    /* SYSCLK/4; SYSCLK/12; SYSCLK/48 */
#define TIMER2_PRESCALER                (12)
#define TIMER3_PRESCALER                (12)


#define TIMER_LOW_BYTE                  TIMER3_LOW_BYTE
#define TIMER_HIGH_BYTE                 TIMER3_HIGH_BYTE

#define ENABLE_MAC_TIMER_INTERRUPT()    ENABLE_TIMER3_INTERRUPT()
#define DISABLE_MAC_TIMER_INTERRUPT()   DISABLE_TIMER3_INTERRUPT()
#define CLEAR_MAC_TIMER_INTERRUPT()     CLEAR_TIMER3_INTERRUPT()
#define START_MAC_TIMER()               START_TIMER3()
#define STOP_MAC_TIMER()                STOP_TIMER3()

#define TIMER_PRESCALER                 TIMER3_PRESCALER


#define TIMEOUT_US(n)                   (((U32)(n)*SYSCLK_KHZ)/(TIMER_PRESCALER*1000L))
#define BYTE_TIME(n)                    ((SYSCLK_HZ/n)*8/TIMER_PRESCALER)

//#define DELAY_2MS                       (U16)((SYSCLK_HZ/TIMER2_PRESCALER)/450)
#define DELAY_1MS_TIMER2                (U16)((SYSCLK_HZ/TIMER2_PRESCALER)/900)
#define DELAY_2MS_TIMER2                (U16)((SYSCLK_HZ/TIMER2_PRESCALER)/450)
#define DELAY_5MS_TIMER2                (U16)((SYSCLK_HZ/TIMER2_PRESCALER)/180)
#define DELAY_15MS_TIMER2               (U16)((SYSCLK_HZ/TIMER2_PRESCALER)/60)
#define DELAY_100MS_TIMER0              (U32)((SYSCLK_HZ/TIMER0_PRESCALER)/10)
#define DELAY_1000MS_TIMER0             (U32)((SYSCLK_HZ/TIMER0_PRESCALER))
#define DELAY_3000MS_TIMER0             (U32)((SYSCLK_HZ/TIMER0_PRESCALER)*3)


extern SEGMENT_VARIABLE(EZMacProTimerMSB, U16, EZMAC_PRO_GLOBAL_MSPACE);
extern SEGMENT_VARIABLE(Timer0MSB, U16, EZMAC_PRO_GLOBAL_MSPACE);
extern SEGMENT_VARIABLE(Timer0MSB_save, U16, EZMAC_PRO_GLOBAL_MSPACE);

void Timer0Init(void);
void Timer2Init(void);
void Timer3Init(void);

void Timer0Start(U32 timeout);
void Timer0Stop(void);
//void Timer2BusyWait_2ms(void);
void Timer2BusyWait_ms(U16 timeout);

void macTimeout (U32);
void extIntTimeout (U32);
void timerIntTimeout (U32);



#endif //_TIMER_H_
