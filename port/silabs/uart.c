

#include "bsp.h"



/*!
 * Initialise UART using Timer1.
 */
void Uart0Init(void)
{
   /* Init ports for UART0. */
   XBR0         |= 0x01;                // Enable UART              (P0.4-0.5)
   P0MDOUT      |= 0x10;                // UART TX Push-Pull
   P0SKIP       &= ~0x30;               // Don't skip P0.4-0.5

   /* UART0 Configuration: 8-bit variable bit rate, level of STOP bit is
    * ignored, RX enabled, ninth bits are zeros, clear RI0 and TI0 bits. */
   SCON0        = 0x10;
 #if (SYSCLK_HZ/UART0_BAUDRATE/2/256 < 1)
   TH1          = (U8)(-(SYSCLK_HZ/UART0_BAUDRATE/2));
   CKCON        &= ~0x0B;                  // T1M = 1; SCA1:0 = xx
   CKCON        |=  0x08;
 #elif (SYSCLK_HZ/UART0_BAUDRATE/2/256 < 4)
   TH1          = (U8)(-(SYSCLK_HZ/UART0_BAUDRATE/2/4));
   CKCON        &= ~0x0B;                  // T1M = 0; SCA1:0 = 01
   CKCON        |=  0x01;
 #elif (SYSCLK_HZ/UART0_BAUDRATE/2/256 < 12)
   TH1          = (U8)(-(SYSCLK_HZ/UART0_BAUDRATE/2/12));
   CKCON        &= ~0x0B;                  // T1M = 0; SCA1:0 = 00
 #else
   TH1          = (U8)(-(SYSCLK_HZ/UART0_BAUDRATE/2/48));
   CKCON        &= ~0x0B;                  // T1M = 0; SCA1:0 = 10
   CKCON        |=  0x02;
 #endif

   /* Timer1 init for UART0. */
   TL1          = TH1;
   TMOD         &= ~0xf0;
   TMOD         |=  0x20;               // TMOD: Timer1 in 8-bit auto-reload
   TR1          = 1;                    // START Timer1
//    IP          |= 0x10;                // Make UART high priority
//   ES0          = 1;                            // Enable UART0 interrupts
   TI0          = 1;                            // Indicate TX0 ready
}





