/*!\file EZMacPro_ExternalInt.h
 * \brief Header of externalInt.c.
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

#ifndef _EZMACPRO_EXTERNALINT_H_
#define _EZMACPRO_EXTERNALINT_H_


                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

                /* ======================================= *
                 *  F U N C T I O N   P R O T O T Y P E S  *
                 * ======================================= */

void extIntDisableInterrupts (void);
void extIntNextRX_Channel (void);
#ifdef FOUR_CHANNEL_IS_USED
void extIntIncrementError (U8 );
#endif//FOUR_CHANNEL_IS_USED
#ifdef PACKET_FORWARDING_SUPPORTED
U8 extIntPacketNeedsForwarding (void);
#endif //PACKET_FORWARDING_SUPPORTED
U8 extIntBadAddrError (void);
void extIntGotoNextStateUsingSECR ( U8 );
void extIntTX_StateMachine (U8, U8, U8);
void extIntRX_StateMachine (U8, U8, U8);
void extIntWakeUp (U8);
void extIntDisableInterrupts (void);
void extIntSetEnable2 (U8);
void extIntSetFunction1 (U8);
U8 extIntHeaderError(void);
U8 extIntBadAddrError(void);





#endif //_EZMACPRO_EXTERNALINT_H_
