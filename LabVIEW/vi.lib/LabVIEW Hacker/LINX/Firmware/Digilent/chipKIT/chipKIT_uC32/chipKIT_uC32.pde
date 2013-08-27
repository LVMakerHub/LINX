#define debug 1

unsigned char commandPacketBuffer[64];
unsigned char responsePacketBuffer[64];

void setup()
{
  Serial.begin(9600);
  
  #ifdef debug
    Serial1.begin(115200);
    Serial1.println("Booting Up...");
  #endif
}

void loop()
{
  checkForPacket(commandPacketBuffer);
}


void checkForPacket(unsigned char* commandPacketBuffer)
{  
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
       commandPacketBuffer[0] = 0xFF;
       
       commandPacketBuffer[1] = (unsigned char) Serial.read();
       unsigned char packetBytesRemaining = commandPacketBuffer[1] - 2;    
       
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
         commandPacketBuffer[i+2] = Serial.read();
       }
       
       #ifdef debug
       Serial1.println("Computing Checksum...");
       #endif
       
       //Full Packet Received - Compute Checksum - Process Packet If Checksum Passes
       if(checksumPassed(commandPacketBuffer))
       {
         #ifdef debug
         Serial1.println("Checksum Passed...");
         #endif
         //Process Packet
         processPacket(commandPacketBuffer, responsePacketBuffer);
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
}

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

int computeChecksum(unsigned char* packetBuffer)
{  
  unsigned char checksum = 0;
  
  //Sum All Bytes In The Packet Except The Last (Checksum Byte)
  for(int i=0; i<(packetBuffer[1] - 1); i++)
  {
    checksum += packetBuffer[i];
  }  
  return checksum; 
}

bool checksumPassed(unsigned char* packetBuffer)
{
  return (computeChecksum(packetBuffer) == packetBuffer[packetBuffer[1]-1]);
}


