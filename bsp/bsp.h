/*!\file bsp.h
 * \brief Board Support Package global header file.
 *
 * \b COPYRIGHT
 * \n Copyright 2012 Silicon Laboratories, Inc.
 * \n http://www.silabs.com
 */

#ifndef _BSP_H_
#define _BSP_H_


                /* ======================================= *
                 *              I N C L U D E              *
                 * ======================================= */

#include "..\common.h"

/*!
 * MCU/MCM selector. Currently only C8051F93x supported.
 */
#ifdef MCU_F930
	#include "C8051F930_defs.h"
#endif //MCU_F930
#ifdef MCM_SI1000
	#include "Si1000_defs.h"
#endif //MCM_SI1000
#ifdef MCM_SI1010
	#include "Si1010_defs.h"
#endif //MCM_SI1010

#include "hardware_defs.h"

#ifdef SPI_ENABLED
	#include "spi.h"
#endif //SPI_ENABLED
#ifdef TIMER_ENABLED
	#include "timer.h"
#endif //TIMER_ENABLED
#ifdef RTC_ENABLED
	#include "rtc.h"
#endif //RTC_ENABLED
#ifdef UART0_ENABLED
	#include "uart.h"
#endif //UART0_ENABLED


/*!
 * Radio selector. Si403x transmitters, Si433x receivers and Si443x transceivers
 * are supported.
 */
#include "si4432_v2.h"

#ifndef B1_ONLY
	#include "si4432_const_v2.h"
	#include "si4431_const_a0.h"
#endif //B1_ONLY

#include "si443x_const_b1.h"


/*!
 * Development Platform selector.
 */
#ifdef SOFTWARE_DEVELOPMENT_BOARD
	#include "dog_glcd.h"
	#include "graphics.h"
	#include "sdbc.h"
#endif //SOFTWARE_DEVELOPMENT_BOARD
#ifdef EZLINK_MODULE
	#include "ezlink.h"
#endif //EZLINK_MODULE
#ifdef SI1000_DAUGHTERCARD_SI1000_MOTHERBOARD
	#include "si1000mb.h"
#endif //SI1000_DAUGHTERCARD_SI1000_MOTHERBOARD
#ifdef SI1010_DAUGHTERCARD_SI1000_MOTHERBOARD
	#include "si1000mb.h"
#endif //SI1010_DAUGHTERCARD_SI1000_MOTHERBOARD

void BoardInit(void);

#endif //_BSP_H_
