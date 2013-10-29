/****************************************************************************************
** 
**  chipKIT_uC32 - LINX Firmware for chipKIT uC32
**
**  Written By:    Sam K
**  Written On:    August 2013
**  Last Updated:  September 2013 - Sam K
**
**  This File May Be Modified And Re-Distributed Freely. Original File Content
**  Written By Sam K And Available At www.github.com/labviewhacker/LVH-LINX
**
****************************************************************************************/

/***************************************************************************************
** 
** Edit #defines in LINX_Config.h to configure the firmware for a specific chipKIT board
**
****************************************************************************************/


/****************************************************************************************
**  Includes
****************************************************************************************/
#include "LINX_Config.h"
#include "LINX.h"

/****************************************************************************************
**  Setup
****************************************************************************************/
void setup()
{
  //Setup LINX Device
  setupLINX();
}

/****************************************************************************************
*  Loop
****************************************************************************************/
void loop()
{
  //Check For A LINX Packet
  #ifdef LINX_SERIAL_INTERFACE_ENABLED
    checkForLINXSerialPacket();
  #endif
  
  #ifdef LINX_ETHERNET_INTERFACE_ENABLED
    checkForLINXEthernetPacket();
  #endif
}











