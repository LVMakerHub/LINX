/*********************************************************************************
 **
 **  LINX_chipKIT_serial - chipKIT LINX fimrware serial functions
 **
 **  Written By:    Sam K
 **  Written On:    August 2013
 **  Last Updated:  August 2013 - Sam K
 **
 **  This File May Be Modified And Re-Distributed Freely. Original File Content
 **  Written By Sam K And Available At www.github.com/labviewhacker/LVH-LINX
 **
 *********************************************************************************/

/****************************************************************************************
**  setupSerial - Initialize Serial Port For LINX 
*****************************************************************************************/
void setupSerial()
{
  //Serial Setup  
  Serial.begin(9600);
}

/****************************************************************************************
**  checkForPacket - Check for a LINX command packet
*****************************************************************************************/
void checkForSerialPacket()
{ 
  //Check for SoF and Packet Size
  if(Serial.available() >= 2)
  {
    #ifdef DEBUG_ENABLED 
    Serial1.println("SoF and PacketSize Rec..."); 
    #endif
    //Check for valid SoF
    if(Serial.read() == 0xFF)
    {
       //SoF is valid.  Check Packet Size And Copy Packet Data Into Buffer
       //Copy SoF Into Packet Buffer
       #ifdef DEBUG_ENABLED 
       Serial1.println("SoF Passed..."); 
       #endif
       serialCommandBuffer[0] = 0xFF;
       
       serialCommandBuffer[1] = (unsigned char) Serial.read();
       unsigned char packetBytesRemaining = serialCommandBuffer[1] - 2;    
       
       #ifdef DEBUG_ENABLED 
       Serial1.print("Waiting For "); 
       Serial1.print(packetBytesRemaining, DEC);
       Serial1.println(" more bytes..."); 
       #endif
       
       //Wait for the remaining bytes
       while(Serial.available() < packetBytesRemaining);
       
       #ifdef DEBUG_ENABLED
       Serial1.println("Rest Of Packet Received...");
       #endif
       
       //Copy Data From Serial Buffer Into Packet Buffer.
       for(int i=0; i<packetBytesRemaining; i++)
       {
         serialCommandBuffer[i+2] = Serial.read();
       }
       
       #ifdef DEBUG_ENABLED
       Serial1.println("Computing Checksum...");
       #endif
       
       //Full Packet Received - Compute Checksum - Process Packet If Checksum Passes
       if(checksumPassed(serialCommandBuffer))
       {
         #ifdef DEBUG_ENABLED
         Serial1.println("Checksum Passed...");
         #endif
         //Process Packet
         processCommand(serialCommandBuffer, serialResponseBuffer);
         //Send Response
         Serial.write(serialResponseBuffer, serialResponseBuffer[1]);   
       }
       else
       {         
         //Checksum Failed
         #ifdef DEBUG_ENABLED
         Serial1.println("Checksum Failed...");
         #endif         
         Serial.flush();
       }  
    }
    else
    {
      //SoF Incorrect - Corrupt Packet - Flush Buffer.
      #ifdef DEBUG_ENABLED 
      Serial1.println("SoF Failed..."); 
      #endif
      Serial.flush();      
    }   
  }
}

