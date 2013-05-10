

                /* ======================================= *
                 *              I N C L U D E              *
                 * ======================================= */

#include "bsp.h"


                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */

SEGMENT_VARIABLE(CurrentLine, U8, TEST_CODE_MSPACE);
SEGMENT_VARIABLE(CurrentChPos, U8, TEST_CODE_MSPACE);
SEGMENT_VARIABLE(CurrentPage, U8, TEST_CODE_MSPACE);
SEGMENT_VARIABLE(CurrentColumn, U8, TEST_CODE_MSPACE);


                /* ======================================= *
                 *     P U B L I C  F U N C T I O N S      *
                 * ======================================= */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdDelay_2ms(void)
  +
  + DESCRIPTION:    wait about 20ms, it uses only a for cycle
  +
  + RETURN:         None
  +
  + NOTES:          the delay depends on the clock of the MCU
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdDelay_2ms(void)
{
    Timer2BusyWait_ms(DELAY_2MS_TIMER2);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdInit(void)
  +
  + DESCRIPTION:    Initialize the LCD for 3.3V operation voltage and SPI comm.
  +
  + RETURN:         None
  +
  + NOTES:          it can be called only 40ms later than the VDD stabilized
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdInit(void)
{
  #ifdef LCD_BACKLIGHT_IS_USED
    LCD_BL_PIN 	= 1;
  #endif
	LCD_NSEL_PIN 	= 1;
	LCD_A0_PIN 		= 0;
	//perform reset
	LCD_RESET_PIN 	= 0;
	LcdDelay_2ms();
	LCD_RESET_PIN 	= 1;
	LcdDelay_2ms();
	LcdDelay_2ms();
	LcdDelay_2ms();
	LCD_NSEL_PIN = 0;
	SpiWriteLCD(0x40);			//display start line 0
	SpiWriteLCD(0xA1);			//ADC reverse
	SpiWriteLCD(0xC0);			//normal COM0~COM63
	SpiWriteLCD(0xA6);			//display normal
	SpiWriteLCD(0xA2);			//set bias 1/9 (Duty 1/65)
	SpiWriteLCD(0x2F);			//booster, regulator and follower on
	SpiWriteLCD(0xF8);			//set internal bosster to 4x
	SpiWriteLCD(0x00);
	SpiWriteLCD(0x27);			//contrast set
	SpiWriteLCD(0x81);
	SpiWriteLCD(0x16);
	SpiWriteLCD(0xAC);			//no indicator
	SpiWriteLCD(0x00);
	LCD_NSEL_PIN = 1;
	LcdOff();
	LcdClearDisplay();
	LcdOn();
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdOn(void)
  +
  + DESCRIPTION:    turns on the LCD screen
  +
  + RETURN:         None
  +
  + NOTES:
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdOn(void)
{
   EA = 0;                             // disable global interrupts
	LCD_NSEL_PIN = 0;
	SpiWriteLCD( 0xAF );
	LCD_NSEL_PIN = 1;
   EA = 1;                             // enable global interrupts
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdOff(void)
  +
  + DESCRIPTION:    turns off the LCD screen (the DDRAM content will be kept)
  +
  + RETURN:         None
  +
  + NOTES:
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdOff(void)
{
   EA = 0;                             // disable global interrupts
	LCD_NSEL_PIN = 0;
	SpiWriteLCD( 0xAE );
	LCD_NSEL_PIN = 1;
   EA = 1;                             // enable global interrupts
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdSetPage(U8 data_in)
  +
  + DESCRIPTION:    it sets the page address (0...7)
  +
  + INPUT:			page address (the LCD is separated to 8x8 rows
  +					and 8 row is calles as a page)
  +					page0 is the top 8 rows
  +
  + RETURN:         None
  +
  + NOTES:
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdSetPage(U8 data_in)
{
   EA = 0;                             // disable global interrupts
	LCD_NSEL_PIN = 0;
	SpiWriteLCD( 0xB0 | data_in );
	LCD_NSEL_PIN = 1;
   EA = 1;                             // enable global interrupts
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdSetColumn(U8 data_in)
  +
  + DESCRIPTION:    it sets the column address (0...127)
  +
  + INPUT:			address of the column
  +
  + RETURN:         None
  +
  + NOTES:
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdSetColumn(U8 data_in)
{
   EA = 0;                             // disable global interrupts
	LCD_NSEL_PIN = 0;
	SpiWriteLCD( 0x10 | ((data_in & 0x70) >> 4) );
	SpiWriteLCD( 0x00 | (data_in & 0x0F) );
	LCD_NSEL_PIN = 1;
   EA = 1;                             // enable global interrupts
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdClearDisplay(void)
  +
  + DESCRIPTION:    it clears the display (the content of the DDRAM!)
  +
  + INPUT:			None
  +
  + RETURN:         None
  +
  + NOTES:
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdClearDisplay(void)
{
	U8 page;

	for(page=1;page<9;page++)
	{
		LcdClearLine(page);
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdClearLine(U8 page)
  +
  + DESCRIPTION:    it clears one line on the LCD
  +
  + INPUT:			None
  +
  + RETURN:         None
  +
  + NOTES:
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdClearLine(U8 line)
{
	U8 column;

	if( (line < LCD_MIN_LINE) || (line > LCD_MAX_LINE) )
	{
		return;
	}
	//select the page
	LcdSetPage(line-1);
	//set to the first column
	LcdSetColumn(0);
   EA = 0;                             // disable global interrupts
	//set A0 to 1 -> access to the DDRAM
	LCD_A0_PIN 	 = 1;
	LCD_NSEL_PIN = 0;
	for(column=0;column<128;column++)
	{
		//clear the selected column
		SpiWriteLCD(0x00);
	}
	LCD_A0_PIN 	 = 0;
	LCD_NSEL_PIN = 1;
   EA = 1;                             // enable global interrupts
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdSetCharCursor(U8 line, U8 ch_pos)
  +
  + DESCRIPTION:    it sets the character position
  +
  + INPUT:			line   - number of the line (1...8
  +							 the LCD is divided to 8 lines
  +						     line1 is the top line
  +					ch_pos - character position
  +							 up to 21 character (1...21) could be in a line
  +							 character 1 is the first on left hand side
  +
  + RETURN:         TRUE   - operation was successfull
  +					FALSE  - operation was ignored
  +
  + NOTES:          If the position is invalid, the function returns without
  +					changing the registers.
  +					The function sets the CurrentLine, CurrentChPos variables!
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
U8 LcdSetCharCursor(U8 line, U8 ch_pos)
{
	//check whether the line and ch_pos valid or not
	if( ((line < LCD_MIN_LINE) || (line > LCD_MAX_LINE)) || ((ch_pos < LCD_MIN_CHAR) || (ch_pos > LCD_MAX_CHAR)) )
	{
		return 0;
	}

	//set page address
	LcdSetPage(line-1);
	//set column address
	LcdSetColumn( ((ch_pos-1)*6) );
	CurrentLine = line;
	CurrentChPos = ch_pos;

	return 1;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdPutInvCh(U8 ch)
  +
  + DESCRIPTION:    it writes one character INVERTED to the next position
  +
  + INPUT:			ch	   - the character needs to be writen
  +
  + RETURN:         None
  +
  + NOTES:          if the position is invalid (the line is full) it writes
  +					the character from the begining of the line.
  +					The function increments the CurrentChPos variable!
  +					If the character invalid, it prints a space char.
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdPutInvCh(U8 ch)
{
	U8 column, data_out, temp8;

	if( (ch > ASCII_5X7_MAX) || (ch < ASCII_5X7_MIN) )
	{
		data_out = ' ';
	}
	else
	{
		data_out = ch;
	}

	//write character
   EA = 0;                             // disable global interrupts
	LCD_A0_PIN 	 = 1;			//set A0 to 1 -> access to the DDRAM
	LCD_NSEL_PIN = 0;
	for(column=0;column<5;column++)
	{
		//write column data
		temp8 = ascii_table5x7[data_out - ASCII_5X7_MIN][column];
		temp8 ^= 0xFF;
		temp8 &= 0x7F;
		SpiWriteLCD( temp8 );
	}
	//space between the characters
	SpiWriteLCD(0x7F);
	LCD_A0_PIN 	 = 0;
	LCD_NSEL_PIN = 1;
   EA = 1;                             // enable global interrupts
	if(	++CurrentChPos > LCD_MAX_CHAR )
	{//end of the line -> set cursor to the beginning of the line
		CurrentChPos = 1;
		LcdSetColumn( 1 );
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdPutChar(U8 line, U8 ch_pos, U8 ch)
  +
  + DESCRIPTION:    it writes one character to the next position
  +
  + INPUT:			ch	   - the character needs to be writen
  +
  + RETURN:         None
  +
  + NOTES:          if the position is invalid (the line is full) it writes
  +					the character from the begining of the line.
  +					The function increments the CurrentChPos variable!
  +					If the character invalid, it prints a space char.
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdPutCh(U8 ch)
{
	U8 column, data_out;

	if( (ch > ASCII_5X7_MAX) || (ch < ASCII_5X7_MIN) )
	{
		data_out = ' ';
	}
	else
	{
		data_out = ch;
	}

	//write character
   EA = 0;                             // disable global interrupts
	LCD_A0_PIN 	 = 1;			//set A0 to 1 -> access to the DDRAM
	LCD_NSEL_PIN = 0;
	for(column=0;column<5;column++)
	{
		//write column data
		SpiWriteLCD(ascii_table5x7[data_out - ASCII_5X7_MIN][column]);
	}
	//space between the characters
	SpiWriteLCD(0);
	LCD_A0_PIN 	 = 0;
	LCD_NSEL_PIN = 1;
   EA = 1;                             // enable global interrupts
	if(	++CurrentChPos > LCD_MAX_CHAR )
	{//end of the line -> set cursor to the beginning of the line
		CurrentChPos = 1;
		LcdSetColumn( 1 );
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdPutChar(U8 line, U8 ch_pos, U8 ch)
  +
  + DESCRIPTION:    it write one character to the requested position
  +
  + INPUT:			line   - number of the line (1...8
  +							 the LCD is divided to 8 lines
  +						     line1 is the top line
  +					ch_pos - character position
  +							 up to 21 character (1...21) could be in a line
  +							 character 1 is the first on left hand side
  +                 ch	   - the character needs to be writen
  +
  + RETURN:         None
  +
  + NOTES:          If the position is invalid, the function returns without
  +					changing the registers.
  +					The function sets the CurrentLine, CurrentChPos variables!
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdPutChar(U8 line, U8 ch_pos, U8 ch)
{
	if( LcdSetCharCursor(line, ch_pos) == 0 )
	{
		return;
	}
	LcdPutCh(ch);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdWriteLine(U8 line, U8 * text)
  +
  + DESCRIPTION:    it writes one complete line
  +
  + INPUT:			line   - number of the line (1...8
  +							 the LCD is divided to 8 lines
  +						     line1 is the top line
  +					text   - address of the string needs to be written
  +
  + RETURN:         None
  +
  + NOTES:          If the line is invalid, the function returns without any changes.
  +					The function doesn't set the CurrentLine, CurrentChPos variables!
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdWriteLine(U8 line, U8 * text)
{
	U8 i,column,temp8;

	if( (line < LCD_MIN_LINE) || (line > LCD_MAX_LINE) )
	{
		return;
	}
	//set page address
	LcdSetPage( line-1 );
	//set column address
	LcdSetColumn( 0 );
	for(i=0;i<21;i++)
	{
		if( (text[i] > ASCII_5X7_MAX) || (text[i] < ASCII_5X7_MIN) )
		{
			temp8 = ' ';
		}
		else
		{
			temp8 = text[i];
		}

		;
		//write character
      EA = 0;                             // disable global interrupts
		LCD_A0_PIN 	 = 1;			//set A0 to 1 -> access to the DDRAM
		LCD_NSEL_PIN = 0;
		for(column=0;column<5;column++)
		{
			//write column data
			SpiWriteLCD(ascii_table5x7[temp8 - ASCII_5X7_MIN][column]);
		}
		//space between the characters
		SpiWriteLCD(0);
		LCD_A0_PIN 	 = 0;
		LCD_NSEL_PIN = 1;
      EA = 1;                             // enable global interrupts
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  U8 LcdSetPictureCursor(U8 page, U8 column)
  +
  + DESCRIPTION:    it sets the character position
  +
  + INPUT:			page   - number of the pages (1...8
  +							 the LCD is divided to 8 pages
  +						     page1 is the top page
  +					column - number of start column
  +							 column1 is the left one
  +
  + RETURN:         TRUE   - operation was successfull
  +					FALSE  - operation was ignored
  +
  + NOTES:          If the position is invalid, the function returns without
  +					changing the registers.
  +					The function sets the CurrentLine, CurrentChPos variables!
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
U8 LcdSetPictureCursor(U8 page, U8 column)
{
	//check whether the input parameters are correct or not
	if( ((page < LCD_MIN_LINE) || (page > LCD_MAX_LINE)) || ((column < LCD_MIN_COLUMN) || (column > LCD_MAX_COLUMN)) )
	{
		return 0;
	}

	//set page address
	LcdSetPage(page-1);
	//set column address
	LcdSetColumn( column-1 );
	CurrentPage = page;
	CurrentColumn = column;

	return 1;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  void LcdDrawPicture(const * picture)
  +
  + DESCRIPTION:    it draw a picture
  +
  + INPUT:			picture - address of the picture (must be stored in the
  +							  FLASH
  +
  + RETURN:         None
  +
  + NOTES:          The U8 LcdSetPictureCursor(U8 page, U8 column) function
  +					has to be called before calling this function!
  +					The function changes the CurrentPage and CurrentColumn variables!
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void LcdDrawPicture(const U8 SEG_CODE* picture)
{
U8 p,pages,page,i,col;
U16 j;

	p = picture[LCD_PIC_PAGE_NMBR];
	//check wheter there are enough column for the picture or not
	if( (LCD_MAX_COLUMN - CurrentColumn + 1) < picture[LCD_PIC_COLUMN_NMBR] )
	{//there are not enough space for the pic -> limit to the available space
		col = (LCD_MAX_COLUMN - CurrentColumn + 1);
	}
	else
	{
		col = picture[LCD_PIC_COLUMN_NMBR];
	}
	//check wheter there are enough pages for the picture or not
	if( (LCD_MAX_LINE - CurrentPage + 1) < picture[LCD_PIC_PAGE_NMBR] )
	{
		page = (LCD_MAX_LINE - CurrentPage + 1);
	}
	else
	{
		page = picture[LCD_PIC_PAGE_NMBR];
	}

	//draw the picture
	for(pages=0;pages<page;pages++)
	{
      EA = 0;                             // disable global interrupts
		LCD_A0_PIN 	 = 1;			//set A0 to 1 -> access to the DDRAM
		LCD_NSEL_PIN = 0;
		j = LCD_PIC_ADDRESS_OFFSET + pages;
		for(i=0;i<col;i++)
		{
			//write column data
			SpiWriteLCD( picture[j] );
			j += p;
		}
		LCD_A0_PIN 	 = 0;
		LCD_NSEL_PIN = 1;
      EA = 1;                             // enable global interrupts
		//set next page
		LcdSetPictureCursor( ++CurrentPage,CurrentColumn );
	}
}


void SpiWriteLCD(U8 data_in)
{
   SPIF = 0;                           // clear SPIF
   SPI_DAT = data_in;                  // write SPI data
   while(!TXBMT);                      // wait on TXBMT
   while((SPI_CFG & 0x80) == 0x80);    // wait on SPIBSY
   SPIF = 0;                           // leave SPIF cleared
}



