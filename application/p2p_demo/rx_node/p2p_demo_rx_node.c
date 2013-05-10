/*!\file p2p_demo_rx_node.c
 * \brief P2P demo code on top of the EZMacPRO stack.
 *
 * \n P2P demo Receiver node code description:
 * \n After power on reset the Receiver node goes into Receive state to wait for
 * \n the first packet. Once that packet is received the Low-Frequency Timer is
 * \n started and the radio is placed into Sleep state. The timer is configured
 * \n to wake the radio up for reception just before the next packet is
 * \n transmitted by the Transmitter node(one per second). Once the packet is
 * \n successfully received its content (packet number) is written on the LCD
 * \n and the radio is placed back into Sleep again. The Low Duty Cycle mode
 * \n allows the radio to stay in low-power state for most of the time and save
 * \n energy.
 * \n LED1 toggles when a packet is received.
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
#include "p2p_demo_rx_node.h"
#include "p2p_demo_rx_menu.h"


                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

SEGMENT_VARIABLE(abRfPayload[5], U8, BUFFER_MSPACE);
SEGMENT_VARIABLE(bPacketLength, U8, APPLICATION_MSPACE);
SEGMENT_VARIABLE(wPacketCounter, U16, APPLICATION_MSPACE);
SEGMENT_VARIABLE(rssiVal, U8, APPLICATION_MSPACE);
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
    U16 wWaitCnt;

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
            vP2P_demo_RxInit();
            /* Go to wait for first packet. */
            DEMO_SR = DEMO_RX_FIRST;
            break;

        case DEMO_RX_FIRST:
            /* First packet received. */
            if(fEZMacPRO_PacketReceived)
            {   /* Clear flag. */
                fEZMacPRO_PacketReceived = 0;
                /* Get packet then place the radio to sleep. */
                vP2P_demo_GetPacketGoToSleep();
                /* Go to Sleep state. */
                DEMO_SR = DEMO_SLEEP;
            }
            break;

        case DEMO_SLEEP:
            /* LFT expired, start waiting for packet. */
            if (fEZMacPRO_LFTimerExpired)
            {   /* Clear flag. */
                fEZMacPRO_LFTimerExpired = 0;
                /* Disable LFT, use the Internal Time Base. */
                EZMacPRO_Reg_Write(LFTMR2, 0x44);
                /* Wake up from Sleep mode. */
                EZMacPRO_Wake_Up();
                /* Wait until device goes to Idle. */
                while (!fEZMacPRO_StateIdleEntered);
                /* Clear State transition flags. */
                fEZMacPRO_StateWakeUpEntered = 0;
                fEZMacPRO_StateIdleEntered = 0;
                /* Start receiving. */
                EZMacPRO_Receive();
              #ifdef SDBC
                /* Display Receive on LCD. */
                vPerformMenu(sMenu_PacketReceive);
              #endif //SDBC
              #ifdef TRACE_ENABLED
                printf("[RX_NODE][DEMO_SLEEP] Woke up.\n");
              #endif //TRACE_ENABLED
                /* Wait until radio is placed to RX. */
                while (!fEZMacPRO_StateRxEntered);
                /* Clear State transition flag. */
                fEZMacPRO_StateRxEntered = 0;
                /* Go to Receive state. */
                DEMO_SR = DEMO_RX;
            }
            break;

        case DEMO_RX:
            /* Wait in Receive state until packet arrives. */
            if(fEZMacPRO_PacketReceived)
            {   /* Clear flag. */
                fEZMacPRO_PacketReceived = 0;
                /* Get packet then place the radio to sleep. */
                vP2P_demo_GetPacketGoToSleep();
                LED1 = !LED1;
                for (wWaitCnt = 0; wWaitCnt < 25000; wWaitCnt++);
                /* Go to back to Sleep state. */
                DEMO_SR = DEMO_SLEEP;
            }
            break;

        default:
            break;
    }
}



void vP2P_demo_RxInit(void)
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
    /* Show TX screen on LCD. */
    vPerformMenu(sMenu_RxScreen);
  #endif //SDBC
    EZMacPRO_Reg_Write(MCR, 0xAC);              // Set data rate to 9.6kbps, DNPL = 1, rad = 1, used 1 channel
    EZMacPRO_Reg_Write(SECR, 0x20);             // State after receive is RX state and state after transmit is Idle state
    EZMacPRO_Reg_Write(TCR, 0xB0);              // LBT Before Talk disabled, Output power: +20 dBm, ACK enable , AFC disable
    EZMacPRO_Reg_Write(RCR, 0x00);              // Search disable
    EZMacPRO_Reg_Write(PFCR, 0xA0);             // Destination address and the custumer filters are enabled
    EZMacPRO_Reg_Write(FR0, 1);                 // set the used frequency channel
    EZMacPRO_Reg_Write(SCID, CUSTOMER_ID);      // set the customer ID to 0x01
    EZMacPRO_Reg_Write(SFID, DEVICE_SELF_ID);   // set the self ID
    LED1 = ILLUMINATE;

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
    /* Display RX screen on LCD. */
    vPerformMenu(sMenu_PacketReceive);
  #endif //SDBC
  #ifdef TRACE_ENABLED
    printf("[RX_NODE][DEMO_BOOT] Startup done.\n");
    printf("[RX_NODE][DEMO_BOOT] Waiting for first packet...\n");
  #endif //TRACE_ENABLED
    /* Wait until radio is placed to RX. */
    while (!fEZMacPRO_StateRxEntered);
    /* Clear State transition flag. */
    fEZMacPRO_StateRxEntered = 0;
}


void vP2P_demo_GetPacketGoToSleep(void)
{
    /* Go to Idle state and read RX buffer. */
    EZMacPRO_Idle();
    /* Wait until device goes to Idle. */
    while (!fEZMacPRO_StateIdleEntered);
    /* Clear State transition flag. */
    fEZMacPRO_StateIdleEntered = 0;
    /* Read out the payload. */
    EZMacPRO_RxBuf_Read(&bPacketLength, &abRfPayload[0]);
    /* Get the data. */
    wPacketCounter = atoi(abRfPayload);
    /* Go to Sleep state. */
    EZMacPRO_Sleep();
  #ifdef SDBC
    /* Update number of received packets on LCD. */
    vPerformMenu(sMenu_RxRefreshCounter);
  #endif //SDBC
  #ifdef TRACE_ENABLED
    printf("[RX_NODE][DEMO_RX] Received packet[rssi:%03bu]. Content:%5u\n", rssiVal, wPacketCounter);
  #endif //TRACE_ENABLED
  #ifdef SDBC
    /* Display Sleep on LCD. */
    vPerformMenu(sMenu_SleepMode);
  #endif //SDBC
  #ifdef TRACE_ENABLED
    printf("[RX_NODE][DEMO_RX] Went to sleep.\n");
  #endif //TRACE_ENABLED
    /* Wait until device goes back to Sleep. */
    while (!fEZMacPRO_StateSleepEntered);
    /* Clear State transition flag. */
    fEZMacPRO_StateSleepEntered = 0;
    /* Set the Low Frequency Timer interval and start it. */
    EZMacPRO_Reg_Write(LFTMR0, 0xC0);       // set the interval to 875ms
    EZMacPRO_Reg_Write(LFTMR1, 0x01);       // set the interval to 875ms
    EZMacPRO_Reg_Write(LFTMR2, 0xC4);       // enable the LFT, use the Internal Time Base
}


