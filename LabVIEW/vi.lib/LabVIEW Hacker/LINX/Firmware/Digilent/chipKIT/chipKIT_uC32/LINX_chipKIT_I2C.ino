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
 #include "LINX_chipKIT_I2C.h"
 #include <Wire.h>
 
 //Open the specified I2C channel and join as master.
 void linxI2COpenMaster(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
 {
   Wire.begin();
     
   responsePacketBuffer[0] = 0xFF;                                    //SoF
   responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
   responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
   responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
   responsePacketBuffer[4] = 0x00;                                    //STATUS
   responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM  
 }
 
 //Write bytes to the specified I2C channel
 void linxI2CWrite(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
 {
   Wire.beginTransmission(commandPacketBuffer[7]);
   for(int i=0; i<(commandPacketBuffer[1] - 8); i++)
   {
     Wire.send(commandPacketBuffer[i+7]);
   }
   Wire.endTransmission();
   
   responsePacketBuffer[0] = 0xFF;                                    //SoF
   responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
   responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
   responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
   responsePacketBuffer[4] = 0x00;                                    //STATUS
   responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM  
 }
