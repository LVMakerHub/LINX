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
 
 
/*********************************************************************************
**  computeChecksum - Compute the checksum for the user specified packet buffer.
*********************************************************************************/
unsigned char computeChecksum(unsigned char* packetBuffer)
{  
  unsigned char checksum = 0;
  
  //Sum All Bytes In The Packet Except The Last (Checksum Byte)
  for(int i=0; i<(packetBuffer[1] - 1); i++)
  {
    checksum += packetBuffer[i];
  }  
  return checksum; 
}

/****************************************************************************************
**  checksumPassed - Check the user specified packet buffer to see if the checksum passes
*****************************************************************************************/
//  
bool checksumPassed(unsigned char* packetBuffer)
{
  return (computeChecksum(packetBuffer) == packetBuffer[packetBuffer[1]-1]);
}

/****************************************************************************************
**  processCommand - Process a LINX command packet and generate a response packet
*****************************************************************************************/
void processCommand(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  
  //TEMP !!!
  int dirBit = 0;
  int dirByte = 0;
  int pinOffset = 0;
  
  
  
  //Store Some Local Values For Convenience
  unsigned char commandLength = commandPacketBuffer[1];
  
  #ifdef DEBUG_ENABLED
    Serial1.print("Processing Packet ..::");
    for(int i=0; i<commandLength; i++)
    {
      Serial1.print("[");
      Serial1.print(commandPacketBuffer[i], HEX);
      Serial1.print("]");
    }
    Serial1.println("::..");
  #endif
  
  //Process Command
  unsigned int command = commandPacketBuffer[4] << 8 | commandPacketBuffer[5];
  switch(command)
  {
    /****************************************************************************************
    * SYSTEM COMMANDS
    ****************************************************************************************/
    case 0x0000: // Sync Packet        
      responsePacketBuffer[0] = 0xFF;                                    //SoF
      responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
      responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
      responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
      responsePacketBuffer[4] = 0x00;                                    //STATUS
      responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM         
      break;
      
    /****************************************************************************************
    * Digital I/O
    ****************************************************************************************/     
    case 0x0041: // Sync Packet
      
      dirBit = 0;
      dirByte = 0;
      pinOffset = 0;
      
      pinMode(13, OUTPUT);
      
      while(pinOffset < commandPacketBuffer[6])
      {
        
        unsigned char digitalPin = (commandPacketBuffer[7 + pinOffset]);
        unsigned char digitalVal = ( ( commandPacketBuffer[( (commandPacketBuffer[6] + 7) + dirByte)] >> dirBit) & 0x01);
        
        #ifdef DEBUG_ENABLED          
          Serial1.print("digitalWrite(");
          Serial1.print(digitalPin, DEC);
          Serial1.print(", ");
          Serial1.print(digitalVal, DEC);
          Serial1.println(")");
        #endif       
        
        digitalWrite(digitalPin, digitalVal);
        
        //Increment Counters
        if(dirBit < 7)
        {
          dirBit++; 
        }
        else
        {
          dirBit = 0; 
          dirByte++;
        }
        pinOffset++;
      }
      
      responsePacketBuffer[0] = 0xFF;                                    //SoF
      responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
      responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
      responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
      responsePacketBuffer[4] = 0x00;                                    //STATUS
      responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM         
      break;
      
    default:  //Default Case
       responsePacketBuffer[0] = 0xFF;                                    //SoF
       responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
       responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
       responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
       responsePacketBuffer[4] = 0x01;                                    //STATUS
       responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM     
       break; 
   }
  
  //Send Response Packet
  #ifdef DEBUG_ENABLED 
    Serial1.print("Sending Response Packet ..::"); 
    for(int i=0; i<responsePacketBuffer[1]; i++)
    {  
      Serial1.print("[");
      Serial1.print(responsePacketBuffer[i], HEX);
      Serial1.print("]");      
    }
    Serial1.println("::..");
  #endif  
}

/****************************************************************************************
**  setupLINX - Enable the user specified interface for LINX communication
*****************************************************************************************/
void setupLINX()
{
  //Serial Setup
  #ifdef SERIAL_ENABLED
    Serial.begin(9600);
  #endif
  
  //Ethernet Setup
  #ifdef ETHERNET_ENABLED
    // TODO
  #endif
  
  //WIFI Setup
  #ifdef WIFI_ENABLED
    // TODO
  #endif

  //Debugging
  #ifdef DEBUG_ENABLED
    Serial1.begin(115200);
    Serial1.println("Booting Up...");
  #endif  
}
