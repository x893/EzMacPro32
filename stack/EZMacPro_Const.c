/*!\file EZMacPro_Const.c
 * \brief EZMacPRO stack constants.
 *
 * \n EZMacPRO version: 3.0.1r
 *
 * \n This software must be used in accordance with the End User License
 * \n Agreement.
 *
 * \b COPYRIGHT
 * \n Copyright 2012 Silicon Laboratories, Inc.
 * \n http://www.silabs.com
 */

/* ======================================= *
 *				I N C L U D E				*
 * ======================================= */

#include "stack.h"

/* ======================================= *
 *			D E F I N I T I O N S			*
 * ======================================= */

/* ======================================= *
 *			C O N S T A N T S			*
 * ======================================= */

#ifdef FOUR_CHANNEL_IS_USED

	#ifdef ANTENNA_DIVERSITY_ENABLED
		#ifdef FREQUENCY_BAND_434
			const SEGMENT_VARIABLE (Parameters[4][11], U8, SEG_CODE) =
			{
				//	FR_S1,	FR_S2,	FR_S3, 	FR_ST,	MAX_CH,	PR1,PR2,PR3,PR4,ST,	PRDT
				{	0x53,	0x53,	0x40,	33,		4,		8,	10,	13,	16,	3,	5},	//DR: 2.4kbps, DEV: +-38.4kHz, BBBW: 85.1kHz
				{	0x53,	0x53,	0x40,	33,		4,		8,	10,	13,	16,	3,	5},	//DR: 9.6kbps, DEV: +-38.4kHz, BBBW: 95.3kHz
				{	0x53,	0x5A,	0x40,	36,		2,		8,	14,	19,	24,	5,	5},	//DR: 50kbps, DEV: +-25kHz, BBBW: 112.1kHz
				{	0x53,	0x61,	0xC0,	0,		1,		8,	18,	25,	32,	7,	5}	//DR: 128kbps, DEV: +-64kHz, BBBW: 269.3kHz
			};
		#elif defined FREQUENCY_BAND_868
			const SEGMENT_VARIABLE (Parameters[4][11], U8, SEG_CODE) =
			{
				//	FR_S1,	FR_S2,	FR_S3,	FR_ST,	MAX_CH,	PR1,PR2,PR3,PR4,ST,	PRDT
				{	0x73,	0x2C,	0x60,	45,		14,		8,	10,	13,	16,	3,	5},	//DR: 2.4kbps, DEV: +-38.4kHz, BBBW: 95.3kHz
				{	0x73,	0x2C,	0x60,	45,		14,		8,	10,	13,	16,	3,	5},	//DR: 9.6kbps, DEV: +-38.4kHz, BBBW: 102.2kHz
				{	0x73,	0x2E,	0xE0,	78,		8,		8,	14,	19,	24,	5,	5},	//DR: 50kbps, DEV: +-25kHz, BBBW: 112.8kHz
				{	0x73,	0x32,	0x00,	100,	6,		8,	18,	25,	32,	7,	5}	//DR: 128kbps, DEV: +-64kHz, BBBW: 269.3kHz
			};
		#elif defined FREQUENCY_BAND_915
			const SEGMENT_VARIABLE (Parameters[4][11], U8, SEG_CODE) =
			{
				//	FR_S1,	FR_S2,	FR_S3,	FR_ST,	MAX_CH,	PR1,PR2,PR3,PR4,ST,	PDTR
				{	0x75,	0x25,	0x80,	48,		60,		8,	10,	13,	16,	3,	5},	//DR: 2.4kbps, DEV: +-38.4kHz, BBBW: 95.3kHz
				{	0x75,	0x25,	0x80,	48,		60,		8,	10,	13,	16,	3,	5},	//DR: 9.6kbps, DEV: +-38.4kHz, BBBW: 102.2kHz
				{	0x75,	0x25,	0x80,	78,		37,		8,	14,	19,	24,	5,	5},	//DR: 50kbps, DEV: +-25kHz, BBBW: 112.8kHz
				{	0x75,	0x25,	0x80,	100,	29,		8,	18,	25,	32,	7,	5}	//DR: 128kbps, DEV: +-64kHz, BBBW: 269.3kHz
			};
		#endif
	#else	// ANTENNA_DIVERSITY_ENABLED
		#ifdef FREQUENCY_BAND_434
			const SEGMENT_VARIABLE (Parameters[4][11], U8, SEG_CODE) =
			{
				//FR_S1,	FR_S2,	FR_S3, 	FR_ST,	MAX_CH,	PR1,	PR2,	PR3,	PR4,	ST,	PDTR
				{	0x53, 0x53, 0x40, 33, 4, 4, 10, 13, 16, 3, 5}, //DR: 2.4kbps, DEV: +-38.4kHz, BBBW: 85.1kHz
				{	0x53, 0x53, 0x40, 33, 4, 4, 10, 13, 16, 3, 5}, //DR: 9.6kbps, DEV: +-38.4kHz, BBBW: 95.3kHz
				{	0x53, 0x5A, 0x40, 36, 2, 4, 14, 19, 24, 5, 5}, //DR: 50kbps, DEV: +-25kHz, BBBW: 112.1kHz
				{	0x53, 0x61, 0xC0, 0, 1, 4, 18, 25, 32, 7, 5} //DR: 128kbps, DEV: +-64kHz, BBBW: 269.3kHz
			};
		#elif defined FREQUENCY_BAND_868
			const SEGMENT_VARIABLE (Parameters[4][11], U8, SEG_CODE) =
			{
				//FR_S1	FR_S2,	FR_S3,	FR_ST,		MAX_CH,	PR1,	PR2,	PR3,	PR4, 	ST,	PDTR
				{	0x73,	0x2C, 0x60, 45, 14, 4, 10, 13, 16, 3, 5}, //DR: 2.4kbps, DEV: +-38.4kHz, BBBW: 95.3kHz
				{	0x73,	0x2C, 0x60, 45, 14, 4, 10, 13, 16, 3, 5}, //DR: 9.6kbps, DEV: +-38.4kHz, BBBW: 102.2kHz
				{	0x73,	0x2E, 0xE0, 78, 8, 4, 14, 19, 24, 5, 5}, //DR: 50kbps, DEV: +-25kHz, BBBW: 112.8kHz
				{	0x73,	0x32, 0x00, 100, 6, 4, 18, 25, 32, 7, 5} //DR: 128kbps, DEV: +-64kHz, BBBW: 269.3kHz
			};
		#elif defined FREQUENCY_BAND_915
			const SEGMENT_VARIABLE( Parameters[4][11], U8, SEG_CODE) =
			{
				//FR_S1,	FR_S2,	FR_S3,	FR_ST,	MAX_CH, PR1,	PR2,	PR3,	PR4,	ST,	PDTR
				{	0x75,	0x25,	0x80,	48,	 60,	 4,		10,	 13,	 16,	 3,	5 },	//DR: 2.4kbps, DEV: +-38.4kHz, BBBW: 95.3kHz
				{	0x75,	0x25,	0x80,	48,	 60,	 4,		10,	 13,	 16,	 3,	5 },	//DR: 9.6kbps, DEV: +-38.4kHz, BBBW: 102.2kHz
				{	0x75,	0x25,	0x80,	78,	 37,	 4,		14,	 19,	 24,	 5,	5 },	//DR: 50kbps, DEV: +-25kHz, BBBW: 112.8kHz
				{	0x75,	0x25,	0x80,	100,	29,	 4,		18,	 25,	 32,	 7,	5 }	 //DR: 128kbps, DEV: +-64kHz, BBBW: 269.3kHz
			};
		#endif
	#endif //ANTENNA_DIVERSITY_ENABLED
#endif //FOUR_CHANNEL_IS_USED
			
#ifdef MORE_CHANNEL_IS_USED
	#ifdef FREQUENCY_BAND_434
		const SEGMENT_VARIABLE (Parameters[4][11], U8, SEG_CODE) =
		{
			//FR_S1,	FR_S2,	FR_S3,	FR_ST,	ST,	CHNBR,	PREAL,	PREARV1,	PREAV2, 	PRDT	PLLT
			{	0x53, 0x53, 0x40, 33, 24, 50, 160, 0x01, 0x40, 4, 0x52}, //DR: 2.4kbps, DEV: +-38.4kHz, BBBW: 85.1kHz
			{	0x53, 0x53, 0x40, 33, 26, 50, 172, 0x01, 0x58, 4, 0x52}, //DR: 9.6kbps, DEV: +-38.4kHz, BBBW: 95.3kHz
			{	0x53, 0x5A, 0x40, 36, 34, 50, 225, 0x01, 0xC2, 3, 0x52}, //DR: 50kbps, DEV: +-25kHz, BBBW: 112.1kHz
			{	0x53, 0x61, 0xC0, 0, 37, 50, 245, 0x01, 0xEA, 3, 0x08}, //DR: 128kbps, DEV: +-64kHz, BBBW: 269.3kHz
		};
	#elif defined FREQUENCY_BAND_868
		const SEGMENT_VARIABLE (Parameters[4][11], U8, SEG_CODE) =
		{
			//FR_S1 	FR_S2,	FR_S3,	FR_ST,	ST,	CHNBR,	PREAL,	PREARV1	PREARV2	PRDT	PLLT
			{	0x73, 0x2C, 0x60, 45, 24, 50, 160, 0x01, 0x40, 4, 0x52}, //DR: 2.4kbps, DEV: +-38.4kHz, BBBW: 95.3kHz
			{	0x73, 0x2C, 0x60, 45, 26, 50, 172, 0x01, 0x58, 4, 0x52}, //DR: 9.6kbps, DEV: +-38.4kHz, BBBW: 102.2kHz
			{	0x73, 0x2E, 0xE0, 78, 34, 50, 225, 0x01, 0xC2, 3, 0x52}, //DR: 50kbps, DEV: +-25kHz, BBBW: 112.8kHz
			{	0x73, 0x32, 0x00, 100, 37, 50, 245, 0x01, 0xEA, 3, 0x08} //DR: 128kbps, DEV: +-64kHz, BBBW: 269.3kHz
		};
	#elif defined FREQUENCY_BAND_915
		const SEGMENT_VARIABLE (Parameters[4][11], U8, SEG_CODE) =
		{
			//	FR_S1	FR_S2	FR_S3	FR_ST	ST	CHNBR	PREAL	PREARV1	PREARV2	PRDT	PLLT
			{	0x75,	0x25,	0x80,	48,		24,	50,		160,	0x01,	0x40,	4,		0x52	},	//DR: 2.4kbps, DEV: +-38.4kHz, BBBW: 95.3kHz
			{	0x75,	0x25,	0x80,	48,		26,	50,		172,	0x01,	0x58,	4,		0x52	},	//DR: 9.6kbps, DEV: +-38.4kHz, BBBW: 102.2kHz
			{	0x75,	0x25,	0x80,	48,		34,	50,		225,	0x01,	0xC2,	3,		0x52	},	//DR: 50kbps,	DEV: +-25kHz, BBBW: 112.8kHz
			{	0x75,	0x25,	0x80,	48,		37,	50,		245,	0x01,	0xEA,	3,		0x08	}	//DR: 128kbps, DEV: +-64kHz, BBBW: 269.3kHz
		};
	#endif
#endif //MORE_CHANNEL_IS_USED

const SEGMENT_VARIABLE( FrequencyTable[50], U8, SEG_CODE) =
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
	21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
	40, 41, 42, 43, 44, 45, 46, 47, 48, 49
};

const SEGMENT_VARIABLE( EZMacProByteTime[4], U16, SEG_CODE) =
{
	BYTE_TIME(2400), BYTE_TIME(9600), BYTE_TIME(50000), BYTE_TIME(128000L)
};
