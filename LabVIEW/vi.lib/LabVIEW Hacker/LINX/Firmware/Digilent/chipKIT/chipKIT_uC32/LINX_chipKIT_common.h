/*********************************************************************************
 **
 **  LINX_chipKIT_Common - chipKIT LINX fimrware common functions
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
#ifndef LINX_CHIPKIT_COMMON_H
#define LINX_CHIPKIT_COMMON_H


// Buffers For LINX Packets
unsigned char ethernetCommandPacketBuffer[64];
unsigned char ethernetResponsePacketBuffer[64];



/*********************************************************************************
**  checkForSerialPacket
**
**  Check for a LINX command packet on the serial port
**
**  Input:  none
**  Output: none
*********************************************************************************/
void checkForSerialPacket();

 
/*********************************************************************************
**  checksumPassed
**
**  Check the user specified packet buffer to see if the checksum passes
**
**  Input:  packetBuffer - pointer to a packet buffer
**  Output: True if the checksum passes, false otherwise
*********************************************************************************/
bool checksumPassed(unsigned char* packetBuffer); 

  
/*********************************************************************************
**  computeChecksum
**
**  Compute the checksum for the user specified packet buffer.
**
**  Input:  packetBuffer - Pointer to a packet buffer
**  Output: The checksum value
*********************************************************************************/
unsigned char computeChecksum(unsigned char* packetBuffer);


/*********************************************************************************
**  processCommand
**
**  Process a LINX command packet and generate a response packet
**
**  Input:  packetBuffer - pointer to a packet buffer
**  Output: True if the checksum passes, false otherwise 
*********************************************************************************/
void processPacket(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer);


/*********************************************************************************
**  setupLINX
**
**  Setup the LINX device
**
**  Input:  none
**  Output: non
*********************************************************************************/
void setupLINX();


#endif //LINX_CHIPKIT_COMMON_H


