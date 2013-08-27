/*********************************************************************************
 **
 **  LINX_chipKIT_common - chipKIT LINX fimrware common functions
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
#ifndef linx_chipkit_common_h
#define linx_chipkit_common_h

// Buffers For LINX Packets
unsigned char serialCommandPacketBuffer[64];
unsigned char serialResponsePacketBuffer[64];
unsigned char tcpCommandPacketBuffer[64];
unsigned char tcpResponsePacketBuffer[64];



/*********************************************************************************
**  checkForPacket
**
**  Check for a LINX command packet
**
**  Input:  none
**  Output: none
*********************************************************************************/
void checkForPacket();

 
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
**  processPacket
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








#endif //linx_chipkit_common_h


