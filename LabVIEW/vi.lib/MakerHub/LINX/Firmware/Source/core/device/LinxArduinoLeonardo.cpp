/****************************************************************************************
**  LINX Arduino Leonardo code
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
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
#include "LinxArduinoLeonardo.h"

/****************************************************************************************
**  Member Variables
****************************************************************************************/
//System
const unsigned char LinxArduinoLeonardo::m_DeviceName[DEVICE_NAME_LEN] = "Arduino Leonardo";

//AI
const unsigned char LinxArduinoLeonardo::m_AiChans[NUM_AI_CHANS] = {0, 1, 2, 3, 4, 5};
const unsigned long LinxArduinoLeonardo::m_AiRefIntVals[NUM_AI_INT_REFS] = {1100000};
const int LinxArduinoLeonardo::m_AiRefCodes[NUM_AI_INT_REFS] = {INTERNAL};

//AO
//None

//DIGITAL
const unsigned char LinxArduinoLeonardo::m_DigitalChans[NUM_DIGITAL_CHANS] ={0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

//PWM
const unsigned char LinxArduinoLeonardo::m_PwmChans[NUM_PWM_CHANS] = {3, 5, 6, 9, 10, 11, 13};

//QE
//None

//SPI
const unsigned char LinxArduinoLeonardo::m_SpiChans[NUM_SPI_CHANS] = {0};
unsigned long LinxArduinoLeonardo::m_SpiSupportedSpeeds[NUM_SPI_SPEEDS] = {8000000, 4000000, 2000000, 1000000, 500000, 250000, 125000};
int LinxArduinoLeonardo::m_SpiSpeedCodes[NUM_SPI_SPEEDS] = {SPI_CLOCK_DIV2, SPI_CLOCK_DIV4, SPI_CLOCK_DIV8, SPI_CLOCK_DIV16, SPI_CLOCK_DIV32, SPI_CLOCK_DIV64, SPI_CLOCK_DIV128};

//I2C
unsigned char LinxArduinoLeonardo::m_I2cChans[NUM_I2C_CHANS] = {0};
unsigned char LinxArduinoLeonardo::m_I2cRefCount[NUM_I2C_CHANS];			

//UART
unsigned char LinxArduinoLeonardo::m_UartChans[NUM_UART_CHANS] = {0};
unsigned long LinxArduinoLeonardo::m_UartSupportedSpeeds[NUM_UART_SPEEDS] = {300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 31250, 38400, 57600, 115200};

//SERVO
Servo* LinxArduinoLeonardo::m_Servos[NUM_SERVO_CHANS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};			//Initialize To Null Pointers

/****************************************************************************************
**  Constructors /  Destructor
****************************************************************************************/
LinxArduinoLeonardo::LinxArduinoLeonardo()
{
	//Arduino Family Code Set At Family Level
	DeviceID = 0x02;	//Leonardo
	DeviceNameLen = DEVICE_NAME_LEN;	 
	DeviceName =  m_DeviceName;

	//LINX API Version
	LinxApiMajor = 2;
	LinxApiMinor = 0;
	LinxApiSubminor = 0;
	
	//DIGITAL
	NumDigitalChans = NUM_DIGITAL_CHANS;			
	DigitalChans = m_DigitalChans;
		
	//AI
	NumAiChans = NUM_AI_CHANS;
	AiChans = m_AiChans;
	AiResolution = AI_RES_BITS;
	AiRefSet = AI_REFV;
	
	AiRefDefault = AI_REFV;
	AiRefSet = AI_REFV;
	AiRefCodes = m_AiRefCodes;
	
	NumAiRefIntVals = NUM_AI_INT_REFS;
	AiRefIntVals = m_AiRefIntVals;
	
	AiRefExtMin = 0;
	AiRefExtMax = 5000000;
	
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
	
	//SERVO
	NumServoChans = NUM_SERVO_CHANS;	
	ServoChans = m_DigitalChans;
	Servos = m_Servos;
	
	//If Debuging Is Enabled Call EnableDebug()
	#if DEBUG_ENABLED >= 0
		EnableDebug(DEBUG_ENABLED);
	#endif
}

//Destructor
LinxArduinoLeonardo::~LinxArduinoLeonardo()
{
	//Handle Any Device Clean Up Here.
	//UartClose();
}

/****************************************************************************************
**  Functions
****************************************************************************************/

