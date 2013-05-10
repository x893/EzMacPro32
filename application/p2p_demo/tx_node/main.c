/*!\file main.c
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

#include "..\..\common.h"
#include "p2p_demo_tx_node.h"

#ifdef SDCC
/*!
 * External startup function of SDCC. It performs operations prior static and
 * global variable initialisation. Watchdog timer should be disabled this way,
 * otherwise it can expire before variable initialisation is carried out, and
 * may prevent program execution jumping into main().
 */
void _sdcc_external_startup(void)
{
    /* Disable Watchdog. */
    DISABLE_WATCHDOG();
}
#endif //SDCC

/*!
 * Interrupt prototypes should be placed in main.c per SDCC user guide.
 */
INTERRUPT_PROTO(timerIntT3_ISR, INTERRUPT_TIMER3);
INTERRUPT_PROTO(externalIntISR, INTERRUPT_INT0);

/*!
 * Main function of the project.
 */
void main(void)
{
    /* Init demo. */
    StateMachine_Init();

    /* Loop until the universe breaks down. */
    while (1)
    {
        /* Run the State Machine. */
        StateMachine();
    }
}
