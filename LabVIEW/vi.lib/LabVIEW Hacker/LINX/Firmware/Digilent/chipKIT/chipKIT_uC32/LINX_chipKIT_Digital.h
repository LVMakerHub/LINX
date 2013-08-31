/*********************************************************************************
 **
 **  chipKIT_uC32 - LINX Digital I/O Functions for chipKIT uC32
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
#ifndef LINX_CHIPKIT_DIGITAL_H
#define LINX_CHIPKIT_DIGITAL_H


/*********************************************************************************
**  linxDigitalWrite
**
**  Write Digital Values Based On Digital Write Command Packet
**
**  Input:  commandPacketBuffer - The buffer containing the command packet
**          responsePacketBuffer - The buffer used build the response packet.
**  Output: none
*********************************************************************************/
void linxDigitalWrite(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer);

/*********************************************************************************
**  linxDigitalRead
**
**  Read Digital Values Based On Digital Read Command Packet
**
**  Input:  commandPacketBuffer - The buffer containing the command packet
**          responsePacketBuffer - The buffer used build the response packet.
**  Output: none
*********************************************************************************/
void linxDigitalRead(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer);


/*********************************************************************************
**  linxSetDigtalPinMode
**
**  Set The Digital Pin Mode Based On The Set Digital Pin Mode Packet
**
**  Input:  commandPacketBuffer - The buffer containing the command packet
**          responsePacketBuffer - The buffer used build the response packet.
**  Output: none
*********************************************************************************/
void linxSetDigtalPinMode(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer);




#endif //LINX_CHIPKIT_DIGITAL_H


