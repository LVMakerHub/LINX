/****************************************************************************************
**  LINX chipKIT Uno32 code
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
#include "utility/LinxChipkit.h"
#include "LinxChipkitUno32.h"

/****************************************************************************************
**  Member Variables
****************************************************************************************/
//System
const unsigned char LinxChipkitUno32::m_DeviceName[DEVICE_NAME_LEN] = "ChipKIT Uno32";

//AI
const unsigned char LinxChipkitUno32::m_AiChans[NUM_AI_CHANS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
const unsigned long LinxChipkitUno32::m_AiRefIntVals[NUM_AI_INT_REFS] = {};
int LinxChipkitUno32::m_AiRefCodes[NUM_AI_INT_REFS] = {};

//AO
//None

//DIGITAL
const unsigned char LinxChipkitUno32::m_DigitalChans[NUM_DIGITAL_CHANS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43};

//PWM
const unsigned char LinxChipkitUno32::m_PwmChans[NUM_PWM_CHANS] = {3, 5, 6, 9, 10};

//QE
//None

//SPI
const unsigned char LinxChipkitUno32::m_SpiChans[NUM_SPI_CHANS] = {0};
unsigned long LinxChipkitUno32::m_SpiSupportedSpeeds[NUM_SPI_SPEEDS] = {40000000, 20000000, 10000000, 5000000, 2500000, 1250000, 625000};
int LinxChipkitUno32::m_SpiSpeedCodes[NUM_SPI_SPEEDS] = {SPI_CLOCK_DIV2, SPI_CLOCK_DIV4, SPI_CLOCK_DIV8, SPI_CLOCK_DIV16, SPI_CLOCK_DIV32, SPI_CLOCK_DIV64, SPI_CLOCK_DIV128};

//I2C
unsigned char LinxChipkitUno32::m_I2cChans[NUM_I2C_CHANS] = {0};
unsigned char LinxChipkitUno32::m_I2cRefCount[NUM_I2C_CHANS];			

//UART
unsigned char LinxChipkitUno32::m_UartChans[NUM_UART_CHANS] = {0, 1};
unsigned long LinxChipkitUno32::m_UartSupportedSpeeds[NUM_UART_SPEEDS] = {300, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200};

//SERVO
Servo* LinxChipkitUno32::m_Servos[NUM_SERVO_CHANS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};			//Initialize To Null Pointers

/****************************************************************************************
**  Constructors /  Destructor
****************************************************************************************/
LinxChipkitUno32::LinxChipkitUno32()
{
	//Digilent Family Code Set At Family Level
	DeviceId = 0x00;	//Uno32
	DeviceNameLen = DEVICE_NAME_LEN;	 
	DeviceName =  m_DeviceName;

	//LINX API Version
	LinxApiMajor = 3;
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
	AiRefExtMax = 3300000;
	
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
	UartMaxBaud = 115200;		//TODO Move this to a LinxDevice Method	
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
	#if DEBUG_ENABLED > 0
		EnableDebug(DEBUG_ENABLED);
	#endif
}

//Destructor
LinxChipkitUno32::~LinxChipkitUno32()
{
	//Handle Any Device Clean Up Here.
	//UartClose();
}

/****************************************************************************************
**  Functions
****************************************************************************************/

