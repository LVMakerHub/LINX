/*********************************************************************************
 **
 **  chipKIT_uC32 - LINX Firmware for chipKIT uC32
 **
 **  Written By:    Sam K
 **  Written On:    August 2013
 **  Last Updated:  August 2013 - Sam K
 **
 **  This File May Be Modified And Re-Distributed Freely. Original File Content
 **  Written By Sam K And Available At www.github.com/labviewhacker/LVH-LINX
 **
 *********************************************************************************/

/************************************************************************************
*  Configuration
************************************************************************************/
//Uncomment To Enable Debugging On The Secondary Serial Port.
#define DEBUG_ENABLED 1

//Uncomment A Line Below To Enable LINX Packets On The Specified Interface.
#define SERIAL_ENABLED 1
//#define ETHERNET_ENABLED 1
//#define WIFI_ENABLED 1

//Comment Lines Below To Disable Various Peripherals
#define LINX_DIGITAL 1


/************************************************************************************
*  Includes
************************************************************************************/
#include "LINX_chipKIT_Common.h"

#ifdef SERIAL_ENABLED
  #include "LINX_chipKIT_Serial.h"
#endif
#ifdef LINX_DIGITAL
  #include "LINX_chipKIT_Digital.h"
#endif

/************************************************************************************
*  Setup
************************************************************************************/
void setup()
{
  //Setup LINX Device
  setupLINX();
}

/************************************************************************************
*  Loop
************************************************************************************/
void loop()
{
  //Check For A LINX Packet
  #ifdef SERIAL_ENABLED
    checkForSerialPacket();
  #endif
}







