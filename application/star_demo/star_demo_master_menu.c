/*!\file star_demo_master_menu.c
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

#include "..\..\common.h"
#include "star_demo_node.h"
#include "star_demo_menu.h"

SEGMENT_VARIABLE(lcd_data[22], U8, TEST_CODE_MSPACE);

#ifdef DOG_LCD_ENABLED

/*!
 * Display the actual menu on the LCD.
 */
void PerformMenu(MENU_ITEM menu_item)
{
	U8 bCnt;

	switch (menu_item)
	{
		case sMenu_StartupScreen:										// Display SiLabs startup screen
			LcdClearDisplay();
			LcdSetPictureCursor(LCD_LINE_1, 20);
			LcdDrawPicture(silabs87x40);
			memcpy(lcd_data, "Network Application	", 21);
			LcdWriteLine(LCD_LINE_7, lcd_data);
			sprintf(lcd_data, "	Star Demo v		");
			memcpy(&lcd_data[13], APP_FW_VERSION, 6);
			LcdWriteLine(LCD_LINE_8, lcd_data);
			break;

		case sMenu_MainScreen:											// Show main screen
			LcdClearDisplay();
			LcdSetPictureCursor(1, 1);
			LcdDrawPicture(ant_slave1_11x8);
			LcdSetPictureCursor(1, 29);
			LcdDrawPicture(ant_slave2_13x8);
			LcdSetPictureCursor(1, 59);
			LcdDrawPicture(ant_slave3_13x8);
			LcdSetPictureCursor(1, 89);
			LcdDrawPicture(ant_slave4_13x8);
			memcpy(lcd_data, "---------------------", 21);
			LcdWriteLine(LCD_LINE_2, lcd_data);
			LcdWriteLine(LCD_LINE_7, lcd_data);
			memcpy(lcd_data, "Master - Boot		", 21);
			LcdWriteLine(LCD_LINE_8, lcd_data);
			break;

		case sMenu_Associate_State:
			memcpy(lcd_data, "Master - Associate	", 21);
			LcdWriteLine(LCD_LINE_8, lcd_data);
			break;

		case sMenu_StatusUpdate_State:
			memcpy(lcd_data, "Master - Slave query ", 21);
			LcdWriteLine(LCD_LINE_8, lcd_data);
			break;

		case sMenu_RefreshScreen:
			for (bCnt = 0; bCnt < MAX_NMBR_OF_SLAVES; bCnt++)			// Loop over Association table.
			{
				LcdShowRssi(bCnt, SlaveInfoTable[bCnt].rssi);			// Update Rssi on the LCD.
				if(SlaveInfoTable[bCnt].associated == ASSOCIATED)		// Associated node info - temperature and voltage units.
					SprintfSlaveInfo(&SlaveInfoTable[bCnt], lcd_data);	// Update slave status.
				else
					SprintfSlaveNotAssoc(lcd_data);						// Update slave status - not associated.
				LcdWriteLine(LCD_LINE_3 + bCnt, lcd_data);
			}
			break;

		case sMenu_SleepState:
			memcpy(lcd_data, "Master - Sleep		", 21);
			LcdWriteLine(LCD_LINE_8, lcd_data);
			break;
	}
	return;
}
#endif //DOG_LCD_ENABLED


#ifdef DOG_LCD_ENABLED
//------------------------------------------------------------------------------------------------
// void TurnOnLedShowRssi(U8 node, U8 rssi)
//
// turn on the LED and print RSSI info for the given slave
//
// Return Value : None
// Parameters	: node - slave address
//				rssi - rssi of the last received slave node
//
//-----------------------------------------------------------------------------------------------
void LcdShowRssi(U8 node, U8 rssi)
{
	U8 rssi_address;

	if (node == 0)
		rssi_address = 13;
	else if (node == 1)
		rssi_address = 43;
	else if (node == 2)
		rssi_address = 73;
	else if (node == 3)
		rssi_address = 103;
	else
		rssi_address = 0;

	PrintRssi(1, rssi_address, 0);		// clear the previous RSSI
	PrintRssi(1, rssi_address, rssi);	// draw the bargraph
}
#endif

#ifdef DOG_LCD_ENABLED
//------------------------------------------------------------------------------------------------
// void PrintRssi(U8 row, U8 column, U8 rssi)
//
// print RSSI information to the LCD
//
// Return Value : None
// Parameters	: row - nmbr of row on the LCD
//				column - start position
//				rssi - rssi of the last received packet
//
//-----------------------------------------------------------------------------------------------
void PrintRssi(U8 row, U8 column, U8 rssi)
{
	LcdSetPictureCursor(row, column);		// Set picture position
	if (rssi == 0)							// Draw the picture
		LcdDrawPicture(rssi_clear_14x8);	// No packet were received
	else if (rssi < 55)
		LcdDrawPicture(rssi12x8);			// rssi1
	else if (rssi < 65)
		LcdDrawPicture(rssi24x8);			// rssi2
	else if (rssi < 75)
		LcdDrawPicture(rssi36x8);			// rssi3
	else if (rssi < 85)
		LcdDrawPicture(rssi48x8);			// rssi4
	else if (rssi < 95)
		LcdDrawPicture(rssi510x8);			// rssi5
	else if (rssi < 105)
		LcdDrawPicture(rssi612x8);			// rssi6
	else
		LcdDrawPicture(rssi714x8);			// rssi7
}
#endif //DOG_LCD_ENABLED

void SprintfSlaveInfo(SlaveInfoTable_t * slave, U8 * s)
{
	U8 tmp1;
	U8 tmp2;

	/* Valid values in Slave Info table. */
	if (slave->temperature != TEMP_INVALID_VALUE &&
		slave->voltage != VOLTAGE_INVALID_VALUE
		)
	{
		tmp1 = slave->temperature;
		// tmp2 = (((slave->voltage & 0x1F) - 1) >> 1) + 17;
		tmp2 = (((slave->voltage & 0x1F)) >> 1) + 17;
#ifdef __CC_ARM	// if compiled with Keil
		sprintf((char *)s, "S(%02u): %s%2u.%1u C	%2u.%1uV",
			slave->address.sfid,
			((tmp1 < TEMP_ZERO_VALUE) ? "-" : "+"),
			((tmp1 < TEMP_ZERO_VALUE) ? ((TEMP_ZERO_VALUE - tmp1)>>1) : ((tmp1 - TEMP_ZERO_VALUE) >> 1)),
			((tmp1 & 0x01) ? 5 : 0),
			(tmp2 / 10),
			(tmp2 % 10));
#endif
#ifdef __C51__	// if compiled with Keil
		sprintf(s, "S(%02bu): %s%2bu.%1bu C	%2bu.%1buV",
		slave->address.sfid,
			((tmp1 < TEMP_ZERO_VALUE) ? "-" : "+"),
			((tmp1 < TEMP_ZERO_VALUE) ? ((TEMP_ZERO_VALUE - tmp1)>>1) : ((tmp1 - TEMP_ZERO_VALUE) >> 1)),
			((tmp1 & 0x01) ? 5 : 0),
			(tmp2 / 10),
			(tmp2 % 10));
#endif //__C51__
#ifdef SDCC	 //if compiled with SDCC
		sprintf(s, "S(%01u%01u): %s%1u%1u.%1u C		%1u%1uV",
			(slave->address.sfid)/10, (slave->address.sfid) % 10,
			((tmp1 < TEMP_ZERO_VALUE) ? "-" : "+"),
			((tmp1 < TEMP_ZERO_VALUE) ? ((TEMP_ZERO_VALUE - tmp1) >> 1) / 10 : ((tmp1 - TEMP_ZERO_VALUE) >> 1) / 10),
			((tmp1 < TEMP_ZERO_VALUE) ? ((TEMP_ZERO_VALUE - tmp1) >> 1) % 10 : ((tmp1 - TEMP_ZERO_VALUE) >> 1) % 10),
			((tmp1 & 0x01) ? 5 : 0),
			(tmp2 / 10),
			(tmp2 % 10));
#endif //SDCC
	}
	else
	{
#ifdef __CC_ARM	//if compiled with Keil
		sprintf((char *)s, "S(%02u): ----- C	----V", slave->address.sfid);
#endif
#ifdef __C51__	//if compiled with Keil
		sprintf(s, "S(%02bu): ----- C	----V", slave->address.sfid);
#endif //__C51__
#ifdef SDCC	 //if compiled with SDCC
		sprintf(s, "S(%01u%01u): ----- C	----V", slave->address.sfid);
#endif //SDCC
	}
	s[12] = 0x7F;	/* Degree sign on the LCD. */
}

void SprintfSlaveNotAssoc(U8 * s)
{
#ifdef __C51__	//if compiled with Keil
	sprintf(s, "S(--): not associated");
#endif //__C51__
#ifdef SDCC	 //if compiled with SDCC
	sprintf(s, "S(--): not associated");
#endif //SDCC
}
