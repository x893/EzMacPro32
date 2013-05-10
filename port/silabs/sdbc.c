

                /* ======================================= *
                 *              I N C L U D E              *
                 * ======================================= */

#include "bsp.h"


                /* ======================================= *
                 *     P U B L I C  F U N C T I O N S      *
                 * ======================================= */

/*!
 * Init board.
 */
void BoardInit(void)
{
    /* Initialise ports. */
    PortInit();
    /* Initialise system clock. */
    SystemClkInit();
    /* Initialise SPI1. */
    Spi1Init();
    /* Initialise Timer0. */
    Timer0Init();
    /* Initialise Timer3. */
    Timer3Init();
    /* Initialise interrupts. */
    InterruptInit();
    /* Initialise the LCD. */
    LcdInit();
    /* Clear LCD. */
    LcdClearDisplay();
    /* Initialise LEDs. */
    LEDInit();
  #ifdef RTC_ENABLED
    /* RTC init. */
    RTC_Init();
  #endif //RTC_ENABLED
  #ifdef UART0_ENABLED
    /* UART0 init. */
    Uart0Init();
  #endif //UART0_ENABLED
}


/*!
 * Init Port registers.
 */
void PortInit(void)
{
    /* Init LPOSC to 10 MHZ clock for now. */
    CLKSEL      = 0x14;

    /* Init Port registers. */
    P0MDIN      &= ~0x0C;           // P0.2 & P0.3 analog input for XTAL

    XBR1        |= 0x40;            // Enable SPI1 (3 wire mode)
    P1MDOUT     |= 0x01;            // Enable SCK push pull
    P1MDOUT     |= 0x04;            // Enable MOSI push pull
    P1SKIP      |= 0x08;            // skip NSS
    P1MDOUT     |= 0x08;            // Enable NSS push pull
    P1SKIP      |= 0xF0;            // skip LEDs
    P1MDOUT     |= 0xF0;            // Enable LEDS push pull

    P2MDOUT     |= 0x38;            // Enable LCD_NSEL, LCD_A0 and LCD_RESET push pull
    P2SKIP      |= 0xB8;            // Skip LCD_NSEL, LCD_A0 and LCD_RESET
    P2SKIP      |= 0x03;            // skip PB3 & PB4
    P2          = 0x03;

    SFRPAGE     = CONFIG_PAGE;
    P0DRV       = 0x10;             // TX high current mode
    P1DRV       = 0xFD;             // MOSI, SCK, NSS, LEDs high current mode
    SFRPAGE     = LEGACY_PAGE;

    XBR2        |= 0x40;            // enable Crossbar
}


/*!
 * Initialise LEDs to OFF.
 */
void LEDInit(void)
{
    LED1 = EXTINGUISH;
    LED2 = EXTINGUISH;
    LED3 = EXTINGUISH;
    LED4 = EXTINGUISH;
}


/*!
 * Initialise interrupts.
 */
void InterruptInit(void)
{
   IT01CF       = 0x06;                     // INT0 mapped to P0.6 active low
   TCON         &= ~0x03;                   // clear IE0 & IT0
}


/*!
 * Initialise system clock.
 */
void SystemClkInit(void)
{
#if defined (SYSCLK__FREQ_24_500MHZ)
    OSCICN      |= 0x80;
    CLKSEL      = SYSCLK_DIV;
    FLSCL       = 0x40;
    FLWR        = 0x01;
#elif defined (SYSCLK__FREQ_16_000MHZ)
    FLSCL       = 0x40;
    FLWR        = 0x01;
    OSCXCN      = 0x67;
    while ((OSCXCN & 0x80) == 0);
    CLKSEL      = (0x01 | SYSCLK_DIV);
#elif defined (SYSCLK__FREQ_8_000MHZ)
    OSCXCN      = 0x67;
    while ((OSCXCN & 0x80) == 0);
    CLKSEL      = (0x01 | SYSCLK_DIV);
#elif defined (SYSCLK__FREQ_4_000MHZ)
    OSCXCN      = 0x67;
    while ((OSCXCN & 0x80) == 0);
    CLKSEL      = (0x01 | SYSCLK_DIV);
#endif
    /* Wait for clock to get ready. */
    while(!(CLKSEL & 0x80));
}



