/****************************************************************************************
**  Configuration
****************************************************************************************/

//Prevent Multiple Includes
#ifndef LINX_CONFIG_H
#define LINX_CONFIG_H

//--------------------------- Device Config -------------------------------------------//
  //Uncomment To Enable Debugging On The Secondary Serial Port.
  //#define DEBUG_ENABLED 1
  
  //Uncomment A Line Below To Enable LINX Packets On The Specified Interface.
  #define LINX_SERIAL_INTERFACE_ENABLED 1
  //#define LINX_ETHERNET_INTERFACE_ENABLED 1
  //#define LINX_WIFI_INTERFACE_ENABLED 1
  
  //Device ID
  #define DEVICE_FAMILY 1
  #define DEVICE_ID 1
  
  //Serial Settings
  #define MAX_BAUD_RATE 115200
  
  //Analog Settings
  #define AI_RESOLUTION 10

//--------------------------- Peripheral Config ---------------------------------------//
  //Comment Lines Below To Disable Various Peripherals
  #define LINX_DIGITAL_ENABLED 1
  #define LINX_I2C_ENABLED 1
  #define LINX_ANALOG_INPUT_ENABLED 1
#endif  //LINX_CONFIG_H

