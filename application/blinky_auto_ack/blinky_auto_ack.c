/*!\file blinky_auto_ack.c
 * \brief Wireless blinky sample code using the EZMacPRO stack.
 *
 * \n Auto-acknowledgement sample code description:
 * \n Pressing PB1 transmits a packet and flashes LED1 if the acknowledgement
 * \n is successfully received before timeout. If the stack goes to error state
 * \n LED1 illuminates permanently.
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
                 *              I N C L U D E              *
                 * ======================================= */

#include "..\..\common.h"
#include "blinky_auto_ack.h"



                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

                /* ======================================= *
                 *            C O N S T A N T S            *
                 * ======================================= */

                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

BIT radioBusy;
SEGMENT_VARIABLE(DEMO_SR, U8, SEG_XDATA);


                /* ======================================= *
                 *      L O C A L   F U N C T I O N S      *
                 * ======================================= */

/*!
 * This is the State Machine of the Demo Application.
 */
void StateMachine(void)
{
    U16 i;
    U8 BUFFER_MSPACE pktLen = 3;
    U8 BUFFER_MSPACE payload[13] = {97,98,99,100,101,102,103,104,105,106,107,108,109};

    switch(DEMO_SR)
    {
        case DEMO_SR_STATE_BOOT_BIT:
            /* Initialise board. */
            BoardInit();
            /* Enable global interrupts. */
            ENABLE_GLOBAL_INTERRUPTS();
            /* Initialise EZMacPRO. */
            EZMacPRO_Init();
            /* Wake up from Sleep.*/
            EZMacPRO_Wake_Up();
            /* Configure EZMacPRO. */
            EZMacPRO_Reg_Write(MCR, 0xE4);      // Set data rate to 128kbps, DNPL = 1, and CID is used
            EZMacPRO_Reg_Write(TCR, 0xF0);      // Output power: +20 dBm, ACK request
//            EZMacPRO_Reg_Write(TCR, 0xF8);      // Listen Before Talk enabled, output power: +20 dBm, ACK request
//            EZMacPRO_Reg_Write(LBTLR, 0x78);    // RSSI threshold -60 dB
//            EZMacPRO_Reg_Write(LBTIR, 0x85);    // Time interval
            EZMacPRO_Reg_Write(SECR, 0x50);     // State after Receive is Idle; state after Transmit is Idle
            EZMacPRO_Reg_Write(FSR, 0);         // Select the first frequency channel
            EZMacPRO_Reg_Write(RCR, 0x00);      // Search disabled

            EZMacPRO_Reg_Write(PFCR, 0x20);     // Enable Destination filter
//            EZMacPRO_Reg_Write(PFCR, 0xA0);     // Enable CID and Destination filter
//            EZMacPRO_Reg_Write(PFCR, 0xA2);     // Promiscuous mode, Enable CID and Destination filter

            /* Indicate boot done. */
            LED1 = EXTINGUISH;
            for(i=0; i<65000; i++);
            LED1 = ILLUMINATE;
            for(i=0; i<65000; i++);
            LED1 = EXTINGUISH;
            /* Go to init receive mode. */
            DEMO_SR = DEMO_SR_STATE_INIT_RX_BIT;
            break;


        case DEMO_SR_STATE_INIT_RX_BIT:
            /* Stack is in Idle mode. */
            if(fEZMacPRO_StateIdleEntered)
            {   /* Clear flag. */
                fEZMacPRO_StateIdleEntered = 0;
                /* Configure EZMacPRO. */
                EZMacPRO_Reg_Write(SCID, 0x02);             /* Set Customer ID. */
                EZMacPRO_Reg_Write(SFID, 0x01);             /* Set Self ID. */
                /* Unlock radio. */
                radioBusy = 0;
                /* Receive mode. */
                EZMacPRO_Receive();
                /* Wait until it goes to RX. */
                while(!fEZMacPRO_StateRxEntered);
                fEZMacPRO_StateRxEntered = 0;
                /* Go to transceiver state. */
                DEMO_SR = DEMO_SR_STATE_TRX_BIT;
            }
            break;


        case DEMO_SR_STATE_TRX_BIT:
            /* Button 1 is pushed. If radio is not busy EZMacPRO transmits a
             * packet. State after TX is RX. SFID = 0x02. */
            if (!PB1 && !radioBusy)
            {
                while(PB1 == 0);
                /* Place stack into Idle. */
                EZMacPRO_Idle();
                /* Wait until it goes to Idle. */
                while(!fEZMacPRO_StateIdleEntered);
                fEZMacPRO_StateIdleEntered = 0;
                /* Configure EZMacPRO. */
                EZMacPRO_Reg_Write(SCID, 0xFF);             /* Set Customer ID. */
                EZMacPRO_Reg_Write(SFID, 0x02);             /* Set Self ID. */
                EZMacPRO_Reg_Write(DID, 0x01);              /* Set Destination ID. */
                EZMacPRO_Reg_Write(PLEN, pktLen);           /* Set packet length before sending packet. */
                EZMacPRO_TxBuf_Write(pktLen, &payload[0]);  /* Load the payload into the TX buffer. */
                /* Lock radio. */
                radioBusy = 1;
                /* Send the packet. */
                EZMacPRO_Transmit();
                break;
            }

            /* Listen before talk timeout occurred.
             * Manually back to Idle. */
            if(fEZMacPRO_LBTTimeout)
            {   /* Clear flag. */
                fEZMacPRO_LBTTimeout = 0;
                /* Recover from Error state by entering Idle.*/
                EZMacPRO_Idle();
                /* Go to init receive mode. */
                DEMO_SR = DEMO_SR_STATE_INIT_RX_BIT;
                break;
            }

            /* Packet successfully sent as acknowledgement received.
             * Automatically back to Idle. */
            if(fEZMacPRO_PacketSent)
            {   /* Clear flag. */
                fEZMacPRO_PacketSent = 0;
                /* Indicate packet transmission. */
                LED1 = ILLUMINATE;
                for(i=0; i<65000; i++);
                LED1 = EXTINGUISH;
                /* Go to init receive mode. */
                DEMO_SR = DEMO_SR_STATE_INIT_RX_BIT;
                break;
            }

            /* No acknowledgement received within timeout interval.
             * Automatically back to Idle. */
            if(fEZMacPRO_AckTimeout)
            {   /* Clear flag. */
                fEZMacPRO_AckTimeout = 0;
                /* Go to init receive mode. */
                DEMO_SR = DEMO_SR_STATE_INIT_RX_BIT;
                break;
            }

            /* Packet received and passed enabled filters, auto-acknowledgement
             * will be sent. Automatically back to Idle. */
            if(fEZMacPRO_PacketReceived)
            {   /* Clear flag. */
                fEZMacPRO_PacketReceived = 0;
                /* Read out payload. */
                EZMacPRO_RxBuf_Read(&pktLen, payload);
                /* Indicate packet reception. */
                LED1 = ILLUMINATE;
                for(i=0; i<65000; i++);
                LED1 = EXTINGUISH;
                /* Go to init receive mode. */
                DEMO_SR = DEMO_SR_STATE_INIT_RX_BIT;
                break;
            }

            /* Packet received but did not pass enabled filters. No
             * auto-acknowledgement will be sent. Automatically back to Idle. */
            if(fEZMacPRO_PacketDiscarded)
            {   /* Clear flag. */
                fEZMacPRO_PacketDiscarded = 0;
                /* Go to init receive mode. */
                DEMO_SR = DEMO_SR_STATE_INIT_RX_BIT;
                break;
            }

            /* Any of these errors occurred:
             *  - WAKE_UP_ERROR
             *  - TX_ERROR_STATE
             *  - RX_ERROR_STATE
             * Re-initialise demo: reset EZMacPRO and radio. */
            if(fEZMacPRO_StateErrorEntered)
            {   /* Clear flag. */
                fEZMacPRO_StateErrorEntered = 0;
                /* Indicate error state. */
                LED1 = ILLUMINATE;
                /* No recovery. */
                while(1);
                /* Reset demo state machine. */
                //Demo_Init();
                break;
            }
            break;

        default:
            break;
    }
}


void StateMachine_Init(void)
{
    /* Disable Watchdog. */
    DISABLE_WATCHDOG();
    /* Init Demo State Machine. */
    DEMO_SR = DEMO_SR_STATE_BOOT_BIT;
}

