/****************************************************************************************
**  LINX Raspberry Pi 5 Code
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Ken Sharp
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include <termios.h>
#include <unistd.h>

#include "utility/LinxDevice.h"
#include "utility/LinxRaspberryPi.h"
#include "LinxRaspberryPi5.h"

/****************************************************************************************
**  Member Variables
****************************************************************************************/
//System
const unsigned char LinxRaspberryPi5::m_DeviceName[DEVICE_NAME_LEN] = "Raspberry Pi 5";

//AI
//None 

//AO
//None

//DIGITAL
const unsigned char LinxRaspberryPi5::m_DigitalChans[NUM_DIGITAL_CHANS] = {7, 11, 12, 13, 15, 16, 18, 22, 29, 31, 32, 33, 35, 36, 37, 38, 40};
const unsigned int LinxRaspberryPi5::m_gpioChan[NUM_DIGITAL_CHANS] =     {4, 17, 18, 27, 22, 23, 24, 25, 5, 6, 12, 13, 19, 16, 26, 20, 21};

//PWM
//None

//QE
//None

//SPI
const unsigned char LinxRaspberryPi5::m_SpiChans[NUM_SPI_CHANS] = {0};
string m_SpiPaths[NUM_SPI_CHANS] = { "/dev/spidev0.1"};
unsigned long LinxRaspberryPi5::m_SpiSupportedSpeeds[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};
int LinxRaspberryPi5::m_SpiSpeedCodes[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};

//I2C
unsigned char LinxRaspberryPi5::m_I2cChans[NUM_I2C_CHANS] = {1};
string m_I2cPaths[NUM_I2C_CHANS] = {"/dev/i2c-1"};
unsigned char LinxRaspberryPi5::m_I2cRefCount[NUM_I2C_CHANS];

//UART
unsigned char LinxRaspberryPi5::m_UartChans[NUM_UART_CHANS] = {0};
int LinxRaspberryPi5::m_UartHandles[NUM_UART_CHANS];
string LinxRaspberryPi5::m_UartPaths[NUM_UART_CHANS] = {"/dev/serial0"};
unsigned long LinxRaspberryPi5::m_UartSupportedSpeeds[NUM_UART_SPEEDS] = {0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
unsigned long LinxRaspberryPi5::m_UartSupportedSpeedsCodes[NUM_UART_SPEEDS] = {B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200};

//SERVO
//None

/****************************************************************************************
**  Constructors /  Destructor
****************************************************************************************/
LinxRaspberryPi5::LinxRaspberryPi5()
{
	DeviceFamily = 0x04;	//Raspberry Pi Family Code
	DeviceId = 0x05;			//Raspberry Pi 5
	DeviceNameLen = DEVICE_NAME_LEN;	 
	DeviceName =  m_DeviceName;

	//LINX API Version
	LinxApiMajor = 2;
	LinxApiMinor = 2;
	LinxApiSubminor = 0;
	
	//DIGITAL
	NumDigitalChans = NUM_DIGITAL_CHANS;			
	DigitalChans = m_DigitalChans;
		
	//AI
	NumAiChans = NUM_AI_CHANS;
	AiChans = 0;
	AiResolution = 0;
	AiRefSet = 0;
		
	AiRefDefault = AI_REFV;
	AiRefSet = AI_REFV;
	AiRefCodes = NULL;
	
	NumAiRefIntVals = NUM_AI_INT_REFS;
	AiRefIntVals = NULL;
	
	AiRefExtMin = 0;
	AiRefExtMax = 0;
	
	//AO
	NumAoChans = 0;
	AoChans = 0;
	AoResolution = 0;
    AoRefDefault = 0;
	AoRefSet = 0;
	
	//PWM
	NumPwmChans = NUM_PWM_CHANS;
	PwmChans = 0;
	
	//QE
	NumQeChans = 0;
	QeChans = 0;
		
	//UART
	NumUartChans = NUM_UART_CHANS;
	UartChans = m_UartChans;	
	UartMaxBaud = m_UartSupportedSpeeds[NUM_UART_SPEEDS - 1];
	NumUartSpeeds = NUM_UART_SPEEDS;
	UartSupportedSpeeds = m_UartSupportedSpeeds;
	UartSupportedSpeedsCodes = m_UartSupportedSpeedsCodes;
	
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
	NumCanChans = NUM_CAN_CHANS;
	CanChans = 0;
	
	//Servo 
	NumServoChans = NUM_SERVO_CHANS;
	ServoChans = 0;
			
	//------------------------------------- Digital -------------------------------------
	// GPIO Base PI OS 6.6.31+rpt-rpi-v8 
	m_gpioBase = 571; 
	//Export GPIO - Set All Digital Handles To NULL
	for(int i=0; i<NUM_DIGITAL_CHANS; i++)
	{
		FILE* digitalExportHandle = fopen("/sys/class/gpio/export", "w");
		fprintf(digitalExportHandle, "%d", m_gpioBase + m_gpioChan[i]);
		fclose(digitalExportHandle);
		
		DigitalDirHandles[m_DigitalChans[i]] = NULL;
		DigitalValueHandles[m_DigitalChans[i]] = NULL;
		DigitalChannels[m_DigitalChans[i]] = m_gpioBase + m_gpioChan[i];
		DigitalDirs[m_DigitalChans[i]] = PI_OS_GPIO_SETTING;
	}
	
	//------------------------------------- I2C -------------------------------------
	//Store I2C Master Paths In Map
	for(int i=0; i<NUM_I2C_CHANS; i++)
	{	
		I2cPaths[I2cChans[i]] = m_I2cPaths[i];
	}
	
	//------------------------------------- SPI -------------------------------------
	//Load SPI Paths And Configure SPI Master Default Values	
	SpiDefaultSpeed = 3900000;
	for(int i=0; i<NUM_SPI_CHANS; i++)
	{				
		SpiBitOrders[SpiChans[i]] = MSBFIRST;		//MSB First
		SpiSetSpeeds[SpiChans[i]] = SpiDefaultSpeed;
		SpiPaths[SpiChans[i]] = m_SpiPaths[i];
	}
	
	//------------------------------------- UART -------------------------------------
	for(int i=0; i<NUM_UART_CHANS; i++)
	{
		UartPaths[m_UartChans[i]] = m_UartPaths[i];
		UartHandles[m_UartChans[i]] = 0;
	}	
	
	//If Debuging Is Enabled Call EnableDebug()
	#if DEBUG_ENABLED > -1
		EnableDebug(DEBUG_ENABLED);
	#endif
}

//Destructor
LinxRaspberryPi5::~LinxRaspberryPi5()
{	
	//Close GPIO Handles If They Are Open
	for(int i=0; i<NUM_DIGITAL_CHANS; i++)
	{
		if(DigitalDirHandles[m_DigitalChans[i]] != NULL)
		{			
			fclose(DigitalDirHandles[m_DigitalChans[i]]);
		}
		if(DigitalValueHandles[m_DigitalChans[i]] != NULL)
		{			
			fclose(DigitalValueHandles[m_DigitalChans[i]]);
		}
	}
	
	//Close I2C Handles
	for(int i=0; i<NUM_I2C_CHANS; i++)
	{
		if(I2cHandles[m_I2cChans[i]] != 0)
		{	
			close(I2cHandles[m_I2cChans[i]]);
		}
	}
	
	//Close SPI Handles
	for(int i=0; i<NUM_SPI_CHANS; i++)
	{
		if(SpiHandles[m_SpiChans[i]] != 0)
		{	
			close(SpiHandles[m_SpiChans[i]]);
		}
	}
	
	//Close UART Handles
	for(int i=0; i<NUM_UART_CHANS; i++)
	{
		if(UartHandles[m_UartChans[i]] != 0)
		{	
			close(UartHandles[m_UartChans[i]]);
		}
	}	
}

/****************************************************************************************
**  Functions
****************************************************************************************/

