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
**  processPacket - Process a LINX command packet and generate a response packet
*****************************************************************************************/
void processPacket(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Store Some Local Values For Convenience
  unsigned char commandLength = commandPacketBuffer[1];
  
  #ifdef debug
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
  switch(commandPacketBuffer[4])
  {
    //MSB CMD == 0
    case 0x00:
      switch(commandPacketBuffer[5])
      {
        
        case 0x00: // Sync Packet        
          responsePacketBuffer[0] = 0xFF;                                    //SoF
          responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
          responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
          responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
          responsePacketBuffer[4] = 0x00;                                    //STATUS
          responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM         
          break;     
          
        default:  //Default Case
          //TODO Throw Command Not Supported Error
           break;        
      }
      break;
    
    default:
      break;    
  }
  
  //Send Response Packet
  #ifdef debug 
    Serial1.print("Sending Response Packet ..:: "); 
    for(int i=0; i<responsePacketBuffer[1]; i++)
    {  
      Serial1.print("[");
      Serial1.print(responsePacketBuffer[i], HEX);
      Serial1.print("]");      
    }
    Serial1.println("::..");
  #endif
  
  Serial.write(responsePacketBuffer, responsePacketBuffer[1]);   
}

void setupLINX()
{
  //Serial Setup
  #ifdef SERIAL_ENABLED
    Serial.begin(9600);
  #endif
  
  //TCP Setup
  #ifdef tcpEnabled
    // TODO
  #endif

  //Debugging
  #ifdef debug
    Serial1.begin(115200);
    Serial1.println("Booting Up...");
  #endif  
}

/****************************************************************************************
**  checkForPacket - Check for a LINX command packet
*****************************************************************************************/
void checkForPacket()
{
#ifdef SERIAL_ENABLED  
  //Check for SoF and Packet Size
  if(Serial.available() >= 2)
  {
    #ifdef debug 
    Serial1.println("SoF and PacketSize Rec..."); 
    #endif
    //Check for valid SoF
    if(Serial.read() == 0xFF)
    {
       //SoF is valid.  Check Packet Size And Copy Packet Data Into Buffer
       //Copy SoF Into Packet Buffer
       #ifdef debug 
       Serial1.println("SoF Passed..."); 
       #endif
       serialCommandPacketBuffer[0] = 0xFF;
       
       serialCommandPacketBuffer[1] = (unsigned char) Serial.read();
       unsigned char packetBytesRemaining = serialCommandPacketBuffer[1] - 2;    
       
       #ifdef debug 
       Serial1.print("Waiting For "); 
       Serial1.print(packetBytesRemaining, DEC);
       Serial1.println(" more bytes..."); 
       #endif
       
       //Wait for the remaining bytes
       while(Serial.available() < packetBytesRemaining);
       
       #ifdef debug
       Serial1.println("Rest Of Packet Received...");
       #endif
       
       //Copy Data From Serial Buffer Into Packet Buffer.
       for(int i=0; i<packetBytesRemaining; i++)
       {
         serialCommandPacketBuffer[i+2] = Serial.read();
       }
       
       #ifdef debug
       Serial1.println("Computing Checksum...");
       #endif
       
       //Full Packet Received - Compute Checksum - Process Packet If Checksum Passes
       if(checksumPassed(serialCommandPacketBuffer))
       {
         #ifdef debug
         Serial1.println("Checksum Passed...");
         #endif
         //Process Packet
         processPacket(serialCommandPacketBuffer, serialResponsePacketBuffer);
       }
       else
       {         
         //Checksum Failed
         #ifdef debug
         Serial1.println("Checksum Failed...");
         #endif
         
         Serial.flush();
       }       
    }
    else
    {
      //SoF Incorrect - Corrupt Packet - Flush Buffer.
      #ifdef debug 
      Serial1.println("SoF Failed..."); 
      #endif
      Serial.flush();      
    }   
  }
#endif //SERIAL_ENABLED
}

