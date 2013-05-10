

#include "bsp.h"



SEGMENT_VARIABLE(EZMacProTimerMSB, U16, EZMAC_PRO_GLOBAL_MSPACE);
SEGMENT_VARIABLE(Timer0MSB, U16, EZMAC_PRO_GLOBAL_MSPACE);
SEGMENT_VARIABLE(Timer0MSB_save, U16, EZMAC_PRO_GLOBAL_MSPACE);


/*!
 * Initialise Timer0.
 */
void Timer0Init(void)
{
    CKCON &= ~0x04;                 // clear T0M bit
#if (TIMER0_PRESCALER == 4)         // SYSCLK/4
    CKCON &= ~0x03;
    CKCON |= 0x01;
#elif (TIMER0_PRESCALER == 12)      // SYSCLK/12
    CKCON &= ~0x03;
#elif (TIMER0_PRESCALER == 48)      // SYSCLK/48
    CKCON &= ~0x03;
    CKCON |= 0x10;
#else
   #error "Wrong TIMER0_PRESCALER value!"
#endif //TIMER0_PRESCALER
    TMOD &= ~0x0f;
    TMOD |=  0x01;                  // set TMOD for 16 bit timer
}


/*!
 * Initialise Timer2.
 */
void Timer2Init(void)
{
#if (TIMER2_PRESCALER == 1)         // SYSCLK
    CKCON |= 0x30;
#elif (TIMER2_PRESCALER == 12)      // SYSCLK/12
    CKCON &= ~0x30;
    TMR2CN &= ~0x01;
#endif //TIMER2_PRESCALER
    TMR2RL = 98;                    // Reload value to be used in Timer2
    TMR2 = TMR2RL;                  // Init the Timer2 register
    TMR2CN = 0x04;                  // Start Timer2 in auto-reload mode
}


/*!
 * Initialise Timer3.
 */
void Timer3Init(void)
{
    CKCON &= ~0xC0;                 // Timer3 uses clock defined in TMR3CN
    TMR3CN &= ~0x01;                // SYSCLK/12
    TMR3RL = 98;                    // Reload value to be used in Timer3
    TMR3 = TMR3RL;                  // Init the Timer3 register
    TMR3CN = 0x04;                  // Start Timer3 in auto-reload mode
}


/*!
 * This function is used to start the 16-bit Timer0 with a timeout parameter.
 */
void Timer0Start(U32 timeout)
{
    UU32 time;

    /* Disable Timer0 INT. */
    DISABLE_TIMER0_INTERRUPT();
    /* Stop Timer0. */
    STOP_TIMER0();

    time.U32 = timeout;
    Timer0MSB = time.U16[MSB];
    Timer0MSB_save = time.U16[MSB];
    time.U16[LSB] = - time.U16[LSB];

    /* Write LSB first. */
    TL0 = time.U8[b0];
    /* Write MSB last. */
    TH0 = time.U8[b1];

    /* Clear Timer0 INT. */
    CLEAR_TIMER0_INTERRUPT();
    /* Start Timer0. */
    START_TIMER0();
    /* Enable Timer0 INT. */
    ENABLE_TIMER0_INTERRUPT();
}


/*!
 * This function is used to stop the 16-bit Timer0.
 */
void Timer0Stop(void)
{
    /* Stop Timer0. */
    STOP_TIMER0();
    /* Disable Timer0 INT. */
    DISABLE_TIMER0_INTERRUPT();
    /* Clear Timer0 INT. */
    CLEAR_TIMER0_INTERRUPT();
}


/*!
 * This function is used to busy wait for timeout msec.
 * Note: longest wait interval is ~32 msec.
 */
void Timer2BusyWait_ms(U16 timeout)
{
    UU16 Delay;

    Delay.U16 = 65535 - timeout;

    /* Set up Timer2. */
    TMR2CN = 0x00;              //Timer2 runs at SYSCLK/12
    CKCON &= ~0x30;             //Timer2 uses clock defined in TMR2CN
    /* Set time period. */
    TMR2H = Delay.U8[MSB];
    TMR2L = Delay.U8[LSB];
    /* Disable Timer2 interrupt. */
    ET2 = 0;
    /* Enable TR2. */
    TMR2CN |= 0x04;
    /* Busy wait for overflow. */
    while((TMR2CN & 0x80) == 0);
    /* Disable TR2. */
    TMR2CN &= ~0x04;
}


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
   UU32 time;
   U8 restoreInts;

   // Disable MAC interrupts
   restoreInts = EX0;                  // save EX0 state
   EX0 = 0;                            // clear EX0

   DISABLE_MAC_TIMER_INTERRUPT();       // Disable Timer interrupt
   STOP_MAC_TIMER();                    // Stop Timer

   time.U32 = longTime;

   EZMacProTimerMSB = time.U16[MSB];

   time.U16[LSB] = - time.U16[LSB];

   TIMER_LOW_BYTE = time.U8[b0];        // write LSB first
   TIMER_HIGH_BYTE = time.U8[b1];       // write MSB last

   CLEAR_MAC_TIMER_INTERRUPT();         // Clear Timer interrupt
   START_MAC_TIMER();                   // Start Timer

   IE |= restoreInts;                  // restore EX0
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

   DISABLE_MAC_TIMER_INTERRUPT();       // Disable Timer interrupt
   STOP_MAC_TIMER();                    // Stop Timer

   time.U32 = longTime;

   EZMacProTimerMSB = time.U16[MSB];

   time.U16[LSB] = - time.U16[LSB];

   TIMER_LOW_BYTE = time.U8[b0];        // write LSB first
   TIMER_HIGH_BYTE = time.U8[b1];       // write MSB last

   CLEAR_MAC_TIMER_INTERRUPT();         // Clear Timer interrupt
   START_MAC_TIMER();                   // Start Timer

}
//================================================================================================



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
void timerIntTimeout (U32 longTime)
{
   UU32 time;

   DISABLE_MAC_TIMER_INTERRUPT();       // Disable Timer interrupt
   STOP_MAC_TIMER();                    // Stop Timer

   time.U32 = longTime;

   EZMacProTimerMSB = time.U16[MSB];

   time.U16[LSB] = - time.U16[LSB];

   TIMER_LOW_BYTE = time.U8[b0];        // write LSB first
   TIMER_HIGH_BYTE = time.U8[b1];       // write MSB last

   CLEAR_MAC_TIMER_INTERRUPT();         // Clear Timer interrupt
   START_MAC_TIMER();                   // Start Timer
}
#endif//TRANSMITTER_ONLY_OPERATION
//================================================================================================
