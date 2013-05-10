/*!\file p2p_demo_fwd_menu.c
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
#include "p2p_demo_fwd_node.h"
#include "p2p_demo_fwd_menu.h"


SEGMENT_VARIABLE(lcd_data[22], U8, APPLICATION_MSPACE);

#ifdef DOG_LCD_ENABLED
/*!
 * Display the actual menu on the LCD.
 */
void vPerformMenu(MENU_ITEM menu_item)
{
    U8 temp;

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

        case sMenu_FwdScreen:
            /* Show Fwd startup screen. */
            LcdClearDisplay();
            memcpy(lcd_data, "Network Application  ", 21);
            LcdWriteLine(LCD_LINE_1, lcd_data);
            memcpy(lcd_data, "---------------------", 21);
            LcdWriteLine(LCD_LINE_2, lcd_data);
            LcdWriteLine(LCD_LINE_7, lcd_data);
            memcpy(lcd_data, "       Fwd Node      ", 21);
            LcdWriteLine(LCD_LINE_3, lcd_data);
            sprintf(lcd_data, "Packet content:%5u", wPacketCounter);
            LcdWriteLine(LCD_LINE_5, lcd_data);
            break;

        case sMenu_FwdRefreshCounter:
            /* Refresh number of packets that has been sent. */
            EZMacPRO_Reg_Read(RCTRL, &temp);
            sprintf(lcd_data, "Packet content:%5u", wPacketCounter);
            if ((wPacketCounter == 0) && ((temp & 0x03) > 0))
            {
                memcpy(lcd_data, "Packet content:  ACK", 20);
            }
            LcdWriteLine(LCD_LINE_5, lcd_data);
            break;
            
        case sMenu_PacketForward:
            /* Show forward state. */
            memcpy(lcd_data, " Receive    Forward *", 21);
            LcdWriteLine(LCD_LINE_8, lcd_data);
            break;

        case sMenu_PacketReceive:
            /* Show receive state. */
            memcpy(lcd_data, " Receive *  Forward  ", 21);
            LcdWriteLine(LCD_LINE_8, lcd_data);
            break;
    }
}

#endif //DOG_LCD_ENABLED