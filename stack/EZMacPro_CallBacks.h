/*!\file EZMacPro_Callbacks.h
 * \brief Header of EZMacProCallBacks.c.
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

#ifndef _EZMACPRO_CALLBACKS_H_
#define _EZMACPRO_CALLBACKS_H_


                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

extern BIT fEZMacPRO_StateWakeUpEntered;
extern BIT fEZMacPRO_StateSleepEntered;
extern BIT fEZMacPRO_StateIdleEntered;
extern BIT fEZMacPRO_StateRxEntered;
extern BIT fEZMacPRO_StateTxEntered;
extern BIT fEZMacPRO_StateErrorEntered;
extern BIT fEZMacPRO_LFTimerExpired;
extern BIT fEZMacPRO_LowBattery;
extern BIT fEZMacPRO_SyncWordReceived;
extern BIT fEZMacPRO_CRCError;
extern BIT fEZMacPRO_PacketDiscarded;
extern BIT fEZMacPRO_PacketReceived;
extern BIT fEZMacPRO_PacketForwarding;
extern BIT fEZMacPRO_PacketSent;
extern BIT fEZMacPRO_LBTTimeout;
extern BIT fEZMacPRO_AckTimeout;
extern BIT fEZMacPRO_AckSending;


                /* ======================================= *
                 *  F U N C T I O N   P R O T O T Y P E S  *
                 * ======================================= */

void EZMacPRO_StateWakeUpEntered(void);
void EZMacPRO_StateSleepEntered(void);
void EZMacPRO_StateIdleEntered(void);
void EZMacPRO_StateRxEntered(void);
void EZMacPRO_StateTxEntered(void);
void EZMacPRO_StateErrorEntered(void);
void EZMacPRO_LFTimerExpired(void);
void EZMacPRO_LowBattery(void);
void EZMacPRO_SyncWordReceived(void);
void EZMacPRO_PacketDiscarded(void);
void EZMacPRO_PacketReceived(U8);
void EZMacPRO_PacketForwarding(void);
void EZMacPRO_PacketSent(void);
void EZMacPRO_CRCError(void);
void EZMacPRO_LBTTimeout(void);
void EZMacPRO_AckTimeout(void);
void EZMacPRO_AckSending(void);



#endif //_EZMACPRO_CALLBACKS_H_
