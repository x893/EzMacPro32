#ifndef _HARDWARE_DEFS_H_
#define _HARDWARE_DEFS_H_

#define LED1_ON()	LED1 = ILLUMINATE
#define LED1_OFF()	LED1 = EXTINGUISH
#define LED2_ON()	LED1 = ILLUMINATE
#define LED2_OFF()	LED1 = EXTINGUISH

                /* ======================================= *
                 *    S O F T W A R E  D E V  B O A R D    *
                 * ======================================= */

#ifdef SOFTWARE_DEVELOPMENT_BOARD

/*!
 * System Clock selection.
 */
#define SYSCLK__FREQ_24_500MHZ
//#define SYSCLK__FREQ_16_000MHZ
//#define SYSCLK__FREQ_8_000MHZ
//#define SYSCLK__FREQ_4_000MHZ


/*!
 * Pin definitions.
 *
 * P0.6  -  RF_IRQ EI0
 * P1.0  -  SCK  (SPI1), Push-Pull,  Digital
 * P1.1  -  MISO (SPI1), Open-Drain, Digital
 * P1.2  -  MOSI (SPI1), Push-Pull,  Digital
 * P1.3  -  NSS  (SPI1), Push-Pull,  Digital
 */
SBIT(NSS, SFR_P1, 3);
SBIT(IRQ, SFR_P0, 6);
SBIT(PB1, SFR_P0, 0);
SBIT(PB2, SFR_P0, 1);
SBIT(PB3, SFR_P2, 0);
SBIT(PB4, SFR_P2, 1);
SBIT(LED1, SFR_P1, 4);
SBIT(LED2, SFR_P1, 5);
SBIT(LED3, SFR_P1, 6);
SBIT(LED4, SFR_P1, 7);

SBIT(LCD_BL_PIN, SFR_P2, 7);
SBIT(LCD_NSEL_PIN, SFR_P2, 5);
SBIT(LCD_A0_PIN, SFR_P2, 3);
SBIT(LCD_RESET_PIN, SFR_P2, 4);

#define ILLUMINATE                      1
#define EXTINGUISH                      0

/*!
 * C8051F930 Hardware Macros.
 */
#define INTERRUPT_SPI            INTERRUPT_SPI1
#define SPIF                     SPIF1
#define TXBMT                    TXBMT1
#define SPI_DAT                  SPI1DAT
#define SPI_CN                   SPI1CN
#define SPI_CFG                  SPI1CFG
#define SPI_CKR                  SPI1CKR

#endif//SOFTWARE_DEVELOPMENT_BOARD


                /* ======================================= *
                 *        E Z L I N K   M O D U L E        *
                 * ======================================= */

#ifdef EZLINK_MODULE

/*!
 * System Clock selection.
 */
#define SYSCLK__FREQ_24_500MHZ


/*!
 * Pin definitions.
 *
 * P0.6  -  RF_IRQ EI0
 * P1.0  -  SCK  (SPI0), Push-Pull,  Digital
 * P1.1  -  MISO (SPI0), Open-Drain, Digital
 * P1.2  -  MOSI (SPI0), Push-Pull,  Digital
 * P1.3  -  NSS  (SPI0), Push-Pull,  Digital
 */
SBIT(NSS,   SFR_P1, 3);
SBIT(IRQ,   SFR_P0, 6);
SBIT(SDN,   SFR_P0, 1);
SBIT(LED1,  SFR_P2, 0);
SBIT(LED2,  SFR_P1, 6);
SBIT(PB1,   SFR_P0, 7);

#define ILLUMINATE                      1
#define EXTINGUISH                      0

/*!
 * C8051F930 Hardware Macros.
 */
#define INTERRUPT_SPI            INTERRUPT_SPI0
#define SPIF                     SPIF1
#define TXBMT                    TXBMT1
#define SPI_DAT                  SPI1DAT
#define SPI_CN                   SPI1CN
#define SPI_CFG                  SPI1CFG
#define SPI_CKR                  SPI1CKR

#endif//EZLINK_MODULE


                /* ======================================= *
                 *            S I 1 0 0 0   M B            *
                 * ======================================= */

#ifdef SI1000_DAUGHTERCARD_SI1000_MOTHERBOARD

/*!
 * System Clock selection.
 */
#define SYSCLK__FREQ_24_500MHZ


/*!
 * Pin definitions.
 *
 * P0.1  -  RF_IRQ EI0
 * P1.0  -  SCK  (SPI0), Push-Pull,  Digital
 * P1.1  -  MISO (SPI0), Open-Drain, Digital
 * P1.2  -  MOSI (SPI0), Push-Pull,  Digital
 * P1.4  -  NSS  (SPI0), Push-Pull,  Digital
 * P2.6  -  SDN          Push-Pull,  Digital
 */
SBIT(IRQ, SFR_P0, 1);
SBIT(NSS, SFR_P1, 4);
SBIT(SDN, SFR_P2, 6);
SBIT(PB1, SFR_P0, 2);
SBIT(PB2, SFR_P0, 3);
// PB3 undefined - Si1000 motherboard only has two switches
// PB4 undefined - Si1000 motherboard only has two switches
SBIT(LED1, SFR_P1, 5);
SBIT(LED2, SFR_P1, 6);
// LED3 undefined - Si1000 motherboard only has two LEDs
// LED4 undefined - Si1000 motherboard only has two LEDs

#define ILLUMINATE                      0
#define EXTINGUISH                      1

/*!
 * C8051F930 Hardware Macros.
 */
#define INTERRUPT_SPI            INTERRUPT_SPI0
#define SPIF                     SPIF1
#define TXBMT                    TXBMT1
#define SPI_DAT                  SPI1DAT
#define SPI_CN                   SPI1CN
#define SPI_CFG                  SPI1CFG
#define SPI_CKR                  SPI1CKR

#endif//SI1000_DAUGHTERCARD_SI1000_MOTHERBOARD


                /* ======================================= *
                 *            S I 1 0 1 0   M B            *
                 * ======================================= */

#ifdef SI1010_DAUGHTERCARD_SI1000_MOTHERBOARD

/*!
 * System Clock selection.
 */
#define SYSCLK__FREQ_24_500MHZ


/*!
 * Pin definitions.
 *
 * P0.1  -  RF_IRQ EI0
 * P1.0  -  SCK  (SPI0), Push-Pull,  Digital
 * P1.1  -  MISO (SPI0), Open-Drain, Digital
 * P1.2  -  MOSI (SPI0), Push-Pull,  Digital
 * P1.3  -  NSS  (SPI0), Push-Pull,  Digital
 * P1.6  -  SDN          Push-Pull,  Digital
 */
SBIT(IRQ, SFR_P0, 1);
SBIT(NSS, SFR_P1, 3);
SBIT(SDN, SFR_P1, 6);
SBIT(PB1, SFR_P0, 2);
SBIT(PB2, SFR_P0, 3);
// PB3 undefined - Si1000 motherboard only has two switches
// PB4 undefined - Si1000 motherboard only has two switches
SBIT(LED1, SFR_P1, 5);
//SBIT(LED2, SFR_P1, 6); // P1.6 used for SDN
// LED3 undefined - Si1000 motherboard only has two LEDs
// LED4 undefined - Si1000 motherboard only has two LEDs

#define ILLUMINATE                      0
#define EXTINGUISH                      1

/*!
 * C8051F930 Hardware Macros.
 */
#define INTERRUPT_SPI            INTERRUPT_SPI0
#define SPIF                     SPIF1
#define TXBMT                    TXBMT1
#define SPI_DAT                  SPI1DAT
#define SPI_CN                   SPI1CN
#define SPI_CFG                  SPI1CFG
#define SPI_CKR                  SPI1CKR

#endif//SI1010_DAUGHTERCARD_SI1000_MOTHERBOARD


                /* ======================================= *
                 *   C O M M O N   D E F I N I T I O N S   *
                 * ======================================= */

#define TRUE                            1
#define FALSE                           0


/*!
 * Disable watchdog.
 */
#define DISABLE_WATCHDOG()              PCA0MD &= ~0x40


/*!
 * Interrupt macros.
 */
#define ENABLE_GLOBAL_INTERRUPTS()      EA=1
#define DISABLE_GLOBAL_INTERRUPTS()     EA=0

#define ENABLE_EXT0_INTERRUPT()         EX0=1
#define DISABLE_EXT0_INTERRUPT()        EX0=0
#define CLEAR_EXT0_INTERRUPT()          IE0=0

#define ENABLE_MAC_INTERRUPTS()         ENABLE_EXT0_INTERRUPT(); \
                                        ENABLE_TIMER3_INTERRUPT()
#define DISABLE_MAC_INTERRUPTS()        DISABLE_EXT0_INTERRUPT(); \
                                        DISABLE_TIMER3_INTERRUPT()

#define ENABLE_MAC_EXT_INTERRUPT()      ENABLE_EXT0_INTERRUPT()
#define DISABLE_MAC_EXT_INTERRUPT()     DISABLE_EXT0_INTERRUPT()
#define CLEAR_MAC_EXT_INTERRUPT()       CLEAR_EXT0_INTERRUPT()

#define ENABLE_UART_INTERRUPT()         ENABLE_UART0_INTERRUPT()
#define SET_UART_INTERRUPT_FLAG()       SET_UART0_INTERRUPT_FLAG()
#define CLEAR_UART_INTERRUPT_FLAG()     CLEAR_UART0_INTERRUPT_FLAG()


/*!
 * UART baud rate.
 */
#define UART0_BAUDRATE                   (115200L)
//#define UART0_BAUDRATE                   (19200L)


/*!
 * System Clock and frequency divider definition.
 */
#if defined (SYSCLK__FREQ_24_500MHZ)            /* Int. Osc. @ 24.5MHz */
  #define SYSCLK_HZ                     (24500000)
  #define SYSCLK_KHZ                    (SYSCLK_HZ/1000)
  #define SYSCLK_DIV                    (0x00)
#elif defined (SYSCLK__FREQ_16_000MHZ)          /* Xtal @ 16MHz */
  #define SYSCLK_HZ                     (16000000)
  #define SYSCLK_KHZ                    (SYSCLK_HZ/1000)
  #define SYSCLK_DIV                    (0x00)
  #ifndef SOFTWARE_DEVELOPMENT_BOARD
    #error "No XT2 on Si10xx testcard!"
  #endif //SOFTWARE_DEVELOPMENT_BOARD
#elif defined (SYSCLK__FREQ_8_000MHZ)           /* Xtal @ 8MHz */
  #define SYSCLK_HZ                     (8000000)
  #define SYSCLK_KHZ                    (SYSCLK_HZ/1000)
  #define SYSCLK_DIV                    (0x10)
  #ifndef SOFTWARE_DEVELOPMENT_BOARD
    #error "No XT2 on Si10xx testcard!"
  #endif //SOFTWARE_DEVELOPMENT_BOARD
#elif defined (SYSCLK__FREQ_4_000MHZ)           /* Xtal @ 4MHz */
  #define SYSCLK_HZ                     (4000000)
  #define SYSCLK_KHZ                    (SYSCLK_HZ/1000)
  #define SYSCLK_DIV                    (0x20)
  #ifndef SOFTWARE_DEVELOPMENT_BOARD
    #error "No XT2 on Si10xx testcard!"
  #endif //SOFTWARE_DEVELOPMENT_BOARD
#else
  #error "System Clock not defined!"
#endif


#endif //_HARDWARE_DEFS_H_

