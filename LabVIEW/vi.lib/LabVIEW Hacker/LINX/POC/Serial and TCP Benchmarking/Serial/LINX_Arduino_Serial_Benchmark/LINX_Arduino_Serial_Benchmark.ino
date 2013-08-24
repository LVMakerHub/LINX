void setup()
{
  //Initialize Serial Port
  Serial.begin(115200);
}

void loop()
{
   //Wait For Packet Size Byte
   while(Serial.available() < 1);
   
   //Store Remaining Packet Size
   unsigned char packetSize = (Serial.read());
   
   //Wait For Entire Packet
   while(Serial.available() < packetSize);
   
   //Echo Packet
   Serial.write(packetSize);
   for(int i=0; i< packetSize; i++)
   {
     Serial.write(Serial.read()); 
   }
}
