
unsigned char packetSize = 0;

void setup()
{
  Serial.begin(115200);
  
  //Wait For Packet Size From LV
  while(Serial.available() < 1){};
  packetSize = Serial.read();
}

void loop()
{
  //Wait for packet from LV
  while(Serial.available() < packetSize){};
  
  //Reply With Same Packet
  for(int i=0; i<packetSize; i++)
  {   
    Serial.write( Serial.read()); 
  }
  Serial.flush();
}
