/*!\file EZMacPro_Defs.h
 * \brief EZMacPRO stack settings.
 *
 * \n EZMacPRO version: 3.0.1r
 *
 * \n This software must be used in accordance with the End User License
 * \n Agreement.
 *
 * \b COPYRIGHT
 * \n Copyright 2012 Silicon Laboratories, Inc.
 * \n http://www.silabs.com
 */

#ifndef _EZMACPRO_DEFS_H_
#define _EZMACPRO_DEFS_H_


                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

/*!
 * EZMacPRO version number.
 * EZMacPRO versioning convention: x.y.z[b/r]
 *  - x: major
 *  - y: minor
 *  - z: bugfix revision
 *  - b/r: beta/release
 */
#define EZMACPRO_VERSION	"3.0.1r"


/*!
 * These preprocessor symbols should be defined in the IDE or command line.
 */
//#define FREQUENCY_BAND_434
//#define FREQUENCY_BAND_868
//#define FREQUENCY_BAND_915

//#define TRANSCEIVER_OPERATION
//#define TRANSMITTER_ONLY_OPERATION
//#define RECEIVER_ONLY_OPERATION

//#define FOUR_CHANNEL_IS_USED
//#define MORE_CHANNEL_IS_USED

//#define STANDARD_PACKET_FORMAT
//#define EXTENDED_PACKET_FORMAT

//#define ANTENNA_DIVERSITY_ENABLED
//#define PACKET_FORWARDING_SUPPORTED


/*!
 * EZMacPRO packet and acknowledgement size definitions.
 */
#define RECEIVED_BUFFER_SIZE            64
#define ACK_BUFFER_SIZE                 16
#define ACK_PAYLOAD_DEFAULT_SIZE        1


/*!
 * Other EZMacPRO definitions.
 */
#define FORWARDED_PACKET_TABLE_SIZE     8

#define MAX_LBT_RETRIES                 2

#define EZMACPRO_ADC_GAIN               0x00

#define EZMACPRO_ADC_AMP_OFFSET         0x00

#define GPIO0_FUNCTION 	                0x12    //TX state
//#define GPIO0_FUNCTION                  0x15    //RX state

#ifdef ANTENNA_DIVERSITY_ENABLED
	#define GPIO1_FUNCTION	            0x17	//ANT1 SW
	#define GPIO2_FUNCTION	            0x18	//ANT2 SW
#else
    #define GPIO1_FUNCTION              0x14    //RX data
    #define GPIO2_FUNCTION              0x0F    //TRX CLK
#endif


/*!
 * Rules checking validity of preprocessor symbol combinations and packet
 * definitions.
 */
#ifndef STANDARD_PACKET_FORMAT
#ifndef EXTENDED_PACKET_FORMAT
#error "Either STANDARD_PACKET_FORMAT or EXTENDED_PACKET_FORMAT has to be defined!"
#endif //EXTENDED_PACKET_FORMAT
#endif //STANDARD_PACKET_FORMAT

#ifdef   EXTENDED_PACKET_FORMAT
#ifdef      STANDARD_PACKET_FORMAT
#error         "Only one packet format can be selected!"
#endif      // STANDARD_PACKET_FORMAT
#endif   // EXTENDED_PACKET_FORMAT

#ifndef  EXTENDED_PACKET_FORMAT
#ifdef      PACKET_FORWARDING_SUPPORTED
#error         "Packet forwarding requires the extended packet configuration!"
#endif      // PACKET_FORWARDING_SUPPORTED
#endif   // EXTENDED_PACKET_FORMAT

#ifndef FOUR_CHANNEL_IS_USED
#ifndef MORE_CHANNEL_IS_USED
#error "Either FOUR_CHANNEL_IS_USED or MORE_CHANNEL_IS_USED has to be defined!"
#endif //MORE_CHANNEL_IS_USED
#endif //FOUR_CHANNEL_IS_USED

#ifdef   FOUR_CHANNEL_IS_USED
#ifdef      MORE_CHANNEL_IS_USED
#error         "Only one channel configuration can be selected!"
#endif      // MORE_CHANNEL_IS_USED
#endif   // FOUR_CHANNEL_IS_USED

#ifdef   MORE_CHANNEL_IS_USED
#ifdef      ANTENNA_DIVERSITY_ENABLED
#error         "Antenna diversity is allowed only four channel!"
#endif      // ANTENNA_DIVERSITY_ENABLED
#endif   // MORE_CHANNEL_IS_USED

#ifdef MORE_CHANNEL_IS_USED
#ifdef 	FREQUENCY_BAND_434
#error		"EZHOP is supported only the 915 MHz band!"
#endif	//FREQUENCY_BAND_434
#endif	//MORE_CHANNEL_IS_USED

#ifdef MORE_CHANNEL_IS_USED
#ifdef 	FREQUENCY_BAND_868
#error		"EZHOP is supported only the 915 MHz band!"
#endif	//FREQUENCY_BAND_868
#endif	//MORE_CHANNEL_IS_USED


#ifdef   TRANSMITTER_ONLY_OPERATION
#ifdef      PACKET_FORWARDING_SUPPORTED
#error         "Packet Forwarding is not supported by Transmitter Only configuration!"
#endif      // PACKET_FORWARDING_SUPPORTED
#endif   // TRANSMITTER_ONLY_OPERATION

#ifdef   RECEIVER_ONLY_OPERATION
#ifdef      PACKET_FORWARDING_SUPPORTED
#error         "Packet Forwarding is not supported by Receiver Only configuration!"
#endif      // PACKET_FORWARDING_SUPPORTED
#endif   // RECEIVER_ONLY_OPERATION

#ifndef   TRANSCEIVER_OPERATION
#ifdef      EXTENDED_PACKET_FORMAT
#error         "Extended packet format is supported only by Transceiver configuration!"
#endif      // EXTENDED_PACKET_FORMAT
#endif   // TRANSCEIVER_OPERATION

#ifndef TRANSCEIVER_OPERATION
#ifndef TRANSMITTER_ONLY_OPERATION
#ifndef RECEIVER_ONLY_OPERATION
#error "Either TRANSCEIVER_OPERATION, TRANSMITTER_ONLY_OPERATION or RECEIVER_ONLY_OPERATION has to be defined!"
#endif //RECEIVER_ONLY_OPERATION
#endif //TRANSMITTER_ONLY_OPERATION
#endif //TRANSCEIVER_OPERATION

#ifdef   TRANSMITTER_ONLY_OPERATION
#ifdef      RECEIVER_ONLY_OPERATION
#error         "Both Tranmitter Only and Receiver Only cannot be defined!"
#endif      // RECEIVER_ONLY_OPERATION
#endif   // TRANSMITTER_ONLY_OPERATION

#ifdef   TRANSCEIVER_OPERATION
#ifdef      TRANSMITTER_ONLY_OPERATION
#error         "Both Tranceiver and Transmitter Only cannot be defined!"
#endif      // TRANSMITTER_ONLY_OPERATION
#ifdef      RECEIVER_ONLY_OPERATION
#error         "Both Transceiver and Receiver Only cannot be defined!"
#endif      // RECEIVER_ONLY_OPERATION
#endif   // TRANSCEIVER_OPERATION

#ifndef FREQUENCY_BAND_434
#ifndef FREQUENCY_BAND_868
#ifndef FREQUENCY_BAND_915
#error "Either FREQUENCY_BAND_434, FREQUENCY_BAND_868 or FREQUENCY_BAND_915 has to be defined!"
#endif //FREQUENCY_BAND_915
#endif //FREQUENCY_BAND_868
#endif //FREQUENCY_BAND_434

#ifdef 	FREQUENCY_BAND_434
#ifdef		FREQUENCY_BAND_868
#error			"Only one frequency band can be defined!"
#endif		//FREQUENCY_BAND_868
#ifdef		FREQUENCY_BAND_915
#error			"Only one frequency band can be defined!"
#endif		//FREQUENCY_BAND_915
#endif	//FREQUENCY_BAND_434

#ifdef 	FREQUENCY_BAND_868
#ifdef		FREQUENCY_BAND_434
#error			"Only one frequency band can be defined!"
#endif		//FREQUENCY_BAND_434
#ifdef		FREQUENCY_BAND_915
#error			"Only one frequency band can be defined!"
#endif		//FREQUENCY_BAND_915
#endif	//FREQUENCY_BAND_868

#ifdef 	FREQUENCY_BAND_915
#ifdef		FREQUENCY_BAND_434
#error			"Only one frequency band can be defined!"
#endif		//FREQUENCY_BAND_434
#ifdef		FREQUENCY_BAND_868
#error			"Only one frequency band can be defined!"
#endif		//FREQUENCY_BAND_868
#endif	//FREQUENCY_BAND_915

#if (FORWARDED_PACKET_TABLE_SIZE > 15)
#error "maximum FORWARDED_PACKET_TABLE_SIZE is 15 !"
#endif

#if (RECEIVED_BUFFER_SIZE > 64)
#error "The maximum size of the Received Data Buffer is 64!"
#endif

#if ((ACK_BUFFER_SIZE > 16) || (ACK_PAYLOAD_DEFAULT_SIZE > 16))
#error "The maximum size of ACK_BUFFER_SIZE and ACK_PAYLOAD_DEFAULT_SIZE is 16!"
#endif

#if (ACK_PAYLOAD_DEFAULT_SIZE > ACK_BUFFER_SIZE)
#error "ACK_PAYLOAD_DEFAULT_SIZE cannot be greater than ACK_BUFFER_SIZE!"
#endif



#endif //_EZMACPRO_DEFS_H_
