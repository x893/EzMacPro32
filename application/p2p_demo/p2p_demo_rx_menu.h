/*!\file p2p_demo_rx_menu.h
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

#ifndef _P2P_DEMO_RX_MENU_H_
#define _P2P_DEMO_RX_MENU_H_



typedef enum _MENU_ITEM
{
	sMenu_StartupScreen,
	sMenu_RxRefreshCounter,
	sMenu_RxScreen,
	sMenu_PacketReceive,
	sMenu_SleepMode,
} MENU_ITEM;


void vPerformMenu(MENU_ITEM menu_item);



#endif //_P2P_DEMO_RX_MENU_H_
