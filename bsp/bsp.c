
                /* ======================================= *
                 *              I N C L U D E              *
                 * ======================================= */

#include "bsp.h"

#ifdef SPI_ENABLED
	#include "spi.c"
#endif //SPI_ENABLED
#ifdef TIMER_ENABLED
	#include "timer.c"
#endif //TIMER_ENABLED
#ifdef RTC_ENABLED
  #include "rtc.c"
#endif //RTC_ENABLED
#ifdef UART0_ENABLED
  #include "uart.c"
#endif //UART0_ENABLED

/*!
 * Radio revision support selector.
 * rev. B1 is the latest.
 */
#ifndef B1_ONLY
  #include "si4431_const_a0.c"
  #include "si4432_const_v2.c"
#endif //B1_ONLY
  #include "si443x_const_b1.c"


/*!
 * Development Platform selector.
 */
#ifdef SOFTWARE_DEVELOPMENT_BOARD
  #include "sdbc.c"
#endif //SOFTWARE_DEVELOPMENT_BOARD
#ifdef EZLINK_MODULE
  #include "ezlink.c"
#endif //EZLINK_MODULE
#ifdef SI1000_DAUGHTERCARD_SI1000_MOTHERBOARD
  #include "si1000mb.c"
#endif //SI1000_DAUGHTERCARD_SI1000_MOTHERBOARD
#ifdef SI1010_DAUGHTERCARD_SI1000_MOTHERBOARD
	#include "si1010mb.c"
#endif //SI1010_DAUGHTERCARD_SI1000_MOTHERBOARD
#ifdef BSP_USER
	#include "bsp_user.c"
#endif // USER BOARD


/*!
 * I/O selector.
 */
#ifdef DOG_LCD_ENABLED
  #include "dog_glcd.c"
  #include "graphics.c"
#endif //DOG_LCD_ENABLED
