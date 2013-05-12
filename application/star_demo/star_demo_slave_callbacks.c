/*!\file star_demo_slave_callbacks.h
 * \brief Star demo callback functions on top of the EZMacPRO stack.
 *
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

#include "..\..\common.h"
#include "star_demo_node.h"

				/* ======================================= *
				 *	 G L O B A L	V A R I A B L E S	 *
				 * ======================================= */

volatile BIT fEZMacPRO_StateWakeUpEntered	= 0;
volatile BIT fEZMacPRO_StateSleepEntered	= 0;
volatile BIT fEZMacPRO_StateIdleEntered		= 0;
volatile BIT fEZMacPRO_StateRxEntered		= 0;
volatile BIT fEZMacPRO_StateTxEntered		= 0;
volatile BIT fEZMacPRO_StateErrorEntered	= 0;

volatile BIT fEZMacPRO_LFTimerExpired		= 0;
volatile BIT fEZMacPRO_LowBattery			= 0;
volatile BIT fEZMacPRO_SyncWordReceived		= 0;
volatile BIT fEZMacPRO_CRCError				= 0;
volatile BIT fEZMacPRO_PacketDiscarded		= 0;
volatile BIT fEZMacPRO_PacketReceived		= 0;
volatile BIT fEZMacPRO_PacketForwarding		= 0;
volatile BIT fEZMacPRO_PacketSent			= 0;
volatile BIT fEZMacPRO_LBTTimeout			= 0;
volatile BIT fEZMacPRO_AckTimeout			= 0;
volatile BIT fEZMacPRO_AckSending			= 0;

				/* ======================================= *
				 *	C A L L B A C K	F U N C T I O N S	*
				 * ======================================= */
/*!
 * The callback function is called when the stack enters Wake-up state.
 */
void EZMacPRO_StateWakeUpEntered(void)	
{
	fEZMacPRO_StateWakeUpEntered = 1;
}

/*!
 * The callback function is called when the stack enters Sleep state.
 */
void EZMacPRO_StateSleepEntered(void)
{
	fEZMacPRO_StateSleepEntered = 1;
}

/*!
 * The callback function is called when the stack enters Idle state.
 */
void EZMacPRO_StateIdleEntered(void)
{
	fEZMacPRO_StateIdleEntered = 1;
}

/*!
 * The callback function is called when the stack enters Receive state.
 */
void EZMacPRO_StateRxEntered(void)
{
	fEZMacPRO_StateRxEntered = 1;
}

/*!
 * The callback function is called when the stack enters Transmit state.
 */
void EZMacPRO_StateTxEntered(void)
{
	fEZMacPRO_StateTxEntered = 1;
}

/*!
 * The callback function is called when the stack enters Error state.
 */
void EZMacPRO_StateErrorEntered(void)
{
	fEZMacPRO_StateErrorEntered = 1;
}


void EZMacPRO_LFTimerExpired(void)
{
	fEZMacPRO_LFTimerExpired = 1;
}


void EZMacPRO_LowBattery(void)
{
	fEZMacPRO_LowBattery = 1;
}


#ifndef TRANSMITTER_ONLY_OPERATION
void EZMacPRO_SyncWordReceived(void)
{
	fEZMacPRO_SyncWordReceived = 1;
}
#endif //TRANSMITTER_ONLY_OPERATION not defined


#ifndef TRANSMITTER_ONLY_OPERATION
void EZMacPRO_PacketDiscarded(void)
{
	fEZMacPRO_PacketDiscarded = 1;
}
#endif //TRANSMITTER_ONLY_OPERATION not defined


#ifndef TRANSMITTER_ONLY_OPERATION
void EZMacPRO_PacketReceived(U8 rssi)
{	/* Save snapshot of RSSI. */
	SlaveInfo.rssi = rssi;
	fEZMacPRO_PacketReceived = 1;
}
#endif //TRANSMITTER_ONLY_OPERATION not defined


#ifdef PACKET_FORWARDING_SUPPORTED
void EZMacPRO_PacketForwarding(void)
{
	fEZMacPRO_PacketForwarding = 1;
}
#endif //PACKET_FORWARDING_SUPPORTED


#ifndef RECEIVER_ONLY_OPERATION
void EZMacPRO_PacketSent(void)
{
	fEZMacPRO_PacketSent = 1;
}
#endif //RECEIVER_ONLY_OPERATION not defined


#ifdef TRANSCEIVER_OPERATION
void EZMacPRO_LBTTimeout (void)
{
	fEZMacPRO_LBTTimeout = 1;
}
#endif //TRANSCEIVER_OPERATION


#ifdef EXTENDED_PACKET_FORMAT
#ifdef TRANSCEIVER_OPERATION
void EZMacPRO_AckTimeout (void)
{
	fEZMacPRO_AckTimeout = 1;
}
#endif //TRANSCEIVER_OPERATION
#endif //EXTENDED_PACKET_FORMAT


void EZMacPRO_CRCError(void)
{
	fEZMacPRO_CRCError = 1;
}


#ifdef EXTENDED_PACKET_FORMAT
#ifdef TRANSCEIVER_OPERATION
void EZMacPRO_AckSending(void)
{
	U8 BUFFER_MSPACE i;

	fEZMacPRO_AckSending = 1;
	//example: RSSI is sent back in Ack payload
	i = EZMacProRSSIvalue;
	EZMacPRO_Ack_Write(1,&i);
}
#endif //TRANSCEIVER_OPERATION
#endif //EXTENDED_PACKET_FORMAT

