#ifndef  RTC_H
#define  RTC_H
//================================================================================================
// rtc.h
//================================================================================================
// Copyright 2009 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Header File Description:
//
// This file is rtc header file Wireless M-bus stack
//
// Target:
//    Any Silicon Labs C8051 MCU.
//
// IDE:
//    Silicon Laboratories IDE   version 3.3
//
// Tool Chains:
//    Keil
//       c51.exe     version 8.0.8
//       bl51.exe    version 6.0.5
//    SDCC
//       sdcc.exe    version 2.8.0
//       aslink.exe  version 1.75
//
// Project Name:
//    F9xx RTC Example code
//
// Beta Release 0.1
//    - 30 JUN 2009
//
// This software must be used in accordance with the Signed License Agreement.
//
// This software may only be used on a Silicon Labs MCU.
//
//=============================================================================
// Includes
//-----------------------------------------------------------------------------
#ifndef COMPILER_DEFS_H
#include "compiler_defs.h"
#endif
//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
#define RTC0CN          0x04                 // RTC address of RTC0CN register
#define RTC0XCN         0x05                 // RTC address of RTC0XCN register
#define RTC0XCF         0x06                 // RTC address of RTC0XCCF register
//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
#define LOAD_CAP_VALUE  0x0E
#define CAP_AUTO_STEP   0x80
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void  rtcDelay (U8);
void  RTC_Init (void);
U8    RTC_Read (U8);
void  RTC_Write (U8, U8);
void  RTC_ClearCapture (void);
void  RTC_ClearAlarm (void);
U32   RTC_ReadCapture (void);
U32   RTC_ReadAlarm (void);
void  RTC_WriteAlarm (U32);
void  RTC_Reset(void);
U32   RTC_GetCapture(void);
void  RTC_StartCapture (void);
U8    RTC_CapturePending (void);
void  RTC_SetAlarm (U32);
void  RTC_WriteTimeout (U32);
void  RTC_Timeout (U32);
//=============================================================================
#endif  // RTC_H
