/****************************************************************************************
**  LINX Configuration data for the Raspberry Pi
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/		

/*
* !! This file should be moved to a better location. Since Wiring devices are scripted into libraries they all depend on this file that is scripted for each specific device config.
*/

#ifndef LINXCONFIG

	#define LINXCONFIG

	#define DEBUG_ENABLED 0
	#define NUM_I2C_CHANS 1
	#define NUM_SPI_CHANS 1
	#define NUM_UART_CHANS 0
	#define DEVICE_FAMILY 4

	#define ARDUINO_VERSION 0

	
#endif //LINXCONFIG
