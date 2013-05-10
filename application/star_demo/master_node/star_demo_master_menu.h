/*!\file star_demo_master_menu.h
 * \brief Star demo code on top of the EZMacPRO stack.
 *
 *
 * \n This software must be used in accordance with the End User License
 * \n Agreement.
 *
 * \b COPYRIGHT
 * \n Copyright 2012 Silicon Laboratories, Inc.
 * \n http://www.silabs.com
 */

#ifndef _STAR_DEMO_MASTER_MENU_H_
#define _STAR_DEMO_MASTER_MENU_H_


typedef enum _MENU_ITEM
{
    sMenu_StartupScreen,
    sMenu_MainScreen,
    sMenu_Associate_State,
    sMenu_StatusUpdate_State,
    sMenu_RefreshScreen,
    sMenu_SleepState,
} MENU_ITEM;

void PerformMenu(MENU_ITEM menu_item);

void SprintfSlaveInfo(SlaveInfoTable_t * slave, U8 * s);
void SprintfSlaveNotAssoc(U8 * s);

void LcdShowRssi(U8 node, U8 rssi);
void PrintRssi(U8 row, U8 column, U8 rssi);





#endif //_STAR_DEMO_MASTER_MENU_H_
