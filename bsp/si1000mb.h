/*
** ============================================================================
**
** FILE
**  control_IO.h
**
** DESCRIPTION
**  Header file for IO control routines
**
** CREATED
**  Silicon Laboratories Hungary Ltd
**
** COPYRIGHT
**  Copyright 2009 Silicon Laboratories, Inc.
**  http://www.silabs.com
**
** ============================================================================
*/
#ifndef DKMB_H
#define DKMB_H

                /* ======================================= *
                 *  F U N C T I O N   P R O T O T Y P E S  *
                 * ======================================= */

void BoardInit(void);
void PortInit(void);
void LEDInit(void);
void InterruptInit(void);
void SystemClkInit(void);

#endif
