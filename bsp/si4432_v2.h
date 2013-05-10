
#ifndef SI4432_H
#define SI4432_H


// Register Defines
//
//    The register names are defined exactly as listed in the Function/Description field of the
// Si4432 data sheet. Upper Case is used for constants. Spaces and miscelaneos characters are
// replaces with underscores.
//
//================================================================================================
#define  SI4432_DEVICE_TYPE                                 0x00
#define  SI4432_DEVICE_VERSION                              0x01
#define  SI4432_DEVICE_STATUS                               0x02
#define  SI4432_INTERRUPT_STATUS_1                          0x03
#define  SI4432_INTERRUPT_STATUS_2                          0x04
#define  SI4432_INTERRUPT_ENABLE_1                          0x05
#define  SI4432_INTERRUPT_ENABLE_2                          0x06
#define  SI4432_OPERATING_AND_FUNCTION_CONTROL_1            0x07
#define  SI4432_OPERATING_AND_FUNCTION_CONTROL_2            0x08
#define  SI4432_CRYSTAL_OSCILLATOR_LOAD_CAPACITANCE         0x09
#define  SI4432_MICROCONTROLLER_OUTPUT_CLOCK                0x0A
#define  SI4432_GPIO0_CONFIGURATION                         0x0B
#define  SI4432_GPIO1_CONFIGURATION                         0x0C
#define  SI4432_GPIO2_CONFIGURATION                         0x0D
#define  SI4432_IO_PORT_CONFIGURATION                       0x0E
#define  SI4432_ADC_CONFIGURATION                           0x0F
#define  SI4432_ADC_SENSOR_AMPLIFIER_OFFSET                 0x10
#define  SI4432_ADC_VALUE                                   0x11
#define  SI4432_TEMPERATURE_SENSOR_CONTROL                  0x12
#define  SI4432_TEMPERATURE_VALUE_OFFSET                    0x13
#define  SI4432_WAKE_UP_TIMER_PERIOD_1                      0x14
#define  SI4432_WAKE_UP_TIMER_PERIOD_2                      0x15
#define  SI4432_WAKE_UP_TIMER_PERIOD_3                      0x16
#define  SI4432_WAKE_UP_TIMER_VALUE_1                       0x17
#define  SI4432_WAKE_UP_TIMER_VALUE_2                       0x18
#define  SI4432_LOW_DUTY_CYCLE_MODE_DURATION                0x19
#define  SI4432_LOW_BATTERY_DETECTOR_THRESHOLD              0x1A
#define  SI4432_BATTERY_VOLTAGE_LEVEL                       0x1B
#define  SI4432_IF_FILTER_BANDWIDTH                         0x1C
#define  SI4432_AFC_LOOP_GEARSHIFT_OVERRIDE                 0x1D
#define  SI4432_AFC_TIMING_CONTROL                          0x1E
#define  SI4432_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE           0x1F
#define  SI4432_CLOCK_RECOVERY_OVERSAMPLING_RATIO           0x20
#define  SI4432_CLOCK_RECOVERY_OFFSET_2                     0x21
#define  SI4432_CLOCK_RECOVERY_OFFSET_1                     0x22
#define  SI4432_CLOCK_RECOVERY_OFFSET_0                     0x23
#define  SI4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN_1           0x24
#define  SI4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN_0           0x25
#define  SI4432_RECEIVED_SIGNAL_STRENGTH_INDICATOR          0x26
#define  SI4432_RSSI_THRESHOLD                              0x27
#define  SI4432_ANTENNA_DIVERSITY_REGISTER_1                0x28
#define  SI4432_ANTENNA_DIVERSITY_REGISTER_2                0x29
#define  SI4431_AFC_LIMIT									0x2A	/* only Si4431 A0 and Si443x B1 */
#define  SI4432_DATA_ACCESS_CONTROL                         0x30
#define  SI4432_EZMAC_STATUS                                0x31
#define  SI4432_HEADER_CONTROL_1                            0x32
#define  SI4432_HEADER_CONTROL_2                            0x33
#define  SI4432_PREAMBLE_LENGTH                             0x34
#define  SI4432_PREAMBLE_DETECTION_CONTROL                  0x35
#define  SI4432_SYNC_WORD_3                                 0x36
#define  SI4432_SYNC_WORD_2                                 0x37
#define  SI4432_SYNC_WORD_1                                 0x38
#define  SI4432_SYNC_WORD_0                                 0x39
#define  SI4432_TRANSMIT_HEADER_3                           0x3A
#define  SI4432_TRANSMIT_HEADER_2                           0x3B
#define  SI4432_TRANSMIT_HEADER_1                           0x3C
#define  SI4432_TRANSMIT_HEADER_0                           0x3D
#define  SI4432_TRANSMIT_PACKET_LENGTH                      0x3E
#define  SI4432_CHECK_HEADER_3                              0x3F
#define  SI4432_CHECK_HEADER_2                              0x40
#define  SI4432_CHECK_HEADER_1                              0x41
#define  SI4432_CHECK_HEADER_0                              0x42
#define  SI4432_HEADER_ENABLE_3                             0x43
#define  SI4432_HEADER_ENABLE_2                             0x44
#define  SI4432_HEADER_ENABLE_1                             0x45
#define  SI4432_HEADER_ENABLE_0                             0x46
#define  SI4432_RECEIVED_HEADER_3                           0x47
#define  SI4432_RECEIVED_HEADER_2                           0x48
#define  SI4432_RECEIVED_HEADER_1                           0x49
#define  SI4432_RECEIVED_HEADER_0                           0x4A
#define  SI4432_RECEIVED_PACKET_LENGTH                      0x4B
#define  SI4432_ANALOG_TEST_BUS                             0x50
#define  SI4432_DIGITAL_TEST_BUS                            0x51
#define  SI4432_TX_RAMP_CONTROL                             0x52
#define  SI4432_PLL_TUNE_TIME                               0x53
#define  SI4432_CALIBRATION_CONTROL                         0x55
#define  SI4432_MODEM_TEST                                  0x56
#define  SI4432_CHARGEPUMP_TEST                             0x57
#define  SI4432_CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE        0x58
#define  SI4432_DIVIDER_CURRENT_TRIMMING                    0x59
#define  SI4432_VCO_CURRENT_TRIMMING                        0x5A
#define  SI4432_VCO_CALIBRATION_OVERRIDE                    0x5B
#define  SI4432_SYNTHESIZER_TEST                            0x5C
#define  SI4432_BLOCK_ENABLE_OVERRIDE_1                     0x5D
#define  SI4432_BLOCK_ENABLE_OVERRIDE_2                     0x5E
#define  SI4432_BLOCK_ENABLE_OVERRIDE_3                     0x5F
#define  SI4432_CHANNEL_FILTER_COEFFICIENT_ADDRESS          0x60
#define  SI4432_CHANNEL_FILTER_COEFFICIENT_VALUE            0x61
#define  SI4432_CRYSTAL_OSCILLATOR_CONTROL_TEST             0x62
#define  SI4432_RC_OSCILLATOR_COARSE_CALIBRATION_OVERRIDE   0x63
#define  SI4432_RC_OSCILLATOR_FINE_CALIBRATION_OVERRIDE     0x64
#define  SI4432_LDO_CONTROL_OVERRIDE                        0x65
#define  SI4432_LDO_LEVEL_SETTING                           0x66
#define  SI4432_DELTASIGMA_ADC_TUNING_1                     0x67
#define  SI4432_DELTASIGMA_ADC_TUNING_2                     0x68
#define  SI4432_AGC_OVERRIDE_1                              0x69
#define  SI4432_AGC_OVERRIDE_2                              0x6A
#define  SI4432_GFSK_FIR_FILTER_COEFFICIENT_ADDRESS         0x6B
#define  SI4432_GFSK_FIR_FILTER_COEFFICIENT_VALUE           0x6C
#define  SI4432_TX_POWER                                    0x6D
#define  SI4432_TX_DATA_RATE_1                              0x6E
#define  SI4432_TX_DATA_RATE_0                              0x6F
#define  SI4432_MODULATION_MODE_CONTROL_1                   0x70
#define  SI4432_MODULATION_MODE_CONTROL_2                   0x71
#define  SI4432_FREQUENCY_DEVIATION                         0x72
#define  SI4432_FREQUENCY_OFFSET_1                          0x73
#define  SI4432_FREQUENCY_OFFSET_2                          0x74
#define  SI4432_FREQUENCY_BAND_SELECT                       0x75
#define  SI4432_NOMINAL_CARRIER_FREQUENCY_1                 0x76
#define  SI4432_NOMINAL_CARRIER_FREQUENCY_0                 0x77
#define  SI4432_FREQUENCY_HOPPING_CHANNEL_SELECT            0x79
#define  SI4432_FREQUENCY_HOPPING_STEP_SIZE                 0x7A
#define  SI4432_TX_FIFO_CONTROL_1                           0x7C
#define  SI4432_TX_FIFO_CONTROL_2                           0x7D
#define  SI4432_RX_FIFO_CONTROL                             0x7E
#define  SI4432_FIFO_ACCESS                                 0x7F

//================================================================================================
//
// Register Bit Masks
//
//================================================================================================

// SI4432_DEVICE_TYPE                              0x00
#define  SI4432_DT_MASK          0x1F

// SI4432_DEVICE_VERSION                           0x01
#define  SI4432_VC_MASK          0x1F

// SI4432_DEVICE_STATUS                            0x02
#define  SI4432_CPS_MASK         0x03
#define  SI4432_LOCKDET          0x04
#define  SI4432_FREQERR          0x08
#define  SI4432_HEADERR          0x10
#define  SI4432_RXFFEM           0x20
#define  SI4432_FFUNFL           0x40
#define  SI4432_FFOVFL           0x80

// SI4432_INTERRUPT_STATUS_1                       0x03
#define  SI4432_ICRCERROR        0x01
#define  SI4432_IPKVALID         0x02
#define  SI4432_IPKSENT          0x04
#define  SI4432_IEXT             0x08
#define  SI4432_IRXFFAFULL       0x10
#define  SI4432_ITXFFAEM         0x20
#define  SI4432_ITXFFAFULL       0x40
#define  SI4432_IFFERR           0x80

// SI4432_INTERRUPT_STATUS_2                       0x04
#define  SI4432_IPOR             0x01
#define  SI4432_ICHIPRDY         0x02
#define  SI4432_ILBD             0x04
#define  SI4432_IWUT             0x08
#define  SI4432_IRSSI            0x10
#define  SI4432_IPREAINVAL       0x20
#define  SI4432_IPREAVAL         0x40
#define  SI4432_ISWDET           0x80

// SI4432_INTERRUPT_ENABLE_1                       0x05
#define  SI4432_ENCRCERROR       0x01
#define  SI4432_ENPKVALID        0x02
#define  SI4432_ENPKSENT         0x04
#define  SI4432_ENEXT            0x08
#define  SI4432_ENRXFFAFULL      0x10
#define  SI4432_ENTXFFAEM        0x20
#define  SI4432_ENTXFFAFULL      0x40
#define  SI4432_ENFFERR          0x80

// SI4432_INTERRUPT_ENABLE_2                       0x06
#define  SI4432_ENPOR            0x01
#define  SI4432_ENCHIPRDY        0x02
#define  SI4432_ENLBDI           0x04 // added I to make unique
#define  SI4432_ENWUT            0x08
#define  SI4432_ENRSSI           0x10
#define  SI4432_ENPREAINVAL      0x20
#define  SI4432_ENPREAVAL        0x40
#define  SI4432_ENSWDET          0x80

// SI4432_OPERATING_FUNCTION_CONTROL_1             0x07
#define  SI4432_XTON             0x01
#define  SI4432_PLLON            0x02
#define  SI4432_RXON             0x04
#define  SI4432_TXON             0x08
#define  SI4432_X32KSEL          0x10
#define  SI4432_ENWT             0x20
#define  SI4432_ENLBD            0x40
#define  SI4432_SWRES            0x80

// SI4432_OPERATING_FUNCTION_CONTROL_2             0x08
#define  SI4432_FFCLRTX          0x01
#define  SI4432_FFCLRRX          0x02
#define  SI4432_ENLDM            0x04
#define  SI4432_AUTOTX           0x08
#define  SI4432_RXMPK            0x10
#define  SI4432_ANTDIV_MASK      0xE0

// SI4432_CRYSTAL_OSCILLATOR_LOAD_CAPACITANCE      0x09
#define  SI4432_XLC_MASK         0x7F
#define  SI4432_XTALSHFT         0x80

// SI4432_MICROCONTROLLER_OUTPUT_CLOCK             0x0A
#define  SI4432_MCLK_MASK        0x07
#define  SI4432_ENLFC            0x08
#define  SI4432_CLKT_MASK        0x30

// SI4432_GPIO0_CONFIGURATION                      0x0B
#define  SI4432_GPIO0_MASK       0x1F
#define  SI4432_PUP0             0x20
#define  SI4432_GPIO0DRV_MASK    0xC0

// SI4432_GPIO1_CONFIGURATION                      0x0C
#define  SI4432_GPIO1_MASK       0x1F
#define  SI4432_PUP1             0x20
#define  SI4432_GPIO1DRV_MASK    0xC0

// SI4432_GPIO2_CONFIGURATION                      0x0D
#define  SI4432_GPIO2_MASK       0x1F
#define  SI4432_PUP2             0x20
#define  SI4432_GPIO2DRV_MASK    0xC0

// SI4432_IO_PORT_CONFIGURATION                    0x0E
#define  SI4432_DIO_MASK         0x07
#define  SI4432_ITSDO            0x08
#define  SI4432_EXTITST_MASK     0x70

// SI4432_ADC_CONFIGURATION                        0x0F
#define  SI4432_ADCGAIN_MASK     0x03
#define  SI4432_ADCREF_MASK      0x0C
#define  SI4432_ADCSEL_MASK      0x70
#define  SI4432_ADCSTART         0x80  //W
#define  SI4432_ADCDONE          0x80  //R

// SI4432_ADC_SENSOR_AMPLIFIER_OFFSET              0x10
#define  SI4432_ADCOFFS_MASK     0x0F

// SI4432_ADC_VALUE                                0x11
// no bits or mask

// SI4432_TEMPERATURE_SENSOR_CONTROL               0x12
#define  SI4432_TSTRIM_MASK      0x0F
#define  SI4432_ENTSTRIM         0x10
#define  SI4432_ENTSOFFS         0x20
#define  SI4432_TSRANGE_MASK     0xC0

// SI4432_TEMPERATURE_VALUE_OFFSET                 0x13
// no bits or mask

// SI4432_WAKE_UP_TIMER_PERIOD_1                  0x14
#define  SI4432_WTD_MASK         0x03
#define  SI4432_WTR_MASK         0x3C

// SI4432_WAKE_UP_TIMER_PERIOD_2                  0x15
// no bits or mask

// SI4432_WAKE_UP_TIMER_PERIOD_3                  0x16
#define  SI4432_WTM_MASK         0x80

// SI4432_WAKE_UP_TIMER_VALUE_1                   0x17
// no bits or mask

// SI4432_WAKE_UP_TIMER_VALUE_2                   0x18
// no bits or mask

// SI4432_LOW_DUTY_CYCLE_MODE_DURATION            0x19
// no bits or mask

// SI4432_LOW_BATTERY_DETECTOR_THRESHOLD           0x1A
#define  SI4432_LBDT_MASK        0x1F

// SI4432_BATTERY_VOLTAGE_LEVEL                    0x1B
#define  SI4432_VBAT_MASK        0x1F

// SI4432_I_F_FILTER_BANDWIDTH                     0x1C
#define  SI4432_FILSET_MASK      0x0F
#define  SI4432_NDEC_MASK        0x70
#define  SI4432_DWN3_BYPASS      0x80

// SI4432_A_F_C_LOOP_GEARSHIFT_OVERRIDE            0x1D
#define  SI4432_AFCGEARH_MASK    0x3F
#define  SI4432_ENAFC            0x40
#define  SI4432_AFCBD            0x80

// SI4432_A_F_C_TIMING_CONTROL                     0x1E
#define  SI4432_LGWAIT_MASK      0x07
#define  SI4432_SHWAIT_MASK      0x38

// SI4432_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE        0x1F
#define  SI4432_CRSLOW_MASK      0x07
#define  SI4432_CRFAST_MASK      0x38
#define  SI4432_RXREADY          0x40

// SI4432_CLOCK_RECOVERY_OVERSAMPLING_RATIO        0x20
// no bits or mask

// SI4432_CLOCK_RECOVERY_OFFSET_2 0x21
#define  SI4432_NCOFF_MASK       0x0F
#define  SI4432_STALLCTRL        0x10
#define  SI4432_RXOSR_MASK       0xE0

// SI4432_CLOCK_RECOVERY_OFFSET_1                  0x22
// no bits or mask

// SI4432_CLOCK_RECOVERY_OFFSET_0                  0x23
// no bits or mask

// SI4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN_1        0x24
#define  SI4432_CRGAIN_MASK      0x07

// SI4432_CLOCK_RECOVERY_TIMING_LOOP_GAIN_0        0x25
// no bits or mask

// SI4432_RECEIVED_SIGNAL_STRENGTH_INDICATOR       0x26
// no bits or mask

// SI4432_RSSI_THRESHOLD                           0x27
// no bits or mask

// SI4432_ANTENNA_DIVERSITY_REGISTER_1             0x28
// no bits or mask

// SI4432_ANTENNA_DIVERSITY_REGISTER_2             0x29
// no bits or mask

// SI4432_DATA_ACCESS_CONTROL                      0x30
#define  SI4432_CRC_MASK         0x03
#define  SI4432_CRC_16			   0x01
#define  SI4432_ENCRC            0x04
#define  SI4432_ENPACTX          0x08
#define  SI4432_CRCDONLY         0x20
#define  SI4432_LSBFRST          0x40
#define  SI4432_ENPACRX          0x80

// SI4432_EZ_MAC_STATUS                            0x31
#define  SI4432_PKSENT           0x01
#define  SI4432_PKTX             0x02
#define  SI4432_CRCERROR         0x04
#define  SI4432_PKVALID          0x08
#define  SI4432_PKRX             0x10
#define  SI4432_PKSRCH           0x20
#define  SI4432_RXCRC1           0x40

// SI4432_HEADER_CONTROL_1                         0x32
#define  SI4432_HDCH_MASK        0x0F
#define  SI4432_BCEN_MASK        0xF0
#define  SI4432_BCEN             0xF0
#define  SI4432_DISABLE_HFILTERS 0x00

// SI4432_HEADER_CONTROL_2                         0x33
#define  SI4432_PREALEN_MASK     0x01
#define  SI4432_SYNCLEN_MASK     0x06
#define  SI4432_FIXPKLEN         0x08
#define  SI4432_HDLEN_MASK       0x70
#define  SI4432_SYNCLEN_2BYTE	   0x02
#define	SI4432_SYNCLEN_3BYTE		0x04
#define	SI4432_HDLEN_2BYTE		0x20
#define	SI4432_HDLEN_3BYTE		0x30
#define	SI4432_HDLEN_4BYTE		0x40

// SI4432_PREAMBLE_LENGTH                          0x34
// no bits or mask

// SI4432_SYNC_WORD_3                              0x36
// no bits or mask

// SI4432_SYNC_WORD_2                              0x37
// no bits or mask

// SI4432_SYNC_WORD_1                              0x38
// no bits or mask

// SI4432_SYNC_WORD_0                              0x39
// no bits or mask

// SI4432_TRANSMIT_HEADER_3                        0x3A
// no bits or mask

// SI4432_TRANSMIT_HEADER_2                        0x3B
// no bits or mask

// SI4432_TRANSMIT_HEADER_1                        0x3C
// no bits or mask

// SI4432_TRANSMIT_HEADER_0                        0x3D
// no bits or mask

// SI4432_TRANSMIT_PACKET_LENGTH                   0x3E
// no bits or mask

// SI4432_CHECK_HEADER_3                           0x3F
// no bits or mask

// SI4432_CHECK_HEADER_2                           0x40
// no bits or mask

// SI4432_CHECK_HEADER_1                           0x41
// no bits or mask

// SI4432_CHECK_HEADER_0                           0x42
// no bits or mask

// SI4432_HEADER_ENABLE_3                          0x43
// no bits or mask

// SI4432_HEADER_ENABLE_2                          0x44
// no bits or mask

// SI4432_HEADER_ENABLE_1                          0x45
// no bits or mask

// SI4432_HEADER_ENABLE_0                          0x46
// no bits or mask

// SI4432_RECEIVED_HEADER_3                        0x47
// no bits or mask

// SI4432_RECEIVED_HEADER_2                        0x48
// no bits or mask

// SI4432_RECEIVED_HEADER_1                        0x49
// no bits or mask

// SI4432_RECEIVED_HEADER_0                        0x4A
// no bits or mask

// SI4432_RECEIVED_PACKET_LENGTH                   0x4B
// no bits or mask

// SI4432_ANALOG_TEST_BUS                          0x50
#define  SI4432_ATB_MASK         0x1F

// SI4432_DIGITAL_TEST_BUS                         0x51
#define  SI4432_DTB_MASK         0x2F
#define  SI4432_ENSCTEST         0x40

// SI4432_TX_RAMP_CONTROL                          0x52
#define  SI4432_TXRAMP_MASK      0x03
#define  SI4432_LDORAMP_MASK     0x0C
#define  SI4432_TXMOD_MASK       0x70

// SI4432_PLL_TUNE_TIME                            0x53
#define  SI4432_PLLT0_MASK       0x07
#define  SI4432_PLLTS_MASK       0xF8

// SI4432_CALIBRATION_CONTROL                      0x55
#define  SI4432_SKIPVCO          0x01
#define  SI4432_VCOCAL           0x02
#define  SI4432_VCOCALDP         0x04
#define  SI4432_RCCAL            0x08
#define  SI4432_ENRCFCAL         0x10
#define  SI4432_ADCCALDONE       0x20
#define  SI4432_XTALSTARTHALF    0x40

// SI4432_MODEM_TEST                               0x56
#define  SI4432_IQSWITCH         0x01
#define  SI4432_REFCLKINV        0x02
#define  SI4432_REFCLKSEL        0x04
#define  SI4432_AFCPOL           0x10
#define  SI4432_DTTYPE           0x20
#define  SI4432_SLICFBYP         0x40
#define  SI4432_BCRFBYP          0x80

// SI4432_CHARGEPUMP_TEST                          0x57
#define  SI4432_CDCCUR_MASK      0x07
#define  SI4432_CDCONLY          0x08
#define  SI4432_CPFORCEDN        0x10
#define  SI4432_CPFORCEUP        0x20
#define  SI4432_FBDIV_RST        0x40
#define  SI4432_PFDRST           0x80

// SI4432_CHARGEPUMP_CURRENT_TRIMMING_OVERRIDE     0x58
#define  SI4432_CPCORR_MASK      0x1F
#define  SI4432_CPCORROV         0x20
#define  SI4432_CPCURR_MASK      0xC0

// SI4432_DIVIDER_CURRENT_TRIMMING                 0x59
#define  SI4432_D1P5TRIM_MASK    0x03
#define  SI4432_D2TRIM_MASK      0x0C
#define  SI4432_D3TRIM_MASK      0x30
#define  SI4432_FBDIVHC          0x40
#define  SI4432_TXCORBOOSTEN     0x80

// SI4432_VCO_CURRENT_TRIMMING                     0x5A
#define  SI4432_VCOCUR_MASK      0x03
#define  SI4432_VCOCORR_MASK     0x3C
#define  SI4432_VCOCORROV        0x40
#define  SI4432_TXCURBOOSTEN     0x80

// SI4432_VCO_CALIBRATION_OVERRIDE                 0x5B
#define  SI4432_VCOCAL_MASK      0x7F
#define  SI4432_VCOCALOV         0x80  //W
#define  SI4432_VCDONE           0x80  //R

// SI4432_SYNTHESIZER_TEST                         0x5C
#define  SI4432_DSRST            0x01
#define  SI4432_DSRSTMOD         0x02
#define  SI4432_DSORDER_MASK     0x0C
#define  SI4432_DSMOD            0x10
#define  SI4432_ENOLOOP          0x20
#define  SI4432_VCOTYPE          0x40
#define  SI4432_DSMDT            0x80

// SI4432_BLOCK_ENABLE_OVERRIDE_1                  0x5D
#define  SI4432_ENMX2            0x01
#define  SI4432_ENBF12           0x02
#define  SI4432_ENDV32           0x04
#define  SI4432_ENBF5            0x08
#define  SI4432_ENPA             0x10
#define  SI4432_ENPGA            0x20
#define  SI4432_ENLNA            0x40
#define  SI4432_ENMIX            0x80

// SI4432_BLOCK_ENABLE_OVERRIDE_2                  0x5E
#define  SI4432_PLLRESET         0x01
#define  SI4432_ENBF2            0x02
#define  SI4432_ENBF11           0x04
#define  SI4432_ENBF3            0x08
#define  SI4432_ENBF4            0x10
#define  SI4432_ENMX3            0x20
#define  SI4432_ENLDET           0x40
#define  SI4432_ENDS             0x80

// SI4432_BLOCK_ENABLE_OVERRIDE_3                  0x5F
#define  SI4432_ENBG             0x01
#define  SI4432_ENCP             0x02
#define  SI4432_ENVCO            0x04
#define  SI4432_DVBSHUNT         0x08
#define  SI4432_ENDV1P5          0x10
#define  SI4432_ENDV2            0x20
#define  SI4432_ENDV31           0x40
#define  SI4432_ENFRDV           0x80

// SI4432_CHANNEL_FILTER_COEFFICIENT_ADDRESS       0x60
#define  SI4432_CHFILADD_MASK    0x0F

// SI4432_CHANNEL_FILTER_COEFFICIENT_VALUE         0x61
#define  SI4432_CHFILVAL_MASK    0x3F

// SI4432_CRYSTAL_OSCILLATOR_CONTROL_TEST          0x62
#define  SI4432_ENBUF            0x01
#define  SI4432_BUFOVR           0x02
#define  SI4432_ENAMP2X          0x04
#define  SI4432_ENBIAS2X         0x08
#define  SI4432_CLKHYST          0x10
#define  SI4432_PWST_MASK        0xE0

// SI4432_RC_OSCILLATOR_COARSE_CALIBRATION_OVERRIDE   0x63
#define  SI4432_RCC_MASK         0x7F
#define  SI4432_RCCOV            0x80

// SI4432_RC_OSCILLATOR_FINE_CALIBRATION_OVERRIDE     0x64
#define  SI4432_RCF_MASK         0x7F
#define  SI4432_RCFOV            0x80

// SI4432_LDO_CONTROL_OVERRIDE   0x65
#define  SI4432_ENDIGPWDN        0x01
#define  SI4432_ENDIGLDO         0x02
#define  SI4432_ENPLLLDO         0x04
#define  SI4432_ENRFLDO          0x08
#define  SI4432_ENIFLDO          0x10
#define  SI4432_ENVCOLDO         0x20
#define  SI4432_ENBIAS           0x40
#define  SI4432_ENSPOR           0x80

// SI4432_LDO_LEVEL_SETTING   0x66
#define  SI4432_DIGLVL_MASK      0x07
#define  SI4432_ENRC32           0x10
#define  SI4432_ENTS             0x20
#define  SI4432_ENXTAL           0x40
#define  SI4432_ENOVR            0x80

// SI4432_DELTASIGMA_ADC_TUNING_1                  0x67
#define  SI4432_ADCTUNE_MASK     0x0F
#define  SI4432_ADCTUNEOVR       0x10
#define  SI4432_ENADC            0x20
#define  SI4432_ENREFDAC         0x40
#define  SI4432_ADCRST           0x80

// SI4432_DELTASIGMA_ADC_TUNING_2                  0x68
#define  SI4432_DSADCREF_MASK    0x07 // added DS to make unique
#define  SI4432_ADCOLOOP         0x08
#define  SI4432_ENVCM            0x10

// SI4432_AGC_OVERRIDE_1                           0x69
#define  SI4432_PGA_MASK         0x0F
#define  SI4432_LNAGAIN          0x10
#define  SI4432_AGCEN            0x20

// SI4432_AGC_OVERRIDE_2   0x6A
#define  SI4432_PGATH_MASK       0x03
#define  SI4432_LNACOMP_MASK     0x3F
#define  SI4432_AGCSLOW          0x40
#define  SI4432_AGCOVPM          0x80

// SI4432_GFSK_FIR_FILTER_COEFFICIENT_ADDRESS      0x6B
#define  SI4432_FIRADD_MASK      0x07

// SI4432_GFSK_FIR_FILTER_COEFFICIENT_VALUE        0x6C
#define  SI4432_FIRVAL_MASK      0x3F

// SI4432_TX_POWER   0x6D
#define  SI4432_TXPOW_MASK       0x07

// SI4432_TX_DATA_RATE_1                           0x6E
// no bits or mask

// SI4432_TX_DATA_RATE_0                           0x6F
// no bits or mask

// SI4432_MODULATION_MODE_CONTROL_1                0x70
#define  SI4432_ENWHITE          0x01
#define  SI4432_ENMANCH          0x02
#define  SI4432_ENMANINV         0x04
#define  SI4432_MANPPOL          0x08
#define  SI4432_ENPHPWDN         0x10
#define  SI4432_TXDTRTSCALE      0x20

// SI4432_MODULATION_MODE_CONTROL_2                0x71
#define  SI4432_MODTYP_MASK      0x03
#define  SI4432_FD_MASK          0x04
#define  SI4432_ENINV            0x08
#define  SI4432_DTMOD_MASK       0x30
#define  SI4432_TRCLK_MASK       0xC0
#define  SI4432_MODTYP_GFSK      0x03
#define  SI4432_FIFO_MODE        0x20
#define  SI4432_TX_DATA_CLK_GPIO 0x40


// SI4432_FREQUENCY_DEVIATION                      0x72
// no bits or mask

// SI4432_FREQUENCY_OFFSET_1                       0x73
// no bits or mask

// SI4432_FREQUENCY_OFFSET_2                       0x74
#define  SI4432_FO_MASK          0x03

// SI4432_FREQUENCY_BAND_SELECTRESERVED            0x75
#define  SI4432_FB_MASK          0x1F
#define  SI4432_HBSEL            0x20
#define  SI4432_SBSEL            0x40

// SI4432_NOMINAL_CARRIER_FREQUENCY_1              0x76
// no bits or mask

// SI4432_NOMINAL_CARRIER_FREQUENCY_0              0x77
// no bits or mask

// SI4432_FREQUENCY_HOPPING_CHANNEL_SELECT         0x79
// no bits or mask

// SI4432_FREQUENCY_HOPPING_STEP_SIZE              0x7A
// no bits or mask

// SI4432_TX_FIFO_CONTROL_1                        0x7C
#define  SI4432_TXAFTHR_MASK     0x3F

// SI4432_TX_FIFO_CONTROL_2                        0x7D
#define  SI4432_TXAETHR_MASK     0x3F

// SI4432_RX_FIFO_CONTROL                          0x7E
#define  SI4432_RXAFTHR_MASK     0x3F
#define  SI4432_RESERVED         0x40

// SI4432_FIFO_ACCESS                              0x7F
// no bits or mask

//================================================================================================
#endif  //SI4432_H
