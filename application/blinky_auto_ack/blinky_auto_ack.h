/*!\file blinky_auto_ack.h
 * \brief Wireless blinky sample code using the EZMacPRO stack.
 *
 *
 * \n This software must be used in accordance with the End User License
 * \n Agreement.
 *
 * \b COPYRIGHT
 * \n Copyright 2012 Silicon Laboratories, Inc.
 * \n http://www.silabs.com
 */

#ifndef _BLINKY_AUTO_ACK_H_
#define _BLINKY_AUTO_ACK_H_


                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

#define DEMO_SR_STATE_BOOT_BIT             (0x10)
#define DEMO_SR_STATE_INIT_RX_BIT          (0x20)
#define DEMO_SR_STATE_TRX_BIT              (0x40)


                /* ======================================= *
                 *  F U N C T I O N   P R O T O T Y P E S  *
                 * ======================================= */

/*!
 * Local function prototypes.
 */
void StateMachine(void);
void StateMachine_Init(void);




#endif //_BLINKY_AUTO_ACK_H_
