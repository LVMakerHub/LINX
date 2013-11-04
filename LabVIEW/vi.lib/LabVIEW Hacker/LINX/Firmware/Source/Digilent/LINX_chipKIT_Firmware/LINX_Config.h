/****************************************************************************************
**  Configuration
****************************************************************************************/


//Prevent Multiple Includes
#ifndef LINX_CONFIG_H
#define LINX_CONFIG_H

/****************************************************************************************
**
**--------------------------- DEVICE CONFIGURATION -------------------------------------- 
**
****************************************************************************************/
  //Uncomment To Enable Debugging On The Secondary Serial Port.
  #define DEBUG_ENABLED 1
  
  //Uncomment A Section Below To Enable LINX Packets On The Specified Interface.
  
  //Serial
  #define LINX_SERIAL_INTERFACE_ENABLED 1
  #define MAX_BAUD_RATE 115200
  
  //Ethernet
  //#define LINX_ETHERNET_INTERFACE_ENABLED 1
  //#define LINX_ETHERNET_DEFAULT_IP {10, 2, 106, 213}
  //#define LINX_ETHERNET_DEFAULT_PORT 6921
  //#define LINX_ETHERNET_RX_BUFF_SIZE 256
  //#define LINX_ETHERNET_TX_BUFF_SIZE 256
  //#define LINX_ETHERNET_DEFAULT_TIMEOUT 1000              //Ethernet Timeout In mS
  
  //WIFI
  //#define LINX_WIFI_INTERFACE_ENABLED 1
  
  //Device ID
  #define DEVICE_FAMILY 1
  #define DEVICE_ID 2
  
  //Serial Settings
  #define MAX_BAUD_RATE 115200
  
  //Analog Settings
  #define AI_RESOLUTION 10

/****************************************************************************************
**
**--------------------------- PERIPHERAL ENABLED / DISABLE ------------------------------ 
**
****************************************************************************************/
  //Comment Lines Below To Disable Various Peripherals
  #define LINX_DIGITAL_ENABLED 1
  #define LINX_I2C_ENABLED 1
  #define LINX_ANALOG_INPUT_ENABLED 1
  //#define LINX_ANALOG_OUTPUT_ENABLED 1
  #define LINX_PWM_ENABLED 1
  #define LINX_SPI_ENABLED 1
  #define LINX_UART_ENABLED 1
  #define LINX_NVS_ENABLED 1
  
/****************************************************************************************
**
**--------------------------- PERIPHERAL CONFIGURATION ---------------------------------- 
**
****************************************************************************************/

//--------------------------- SPI -----------------------------------------------------//
unsigned char SPI_csActiveLow  = 1;  //CS Pin Defaults To Active Low
unsigned char SPI_csPin = 10;        //Default CS Pin
#endif  //LINX_CONFIG_H

