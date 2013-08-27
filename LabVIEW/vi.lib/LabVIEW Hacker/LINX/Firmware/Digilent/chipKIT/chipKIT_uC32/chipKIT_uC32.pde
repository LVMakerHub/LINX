#define debug 1

unsigned char packetBuffer[64];

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
  checkForPacket(packetBuffer);
}


void checkForPacket(unsigned char* packetBuffer)
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
       packetBuffer[0] = 0xFF;
       
       packetBuffer[1] = (unsigned char) Serial.read();
       unsigned char packetBytesRemaining = packetBuffer[1] - 2;    
       
       #ifdef debug 
       Serial1.print("Waiting For "); 
       Serial1.print(packetBytesRemaining, DEC);
       Serial1.println(" more bytes..."); 
       #endif
       
       //Wait for the remaining bytes
       while(Serial.available() <= packetBytesRemaining);
       
       #ifdef debug
       Serial1.println("Rest Of Packet Received...");
       #endif
       
       //Copy Data From Serial Buffer Into Packet Buffer.
       for(int i=0; i<packetBytesRemaining; i++)
       {
         packetBuffer[i+2] = Serial.read();
       }
       
       #ifdef debug
       Serial1.println("Computing Checksum");
       #endif
       
       //Full Packet Received - Compute Checksum - Process Packet If Checksum Passes
       if(computeChecksum(packetBuffer))
       {
         #ifdef debug
         Serial1.println("Checksum Passed");
         #endif
         processPacket(packetBuffer);
       }
       else
       {
         
         //Checksum Failed
         #ifdef debug
         Serial1.println("Checksum Failed");
         #endif
         
         Serial.flush();
       }       
    }
    //Corrupt Packet.  Flush Buffer.
    #ifdef debug 
    Serial1.println("SoF Failed..."); 
    #endif
    Serial.flush();
  }
}

void processPacket(unsigned char* packetBuffer)
{
  //Store Some Local Values For Convenience
  unsigned char length = packetBuffer[1];
  
  //Return Entire Packet
  for(int i=0; i<packetBuffer[1]; i++)
  {
    Serial.print(packetBuffer[i]);    
  }
  
}

int computeChecksum(unsigned char* packetBuffer)
{
  //Store Some Local Values For Convenience
  unsigned char checksum = 0;
  unsigned char length = packetBuffer[1];
  
  //Sum All Bytes In The Packet Except The Last (Checksum Byte)
  for(int i=0; i < (length-1); i++)
  {
    checksum += packetBuffer[i];
  }
  
  //Compare Computed Checksum With Packet Checksum And Return Result
  if(checksum == packetBuffer[length-1])
  {
    return 1;    
  }
  else
  {
    return 0; 
  }
}
