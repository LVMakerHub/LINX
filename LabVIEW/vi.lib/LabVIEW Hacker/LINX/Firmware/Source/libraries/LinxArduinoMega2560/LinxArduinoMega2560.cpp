/****************************************************************************************
**  LINX Arduino Mega 2560 Code
**
**  For more information see:           www.labviewhacker.com/linx
**  For support visit the forums at:    www.labviewhacker.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include <SPI.h>

#include "utility/LinxDevice.h"
#include "utility/LinxWiringDevice.h"
#include "utility/LinxArduino.h"
#include "LinxArduinoMega2560.h"

/****************************************************************************************
**  Member Variables
****************************************************************************************/
//System
const unsigned char LinxArduinoMega2560::m_DeviceName[DEVICE_NAME_LEN] = "Arduino Mega 2560";

//AI
const unsigned char LinxArduinoMega2560::m_AiChans[NUM_AI_CHANS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

//AO
//None

//DIGITAL
const unsigned char LinxArduinoMega2560::m_DigitalChans[NUM_DIGITAL_CHANS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53};

//PWM
const unsigned char LinxArduinoMega2560::m_PwmChans[NUM_PWM_CHANS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 44, 45, 46};

//QE
//None

//SPI
const unsigned char LinxArduinoMega2560::m_SpiChans[NUM_SPI_CHANS] = {0};
unsigned long LinxArduinoMega2560::m_SpiSupportedSpeeds[NUM_SPI_SPEEDS] = {8000000, 4000000, 2000000, 1000000, 500000, 250000, 125000};
int LinxArduinoMega2560::m_SpiSpeedCodes[NUM_SPI_SPEEDS] = {SPI_CLOCK_DIV2, SPI_CLOCK_DIV4, SPI_CLOCK_DIV8, SPI_CLOCK_DIV16, SPI_CLOCK_DIV32, SPI_CLOCK_DIV64, SPI_CLOCK_DIV128};

//I2C
unsigned char LinxArduinoMega2560::m_I2cChans[NUM_I2C_CHANS] = {0};
unsigned char LinxArduinoMega2560::m_I2cRefCount[NUM_I2C_CHANS];			

//UART
unsigned char LinxArduinoMega2560::m_UartChans[NUM_UART_CHANS] = {0, 1, 2, 3};
unsigned long LinxArduinoMega2560::m_UartSupportedSpeeds[NUM_UART_SPEEDS] = {300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 31250, 38400, 57600, 115200};

/****************************************************************************************
**  Constructors /  Destructor
****************************************************************************************/
LinxArduinoMega2560::LinxArduinoMega2560()
{
	//Arduino Family Code Set At Family Level
	DeviceID = 0x01;	//Mega 2560
	DeviceNameLen = DEVICE_NAME_LEN;	 
	DeviceName =  m_DeviceName;

	//LINX API Version
	LinxApiMajor = 1;
	LinxApiMinor = 2;
	LinxApiSubminor = 0;
	
	//DIGITAL
	NumDigitalChans = NUM_DIGITAL_CHANS;			
	DigitalChans = m_DigitalChans;
		
	//AI
	NumAiChans = NUM_AI_CHANS;
	AiChans = m_AiChans;
	AiResolution = AI_RES_BITS;
	AiRef = AI_REFV;
	
	//AO
	NumAoChans = 0;
	AoChans = 0;
	
	//PWM
	NumPwmChans = NUM_PWM_CHANS;
	PwmChans = m_PwmChans;
	
	//QE
	NumQeChans = 0;
	QeChans = 0;
	
	
	//UART
	NumUartChans = NUM_UART_CHANS;
	UartChans = m_UartChans;	
	UartMaxBaud = m_UartSupportedSpeeds[NUM_UART_SPEEDS - 1];
	NumUartSpeeds = NUM_UART_SPEEDS;
	UartSupportedSpeeds = m_UartSupportedSpeeds;

	//I2C
	NumI2cChans = NUM_I2C_CHANS;	
	I2cChans = m_I2cChans;
	I2cRefCount = m_I2cRefCount;
		
	//SPI
	NumSpiChans = NUM_SPI_CHANS;	
	SpiChans = m_SpiChans;		
	NumSpiSpeeds = NUM_SPI_SPEEDS;
	SpiSupportedSpeeds = m_SpiSupportedSpeeds;
	SpiSpeedCodes = m_SpiSpeedCodes;
		
	//CAN
	NumCanChans = 0;
	CanChans = 0;
	
	//If Debuging Is Enabled Call EnableDebug()
	#if DEBUG_ENABLED > 0
		EnableDebug(DEBUG_ENABLED);
	#endif
}

//Destructor
LinxArduinoMega2560::~LinxArduinoMega2560()
{
	//Handle Any Device Clean Up Here.
	//UartClose();
}

/****************************************************************************************
**  Functions
****************************************************************************************/

