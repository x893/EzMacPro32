

#include "bsp.h"


//-----------------------------------------------------------------------------
// RTC_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
//
//-----------------------------------------------------------------------------
void RTC_Init (void)
{
   SEGMENT_VARIABLE(Is_RTC_Ready, static U8, SEG_XDATA) = FALSE;

   if (!Is_RTC_Ready) {
      RTC0KEY = 0xA5;                     // unlock the RTC interface
      RTC0KEY = 0xF1;

      RTC_Write (RTC0XCN, 0x60);          // Configure the smaRTClock
      // oscillator for crystal mode
      // Bias Doubling Enabled
      // AGC Disabled

      RTC_Write (RTC0XCF, CAP_AUTO_STEP|LOAD_CAP_VALUE);
      // load capacitance value from rtc.h

      RTC_Write (RTC0CN, 0x80);           // enable RTC

      rtcDelay(150);

      // wait for clock ready
      while ((RTC_Read (RTC0XCN) & 0x10)== 0x00);

      rtcDelay(150);                       // wait 1 ms

      // wait for cap ready
      while ((RTC_Read (RTC0XCF) & 0x40)== 0x00);

      RTC_Write (RTC0XCN, 0x40);          // disable bias doubling

      //RTC_Write (RTC0CN, 0xC0);          // Enable Missing clock detector

      RTC_ClearCapture ();                // clear CAPTUREn registers
      RTC_ClearAlarm ();                  // clear ALARMn registers

      RTC_Write (RTC0CN, 0xC2);           // transfer capture to clock
      RTC_Write (RTC0CN, 0xD0);           // enable RTC run
      Is_RTC_Ready = TRUE;
   }
}
//-----------------------------------------------------------------------------
// rtcDelay ()
//
// Return Value : None
// Parameters   : time (see note)
//
// This function will implement a delay using LP OSC.  A software delay is
// used to conserve hardware resources. The do loop will be at least
// 6.4 us * 3 clocks for a DJNZ instruction.
//-----------------------------------------------------------------------------
void rtcDelay (U8 time)
{
   U8 clock;
   U8 flash;

   clock = CLKSEL;
   flash = FLSCL;

   if(flash == 0x40)
   {
      CLKSEL   = 0x14;
      FLSCL    = 0x00;
      FLWR     = 0xFF;                 // errata fix
   }

   CLKSEL    =  0x74;                  // Switch to 156 kHz low power
   while(!(CLKSEL & 0x80));            // Poll CLKRDY

   do{}while (--time);                 // C code for DJNZ instruction

   // restore clock
   FLSCL     = flash;
   CLKSEL    = clock;
}

//=============================================================================
// RTC Primitive Fuctions - used for read, write
//-----------------------------------------------------------------------------
// RTC_Read ()
//-----------------------------------------------------------------------------
//
// Return Value : RTC0DAT
// Parameters   : reg
//
// This function will read one byte from the specified RTC register.
// Using a register number greater that 0x0F is not permited,
//
//-----------------------------------------------------------------------------
unsigned char RTC_Read (U8 reg)
{
   reg &= 0x0F;                        // mask low nibble
   RTC0ADR  = (reg |0x80);             // write address setting BUSY bit
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   return RTC0DAT;                     // return value
}
//-----------------------------------------------------------------------------
// RTC_Write ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : reg, value
//
// This function will Write one byte from the specified RTC register.
//
//-----------------------------------------------------------------------------
void RTC_Write (unsigned char reg, unsigned char value)
{
   reg &= 0x0F;                        // mask low nibble
   RTC0ADR  = reg;                     // pick register
   RTC0DAT = value;                    // write data
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
}

//=============================================================================
// RTC Clear Functions
//-----------------------------------------------------------------------------
// RTC_ClearCapture ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : reg, value
//
// This function will clear all CAPTURE registers of the RTC.
//
//-----------------------------------------------------------------------------
void RTC_ClearCapture (void)
{
   unsigned char n;

   for (n=0;n<4;n++)                   // n = 0-3 for CAPTURE0-3
   {
      while ((RTC0ADR & 0x80) == 0x80);// poll on the BUSY bit
      RTC0ADR = n;                     // select CAPTUREn register
      RTC0DAT = 0x00;                  // clear data
   }
   while ((RTC0ADR & 0x80) == 0x80);// poll on the BUSY bit
}
//-----------------------------------------------------------------------------
// RTC_ClearAlarm ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : reg, value
//
// This function will clear all ALARM registers of the RTC.
//
//-----------------------------------------------------------------------------
void RTC_ClearAlarm (void)
{
   unsigned char n;

   for (n=8;n<12;n++)                  // n = 8-11 for ALARM0-3
   {
      while ((RTC0ADR & 0x80) == 0x80);// poll on the BUSY bit
      RTC0ADR = n;                     // select ALARMn
      RTC0DAT = 0x00;                  // clear data
   }
   while ((RTC0ADR & 0x80) == 0x80);// poll on the BUSY bit

}
//=============================================================================
// RTC 32-bit Read and Write functions
//-----------------------------------------------------------------------------
// RTC_ReadCapture  ()
//-----------------------------------------------------------------------------
//
// Return Value : U32 ca
// Parameters   : none
//
//-----------------------------------------------------------------------------
U32 RTC_ReadCapture (void)
{
   UU32 capture;

   RTC0ADR = (0x80 | 0x00);            // read CAPTURE0
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   capture.U8[b0]= RTC0DAT;

   RTC0ADR = (0x80 | 0x01);            // read CAPTURE1
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   capture.U8[b1]= RTC0DAT;

   RTC0ADR = (0x80 | 0x02);            // read CAPTURE2
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   capture.U8[b2]= RTC0DAT;

   RTC0ADR = (0x80 | 0x03);            // read CAPTURE3
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   capture.U8[b3]= RTC0DAT;

   return capture.U32;
}
//-----------------------------------------------------------------------------
// RTC_ReadAlarm  ()
//-----------------------------------------------------------------------------
//
// Return Value : U32 Alarm time
// Parameters   : none
//
// This function is provided if you want to read the current alarm value and
// modify the results.
//
//-----------------------------------------------------------------------------
#if 0  // if you want to use, uncomment it
U32 RTC_ReadAlarm (void)
{
   UU32 alarm;

   RTC0ADR = (0x80 | 0x08);            // read ALARM0
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   alarm.U8[b0]= RTC0DAT;

   RTC0ADR = (0x80 | 0x09);            // read ALARM1
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   alarm.U8[b1]= RTC0DAT;

   RTC0ADR = (0x80 | 0x0A);            // read ALARM2
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   alarm.U8[b2]= RTC0DAT;

   RTC0ADR = (0x80 | 0x0B);            // read ALARM3
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   alarm.U8[b3]= RTC0DAT;

   return alarm.U32;
}
#endif
//-----------------------------------------------------------------------------
// RTC_WriteAlarm  ()
//-----------------------------------------------------------------------------
//
// Parameters   : U32 Alarm time
// Return Value : none
//
// This function will write to the alarm, but does not enable it.
//
//-----------------------------------------------------------------------------
void RTC_WriteAlarm(U32 time)
{
   UU32 alarm;
   alarm.U32 = time;

   RTC0ADR = 0x08;
   RTC0DAT = alarm.U8[b0];
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   RTC0DAT = alarm.U8[b1];
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   RTC0DAT = alarm.U8[b2];
   while ((RTC0ADR & 0x80) == 0x80);   // poll on the BUSY bit
   RTC0DAT = alarm.U8[b3];
}


/*!
 * Reset SmaRTClock counter.
 */
void RTC_Reset(void)
{
    RTC_Write(CAPTURE0, 0x00);
    RTC_Write(CAPTURE1, 0x00);
    RTC_Write(CAPTURE2, 0x00);
    RTC_Write(CAPTURE3, 0x00);
    RTC_Write(RTC0CN, 0x92);
}



//-----------------------------------------------------------------------------
//
// RTC Read Capture and GetCapture
//
// There is an important distiction between ReadCapture and GetCapture functions.
// The ReadCapture will read the current contents of the cpature regiuster.
// The GetCapture fucntion will initiate a capture sequence and return the
// value. The ReadCapture function is a low level function that does not wait.
// The GetCapture may take up tp 30 us to capture the current time.
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// RTC_GetCapture  ()
//-----------------------------------------------------------------------------
//
// Return Value : U32 Capture time
// Parameters   :
//
// An RTC capture is a three step process. First the Start capture function
// is called, then wait on capture pending, then read capture.
// A rtc cpature may take up to 30.5 usec or 610 instructions at 20 MHz.
//
// RTC_StartCapture ();
// while(RTC_CapturePending());
// capture = RTC_ReadCapture ()
//
// Note that if you want to perform tasks while the capture is takeing place,
// you must use the primitive functions.
//
//-----------------------------------------------------------------------------
U32 RTC_GetCapture(void)
{
   RTC_StartCapture ();
   while(RTC_CapturePending());
   return RTC_ReadCapture ();
}
//-----------------------------------------------------------------------------
// RTC_StartCapture  ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   :
//
//-----------------------------------------------------------------------------
void RTC_StartCapture (void)
{
   RTC_Write (RTC0CN, 0xD1);         // start capture,
}
//-----------------------------------------------------------------------------
// RTC_StartCapture  ()
//-----------------------------------------------------------------------------
//
// Return Value : 1 if capture pending 0 if capture is complete
// Parameters   : none
//
//-----------------------------------------------------------------------------
U8 RTC_CapturePending (void)
{
   return (RTC_Read(RTC0CN) & 0x01);
}
//-----------------------------------------------------------------------------
//
// RTC SetAlarm and WriteAlarm
//
// There is an important distiction between WriteAlarm and SetAlarm functions.
// The SetAlarm function writes the value and enables the alarm.
// The WriteAlarm will write a value to the Alarm registers, but does not
// enable the alarm. the SetAlarm is the high level function and uses the
// WriteAlarm function.
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// RTC_SetAlarm  ()
//-----------------------------------------------------------------------------
//
// Parameters   : U32 Alarm time
// Return Value : none
//
// This function will write to the alarm and enable it.
//
//-----------------------------------------------------------------------------
#if 0  // if you want to use, uncomment it
void RTC_SetAlarm(U32 time)
{
   RTC_WriteAlarm (time);
   RTC_Write (RTC0CN, 0xD8);         // enable RTC alarm
}
#endif
//-----------------------------------------------------------------------------
// RTC_SetTimeout  ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   :
//
//
//-----------------------------------------------------------------------------
#if 0  // if you want to use, uncomment it
void RTC_SetTimeout (U32 timeout)
{
   RTC_WriteAlarm(RTC_ReadCapture() + timeout);
   RTC_Write (RTC0CN, 0xD8);         // enable TRTC alarm
}
#endif
//-----------------------------------------------------------------------------
// RTC_SetTimeout  ()
//-----------------------------------------------------------------------------
//
// Return Value : none
// Parameters   : U32 Timeout
//
// This function will set an incremental alarm based on the current time.
//
//-----------------------------------------------------------------------------
void RTC_Timeout (U32 timeout)
{
   U32 capture;
//   RTC_Write (RTC0CN, 0xD0);           // clear alarm
   RTC_StartCapture ();
   while(RTC_CapturePending());
   capture = RTC_ReadCapture ();
   RTC_WriteAlarm(capture + timeout);
   RTC_Write (RTC0CN, 0xD8);           // enable RTC alarm
}

//=============================================================================
