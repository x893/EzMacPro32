/*!
 * Memory qualifiers.
 */
#define REGISTER_MSPACE                 SEG_IDATA
//#define REGISTER_MSPACE               SEG_DATA
#define BUFFER_MSPACE                   SEG_XDATA
#define FORWARDED_PACKET_TABLE_MSPACE   SEG_XDATA
#define TEST_CODE_MSPACE                SEG_XDATA
#define EZMAC_PRO_GLOBAL_MSPACE         SEG_DATA
#define APPLICATION_MSPACE              SEG_XDATA

/*!
 * Module with STM32.
 */
#ifdef STM32F10X_MD

	#define BSP_USER
	#define STM32_MODULE
	#define SPI_ENABLED
	#define TIMER_ENABLED

#endif //STM32

/*!
 * Software Development Board.
 */
#ifdef SDBC
	#define MCU_F930
	#define SOFTWARE_DEVELOPMENT_BOARD
	#define SPI_ENABLED
	#define TIMER_ENABLED
	#define DOG_LCD_ENABLED
#endif //SDBC


/*!
 * EZLink module.
 */
#ifdef EZLINK
	#define MCU_F930
	#define EZLINK_MODULE
	#define SPI_ENABLED
	#define TIMER_ENABLED
#endif //SDBC


/*!
 * Si1000 on an Si1000MB module.
 */
#ifdef SI1000MB_SI1000
	#define MCM_SI1000
	#define SI1000_DAUGHTERCARD_SI1000_MOTHERBOARD
	#define SPI_ENABLED
	#define TIMER_ENABLED
#endif //SDBC


/*!
 * Si1010 on an Si1000MB module.
 */
#ifdef SI1000MB_SI1010
	#define MCM_SI1010
	#define SI1010_DAUGHTERCARD_SI1000_MOTHERBOARD
	#define SPI_ENABLED
	#define TIMER_ENABLED
#endif //SDBC


/*!
 * Enable trace through UART.
 */
#ifdef TRACE_ENABLED
	#define UART0_ENABLED
#endif //TRACE_ENABLED


/*!
 * Radio chip revision.
 */
#define B1_ONLY

#include "compiler_defs.h"
#include "bsp\bsp.h"
#include "stack\stack.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
