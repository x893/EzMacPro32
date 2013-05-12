/*!\file p2p_demo_tx_node.c
 * \brief P2P demo code on top of the EZMacPRO stack.
 *
 * \n P2P demo Transmitter node code description:
 * \n After power on reset the Transmitter node goes into Sleep state to wait
 * \n for the 1 second Low-Frequency Timer timeout. When the timer expires a
 * \n packet is transmitted with an ascending number in its payload. The content
 * \n of the last packet is always updated on the LCD before the radio is placed
 * \n into Sleep.
 * \n LED1 toggles when a packet transmission is acknowledged by the Receiver.
 *
 *
 * \n This software must be used in accordance with the End User License
 * \n Agreement.
 *
 * \b COPYRIGHT
 * \n Copyright 2012 Silicon Laboratories, Inc.
 * \n http://www.silabs.com
 */

/* ==================================== *
 *				I N C L U D E			*
 * ==================================== */
#include "..\..\common.h"
#include "p2p_demo_node.h"
#include "p2p_demo_tx_menu.h"

/* ==================================== *
 *	 G L O B A L	V A R I A B L E S	*
 * ==================================== */

SEGMENT_VARIABLE(abRfPayload[5], U8, BUFFER_MSPACE);
SEGMENT_VARIABLE(bPacketLength, U8, APPLICATION_MSPACE);
SEGMENT_VARIABLE(wPacketCounter, U16, APPLICATION_MSPACE);
SEGMENT_VARIABLE(DEMO_SR, U8, APPLICATION_MSPACE);

#ifdef TRACE_ENABLED
	SEGMENT_VARIABLE(printBuf[40], U8, APPLICATION_MSPACE);
#endif

/* ==================================== *
 *		L O C A L	F U N C T I O N S	*
 * ==================================== */

/*!
 * Entry point to the state machine.
 */
void StateMachine_Init(void)
{
	/* Disable Watchdog. */
	DISABLE_WATCHDOG();
	/* Init Demo State Machine. */
	DEMO_SR = DEMO_BOOT;
}


/*!
 * This is the State Machine of the Demo Application.
 */
void StateMachine(void)
{
	switch (DEMO_SR)
	{
		case DEMO_BOOT:
			BoardInit();
			ENABLE_GLOBAL_INTERRUPTS();
			EZMacPRO_Init();
			/* Wait until device goes to Sleep. */
			WAIT_FLAG_TRUE(fEZMacPRO_StateSleepEntered);
			/* Clear State transition flags. */
			fEZMacPRO_StateWakeUpEntered = 0;
			vP2P_demo_TxInit();			// Point to point demo initialisation.
			DEMO_SR = DEMO_TX;			// Go to TX state.
			break;

		case DEMO_TX:
			// LFT expired, send next packet.
			if (fEZMacPRO_LFTimerExpired)
			{
				fEZMacPRO_LFTimerExpired = 0;
				// Send packet then place the radio to sleep.
				vP2P_demo_SendPacketGoToSleep();
				DEMO_SR = DEMO_TX_WF_ACK;	// Go to TX wait for acknowledgement state.
			}
			break;

		case DEMO_TX_WF_ACK:
			// Auto-acknowledgement has arrived.
			if (fEZMacPRO_PacketSent)
			{
				fEZMacPRO_PacketSent = 0;
				LED1_TOGGLE();
				DEMO_SR = DEMO_TX;			// Go to TX state.
			}

			// Auto-acknowledgement has not arrived.
			if (fEZMacPRO_AckTimeout)
			{
				fEZMacPRO_AckTimeout = 0;
				DEMO_SR = DEMO_TX;			// Go to TX state.
			}
			break;

		default:
			break;
	}
}



void vP2P_demo_TxInit(void)
{
	// Configure and start 2sec timeout for Silabs splash screen.
	EZMacPRO_Reg_Write(LFTMR0, LFTMR0_TIMEOUT_SEC(STARTUP_TIMEOUT));
	EZMacPRO_Reg_Write(LFTMR1, LFTMR1_TIMEOUT_SEC(STARTUP_TIMEOUT));
	EZMacPRO_Reg_Write(LFTMR2, 0x80 | LFTMR2_TIMEOUT_SEC(STARTUP_TIMEOUT));

	// Show Silabs logo, demo name & version.
	PERFORM_MENU(sMenu_StartupScreen);
	/* Wait here until LFT expires. */
	WAIT_FLAG_TRUE(fEZMacPRO_LFTimerExpired);

	// Disable LFT.
	EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_SEC(STARTUP_TIMEOUT));
	// Init counter.
	wPacketCounter = 0;
	// Init packet content.
	memcpy(&abRfPayload[0], "		", 6);
	EZMacPRO_Reg_Write(MCR, 0xAC);				// Set data rate to 9.6kbps, DNPL = 1, rad = 1, used 1 channel
	EZMacPRO_Reg_Write(SECR, 0x20);			 // State after receive is RX state and state after transmit is Sleep state
	EZMacPRO_Reg_Write(TCR, 0xB8);				// LBT Before Talk enabled, Output power: +20 dBm, no ACK, AFC disable
	EZMacPRO_Reg_Write(LBTLR, 0x78);			// RSSI threshold -60 dB
	EZMacPRO_Reg_Write(LBTIR, 0x8A);			// Time interval
	EZMacPRO_Reg_Write(FR0, 1);				 // set the used frequency channel
	EZMacPRO_Reg_Write(SCID, CUSTOMER_ID);		// set the customer ID to 0x01
	EZMacPRO_Reg_Write(SFID, DEVICE_SELF_ID);	// set the self ID
	EZMacPRO_Reg_Write(LFTMR0, 0x00);			// Set the interval to 1s
	EZMacPRO_Reg_Write(LFTMR1, 0x02);			// Set the interval to 1s
	EZMacPRO_Reg_Write(LFTMR2, 0xC4);			// Enable LFT, use Internal Time Base

	// Show TX screen on LCD.
	PERFORM_MENU(sMenu_TxScreen);
 
	TRACE("[TX_NODE][DEMO_BOOT] Startup done.\n");
	TRACE("[TX_NODE][DEMO_TX] Went to sleep.\n");
}


void vP2P_demo_SendPacketGoToSleep()
{
	/* Set packet length. */
	bPacketLength = 5;
	/* Increase counter, set up packet content. */
	sprintf((char *)abRfPayload, "%1u", ++wPacketCounter);
	/* Set Destination ID. */
	EZMacPRO_Reg_Write(DID, 0x01);
	/* Write the packet length and payload to the TX buffer. */
	EZMacPRO_TxBuf_Write(bPacketLength, &abRfPayload[0]);
	/* Wake up from Sleep mode. */
	EZMacPRO_Wake_Up();
	/* Wait until device goes to Idle. */
	WAIT_FLAG_TRUE(fEZMacPRO_StateIdleEntered);
	/* Clear State transition flags. */
	fEZMacPRO_StateWakeUpEntered = 0;

	/* Display Transmit on LCD. */
	PERFORM_MENU(sMenu_PacketTransmit);
	TRACE("[TX_NODE][DEMO_SLEEP] Woke up.\n");
	/* Send the packet. */
	EZMacPRO_Transmit();
	/* Wait until device goes back to Sleep. */
	WAIT_FLAG_TRUE(fEZMacPRO_StateSleepEntered);
	
	/* Update number of transmitted packets on LCD. */
	PERFORM_MENU(sMenu_TxRefreshCounter);
	TRACE("[TX_NODE][DEMO_TX] Transmitted packet. Packet content:%5u\n", wPacketCounter);
	/* Display Sleep on LCD. */
	PERFORM_MENU(sMenu_SleepMode);
	TRACE("[TX_NODE][DEMO_SLEEP] Went to sleep.\n");
}

