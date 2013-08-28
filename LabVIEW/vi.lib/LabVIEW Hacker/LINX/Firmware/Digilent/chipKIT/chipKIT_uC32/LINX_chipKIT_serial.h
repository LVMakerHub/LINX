/*********************************************************************************
 **
 **  LINX_chipKIT_serial - chipKIT LINX fimrware serial functions
 **
 **  Written By:    Sam K
 **  Written On:    August 2013
 **  Last Updated:  August 2013 - Sam K
 **
 **  This File May Be Modified And Re-Distributed Freely. Original File Content
 **  Written By Sam K And Available At www.github.com/labviewhacker/LVH-LINX
 **
 *********************************************************************************/

//Prevent Multiple Includes
#ifndef LINX_CHIPKIT_SERIAL_H
#define LINX_CHIPKIT_SERIAL_H

// Buffers For LINX Packets
unsigned char serialCommandBuffer[64];
unsigned char serialResponseBuffer[64];

/****************************************************************************************
**  checkForPacket - Check for a LINX command packet
*****************************************************************************************/
//void checkForSerialPacket();

#endif //LINX_CHIPKIT_SERIAL_H


