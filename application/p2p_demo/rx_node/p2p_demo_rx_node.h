/*!\file p2p_demo_rx_node.h
 * \brief P2P demo code on top of the EZMacPRO stack.
 *
 *
 * \n This software must be used in accordance with the End User License
 * \n Agreement.
 *
 * \b COPYRIGHT
 * \n Copyright 2012 Silicon Laboratories, Inc.
 * \n http://www.silabs.com
 */

#ifndef _P2P_DEMO_RX_NODE_H_
#define _P2P_DEMO_RX_NODE_H_


                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

/*!
 * Application Firmware version.
 */
#define APP_FW_VERSION    "1.0.0b"


/*!
 * Addresses used in this demo.
 */
#define CUSTOMER_ID                         (0x01)
#define DEVICE_SELF_ID	                    (0x01)


/*!
 * States of the Star demo.
 */
#define DEMO_BOOT                           (0x10)
#define DEMO_RX_FIRST                       (0x20)
#define DEMO_SLEEP                          (0x40)
#define DEMO_RX                             (0x80)


#define STARTUP_TIMEOUT                     (3)         // sec


/*!
 * LFT timeout macros used exclusively with LFTMR registers.
 */
#define LFTMR2_TIMEOUT_MSEC(n)              (0x44)      // Internal Time Base, WTR=0x04
#define LFTMR1_TIMEOUT_MSEC(n)              (U8)(((U16)((n*32.768F)/64)&0xFF00)>>8) // WTR=0x04 assumed
#define LFTMR0_TIMEOUT_MSEC(n)              (U8)((U16)((n*32.768F)/64)&0x00FF)      // WTR=0x04 assumed
#define LFTMR2_TIMEOUT_SEC(n)               (0x44)      // WTR=0x04
#define LFTMR1_TIMEOUT_SEC(n)               (U8)(((U16)((n*1000*32.768F)/64)&0xFF00)>>8) // WTR=0x04 assumed
#define LFTMR0_TIMEOUT_SEC(n)               (U8)((U16)((n*1000*32.768F)/64)&0x00FF)      // WTR=0x04 assumed


                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

extern SEGMENT_VARIABLE(abRfPayload[5], U8, BUFFER_MSPACE);
extern SEGMENT_VARIABLE(bPacketLength, U8, APPLICATION_MSPACE);
extern SEGMENT_VARIABLE(wPacketCounter, U16, APPLICATION_MSPACE);
extern SEGMENT_VARIABLE(rssiVal, U8, APPLICATION_MSPACE);
extern SEGMENT_VARIABLE(DEMO_SR, U8, APPLICATION_MSPACE);


                /* ======================================= *
                 *  F U N C T I O N   P R O T O T Y P E S  *
                 * ======================================= */

/*!
 * Local function prototypes.
 */
void StateMachine_Init(void);
void StateMachine(void);

void vP2P_demo_RxInit(void);
void vP2P_demo_GetPacketGoToSleep(void);


#endif //_P2P_DEMO_RX_NODE_H_
