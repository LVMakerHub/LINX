/****************************************************************************************
**  Includes
****************************************************************************************/
#include "LINX.h"

//Include Ethernet Headers If Necissary
#ifdef LINX_ETHERNET_INTERFACE_ENABLED 
  //#include <NetworkShield.h> 
  //#include <DNETcK.h>                              
#endif //LINX_ETHERNET_INTERFACE_ENABLED

//Include WIFI Headers If Necissary
#ifdef LINX_WIFI_INTERFACE_ENABLED 
  #include <WiFiShieldOrPmodWiFi_G.h> 
  #include <DNETcK.h> 
  #include <DWIFIcK.h>                               
#endif //LINX_WIFI_INTERFACE_ENABLED

#ifdef LINX_I2C_ENABLED
  #include <Wire.h>
#endif //LINX_I2C_ENABLED

#ifdef LINX_SPI_ENABLED
  #include <SPI.h>
#endif //LINX_SPI_ENABLED

#ifdef LINX_NVS_ENABLED
  #include <EEPROM.h>
#endif //LINX_NVS_ENABLED

/****************************************************************************************
**
**--------------------------- VARIABLES ------------------------------------------------ 
**
****************************************************************************************/
//Load User Configuration From Non-Volatile Storage
#ifdef LINX_NVS_ENABLED
  #define NVS_USERID 0x00
  #define NVS_ETHERNET_IP 0x02
  #define NVS_ETHERNET_PORT 0x06
  #define NVS_WIFI_IP 0x08
  #define NVS_WIFI_PORT 0x0C
  #define NVS_WIFI_SSID_SIZE 0x0E
  #define NVS_WIFI_SSID 0x0F
  #define NVS_WIFI_SECURITY_TYPE 0x30
  #define NVS_WIFI_PW_SIZE 0x31
  #define NVS_WIFI_PW 0x32
  #define NVS_SERIAL_INTERFACE_MAX_BAUD 0x72

  unsigned short userID;
  
  unsigned long ethernetIP;
  unsigned short ethernetPort;  
  
  unsigned long wifiIP;
  unsigned short wifiPort;
  unsigned char wifiSSIDSize;
  char wifiSSID[32];
  unsigned char wifiSecurity;
  unsigned char wifiPwSize;
  char wifiPw[64];
  
  unsigned long serialInterfaceMaxBaud = 115200;
  
#endif //LINX_NVS_ENABLED

//Serial Interface
#ifdef LINX_SERIAL_INTERFACE_ENABLED
  //LINX Serial Packet Send / Receive Buffers
  unsigned char serialCommandBuffer[64];
  unsigned char serialResponseBuffer[64];
#endif  //LINX_SERIAL_INTERFACE_ENABLED

//Ethernet Interface
#ifdef LINX_ETHERNET_INTERFACE_ENABLED

  STATE ethernetState = LISTEN;
  DNETcK::STATUS ethernetStatus;
  TcpServer ethernetTCPServer;
  TcpClient ethernetTCPClient;
  int ethernetClientCount = 0;
  unsigned int ethernetStartTime = 0;
  unsigned int ethernetTimeout = 5000;         //UPDATE THESE TO LOAD FROM NVS (LOW PRIORITY)
  
  unsigned char ethernetCommandBuffer[256];    //UPDATE THESE TO LOAD FROM NVS (LOW PRIORITY)
  unsigned char ethernetResponseBuffer[256];
  
#endif  //LINX_ETHERNET_INTERFACE_ENABLED
  

//WIFI Interface
#ifdef LINX_WIFI_INTERFACE_ENABLED
  STATE wifiState = INITIALIZE;
  DNETcK::STATUS wifiStatus;
  TcpServer wifiTCPServer;
  TcpClient wifiTCPClient;
  int wifiClientCount = 0;
  unsigned int wifiStartTime = 0;
  unsigned int wifiTimeout = 5000;         //UPDATE THESE TO LOAD FROM NVS (LOW PRIORITY)
  
  unsigned char wifiCommandBuffer[256];    //UPDATE THESE TO LOAD FROM NVS (LOW PRIORITY)
  unsigned char wifiResponseBuffer[256];
  
  unsigned char wifiServerFail = 0;
#endif //LINX_WIFI_INTERFACE_ENABLED
  



#ifdef LINX_I2C_ENABLED
  unsigned char I2C0Open = 0;
  unsigned char I2C1Open = 0;
  unsigned char I2C2Open = 0;
  unsigned char I2C3Open = 0;
#endif //LINX_I2C_ENABLED

unsigned long startTime;
unsigned long configTimeout = 10000; //10 Second Config Timeout (Including LINX Delay After Boot)



/****************************************************************************************
**  Functions
****************************************************************************************/

/****************************************************************************************
**
**--------------------------- COMMON ---------------------------------------------------- 
**
****************************************************************************************/

//--------------------------- setupLINX -----------------------------------------------//
void setupLINX()
{ 
  
  //Debugging
  #ifdef DEBUG_ENABLED
    Serial1.begin(115200);
    Serial1.println("\n\nBooting Up...");
  #endif  
  
  //Load Config Values
  loadNVSConfig();
  
  // Setup Serial Port For LINX
  #ifdef LINX_SERIAL_INTERFACE_ENABLED
    setupLINXSerialInterface();
  #endif
  
  #ifdef DEBUG_ENABLED
    Serial1.println("Entering Config Mode");   
  #endif  //DEBUG_ENABLED   
  
  startTime = millis();
  while( (millis() - startTime) < configTimeout)
  {
    //Check Config Data (Or Normal Serial Commands
    #ifdef LINX_SERIAL_INTERFACE_ENABLED
      checkForLINXSerialPacket();
    #endif
  }
  
  #ifdef DEBUG_ENABLED
    Serial1.println("Entering Normal Operation");   
  #endif  //DEBUG_ENABLED   
  
  // Configure Other Interfaces
  #ifdef LINX_ETHERNET_INTERFACE_ENABLED
    setupLINXEthernetInterface();
  #endif //LINX_Ethernet_INTERFACE_ENABLED
  #ifdef LINX_WIFI_INTERFACE_ENABLED
    setupLINXWifiInterface();
  #endif //LINX_WIFI_INTERFACE_ENABLED  
}


//--------------------------- getDeviceID ---------------------------------------------//
void getDeviceID(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x08;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS
  #ifdef DEVICE_FAMILY
    responsePacketBuffer[5] = DEVICE_FAMILY;                         //Device Family
  #else
    responsePacketBuffer[5] = 0x00;                                 
  #endif
  #ifdef DEVICE_ID
    responsePacketBuffer[6] = DEVICE_ID;                             //Device ID
  #else
    responsePacketBuffer[6] = 0x00;    
  #endif
 
  responsePacketBuffer[7] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- getFrimwareVersion ---------------------------------------------//
void getFrimwareVersion(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x09;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS
  responsePacketBuffer[5] = FIRMWARE_VER_MAJOR;                      //Version
  responsePacketBuffer[6] = FIRMWARE_VER_MINOR;                      //...
  responsePacketBuffer[7] = FIRMWARE_VER_SUBMINOR;                   //... 
  responsePacketBuffer[8] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- setUserDeviceID ---------------------------------------------//
void setUserDeviceID(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Update Value In RAM
  userID = commandPacketBuffer[6] << 8 | commandPacketBuffer[7];
  //Update NVS
  EEPROM.write(NVS_USERID, commandPacketBuffer[6]);
  EEPROM.write(NVS_USERID+1, commandPacketBuffer[7]);
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- getUserDeviceID ---------------------------------------------//
void getUserDeviceID(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x08;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = (userID>>8) & 0xFF;                      //User ID MSB
  responsePacketBuffer[6] = userID & 0xFF;                           //User ID LSB
  responsePacketBuffer[7] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- setEthernetIP ---------------------------------------------//
void setEthernetIP(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Update Value In RAM
  ethernetIP = (commandPacketBuffer[6]<<24) | (commandPacketBuffer[7]<<16) | (commandPacketBuffer[8]<<8) | (commandPacketBuffer[9]);
  //Update NVS
  EEPROM.write(NVS_ETHERNET_IP, commandPacketBuffer[6]);
  EEPROM.write(NVS_ETHERNET_IP+1, commandPacketBuffer[7]);
  EEPROM.write(NVS_ETHERNET_IP+2, commandPacketBuffer[8]);
  EEPROM.write(NVS_ETHERNET_IP+3, commandPacketBuffer[9]);
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- getEthernetIP ---------------------------------------------//
void getEthernetIP(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x0A;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = ((ethernetIP>>24) & 0xFF);               //Ethernet IP MSB  
  responsePacketBuffer[6] = ((ethernetIP>>16) & 0xFF);               //Ethernet IP ...
  responsePacketBuffer[7] = ((ethernetIP>>8) & 0xFF);                //Ethernet IP ...
  responsePacketBuffer[8] = ((ethernetIP) & 0xFF);                   //Ethernet IP LSB  
  responsePacketBuffer[9] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- setEthernetPort ---------------------------------------------//
void setEthernetPort(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Update Value In RAM
  ethernetPort = ((commandPacketBuffer[6]<<8) | (commandPacketBuffer[7]));
  //Update NVS
  EEPROM.write(NVS_ETHERNET_PORT, commandPacketBuffer[6]);
  EEPROM.write(NVS_ETHERNET_PORT+1, commandPacketBuffer[7]);
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- getEthernetPort ---------------------------------------------//
void getEthernetPort(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x08;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = ((ethernetPort>>8) & 0xFF);              //Ethernet PORT MSB
  responsePacketBuffer[6] = (ethernetPort & 0xFF);                   //Ethernet PORT LSB
  responsePacketBuffer[7] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- setWIFIIP ---------------------------------------------//
void setWifiIP(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Update Value In RAM
  wifiIP = (commandPacketBuffer[6]<<24) | (commandPacketBuffer[7]<<16) | (commandPacketBuffer[8]<<8) | (commandPacketBuffer[9]);
  //Update NVS
  EEPROM.write(NVS_WIFI_IP, commandPacketBuffer[6]);
  EEPROM.write(NVS_WIFI_IP+1, commandPacketBuffer[7]);
  EEPROM.write(NVS_WIFI_IP+2, commandPacketBuffer[8]);
  EEPROM.write(NVS_WIFI_IP+3, commandPacketBuffer[9]);
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- getWifiIP ---------------------------------------------//
void getWifiIP(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x0A;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = ((wifiIP>>24) & 0xFF);                   //WIFI IP MSB
  responsePacketBuffer[6] = ((wifiIP>>16) & 0xFF);                   //WIFI IP ...
  responsePacketBuffer[7] = ((wifiIP>>8) & 0xFF);                    //WIFI IP ...
  responsePacketBuffer[8] = ((wifiIP) & 0xFF);                       //WIFI IP LSB  
  responsePacketBuffer[9] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- setWifiPort ---------------------------------------------//
void setWifiPort(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Update Value In RAM
  wifiPort = ((commandPacketBuffer[6]<<8) | (commandPacketBuffer[7]));
  //Update NVS
  EEPROM.write(NVS_WIFI_PORT, commandPacketBuffer[6]);
  EEPROM.write(NVS_WIFI_PORT+1, commandPacketBuffer[7]);
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- getWifiPort ---------------------------------------------//
void getWifiPort(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x08;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = ((wifiPort>>8) & 0xFF);                  //WIFI PORT MSB
  responsePacketBuffer[6] = (wifiPort & 0xFF);                       //WIFI PORT LSB
  responsePacketBuffer[7] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- setWifiSSID ---------------------------------------------//
void setWifiSSID(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Update SSID Size In RAM And NVS
  if(commandPacketBuffer[6] > 32)
  {
    wifiSSIDSize = 32;
    EEPROM.write(NVS_WIFI_SSID_SIZE, 32);
  }
  else
  {
    wifiSSIDSize =commandPacketBuffer[6];
    EEPROM.write(NVS_WIFI_SSID_SIZE, commandPacketBuffer[6]);
  }
    
  //Update SSID Value In RAM And NVS
  for(int i=0; i<wifiSSIDSize; i++)
  {
    wifiSSID[i] = commandPacketBuffer[7+i];
    EEPROM.write(NVS_WIFI_SSID+i, commandPacketBuffer[7+i]);    
  }
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- getWifiSSID ---------------------------------------------//
void getWifiSSID(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = wifiSSIDSize + 7;                        //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = wifiSSIDSize;                            //SSID SIZE
  
  for(int i=0; i<wifiSSIDSize; i++)
  {
    responsePacketBuffer[i+6] = wifiSSID[i];
  }
    
  responsePacketBuffer[wifiSSIDSize+6] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- setWifiSecurity ---------------------------------------------//
void setWifiSecurity(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Update Value In RAM
  wifiSecurity = commandPacketBuffer[6];
  //Update NVS
  EEPROM.write(NVS_WIFI_SECURITY_TYPE, commandPacketBuffer[6]);
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- getWifiSecurity ---------------------------------------------//
void getWifiSecurity(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x07;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = wifiSecurity;                            //WIFI PORT MSB  
  responsePacketBuffer[6] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- setWifiPw ---------------------------------------------//
void setWifiPw(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Update PW Size In RAM And NVS
  if(commandPacketBuffer[6] > 64)
  {
    wifiPwSize = 64;
    EEPROM.write(NVS_WIFI_PW_SIZE, 64);
  }
  else
  {
    wifiPwSize = commandPacketBuffer[6];
    EEPROM.write(NVS_WIFI_PW_SIZE, commandPacketBuffer[6]);
  }  
  
  //Update PW Value In RAM And NVS
  for(int i=0; i<wifiPwSize; i++)
  {
    wifiPw[i] = commandPacketBuffer[7+i];
    EEPROM.write(NVS_WIFI_PW+i, commandPacketBuffer[i+7]);    
  }
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}


//--------------------------- setSerialInterfaceMaxBaud ---------------------------------------------//
void setSerialInterfaceMaxBaud(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Update Value In RAM
  serialInterfaceMaxBaud = (unsigned long)(((unsigned long)commandPacketBuffer[6]<<24) | ((unsigned long)commandPacketBuffer[7]<<16) | ((unsigned long)commandPacketBuffer[8]<<8) | ((unsigned long)commandPacketBuffer[9]));
  
  //Update NVS
  EEPROM.write(NVS_SERIAL_INTERFACE_MAX_BAUD, commandPacketBuffer[6]);
  EEPROM.write(NVS_SERIAL_INTERFACE_MAX_BAUD+1, commandPacketBuffer[7]);
  EEPROM.write(NVS_SERIAL_INTERFACE_MAX_BAUD+2, commandPacketBuffer[8]);
  EEPROM.write(NVS_SERIAL_INTERFACE_MAX_BAUD+3, commandPacketBuffer[9]);
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- getSerialInterfaceMaxBaud ---------------------------------------------//
void getSerialInterfaceMaxBaud(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x0A;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[5] = ((serialInterfaceMaxBaud>>24) & 0xFF);   //WIFI IP MSB
  responsePacketBuffer[6] = ((serialInterfaceMaxBaud>>16) & 0xFF);   //WIFI IP ...
  responsePacketBuffer[7] = ((serialInterfaceMaxBaud>>8) & 0xFF);    //WIFI IP ...
  responsePacketBuffer[8] = ((serialInterfaceMaxBaud) & 0xFF);       //WIFI IP LSB   
  responsePacketBuffer[9] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}

//--------------------------- getDeviceName ---------------------------------------------//
void getDeviceName(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  U8ArrayResponse(commandPacketBuffer, responsePacketBuffer, DEVICE_NAME_LENGTH, deviceName);
}
  
  


//--------------------------- processCommand ------------------------------------------//
void processCommand(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Store Some Local Values For Convenience
  unsigned char commandLength = commandPacketBuffer[1];
  unsigned int command = commandPacketBuffer[4] << 8 | commandPacketBuffer[5];
  
  #ifdef DEBUG_ENABLED
    Serial1.print("Processing Packet       ..::");
    for(int i=0; i<commandLength; i++)
    {
      Serial1.print("[");
      Serial1.print(commandPacketBuffer[i], HEX);
      Serial1.print("]");
    }
    Serial1.println("::..");
  #endif
  
  //Process Command  
  switch(command)
  {
    /************************************************************************************
    * SYSTEM COMMANDS
    ************************************************************************************/
    case 0x0000: // Sync Packet        
      statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);
      break;
      
    case 0x0003: // Get Device ID     
      getDeviceID(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x0004: // Get Firmware Version   
      getFrimwareVersion(commandPacketBuffer, responsePacketBuffer);
      break;
    
    #ifdef LINX_SERIAL_INTERFACE_ENABLED
    case 0x0005: // Get Max Baud Rate      
      getMaxBaudRate(commandPacketBuffer, responsePacketBuffer);
      break;
      
    case 0x0006: // Set Baud Rate      
      setInterfaceBaudRate(commandPacketBuffer, responsePacketBuffer);
      break;
    #endif  //LINX_SERIAL_INTERFACE_ENABLED
    
    case 0x0008: // Get DIO Channels
      #ifdef NUMDIOCHANS
        linxGetDIOChannels(commandPacketBuffer, responsePacketBuffer);
      #endif //NUMDIOCHANS
       break;
    case 0x0009: // Get AI Channels
      #ifdef NUMAICHANS
        linxGetAIChannels(commandPacketBuffer, responsePacketBuffer);
      #endif //NUMAICHANS
       break;
    case 0x000A: // Get AO Channels
      #ifdef NUMAOCHANS
        linxGetAOChannels(commandPacketBuffer, responsePacketBuffer);
      #endif //NUMAOCHANS
       break;
    case 0x000B: // Get PWM Channels
      #ifdef NUMPWMCHANS
        linxGetPWMChannels(commandPacketBuffer, responsePacketBuffer);
      #endif //NUMPWMCHANS
       break;
    case 0x000C: // Get QE Channels
      #ifdef NUMQECHANS
        linxGetQEChannels(commandPacketBuffer, responsePacketBuffer);
      #endif //NUMQECHANS
       break;
    case 0x000D: // Get UART Channels
      #ifdef NUMUARTCHANS      
        linxGetUARTChannels(commandPacketBuffer, responsePacketBuffer);
      #endif //NUMUARTCHANS
       break;
    case 0x000E: // Get I2C Channels
      #ifdef NUMI2CCHANS
        linxGetI2CChannels(commandPacketBuffer, responsePacketBuffer);
      #endif //NUMI2CCHANS
       break;
    case 0x000F: // Get SPI Channels
      #ifdef NUMSPICHANS
        linxGetSPIChannels(commandPacketBuffer, responsePacketBuffer);
      #endif //NUMSPICHANS
       break;
    case 0x0010: // Get CAN Channels
      #ifdef NUMCANCHANS
        linxGetCANChannels(commandPacketBuffer, responsePacketBuffer);
      #endif //NUMCANCHANS
       break;
       
    
    case 0x0011: // Disconnect
      linxDisconnect(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x0012: // Set Device User ID
      setUserDeviceID(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x0013: // Get Device User ID
      getUserDeviceID(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x0014: // Set Device Ethernet IP
      setEthernetIP(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x0015: // Get Device Ethernet IP
      getEthernetIP(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x0016: // Set Device Ethernet Port
      setEthernetPort(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x0017: // Get Device Ethernet Port
      getEthernetPort(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x0018: // Set Device WIFI IP
      setWifiIP(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x0019: // Get Device WIFI IP
      getWifiIP(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x001A: // Set Device WIFI Port
      setWifiPort(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x001B: // Get Device WIFI Port
      getWifiPort(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x001C: // Set Device WIFI SSID
      setWifiSSID(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x001D: // Get Device WIFI SSID
      getWifiSSID(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x001E: // Set Device WIFI Security Type
      setWifiSecurity(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x001F: // Get Device WIFI Security Type
      getWifiSecurity(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x0020: // Set Device WIFI Password
      setWifiPw(commandPacketBuffer, responsePacketBuffer);
      break;    
    //case 0x0021: // Get Device WIFI Password
      //getWifiPw(commandPacketBuffer, responsePacketBuffer);   //Not Implemented For Security Reasons (not that that's going to stop anyone :D )
      //break;
    case 0x0022: // Set Serial Interface Max Baud
      setSerialInterfaceMaxBaud(commandPacketBuffer, responsePacketBuffer);
      break;  
    case 0x0023: // Get Serial Interface Max Baud
      getSerialInterfaceMaxBaud(commandPacketBuffer, responsePacketBuffer);
      break;
    case 0x0024: // Get Device Name
      getDeviceName(commandPacketBuffer, responsePacketBuffer);
      break;
      
      
    /************************************************************************************
    * DIGITAL I/O
    ************************************************************************************/     
    #ifdef LINX_DIGITAL_ENABLED    
    case 0x0040: // Set Digital Pin Mode
      linxSetDigtalPinMode(commandPacketBuffer, responsePacketBuffer);      
      break;      
    case 0x0041: // Digital Write
      linxDigitalWrite(commandPacketBuffer, responsePacketBuffer);      
      break;      
    case 0x0042: // Digital Read
      linxDigitalRead(commandPacketBuffer, responsePacketBuffer);      
      break;     
    case 0x0043: // Digital Write Square Wave
      linxDigitalWriteSquareWave(commandPacketBuffer, responsePacketBuffer);      
      break;     
    case 0x0044: // Digital Read Pulse Width
      linxDigitalReadPulseWidth(commandPacketBuffer, responsePacketBuffer);      
      break;      
    #endif //LINX_DIGITAL_ENABLED


    /************************************************************************************
    **  Analog I/O
    ************************************************************************************/    
    #ifdef LINX_ANALOG_INPUT_ENABLED
    case 0x0061:  //Get AI Ref
      linxGetAIRef(commandPacketBuffer, responsePacketBuffer);      
      break;
    case 0x0064:  //Analog Read
      linxAnalogRead(commandPacketBuffer, responsePacketBuffer);      
      break;
      
    #endif //LINX_ANALOG_INPUT_ENABLED
    
    /************************************************************************************
    **  UART
    ************************************************************************************/     
    #ifdef LINX_UART_ENABLED
    case 0x0C0: // UART Open
      linxUARTOpen(commandPacketBuffer, responsePacketBuffer);      
      break; 
    case 0x0C1: // UART Set Baud Rate
      linxUARTSetBaudRate(commandPacketBuffer, responsePacketBuffer);      
      break; 
    case 0x0C2: // UART Get Bytes Available
      linxUARTGetBytesAvailable(commandPacketBuffer, responsePacketBuffer);      
      break; 
    case 0x0C3: // UART Read
      linxUARTRead(commandPacketBuffer, responsePacketBuffer);      
      break; 
    case 0x0C4: // UART Write
      linxUARTWrite(commandPacketBuffer, responsePacketBuffer);      
      break;      
    case 0x0C5: // UART Close
      linxUARTClose(commandPacketBuffer, responsePacketBuffer);      
      break; 
    #endif  //LINX_UART_ENABLED


    /************************************************************************************
    **  I2C
    ************************************************************************************/     
    #ifdef LINX_I2C_ENABLED
    case 0x00E0: // I2C Open - Master
      linxI2COpenMaster(commandPacketBuffer, responsePacketBuffer);      
      break;
    case 0x00E2: // I2C Write
      linxI2CWrite(commandPacketBuffer, responsePacketBuffer);      
      break;
    case 0x00E3: // I2C Read
      linxI2CRead(commandPacketBuffer, responsePacketBuffer);      
      break;
    case 0x00E4: // I2C Close
      linxI2CCloseMaster(commandPacketBuffer, responsePacketBuffer);      
      break;
      
    #endif  //LINX_I2C_ENABLED

    /************************************************************************************
    **  PWM
    ************************************************************************************/     
    #ifdef LINX_PWM_ENABLED
      break;
    case 0x0083: // PWM Set Duty Cycle
      linxPWMSetDutyCycle(commandPacketBuffer, responsePacketBuffer);      
      break;
      
    #endif  //LINX_PWM_ENABLED
    
    /************************************************************************************
    **  SPI
    ************************************************************************************/     
    #ifdef LINX_SPI_ENABLED
    case 0x0100: // SPI Open Master
      linxSPIOpenMaster(commandPacketBuffer, responsePacketBuffer);      
      break;
    case 0x0101: // SPI Set Bit Order
      linxSPISetBitOrder(commandPacketBuffer, responsePacketBuffer);      
      break;
    case 0x0103: // SPI Set Mode
      linxSPISetMode(commandPacketBuffer, responsePacketBuffer);      
      break;
    case 0x0104: // SPI Set Frame Size
      //Removed This Command    
      break;
    case 0x0105: // SPI Set CS Logic Level
      //Removed This Command         
      break;
     case 0x0106: // SPI Set CS Pin
      //Removed This Command        
      break;      
    case 0x0107: // SPI Write/Read
      linxSPIWriteRead(commandPacketBuffer, responsePacketBuffer);      
      break;      
    #endif  //LINX_SPI_ENABLED
    
    /************************************************************************************
    **  CUSTOM COMMANDS
    ************************************************************************************/ 
    //Add custom commands here using command range 0xFC00- 0xFFFF  
    case 0xFC00:
       digitalBlinkCustomCommand(commandPacketBuffer, responsePacketBuffer);
       break;
        
      
    default:  //Default Case
       statusResponse(commandPacketBuffer, responsePacketBuffer, 0x01);
       break; 
   }
   
   //If Debugging Enabled Print Response Packet
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

//--------------------------- checksumPassed ------------------------------------------//
bool checksumPassed(unsigned char* packetBuffer)
{
  return (computeChecksum(packetBuffer) == packetBuffer[packetBuffer[1]-1]);
}
 
//--------------------------- computeChecksum -----------------------------------------//
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

//--------------------------- linxDisconnect ------------------------------------------//
void linxDisconnect(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  #ifdef LINX_ETHERNET_INTERFACE_ENABLED
    ethernetState = CLOSE;  
    statusResponse(commandPacketBuffer, responsePacketBuffer, 0);
    
    #ifdef DEBUG_ENABLED          
      Serial1.println("Disconnect Command Received...");
    #endif  //DEBUG_ENABLED   
  #endif //LINX_ETHERNET_INTERFACE_ENABLED
}

//--------------------------- statusResponse ------------------------------------------//
void statusResponse(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, unsigned char linxStatus)
{
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x06;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = linxStatus;                              //STATUS
  responsePacketBuffer[5] = computeChecksum(responsePacketBuffer);   //CHECKSUM   
}

//--------------------------- U8ArrayResponse ------------------------------------------//
void U8ArrayResponse(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, unsigned char ArraySize, unsigned char* data)
{
  for(int i=0; i<ArraySize; i++)
  {
      responsePacketBuffer[i+5] = data[i];      
  }
  
  responsePacketBuffer[0] = 0xFF;                                                //SoF
  responsePacketBuffer[1] = ArraySize + 6;                                       //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                              //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                              //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                                //STATUS
  responsePacketBuffer[ArraySize + 5] = computeChecksum(responsePacketBuffer);   //CHECKSUM
}



/****************************************************************************************
**
**--------------------------- SERIAL INTERFACE ------------------------------------------ 
**
****************************************************************************************/
#ifdef LINX_SERIAL_INTERFACE_ENABLED

//--------------------------- checkForLINXSerialPacket --------------------------------//
void checkForLINXSerialPacket()
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
        
         //Send Response Packet         
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

//--------------------------- setupLINXSerialInterface ---------------------------------------------//
void setupLINXSerialInterface()
{
  //Serial Setup  
  Serial.begin(9600);
}

//--------------------------- setInterfaceBaudRate ---------------------------------------------//
void setInterfaceBaudRate(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
       Serial.end();
       Serial.begin((unsigned long)(((unsigned long)commandPacketBuffer[6]<<24) | ((unsigned long)commandPacketBuffer[7]<<16) | ((unsigned long)commandPacketBuffer[8]<<8) | ((unsigned long)commandPacketBuffer[9])));
       delay(1000);   //Give Host Time To Update Serial Baud Rate 
       
       responsePacketBuffer[0] = 0xFF;                                    //SoF
       responsePacketBuffer[1] = 0x08;                                    //PACKET SIZE
       responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
       responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
       responsePacketBuffer[4] = 0x00;                                    //STATUS
       responsePacketBuffer[5] = 0x13;                                    //Delay MSB (ms)
       responsePacketBuffer[6] = 0x88;                                    //Delay LSB (ms)
       responsePacketBuffer[7] = computeChecksum(responsePacketBuffer);   //CHECKSUM  
}

//--------------------------- getMaxBaudRate ------------------------------------------//
void getMaxBaudRate(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  unsigned long baudRate;
  
  if(serialInterfaceMaxBaud != 0)
  {
    baudRate = serialInterfaceMaxBaud;
  }
  else
  {
     baudRate = 9600;    
  }
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x0A;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS
  responsePacketBuffer[5] = (baudRate >> 24) & 0xFF;                 //Baud Rate MSB
  responsePacketBuffer[6] = (baudRate >> 16) & 0xFF;      
  responsePacketBuffer[7] = (baudRate >> 8) & 0xFF;   
  responsePacketBuffer[8] = (baudRate) & 0xFF;                       //Baud Rate LSB
  responsePacketBuffer[9] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
}
#endif  //LINX_SERIAL_INTERFACE_ENABLED

/****************************************************************************************
**
**--------------------------- ETHERNET INTERFACE --------------------------------------- 
**
****************************************************************************************/
#ifdef LINX_ETHERNET_INTERFACE_ENABLED
//--------------------------- setupLINXEthernetInterface ------------------------------//
void setupLINXEthernetInterface()
{
  
  
  
  IPv4 deviceEthernetIpAddress = {((ethernetIP>>24)&0xFF), ((ethernetIP>>16)&0xFF), ((ethernetIP>>8)&0xFF), (ethernetIP&0xFF)};
  //deviceEthernetIpAddress.rgbIP[0] = ((ethernetIP>>24)&0xFF);
  //deviceEthernetIpAddress.rgbIP[1] = ((ethernetIP>>16)&0xFF);
  //6deviceEthernetIpAddress.rgbIP[2] = ((ethernetIP>>8)&0xFF);
  //deviceEthernetIpAddress.rgbIP[3] = (ethernetIP&0xFF);
  
  //Initialize Ethernet Stack
  DNETcK::begin(deviceEthernetIpAddress);
  
  #ifdef DEBUG_ENABLED          
    Serial1.println("Initializing Ethernet Stack...");
    Serial1.print("Using IP Address - ");
    Serial1.print(deviceEthernetIpAddress.rgbIP[0], DEC);
    Serial1.print(".");
    Serial1.print(deviceEthernetIpAddress.rgbIP[1], DEC);
    Serial1.print(".");
    Serial1.print(deviceEthernetIpAddress.rgbIP[2], DEC);
    Serial1.print(".");
    Serial1.println(deviceEthernetIpAddress.rgbIP[3], DEC);    
  #endif  //DEBUG_ENABLED
  
}

//--------------------------- checkForLINXEthernetPacket ------------------------------//
void checkForLINXEthernetPacket()
{
  //Ethernet Server Core
  switch(ethernetState)
  {
    case LISTEN:  
      //Listen For Connection On Specified Port
      if(ethernetTCPServer.startListening(ethernetPort))
      {
        ethernetState = ISLISTENING;
        
        #ifdef DEBUG_ENABLED          
          Serial1.print("Ethernet Starting To Listen On Port ");
          Serial1.println(ethernetPort, DEC);
        #endif  //DEBUG_ENABLED
      }
      else
      {
        #ifdef DEBUG_ENABLED          
          Serial1.print("Unable To Start Listenging On Ethernet Port ");
          Serial1.println(ethernetPort);
        #endif  //DEBUG_ENABLED
        
        ethernetState = EXIT;        
      }
      break;
      
    case ISLISTENING:
      //Listen For Clients
      #ifdef DEBUG_ENABLED          
          //Serial1.println("STATE => ISLISTENTING");
      #endif  //DEBUG_ENABLED
      if(ethernetTCPServer.isListening(&ethernetStatus))
      {
        ethernetState = AVAILABLECLIENT;
        #ifdef DEBUG_ENABLED          
          Serial1.print("Ethernet Listening On Port ");
          Serial1.println(ethernetPort);
        #endif  //DEBUG_ENABLED
      }
      else if(DNETcK::isStatusAnError(ethernetStatus))
      {
        ethernetState = EXIT;
      }
      else
      {
         #ifdef DEBUG_ENABLED
          //Serial1.print("Ethernet Status => ");         
          //Serial1.println(ethernetStatus, DEC);
        #endif  //DEBUG_ENABLED
        
      }
      break;
      
    case AVAILABLECLIENT:
      #ifdef DEBUG_ENABLED          
        //Serial1.println("STATE => AVAILABLECLIENT");
      #endif  //DEBUG_ENABLED
      if( (ethernetClientCount = ethernetTCPServer.availableClients()) > 0)
      {
        ethernetState = ACCEPTCLIENT;
       
        #ifdef DEBUG_ENABLED          
          Serial1.println("Ethernet Client Available...");
        #endif  //DEBUG_ENABLED
      }
      else
      {
        #ifdef DEBUG_ENABLED          
          //Serial1.println("Waiting For Ethernet Client...");
        #endif  //DEBUG_ENABLED
      }
      break;
      
    case ACCEPTCLIENT:
      //Accept The Cleint Connection
      #ifdef DEBUG_ENABLED          
        //Serial1.println("STATE => ACCEPTCLIENT");
      #endif  //DEBUG_ENABLED
      
      //Close Any Previous Connections Just In Case
      ethernetTCPClient.close();
      
      if(ethernetTCPServer.acceptClient(&ethernetTCPClient))
      {
        ethernetState = READ;
        ethernetStartTime = (unsigned) millis();
        #ifdef DEBUG_ENABLED          
          Serial1.println("Client Connected...");
        #endif  //DEBUG_ENABLED
      }
      else
      {
        ethernetState = CLOSE;
        #ifdef DEBUG_ENABLED          
          Serial1.println("Failed To Accept Ethernet Connection...");
          Serial1.print("Ethernet Status => ");         
          Serial1.println(ethernetStatus, DEC);
        #endif  //DEBUG_ENABLED
      }
      break;
      
    case READ:
      //Read Ethernet TCP Bytes
      
      //If There Are Bytes Available Have A Look, If Not Loop (Remain In Read Unless Timeout)
      if(ethernetTCPClient.available() > 0)
      {
        //Read First Byte, Check If It Is SoF (0xFF)
        if ( (ethernetCommandBuffer[0] = ethernetTCPClient.readByte()) == 0xFF)
        {        
          #ifdef DEBUG_ENABLED          
            Serial1.println("SoF Received...");
          #endif  //DEBUG_ENABLED
          
          //SoF Received, Reset ethernetStartTime
          ethernetStartTime = (unsigned)millis();
          
          //SoF Received Wait For Packet Size
          while(ethernetTCPClient.available() < 1)
          {
            if( ((unsigned)millis() - ethernetStartTime) > ethernetTimeout)
            {
              ethernetState = CLOSE;
              #ifdef DEBUG_ENABLED          
                Serial1.println("Ethernet Connection Timed Out...");
              #endif  //DEBUG_ENABLED   
            }             
          }         
          ethernetCommandBuffer[1] = ethernetTCPClient.readByte();
          #ifdef DEBUG_ENABLED          
            Serial1.println("Packet Size Received...");
          #endif  //DEBUG_ENABLED
          
          //Read Remainder Of Packet As It Comes In
          for (int i=0; i< ethernetCommandBuffer[1]-2; i++)
          {
            while(ethernetTCPClient.available() < 1)
            {
              if( ((unsigned)millis() - ethernetStartTime) > ethernetTimeout)
              {
                ethernetState = CLOSE;
                #ifdef DEBUG_ENABLED          
                  Serial1.println("Ethernet Connection Timed Out...");
                #endif  //DEBUG_ENABLED   
              }
            } 
            ethernetCommandBuffer[i+2] = ethernetTCPClient.readByte();
          }
          #ifdef DEBUG_ENABLED          
            Serial1.println("Full Packet Received...");
          #endif  //DEBUG_ENABLED
          
          //Checksum
          if(checksumPassed(ethernetCommandBuffer))
          {
            #ifdef DEBUG_ENABLED          
              Serial1.println("Checksum Passed, Processing Packet...");
            #endif  //DEBUG_ENABLED
            
            //Process Command And Respond
            processCommand(ethernetCommandBuffer, ethernetResponseBuffer);
            ethernetTCPClient.writeStream(ethernetResponseBuffer, ethernetResponseBuffer[1]);            
          }
          else
          {
            #ifdef DEBUG_ENABLED          
              Serial1.println("Checksum Failed...");
            #endif  //DEBUG_ENABLED            
          }          
        }
        else
        {
          ethernetState = CLOSE;
          #ifdef DEBUG_ENABLED          
            Serial1.println("Bad SoF Received...");
          #endif  //DEBUG_ENABLED          
        }
        
        //Data Received, Reset Timeout
        ethernetStartTime = (unsigned)millis();
        
      }
      //Check For Timeout
      else if( ((unsigned)millis() - ethernetStartTime) > ethernetTimeout)
      {
        ethernetState = CLOSE;
        #ifdef DEBUG_ENABLED          
          Serial1.println("Ethernet Connection Timed Out...");
        #endif  //DEBUG_ENABLED        
      }
      else
      {
        //No Data To Read But No Timeout Either, Loop To Read Again
        ethernetState = READ;
        //ethernetStartTime = (unsigned) millis();
        
        #ifdef DEBUG_ENABLED          
          Serial1.print(((unsigned)millis() - ethernetStartTime), DEC);
          Serial1.println(" mS With No Data");
        #endif  //DEBUG_ENABLED
      } 
      break;
   
    case WRITE:
      //Not Used
       #ifdef DEBUG_ENABLED          
          Serial1.println("Ethernet Write Case...");
        #endif  //DEBUG_ENABLED   
      break;

    case CLOSE:
      //Close TCP Connection, Return To Listening State
      #ifdef DEBUG_ENABLED          
        Serial1.println("Closing Ethernet TCP Connection...");
      #endif  //DEBUG_ENABLED
      ethernetTCPClient.close();
      ethernetState = ISLISTENING;
      break;
      
    case EXIT:
      //Something Went Wrong.  Try To Close Connection And Quit.
      ethernetTCPServer.close();
      ethernetTCPClient.close();
      ethernetState = DONE;
      
      #ifdef DEBUG_ENABLED          
        Serial1.println("Shutting Down Ethernet TCP Server...");
      #endif  //DEBUG_ENABLED
      break;
      
    case DONE:
      //Ethernet TCP Serer Shut Down.
      ethernetState = DONE;
      #ifdef DEBUG_ENABLED          
        Serial1.println("Ethernet TCP Server Offline");
      #endif  //DEBUG_ENABLED
    break;
    
    default:
      ethernetState = EXIT;
      
      #ifdef DEBUG_ENABLED          
        Serial1.println("Unknown Ethernet TCP State");
      #endif  //DEBUG_ENABLED
      break;
  } 
  
  //Every Iteration Run Periodic Network Tasks
  DNETcK::periodicTasks(); 
}

#endif  //LINX_ETHERNET_INTERFACE_ENABLED

/****************************************************************************************
**
**--------------------------- WIFI INTERFACE --------------------------------------- 
**
****************************************************************************************/
#ifdef LINX_WIFI_INTERFACE_ENABLED
//--------------------------- setupLINXWifiInterface ------------------------------//
void setupLINXWifiInterface()
{
  //Configure Wifi IP
  IPv4 deviceWifiIpAddress = {((wifiIP>>24)&0xFF), ((wifiIP>>16)&0xFF), ((wifiIP>>8)&0xFF), (wifiIP&0xFF)};
  
  //Start Wifi Server - TODO UPDATE WITH SECURITY SETTINGS
  #ifdef DEBUG_ENABLED          
    Serial1.println("Initializing Wifi Stack...");
    Serial1.print("Using Wifi IP Address - ");
    Serial1.print(deviceWifiIpAddress.rgbIP[0], DEC);
    Serial1.print(".");
    Serial1.print(deviceWifiIpAddress.rgbIP[1], DEC);
    Serial1.print(".");
    Serial1.print(deviceWifiIpAddress.rgbIP[2], DEC);
    Serial1.print(".");
    Serial1.println(deviceWifiIpAddress.rgbIP[3], DEC);  
  #endif  //DEBUG_ENABLED
  
    
  //Connec To Wifi Network
  int conID = DWIFIcK::INVALID_CONNECTION_ID;
  
  //SSID
  const char * szSsid; 
  szSsid = wifiSSID;
  
  //PW
  const char * szPassPhrase;
  szPassPhrase = wifiPw;  
  
  DWIFIcK::WPA2KEY key;
  for(int i=0; i<32; i++)
  {
    key.rgbKey[i] = wifiPw[i];
  }
  
  
  
  switch(wifiSecurity)
  {
     case 0x00:
       //No Security
       #ifdef DEBUG_ENABLED  
         Serial1.println("Connecting To Unsecure Network...");
       #endif //DEBUG_ENABLED
       conID = DWIFIcK::connect(szSsid, &wifiStatus);
       break;
     case 0x01:
       //WPA2 Passphrase       
       conID = DWIFIcK::connect(szSsid, szPassPhrase, &wifiStatus);
       break;
     case 0x02:
       //WPA2 Key  --Untested--
        conID = DWIFIcK::connect(szSsid, szPassPhrase, &wifiStatus);
       break;
     case 0x03:
       #ifdef DEBUG_ENABLED  
         Serial1.println("WEP40");
       #endif //DEBUG_ENABLED
       break;
     case 0x04:
       #ifdef DEBUG_ENABLED  
         Serial1.println("WEP104");
       #endif //DEBUG_ENABLED
       break;
     default:
       #ifdef DEBUG_ENABLED  
         Serial1.println("Unknown");
       #endif //DEBUG_ENABLED
       break;     
   }
  
 
  //
  if(conID != DWIFIcK::INVALID_CONNECTION_ID)
  {
    #ifdef DEBUG_ENABLED          
      Serial1.println("Connecting To Wifi Network");
    #endif  //DEBUG_ENABLED
    wifiState = INITIALIZE;
  }
  else
  {
    #ifdef DEBUG_ENABLED          
    Serial1.print("Unable to connection, status: ");
    Serial1.println(wifiStatus, DEC);
    #endif  //DEBUG_ENABLED
    wifiState = EXIT;
  }

 // intialize the stack with a static IP
  DNETcK::begin(deviceWifiIpAddress); 
}

//--------------------------- checkForLINXWifiPacket ------------------------------//
void checkForLINXWifiPacket()
{
  if(wifiServerFail == 0)
    {
    //Wifi Server Core
    switch(wifiState)
    {
      case INITIALIZE:
          if(DNETcK::isInitialized(&wifiStatus))
          {
            #ifdef DEBUG_ENABLED
              Serial1.println("TCP Stack Initialized");
            #endif  //DEBUG_ENABLED
            wifiState = LISTEN;
          }
          else if(DNETcK::isStatusAnError(wifiStatus))
          {
            #ifdef DEBUG_ENABLED
              Serial1.print("Error in initializing, status: ");
              Serial1.println(wifiStatus, DEC);
            #endif  //DEBUG_ENABLED
            wifiState = EXIT;
          }
          break;    
      case LISTEN:  
        //Listen For Connection On Specified Port
        if(wifiTCPServer.startListening(wifiPort))
        {
          wifiState = ISLISTENING;
          
          #ifdef DEBUG_ENABLED          
            Serial1.print("Wifi Starting To Listen On Port ");
            Serial1.println(wifiPort, DEC);
          #endif  //DEBUG_ENABLED
        }
        else
        {
          #ifdef DEBUG_ENABLED          
            Serial1.print("Unable To Start Listenging On Wifi Port ");
            Serial1.println(wifiPort);
          #endif  //DEBUG_ENABLED
          
          wifiState = EXIT;        
        }
        break;
        
      case ISLISTENING:
        //Listen For Clients
        #ifdef DEBUG_ENABLED          
            //Serial1.println("STATE => ISLISTENTING");
        #endif  //DEBUG_ENABLED
        if(wifiTCPServer.isListening(&wifiStatus))
        {
          wifiState = AVAILABLECLIENT;
          #ifdef DEBUG_ENABLED          
            Serial1.print("Wifi Listening On Port ");
            Serial1.println(wifiPort);
          #endif  //DEBUG_ENABLED
        }
        else if(DNETcK::isStatusAnError(wifiStatus))
        {
          wifiState = EXIT;
        }
        else
        {
           #ifdef DEBUG_ENABLED
            //Serial1.print("Wifi Status => ");         
            //Serial1.println(wifiStatus, DEC);
          #endif  //DEBUG_ENABLED
          
        }
        break;
        
      case AVAILABLECLIENT:
        #ifdef DEBUG_ENABLED          
          //Serial1.println("STATE => AVAILABLECLIENT");
        #endif  //DEBUG_ENABLED
        if( (wifiClientCount = wifiTCPServer.availableClients()) > 0)
        {
          wifiState = ACCEPTCLIENT;
         
          #ifdef DEBUG_ENABLED          
            Serial1.println("Wifi Client Available...");
          #endif  //DEBUG_ENABLED
        }
        else
        {
          #ifdef DEBUG_ENABLED          
            //Serial1.println("Waiting For Wifi Client...");
          #endif  //DEBUG_ENABLED
        }
        break;
        
      case ACCEPTCLIENT:
        //Accept The Cleint Connection
        #ifdef DEBUG_ENABLED          
          //Serial1.println("STATE => ACCEPTCLIENT");
        #endif  //DEBUG_ENABLED
        
        //Close Any Previous Connections Just In Case
        wifiTCPClient.close();
        
        if(wifiTCPServer.acceptClient(&wifiTCPClient))
        {
          wifiState = READ;
          wifiStartTime = (unsigned) millis();
          #ifdef DEBUG_ENABLED          
            Serial1.println("Client Connected...");
          #endif  //DEBUG_ENABLED
        }
        else
        {
          wifiState = CLOSE;
          #ifdef DEBUG_ENABLED          
            Serial1.println("Failed To Accept Wifi Connection...");
            Serial1.print("Wifi Status => ");         
            Serial1.println(wifiStatus, DEC);
          #endif  //DEBUG_ENABLED
        }
        break;
        
      case READ:
        //Read Wifi TCP Bytes
        
        //If There Are Bytes Available Have A Look, If Not Loop (Remain In Read Unless Timeout)
        if(wifiTCPClient.available() > 0)
        {
          //Read First Byte, Check If It Is SoF (0xFF)
          if ( (wifiCommandBuffer[0] = wifiTCPClient.readByte()) == 0xFF)
          {        
            #ifdef DEBUG_ENABLED          
              Serial1.println("SoF Received...");
            #endif  //DEBUG_ENABLED
            
            //SoF Received, Reset wifiStartTime
            wifiStartTime = (unsigned)millis();
            
            //SoF Received Wait For Packet Size
            while(wifiTCPClient.available() < 1)
            {
              if( ((unsigned)millis() - wifiStartTime) > wifiTimeout)
              {
                wifiState = CLOSE;
                #ifdef DEBUG_ENABLED          
                  Serial1.println("Wifi Connection Timed Out...");
                #endif  //DEBUG_ENABLED   
              }             
            }         
            wifiCommandBuffer[1] = wifiTCPClient.readByte();
            #ifdef DEBUG_ENABLED          
              Serial1.println("Packet Size Received...");
            #endif  //DEBUG_ENABLED
            
            //Read Remainder Of Packet As It Comes In
            for (int i=0; i< wifiCommandBuffer[1]-2; i++)
            {
              while(wifiTCPClient.available() < 1)
              {
                if( ((unsigned)millis() - wifiStartTime) > wifiTimeout)
                {
                  wifiState = CLOSE;
                  #ifdef DEBUG_ENABLED          
                    Serial1.println("Wifi Connection Timed Out...");
                  #endif  //DEBUG_ENABLED   
                }
              } 
              wifiCommandBuffer[i+2] = wifiTCPClient.readByte();
            }
            #ifdef DEBUG_ENABLED          
              Serial1.println("Full Packet Received...");
            #endif  //DEBUG_ENABLED
            
            //Checksum
            if(checksumPassed(wifiCommandBuffer))
            {
              #ifdef DEBUG_ENABLED          
                Serial1.println("Checksum Passed, Processing Packet...");
              #endif  //DEBUG_ENABLED
              
              //Process Command And Respond
              processCommand(wifiCommandBuffer, wifiResponseBuffer);
              wifiTCPClient.writeStream(wifiResponseBuffer, wifiResponseBuffer[1]);
              #ifdef DEBUG_ENABLED          
                Serial1.println("WIFI Response Packet Sent");
              #endif  //DEBUG_ENABLED
            }
            else
            {
              #ifdef DEBUG_ENABLED          
                Serial1.println("Checksum Failed...");
              #endif  //DEBUG_ENABLED            
            }          
          }
          else
          {
            wifiState = CLOSE;
            #ifdef DEBUG_ENABLED          
              Serial1.println("Bad SoF Received...");
            #endif  //DEBUG_ENABLED          
          }
          
          //Data Received, Reset Timeout
          wifiStartTime = (unsigned)millis();
          
        }
        //Check For Timeout
        else if( ((unsigned)millis() - wifiStartTime) > wifiTimeout)
        {
          wifiState = CLOSE;
          #ifdef DEBUG_ENABLED          
            Serial1.println("Wifi Connection Timed Out...");
          #endif  //DEBUG_ENABLED        
        }
        else
        {
          //No Data To Read But No Timeout Either, Loop To Read Again
          wifiState = READ;
          //wifiStartTime = (unsigned) millis();
          
          #ifdef DEBUG_ENABLED          
            Serial1.print(((unsigned)millis() - wifiStartTime), DEC);
            Serial1.println(" mS With No Data");
          #endif  //DEBUG_ENABLED
        } 
        break;
     
      case WRITE:
        //Not Used
         #ifdef DEBUG_ENABLED          
            Serial1.println("Wifi Write Case...");
          #endif  //DEBUG_ENABLED   
        break;
  
      case CLOSE:
        //Close TCP Connection, Return To Listening State
        #ifdef DEBUG_ENABLED          
          Serial1.println("Closing Wifi TCP Connection...");
        #endif  //DEBUG_ENABLED
        wifiTCPClient.close();
        wifiState = ISLISTENING;
        break;
        
      case EXIT:
        //Something Went Wrong.  Try To Close Connection And Quit.
        wifiTCPServer.close();
        wifiTCPClient.close();
        wifiState = DONE;
        wifiServerFail = 1;
        
        #ifdef DEBUG_ENABLED          
          Serial1.println("Shutting Down Wifi TCP Server...");
        #endif  //DEBUG_ENABLED
        break;
        
      case DONE:
        //Wifi TCP Serer Shut Down.
        wifiState = DONE;
        wifiServerFail = 1;
        #ifdef DEBUG_ENABLED          
          Serial1.println("Wifi TCP Server Offline");
        #endif  //DEBUG_ENABLED
      break;
      
      default:
        wifiState = EXIT;
        
        #ifdef DEBUG_ENABLED          
          Serial1.println("Unknown Wifi TCP State");
        #endif  //DEBUG_ENABLED
        break;
    } 
    
    //Every Iteration Run Periodic Network Tasks
    DNETcK::periodicTasks(); 
  }
}


#endif //LINX_WIFI_INTERFACE_ENABLED

/****************************************************************************************
**
**------------------------------ SYSTEM ------------------------------------------------- 
**
****************************************************************************************/

//--------------------------- linxGetDIOChannels ----------------------------------------//
void linxGetDIOChannels(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  U8ArrayResponse(commandPacketBuffer, responsePacketBuffer, NUMDIOCHANS, DIOChans);  
}

//--------------------------- linxGetAIChannels ----------------------------------------//
void linxGetAIChannels(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  U8ArrayResponse(commandPacketBuffer, responsePacketBuffer, NUMAICHANS, AIChans);  
}

//--------------------------- linxGetAOChannels ----------------------------------------//
void linxGetAOChannels(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  U8ArrayResponse(commandPacketBuffer, responsePacketBuffer, NUMAOCHANS, AOChans);  
}

//--------------------------- linxGetPWMChannels ----------------------------------------//
void linxGetPWMChannels(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  U8ArrayResponse(commandPacketBuffer, responsePacketBuffer, NUMPWMCHANS, PWMChans);  
}

//--------------------------- linxGetQEChannels ----------------------------------------//
void linxGetQEChannels(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  U8ArrayResponse(commandPacketBuffer, responsePacketBuffer, NUMQECHANS, QEChans);  
}

//--------------------------- linxGetUARTChannels ----------------------------------------//
void linxGetUARTChannels(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  U8ArrayResponse(commandPacketBuffer, responsePacketBuffer, NUMUARTCHANS, UARTChans);  
}

//--------------------------- linxGetI2CChannels ----------------------------------------//
void linxGetI2CChannels(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  U8ArrayResponse(commandPacketBuffer, responsePacketBuffer, NUMI2CCHANS, I2CChans);  
}

//--------------------------- linxGetSPIChannels ----------------------------------------//
void linxGetSPIChannels(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  U8ArrayResponse(commandPacketBuffer, responsePacketBuffer, NUMSPICHANS, SPIChans);  
}

//--------------------------- linxGetCANChannels ----------------------------------------//
void linxGetCANChannels(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  U8ArrayResponse(commandPacketBuffer, responsePacketBuffer, NUMCANCHANS, CANChans);  
}
/****************************************************************************************
**
**--------------------------- DIGITAL IO ------------------------------------------------ 
**
****************************************************************************************/
#ifdef LINX_DIGITAL_ENABLED

//--------------------------- linxDigitalWrite ----------------------------------------//
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
  
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);
}


//--------------------------- linxDigitalRead -----------------------------------------//
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


//--------------------------- linxSetDigitalPinMode -----------------------------------//
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
  
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);
}

//--------------------------- linxDigitalWriteSquareWave ----------------------------------------//
void linxDigitalWriteSquareWave(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{  
  
  unsigned int freq = (commandPacketBuffer[7] << 8) | (commandPacketBuffer[8]);
  unsigned long duration = (commandPacketBuffer[9] << 24) | (commandPacketBuffer[10] << 16) | (commandPacketBuffer[11] << 8) | commandPacketBuffer[12];
  
  #ifdef DEBUG_ENABLED
      Serial1.print("Square Wave Freq =  ");      
      Serial1.println(freq, DEC);
   #endif
  
  
  if(freq == 0)
  {
    noTone(commandPacketBuffer[6]);
  }
  else if(duration == 0)
  {
    tone(commandPacketBuffer[6], freq);
  }
  else
  {
    tone(commandPacketBuffer[6], freq, duration);
  }
  
  
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);
}

//--------------------------- linxDigitalReadPulseWidth ----------------------------------------//
void linxDigitalReadPulseWidth(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{    
  unsigned long timeout = (commandPacketBuffer[10] << 24) | (commandPacketBuffer[11] << 16) | (commandPacketBuffer[12] << 8) | (commandPacketBuffer[13]);
  unsigned long pulseWidth = 0;
  
  //Stimulus
  if(commandPacketBuffer[8] == 1)
  {
    #ifdef DEBUG_ENABLED
      Serial1.println("Sending Active LOW Stim");  
    #endif
    pinMode(commandPacketBuffer[7], OUTPUT);
    
    //High Low High
    digitalWrite(commandPacketBuffer[7], HIGH);
    delay(1);
    digitalWrite(commandPacketBuffer[7], LOW);
    delay(1);
    digitalWrite(commandPacketBuffer[7], HIGH);
    
  }
  else if(commandPacketBuffer[8] == 2)
  {
    #ifdef DEBUG_ENABLED
      Serial1.println("Sending Active HIGH Stim");  
    #endif
    pinMode(commandPacketBuffer[7], OUTPUT);
    
    //Low High Low
    digitalWrite(commandPacketBuffer[7], LOW);
    delay(1);
    digitalWrite(commandPacketBuffer[7], HIGH);
    delay(1);
    digitalWrite(commandPacketBuffer[7], LOW);
  }
  
  pinMode(commandPacketBuffer[6], INPUT);
  if(commandPacketBuffer[9] == 0)
  {
    pulseWidth = pulseIn(commandPacketBuffer[6], LOW,  timeout);
  }
  else if(commandPacketBuffer[9] == 1)
  {
    pulseWidth = pulseIn(commandPacketBuffer[6], HIGH,  timeout);
  }
  
  //Fill In The Rest Of The Response Packet
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x0A;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM 
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM 
  responsePacketBuffer[4] = 0x00;                                    //STATUS
  responsePacketBuffer[5] = (pulseWidth >> 24) & 0xFF;
  responsePacketBuffer[6] = (pulseWidth >> 16) & 0xFF;
  responsePacketBuffer[7] = (pulseWidth >> 8) & 0xFF;
  responsePacketBuffer[8] = pulseWidth & 0xFF; 
  responsePacketBuffer[9] = computeChecksum(responsePacketBuffer);   //CHECKSUM   
}


#endif  //LINX_DIGITAL_ENABLED


/****************************************************************************************
**
**--------------------------- I2C ------------------------------------------------------- 
**
****************************************************************************************/
#ifdef LINX_I2C_ENABLED

//--------------------------- linxI2CClose --------------------------------------------//
void linxI2CClose(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Nothing To Do Here For Current Targets  
 
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);
   
}

 
//--------------------------- linxI2COpenMaster ---------------------------------------//
void linxI2COpenMaster(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
    
  //Only Call Wire.Begin If It Has Not Been Called Since the uC Boot
  //If Wire.Begin Is Called A Second Time The Following Transmit Will Hang The Device
  if(I2C0Open == 0)
  {
    I2C0Open = 1;
    Wire.begin();
  }
    
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00); 
}

void linxI2CCloseMaster(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00); 
}
 
 
//--------------------------- linxI2CWrite --------------------------------------------//
void linxI2CWrite(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  Wire.beginTransmission(commandPacketBuffer[7]);
  #ifdef DEBUG_ENABLED
      Serial1.print("Sending ");      
      Serial1.print(commandPacketBuffer[1] - 9, DEC);
      Serial1.println(" Bytes Via I2C...");
    #endif
    
  for(int i=0; i<(commandPacketBuffer[1] - 9); i++)
  {
    //Hardware Agnostic I2C Write
    linx_WireWrite(commandPacketBuffer[i+8]);
   
      
    #ifdef DEBUG_ENABLED      
      Serial1.print("Sent ");
      Serial1.println(commandPacketBuffer[i+8], HEX);
    #endif
  }
  #ifdef DEBUG_ENABLED      
      Serial1.println("Exited I2C Sent Loop");
  #endif
  Wire.endTransmission();
  
  #ifdef DEBUG_ENABLED      
      Serial1.println("I2C Transfer Complete...");
  #endif
  
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);
}

//--------------------------- linxI2CRead --------------------------------------------//
void linxI2CRead(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Ignore Channel, Assuming Channel 1 With Device As Master
  #ifdef DEBUG_ENABLED      
      Serial1.println("I2C Read...");
  #endif
  
  
 
  //Request Bytes From I2C Device
  Wire.requestFrom(commandPacketBuffer[7], commandPacketBuffer[8]);
  unsigned long reqTime = millis();
  unsigned long timeout = (((unsigned long)commandPacketBuffer[9])<<8) | commandPacketBuffer[10];
  
  //Wait To Receive Bytes Or Tme Out
  while( (Wire.available() < commandPacketBuffer[8]) & ((millis()-reqTime)<timeout) )
  {
    delay(1);
    #ifdef DEBUG_ENABLED      
      Serial1.print(((millis()-reqTime)), DEC);
      Serial1.println("ms");
      Serial1.println(Wire.available());
    #endif    
  }
  
  if(Wire.available() >= commandPacketBuffer[8])
  {
    //All Data Received    
    responsePacketBuffer[0] = 0xFF;                                                //SoF
    responsePacketBuffer[1] = commandPacketBuffer[8] + 6;                          //PACKET SIZE (DATA + 6 BYTES)
    responsePacketBuffer[2] = commandPacketBuffer[2];                              //PACKET NUM 
    responsePacketBuffer[3] = commandPacketBuffer[3];                              //PACKET NUM 
    responsePacketBuffer[4] = 0x00;                                                //STATUS
    
    //Fill Data
    for(int i=0; i<commandPacketBuffer[8]; i++)
    {
      responsePacketBuffer[i+5] = linx_WireReceive();      
    }    
    responsePacketBuffer[commandPacketBuffer[8] + 5] = computeChecksum(responsePacketBuffer);  //CHECKSUM   
  }
  else
  {
    #ifdef DEBUG_ENABLED      
      Serial1.print("I2C Read Timeout.  Read ");
      Serial1.print(Wire.available(), DEC);
      Serial1.println("bytes");
    #endif
  
    //Timeout
    statusResponse(commandPacketBuffer, responsePacketBuffer, 0x80);
  }
  
}

#endif  //LINX_I2C_ENABLED

/****************************************************************************************
**
**--------------------------- ANALOG INPUT ---------------------------------------------- 
**
****************************************************************************************/
#ifdef LINX_ANALOG_INPUT_ENABLED

//--------------------------- linxGetAIRef ------------------------------------------//
void linxGetAIRef(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  unsigned long AIRefV = AI_REFERENCE * 1000000;
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 10;                                      //PACKET NUM (MSB)
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS
  responsePacketBuffer[5] = (AIRefV>>24) & 0xFF;                     //AIREF MSB
  responsePacketBuffer[6] = (AIRefV>>16) & 0xFF;                     //...
  responsePacketBuffer[7] = (AIRefV>>8) & 0xFF;                      //...
  responsePacketBuffer[8] = AIRefV & 0xFF;                           //AIREF LSB
  responsePacketBuffer[9] = computeChecksum(responsePacketBuffer);   //CHECKSUM    
  
}

//--------------------------- linxAnalogRead ------------------------------------------//
void linxAnalogRead(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Prep Response Packet
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS
  responsePacketBuffer[5] = AI_RESOLUTION;                           //RESOLUTION 
  
  unsigned int analogValue = 0;
  unsigned char currentResponseOffset = 6;
  unsigned char responseBitsRemaining = 8; 
  unsigned char dataBitsRemaining = AI_RESOLUTION;
  
  responsePacketBuffer[currentResponseOffset] = 0x00;    //Clear Next Response Byte   
  
  //Loop Over All AI Pins In Command Packet
  for(int i=0; i<(commandPacketBuffer[1]-7); i++)
  {
    analogValue = analogRead(commandPacketBuffer[i+6]);
    
    #ifdef DEBUG_ENABLED
      Serial1.print("AI");
      Serial1.print(commandPacketBuffer[i+6], DEC);
      Serial1.print(" Value = ");
      Serial1.println(analogValue, HEX);
    #endif
    
    dataBitsRemaining = AI_RESOLUTION;
    
    //Byte Packet AI Values In Response Packet
    while(dataBitsRemaining > 0)
    {
      responsePacketBuffer[currentResponseOffset] |= ( (analogValue>>(AI_RESOLUTION - dataBitsRemaining)) << (8 - responseBitsRemaining) );
       
      if(responseBitsRemaining > dataBitsRemaining)
      {
        //Current Byte Still Has Empty Bits
        responseBitsRemaining -= dataBitsRemaining;
        dataBitsRemaining = 0;
      }
      else
      {
        //Current Byte Full
        dataBitsRemaining = dataBitsRemaining - responseBitsRemaining;
        currentResponseOffset++;
        responseBitsRemaining = 8;
        responsePacketBuffer[currentResponseOffset] = 0x00;    //Clear Next Response Byte     
      } 
    } 
  }
  
  //Complete Response Packet
  if(responseBitsRemaining == 8)
  {
    responsePacketBuffer[1] = currentResponseOffset+1;                                       //PACKET SIZE
    responsePacketBuffer[currentResponseOffset] = computeChecksum(responsePacketBuffer);     //CHECKSUM    
  }
  else
  {     
    responsePacketBuffer[1] = currentResponseOffset+2;                                       //PACKET SIZE
    responsePacketBuffer[currentResponseOffset+1] = computeChecksum(responsePacketBuffer);   //CHECKSUM 
  }  
}

#endif  //LINX_ANALOG_INPUT_ENABLED


/****************************************************************************************
**
**--------------------------- PWM ------------------------------------------------------ 
**
****************************************************************************************/


#ifdef LINX_PWM_ENABLED

//--------------------------- linxPWMSetFrequency -------------------------------------//
void linxPWMSetFrequency(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  
  /*TODO
  * Need to figure out how 'cross platform' this is.  Changing Freq will break the analogWrite() function
  * but that could be replaced by platform specific 'set duty cycle' functions.
  */    
}


//--------------------------- linxPWMSetDutyCycle -------------------------------------//
void linxPWMSetDutyCycle(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Loop Over All PWM Pins To Set
  for(int i=0; i<commandPacketBuffer[6]; i++)
  {
    //Config PWM Pin As Output
    pinMode(commandPacketBuffer[i+7], OUTPUT);
    
    //Set Duty Cycle
    analogWrite(commandPacketBuffer[i+7], commandPacketBuffer[i+7+commandPacketBuffer[6]]);
  }  
  
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);
}


#endif  //LINX_PWM_ENABLED

/****************************************************************************************
**
**--------------------------- SPI ------------------------------------------------------ 
**
****************************************************************************************/
#ifdef LINX_SPI_ENABLED

//--------------------------- linxSPIOpenMaster ------------------------------------------------//
void linxSPIOpenMaster(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  SPI.begin();
  
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);  
}


//--------------------------- linxSPISetBitOrder --------------------------------------//
void linxSPISetBitOrder(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  SPI.setBitOrder(commandPacketBuffer[7]);
  
 //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);    
}


//--------------------------- linxSPISetMode ------------------------------------------//
void linxSPISetMode(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  SPI.setDataMode(commandPacketBuffer[7]);
  
 //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);  
}

//--------------------------- linxSPIWriteRead ----------------------------------------//
void linxSPIWriteRead(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  unsigned char numDataBytes = commandPacketBuffer[1] - 11;
  unsigned char frameSize = commandPacketBuffer[7];
  unsigned char SPI_csPin = commandPacketBuffer[8];
  unsigned char SPI_csActiveHigh = commandPacketBuffer[9];
  unsigned char byteCounter = 0;
  
  
  //Prep responsePacketBuffer
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = numDataBytes + 6;                        //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS
  
  //Set CS Pin As DO
  pinMode(SPI_csPin, OUTPUT);
  
  //Set CS Pin Before Starting SPI Transfer
  digitalWrite(SPI_csPin, (SPI_csActiveHigh ^ 0x01));  
  
  //Loop Over Data To Send, Send And Build Response
  for(int i=0; i<numDataBytes; i++)
  {
    if(byteCounter == 0)
    {
      //Start Frame
      digitalWrite(SPI_csPin, SPI_csActiveHigh);      
    }   
    
    responsePacketBuffer[i+5] = SPI.transfer(commandPacketBuffer[i+10]);
    byteCounter++;
   
    if(byteCounter == frameSize)
    {
      //End Of Frame
      digitalWrite(SPI_csPin, (SPI_csActiveHigh ^ 0x01));
      byteCounter = 0; 
      //May Need A Delay Here Depending On SPI Device      
    }   
  }
  
  //Add Response Packet Checksum
  responsePacketBuffer[numDataBytes + 5] = computeChecksum(responsePacketBuffer);   //CHECKSUM    
}

#endif  //LINX_SPI_ENABLED



/****************************************************************************************
**
**--------------------------- UART ------------------------------------------------------ 
**
****************************************************************************************/

#ifdef LINX_UART_ENABLED

//--------------------------- linxUARTOpen --------------------------------------------//
void linxUARTOpen(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  unsigned long baudRate = (commandPacketBuffer[7] << 24) | (commandPacketBuffer[8] << 16) | (commandPacketBuffer[9] << 8) | (commandPacketBuffer[10]);
    
  linx_SerialBegin(commandPacketBuffer[6], baudRate);
  
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);  
}


//--------------------------- linxUARTSetBaudRate -------------------------------------//
void linxUARTSetBaudRate(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  unsigned long baudRate = (commandPacketBuffer[7] << 24) | (commandPacketBuffer[8] << 16) | (commandPacketBuffer[9] << 8) | (commandPacketBuffer[10]);
  
  linx_SerialBegin(commandPacketBuffer[6], baudRate);
  
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);    
}


//--------------------------- linxUARTGetBytesAvailable -------------------------------//
void linxUARTGetBytesAvailable(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  
  responsePacketBuffer[5] = linx_SerialAvailable(commandPacketBuffer[6]);
  
  responsePacketBuffer[0] = 0xFF;                                    //SoF
  responsePacketBuffer[1] = 0x07;                                    //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                  //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                  //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                    //STATUS  
  responsePacketBuffer[6] = computeChecksum(responsePacketBuffer);   //CHECKSUM   
}

//--------------------------- linxUARTRead --------------------------------------------//
void linxUARTRead(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  unsigned char bytesToRead = commandPacketBuffer[7];
    
  for(int i=0; i<bytesToRead; i++)
  {
    responsePacketBuffer[i+5] = linx_SerialRead(commandPacketBuffer[6]);      
  } 
    
  responsePacketBuffer[0] = 0xFF;                                                //SoF
  responsePacketBuffer[1] = bytesToRead + 6;                                     //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                              //PACKET NUM (MSB)
  responsePacketBuffer[3] = commandPacketBuffer[3];                              //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                                //STATUS  
  responsePacketBuffer[bytesToRead+5] = computeChecksum(responsePacketBuffer);   //CHECKSUM     
}

//--------------------------- linxUARTWrite -------------------------------------------//
void linxUARTWrite(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  unsigned char bytesToWrite = commandPacketBuffer[1] - 8;
  
  //Write To UART
  for(int i=0; i<bytesToWrite; i++)
  {
    linx_SerialPrint(commandPacketBuffer[6], commandPacketBuffer[i+7]);      
  }
 
  
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);      
}

//--------------------------- linxUARTClose -------------------------------------------//
void linxUARTClose(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Noting To Do Here...
  
  //Send Status OK Response
  statusResponse(commandPacketBuffer, responsePacketBuffer, 0x00);  
}

#endif  //LINX_UART_ENABLED


//--------------------------- loadNVSConfig -------------------------------------------//
#ifdef LINX_NVS_ENABLED
void loadNVSConfig()
{
  //Debugging
  #ifdef DEBUG_ENABLED
    Serial1.println("Loading User Device Configuration...");
  #endif  
  
  userID = (EEPROM.read(NVS_USERID) << 8) | EEPROM.read(NVS_USERID+1);
  
  ethernetIP = (EEPROM.read(NVS_ETHERNET_IP) << 24) |(EEPROM.read(NVS_ETHERNET_IP+1) << 16) |(EEPROM.read(NVS_ETHERNET_IP+2) << 8) | EEPROM.read(NVS_ETHERNET_IP+3);
  ethernetPort = (EEPROM.read(NVS_ETHERNET_PORT) << 8) | EEPROM.read(NVS_ETHERNET_PORT+1);

  wifiIP = (EEPROM.read(NVS_WIFI_IP) << 24) |(EEPROM.read(NVS_WIFI_IP+1) << 16) |(EEPROM.read(NVS_WIFI_IP+2) << 8) | EEPROM.read(NVS_WIFI_IP+3);
  wifiPort = (EEPROM.read(NVS_WIFI_PORT) << 8) | EEPROM.read(NVS_WIFI_PORT+1);
    
  wifiSSIDSize = EEPROM.read(NVS_WIFI_SSID_SIZE);
  if(wifiSSIDSize > 32)
  {
    wifiSSIDSize = 32;
  }
  
  for(int i=0; i<wifiSSIDSize; i++)
  {
    wifiSSID[i] = EEPROM.read(i+NVS_WIFI_SSID);
  }
  wifiSecurity = EEPROM.read(NVS_WIFI_SECURITY_TYPE);
  wifiPwSize = EEPROM.read(NVS_WIFI_PW_SIZE);
  
  if(wifiPwSize > 64)
  {
    wifiPwSize = 64;
  }
  for(int i=0; i<wifiPwSize; i++)
  {
    wifiPw[i] = EEPROM.read(i+NVS_WIFI_PW);    
  }
  
  //serialInterfaceMaxBaud = ((unsigned long)(((unsigned long)EEPROM.read(NVS_SERIAL_INTERFACE_MAX_BAUD)<<24) | ((unsigned long)EEPROM.read(NVS_SERIAL_INTERFACE_MAX_BAUD+1)<<16) | ((unsigned long)EEPROM.read(NVS_SERIAL_INTERFACE_MAX_BAUD+2)<<8) | ((unsigned long)EEPROM.read(NVS_SERIAL_INTERFACE_MAX_BAUD+3))));
  //if(serialInterfaceMaxBaud < 200 | serialInterfaceMaxBaud > 200000)
  //{
  // serialInterfaceMaxBaud = 9600;
  //}
  
  
 //Debugging
 #ifdef DEBUG_ENABLED
   Serial1.print("User Device ID     = ");
   Serial1.println(userID, HEX);
   
   Serial1.println("Serial Max Baud  = ");
   Serial1.println(serialInterfaceMaxBaud, DEC);
   
   Serial1.print("Ethernet IP        = ");
   Serial1.print((ethernetIP>>24) & 0xFF, DEC);
   Serial1.print(".");
   Serial1.print((ethernetIP>>16) & 0xFF, DEC);   
   Serial1.print(".");
   Serial1.print((ethernetIP>>8) & 0xFF, DEC);   
   Serial1.print(".");
   Serial1.println(ethernetIP & 0xFF, DEC);
   
   Serial1.print("Ethernet Port      = ");
   Serial1.println(ethernetPort, DEC);
   
   Serial1.print("WIFI IP            = ");
   Serial1.print((wifiIP>>24) & 0xFF, DEC);
   Serial1.print(".");
   Serial1.print((wifiIP>>16) & 0xFF, DEC);   
   Serial1.print(".");
   Serial1.print((wifiIP>>8) & 0xFF, DEC);   
   Serial1.print(".");
   Serial1.println(wifiIP & 0xFF, DEC);
   
   Serial1.print("WIFI Port          = ");
   Serial1.println(wifiPort, DEC);
   
   Serial1.print("WIFI SSID Size     = ");
   Serial1.println(wifiSSIDSize, DEC);
   
   Serial1.print("WIFI SSID          = ");
   for(int i=0; i<wifiSSIDSize; i++)
   {
     Serial1.print(wifiSSID[i]);
   }
   Serial1.println();
   
   Serial1.print("WIFI Security Type = ");
   switch(wifiSecurity)
   {
     case 0x00:
       Serial1.println("None");
       break;
     case 0x01:
       Serial1.println("WPA2 Password");
       break;
     case 0x02:
       Serial1.println("WPA2 Key");
       break;
     case 0x03:
       Serial1.println("WEP40");
       break;
     case 0x04:
       Serial1.println("WEP104");
       break;
     default:
       Serial1.println("Unknown");
       break;     
   }
   
   Serial1.print("WIFI PW Size       = ");
   Serial1.println(wifiPwSize, DEC);
   
   Serial1.print("WIFI PW            = ");
   for(int i=0; i<wifiPwSize; i++)
   {
     Serial1.print(wifiPw[i]);
   }
   Serial1.println();
 #endif  //DEBUG_ENABLED
  
}
#endif //LINX_NVS_ENABLED




void linx_WireWrite(unsigned char data)
{
  #if DEVICE_FAMILY == 2
    Wire.write(data);
  #else
    Wire.send(data);
  #endif 
}

unsigned char linx_WireReceive()
{
  #if DEVICE_FAMILY == 2
    //Arduino IDE
    return Wire.read();
  #else
    //MPIDE
    return Wire.receive();
  #endif  
}

void linx_SerialBegin(unsigned char channel, unsigned long baudrate)
{
    switch(channel)
    {
      case 0:
        #if NUMUARTCHANS > 0
          Serial.begin(baudrate);
        #endif //NUMUARTCHANS > 0
        break; 
     case 1:
        #if NUMUARTCHANS > 1
          Serial1.begin(baudrate);
        #endif //NUMUARTCHANS > 1
        break;
     case 2:
        #if NUMUARTCHANS > 2
          Serial2.begin(baudrate);
        #endif //NUMUARTCHANS > 2
        break;
     case 3:
        #if NUMUARTCHANS > 3
          Serial3.begin(baudrate);
        #endif //NUMUARTCHANS > 3
        break;
    }     
}

unsigned char linx_SerialAvailable(unsigned char channel)
{
    switch(channel)
    {
      case 0:
        #if NUMUARTCHANS > 0
          return Serial.available();
        #endif //NUMUARTCHANS > 0
        break; 
     case 1:
        #if NUMUARTCHANS > 1
          return Serial1.available();
        #endif //NUMUARTCHANS > 1
        break;
     case 2:
        #if NUMUARTCHANS > 2
          return Serial2.available();
        #endif //NUMUARTCHANS > 2
        break;
     case 3:
       #if NUMUARTCHANS > 3
          return Serial3.available();
        #endif //NUMUARTCHANS > 3
        break;
    }   
}
  

void linx_SerialPrint(unsigned char channel, unsigned char data)
{
    switch(channel)
    {
      case 0:
        #if NUMUARTCHANS > 0
          Serial.print(data);
        #endif //NUMUARTCHANS > 0
        break; 
     case 1:
       #if NUMUARTCHANS > 1
          Serial1.print(data);
        #endif //NUMUARTCHANS > 1
        break;
     case 2:
        #if NUMUARTCHANS > 2
          Serial2.print(data);
        #endif //NUMUARTCHANS > 2
        break;
     case 3:
        #if NUMUARTCHANS > 3
          Serial3.print(data);
        #endif //NUMUARTCHANS > 3
        break;
    }     
}

unsigned char linx_SerialRead(unsigned char channel)
{
  switch(channel)
    {
      case 0:
        #if NUMUARTCHANS > 0
          return Serial.read();
        #endif //NUMUARTCHANS > 0 
        break; 
     case 1:
        #if NUMUARTCHANS > 1
          return Serial1.read();
        #endif //NUMUARTCHANS > 1 
        break;
     case 2:
        #if NUMUARTCHANS > 2
          return Serial2.read();
        #endif //NUMUARTCHANS > 2 
        break;
     case 3:
        #if NUMUARTCHANS > 3
          return Serial3.read();
        #endif //NUMUARTCHANS > 3 
        break;
    }  
}


//Custom Blink Command
void digitalBlinkCustomCommand(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
  //Pull Data Out Of Packet From LabVIEW
  unsigned char digitalChannel = commandPacketBuffer[6];
  unsigned char onTime = commandPacketBuffer[7];
  unsigned char offTime = commandPacketBuffer[8];
  unsigned char numBlinks = commandPacketBuffer[9];
  
  //Perform Blink Action Using Data From LabVIEW
  pinMode(digitalChannel, OUTPUT);
  for(int i=0; i<numBlinks; i++)
  {
    digitalWrite(digitalChannel, HIGH);
    delay(onTime);
    digitalWrite(digitalChannel, LOW);
    delay(offTime);
  }
  
  //Fill Response Packet Buffer With Response Data
  responsePacketBuffer[0] = 0xFF;                                              //SoF Always 0xFF
  responsePacketBuffer[1] = 14;                                                //PACKET SIZE
  responsePacketBuffer[2] = commandPacketBuffer[2];                            //PACKET NUM (MSB) This Matches The Packet Number We Got From LV
  responsePacketBuffer[3] = commandPacketBuffer[3];                            //PACKET NUM (LSB)
  responsePacketBuffer[4] = 0x00;                                              //STATUS - OK 
  responsePacketBuffer[5] = 'A';                                               //Response Data "All Done"
  responsePacketBuffer[6] = 'l';
  responsePacketBuffer[7] = 'l';
  responsePacketBuffer[8] = ' ';
  responsePacketBuffer[9] = 'D';
  responsePacketBuffer[10] = 'o';
  responsePacketBuffer[11] = 'n';
  responsePacketBuffer[12] = 'e';    
  responsePacketBuffer[13] = computeChecksum(responsePacketBuffer);  //CHECKSUM - Use Helper Function To Calculate Checksum After Buffer Is Loaded With All Response Data
}



