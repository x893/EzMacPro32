/*!\file star_demo_master_node.c
 * \brief Star demo code on top of the EZMacPRO stack.
 *
 * \n The Star demo implements a simple application where the Master
 * \n periodically queries the temperature and supply voltage level of the
 * \n Slaves. A Slave can join the network by associating with the Master. To
 * \n do so the Slave has to be placed into Wait-for-beacon state by pressing
 * \n PB1. At this time a temporary random address is generated for the duration
 * \n of association. A beacon frame is transmitted periodically to the
 * \n multicast address of the network. Slaves willing to associate reply to
 * \n this with an associate request frame random time later. Then the Master
 * \n sends an associate response back, containing the new address assigned to
 * \n the Slave. Association is completed when the Master's response is
 * \n acknowledged and the Slave replaces its temporary address with the
 * \n assigned one. If this is successful, the Slave is periodically queried by
 * \n the Master. LED1 shows the power state of the radio. Off means the radio
 * \n is in Sleep state.
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
#include "star_demo_menu.h"


/* ======================================= *
 *	 G L O B A L	V A R I A B L E S	 *
 * ======================================= */

SEGMENT_VARIABLE(SlaveInfoTable[MAX_NMBR_OF_SLAVES], SlaveInfoTable_t, APPLICATION_MSPACE);
SEGMENT_VARIABLE(rfPayload,		Frame_uu,	APPLICATION_MSPACE);
SEGMENT_VARIABLE(packetLength,	U8,			APPLICATION_MSPACE);
SEGMENT_VARIABLE(rndCounter,	U16,		APPLICATION_MSPACE);
SEGMENT_VARIABLE(slaveAddr,		Addr_t,		APPLICATION_MSPACE);
SEGMENT_VARIABLE(rndAddr,		Addr_t,		APPLICATION_MSPACE);
SEGMENT_VARIABLE(nodeCnt,		U8,			APPLICATION_MSPACE);
SEGMENT_VARIABLE(DEMO_SR,		U8,			APPLICATION_MSPACE);

#ifdef TRACE_ENABLED
	SEGMENT_VARIABLE(printBuf[40], U8, APPLICATION_MSPACE);
#endif

/* ======================================= *
 *		L O C A L	F U N C T I O N S		*
 * ======================================= */

/*!
 * Entry point to the state machine.
 */
void StateMachine_Init(void)
{
	DISABLE_WATCHDOG();
	DEMO_SR = DEMO_BOOT_INIT;
}

/*!
 * This is the State Machine of the Demo Application.
 */
void StateMachine(void)
{
	switch (DEMO_SR & DEMO_STATE_MASK)
	{
		case DEMO_BOOT:
			BoardInit();				// Initialise board.
			MasterNodeBoot();			// Boot Master Node.
			break;

		case DEMO_ASSOC:
			MasterNodeAssociate();		// Associate slaves.
			break;

		case DEMO_SU:
			MasterNodeStatusUpdate();	// Get status updates from slaves.
			break;

		case DEMO_SLEEP:
			MasterNodeSleep();			// Sleep between cycles.
			break;
	}
}

/*!
 * Boot Master Node.
 */
void MasterNodeBoot(void)
{
	switch (DEMO_SR)
	{
		case DEMO_BOOT_INIT:

			ENABLE_GLOBAL_INTERRUPTS();					// Enable global interrupts.
			EZMacPRO_Init();							// Initialise EZMacPRO.

			WAIT_FLAG_TRUE(fEZMacPRO_StateSleepEntered);// Wait until device goes to Sleep.
			fEZMacPRO_StateWakeUpEntered = 0;			// Clear State transition flags.

			// Configure and start 2sec timeout for Silabs splash screen.
			EZMacPRO_Reg_Write(LFTMR0, LFTMR0_TIMEOUT_SEC(STARTUP_TIMEOUT));
			EZMacPRO_Reg_Write(LFTMR1, LFTMR1_TIMEOUT_SEC(STARTUP_TIMEOUT));
			EZMacPRO_Reg_Write(LFTMR2, 0x80 | LFTMR2_TIMEOUT_SEC(STARTUP_TIMEOUT));

			PERFORM_MENU(sMenu_StartupScreen);			// Show Silabs logo, demo name & version.

			WAIT_FLAG_TRUE(fEZMacPRO_LFTimerExpired);	// Wait here until LFT expires.
														// Disable LFT.
			EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_SEC(STARTUP_TIMEOUT));
			fEZMacPRO_LFTimerExpired = 0;

														// Star demo Master node initialisation.
			EZMacPRO_Reg_Write(MCR, 0xAC);				// CIDE=1, DR=9.6kbps, RAD=1, DNPL=1, NFR=0
			EZMacPRO_Reg_Write(SECR, 0x60);				// State after receive is RX state and state after transmit is Idle state
			EZMacPRO_Reg_Write(RCR, 0x00);				// Search disable
			EZMacPRO_Reg_Write(FR0, 0);					// Set the used frequency channel
			EZMacPRO_Reg_Write(TCR, (0x70|LBT_SWITCH)); // LBT enabled/disabled, Output power: +20 dBm, ACK disable, AFC disable
			EZMacPRO_Reg_Write(LBTLR, 0x78);			// RSSI threshold -60 dB
			EZMacPRO_Reg_Write(LBTIR, 0x8A);			// Time interval
			EZMacPRO_Reg_Write(LBDR, 0x80);				// Enable Low Battery Detect
			EZMacPRO_Reg_Write(PFCR, 0x28);				// Destination address filter is enabled
			EZMacPRO_Reg_Write(SCID, DEMO_MASTER_CID);	// Set customer ID
			EZMacPRO_Reg_Write(SFID, DEMO_MASTER_SFID); // Set self ID

			for (nodeCnt = 0; nodeCnt < MAX_NMBR_OF_SLAVES; nodeCnt++)
			{	// Init slave info table.
				SlaveInfoTable[nodeCnt].associated = NOT_ASSOCIATED;
				SlaveInfoTable[nodeCnt].address.cid = DEMO_MASTER_CID;
				SlaveInfoTable[nodeCnt].address.sfid = nodeCnt+1;
				SlaveInfoTable[nodeCnt].timeout = TIMEOUT_INITIAL_VALUE;
				SlaveInfoTable[nodeCnt].temperature = TEMP_INVALID_VALUE;
				SlaveInfoTable[nodeCnt].voltage = VOLTAGE_INVALID_VALUE;
				SlaveInfoTable[nodeCnt].rssi = RSSI_INITIAL_VALUE;
			}

			PERFORM_MENU(sMenu_MainScreen);				// Show main screen.
			TRACE("[DEMO_BOOT] Master startup done.\n");

			EZMacPRO_Wake_Up();							// Wake up from Sleep mode.
			WAIT_FLAG_TRUE(fEZMacPRO_StateIdleEntered);	// Wait until device goes to Idle.
			fEZMacPRO_StateWakeUpEntered = 0;			// Clear State transition flags.
			DEMO_SR = DEMO_ASSOC_BEACON_TX;				// Go to next state.
			break;
	}
}


/*!
 * Associate slaves.
 */
void MasterNodeAssociate(void)
{
	/* LFT expired. Go to Status Update state. */
	if (fEZMacPRO_LFTimerExpired)
	{
										// Disable LFT
		EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
		fEZMacPRO_LFTimerExpired = 0;	// Clear flag

		EZMacPRO_Idle();				// Go to Idle state.

		PERFORM_MENU(sMenu_StatusUpdate_State);		// Display Status Update state on LCD.

		WAIT_FLAG_TRUE(fEZMacPRO_StateIdleEntered);	// Wait until device goes to Idle.

		TRACE("[DEMO_ASSOC] Search ended.\n");
		TRACE("[DEMO_SU] Start to query slaves.\n");

		nodeCnt = 0;				// Reset node counter.
		DEMO_SR = DEMO_SU_REQ_TX;	// Go ahead and request status updates.
		return;
	}

	switch(DEMO_SR)
	{
		case DEMO_ASSOC_BEACON_TX:
			/* Configure and start 200msec timeout for Beacon time frame. */
			EZMacPRO_Reg_Write(LFTMR0, LFTMR0_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
			EZMacPRO_Reg_Write(LFTMR1, LFTMR1_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
			EZMacPRO_Reg_Write(LFTMR2, 0x80 | LFTMR2_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
			LED1_ON();		// LED1 indicates the radio is ON

			/* Display Associate state on LCD. */
			PERFORM_MENU(sMenu_Associate_State);
			TRACE("[DEMO_ASSOC] Search for slaves to associate with.\n");

			/* Configure Beacon Frame. */
			EZMacPRO_Reg_Write(TCR, (0x70 | LBT_SWITCH));	// LBT enabled/disabled, Output power: +20 dBm, ACK disable, AFC disable
			EZMacPRO_Reg_Write(DID, DEMO_MASTER_MCAST);		// Set Destination ID
			
			rfPayload.frameUnion.beacon.type = FRAME_BEACON;	// Assemble beacon frame
			// Write the packet length and payload to the TX buffer
			EZMacPRO_TxBuf_Write(sizeof(FrameBeacon_t), &rfPayload.frameRaw[0]);
			EZMacPRO_Transmit();								// Send the packet

			WAIT_FLAG_TRUE(fEZMacPRO_StateIdleEntered);	// Wait until device goes to Idle.

			DEMO_SR = DEMO_ASSOC_REQ_RX;	// Go to next state.
			break;

		case DEMO_ASSOC_REQ_RX:
			 /* Go to receive state. */
			 EZMacPRO_Receive();
			 /* Wait until radio is placed to RX. */
			 while (!fEZMacPRO_StateRxEntered);
			 /* Clear State transition flag. */
			 fEZMacPRO_StateRxEntered = 0;
			 /* Go to next state. */
			 DEMO_SR = DEMO_ASSOC_REQ_RXD;
			break;

		case DEMO_ASSOC_REQ_RXD:
			 if (fEZMacPRO_PacketReceived)
			 {	/* Clear flag. */
				 fEZMacPRO_PacketReceived = 0;
				 /* Free slot in association table. Check message type. */
				 if ((nodeCnt = SearchFreeSlotInAssocTable()) != ASSOC_TABLE_FULL)
				 {	/* Read out the payload. */
					 EZMacPRO_RxBuf_Read(&packetLength, &rfPayload.frameRaw[0]);
					 /* Message is association request. */
					 if (rfPayload.frameUnion.assocReq.type == FRAME_ASSOC_REQ)
					 {	/* Save address of the slave. */
						 EZMacPRO_Reg_Read(RCID, &slaveAddr.cid);
						 EZMacPRO_Reg_Read(RSID, &slaveAddr.sfid);
						 /* Go to next state. */
						 DEMO_SR = DEMO_ASSOC_RESP_TX;
					 }
				 }
			 }
			 break;

		case DEMO_ASSOC_RESP_TX:
			EZMacPRO_Idle();							// Go to Idle state to read Beacon frame response.
			WAIT_FLAG_TRUE(fEZMacPRO_StateIdleEntered);	// Wait until device goes to Idle.

			EZMacPRO_Reg_Write(DID, slaveAddr.sfid);	// Send Association response with new address of the node.
														// Assemble Association response frame.
			rfPayload.frameUnion.assocResp.type = FRAME_ASSOC_RESP;
			rfPayload.frameUnion.assocResp.newAddr.cid = SlaveInfoTable[nodeCnt].address.cid;
			rfPayload.frameUnion.assocResp.newAddr.sfid = SlaveInfoTable[nodeCnt].address.sfid;
														// Write the packet length and payload to the TX buffer.
			EZMacPRO_TxBuf_Write(sizeof(FrameAssocResp_t), &rfPayload.frameRaw[0]);
			EZMacPRO_Transmit();						// Send the packet.
			WAIT_FLAG_TRUE(fEZMacPRO_StateIdleEntered);	// Wait until device goes to Idle.

			DEMO_SR = DEMO_ASSOC_RESP_ACK_RX;			// Go to next state.
			break;

		case DEMO_ASSOC_RESP_ACK_RX:
			EZMacPRO_Receive();							// Go to receive state.
			WAIT_FLAG_TRUE(fEZMacPRO_StateRxEntered);	// Wait until radio is placed to RX.
			DEMO_SR = DEMO_ASSOC_RESP_ACK_RXD;			// Go to next state.
			break;

		case DEMO_ASSOC_RESP_ACK_RXD:
			if (fEZMacPRO_PacketReceived)
			{	// Clear flag.
				fEZMacPRO_PacketReceived = 0;
				EZMacPRO_RxBuf_Read(&packetLength, &rfPayload.frameRaw[0]);	// Read out the payload of the acknowledgement.

				// Message is the acknowledgement of association response.
				if (rfPayload.frameUnion.assocRespAck.type == FRAME_ASSOC_RESP_ACK)
				{	// From the right node.
					if (memcmp(&rfPayload.frameUnion.assocRespAck.rndAddr, &slaveAddr, sizeof(Addr_t)) == 0)
					{	// Set flag.
						SlaveInfoTable[nodeCnt].associated = ASSOCIATED;
						TRACE("[DEMO_ASSOC] Slave associated with address: %02bu.\n", SlaveInfoTable[nodeCnt].address.sfid);
					}
				}

				EZMacPRO_Idle();							// Go to Idle state.
				WAIT_FLAG_TRUE(fEZMacPRO_StateIdleEntered);	// Wait until device goes to Idle.
				DEMO_SR = DEMO_ASSOC_REQ_RX;				// Go to next state.
			}
			break;
	}
}


/*!
 * Query slaves.
 */
void MasterNodeStatusUpdate(void)
{
	/* LFT expired. Go to Status Update state. */
	if (fEZMacPRO_LFTimerExpired)
	{
		if (fEZMacPRO_StateSleepEntered)				// Successful Slave query. Master was in Sleep state.
		{												// Wake up the stack.
			fEZMacPRO_StateSleepEntered = 0;
			EZMacPRO_Wake_Up();							// Wake up from Sleep mode.
			WAIT_FLAG_TRUE(fEZMacPRO_StateIdleEntered);	// Wait until device goes to Idle.
			fEZMacPRO_StateWakeUpEntered = 0;			// Clear State transition flags.
		}
		else											// Slave query failed. No response from Slave.
		{
			EZMacPRO_Idle();							// Go to Idle state.
			WAIT_FLAG_TRUE(fEZMacPRO_StateIdleEntered);	// Wait until device goes to Idle.
			// Decrement timeout value of the node, replace temperature,
			// supply voltage and rssi values with zero.
			if (--SlaveInfoTable[nodeCnt].timeout == TIMEOUT_ZERO_VALUE)
			{	// Timeout. Disassociate Slave.
				SlaveInfoTable[nodeCnt].associated = NOT_ASSOCIATED;
				SlaveInfoTable[nodeCnt].timeout = TIMEOUT_INITIAL_VALUE;
			}
			SlaveInfoTable[nodeCnt].temperature = TEMP_INVALID_VALUE;
			SlaveInfoTable[nodeCnt].voltage = VOLTAGE_INVALID_VALUE;
			SlaveInfoTable[nodeCnt].rssi = RSSI_ZERO_VALUE;
		}

		LED1_ON();	// LED1 indicates the radio is ON.
														// Disable LFT.
		EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_MSEC(TIMEFRAME_SU_PER_SLAVE));
		fEZMacPRO_LFTimerExpired = 0;

		if (++nodeCnt < MAX_NMBR_OF_SLAVES)				// Jump to next node in Slave Info table.
			DEMO_SR = DEMO_SU_REQ_TX;					// Go ahead and request status update from next node.
		else
		{	// Go to Sleep between cycles.
			TRACE("[DEMO_SU] Slave query ended.\n");
			DEMO_SR = DEMO_SLEEP_INIT;
		}
	}

	switch(DEMO_SR)
	{
		case DEMO_SU_REQ_TX:
			// Configure and start 200msec timeout for Status Update time frame.
			EZMacPRO_Reg_Write(LFTMR0, LFTMR0_TIMEOUT_MSEC(TIMEFRAME_SU_PER_SLAVE));
			EZMacPRO_Reg_Write(LFTMR1, LFTMR1_TIMEOUT_MSEC(TIMEFRAME_SU_PER_SLAVE));
			EZMacPRO_Reg_Write(LFTMR2, 0x80 | LFTMR2_TIMEOUT_MSEC(TIMEFRAME_SU_PER_SLAVE));

			// Associated. Query status.
			if (SlaveInfoTable[nodeCnt].associated == ASSOCIATED)
			{											// Set DID of Status Update request frame.
				EZMacPRO_Reg_Write(DID, SlaveInfoTable[nodeCnt].address.sfid);
														// Assemble Status Update request frame.
				rfPayload.frameUnion.statusUpdateReq.type = FRAME_SU_REQ;
														// Write the packet length and payload to the TX buffer.
				EZMacPRO_TxBuf_Write(sizeof(FrameStatusUpdateReq_t), &rfPayload.frameRaw[0]);
				TRACE("[DEMO_SU] Slave[%02bu]: associated. Query slave.\n", SlaveInfoTable[nodeCnt].address.sfid);

				EZMacPRO_Transmit();					// Send the packet.
				WAIT_FLAG_TRUE(fEZMacPRO_StateIdleEntered);	// Wait until device goes to Idle.
				DEMO_SR = DEMO_SU_RESP_RX;				// Go to wait for status update response.
			}
			else										// Not associated.
			{
				EZMacPRO_Sleep();						// Go to Sleep state.
				while (!fEZMacPRO_StateSleepEntered);	// Wait until device goes to Sleep.
				LED1_OFF();								// LED1 indicates the radio is OFF.
				TRACE("[DEMO_SU] Slave[%02bu]: not associated. Skip slave.\n", SlaveInfoTable[nodeCnt].address.sfid);
				DEMO_SR = DEMO_SU_SLEEP;				// Go to Status Update Sleep state.
			}
			break;

		case DEMO_SU_RESP_RX:
			EZMacPRO_Receive();							// Go to receive state.
			WAIT_FLAG_TRUE(fEZMacPRO_StateRxEntered);	// Wait until radio is placed to RX.
			DEMO_SR = DEMO_SU_RESP_RXD;					// Go to next state.
			break;

		case DEMO_SU_RESP_RXD:
			if (fEZMacPRO_PacketReceived)
			{
				fEZMacPRO_PacketReceived = 0;
				// Read out the payload of the packet.
				EZMacPRO_RxBuf_Read(&packetLength, &rfPayload.frameRaw[0]);
				// Frame is status update response.
				if (rfPayload.frameUnion.statusUpdateResp.type == FRAME_SU_RESP)
				{	// From the right node.
					EZMacPRO_Reg_Read(RCID, &slaveAddr.cid);
					EZMacPRO_Reg_Read(RSID, &slaveAddr.sfid);
					// Frame is sent to Master.
					if (memcmp(&SlaveInfoTable[nodeCnt].address, &slaveAddr, sizeof(Addr_t)) == 0)
					{	// Update status.
						SlaveInfoTable[nodeCnt].temperature = rfPayload.frameUnion.statusUpdateResp.temperature;
						SlaveInfoTable[nodeCnt].voltage = rfPayload.frameUnion.statusUpdateResp.voltage;
						SlaveInfoTable[nodeCnt].rssi = rfPayload.frameUnion.statusUpdateResp.rssi;

						EZMacPRO_Idle();							// Go to Idle state.

						WAIT_FLAG_TRUE(fEZMacPRO_StateIdleEntered);	// Wait until device goes to Idle.
						EZMacPRO_Sleep();							// Go to Sleep state.
						while (!fEZMacPRO_StateSleepEntered);		// Wait until device goes to Sleep.
						LED1_OFF();									// LED1 indicates the radio is OFF.
						DEMO_SR = DEMO_SU_SLEEP;					// Go to sleep state.
						break;
					}
				}
			}
			break;

		case DEMO_SU_SLEEP:
			// Wait here until current Status Update time slot is over.
			break;
	}
}


/*!
 * Sleep between cycles.
 */
void MasterNodeSleep(void)
{
	if (fEZMacPRO_LFTimerExpired)					// LFT expired. Go to Status Update state.
	{												// Disable LFT.
		EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_SEC(TIMEFRAME_SLEEP));
		fEZMacPRO_LFTimerExpired = 0;

		EZMacPRO_Wake_Up();							// Wake up from Sleep mode.

		WAIT_FLAG_TRUE(fEZMacPRO_StateIdleEntered);	// Wait until device goes to Idle.
		fEZMacPRO_StateWakeUpEntered = 0;			// Clear State transition flags.
		TRACE("[DEMO_ASSOC] Master woke up.\n");
		DEMO_SR = DEMO_ASSOC_BEACON_TX;				// Go ahead and request status updates.
		return;
	}

	switch(DEMO_SR)
	{
		case DEMO_SLEEP_INIT:
			EZMacPRO_Sleep();								// Go to Sleep state.
			WAIT_FLAG_TRUE(fEZMacPRO_StateSleepEntered);	// Wait until device goes to Sleep.
															// Configure and start inter-cycle timeout.
			EZMacPRO_Reg_Write(LFTMR0, LFTMR0_TIMEOUT_SEC(TIMEFRAME_SLEEP));
			EZMacPRO_Reg_Write(LFTMR1, LFTMR1_TIMEOUT_SEC(TIMEFRAME_SLEEP));
			EZMacPRO_Reg_Write(LFTMR2, 0x80 | LFTMR2_TIMEOUT_SEC(TIMEFRAME_SLEEP));
			LED1_OFF();										// LED1 indicates the radio is OFF.
															// Update parameters of Slaves on LCD.
			PERFORM_MENU(sMenu_RefreshScreen);				// Display Status Update state on UART.
			TRACE("[DEMO_SLEEP] Slave Info Table:\n", SlaveInfoTable[nodeCnt].address.sfid);
			TRACE_SLAVE_INFO();
			PERFORM_MENU(sMenu_SleepState);					// Display Status Update state on LCD.
			TRACE("[DEMO_SLEEP] Master went to sleep.\n\n\n");

			DEMO_SR = DEMO_SLEEP_LOOP;						// Go to sleep loop state.
			break;

		case DEMO_SLEEP_LOOP:
			// Wait here between cycles.
			break;
	}
}


/*!
 * Search for free slot in association table.
 */
U8 SearchFreeSlotInAssocTable(void)
{
	U8 cnt;
	for (cnt = 0; cnt < MAX_NMBR_OF_SLAVES; cnt++)	// Look for free slot in the association table.
		if (!SlaveInfoTable[cnt].associated)		// Free slot found.
			return cnt;								// Return index.
	return ASSOC_TABLE_FULL;
}

#ifdef TRACE_ENABLED
/*!
 * Print Slave info to UART.
 */
void PrintSlaveInfo(void)
{
	U8 bCnt;

	/* Loop over Association table. */
	for (bCnt = 0; bCnt < MAX_NMBR_OF_SLAVES; bCnt++)
	{	/* Associated node info - temperature and voltage units. */
		if (SlaveInfoTable[bCnt].associated == ASSOCIATED)
		{	/* Print slave status. */
			SprintfSlaveInfo(&SlaveInfoTable[bCnt], printBuf);
			/* Print Rssi. */
			sprintf(&printBuf[21], "	[rssi:%03bu]", SlaveInfoTable[bCnt].rssi);
		}
		else
		{	/* Update slave status - not associated. */
			SprintfSlaveNotAssoc(printBuf);
		}
		printf("	%s\n", printBuf);
	}
}
#endif //TRACE_ENABLED
