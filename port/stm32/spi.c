#include "bsp.h"

U8 spiWriteReadReg (U8 reg, U8 value)
{
	RF_NSS_LOW();
	SPI_READ();				// Reset RXNE bit from previous
	SPI_WAIT_TX_READY();	// Write register address
	SPI_WRITE(reg);
	SPI_WAIT_RX_READY();	// Read data
	SPI_READ();
	SPI_WAIT_TX_READY();	// Write value
	SPI_WRITE(value);
	SPI_WAIT_RX_READY();	// Read data
	value = SPI_READ();
	SPI_WAIT_BUSY();		// Wait for BUSY (can remove)
	RF_NSS_HIGH();
	return value;
}

U8 macSpiWriteReadReg (U8 reg, U8 value)
{
	U8 restoreTIM = GET_MAC_TIMER_INTERRUPT();
	U8 restoreEXT = GET_MAC_EXT_INTERRUPT();
	DISABLE_MAC_INTERRUPTS();

	value = spiWriteReadReg(reg, value);

	SET_MAC_TIMER_INTERRUPT(restoreTIM);
	SET_MAC_EXT_INTERRUPT(restoreEXT);

	return value;
}

//================================================================================================
//
// spi Functions for EZMacPro.c module
//
//================================================================================================
//
// Notes:
//
// The spi functions in this module are for use in the main thread. The EZMacPro API calls should
// only be used in the main thread. The SPI is used by the main thread, as well as the external
// interrupt INT0 thread, and the T0 interrupt. Since all SPI tranfers are multiple bytes. It is
// important that MAC interrupts are disabled when using the SPI from the main thread.
//
// These SPI functions may be interrupted by other interrupts, so the double buffered transfers
// are managed with this in mind.
//
// The double buffered transfer maximizes the data throughput and elimiates any software
// delay between bytes. The clock is continuous for the two byte transfer. Instead of using the
// SPIF flag for each byte, the TXBMT is used to keep the transmit buffer full, then the SPIBSY
// bit is used to determine when all bits have been transfered. The SPIF flag should not be
// polled in double buffered transfers.
//
//------------------------------------------------------------------------------------------------
// Function Name: macSpiWriteReg()
//						Write a register of the radio.
// Return Values: None
// Parameters	 :	U8 reg - register address from the si4432.h file.
//    				U8 value - value to write to register
// Notes:
//
//    MAC interrupts are preserved and restored.
//    Write uses a Double buffered transfer.
//-----------------------------------------------------------------------------------------------
void macSpiWriteReg (U8 reg, U8 value)
{
	macSpiWriteReadReg(reg | 0x80, value);
}

//------------------------------------------------------------------------------------------------
// Function Name: macSpiReadReg()
//						Read a register from the radio.
//
// Return Value : U8 value - value returned from the si4432 register
// Parameters   : U8 reg - register address from the si4432.h file.
//
//-----------------------------------------------------------------------------------------------
U8 macSpiReadReg (U8 reg)
{
	return macSpiWriteReadReg(reg, 0);
}

//------------------------------------------------------------------------------------------------
// Function Name: macSpiWriteFIFO()
//						Write the FIFO of the radio.
//
// Return Value : None
// Parameters   :	n - the length of trasnmitted bytes
//						buffer - the transmitted bytes
//
//-----------------------------------------------------------------------------------------------
void macSpiWriteFIFO (U8 n, VARIABLE_SEGMENT_POINTER(buffer, U8, BUFFER_MSPACE))
{
	U8 restoreTIM = GET_MAC_TIMER_INTERRUPT();
	U8 restoreEXT = GET_MAC_EXT_INTERRUPT();
	DISABLE_MAC_INTERRUPTS();

	RF_NSS_LOW();
	SPI_WAIT_TX_READY();
	SPI_WRITE(0x80 | SI4432_FIFO_ACCESS);
	while(n--)
	{
		SPI_WAIT_TX_READY();
		SPI_WRITE(*buffer++);
	}
	SPI_WAIT_BUSY();
	RF_NSS_HIGH();

	SET_MAC_TIMER_INTERRUPT(restoreTIM);
	SET_MAC_EXT_INTERRUPT(restoreEXT);
}

//================================================================================================
//
// spi Functions for externalInt.c module
//
//================================================================================================
//
// Notes:
//
// The spi functions in this module are for use in the externalInt thread. The SPI is used by the
// main thread, as well as the external interrupt INT0 thread, and the T0 interrupt. The SPI
// functions are duplicated for the externalInt module. If the same SPI functions were used, the
// linker would generate a multiple call to segment warning. The linker would not be able to
// overlay the threads separately, local data may be corrupted be a reentrant function call,
// the SPI transfer may be corrupted.
//
// These SPI functions may be interrupted by a high priority interrupt, so the double buffered
// transfers are managed with this in mind.
//
// The double buffered transfer maximizes the data throughput and eliminates any software
// delay between bytes. The clock is continuous for the two byte transfer. Instead of using the
// SPIF flag for each byte, the TXBMT is used to keep the transmit buffer full, then the SPIBSY
// bit is used to determine when all bits have been transferred. The SPIF flag should not be
// polled in double buffered transfers.
//
//------------------------------------------------------------------------------------------------
// Function Name
//    extIntSpiWriteReg()
//
// Return Value   : None
// Parameters :
//    U8 reg - register address from the si4432.h file.
//    U8 value - value to write to register
//
// Notes:
//    Write uses a Double buffered transfer.
//
//-----------------------------------------------------------------------------------------------
void SpiWriteReg (U8 reg, U8 value) __attribute__((alias("extIntSpiWriteReg")));
void timerIntSpiWriteReg (U8 reg, U8 value) __attribute__((alias("extIntSpiWriteReg")));
void extIntSpiWriteReg   (U8 reg, U8 value)
{
	spiWriteReadReg(reg | 0x80, value);
}

//------------------------------------------------------------------------------------------------
// Function Name
//    extIntSpiReadReg()
//
// Return Value : U8 value - value returned from the si4432 register
// Parameters   : U8 reg - register address from the si4432.h file.
//
// Notes:
//    Read uses a Double buffered transfer.
//
//-----------------------------------------------------------------------------------------------
U8 SpiReadReg (U8 reg) __attribute__((alias("extIntSpiReadReg")));
U8 timerIntSpiReadReg (U8 reg) __attribute__((alias("extIntSpiReadReg")));
U8 extIntSpiReadReg   (U8 reg)
{
	return spiWriteReadReg(reg, 0);
}

//------------------------------------------------------------------------------------------------
// Function Name
//    extIntSpiWriteFIFO()
//
// Return Value : None
// Parameters   :
//    U8 n - the number of bytes to be written
//    *buffer - pointer to address of write buffer
//
// Notes:
//    Write FIFO uses Double buffered transfers.
//    The WriteFIFO function is only included if packet forwarding is defined.
//    Packet forwarding requires writing the forward packet back to the TX buffer.
//
//-----------------------------------------------------------------------------------------------
#ifdef PACKET_FORWARDING_SUPPORTED
void extIntSpiWriteFIFO (U8 n, VARIABLE_SEGMENT_POINTER(buffer, U8, BUFFER_MSPACE))
{
	RF_NSS_LOW();
	SPI_WAIT_TX_READY();
	SPI_WRITE(0x80 | SI4432_FIFO_ACCESS);
	while (n--)
	{
		SPI_WAIT_TX_READY();
		SPI_WRITE(*buffer++);
	}
	SPI_WAIT_BUSY();
	RF_NSS_HIGH();
}
#endif

//------------------------------------------------------------------------------------------------
// Function Name
//    extIntSpiReadFIFO()
//
// Return Value : None
// Parameters   :
//
// Notes:
//    This function does not use double buffered data transfers to prevent data loss.
//    This function may be interrupted by another process and should not loose data
//    or hang on the SPIF flag.
//
//    This function is not included for the Transmitter only configuration.
//
//-----------------------------------------------------------------------------------------------
#ifndef TRANSMITTER_ONLY_OPERATION
void extIntSpiReadFIFO (U8 n, VARIABLE_SEGMENT_POINTER(buffer, U8, BUFFER_MSPACE))
{
	RF_NSS_LOW();
	SPI_READ();
	SPI_WAIT_TX_READY();
	SPI_WRITE(SI4432_FIFO_ACCESS);
	SPI_WAIT_RX_READY();
	SPI_READ();
	while (n--)
	{
		SPI_WAIT_TX_READY();
		SPI_WRITE(0);
		SPI_WAIT_RX_READY();
		*buffer++ = SPI_READ();
	}
	SPI_WAIT_BUSY();
	RF_NSS_HIGH();
}
#endif

//================================================================================================
//
// spi Functions for timerInt.c module
//
//================================================================================================
//
// Notes:
//
// The spi functions in this module are for use in the timerInt thread. The SPI is used by the
// main thread, as well as the external interrupt INT0 thread, and the T0 interrupt. The SPI
// functions are duplicated for the timerInt module. If the same SPI functions were used, the
// linker would generate a multiple call to segment warning. The linker would not be able to
// overlay the threads separately, local data may be corrupted be a reentrant function call,
// the SPI transfer may be corrupted.
//
// These SPI functions may be interrupted by a high priority interrupt, so the double buffered
// transfers are managed with this in mind.
//
// The double buffered transfer maximizes the data throughput and eliminates any software
// delay between bytes. The clock is continuous for the two byte transfer. Instead of using the
// SPIF flag for each byte, the TXBMT is used to keep the transmit buffer full, then the SPIBSY
// bit is used to determine when all bits have been transferred. The SPIF flag should not be
// polled in double buffered transfers.
//
//------------------------------------------------------------------------------------------------
// Function Name
//    timerIntSpiWriteReg()
//
// Return Value   : None
// Parameters :
//    U8 reg - register address from the si4432.h file.
//    U8 value - value to write to register
//
// Notes:
//    Write uses a Double buffered transfer.
//    This function is not included in the Transmitter only configuration.
//
//-----------------------------------------------------------------------------------------------
// void timerIntSpiWriteReg (U8 reg, U8 value)
// {
//	spiWriteReadReg(reg | 0x80, value);
// }
//------------------------------------------------------------------------------------------------
// Function Name
//    timerIntSpiReadReg()
//
// Return Value : U8 value - value returned from the si4432 register
// Parameters   : U8 reg - register address from the si4432.h file.
//
// Notes:
//    Read uses a Double buffered transfer.
//    This function is not included for the Transmitter only configuration.
//
//-----------------------------------------------------------------------------------------------
// U8 timerIntSpiReadReg (U8 macReg)
// {
//	return spiWriteReadReg(reg, value);
// }
