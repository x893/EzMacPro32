#ifndef SPI_H
#define SPI_H

#define SPI_WRITE(data)		RF_SPI->DR = data
#define SPI_READ()			RF_SPI->DR
#define SPI_WAIT_TX_READY()	while (!(RF_SPI->SR & SPI_I2S_FLAG_TXE))
#define SPI_WAIT_RX_READY()	while (!(RF_SPI->SR & SPI_I2S_FLAG_RXNE))
#define SPI_WAIT_BUSY()		while (RF_SPI->SR & SPI_I2S_FLAG_BSY)

void SpiWriteReg(U8, U8);
U8   SpiReadReg(U8);

void macSpiWriteReg(U8, U8);
U8   macSpiReadReg(U8);

void macSpiWriteFIFO(U8, VARIABLE_SEGMENT_POINTER(payload, U8, BUFFER_MSPACE));
void macSpiReadFIFO(U8, VARIABLE_SEGMENT_POINTER(payload, U8, BUFFER_MSPACE));

void extIntSpiWriteReg (U8, U8);
U8   extIntSpiReadReg (U8);

void extIntSpiWriteFIFO (U8, VARIABLE_SEGMENT_POINTER(payload, U8, BUFFER_MSPACE));
void extIntSpiReadFIFO (U8, VARIABLE_SEGMENT_POINTER(payload, U8, BUFFER_MSPACE));

void timerIntSpiWriteReg (U8, U8);
U8   timerIntSpiReadReg (U8);

#endif //SPI_H
