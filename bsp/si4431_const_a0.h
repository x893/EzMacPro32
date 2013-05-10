/******************************************************************************
**  si4431_const_a0.h
*******************************************************************************
**  Header File Description:
**      Header of si4431_const_a0.c
**
**  Target:
**      Any Silicon Labs C8051 MCU
**
**  IDE:
**      Silicon Laboratories IDE v4.17
**
**  Tool Chains:
**      Keil
**          c51.exe     v8.16
**          bl51.exe    v6.15
**      Raisonance
**          rc51.exe    v3.08.10
**          lx51.exe    v5.08.10
**      SDCC
**          sdcc.exe    v3.0.0
**          sdld.exe    v2.0.0
**
**  Project Name:
**      EZMacPro stack v2.2.0r
**
**  Release 2.0
**      - Official release of the new EZMacPro stack with Frequency Hopper
**        extension
**  Release 2.1 (07.30.2010)
**      - Support 50 channels in EZHOP in all data rate
**      - Support the Si10xx motherboard, the Si101x and Si100x daughter card
**        platform
**      - Support the Raisonance compiler
**      - Support the 24.5MHz internal oscillator
**      - EZHOP only supports the 915MHz band
**  Release 2.2 (03.09.2011)
**      - Support custom data in ACK payload
**
**  This software must be used in accordance with the End User License
**  Agreement.
**
**  Copyright 2011 Silicon Laboratories, Inc.
**  http://www.silabs.com
******************************************************************************/
#ifndef SI4431_CONST_A0_H
#define SI4431_CONST_A0_H

#define NUMBER_OF_SAMPLE_SETTING		4
#define NUMBER_OF_PARAMETER_A0			14

extern const SEGMENT_VARIABLE (RfSettingsA0[NUMBER_OF_SAMPLE_SETTING][NUMBER_OF_PARAMETER_A0], U8, SEG_CODE);

#endif //SI4431_CONST_A0_H
