

/*********************************************************************************
 **
 **  LINX_chipKIT_I2C - LINX I2C Functions for chipKIT Devices
 **
 **  Written By:    Sam K
 **  Written On:    September 2013
 **  Last Updated:  September 2013 - Sam K
 **
 **  This File May Be Modified And Re-Distributed Freely. Original File Content
 **  Written By Sam K And Available At www.github.com/labviewhacker/LVH-LINX
 **
 *********************************************************************************/

//Prevent Multiple Includes
#ifndef LINX_CHIPKIT_I2C_H
#define LINX_CHIPKIT_I2C_H

/*********************************************************************************
**  linxI2COpenMaster
**
**  Open the specified I2C channel and join as master.
**
**  Input:  commandPacketBuffer - The buffer containing the command packet
**          responsePacketBuffer - The buffer used build the response packet.
**  Output: none
*********************************************************************************/
void linxI2COpenMaster(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer);

/*********************************************************************************
**  linxI2CWrite
**
**  Write bytes to the specified I2C channel
**
**  Input:  commandPacketBuffer - The buffer containing the command packet
**          responsePacketBuffer - The buffer used build the response packet.
**  Output: none
*********************************************************************************/
void linxI2CWrite(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer);

#endif //LINX_CHIPKIT_I2C_H

