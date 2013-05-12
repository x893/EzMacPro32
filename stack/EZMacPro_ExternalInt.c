/*!\file EZMacPro_ExternalInt.c
 * \brief External interrupt triggered state machine and functions.
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
 *	 G L O B A L	V A R I A B L E S	 *
 * ======================================= */

/* ======================================= *
 *		L O C A L	F U N C T I O N S		*
 * ======================================= */

//------------------------------------------------------------------------------------------------
// Function Name
//	externalIntISR()
//
// Return Value : None
// Parameters	: None
//
// This is the Interrupt Service Routing for the External INT0 interrupt. The INT0 interrupt
// is connected to the IRQ output from the Si4432 radio. The INT0 interrupt is configured as
// active low level sensitive. So the Si4432 interrupt must be cleared before exiting the ISR.
//
//
// The Basic States (Wake-up, Receive, and Transmit) are implemented using if..else if bit
// tests for the corresponding bit in the master control register. The detailed TX and RX
// state machines are implemented in separate functions.
//
// Conditional compile time switches remove the TX or RX state machines for the RX only and
// and TX only builds.
//
// Most interrupt events should disable to the timer interrupt to cancel any pending
// timeout events. If a interrupt event is to initiate a timeout event, the extIntTimeout()
// function should be used.
//
// The external interrupt should not call functions from other modules. This would create cause
// a multiple call to segment warning and result in poor RAM usage.
//
//-----------------------------------------------------------------------------------------------
INTERRUPT(externalIntISR, INTERRUPT_INT0)
{
	U8 state;
	U8 msr;
	U8 intStatus1;
	U8 intStatus2;

	// clear MAC external interrupt (8051 INT0 interrupt)
	// then always read both interrupt status registers to clear IQR pin
	CLEAR_MAC_EXT_INTERRUPT();
	intStatus1 = extIntSpiReadReg(SI4432_INTERRUPT_STATUS_1);
	intStatus2 = extIntSpiReadReg(SI4432_INTERRUPT_STATUS_2);

	if (intStatus1 != 0 ||
		(intStatus2 & (SI4432_ISWDET | SI4432_IPREAVAL | SI4432_IPREAINVAL | SI4432_IRSSI | SI4432_ICHIPRDY | SI4432_IPOR)) != 0)
	{
		msr = EZMacProReg.name.MSR;
		state = msr & 0x0F;

		if (msr == EZMAC_PRO_WAKE_UP)	// if the MAC is in Wake up state call the WakeUp function
			extIntWakeUp(intStatus2);
#ifndef RECEIVER_ONLY_OPERATION
		else if (msr & TX_STATE_BIT)	// if the MAC is in transmit state then call the transmit state machine
			extIntTX_StateMachine(state, intStatus1, intStatus2);
#endif
#ifndef TRANSMITTER_ONLY_OPERATION
		else if (msr & RX_STATE_BIT)	// if the MAC is in receive state then call the receiver
			extIntRX_StateMachine(state, intStatus1, intStatus2);
#endif
		else
			extIntDisableInterrupts();
	}

	// Low Frequency Timer
	// if Wake up timer interrupt is occurred the proper callback function will be called
	if (intStatus2 & SI4432_IWUT)
	{
		EZMacPRO_LFTimerExpired();
		//! ENABLE_MAC_EXT_INTERRUPT();
	}

	// Low Battery Detect
	// if low battery detect interrupt is occurred the proper callback function will be called
	if (intStatus2 & SI4432_ILBD)
	{
		EZMacPRO_LowBattery();
		//! ENABLE_MAC_EXT_INTERRUPT();
	}
}
//------------------------------------------------------------------------------------------------
// Function Name
//	extIntWakeUp()
//
// Return Value : None
// Parameters	: None
//
// The EZMAC_PRO_WAKE_UP state is used when starting the radio XTAL after sleep mode or when
// resetting the radio using a SW Reset SPI command.
//
// If this function is called, an interrupt has occurred before the wake-up timeout. If the
// interrupt source is the ichiprdy bit, the crystal has started successfully.
//
// When performing a SWRESET the interrupt enable will be reset to the default state.
// This enables the POR interrupt. So it is possible to get a POR interrupt before the
// chip ready interrupt. If this happens, a second attempt is made to start the crystal.
//
//-----------------------------------------------------------------------------------------------
void extIntWakeUp(U8 intStatus2)
{
	// if chip ready interrupt is occured
	if (intStatus2 & SI4432_ICHIPRDY)
	{
		extIntDisableInterrupts();				// disable all interrupts
		EZMacProReg.name.MSR = EZMAC_PRO_IDLE;	// go to Idle state
		EZMacPRO_StateIdleEntered();			// Call the Idle state entered callback function.
	}
	else if (intStatus2 & SI4432_IPOR)			// if software reset interrupt is occurred
	{
		extIntSpiWriteReg(SI4432_INTERRUPT_ENABLE_1, 0);
		extIntSetEnable2 (SI4432_ENCHIPRDY);	// enable chip ready interrupt

		extIntTimeout (TIMEOUT_XTAL_START);		// set time out
		ENABLE_MAC_TIMER_INTERRUPT();
		extIntSetFunction1(SI4432_XTON);		// enable the XTAL
	}
	else
	{
		extIntDisableInterrupts();				// disable all interrupts
		EZMacProReg.name.MSR = WAKE_UP_ERROR;
		EZMacPRO_StateErrorEntered();			// call the error state entered callback function
	}
}

//------------------------------------------------------------------------------------------------
// Function Name
//	extIntTX_StateMachine()
//
// Return Value : None
// Parameters	: U8 state - TX state, least significant nibble of MSR
//
// This function implements the detailed TX state machine. The state machine is implemented
// using switch...case... statements. For efficient compilation, the states are a series
// of sequential continuous integers (no missing values). The states are enumerated in the
// EZMacPro.h header file.
//
// Conditional compile time switches remove the unused TX states if LBT of ACK is not supported.
// These states are also removed from the enumeration at the begriming or end of the list.
//
// Conditional compile time switches remove this entire function for the Receiver only build.
//
//-----------------------------------------------------------------------------------------------
#ifndef RECEIVER_ONLY_OPERATION
void	extIntTX_StateMachine(U8 msr, U8 intStatus1, U8 intStatus2)
{
	#ifdef EXTENDED_PACKET_FORMAT
		#ifndef TRANSMITTER_ONLY_OPERATION
	U8 temp8;
		#endif
	#endif

	switch (msr)
	{
	#ifdef TRANSCEIVER_OPERATION
		case TX_STATE_LBT_START_LISTEN:
		case TX_STATE_LBT_LISTEN:
		case TX_STATE_LBT_RANDOM_LISTEN:
			// if RSSI interrupt is occured set the LBT flag
			if (intStatus2 & SI4432_IRSSI)
				BusyLBT = 1;
			break;
	#endif

		case TX_STATE_WAIT_FOR_TX:
			// if packet sent interrupt is occured
			if (intStatus1 & SI4432_IPKSENT)
			{
	#ifdef FOUR_CHANNEL_IS_USED
				// if Automatic Frequency Change feature is on then send the same packet on the four channels
				if (EZMacProReg.name.TCR & 0x04)	// if AFCH==1 && ACKRQ = ignore
				{
					if (EZMacProCurrentChannel < (EZMacProReg.name.MCR & 0x03))
					{
						// select the next frequency channel
						EZMacProCurrentChannel++;
						extIntSpiWriteReg (SI4432_FREQUENCY_HOPPING_CHANNEL_SELECT,EZMacProReg.array[FR0+EZMacProCurrentChannel]);
						// start timer with packet transmit timeout
						extIntTimeout(TimeoutTX_Packet);
						ENABLE_MAC_INTERRUPTS();
						// enable TX
						extIntSetFunction1(SI4432_TXON|SI4432_XTON);
						break;
					}
				}
	#endif
	#ifdef EXTENDED_PACKET_FORMAT
				if (!(intStatus2 & SI4432_IWUT))
				{
		#ifndef TRANSMITTER_ONLY_OPERATION
					if ((EZMacProReg.name.TCR & 0x84) == 0x80) // if ACKRQ = 1 && AFCH==0
					{
			#ifndef B1_ONLY
						if (EZMacProReg.name.DTR == 0)//if rev V2 chip is used
						// set the RX deviation (only rev V2)
						extIntSpiWriteReg (SI4432_FREQUENCY_DEVIATION, RX_Freq_dev);
			#endif
						// disable PKSENT, enable PKVALID & CRCERROR
						extIntSpiWriteReg (SI4432_INTERRUPT_ENABLE_1, SI4432_ENCRCERROR|SI4432_ENPKVALID);
						// disable Enable2 interrupts
						extIntSetEnable2(0x00);
						// start timer with ACK timeout
						extIntTimeout(TimeoutACK);
						// go to next state
						EZMacProReg.name.MSR = TX_STATE_BIT | TX_STATE_WAIT_FOR_ACK;
						ENABLE_MAC_INTERRUPTS();
						// turn on RX, leave XTAL on
						extIntSetFunction1(SI4432_RXON | SI4432_XTON);
						break;
					}
		#endif
				}
	#endif
				
				DISABLE_MAC_TIMER_INTERRUPT();		// cancel timeout
				// Next state after TX
				// disable PKSENT
				// call the packet sent callback function
				EZMacPRO_PacketSent();
				extIntSpiWriteReg (SI4432_INTERRUPT_ENABLE_1, 0x00);
				extIntGotoNextStateUsingSECR(1);	// go to the next state
			}
			break;

	#ifdef EXTENDED_PACKET_FORMAT
		#ifdef TRANSCEIVER_OPERATION
		case TX_STATE_WAIT_FOR_ACK:
			// read out the destination ID
			if (EZMacProReg.name.MCR & 0x80 )		// if CID is used
				temp8 = extIntSpiReadReg(SI4432_RECEIVED_HEADER_0);
			else
				temp8 = extIntSpiReadReg(SI4432_RECEIVED_HEADER_1);

			// if packet received and the packet is sent to me
			if (
				(intStatus1 & SI4432_IPKVALID) == SI4432_IPKVALID &&
				temp8 == EZMacProReg.name.SFID
				)
			{
				// read out the control byte
				temp8 = extIntSpiReadReg(SI4432_RECEIVED_HEADER_3);
				// if the packet is an acknowledgement
				if (temp8 & 0x08)
				{
					// disable PKVALID & CRCERROR
					extIntSpiWriteReg (SI4432_INTERRUPT_ENABLE_1, 0x00);
					// cancel timeout
					DISABLE_MAC_TIMER_INTERRUPT();

					// if use dynamic payload length read out the received packet length and save to PLEN MAC register
					if ((EZMacProReg.name.MCR & 0x04) == 0x04)
					{
						EZMacProReg.name.PLEN = extIntSpiReadReg(SI4432_RECEIVED_PACKET_LENGTH);
					}

					// read out the received payload from the FIFO and save the RxBuffer
					extIntSpiReadFIFO (EZMacProReg.name.PLEN, RxBuffer);

					//call the packet sent callback function
					EZMacPRO_PacketSent();
					// go next state
					extIntGotoNextStateUsingSECR(1);
					break;
				}
				// if the packet is not an acknowledgement
				else
				{
					// clear RX FIFO
					temp8 = extIntSpiReadReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2);
					temp8 |= SI4432_FFCLRRX;
					extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8);
					temp8 &= ~SI4432_FFCLRRX;
					extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8);
					// enable RX
					extIntSetFunction1(SI4432_RXON|SI4432_XTON);
					break;
				}
			}
			// if received a packet with CRC error
			else if ((intStatus1 & SI4432_ICRCERROR)== SI4432_ICRCERROR)
			{
			#ifdef FOUR_CHANNEL_IS_USED
		 		extIntIncrementError (EZMAC_PRO_ERROR_BAD_CRC);
			#endif
				// go next state
				extIntGotoNextStateUsingSECR(1);
				break;
			}
			else
			{
				// clear RX FIFO
				temp8 = extIntSpiReadReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2);
				temp8 |= SI4432_FFCLRRX;
				extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8);
				temp8 &= ~SI4432_FFCLRRX;
				extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8);
				// enable RX
				extIntSetFunction1(SI4432_RXON|SI4432_XTON);
				break;
			}
		#endif
	#endif
		case TX_ERROR_STATE:
			DISABLE_MAC_INTERRUPTS();		// clear EX0 & ET0
			// disable all Si443x interrupt sources
			extIntSpiWriteReg(SI4432_INTERRUPT_ENABLE_1, 0);
			extIntSetEnable2(0);
			// clear interrupts
			extIntSpiReadReg(SI4432_INTERRUPT_STATUS_1);
			extIntSpiReadReg(SI4432_INTERRUPT_STATUS_2);
			STOP_MAC_TIMER();				// stop Timer
			CLEAR_MAC_TIMER_INTERRUPT();	// clear flag
			CLEAR_MAC_EXT_INTERRUPT();
			if (EZMacProReg.name.DTR == 0)	// if the rev V2 chip is used
				// this register setting is need for good current consumption in Idle mode (only rev V2)
				extIntSpiWriteReg (SI4432_CRYSTAL_OSCILLATOR_CONTROL_TEST, SI4432_BUFOVR);
			break;

	#ifdef TRANSCEIVER_OPERATION
		case TX_ERROR_CHANNEL_BUSY:
			break;
	#endif
	}
}
#endif

//------------------------------------------------------------------------------------------------
// Function Name
//	extIntRX_StateMachine()
//
// Return Value : None
// Parameters	: U8 state - RX state, least significant nibble of MSR
//
// This function implements the detailed RX state machine. The state machine is implemented
// using switch...case... statements. For efficient compilation, the states are a series
// of sequential continuous integers (no missing values). The states are enumerated in the
// EZMacPro.h header file.
//
// Conditional compile time switches remove the unused RX states if LBT, ACK, or packet
// forwarding is not supported. These states are also removed from the enumeration at the
// begriming or end of the list.
//
// Conditional compile time switches remove this entire function for the Transmitter only build.
//
//-----------------------------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY_OPERATION
void	extIntRX_StateMachine(U8 msr, U8 intStatus1, U8 intStatus2)
{
	U8 temp8;

	#ifdef ANTENNA_DIVERSITY_ENABLED
	U8 rssi1,rssi2;
	#endif
	switch (msr)
	{
	#ifdef FOUR_CHANNEL_IS_USED
		case RX_STATE_FREQUENCY_SEARCH:
			//if the preamble vaild interrupt is occured
			if (intStatus2 & SI4432_IPREAVAL)
			{	// start timer with sync word timeout
				extIntTimeout(TimeoutSyncWord);
				// go to the next state
				EZMacProReg.name.MSR = RX_STATE_BIT | RX_STATE_WAIT_FOR_SYNC;
				// enable SWDET interrupt
				extIntSetEnable2(SI4432_ENSWDET);
				ENABLE_MAC_INTERRUPTS();
			}
			break;
	#endif

	#ifdef MORE_CHANNEL_IS_USED
		case RX_STATE_FREQUENCY_SEARCH:
			//if the preamble valid interrupt is occured
			if (intStatus2 & SI4432_IPREAVAL)
			{
				//Disable the receiver
					extIntSetFunction1(SI4432_XTON);
				//Enable the receiver
					extIntSetFunction1(SI4432_XTON|SI4432_RXON);
				// start timer with channel search timeout
				extIntTimeout(TimeoutChannelSearch);
					ENABLE_MAC_TIMER_INTERRUPT();
				//go to the next state
				EZMacProReg.name.MSR = RX_STATE_BIT | RX_STATE_WAIT_FOR_PREAMBLE;
			}
			break;

		case RX_STATE_WAIT_FOR_PREAMBLE:
			if (intStatus2 & SI4432_IPREAVAL)
			{
				// start timer with sync word timeout
				extIntTimeout(TimeoutSyncWord);
				EZMacProReg.name.MSR = RX_STATE_BIT | RX_STATE_WAIT_FOR_SYNC;
				ENABLE_MAC_INTERRUPTS();
				// enable SWDET interrupt
				extIntSetEnable2(SI4432_ENSWDET);
				break;
			}
			break;
	#endif

		case RX_STATE_WAIT_FOR_SYNC:
			//if sync word detect interrupt is occured
			if (intStatus2 & SI4432_ISWDET)
			{
				DISABLE_MAC_TIMER_INTERRUPT();
				// disable interrupt enable 2
				extIntSetEnable2(0x00);
	#ifndef ANTENNA_DIVERSITY_ENABLED
				//read out the RSSI value
				EZMacProRSSIvalue = extIntSpiReadReg(SI4432_RECEIVED_SIGNAL_STRENGTH_INDICATOR);
	#else
				//read out the RSSI values
					rssi1 = extIntSpiReadReg(SI4432_ANTENNA_DIVERSITY_REGISTER_1);
					rssi2 = extIntSpiReadReg(SI4432_ANTENNA_DIVERSITY_REGISTER_2);
				//determine the bigger
					if (rssi1 > rssi2)
					{
					Selected_Antenna = 1;
					EZMacProRSSIvalue = rssi1;
				}
				else
				{
					Selected_Antenna = 2;
					EZMacProRSSIvalue = rssi2;
				}
	#endif //ANTENNA_DIVERSITY_ENABLED
				//call the syn word received callback function
				EZMacPRO_SyncWordReceived();
				//clear the receive status variable, because a new packet is coming
				//and set the frequency channel ID
				EZMacProReceiveStatus = 0x00;
	#ifdef FOUR_CHANNEL_IS_USED
				//search disable
				if ((EZMacProReg.name.RCR & 0x04) == 0x00)
					EZMacProCurrentChannel = EZMacProReg.name.FSR & 0x03;
				//store the current freq. channel
				EZMacProReg.name.RFSR = EZMacProCurrentChannel;
	#endif
	#ifdef MORE_CHANNEL_IS_USED
				//store the current freq. channel
				EZMacProReg.name.RFSR = SelectedChannel;
	#endif

				// start timer with rx packet timeout
				extIntTimeout(TimeoutRX_Packet);
				ENABLE_MAC_INTERRUPTS();
				// go to the next state
				EZMacProReg.name.MSR = RX_STATE_BIT | RX_STATE_WAIT_FOR_PACKET;
				// Enable SI4432_ENPKVALID and SI4432_ENCRCERROR interrupts
				extIntSpiWriteReg(SI4432_INTERRUPT_ENABLE_1, SI4432_ENPKVALID|SI4432_ENCRCERROR);
			}
			break;

		case RX_STATE_WAIT_FOR_PACKET:
			//if valid packet receive
			if ((intStatus1 & SI4432_IPKVALID)== SI4432_IPKVALID)
			{
				//Disable All interrupts
				extIntDisableInterrupts();
				//Disable the receiver
				extIntSetFunction1(SI4432_XTON);

				//if use dynamic payload length read out the received packet length and save to PLEN MAC register
				if (EZMacProReg.name.MCR & 0x04)
				{
					EZMacProReg.name.PLEN = extIntSpiReadReg(SI4432_RECEIVED_PACKET_LENGTH);
				}
				//read out the headers
	#ifdef EXTENDED_PACKET_FORMAT
				if (EZMacProReg.name.MCR & 0x80)
				{	// if CID is used
					EZMacProReg.name.RCTRL = extIntSpiReadReg(SI4432_RECEIVED_HEADER_3);
					EZMacProReg.name.RCID = extIntSpiReadReg(SI4432_RECEIVED_HEADER_2);
					EZMacProReg.name.RSID = extIntSpiReadReg(SI4432_RECEIVED_HEADER_1);
					EZMacProReg.name.DID = extIntSpiReadReg(SI4432_RECEIVED_HEADER_0);
				}
				else
				{
					EZMacProReg.name.RCTRL = extIntSpiReadReg(SI4432_RECEIVED_HEADER_3);
					EZMacProReg.name.RSID = extIntSpiReadReg(SI4432_RECEIVED_HEADER_2);
					EZMacProReg.name.DID = extIntSpiReadReg(SI4432_RECEIVED_HEADER_1);
				}
	#else // STANDARD_PACKET_FORMAT
				if (EZMacProReg.name.MCR & 0x80)
				{	// if CID is used
					EZMacProReg.name.RCID = extIntSpiReadReg(SI4432_RECEIVED_HEADER_3);
					EZMacProReg.name.RSID = extIntSpiReadReg(SI4432_RECEIVED_HEADER_2);
					EZMacProReg.name.DID = extIntSpiReadReg(SI4432_RECEIVED_HEADER_1);
				}
				else
				{
					EZMacProReg.name.RSID = extIntSpiReadReg(SI4432_RECEIVED_HEADER_3);
					EZMacProReg.name.DID = extIntSpiReadReg(SI4432_RECEIVED_HEADER_2);
				}
	#endif
				//if received an ACK message and PF is disable then restart the receiving
				if (((EZMacProReg.name.RCTRL & 0x08) == 0x08) && ((EZMacProReg.name.RCR & 0x80) == 0x00))
				{
					// clear RX FIFO
					temp8 = extIntSpiReadReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2);
					temp8 |= SI4432_FFCLRRX;
					extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8);
					temp8 &= ~SI4432_FFCLRRX;
					extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8);

	#ifdef FOUR_CHANNEL_IS_USED
					extIntSetEnable2 (SI4432_ENPREAVAL);
					if ((EZMacProReg.name.RCR & 0x04) == 0x04)
					{
						//jump to the next channel
						extIntNextRX_Channel();
						// start timer with channel search timeout
						extIntTimeout(TimeoutChannelSearch);
						ENABLE_MAC_TIMER_INTERRUPT();
					}
	#endif
	#ifdef MORE_CHANNEL_IS_USED
					//enable the preamble valid interrupt
					extIntSetEnable2(SI4432_ENPREAVAL);
					//select the first freq. channel
					SelectedChannel = 0;
					extIntSpiWriteReg (SI4432_FREQUENCY_HOPPING_CHANNEL_SELECT,EZMacProReg.array[FR0+SelectedChannel]);
					// start timer with channel search timeout
					extIntTimeout(TimeoutChannelSearch);
					ENABLE_MAC_TIMER_INTERRUPT();
	#endif
					//go to the next state
					EZMacProReg.name.MSR = RX_STATE_BIT | RX_STATE_FREQUENCY_SEARCH;
					ENABLE_MAC_EXT_INTERRUPT();
					//start receiver
					extIntSetFunction1 (SI4432_RXON | SI4432_XTON);
					break;
				}

				//read out the received payload from the FIFO and save to RxBuffer
				extIntSpiReadFIFO (EZMacProReg.name.PLEN, RxBuffer);

				/* If the packet is meant to me. */
				if (!extIntHeaderError())
				{
	#ifdef STANDARD_PACKET_FORMAT
					//save the RSSI value to RSSI Mac register
					EZMacProReg.name.RSSI = EZMacProRSSIvalue;
					//save the receive status to the RSR Mac register
					EZMacProReg.name.RSR = EZMacProReceiveStatus;
					/* Call PacketReceived callback with RSSI value. */
					EZMacPRO_PacketReceived(EZMacProRSSIvalue);
					// all done use SECR to determine next state
					extIntGotoNextStateUsingSECR(0);
					break;
	#endif
	#ifdef EXTENDED_PACKET_FORMAT
		#ifndef RECEIVER_ONLY_OPERATION
					// Is an ACK requested and the Sender ID isn't a broadcast address
					if (((EZMacProReg.name.RCTRL & 0x04)	== 0x04) && ( EZMacProReg.name.RSID != 255))
					{
			#ifndef B1_ONLY
						if (EZMacProReg.name.DTR == 0) //if rev V2 chip is used
							//set the TX deviation(only V2)
							extIntSpiWriteReg (SI4432_FREQUENCY_DEVIATION, TX_Freq_dev);
			#endif

			#ifdef ANTENNA_DIVERSITY_ENABLED
				#ifndef B1_ONLY
						//if revision V2 or A0 chip is used
						if ((EZMacProReg.name.DTR == 0x00) || (EZMacProReg.name.DTR == 0x01))
						{
							//switch OFF the internal algorithm
							temp8 = extIntSpiReadReg(SI4432_OPERATING_AND_FUNCTION_CONTROL_2);
							extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8 & 0x7F);

							//select the proper transmit antenna according to Selected_Antenna value
							if ( Selected_Antenna == 1 )
							{
								//select antenna 1
								extIntSpiWriteReg(SI4432_GPIO1_CONFIGURATION, 0x1D);
								extIntSpiWriteReg(SI4432_GPIO2_CONFIGURATION, 0x1F);
							}
							else
							{
								//select antenna 2
								extIntSpiWriteReg(SI4432_GPIO1_CONFIGURATION, 0x1F);
								extIntSpiWriteReg(SI4432_GPIO2_CONFIGURATION, 0x1D);
							}
						}
				#endif
			#endif


			#ifndef PACKET_FORWARDING_SUPPORTED
						//set the ACK packet preamble to 4 byte if the packet forwarding not complied
						PreamRegValue = extIntSpiReadReg(SI4432_PREAMBLE_LENGTH);
				#ifdef FOUR_CHANNEL_IS_USED
						extIntSpiWriteReg(SI4432_PREAMBLE_LENGTH, 0x08);
				#endif
				#ifdef MORE_CHANNEL_IS_USED
						temp8 = extIntSpiReadReg(SI4432_HEADER_CONTROL_2);
						extIntSpiWriteReg(SI4432_HEADER_CONTROL_2, (temp8 & 0xFE));
						extIntSpiWriteReg(SI4432_PREAMBLE_LENGTH, 0x08);
				#endif
			#endif
						//set the ACK packet
						// the transmit registers are volatile and need to be restored by the transmit function
						temp8 = (EZMacProReg.name.MCR >>3) & 0x03;

						if (EZMacProReg.name.MCR & 0x80)
						{	// if CID is used
							// set the control byte of the ACK packet( clear ACKREQ bit, set ACK bit)
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_3, ((EZMacProReg.name.RCTRL & ~0x04) | 0x08 | temp8));
							// copy CID from RCID
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_2, EZMacProReg.name.RCID);
							// set Sender ID to Self ID
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_1, EZMacProReg.name.SFID);
							// set DID to the Received SID
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_0, EZMacProReg.name.RSID);
						}
						else
						{
							//set the control byte of the ACK packet( clear ACKREQ bit, set ACK bit)
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_3, ((EZMacProReg.name.RCTRL & ~0x04) | 0x08 | temp8));
							// set Sender ID to Self ID
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_2, EZMacProReg.name.SFID);
							// set DID to the Received SID
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_1, EZMacProReg.name.RSID);
						}

						//clear TX FIFO
						temp8 = extIntSpiReadReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2);
						temp8 |= SI4432_FFCLRTX;
						extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8);
						temp8 &= ~SI4432_FFCLRTX;
						extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8);

						//if dynamic payload length mode is set
						if (EZMacProReg.name.MCR & 0x04)
						{
							//set the payload length to default
							AckBufSize = ACK_PAYLOAD_DEFAULT_SIZE;
							extIntSpiWriteReg(SI4432_TRANSMIT_PACKET_LENGTH,AckBufSize);

							//clear AckBuffer
							for (temp8 = 0; temp8 < AckBufSize; temp8++)
								AckBuffer[temp8] = 0x00;

							//callback to customise Ack Packet payload
							EZMacPRO_AckSending();

							//Fill FIFO from AckBuffer
							for (temp8 = 0; temp8 < AckBufSize; temp8++)
								extIntSpiWriteReg(SI4432_FIFO_ACCESS,AckBuffer[temp8]);
						}
						else //if static payload length mode is set
						{
							//set the fix payload length
							//AckBufSize = EZMacProReg.name.PLEN;
							//extIntSpiWriteReg(SI4432_TRANSMIT_PACKET_LENGTH,AckBufSize);
							extIntSpiWriteReg(SI4432_TRANSMIT_PACKET_LENGTH,EZMacProReg.name.PLEN);

							//if fix packet length is greater than ack buffer size
							if (EZMacProReg.name.PLEN > ACK_BUFFER_SIZE)
							{
								//Clear AckBuffer
								for (temp8=0; temp8 < ACK_BUFFER_SIZE; temp8++)
									AckBuffer[temp8] = 0x00;

								//Callback to customise Ack Packet payload
								EZMacPRO_AckSending();

								//Fill FIFO from AckBuffer
								for (temp8 = 0; temp8 < ACK_BUFFER_SIZE; temp8++)
									extIntSpiWriteReg(SI4432_FIFO_ACCESS,AckBuffer[temp8]);
								for (temp8 = ACK_BUFFER_SIZE; temp8 < EZMacProReg.name.PLEN; temp8++)
									extIntSpiWriteReg(SI4432_FIFO_ACCESS,0x00);

							}
							else//if fix packet length is smaller or equal than ack buffer size
							{
								//Clear AckBuffer
								for (temp8=0;temp8 < EZMacProReg.name.PLEN;temp8++)
									AckBuffer[temp8] = 0x00;

								//Callback to customise Ack Packet payload
								EZMacPRO_AckSending();

								//Fill FIFO from AckBuffer
								for (temp8 = 0; temp8 < EZMacProReg.name.PLEN; temp8++)
									extIntSpiWriteReg(SI4432_FIFO_ACCESS,AckBuffer[temp8]);
							}

						}

						// enable ENPKSENT bit
						extIntSpiWriteReg(SI4432_INTERRUPT_ENABLE_1, SI4432_ENPKSENT);

						// go to the next state
						EZMacProReg.name.MSR = RX_STATE_BIT | RX_STATE_WAIT_FOR_SEND_ACK;

						// start timer with transmit packet timeout
						extIntTimeout(TimeoutTX_Packet);
						ENABLE_MAC_INTERRUPTS();
						// enable transmit
						extIntSetFunction1(SI4432_TXON | SI4432_XTON);
						break;
					}
					/* No ack requested, or the sender ID is the broadcast address. */
					else
					{
						//save the RSSI value to RSSI Mac register
						EZMacProReg.name.RSSI = EZMacProRSSIvalue;
						//save the receive status to the RSR Mac register
						EZMacProReg.name.RSR = EZMacProReceiveStatus;
						/* Call PacketReceived callback with RSSI value. */
						EZMacPRO_PacketReceived(EZMacProRSSIvalue);
						// all done use SECR to determine next state
						extIntGotoNextStateUsingSECR(0);
						break;
					}
		#endif //RECEIVER_ONLY_OPERATION
	#endif //EXTENDED_PACKET_FORMAT
				}
	#ifdef PACKET_FORWARDING_SUPPORTED
				else // not for me
				{
					// does it need forwarded?
					if (extIntPacketNeedsForwarding())
					{
		#ifndef B1_ONLY
						if (EZMacProReg.name.DTR == 0) //if rev V2 chip is used
						//set the TX deviation (only rev V2)
						extIntSpiWriteReg (SI4432_FREQUENCY_DEVIATION, TX_Freq_dev);
		#endif //B1_ONLY
		#ifdef ANTENNA_DIVERSITY_ENABLED
				#ifndef B1_ONLY
						//if revision V2 or A0 chip is used
						if ((EZMacProReg.name.DTR == 0x00) || (EZMacProReg.name.DTR == 0x01))
						{
							//switch OFF the internal algorithm
							temp8 = extIntSpiReadReg(SI4432_OPERATING_AND_FUNCTION_CONTROL_2);
							extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8 & 0x7F);

							//select the transmit antenna according to the Selected_Antenna value
							if ( Selected_Antenna == 1 )
							{
								//select antenna 1
								extIntSpiWriteReg(SI4432_GPIO1_CONFIGURATION, 0x1D);
								extIntSpiWriteReg(SI4432_GPIO2_CONFIGURATION, 0x1F);
							}
							else
							{
								//select antenna 2
								extIntSpiWriteReg(SI4432_GPIO1_CONFIGURATION, 0x1F);
								extIntSpiWriteReg(SI4432_GPIO2_CONFIGURATION, 0x1D);
							}
						}
				#endif //B1_ONLY
			#endif //ANTENNA_DIVERSITY_ENABLED
						// call the forward packet callback function
						EZMacPRO_PacketForwarding();
						// decrement radius
						// already checked for non-zero radius
						temp8 = EZMacProReg.name.RCTRL;
						temp8--;
						// write modified RX Header back to TX
						// The transmit registers are volatile and need to be restored by transmit function
						// Only Extended packet format supports forwarding
						// Set the packet headers
						if (EZMacProReg.name.MCR & 0x80)
						{	// if CID is used
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_0, EZMacProReg.name.DID);
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_1, EZMacProReg.name.RSID);
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_2, EZMacProReg.name.RCID);
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_3, temp8);
						}
						else
						{
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_1, EZMacProReg.name.DID);
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_2, EZMacProReg.name.RSID);
							extIntSpiWriteReg(SI4432_TRANSMIT_HEADER_3, temp8);
						}

						// write the transmit packet length back
						extIntSpiWriteReg(SI4432_TRANSMIT_PACKET_LENGTH, EZMacProReg.name.PLEN);
						// write RX packet back to FIFO
						extIntSpiWriteFIFO (EZMacProReg.name.PLEN, RxBuffer);

						// save the RSSI value to RSSI Mac register
						EZMacProReg.name.RSSI = EZMacProRSSIvalue;
						// save the receive status to the RSR Mac register
						EZMacProReg.name.RSR = EZMacProReceiveStatus;
						// Call PacketReceived callback with RSSI value.
						EZMacPRO_PacketReceived(EZMacProRSSIvalue);

						if (EZMacProReg.name.TCR & 0x08)
						{	// LBT enabled
							// Set Listen Before Talk Limit to RSSI threshold register
							extIntSpiWriteReg(SI4432_RSSI_THRESHOLD, EZMacProReg.name.LBTLR);

							// disable all Si443x interrupt enable 1 sources
							extIntSpiWriteReg(SI4432_INTERRUPT_ENABLE_1, 0x00);
							// enable RSSI interrupt
							extIntSetEnable2(SI4432_ENRSSI);

							EZMacProLBT_Retrys = 0;
							BusyLBT = 0;
							//go to the next state
							EZMacProReg.name.MSR = RX_STATE_BIT | RX_STATE_FORWARDING_LBT_START_LISTEN;
							//wait for the fix 0.5ms
							//start timer with fix ETSI LBT timeout
							extIntTimeout(TIMEOUT_LBTI_ETSI);

							//start receiver
							extIntSetFunction1(SI4432_RXON|SI4432_XTON);
							ENABLE_MAC_TIMER_INTERRUPT();
							break;
						}
						else
						{
							// enable ENPKSENT bit
							extIntSpiWriteReg(SI4432_INTERRUPT_ENABLE_1, SI4432_ENPKSENT);
							// enable TX
							extIntSetFunction1(SI4432_TXON|SI4432_XTON);
							// start timer with transmit packet timeout
							extIntTimeout(TimeoutTX_Packet);
							// go to the next state
							EZMacProReg.name.MSR = RX_STATE_BIT | RX_STATE_FORWARDING_WAIT_FOR_TX;
							ENABLE_MAC_TIMER_INTERRUPT();
							break;
						}
					}
				}
	#endif //PACKET_FORWARDING_SUPPORTED

	#ifdef FOUR_CHANNEL_IS_USED
				//not for me, neither needs to be forwarded
				extIntIncrementError(EZMAC_PRO_ERROR_BAD_ADDR);
	#endif //FOUR_CHANNEL_IS_USED
				//save the receive status to the RSR Mac register
				EZMacProReg.name.RSR = EZMacProReceiveStatus;
				//call the packet discarded callback
				EZMacPRO_PacketDiscarded();
				// all done use SECR to determine next state
				extIntGotoNextStateUsingSECR(0);
				break;
			} // end ipvalid
			//if crc error occurred
			else if ((intStatus1 & SI4432_ICRCERROR)== SI4432_ICRCERROR)
			{
	#ifdef FOUR_CHANNEL_IS_USED
				extIntIncrementError(EZMAC_PRO_ERROR_BAD_CRC);
	#endif
				EZMacProReg.name.RSR = EZMacProReceiveStatus;
				EZMacPRO_CRCError();

				// clear RX FIFO
				temp8 = extIntSpiReadReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2);
				temp8 |= SI4432_FFCLRRX;
				extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8);
				temp8 &= ~SI4432_FFCLRRX;
				extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8);
	#ifdef FOUR_CHANNEL_IS_USED
				// enable preamble detect interrupt
				extIntSetEnable2 (SI4432_ENPREAVAL);

				if ((EZMacProReg.name.RCR & 0x04) == 0x04)
				{
					//jump to the next channel
					extIntNextRX_Channel();
					//start timer with channel search timeout
					extIntTimeout(TimeoutChannelSearch);
					ENABLE_MAC_TIMER_INTERRUPT();
				}
	#endif

	#ifdef MORE_CHANNEL_IS_USED
				//enable the preamble detect interrupt
				extIntSetEnable2(SI4432_ENPREAVAL);
				//select the nex channel
				SelectedChannel = 0;
				extIntSpiWriteReg (SI4432_FREQUENCY_HOPPING_CHANNEL_SELECT,EZMacProReg.array[FR0+SelectedChannel]);
				// start timer with channel search timeout
				extIntTimeout(TimeoutChannelSearch);
				ENABLE_MAC_TIMER_INTERRUPT();
	#endif
				//go to the next state
				EZMacProReg.name.MSR = RX_STATE_BIT | RX_STATE_FREQUENCY_SEARCH;
				ENABLE_MAC_EXT_INTERRUPT();
				//start receiver
				extIntSetFunction1 (SI4432_RXON | SI4432_XTON);
			}
			break;

	#ifdef EXTENDED_PACKET_FORMAT
		#ifndef RECEIVER_ONLY_OPERATION
		case RX_STATE_WAIT_FOR_SEND_ACK:
			//if packet sent interrupt is occured
			if ((intStatus1 & SI4432_IPKSENT) == SI4432_IPKSENT)
			{
				//Disable interrupts
				extIntSpiWriteReg (SI4432_INTERRUPT_ENABLE_1, 0x00);
				// cancel timeout
				DISABLE_MAC_TIMER_INTERRUPT();
				//save the RSSI value to RSSI Mac register
				EZMacProReg.name.RSSI = EZMacProRSSIvalue;
				//save the receive status to the RSR Mac register
				EZMacProReg.name.RSR = EZMacProReceiveStatus;
				//call Packet received call back function
				EZMacPRO_PacketReceived(EZMacProRSSIvalue);
			#ifdef ANTENNA_DIVERSITY_ENABLED
				#ifndef B1_ONLY
				//if revision V2 or A0 chip is used
				if ((EZMacProReg.name.DTR == 0x00) || (EZMacProReg.name.DTR == 0x01))
				{
					//switch BACK the internal algorithm
					temp8 = extIntSpiReadReg(SI4432_OPERATING_AND_FUNCTION_CONTROL_2);
					extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8 | 0x80);
					//the gpios control the rf chip automatically
					extIntSpiWriteReg(SI4432_GPIO1_CONFIGURATION, 0x17);
					extIntSpiWriteReg(SI4432_GPIO2_CONFIGURATION, 0x18);
				}
				#endif
			#endif

			#ifndef PACKET_FORWARDING_SUPPORTED
				//set BACK the preamble length
				#ifdef FOUR_CHANNEL_IS_USED
				extIntSpiWriteReg(SI4432_PREAMBLE_LENGTH, PreamRegValue);
				#endif
				#ifdef MORE_CHANNEL_IS_USED
				temp8 = extIntSpiReadReg(SI4432_HEADER_CONTROL_2);
				extIntSpiWriteReg(SI4432_HEADER_CONTROL_2, (temp8 | 0x01));
				extIntSpiWriteReg(SI4432_PREAMBLE_LENGTH, PreamRegValue);
				#endif
			#endif
				// all done use SECR to determine next state
				extIntGotoNextStateUsingSECR(0);
				break;
			}

			if (intStatus2 & SI4432_IWUT)
			{
				EZMacProReg.name.MSR = (RX_STATE_BIT | RX_ERROR_STATE);
				//Call the Error state entered callback.
				EZMacPRO_StateErrorEntered();
			}
			break;
		#endif
	#endif

	#ifdef PACKET_FORWARDING_SUPPORTED
		#ifdef TRANSCEIVER_OPERATION
		case RX_STATE_FORWARDING_LBT_START_LISTEN:
		case RX_STATE_FORWARDING_LBT_LISTEN:
		case RX_STATE_FORWARDING_LBT_RANDOM_LISTEN:
			// if RSSI interrupt is occured set the LBT flag
			if (intStatus2 & SI4432_IRSSI)
			{
				BusyLBT = 1;
			}
			break;

		#endif
		case RX_STATE_FORWARDING_WAIT_FOR_TX:
			//if packet sent interrupt is occured
			if ((intStatus1 & SI4432_IPKSENT) == SI4432_IPKSENT)
			{
				DISABLE_MAC_INTERRUPTS();
		#ifdef FOUR_CHANNEL_IS_USED
				// disable 1 interrupts
				extIntSpiWriteReg(SI4432_INTERRUPT_ENABLE_1, 0x00);
				extIntSetEnable2(SI4432_ENPREAVAL);
			#ifdef ANTENNA_DIVERSITY_ENABLED
				#ifndef B1_ONLY
				//if revision V2 or A0 chip is used
				if ((EZMacProReg.name.DTR == 0x00) || (EZMacProReg.name.DTR == 0x01))
				{
					//switch BACK the internal algorithm
					temp8 = extIntSpiReadReg(SI4432_OPERATING_AND_FUNCTION_CONTROL_2);
					extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8 | 0x80);
					//the gpios control the rf chip automatically
					extIntSpiWriteReg(SI4432_GPIO1_CONFIGURATION, 0x17);
					extIntSpiWriteReg(SI4432_GPIO2_CONFIGURATION, 0x18);
				}
				#endif
			#endif
				//if search is enabled
				if ((EZMacProReg.name.RCR & 0x04) == 0x04)
				{
					//jump to the next channel
					extIntNextRX_Channel();
					// start timer with channel search timeout
					extIntTimeout(TimeoutChannelSearch);
					ENABLE_MAC_TIMER_INTERRUPT();
				}
		#endif
		#ifdef MORE_CHANNEL_IS_USED
				//enable the preamble valid interrupt
				extIntSetEnable2(SI4432_ENPREAVAL);
				//select the next channel
				SelectedChannel = 0;
				extIntSpiWriteReg (SI4432_FREQUENCY_HOPPING_CHANNEL_SELECT,EZMacProReg.array[FR0+SelectedChannel]);
				// start timer with channel search timeout
				extIntTimeout(TimeoutChannelSearch);
				ENABLE_MAC_TIMER_INTERRUPT();
		#endif
				//go to the next state
				EZMacProReg.name.MSR = RX_STATE_BIT | RX_STATE_FREQUENCY_SEARCH;
				ENABLE_MAC_EXT_INTERRUPT();
				//start receiver
				extIntSetFunction1(SI4432_RXON | SI4432_XTON);
			}
			break;
	#endif // PACKET_FORWARDING_SUPPORTED

		case RX_ERROR_STATE:
			DISABLE_MAC_INTERRUPTS();		// clear EX0 & ET0
			// disable all Si443x interrupt sources
			extIntSpiWriteReg(SI4432_INTERRUPT_ENABLE_1, 0x00);
			extIntSetEnable2(0x00);
			// clear interrupts
			extIntSpiReadReg(SI4432_INTERRUPT_STATUS_1);
			extIntSpiReadReg(SI4432_INTERRUPT_STATUS_2);
			STOP_MAC_TIMER();				// stop Timer
			CLEAR_MAC_TIMER_INTERRUPT();	 // clear flag
			CLEAR_MAC_EXT_INTERRUPT();
			if (EZMacProReg.name.DTR == 0)//if rev V2 chip is used
			// this register setting is need for good current consumption in Idle mode (only rev V2)
			extIntSpiWriteReg (SI4432_CRYSTAL_OSCILLATOR_CONTROL_TEST, SI4432_BUFOVR);
			break;

		default:
			break;
	}	// end switch
}
#endif // TRANSMITTER_ONLY_OPERATION

//------------------------------------------------------------------------------------------------
// Function Name
//	extIntDisableInterrupts()
//
// Return Value : None
// Parameters	: None
//
// This function is used to disable all interrupts at the completion of a state. Using it saves
// a few bytes of code.
//-----------------------------------------------------------------------------------------------
void extIntDisableInterrupts (void)
{
	DISABLE_MAC_TIMER_INTERRUPT();		// disable timer since no subsequent timeout is used
	extIntSpiWriteReg(SI4432_INTERRUPT_ENABLE_1, 0);	// clear Interrupt Enable 1 register
	extIntSetEnable2 (0);								// clear Interrupt Enable 2 register
}

//------------------------------------------------------------------------------------------------
// Function Name
//	extIntNextRX_Channel()
//
// Return Value : None
// Parameters	: None
//
// Notes:
//
// This function will advance to the next channel. The channel is incremented and then checked
// against the frequency mask in the MAC RCR register. Setting all mask bits is not permitted.
//
// This function is not included for the Transmitter only configuration.
//
//-----------------------------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY_OPERATION
void extIntNextRX_Channel (void)
{
	U8	mask;
	U8	n;

	n = EZMacProCurrentChannel;

	//initialize mask
	mask = 0x08;
	mask <<= n;

	do
	{
		n++;							 // increment n
		n &= 0x03;						// wrap modulo 4
		mask <<=1 ;						// shift mask left
		if (mask == 0x80) mask = 0x08;	// wrap mask to bits 3-6

		// continue until unmasked bit is found
	} while ((EZMacProReg.name.RCR & mask) == mask);

	extIntSpiWriteReg (SI4432_FREQUENCY_HOPPING_CHANNEL_SELECT,EZMacProReg.array[FR0+n]);
	EZMacProCurrentChannel = n;
}
#endif // TRANSMITTER_ONLY_OPERATION not defined
//------------------------------------------------------------------------------------------------
// Function Name
//	extIntGotoNextStateUsingSECR()
//
// Return Value : None
// Parameters	: U8 tx - use SATX bits if non-zero
//
// Notes:
//
// This function is used to put the radio into a programmable state depending on the contents
// of the MAC SECR register. After transmit the SATX bits are used. After RX the SARX bits are
// used.
//
// A U8 (unsigned char) is used for the boolean tx parameter instead of a bit, because a U8 is
// passed in a register.
//
//-----------------------------------------------------------------------------------------------
void extIntGotoNextStateUsingSECR(U8 tx)
{
	U8 next;

#ifndef TRANSMITTER_ONLY_OPERATION
	U8 temp8;
#endif

	//Disable All interrupts
	extIntDisableInterrupts();
#ifndef TRANSMITTER_ONLY_OPERATION
	#ifndef B1_ONLY
	if (EZMacProReg.name.DTR == 0) //if rev V2 chip is used
		//set the RX deviation in all case (only rev V2)
		extIntSpiWriteReg (SI4432_FREQUENCY_DEVIATION, RX_Freq_dev);
	#endif
#endif
	//determine the next state
	if (tx)
	{	//the next state after transmit
		next = EZMacProReg.name.SECR >> 6;
	}
	else
	{	//the next state after receive
		next =	(EZMacProReg.name.SECR >> 4) & 0x03;
	}

	if (next == 0)
	{	//go to sleep mode
		// stop XTAL ( TX, RX, PLLON)
		extIntSetFunction1 (0x00);
		EZMacProReg.name.MSR = EZMAC_PRO_SLEEP;
		//Call the Sleep state entered callback.
		EZMacPRO_StateSleepEntered();
	}
#ifndef TRANSMITTER_ONLY_OPERATION
	else if (next == 2)
	{	//go to RX mode

		// clear RX FIFO
		temp8 = extIntSpiReadReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2);
		temp8 |= SI4432_FFCLRRX;
		extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8);
		temp8 &= ~SI4432_FFCLRRX;
		extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8);

	#ifdef FOUR_CHANNEL_IS_USED
		// enable preamble detect interrupt
		extIntSetEnable2 (SI4432_ENPREAVAL);

		if ((EZMacProReg.name.RCR & 0x04) == 0x04)
		{
		 //jump to the next channel
		 extIntNextRX_Channel();
		 // start timer with channel search timeout
		 extIntTimeout(TimeoutChannelSearch);
		 ENABLE_MAC_TIMER_INTERRUPT();
		}
	#endif

	#ifdef MORE_CHANNEL_IS_USED
		//enable the preamble detect interrupt
		extIntSetEnable2(SI4432_ENPREAVAL);
		//select the nex channel
		SelectedChannel = 0;
		extIntSpiWriteReg (SI4432_FREQUENCY_HOPPING_CHANNEL_SELECT,EZMacProReg.array[FR0+SelectedChannel]);
		// start timer with channel search timeout
		extIntTimeout(TimeoutChannelSearch);
		ENABLE_MAC_TIMER_INTERRUPT();
	#endif
		//go to the next state
		EZMacProReg.name.MSR = RX_STATE_BIT | RX_STATE_FREQUENCY_SEARCH;
		ENABLE_MAC_EXT_INTERRUPT();
		//start receiver
		extIntSetFunction1 (SI4432_RXON | SI4432_XTON);
	}
#endif // TRANSMITTER_ONLY_OPERATION
	else	//	default
	{
		// go to Idle mode
		// disable RX & TX
		extIntSetFunction1 (SI4432_XTON);
#ifndef B1_ONLY
		if (EZMacProReg.name.DTR == 0)//if rev V2 chip is used
			//this register setting is need for good current consumption in Idle mode (only rev V2)
			extIntSpiWriteReg (SI4432_CRYSTAL_OSCILLATOR_CONTROL_TEST, SI4432_BUFOVR);
#endif
#ifdef ANTENNA_DIVERSITY_ENABLED
	#ifndef B1_ONLY
		//if revision V2 or A0 chip is used
		if (EZMacProReg.name.DTR == 0x00 || EZMacProReg.name.DTR == 0x01)
		{

			//switch BACK the internal algorithm
			temp8 = extIntSpiReadReg(SI4432_OPERATING_AND_FUNCTION_CONTROL_2);
			extIntSpiWriteReg (SI4432_OPERATING_AND_FUNCTION_CONTROL_2, temp8 | 0x80);
			//the gpios control the rf chip automatically
			extIntSpiWriteReg(SI4432_GPIO1_CONFIGURATION, 0x17);
			extIntSpiWriteReg(SI4432_GPIO2_CONFIGURATION, 0x18);
		}
	#endif
#endif //ANTENNA_DIVERSITY_ENABLED

		EZMacProReg.name.MSR = EZMAC_PRO_IDLE;
		//Call the Idle state entered callback function.
		EZMacPRO_StateIdleEntered();
	}
}

//------------------------------------------------------------------------------------------------
// Function Name
//	extIntIncrementError()
//
// Return Value : None
// Parameters	: U8 mask - SECR error mask
//
// Notes:
//
// This function will increment the appropriate error counter register if the error type is
// enabled in the MAC SECR register. The error codes for the SECR register are defined in the
// EZMacPro.h header file. First the error code is compared to the error mask in the SECR
// register. The counting is enabled for the particular type of error, the error count is
// incremented. The error count corresponding to the current frequency is incremented.
//
// This function is not included for the Transmitter only configuration.
//
//------------------------------------------------------------------------------------------------
#ifdef FOUR_CHANNEL_IS_USED
	#ifndef TRANSMITTER_ONLY_OPERATION
void extIntIncrementError (U8 m)
{
	m &= 0x0F;							// ignore upper nibble

	if ((EZMacProReg.name.SECR & m) == m )
	{
		if (EZMacProReg.array[EC0+EZMacProCurrentChannel] < 255)
		 EZMacProReg.array[EC0+EZMacProCurrentChannel]++;
	}
}
	#endif
#endif
//------------------------------------------------------------------------------------------------
// Function Name
//
// Return Value :
//	U8 status (U8 used for boolean)
//		1 packet needs forwarded
//		0 packet does not need forwarded
//
// Parameters	: None
//
// Notes:
//
// This function searches the packet forward table to determine if the same packet has
// already been forwarded. If the packet is found in the table, the function returns a 0
// and the packet is not forward. If the packet is not found in the table, the packet is
// added to the table and the function returns a 1 to forward the packet.
//
// A simple FIFO is used for the Forwarded packet table. This provides a basic packet
// forwarding function using a simple table. The FIFO size must be much smaller than the
// number of sequence numbers {16}.
//
// This function is only included if PACKET_FORWARDING_SUPPORTED is defined.
//
//------------------------------------------------------------------------------------------------
#ifdef PACKET_FORWARDING_SUPPORTED
U8 extIntPacketNeedsForwarding (void)
{

	static U8 nextFree = 0;
	U8 i;
	U8 seq;

	// first check the PFEN control bit
	if ((EZMacProReg.name.RCR & 0x80) == 0x80)
	{
		// then check radius
		if ((EZMacProReg.name.RCTRL & 0x03) == 0)
		 return 0;

		seq = EZMacProReg.name.RCTRL >> 4;

		// search table for sid
		for (i = 0; i < FORWARDED_PACKET_TABLE_SIZE; i++)
		{
		 if (ForwardedPacketTable[i].sid == EZMacProReg.name.RSID)
		 {
			if (ForwardedPacketTable[i].seq == seq)
			{
				// received on same channel
				if (ForwardedPacketTable[i].chan == EZMacProCurrentChannel)
				{
					// identical entry already in table
					// do not forward
					return 0;
				}
			}
		 }
		}

		// entry not found in table
		// add to FIFO and forward
		ForwardedPacketTable[nextFree].sid = EZMacProReg.name.RSID;
		ForwardedPacketTable[nextFree].seq = seq;
		ForwardedPacketTable[nextFree].chan = EZMacProCurrentChannel;

		if (nextFree < (FORWARDED_PACKET_TABLE_SIZE - 1))
		 nextFree++;
		else
		 nextFree = 0;

		return 1;
	}
	else
	{
		return 0;
	}
}
#endif
//------------------------------------------------------------------------------------------------
// Function Name
//	extIntSetEnable2()
//
// Return Value : None
// Parameters	: U8 value
//
// Notes:
//
// This function is used instead of extIntSpiWriteReg() when writing to the
// SI4432_INTERRUPT_ENABLE_2 register. This function adds support for the low frequency timer
// and low battery detector, if the build options are defined. If not, the register is written
// directly.
//
//------------------------------------------------------------------------------------------------
void extIntSetEnable2(U8 value)
{
	if ((EZMacProReg.name.LFTMR2 & 0x80))
		value |= SI4432_ENWUT;

	if ((EZMacProReg.name.LBDR & 0x80))
		value |= SI4432_ENLBDI;

	extIntSpiWriteReg(SI4432_INTERRUPT_ENABLE_2, value);
}

//------------------------------------------------------------------------------------------------
// Function Name
//	extIntSetFunction1()
//
// Return Value : None
// Parameters	: U8 value
//
// This function is used instead of extIntSpiWriteReg() when writing to the
// SI4432_OPERATING_AND_FUNCTION_CONTROL_1 register. This function adds support for the
// low frequency timer and low battery detector, if the build options are defined. If not,
// the register is written directly.
//
//------------------------------------------------------------------------------------------------
void extIntSetFunction1(U8 value)
{
	if (EZMacProReg.name.LFTMR2 & 0x80)
	{
		value |= SI4432_ENWT;
		ENABLE_MAC_EXT_INTERRUPT();
	}
	if (EZMacProReg.name.LFTMR2 & 0x40)
		value |= SI4432_X32KSEL;

	if (EZMacProReg.name.LBDR & 0x80)
		value |= SI4432_ENLBD;

	extIntSpiWriteReg(SI4432_OPERATING_AND_FUNCTION_CONTROL_1, value);
}

//------------------------------------------------------------------------------------------------
// Function Name
//	extIntHeaderError()
//
// Return Value : U8 error - (1 error, 0 no error)
// Parameters	: None
//
// Notes:
//
// This function provides SW header filtering for the received packet after receiving the whole
// packet. The function will return a 1 (True) if there is a header error. So the function is
// named for the error.
//
// The filters and error counters are enabled separately. So the filters are checked in the
// order they are received and grouped according to the errors. This function calls the
// BadAddrError function to test the address filters and increments the bad address if
// there is an address error.
//
// Note that the Si4432 HW filters could be used only if the multi-cast filter is not used
// and there is no mechanism to distinguish between CID, DID, and SID header errors.
//
// This function is not included for the Transmitter only configuration.
//
//------------------------------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY_OPERATION
U8 extIntHeaderError(void)
{
	U8 rcid;
	U8 packetLength;

	if (EZMacProReg.name.PFCR & 0x02) // PFEN=1 - promiscuous mode
		return 0;

	if (EZMacProReg.name.MCR & 0x80)
	{	// if CID is used
		// if the Customer ID filter is enabled then CID will be checked
		if ((EZMacProReg.name.PFCR & 0x80) == 0x80)
		{
	#ifdef EXTENDED_PACKET_FORMAT
			rcid = extIntSpiReadReg(SI4432_RECEIVED_HEADER_2);
	#else
			rcid = extIntSpiReadReg(SI4432_RECEIVED_HEADER_3);
	#endif
			if (rcid != EZMacProReg.name.SCID)
			{
	#ifdef FOUR_CHANNEL_IS_USED
				//increment error counter
				extIntIncrementError(EZMAC_PRO_ERROR_BAD_CID);
	#endif
				return 1;
			}
		}
	}

	if (extIntBadAddrError())
	{
	#ifdef FOUR_CHANNEL_IS_USED
		//increment error counter
		extIntIncrementError(EZMAC_PRO_ERROR_BAD_ADDR);
	#endif //FOUR_CHANNEL_IS_USED
		return 1;
	}

	//if the Packet Length filter is enabled then Received Packet length will be checked
	if ((EZMacProReg.name.PFCR & 0x04) == 0x04)
	{
		packetLength = extIntSpiReadReg(SI4432_RECEIVED_PACKET_LENGTH);

		if (packetLength > EZMacProReg.name.MPL)
		{
			return 1;
			// no error count
		}
	}

	return 0;
}
#endif
//------------------------------------------------------------------------------------------------
// Function Name
//	extIntBadAddrError()
//
// Return Value : U8 error - (1 error, 0 no error)
// Parameters	: None
//
// Notes:
//
// This function applies the address (SID & DID) filters and returns a 1 (True) if there is a
// address error.
//
// Note that if packed forwarding is enabled the DID filter is not applied until after the
// packet is received and considered for forwarding, in the externalInt.c module.
//
// This function is not included for the Transmitter only configuration.
//
//------------------------------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY_OPERATION
U8 extIntBadAddrError(void)
{
	U8 rsid;
	U8 rdid;

	if (EZMacProReg.name.PFCR & 0x40)
	{	// if the Sender filter is enabled then SID will be checked
	#ifdef EXTENDED_PACKET_FORMAT
		if (EZMacProReg.name.MCR & 0x80)
			rsid = extIntSpiReadReg(SI4432_RECEIVED_HEADER_1);	// if CID is used
		else
			rsid = extIntSpiReadReg(SI4432_RECEIVED_HEADER_2);
	#else
		if (EZMacProReg.name.MCR & 0x80)
			rsid = extIntSpiReadReg(SI4432_RECEIVED_HEADER_2);	// if CID is used
		else
			rsid = extIntSpiReadReg(SI4432_RECEIVED_HEADER_3);
	#endif
		if ((EZMacProReg.name.SFLT & EZMacProReg.name.SMSK) != (rsid & EZMacProReg.name.SMSK))
			return 1;
	}

	if (EZMacProReg.name.PFCR & 0x20)
	{	// if the Destination filter is enabled then DID will be checked
		// read DID from appropriate header byte
	#ifdef EXTENDED_PACKET_FORMAT
		if (EZMacProReg.name.MCR & 0x80)
			rdid = extIntSpiReadReg(SI4432_RECEIVED_HEADER_0);	// if CID is used
		else
			rdid = extIntSpiReadReg(SI4432_RECEIVED_HEADER_1);
	#else
		if (EZMacProReg.name.MCR & 0x80)
			rdid = extIntSpiReadReg(SI4432_RECEIVED_HEADER_1);	//if CID is used
		else
			rdid = extIntSpiReadReg(SI4432_RECEIVED_HEADER_2);
	#endif
		if (rdid == EZMacProReg.name.SFID)
		{
			EZMacProReceiveStatus |= 0x80;
			return 0;
		}

		if (EZMacProReg.name.PFCR & 0x08)
		{	// if the Broadcast filter is enabled then DID will be checked
			if (rdid == 0xFF)
			{
				EZMacProReceiveStatus |= 0x20;
				return 0;
			}
		}

		if (EZMacProReg.name.PFCR & 0x10)
		{	// if the Multi-cast filter is enabled
			// Multi-cast address mode
			if (EZMacProReg.name.PFCR & 0x01)
			{
				if (rdid == EZMacProReg.name.MCA_MCM)
				{
					EZMacProReceiveStatus |= 0x40;
					return 0;
				}
			}
			else // multi-cast mask mode
			{
				if ((rdid & EZMacProReg.name.MCA_MCM) == (EZMacProReg.name.SFID & EZMacProReg.name.MCA_MCM))
					return 0;
			}
		}
		return 1; // none of above
	}
	return 0; // no DID Error - passes everything else
}
#endif

