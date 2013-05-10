

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
    /* Initialise SPI0. */
    Spi1Init();
    /* Initialise Timer0. */
    Timer3Init();
    /* Initialise interrupts. */
    InterruptInit();
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

    P0SKIP      |= 0x40;            // skip P0.6
    P0MDOUT     |= 0x10;            // UART TX Push-Pull
    XBR0        |= 0x01;            // Enable UART

    XBR1        |= 0x40;            // Enable SPI1 (3 wire mode)
    P1MDOUT     |= 0x01;            // Enable SCK push pull
    P1MDOUT     |= 0x04;            // Enable MOSI push pull
    P1SKIP      |= 0x08;            // skip NSS
    P1MDOUT     |= 0x08;            // Enable NSS push pull
    P1SKIP      |= 0x40;            // skip TR_LED
    P1MDOUT     |= 0x40;            // Enable TR_LED push pull
    P2SKIP      |= 0x01;            // skip RC_LED
    P2MDOUT     |= 0x01;            // Enable RC_LED push pull
    P0SKIP      |= 0x02;            // skip SDN
    P0MDOUT     |= 0x02;            // Enable SDN push pull

    P0SKIP      |= 0x80;            // skip PB

    SFRPAGE     = CONFIG_PAGE;
    P0DRV       = 0x12;             // TX high current mode
    P1DRV       = 0x4D;             // MOSI, SCK, NSS, TR_LED high current mode
    P2DRV       = 0x01;             // RC_LED high current mode
    SFRPAGE     = LEGACY_PAGE;

    XBR2        |= 0x40;            // enable Crossbar

    SDN         = 0;
}


/*!
 * Initialise LEDs to OFF.
 */
void LEDInit(void)
{
    LED1 = EXTINGUISH;
    LED2 = EXTINGUISH;
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



