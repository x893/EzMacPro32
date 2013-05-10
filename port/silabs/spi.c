

#include "bsp.h"


/*!
 * Init SPI0.
 */
void Spi0Init(void)
{
    SPI0CFG = 0x70;             // data sampled on rising edge, clock active low
                                // 8-bit data words, master mode
    SPI0CN  = 0x0F;             // 4-wire mode; SPI enabled; flags cleared
    SPI0CKR = 119;              // initialise SPI prescaler
    NSS0MD0 = 1;
}

/*!
 * Init SPI1.
 */
void Spi1Init(void)
{
    SPI1CFG = 0x40;                    // master mode
    SPI1CN  = 0x00;                    // 3 wire master mode
    SPI1CKR = SPI_CKR_VALUE;           // initialize SPI prescaler
    SPI1CN  |= 0x01;                   // enable SPI
    NSS     = 1;                       // set NSS high
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
   U8 restoreInts;

   // Disable MAC interrupts
   restoreInts = IE & 0x03;           // save EX0 & ET0 state
   IE &= ~0x03;                       // clear EX0 & ET0

   // Send SPI data using double buffered write
   NSS = 0;                            // drive NSS low
   SPIF = 0;                           // clear SPIF
   SPI_DAT = (reg | 0x80);             // write reg address
   while(!TXBMT);                      // wait on TXBMT
   SPI_DAT = value;                    // write value
   while(!TXBMT);                      // wait on TXBMT
   while((SPI_CFG & 0x80) == 0x80);    // wait on SPIBSY

   SPIF = 0;                           // leave SPIF cleared
   NSS = 1;                            // drive NSS high

   // Restore MAC interrupts
   IE |= restoreInts;                  // restore EX0 & ET0
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
   U8 value;
   U8 restoreInts;

   // Disable MAC interrupts
   restoreInts = IE & 0x03;             // save EX0 & ET0 state
   IE &= ~0x03;                         // clear EX0 & ET0

   // Send SPI data using double buffered write
   NSS = 0;                            // dsrive NSS low
   SPIF = 0;                           // cleat SPIF
   SPI_DAT = ( reg );                  // write reg address
   while(!TXBMT);                      // wait on TXBMT
   SPI_DAT = 0x00;                     // write anything
   while(!TXBMT);                      // wait on TXBMT
   while((SPI_CFG & 0x80) == 0x80);    // wait on SPIBSY
   value = SPI_DAT;                    // read value
   SPIF = 0;                           // leave SPIF cleared
   NSS = 1;                            // drive NSS low

   // Restore MAC interrupts
   IE |= restoreInts;                  // restore EX0 & ET0

   return value;
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
   U8 restoreInts;

   // Disable MAC interrupts
   restoreInts = (IE & 0x03);          // save EX0 & ET0 state
   IE &= ~0x03;                        // clear EX0 & ET0

   NSS = 0;                            // drive NSS low
   SPIF = 0;                           // clear SPIF
   SPI_DAT = (0x80 | SI4432_FIFO_ACCESS);

   while(n--)
   {
      while(!TXBMT);                   // wait on TXBMT
      SPI_DAT = *buffer++;             // write buffer
   }

   while(!TXBMT);                      // wait on TXBMT
   while((SPI_CFG & 0x80) == 0x80);    // wait on SPIBSY

   SPIF = 0;                           // leave SPI  cleared
   NSS = 1;                            // drive NSS high

    // Restore MAC interrupts
    IE |= restoreInts;                  // restore EX0 & ET0
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
void extIntSpiWriteReg (U8 macReg, U8 value)
{
   NSS = 0;                            // drive NSS low
   SPIF = 0;                           // clear SPIF
   SPI_DAT = (macReg | 0x80);          // write reg address
   while(!TXBMT);                      // wait on TXBMT
   SPI_DAT = value;                    // write value
   while(!TXBMT);                      // wait on TXBMT
   while((SPI_CFG & 0x80) == 0x80);    // wait on SPIBSY

   SPIF = 0;                           // leave SPIF cleared
   NSS = 1;                            // drive NSS high
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
U8 extIntSpiReadReg (U8 macReg)
{
   U8 value;

   NSS = 0;                            // drive NSS low
   SPIF = 0;                           // cleat SPIF
   SPI_DAT = ( macReg );               // write reg address
   while(!TXBMT);                      // wait on TXBMT
   SPI_DAT = 0x00;                     // write anything
   while(!TXBMT);                      // wait on TXBMT
   while((SPI_CFG & 0x80) == 0x80);    // wait on SPIBSY
   value = SPI_DAT;                    // read value
   SPIF = 0;                           // leave SPIF cleared
   NSS = 1;                            // drive NSS low

   return value;
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
   NSS = 0;                            // drive NSS low
   SPIF = 0;                           // clear SPIF
   SPI_DAT = (0x80 | SI4432_FIFO_ACCESS);

   while(n--)
   {
      while(!TXBMT);                   // wait on TXBMT
      SPI_DAT = *buffer++;             // write buffer
   }

   while(!TXBMT);                      // wait on TXBMT
   while((SPI_CFG & 0x80) == 0x80);    // wait on SPIBSY

   SPIF = 0;                           // leave SPI  cleared
   NSS = 1;                            // drive NSS high
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
   NSS = 0;                            // drive NSS low
   SPIF = 0;                           // clear SPIF
   SPI_DAT = (SI4432_FIFO_ACCESS);
   while(!SPIF);                       // wait on SPIF
   ACC = SPI_DAT;                      // discard first byte

   while(n--)
   {
      SPIF = 0;                        // clear SPIF
      SPI_DAT = 0x00;                  // write anything
      while(!SPIF);                    // wait on SPIF
      *buffer++ = SPI_DAT;               // copy to buffer
   }

   SPIF = 0;                           // leave SPIF cleared
   NSS = 1;                            // drive NSS high
}
#endif // TRANSMITTER_ONLY_OPERATION not defined


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
void timerIntSpiWriteReg (U8 macReg, U8 value)
{
   NSS = 0;                            // drive NSS low
   SPIF = 0;                           // clear SPIF
   SPI_DAT = (macReg | 0x80);          // write reg address
   while(!TXBMT);                      // wait on TXBMT
   SPI_DAT = value;                    // write value
   while(!TXBMT);                      // wait on TXBMT
   while((SPI_CFG & 0x80) == 0x80);    // wait on SPIBSY

   SPIF = 0;                           // leave SPIF cleared
   NSS = 1;                            // drive NSS high
}
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
U8 timerIntSpiReadReg (U8 macReg)
{
   U8 value;

   NSS = 0;                            // drive NSS low
   SPIF = 0;                           // cleat SPIF
   SPI_DAT = ( macReg );               // write reg address
   while(!TXBMT);                      // wait on TXBMT
   SPI_DAT = 0x00;                     // write anything
   while(!TXBMT);                      // wait on TXBMT
   while((SPI_CFG & 0x80) == 0x80);    // wait on SPIBSY
   value = SPI_DAT;                    // read value
   SPIF = 0;                           // leave SPIF cleared
   NSS = 1;                            // drive NSS low

   return value;
}













