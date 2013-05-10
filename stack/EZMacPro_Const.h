/*!\file EZMacPro_Const.h
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

#ifndef _EZMACPRO_CONST_H_
#define _EZMACPRO_CONST_H_


                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

//definition for the parameters table
#define START_FREQUENCY_1                   (0)
#define START_FREQUENCY_2                   (1)
#define START_FREQUENCY_3                   (2)
#define STEP_FREQUENCY                      (3)
#define MAX_CHANNEL_NUMBER                  (4)
#define PREAMBLE_IF_ONE_CHANNEL             (5)
#define PREAMBLE_IF_TWO_CHANNEL             (6)
#define PREAMBLE_IF_THREE_CHANNEL           (7)
#define PREAMBLE_IF_FOUR_CHANNEL            (8)

#ifdef FOUR_CHANNEL_IS_USED
    #define SEARCH_TIME                     (9)
    #define PREAMBLE_DETECTION_THRESHOLD    (10)
#endif
#ifdef MORE_CHANNEL_IS_USED
    #define SEARCH_TIME                     (4)
    #define PREAMBLE_DETECTION_THRESHOLD    (9)
#endif

#define CHANNEL_NUMBERS                     (5)
#define PREAMBLE_LENGTH                     (6)
#define PREAMBLE_LENGTH_REG_VALUE1          (7)
#define PREAMBLE_LENGTH_REG_VALUE2          (8)
#define PLL_TUNE_TIME_REG_VALUE             (10)


                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

//Channel parameter table
#ifdef FOUR_CHANNEL_IS_USED
	extern const SEGMENT_VARIABLE (Parameters[4][11], U8, SEG_CODE);
#endif
#ifdef MORE_CHANNEL_IS_USED
	extern const SEGMENT_VARIABLE (Parameters[4][11], U8, SEG_CODE);
#endif

extern const SEGMENT_VARIABLE (EZMacProByteTime[4], U16, SEG_CODE);
extern const SEGMENT_VARIABLE (FrequencyTable[50],U8, SEG_CODE);


#endif //_EZMACPRO_CONST_H_
