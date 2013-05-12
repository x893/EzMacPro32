/*!\file p2p_demo_rx_menu.c
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
#include "p2p_demo_node.h"
#include "p2p_demo_rx_menu.h"

SEGMENT_VARIABLE(lcd_data[22], U8, APPLICATION_MSPACE);

#ifdef DOG_LCD_ENABLED
/*!
 * Display the actual menu on the LCD.
 */
void vPerformMenu(MENU_ITEM menu_item)
{
    switch (menu_item)
    {
        case sMenu_StartupScreen:
            /* Display SiLabs startup screen. */
            LcdClearDisplay();
            LcdSetPictureCursor(LCD_LINE_1, 20);
            LcdDrawPicture(silabs87x40);
            memcpy(lcd_data, "Network Application  ", 21);
            LcdWriteLine(LCD_LINE_7, lcd_data);
            sprintf(lcd_data, "  P2P Demo v         ");
            memcpy(&lcd_data[12], APP_FW_VERSION, 6);
            LcdWriteLine(LCD_LINE_8, lcd_data);
            break;

        case sMenu_RxScreen:
            /* Show Rx startup screen. */
            LcdClearDisplay();
            memcpy(lcd_data, "Network Application  ", 21);
            LcdWriteLine(LCD_LINE_1, lcd_data);
            memcpy(lcd_data, "---------------------", 21);
            LcdWriteLine(LCD_LINE_2, lcd_data);
            LcdWriteLine(LCD_LINE_7, lcd_data);
            memcpy(lcd_data, "       Rx Node       ", 21);
            LcdWriteLine(LCD_LINE_3, lcd_data);
            sprintf(lcd_data, "Packet content:%5u", wPacketCounter);
            LcdWriteLine(LCD_LINE_5, lcd_data);
            break;

        case sMenu_RxRefreshCounter:
            /* Refresh number of packets sent. */
            sprintf(lcd_data, "Packet content:%5u", wPacketCounter);
            LcdWriteLine(LCD_LINE_5, lcd_data);
            break;

        case sMenu_PacketReceive:
            /* Show receive mode. */ 
            memcpy(lcd_data, " Sleep     Receive * ", 21);
            LcdWriteLine(LCD_LINE_8, lcd_data);

            break;

        case sMenu_SleepMode:
            /* Show sleep mode. */ 
            memcpy(lcd_data, " Sleep *   Receive   ", 21);
            LcdWriteLine(LCD_LINE_8, lcd_data);
            break;
    }
}

#endif //DOG_LCD_ENABLED
