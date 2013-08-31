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

// Write Digital Values Based On Digital Write Command Packet
void linxDigitalWrite(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{  
  int dirBit = 0;
  int dirByte = 0;
  int pinOffset = 0;
  
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
    
    pinMode(digitalPin, OUTPUT);
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
}

// Read Digital Values Based On Digital Read Command Packet
void linxDigitalRead(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  
  unsigned char bitOffset = 8;
  unsigned char byteOffset = 5;
  unsigned char retVal = 0;
  
  //Loop Over Pins To Read
  for(int i=0; i<commandPacketBuffer[1]-7; i++)
  {
    //Increment Offsets
    if(bitOffset == 0)
    {
      //Insert retVal Into Response Buffer
      responsePacketBuffer[byteOffset] = retVal;
      retVal = 0x00;
      byteOffset++;
      bitOffset = 7;      
    }
    else
    {
      bitOffset--;      
    } 
    
    unsigned char pinNumber = commandPacketBuffer[6+i];
    
    //Set Pin As Input (Might Make This Configurable)    
    pinMode(pinNumber, INPUT);
    
    //Read Pin And Insert Value Into retVal
    retVal = retVal | (digitalRead(pinNumber) << bitOffset);
  }
  responsePacketBuffer[byteOffset] = retVal;
  
  //Fill In The Rest Of The Response Packet
  responsePacketBuffer[0] = 0xFF;                                                //SoF
  responsePacketBuffer[1] = byteOffset + 2;                                      //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                              //PACKET NUM 
  responsePacketBuffer[3] = commandPacketBuffer[3];                              //PACKET NUM 
  responsePacketBuffer[4] = 0x00;                                                //STATUS
  responsePacketBuffer[byteOffset + 1] = computeChecksum(responsePacketBuffer);  //CHECKSUM   
}

// Set The Digital Pin Mode Based On The Set Digital Pin Mode Packet
void linxSetDigtalPinMode(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  int dirBit = 0;
  int dirByte = 0;
  int pinOffset = 0;
  
  while(pinOffset < commandPacketBuffer[6])
  {
    
    unsigned char digitalPin = (commandPacketBuffer[7 + pinOffset]);
    unsigned char digitalVal = ( ( commandPacketBuffer[( (commandPacketBuffer[6] + 7) + dirByte)] >> dirBit) & 0x01);
    
    #ifdef DEBUG_ENABLED          
      Serial1.print("pinMode(");
      Serial1.print(digitalPin, DEC);
      Serial1.print(", ");
      Serial1.print(digitalVal, DEC);
      Serial1.println(")");
    #endif       
    
    pinMode(digitalPin, digitalVal);
    
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
}

