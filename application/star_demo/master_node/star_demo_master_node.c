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
                 *              I N C L U D E              *
                 * ======================================= */

#include "..\..\common.h"
#include "star_demo_master_node.h"
#include "star_demo_master_menu.h"


                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

SEGMENT_VARIABLE(SlaveInfoTable[MAX_NMBR_OF_SLAVES], SlaveInfoTable_t, APPLICATION_MSPACE);
SEGMENT_VARIABLE(rfPayload, Frame_uu, APPLICATION_MSPACE);
SEGMENT_VARIABLE(packetLength, U8, APPLICATION_MSPACE);
SEGMENT_VARIABLE(rndCounter, U16, APPLICATION_MSPACE);
SEGMENT_VARIABLE(slaveAddr, Addr_t, APPLICATION_MSPACE);
SEGMENT_VARIABLE(rndAddr, Addr_t, APPLICATION_MSPACE);
SEGMENT_VARIABLE(nodeCnt, U8, APPLICATION_MSPACE);
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
            /* Initialise board. */
            BoardInit();
            /* Boot Master Node. */
            MasterNodeBoot();
            break;

        case DEMO_ASSOC:
            /* Associate slaves. */
            MasterNodeAssociate();
            break;

        case DEMO_SU:
            /* Get status updates from slaves. */
            MasterNodeStatusUpdate();
            break;

        case DEMO_SLEEP:
            /* Sleep between cycles. */
            MasterNodeSleep();
            break;

        default:
            break;
    }
}


/*!
 * Boot Master Node.
 */
void MasterNodeBoot(void)
{
    switch(DEMO_SR)
    {
        case DEMO_BOOT_INIT:
            /* Enable global interrupts. */
            ENABLE_GLOBAL_INTERRUPTS();
            /* Initialise EZMacPRO. */
            EZMacPRO_Init();
            /* Wait until device goes to Sleep. */
            while (!fEZMacPRO_StateSleepEntered);
            /* Clear State transition flags. */
            fEZMacPRO_StateWakeUpEntered = 0;
            fEZMacPRO_StateSleepEntered = 0;
            /* Configure and start 2sec timeout for Silabs splash screen. */
            EZMacPRO_Reg_Write(LFTMR0, LFTMR0_TIMEOUT_SEC(STARTUP_TIMEOUT));
            EZMacPRO_Reg_Write(LFTMR1, LFTMR1_TIMEOUT_SEC(STARTUP_TIMEOUT));
            EZMacPRO_Reg_Write(LFTMR2, 0x80 | LFTMR2_TIMEOUT_SEC(STARTUP_TIMEOUT));
          #ifdef SDBC
            /* Show Silabs logo, demo name & version. */
            PerformMenu(sMenu_StartupScreen);
          #endif //SDBC
            /* Wait here until LFT expires. */
            while(!fEZMacPRO_LFTimerExpired);
            /* Clear flag. */
            fEZMacPRO_LFTimerExpired = 0;
            /* Disable LFT. */
            EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_SEC(STARTUP_TIMEOUT));
            /* Star demo Master node initialisation. */
            EZMacPRO_Reg_Write(MCR, 0xAC);              // CIDE=1, DR=9.6kbps, RAD=1, DNPL=1, NFR=0
            EZMacPRO_Reg_Write(SECR, 0x60);             // State after receive is RX state and state after transmit is Idle state
            EZMacPRO_Reg_Write(RCR, 0x00);              // Search disable
            EZMacPRO_Reg_Write(FR0, 0);                 // Set the used frequency channel
            EZMacPRO_Reg_Write(TCR, (0x70|LBT_SWITCH)); // LBT enabled/disabled, Output power: +20 dBm, ACK disable, AFC disable
            EZMacPRO_Reg_Write(LBTLR, 0x78);            // RSSI threshold -60 dB
            EZMacPRO_Reg_Write(LBTIR, 0x8A);            // Time interval
            EZMacPRO_Reg_Write(LBDR, 0x80);             // Enable Low Battery Detect
            EZMacPRO_Reg_Write(PFCR, 0x28);             // Destination address filter is enabled
            EZMacPRO_Reg_Write(SCID, DEMO_MASTER_CID);  // Set customer ID
            EZMacPRO_Reg_Write(SFID, DEMO_MASTER_SFID); // Set self ID
            /* Init slave info table. */
            for (nodeCnt = 0; nodeCnt < MAX_NMBR_OF_SLAVES; nodeCnt++)
            {
                SlaveInfoTable[nodeCnt].associated = NOT_ASSOCIATED;
                SlaveInfoTable[nodeCnt].address.cid = DEMO_MASTER_CID;
                SlaveInfoTable[nodeCnt].address.sfid = nodeCnt+1;
                SlaveInfoTable[nodeCnt].timeout = TIMEOUT_INITIAL_VALUE;
                SlaveInfoTable[nodeCnt].temperature = TEMP_INVALID_VALUE;
                SlaveInfoTable[nodeCnt].voltage = VOLTAGE_INVALID_VALUE;
                SlaveInfoTable[nodeCnt].rssi = RSSI_INITIAL_VALUE;
            }
          #ifdef SDBC
            /* Show main screen. */
            PerformMenu(sMenu_MainScreen);
          #endif //SDBC
          #ifdef TRACE_ENABLED
            printf("[DEMO_BOOT] Master startup done.\n");
          #endif //TRACE_ENABLED
            /* Wake up from Sleep mode. */
            EZMacPRO_Wake_Up();
            /* Wait until device goes to Idle. */
            while (!fEZMacPRO_StateIdleEntered);
            /* Clear State transition flags. */
            fEZMacPRO_StateWakeUpEntered = 0;
            fEZMacPRO_StateIdleEntered = 0;
            /* Go to next state. */
            DEMO_SR = DEMO_ASSOC_BEACON_TX;
            break;

        default:
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
    {   /* Clear flag. */
        fEZMacPRO_LFTimerExpired = 0;
        /* Disable LFT. */
        EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
        /* Go to Idle state. */
        EZMacPRO_Idle();
      #ifdef SDBC
        /* Display Status Update state on LCD. */
        PerformMenu(sMenu_StatusUpdate_State);
      #endif //SDBC
        /* Wait until device goes to Idle. */
        while (!fEZMacPRO_StateIdleEntered);
        /* Clear State transition flag. */
        fEZMacPRO_StateIdleEntered = 0;
      #ifdef TRACE_ENABLED
        printf("[DEMO_ASSOC] Search ended.\n");
        printf("[DEMO_SU] Start to query slaves.\n");
      #endif //TRACE_ENABLED
        /* Reset node counter. */
        nodeCnt = 0;
        /* Go ahead and request status updates. */
        DEMO_SR = DEMO_SU_REQ_TX;
    }

    switch(DEMO_SR)
    {
        case DEMO_ASSOC_BEACON_TX:
            /* Configure and start 200msec timeout for Beacon time frame. */
            EZMacPRO_Reg_Write(LFTMR0, LFTMR0_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
            EZMacPRO_Reg_Write(LFTMR1, LFTMR1_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
            EZMacPRO_Reg_Write(LFTMR2, 0x80 | LFTMR2_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
            /* LED1 indicates the radio is ON. */
            LED1 = ILLUMINATE;
          #ifdef SDBC
            /* Display Associate state on LCD. */
            PerformMenu(sMenu_Associate_State);
          #endif //SDBC
          #ifdef TRACE_ENABLED
            printf("[DEMO_ASSOC] Search for slaves to associate with.\n");
          #endif //TRACE_ENABLED
            /* Configure Beacon Frame. */
            EZMacPRO_Reg_Write(TCR, (0x70|LBT_SWITCH)); // LBT enabled/disabled, Output power: +20 dBm, ACK disable, AFC disable
            EZMacPRO_Reg_Write(DID, DEMO_MASTER_MCAST); // Set Destination ID
            /* Assemble beacon frame. */
            rfPayload.frameUnion.beacon.type = FRAME_BEACON;
            /* Write the packet length and payload to the TX buffer. */
            EZMacPRO_TxBuf_Write(sizeof(FrameBeacon_t), &rfPayload.frameRaw);
            /* Send the packet. */
            EZMacPRO_Transmit();
            /* Wait until device goes to Idle. */
            while (!fEZMacPRO_StateIdleEntered);
            /* Clear State transition flags. */
            fEZMacPRO_StateIdleEntered = 0;
            /* Go to next state. */
            DEMO_SR = DEMO_ASSOC_REQ_RX;
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
             if(fEZMacPRO_PacketReceived)
             {   /* Clear flag. */
                 fEZMacPRO_PacketReceived = 0;
                 /* Free slot in association table. Check message type. */
                 if((nodeCnt = SearchFreeSlotInAssocTable()) != ASSOC_TABLE_FULL)
                 {   /* Read out the payload. */
                     EZMacPRO_RxBuf_Read(&packetLength, &rfPayload.frameRaw);
                     /* Message is association request. */
                     if(rfPayload.frameUnion.assocReq.type == FRAME_ASSOC_REQ)
                     {   /* Save address of the slave. */
                         EZMacPRO_Reg_Read(RCID, &slaveAddr.cid);
                         EZMacPRO_Reg_Read(RSID, &slaveAddr.sfid);
                         /* Go to next state. */
                         DEMO_SR = DEMO_ASSOC_RESP_TX;
                     }
                 }
             }
             break;

         case DEMO_ASSOC_RESP_TX:
             /* Go to Idle state to read Beacon frame response. */
             EZMacPRO_Idle();
             /* Wait until device goes to Idle. */
             while (!fEZMacPRO_StateIdleEntered);
             /* Clear State transition flag. */
             fEZMacPRO_StateIdleEntered = 0;
             /* Send Association response with new address of the node. */
             EZMacPRO_Reg_Write(DID, slaveAddr.sfid);
             /* Assemble Association response frame. */
             rfPayload.frameUnion.assocResp.type = FRAME_ASSOC_RESP;
             rfPayload.frameUnion.assocResp.newAddr.cid = SlaveInfoTable[nodeCnt].address.cid;
             rfPayload.frameUnion.assocResp.newAddr.sfid = SlaveInfoTable[nodeCnt].address.sfid;
             /* Write the packet length and payload to the TX buffer. */
             EZMacPRO_TxBuf_Write(sizeof(FrameAssocResp_t), &rfPayload.frameRaw);
             /* Send the packet. */
             EZMacPRO_Transmit();
             /* Wait until device goes to Idle. */
             while (!fEZMacPRO_StateIdleEntered);
             /* Clear State transition flags. */
             fEZMacPRO_StateIdleEntered = 0;
             /* Go to next state. */
             DEMO_SR = DEMO_ASSOC_RESP_ACK_RX;
             break;

         case DEMO_ASSOC_RESP_ACK_RX:
             /* Go to receive state. */
             EZMacPRO_Receive();
             /* Wait until radio is placed to RX. */
             while (!fEZMacPRO_StateRxEntered);
             /* Clear State transition flag. */
             fEZMacPRO_StateRxEntered = 0;
             /* Go to next state. */
             DEMO_SR = DEMO_ASSOC_RESP_ACK_RXD;
             break;

         case DEMO_ASSOC_RESP_ACK_RXD:
             if(fEZMacPRO_PacketReceived)
             {   /* Clear flag. */
                 fEZMacPRO_PacketReceived = 0;
                 /* Read out the payload of the acknowledgement. */
                 EZMacPRO_RxBuf_Read(&packetLength, &rfPayload.frameRaw);
                 /* Message is the acknowledgement of association response. */
                 if(rfPayload.frameUnion.assocRespAck.type == FRAME_ASSOC_RESP_ACK)
                 {   /* From the right node. */
                     if(memcmp(&rfPayload.frameUnion.assocRespAck.rndAddr, &slaveAddr, sizeof(Addr_t)) == 0)
                     {   /* Set flag. */
                         SlaveInfoTable[nodeCnt].associated = ASSOCIATED;
                       #ifdef TRACE_ENABLED
                         printf("[DEMO_ASSOC] Slave associated with address: %02bu.\n", SlaveInfoTable[nodeCnt].address.sfid);
                       #endif //TRACE_ENABLED
                     }
                 }
                 /* Go to Idle state. */
                 EZMacPRO_Idle();
                 /* Wait until device goes to Idle. */
                 while (!fEZMacPRO_StateIdleEntered);
                 /* Clear State transition flag. */
                 fEZMacPRO_StateIdleEntered = 0;
                 /* Go to next state. */
                 DEMO_SR = DEMO_ASSOC_REQ_RX;
             }
             break;

         default:
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
    {   /* Clear flag. */
        fEZMacPRO_LFTimerExpired = 0;
        /* Successful Slave query. Master was in Sleep state. Wake up the stack. */
        if(fEZMacPRO_StateSleepEntered)
        {   /* Clear flag. */
            fEZMacPRO_StateSleepEntered = 0;
            /* Wake up from Sleep mode. */
            EZMacPRO_Wake_Up();
            /* Wait until device goes to Idle. */
            while (!fEZMacPRO_StateIdleEntered);
            /* Clear State transition flags. */
            fEZMacPRO_StateWakeUpEntered = 0;
            fEZMacPRO_StateIdleEntered = 0;
        }
        /* Slave query failed. No response from Slave. */
        else
        {   /* Go to Idle state. */
            EZMacPRO_Idle();
            /* Wait until device goes to Idle. */
            while (!fEZMacPRO_StateIdleEntered);
            /* Clear State transition flag. */
            fEZMacPRO_StateIdleEntered = 0;
            /* Decrement timeout value of the node, replace temperature, supply
               voltage and rssi values with zero. */
            if(--SlaveInfoTable[nodeCnt].timeout == TIMEOUT_ZERO_VALUE)
            {   /* Timeout. Disassociate Slave. */
                SlaveInfoTable[nodeCnt].associated = NOT_ASSOCIATED;
                SlaveInfoTable[nodeCnt].timeout = TIMEOUT_INITIAL_VALUE;
            }
            SlaveInfoTable[nodeCnt].temperature = TEMP_INVALID_VALUE;
            SlaveInfoTable[nodeCnt].voltage = VOLTAGE_INVALID_VALUE;
            SlaveInfoTable[nodeCnt].rssi = RSSI_ZERO_VALUE;

        }
        /* LED1 indicates the radio is ON. */
        LED1 = ILLUMINATE;
        /* Disable LFT. */
        EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_MSEC(TIMEFRAME_SU_PER_SLAVE));
        /* Jump to next node in Slave Info table. */
        if(++nodeCnt < MAX_NMBR_OF_SLAVES)
        {   /* Go ahead and request status update from next node. */
            DEMO_SR = DEMO_SU_REQ_TX;
        }
        /* Go to Sleep between cycles. */
        else
        {
          #ifdef TRACE_ENABLED
            printf("[DEMO_SU] Slave query ended.\n");
          #endif //TRACE_ENABLED
            /* Go to sleep between cycles. */
            DEMO_SR = DEMO_SLEEP_INIT;
        }
    }

    switch(DEMO_SR)
    {
        case DEMO_SU_REQ_TX:
            /* Configure and start 200msec timeout for Status Update time frame. */
            EZMacPRO_Reg_Write(LFTMR0, LFTMR0_TIMEOUT_MSEC(TIMEFRAME_SU_PER_SLAVE));
            EZMacPRO_Reg_Write(LFTMR1, LFTMR1_TIMEOUT_MSEC(TIMEFRAME_SU_PER_SLAVE));
            EZMacPRO_Reg_Write(LFTMR2, 0x80 | LFTMR2_TIMEOUT_MSEC(TIMEFRAME_SU_PER_SLAVE));
            /* Associated. Query status. */
            if(SlaveInfoTable[nodeCnt].associated == ASSOCIATED)
            {   /* Set DID of Status Update request frame. */
                EZMacPRO_Reg_Write(DID, SlaveInfoTable[nodeCnt].address.sfid);
                /* Assemble Status Update request frame. */
                rfPayload.frameUnion.statusUpdateReq.type = FRAME_SU_REQ;
                /* Write the packet length and payload to the TX buffer. */
                EZMacPRO_TxBuf_Write(sizeof(FrameStatusUpdateReq_t), &rfPayload.frameRaw);
              #ifdef TRACE_ENABLED
                printf("[DEMO_SU] Slave[%02bu]: associated. Query slave.\n", SlaveInfoTable[nodeCnt].address.sfid);
              #endif //TRACE_ENABLED
                /* Send the packet. */
                EZMacPRO_Transmit();
                /* Wait until device goes to Idle. */
                while (!fEZMacPRO_StateIdleEntered);
                /* Clear State transition flags. */
                fEZMacPRO_StateIdleEntered = 0;
                /* Go to wait for status update response. */
                DEMO_SR = DEMO_SU_RESP_RX;
                break;
            }
            /* Not associated. */
            else
            {   /* Go to Sleep state. */
                EZMacPRO_Sleep();
                /* Wait until device goes to Sleep. */
                while (!fEZMacPRO_StateSleepEntered);
                /* LED1 indicates the radio is OFF. */
                LED1 = EXTINGUISH;
              #ifdef TRACE_ENABLED
                printf("[DEMO_SU] Slave[%02bu]: not associated. Skip slave.\n", SlaveInfoTable[nodeCnt].address.sfid);
              #endif //TRACE_ENABLED
                /* Go to Status Update Sleep state. */
                DEMO_SR = DEMO_SU_SLEEP;
                break;
            }
            break;

        case DEMO_SU_RESP_RX:
            /* Go to receive state. */
            EZMacPRO_Receive();
            /* Wait until radio is placed to RX. */
            while (!fEZMacPRO_StateRxEntered);
            /* Clear State transition flag. */
            fEZMacPRO_StateRxEntered = 0;
            /* Go to next state. */
            DEMO_SR = DEMO_SU_RESP_RXD;
            break;

        case DEMO_SU_RESP_RXD:
            if(fEZMacPRO_PacketReceived)
            {   /* Clear flag. */
                fEZMacPRO_PacketReceived = 0;
                /* Read out the payload of the packet. */
                EZMacPRO_RxBuf_Read(&packetLength, &rfPayload.frameRaw);
                /* Frame is status update response. */
                if(rfPayload.frameUnion.statusUpdateResp.type == FRAME_SU_RESP)
                {   /* From the right node. */
                    EZMacPRO_Reg_Read(RCID, &slaveAddr.cid);
                    EZMacPRO_Reg_Read(RSID, &slaveAddr.sfid);
                    /* Frame is sent to Master. */
                    if(memcmp(&SlaveInfoTable[nodeCnt].address, &slaveAddr, sizeof(Addr_t)) == 0)
                    {   /* Update status. */
                        SlaveInfoTable[nodeCnt].temperature = rfPayload.frameUnion.statusUpdateResp.temperature;
                        SlaveInfoTable[nodeCnt].voltage = rfPayload.frameUnion.statusUpdateResp.voltage;
                        SlaveInfoTable[nodeCnt].rssi = rfPayload.frameUnion.statusUpdateResp.rssi;
                        /* Go to Idle state. */
                        EZMacPRO_Idle();
                        /* Wait until device goes to Idle. */
                        while (!fEZMacPRO_StateIdleEntered);
                        /* Clear State transition flag. */
                        fEZMacPRO_StateIdleEntered = 0;
                        /* Go to Sleep state. */
                        EZMacPRO_Sleep();
                        /* Wait until device goes to Sleep. */
                        while (!fEZMacPRO_StateSleepEntered);
                        /* LED1 indicates the radio is OFF. */
                        LED1 = EXTINGUISH;
                        /* Go to sleep state. */
                        DEMO_SR = DEMO_SU_SLEEP;
                        break;
                    }
                }
            }
            break;

        case DEMO_SU_SLEEP:
            /* Wait here until current Status Update time slot is over. */
            break;

        default:
            break;
    }
}


/*!
 * Sleep between cycles.
 */
void MasterNodeSleep(void)
{
    /* LFT expired. Go to Status Update state. */
    if (fEZMacPRO_LFTimerExpired)
    {   /* Clear flag. */
        fEZMacPRO_LFTimerExpired = 0;
        /* Disable LFT. */
        EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_SEC(TIMEFRAME_SLEEP));
        /* Wake up from Sleep mode. */
        EZMacPRO_Wake_Up();
        /* Wait until device goes to Idle. */
        while (!fEZMacPRO_StateIdleEntered);
        /* Clear State transition flags. */
        fEZMacPRO_StateWakeUpEntered = 0;
        fEZMacPRO_StateIdleEntered = 0;
      #ifdef TRACE_ENABLED
        printf("[DEMO_ASSOC] Master woke up.\n");
      #endif //TRACE_ENABLED
        /* Go ahead and request status updates. */
        DEMO_SR = DEMO_ASSOC_BEACON_TX;
    }

    switch(DEMO_SR)
    {
        case DEMO_SLEEP_INIT:
            /* Go to Sleep state. */
            EZMacPRO_Sleep();
            /* Wait until device goes to Sleep. */
            while (!fEZMacPRO_StateSleepEntered);
            /* Clear State transition flag. */
            fEZMacPRO_StateSleepEntered = 0;
            /* Configure and start inter-cycle timeout. */
            EZMacPRO_Reg_Write(LFTMR0, LFTMR0_TIMEOUT_SEC(TIMEFRAME_SLEEP));
            EZMacPRO_Reg_Write(LFTMR1, LFTMR1_TIMEOUT_SEC(TIMEFRAME_SLEEP));
            EZMacPRO_Reg_Write(LFTMR2, 0x80 | LFTMR2_TIMEOUT_SEC(TIMEFRAME_SLEEP));
            /* LED1 indicates the radio is OFF. */
            LED1 = EXTINGUISH;
          #ifdef SDBC
            /* Update parameters of Slaves on LCD. */
            PerformMenu(sMenu_RefreshScreen);
          #endif //SDBC
          #ifdef TRACE_ENABLED
            /* Display Status Update state on UART. */
            printf("[DEMO_SLEEP] Slave Info Table:\n", SlaveInfoTable[nodeCnt].address.sfid);
            PrintSlaveInfo();
          #endif //TRACE_ENABLED
          #ifdef SDBC
            /* Display Status Update state on LCD. */
            PerformMenu(sMenu_SleepState);
          #endif //SDBC
          #ifdef TRACE_ENABLED
            printf("[DEMO_SLEEP] Master went to sleep.\n\n\n");
          #endif //TRACE_ENABLED
            /* Go to sleep loop state. */
            DEMO_SR = DEMO_SLEEP_LOOP;
            break;

        case DEMO_SLEEP_LOOP:
            /* Wait here between cycles. */
            break;

        default:
            break;
    }
}


/*!
 * Search for free slot in association table.
 */
U8 SearchFreeSlotInAssocTable(void)
{
    U8 cnt;

    /* Look for free slot in the association table. */
    for(cnt=0; cnt<MAX_NMBR_OF_SLAVES; cnt++)
    {   /* Free slot found. */
        if(!SlaveInfoTable[cnt].associated)
        {   /* Return index. */
            return cnt;
        }
    }
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
    {   /* Associated node info - temperature and voltage units. */
        if(SlaveInfoTable[bCnt].associated == ASSOCIATED)
        {   /* Print slave status. */
            SprintfSlaveInfo(&SlaveInfoTable[bCnt], printBuf);
            /* Print Rssi. */
            sprintf(&printBuf[21], "   [rssi:%03bu]", SlaveInfoTable[bCnt].rssi);
        }
        else
        {   /* Update slave status - not associated. */
            SprintfSlaveNotAssoc(printBuf);
        }
        printf("    %s\n", printBuf);
    }
}
#endif //TRACE_ENABLED



