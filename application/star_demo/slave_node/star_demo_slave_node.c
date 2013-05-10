/*!\file star_demo_slave_node.c
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
#include "star_demo_slave_node.h"


                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

SEGMENT_VARIABLE(SlaveInfo, SlaveInfo_t, APPLICATION_MSPACE);
SEGMENT_VARIABLE(rfPayload, Frame_uu, APPLICATION_MSPACE);
SEGMENT_VARIABLE(packetLength, U8, APPLICATION_MSPACE);
SEGMENT_VARIABLE(rndCounter, U16, APPLICATION_MSPACE);
SEGMENT_VARIABLE(masterAddr, Addr_t, APPLICATION_MSPACE);
SEGMENT_VARIABLE(rndAddr, Addr_t, APPLICATION_MSPACE);
SEGMENT_VARIABLE(DEMO_SR, U8, APPLICATION_MSPACE);


                /* ======================================= *
                 *      L O C A L   F U N C T I O N S      *
                 * ======================================= */

/*!
 * Entry point to the State machine.
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
            /* Boot Slave Node. */
            SlaveNodeBoot();
            break;

        case DEMO_ASSOC:
            /* Associate with Master. */
            SlaveNodeAssociate();
            break;

        case DEMO_SU:
            /* Respond to Status update requests of the Master. */
            SlaveNodeStatusUpdate();
            break;

        case DEMO_SLEEP:
            /* Radio in Sleep between Status Updates. */
            SlaveNodeSleep();
            break;

        default:
            break;
    }
}


/*!
 * Boot Slave Node.
 */
void SlaveNodeBoot(void)
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
            /* Star demo Slave node initialisation. */
            EZMacPRO_Reg_Write(MCR, 0xAC);                  // CIDE=1, DR=9.6kbps, RAD=1, DNPL=1, NFR=0
            EZMacPRO_Reg_Write(SECR, 0x60);                 // State after receive is RX state and state after transmit is Idle state
            EZMacPRO_Reg_Write(RCR, 0x00);                  // Search disabled
            EZMacPRO_Reg_Write(FR0, 0);                     // Set used frequency channel
            EZMacPRO_Reg_Write(TCR, (0x70|LBT_SWITCH));     // LBT enabled/disabled, Output power: +20 dBm, ACK disable, AFC disable
            EZMacPRO_Reg_Write(LBDR, 0x80);                 // Enable Low Battery Detect
            EZMacPRO_Reg_Write(PFCR, 0x91);                 // CID filter and Mcast addr. filter (MCA mode) are enabled
            EZMacPRO_Reg_Write(SCID, DEMO_SLAVE_CID);       // Set the Customer ID
            EZMacPRO_Reg_Write(MCA_MCM, DEMO_SLAVE_MCAST);  // Set the MCA
            /* Init slave info table. */
            SlaveInfo.associated = NOT_ASSOCIATED;
            SlaveInfo.temperature = TEMP_INVALID_VALUE;
            SlaveInfo.voltage = VOLTAGE_INVALID_VALUE;
            SlaveInfo.rssi = RSSI_INITIAL_VALUE;
          #ifdef TRACE_ENABLED
            printf("[DEMO_BOOT] Slave startup done.\n");
            printf("[DEMO_BOOT] Wait for button press...\n\n");
          #endif //TRACE_ENABLED
            /* Go ahead and generate temporary random SFID. */
            DEMO_SR = DEMO_BOOT_RND_ADDR;
            break;

        case DEMO_BOOT_RND_ADDR:
            rndCounter++;
            /* Button is pushed, generate random SFID. */
            if (PB1 == 0)
            {   /* Wait until button released. */
                while (PB1 == 0);
                /* Save CID. */
                rndAddr.cid = DEMO_SLAVE_CID;
                /* Generate temporary random SFID: 5-255. */
                rndAddr.sfid = GenTempRndAddr();
                /* Write temporary SFID. */
                EZMacPRO_Reg_Write(SFID, rndAddr.sfid);
              #ifdef TRACE_ENABLED
                printf("[DEMO_BOOT] Button pressed.\n");
                printf("[DEMO_BOOT] Temporary address generated: %02bu.\n", rndAddr.sfid);
              #endif //TRACE_ENABLED
                /* Wake up from Sleep mode. */
                EZMacPRO_Wake_Up();
                /* Wait until device goes to Idle. */
                while (!fEZMacPRO_StateIdleEntered);
                /* Clear State transition flags. */
                fEZMacPRO_StateWakeUpEntered = 0;
                fEZMacPRO_StateIdleEntered = 0;
                /* Go ahead to wait for Beacon frame. */
                DEMO_SR = DEMO_ASSOC_BEACON_RX;
                break;
            }
            break;

        default:
            break;
    }
}


void SlaveNodeAssociate(void)
{
    /* LFT expired. Go back and start waiting for next Beacon. */
    if (fEZMacPRO_LFTimerExpired)
    {   /* Clear flag. */
        fEZMacPRO_LFTimerExpired = 0;
        /* Go to Idle state. */
        EZMacPRO_Idle();
        /* Wait until device goes to Idle. */
        while (!fEZMacPRO_StateIdleEntered);
        /* Clear State transition flag. */
        fEZMacPRO_StateIdleEntered = 0;
        /* Disable LFT. */
        EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
      #ifdef TRACE_ENABLED
        printf("[DEMO_ASSOC] Association timeout.\n");
      #endif //TRACE_ENABLED
        /* Go back and start waiting for next Beacon. */
        DEMO_SR = DEMO_ASSOC_BEACON_RX;
    }

    switch(DEMO_SR)
    {
        case DEMO_ASSOC_BEACON_RX:
            /* Go to Idle state before sending Beacon. */
            EZMacPRO_Idle();
            /* Wait until device goes to Idle. */
            while (!fEZMacPRO_StateIdleEntered);
            /* Clear State transition flag. */
            fEZMacPRO_StateIdleEntered = 0;
            /* LED1 indicates the radio is ON. */
            LED1 = ILLUMINATE;
          #ifdef TRACE_ENABLED
            printf("[DEMO_ASSOC] Wait for association.\n");
          #endif //TRACE_ENABLED
            /* Go to Receive state. */
            EZMacPRO_Receive();
            /* Wait until radio is placed to RX. */
            while (!fEZMacPRO_StateRxEntered);
            /* Clear State transition flag. */
            fEZMacPRO_StateRxEntered = 0;
            /* Go ahead to wait for Beacon frame. */
            DEMO_SR = DEMO_ASSOC_BEACON_RXD;
            break;

        case DEMO_ASSOC_BEACON_RXD:
            /* Packet received Check if it is the Beacon Frame. */
            if (fEZMacPRO_PacketReceived)
            {   /* Clear flag. */
                fEZMacPRO_PacketReceived = 0;
                /* Read out the payload of the packet. */
                EZMacPRO_RxBuf_Read(&packetLength, rfPayload.frameRaw);
                /* Message is the Beacon frame, send association request. */
                if(rfPayload.frameUnion.beacon.type == FRAME_BEACON)
                {   /* Save SFID of the Master. */
                    EZMacPRO_Reg_Read(RSID, &masterAddr.sfid);
                    /* Go ahead and wait for Association Response frame. */
                    DEMO_SR = DEMO_ASSOC_REQ_TX;
                    break;
                }
            }
            break;

        case DEMO_ASSOC_REQ_TX:
            /* Go to Idle state before sending Assoc. req. */
            EZMacPRO_Idle();
            /* Wait until device goes to Idle. */
            while (!fEZMacPRO_StateIdleEntered);
            /* Clear State transition flag. */
            fEZMacPRO_StateIdleEntered = 0;
            /* Set the Low Frequency Timer interval and start it. */
            EZMacPRO_Reg_Write(LFTMR0, LFTMR0_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
            EZMacPRO_Reg_Write(LFTMR1, LFTMR1_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
            EZMacPRO_Reg_Write(LFTMR2, 0x80 | LFTMR2_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
            EZMacPRO_Reg_Write(TCR, (0x70|0x08));     // LBT enabled, Output power: +20 dBm, ACK disable, AFC disable
            /* Destination is the Master node. */
            EZMacPRO_Reg_Write(DID, masterAddr.sfid);   // Set Destination ID
            /* Assemble Association Request frame. */
            rfPayload.frameUnion.assocReq.type = FRAME_ASSOC_REQ;
            /* Write the packet length and payload to the TX buffer. */
            EZMacPRO_TxBuf_Write(sizeof(FrameAssocReq_t), &rfPayload.frameRaw);
            /* Wait random time between 0-10msec. */
            Timer2BusyWait_ms((rndAddr.sfid%10) * DELAY_1MS_TIMER2);
            /* Send the packet. */
            EZMacPRO_Transmit();
            /* Wait until device goes to Idle or LBTTimeout. */
            while (!fEZMacPRO_StateIdleEntered && !fEZMacPRO_LBTTimeout);
            if(fEZMacPRO_StateIdleEntered)
            {   /* Clear State transition flags. */
                fEZMacPRO_StateIdleEntered = 0;
            }
            if(fEZMacPRO_LBTTimeout)
            {   /* Clear LBT timeout flag. */
                fEZMacPRO_LBTTimeout = 0;
                /* Recover from Error state by entering Idle.*/
                EZMacPRO_Idle();
                /* Stay in this state and wait till timeout. */
                break;
            }
            /* Go to next state. */
            DEMO_SR = DEMO_ASSOC_RESP_RX;
            break;

        case DEMO_ASSOC_RESP_RX:
            /* Start waiting for Association response frame. */
            EZMacPRO_Receive();
            /* Wait until radio is placed to RX. */
            while (!fEZMacPRO_StateRxEntered);
            /* Clear State transition flag. */
            fEZMacPRO_StateRxEntered = 0;
            /* Go to next state. */
            DEMO_SR = DEMO_ASSOC_RESP_RXD;
            break;

        case DEMO_ASSOC_RESP_RXD:
            /* Packet received. */
            if (fEZMacPRO_PacketReceived)
            {   /* Clear flag. */
                fEZMacPRO_PacketReceived = 0;
                /* Read out the payload of the packet. */
                EZMacPRO_RxBuf_Read(&packetLength, &rfPayload.frameRaw);
                /* Message is the Assoc. resp. frame! */
                if(rfPayload.frameUnion.assocResp.type == FRAME_ASSOC_RESP)
                {   /* Go to Idle state to overwrite temporary SFID. */
                    EZMacPRO_Idle();
                    /* Wait until device goes to Idle. */
                    while (!fEZMacPRO_StateIdleEntered);
                    /* Clear State transition flag. */
                    fEZMacPRO_StateIdleEntered = 0;
                    /* Assign new SFID (given by Master) to the node! */
                    EZMacPRO_Reg_Write(SFID, rfPayload.frameUnion.assocResp.newAddr.sfid);
                    /* Go ahead and acknowledge Association response. */
                    DEMO_SR = DEMO_ASSOC_RESP_ACK_TX;
                    break;
                }
            }
            break;

        case DEMO_ASSOC_RESP_ACK_TX:
            /* Acknowledge Association response. */
            EZMacPRO_Reg_Write(TCR, (0x70|LBT_SWITCH));     // LBT enabled/disabled, Output power: +20 dBm, ACK disable, AFC disable
            EZMacPRO_Reg_Write(DID, masterAddr.sfid);       // Set DID = Master node
            /* Assemble Association Response acknowledgement frame. */
            rfPayload.frameUnion.assocRespAck.type = FRAME_ASSOC_RESP_ACK;
            rfPayload.frameUnion.assocRespAck.rndAddr.sfid = rndAddr.sfid;
            /* Write the packet length and payload to the TX buffer. */
            EZMacPRO_TxBuf_Write(sizeof(FrameAssocRespAck_t), &rfPayload.frameRaw);
            /* Send the packet. */
            EZMacPRO_Transmit();
            /* Wait until device goes to Idle. */
            while (!fEZMacPRO_StateIdleEntered);
            /* Clear State transition flags. */
            fEZMacPRO_StateIdleEntered = 0;
            /* Disable LFT. */
            EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_MSEC(TIMEFRAME_ASSOC));
            /* Reconfigure PFCR to accept only packets destined to itself. */
            EZMacPRO_Reg_Write(PFCR, 0xA0);         // CID filter and DID filter enabled
          #ifdef TRACE_ENABLED
            /* Temporary reuse of rndAddr variable. */
            EZMacPRO_Reg_Read(SFID, &rndAddr.sfid);
            printf("[DEMO_ASSOC] Associated. Slave address: %02bu.\n", rndAddr.sfid);
          #endif //TRACE_ENABLED
            /* Start waiting for Status Update requests. */
            DEMO_SR = DEMO_SU_REQ_RX;
            break;

        default:
            break;
    }
}


void SlaveNodeStatusUpdate(void)
{
//    U16 tmpCnt;

    switch(DEMO_SR)
    {
        case DEMO_SU_REQ_RX:
            /* Configure ADC for temperature measurement. */
            EZMacPRO_Reg_Write(ADCTSR, 0x80);
            /* Wait 1msec here. */
            Timer2BusyWait_ms(DELAY_1MS_TIMER2);
//            for(tmpCnt=0; tmpCnt<65000; tmpCnt++);
            /* Get temperature value. */
            EZMacPRO_Reg_Read(ADCTSV, &SlaveInfo.temperature);
            /* TODO: does this save power?? */
            EZMacPRO_Reg_Write(ADCTSR, 0x00);
            /* Get battery voltage. Should be different from 0x00. */
            do
            {   /* Read out battery voltage. */
                EZMacPRO_Reg_Read(LBDR, &SlaveInfo.voltage);
            } while (SlaveInfo.voltage == 0x00);
            /* RSSI value is saved in the EZMacPRO_PacketReceived() callback. */

          #ifdef TRACE_ENABLED
            printf("[DEMO_SU] Battery and temperature measurement done.\n");
            printf("[DEMO_SU] Wait for status update request.\n");
          #endif //TRACE_ENABLED
            /* Go to Receive state. */
            EZMacPRO_Receive();
            /* Wait until radio is placed to RX. */
            while (!fEZMacPRO_StateRxEntered);
            /* Clear State transition flag. */
            fEZMacPRO_StateRxEntered = 0;
            /* Go ahead to wait for Status Update frame. */
            DEMO_SR = DEMO_SU_REQ_RXD;
            break;

        case DEMO_SU_REQ_RXD:
            /* Packet received. */
            if (fEZMacPRO_PacketReceived)
            {   /* Clear flag. */
                fEZMacPRO_PacketReceived = 0;
                /* Read out the payload of the packet. */
                EZMacPRO_RxBuf_Read(&packetLength, &rfPayload.frameRaw);
                /* Message is Status Update request. */
                if(rfPayload.frameUnion.statusUpdateResp.type == FRAME_SU_REQ)
                {
                  #ifdef TRACE_ENABLED
                    printf("[DEMO_SU] Status update request received.\n");
                  #endif //TRACE_ENABLED
                    /* Go ahead and send Status Update response. */
                    DEMO_SR = DEMO_SU_RESP_TX;
                    break;
                }
            }
            break;

        case DEMO_SU_RESP_TX:
            /* Go to Idle state before sending Status Update resp. */
            EZMacPRO_Idle();
            /* Wait until device goes to Idle. */
            while (!fEZMacPRO_StateIdleEntered);
            /* Clear State transition flag. */
            fEZMacPRO_StateIdleEntered = 0;
            EZMacPRO_Reg_Write(TCR, (0x70|LBT_SWITCH));     // LBT enabled/disabled, Output power: +20 dBm, ACK disable, AFC disable
            EZMacPRO_Reg_Write(DID, masterAddr.sfid);       // Set DID = Master node
            /* Assemble Status Update Response. */
            rfPayload.frameUnion.statusUpdateResp.type = FRAME_SU_RESP;
            rfPayload.frameUnion.statusUpdateResp.temperature = SlaveInfo.temperature;
            rfPayload.frameUnion.statusUpdateResp.voltage = SlaveInfo.voltage;
            rfPayload.frameUnion.statusUpdateResp.rssi = SlaveInfo.rssi;
            /* Write the packet length and payload to the TX buffer. */
            EZMacPRO_TxBuf_Write(sizeof(FrameStatusUpdateResp_t), &rfPayload.frameRaw);
            /* Send the packet. */
            EZMacPRO_Transmit();
            /* Wait until device goes to Idle. */
            while (!fEZMacPRO_StateIdleEntered);
            /* Clear State transition flags. */
            fEZMacPRO_StateIdleEntered = 0;
          #ifdef TRACE_ENABLED
            printf("[DEMO_SU] Status update response sent.\n");
          #endif //TRACE_ENABLED
            /* Go to Sleep state. */
            EZMacPRO_Sleep();
            /* Wait until device goes back to Sleep. */
            while (!fEZMacPRO_StateSleepEntered);
            /* Clear State transition flag. */
            fEZMacPRO_StateSleepEntered = 0;
            /* Set the Low Frequency Timer interval and start it. */
            EZMacPRO_Reg_Write(LFTMR0, LFTMR0_TIMEOUT_MSEC(TIMEFRAME_TOTAL));
            EZMacPRO_Reg_Write(LFTMR1, LFTMR1_TIMEOUT_MSEC(TIMEFRAME_TOTAL));
            EZMacPRO_Reg_Write(LFTMR2, 0x80 | LFTMR2_TIMEOUT_MSEC(TIMEFRAME_TOTAL));
            /* LED1 indicates the radio is OFF. */
            LED1 = EXTINGUISH;
          #ifdef TRACE_ENABLED
            /* Temporary reuse of rndAddr variable. */
            EZMacPRO_Reg_Read(SFID, &rndAddr.sfid);
            printf("[DEMO_SLEEP] Slave[%02bu] went to sleep.\n\n\n", rndAddr.sfid);
          #endif //TRACE_ENABLED
            /* Go to Sleep state. */
            DEMO_SR = DEMO_SLEEP;
            break;

        default:
            break;
    }
}


void SlaveNodeSleep(void)
{
    switch(DEMO_SR)
    {
        case DEMO_SLEEP:
            /* Wait here until LFT expires. */
            if (fEZMacPRO_LFTimerExpired)
            {   /* Clear flag. */
                fEZMacPRO_LFTimerExpired = 0;
                /* Wake up from Sleep mode. */
                EZMacPRO_Wake_Up();
                /* Wait until device goes to Idle. */
                while (!fEZMacPRO_StateIdleEntered);
                /* Clear State transition flags. */
                fEZMacPRO_StateWakeUpEntered = 0;
                fEZMacPRO_StateIdleEntered = 0;
                /* Disable LFT. */
                EZMacPRO_Reg_Write(LFTMR2, ~0x80 & LFTMR2_TIMEOUT_MSEC(TIMEFRAME_TOTAL));
                /* LED1 indicates the radio is ON. */
                LED1 = ILLUMINATE;
              #ifdef TRACE_ENABLED
                /* Temporary reuse of rndAddr variable. */
                EZMacPRO_Reg_Read(SFID, &rndAddr.sfid);
                printf("[DEMO_ASSOC] Slave[%02bu] woke up.\n", rndAddr.sfid);
              #endif //TRACE_ENABLED
                /* Go ahead and start waiting for Status Update request. */
                DEMO_SR = DEMO_SU_REQ_RX;
                break;
            }
            break;

        default:
            break;
    }
}


/*!
 * Generate temporary random SFID for slave for the duration of association.
 * Note: result will fall in the 0x05-0xFD range. Permanent slave ID domain,
 * multicast and broadcast addresses are reserved.
 */
U8 GenTempRndAddr(void)
{
    return ((rndCounter%(253-MAX_NMBR_OF_SLAVES))+MAX_NMBR_OF_SLAVES+1);
}


