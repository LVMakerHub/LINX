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
#define debug 1

//Uncomment To Enable LINX Packets On The Primary Serial Port.
#define SERIAL_ENABLED 1

//Uncomment To Enable TCP LINX Packets
//#define tcpEnabled 1

/************************************************************************************
*  Includes
************************************************************************************/
#include "LINX_chipKIT_common.h"

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
  checkForPacket();
}







