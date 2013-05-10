/*!\file p2p_demo_fwd_node.c
 * \brief P2P demo code on top of the EZMacPRO stack.
 *
 * \n P2P demo Forwarder node code description:
 * \n After power on reset the Forwarder node goes into Receive state. When a
 * \n packet that is not destined to the node is received, the stack
 * \n automatically forwards (repeats) it.
 * \n LED1 toggles when a packet is forwarded.
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
#include "p2p_demo_fwd_node.h"
#include "p2p_demo_fwd_menu.h"


                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

SEGMENT_VARIABLE(abRfPayload[5], U8, BUFFER_MSPACE);
SEGMENT_VARIABLE(bPacketLength, U8, APPLICATION_MSPACE);
SEGMENT_VARIABLE(wPacketCounter, U16, APPLICATION_MSPACE);
SEGMENT_VARIABLE(DEMO_SR, U8, APPLICATION_MSPACE);

#ifdef TRACE_ENABLED
  SEGMENT_VARIABLE(printBuf[40], U8, APPLICATION_MSPACE);
#endif //TRACE_ENABLED


                /* ======================================= *
                 *      L O C A L   F U N C T I O N S      *
                 * ======================================= */

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
            /* Initialise board. */
            BoardInit();
            /* Enable global interrupts. */
            ENABLE_GLOBAL_INTERRUPTS();
            /* Initialise EZMacPRO. */
            EZMacPRO_Init();
            /* Wait until device goes to Sleep. */
            while (!fEZMacPRO_StateSleepEntered);
            /* Clear State transition flags. */
            fEZMacPRO_StateWakeUpEntered = 0;
            fEZMacPRO_StateSleepEntered = 0;
            /* Point to point demo initialisation. */
            vP2P_demo_FwdInit();
            /* Go to wait for first packet. */
            DEMO_SR = DEMO_FWD;
            break;

        case DEMO_FWD:
            /* Wait in Receive state until packet arrives. */
            if(fEZMacPRO_PacketForwarding)
            {   /* Clear flag. */
                fEZMacPRO_PacketForwarding = 0;
                /* Get packet then place the radio to sleep. */
                vP2P_demo_FwdPacket();
            }
            break;

        default:
            break;
    }
}



void vP2P_demo_FwdInit(void)
{
    /* Configure and start 2sec timeout for Silabs splash screen. */
    EZMacPRO_Reg_Write(LFTMR0, LFTMR0_TIMEOUT_SEC(STARTUP_TIMEOUT));
    EZMacPRO_Reg_Write(LFTMR1, LFTMR1_TIMEOUT_SEC(STARTUP_TIMEOUT));
    EZMacPRO_Reg_Write(LFTMR2, 0x80 | LFTMR2_TIMEOUT_SEC(STARTUP_TIMEOUT));
  #ifdef SDBC
    /* Show Silabs logo, demo name & version. */
    vPerformMenu(sMenu_StartupScreen);
  #endif //SDBC
    /* Wait here until LFT expires. */
    while(!fEZMacPRO_LFTimerExpired);
    /* Clear flag. */
    fEZMacPRO_LFTimerExpired = 0;
    /* Disable LFT. */
    EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_SEC(STARTUP_TIMEOUT));
    /* Init counter. */
    wPacketCounter = 0;
    /* Init packet content. */
    memcpy(&abRfPayload[0], "      ", 6);
  #ifdef SDBC
    /* Show FWD screen on LCD. */
    vPerformMenu(sMenu_FwdScreen);
  #endif //SDBC
    EZMacPRO_Reg_Write(MCR, 0xAC);              // Set data rate to 9.6kbps, DNPL = 1, used 1 channel
    EZMacPRO_Reg_Write(SECR, 0x60);             // State after receive is RX state and state after transmit is Idle state
    EZMacPRO_Reg_Write(TCR, 0xB8);              // LBT Before Talk enabled, Output power: +20 dBm, no ACK, AFC disable
    EZMacPRO_Reg_Write(RCR, 0x80);              // Packet Forwarding enable
    EZMacPRO_Reg_Write(LBTLR, 0x78);            // RSSI threshold -60 dB
    EZMacPRO_Reg_Write(LBTIR, 0x8A);            // Time interval
    EZMacPRO_Reg_Write(PFCR, 0xA0);             // Use Customer ID filter and Destionation ID filter
    EZMacPRO_Reg_Write(FR0, 1);                 // set the used frequency channel
    EZMacPRO_Reg_Write(SCID, CUSTOMER_ID);      // set the customer ID to 0x01
    EZMacPRO_Reg_Write(SFID, DEVICE_SELF_ID);   // set the self ID
    /* Wake up from Sleep mode. */
    EZMacPRO_Wake_Up();
    /* Wait until device goes to Idle. */
    while (!fEZMacPRO_StateIdleEntered);
    /* Clear State transition flags. */
    fEZMacPRO_StateWakeUpEntered = 0;
    fEZMacPRO_StateIdleEntered = 0;
    /* Go to receive state. */
    EZMacPRO_Receive();
  #ifdef SDBC
    /* Display FWD screen on LCD. */
    vPerformMenu(sMenu_FwdScreen);
  #endif //SDBC
  #ifdef TRACE_ENABLED
    printf("[FWD_NODE][DEMO_BOOT] Startup done.\n");
    printf("[FWD_NODE][DEMO_BOOT] Waiting for packet...\n");
  #endif //TRACE_ENABLED
    /* Wait until radio is placed to RX. */
    while (!fEZMacPRO_StateRxEntered);
    /* Clear State transition flag. */
    fEZMacPRO_StateRxEntered = 0;
}


void vP2P_demo_FwdPacket(void)
{
    U16 wWaitCnt;

    /* Read out the payload. */
    EZMacPRO_RxBuf_Read(&bPacketLength, &abRfPayload[0]);
    /* Get the data. */
    wPacketCounter = atoi(abRfPayload);
  #ifdef SDBC
    /* Update number of forwarded packets on LCD. */
    vPerformMenu(sMenu_FwdRefreshCounter);
  #endif //SDBC
  #ifdef TRACE_ENABLED
    printf("[FWD_NODE][DEMO_FWD] Packet received. Packet content:%5u\n", wPacketCounter);
  #endif //TRACE_ENABLED
  #ifdef SDBC
    /* Display Forward on LCD. */
    vPerformMenu(sMenu_PacketForward);
  #endif //SDBC
  #ifdef TRACE_ENABLED
    printf("[FWD_NODE][DEMO_FWD] Packet transmitted. Packet content:%5u\n", wPacketCounter);
  #endif //TRACE_ENABLED
    LED1 = !LED1;
    for (wWaitCnt = 0; wWaitCnt < 50000; wWaitCnt++);
  #ifdef SDBC
    /* Display Receive on LCD. */
    vPerformMenu(sMenu_PacketReceive);
  #endif //SDBC
  #ifdef TRACE_ENABLED
    printf("[FWD_NODE][DEMO_FWD] Went to Receive.\n");
  #endif //TRACE_ENABLED
}


