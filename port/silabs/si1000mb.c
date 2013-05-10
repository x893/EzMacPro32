

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
    U16 delay;

    /* Init LPOSC to 10 MHZ clock for now. */
    CLKSEL      = 0x14;

    XBR0        |= 0x01;            // Enable UART
    P0MDOUT     |= 0x10;            // UART TX Push-Pull
    P0SKIP      |= 0xCF;            // skip P0.0-3 & 0.6-7

    XBR1        |= 0x40;            // Enable SPI1 (3 wire mode)
    P1MDOUT     |= 0x01;            // Enable SCK push pull
    P1MDOUT     |= 0x04;            // Enable MOSI push pull
    P1SKIP      |= 0x08;            // skip P1.3
    P1SKIP      |= 0x10;            // skip NSS
    P1MDOUT     |= 0x10;            // Enable NSS push pull
    P1SKIP      |= 0x60;            // skip LED1 and LED2

    P2MDOUT     |= 0x40;            // SDN push pull

    SFRPAGE     = CONFIG_PAGE;
    P0DRV       = 0x10;             // TX high current mode
    P1DRV       = 0x75;             // MOSI, SCK, NSS, LEDs high current mode
    P2DRV       = 0x40;             // SDN high current
    SFRPAGE     = LEGACY_PAGE;

    XBR2        |= 0x40;            // enable Crossbar

    SDN         = 0;

    // SND delay (3907/156250=25 ms)
    CLKSEL      = 0x74;
    delay       = 3907;
    do{}while (--delay);
    CLKSEL      = 0x14;
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
//   IT01CF       = 0x06;                     // INT0 mapped to P0.6 active low
//   TCON         &= ~0x03;                   // clear IE0 & IT0
    IT01CF    = 0x01;                   // INT0 mapped to P0.1 active low
    TCON &= ~0x03;                      // clear IE0 & IT0
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



