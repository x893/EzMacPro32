#ifndef SPI_H
#define SPI_H


/*!
 * SPI clock divider definition.
 */
#if defined (SYSCLK__FREQ_24_500MHZ)
    #define SPI_CKR_VALUE 0x01             // SPI CLK SYSCLK/4
#else
    #define SPI_CKR_VALUE 0x00             // SPI CLK SYSCLK/2
#endif


void Spi0Init(void);
void Spi1Init(void);



void macSpiWriteReg(U8, U8);
U8   macSpiReadReg(U8);
void macSpiWriteFIFO(U8, VARIABLE_SEGMENT_POINTER(payload, U8, BUFFER_MSPACE));
void macSpiReadFIFO(U8, VARIABLE_SEGMENT_POINTER(payload, U8, BUFFER_MSPACE));

void extIntSpiWriteReg (U8, U8);
U8 extIntSpiReadReg (U8);
void extIntSpiWriteFIFO (U8, VARIABLE_SEGMENT_POINTER(payload, U8, BUFFER_MSPACE));
void extIntSpiReadFIFO (U8, VARIABLE_SEGMENT_POINTER(payload, U8, BUFFER_MSPACE));


void timerIntSpiWriteReg (U8, U8);
U8 timerIntSpiReadReg (U8);


#endif //SPI_H
