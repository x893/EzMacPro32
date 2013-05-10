
#ifndef _DOG_GLCD_H_
#define _DOG_GLCD_H_

                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

#define LCD_BACKLIGHT_IS_USED

#define LCD_MIN_LINE				(1)
#define LCD_MAX_LINE				(8)
#define LCD_MIN_CHAR				(1)
#define LCD_MAX_CHAR				(21)
#define LCD_MIN_COLUMN				(1)
#define LCD_MAX_COLUMN				(128)
#define LCD_PIC_PAGE_NMBR			(0)
#define	LCD_PIC_COLUMN_NMBR			(1)
#define LCD_PIC_ADDRESS_OFFSET		(2)

#define ASCII_5X7_MIN				(0x20)
#define ASCII_5X7_MAX				(0x7F)

#define LCD_LINE_1					(1)
#define LCD_LINE_2					(2)
#define LCD_LINE_3					(3)
#define LCD_LINE_4					(4)
#define LCD_LINE_5					(5)
#define LCD_LINE_6					(6)
#define LCD_LINE_7					(7)
#define LCD_LINE_8					(8)


                /* ======================================= *
                 *            V A R I A B L E S            *
                 * ======================================= */

                /* ======================================= *
                 *  F U N C T I O N   P R O T O T Y P E S  *
                 * ======================================= */

void LcdInit(void);
void LcdOn(void);
void LcdOff(void);
void LcdSetPage(U8 data_in);
void LcdSetColumn(U8 data_in);
void LcdClearDisplay(void);
void LcdClearLine(U8 line);
U8 LcdSetCharCursor(U8 line, U8 ch_pos);
void LcdPutCh(U8 ch);
void LcdPutInvCh(U8 ch);
void LcdPutChar(U8 line, U8 ch_pos, U8 ch);
void LcdWriteLine(U8 line, U8 * text);
U8 LcdSetPictureCursor(U8 page, U8 column);
void LcdDrawPicture(const U8 SEG_CODE* picture);

void SpiWriteLCD(U8 data_in);


#endif //_DOG_GLCD_H_
