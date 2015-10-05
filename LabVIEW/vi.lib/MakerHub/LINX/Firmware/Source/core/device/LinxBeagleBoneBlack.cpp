/****************************************************************************************
**  LINX BeagleBone Black Code
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
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>		//UART Support
#include <map>
#include <string>

#include "utility/LinxDevice.h"
#include "utility/LinxBeagleBone.h"
#include "LinxBeagleBoneBlack.h"

using namespace std;

/****************************************************************************************
**  Member Variables
****************************************************************************************/
//System
const unsigned char LinxBeagleBoneBlack::m_DeviceName[DEVICE_NAME_LEN] = "BeagleBone Black";

//AI
const unsigned char LinxBeagleBoneBlack::m_AiChans[NUM_AI_CHANS] = {0, 1, 2, 3, 4, 5, 6, 7};
const string LinxBeagleBoneBlack::m_AiValuePaths[NUM_AI_CHANS] = {"/sys/bus/iio/devices/iio:device0/in_voltage0_raw", "/sys/bus/iio/devices/iio:device0/in_voltage1_raw", "/sys/bus/iio/devices/iio:device0/in_voltage2_raw", "/sys/bus/iio/devices/iio:device0/in_voltage3_raw", "/sys/bus/iio/devices/iio:device0/in_voltage4_raw", "/sys/bus/iio/devices/iio:device0/in_voltage5_raw", "/sys/bus/iio/devices/iio:device0/in_voltage6_raw", "/sys/bus/iio/devices/iio:device0/in_voltage7_raw"};
//int LinxBeagleBoneBlack::m_AiHandles[NUM_AI_CHANS];
const unsigned long LinxBeagleBoneBlack::m_AiRefIntVals[NUM_AI_INT_REFS] = {};
const int LinxBeagleBoneBlack::m_AiRefCodes[NUM_AI_INT_REFS] = {};

//AO
//None

//DIGITAL
const unsigned char LinxBeagleBoneBlack::m_DigitalChans[NUM_DIGITAL_CHANS] = { 7, 8, 9, 10, 11, 12, 15, 16, 17, 18, 26,          58, 61, 69, 73, 76};
const unsigned char LinxBeagleBoneBlack::m_gpioChan[NUM_DIGITAL_CHANS] =     {66, 67, 69, 68, 45, 44, 47, 46, 27, 65, 61,      60, 48, 49, 115, 112};

//PWM
const unsigned char LinxBeagleBoneBlack::m_PwmChans[NUM_PWM_CHANS] = {13, 19, 60, 62};
const string LinxBeagleBoneBlack::m_PwmDirPaths[NUM_PWM_CHANS] = {"/sys/devices/ocp.3/pwm_test_P8_13.16/", "/sys/devices/ocp.3/pwm_test_P8_19.17/", "/sys/devices/ocp.3/pwm_test_P9_14.18/", "/sys/devices/ocp.3/pwm_test_P9_16.19/"};
const string LinxBeagleBoneBlack::m_PwmDtoNames[NUM_PWM_CHANS] = {"bone_pwm_P8_13", "bone_pwm_P8_19", "bone_pwm_P9_14", "bone_pwm_P9_16"};
unsigned long m_PwmDefaultFrequency = 3000;

//QE
//None

//SPI
string m_SpiPaths[NUM_SPI_CHANS] = { "/dev/spidev1.1"};
string m_SpiDtoNames[NUM_SPI_CHANS] = { "BB-SPIDEV0"};
unsigned char LinxBeagleBoneBlack::m_SpiChans[NUM_SPI_CHANS] = {0};
unsigned long LinxBeagleBoneBlack::m_SpiSupportedSpeeds[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};
int LinxBeagleBoneBlack::m_SpiSpeedCodes[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};

//I2C
unsigned char LinxBeagleBoneBlack::m_I2cChans[NUM_I2C_CHANS] = {1, 2};
unsigned char LinxBeagleBoneBlack::m_I2cRefCount[NUM_I2C_CHANS];
string m_I2cPaths[NUM_I2C_CHANS] = {"/dev/i2c-2", "/dev/i2c-1" };		//Out of order numbering is correct for BBB!!
string m_I2cDtoNames[NUM_I2C_CHANS] = {"BB-I2C1", "BB-I2C2" };

//UART
string m_UartDtoNames[NUM_UART_CHANS] = {"BB-UART0", "BB-UART1", "BB-UART4"};
unsigned char LinxBeagleBoneBlack::m_UartChans[NUM_UART_CHANS] = {0, 1, 4};
string LinxBeagleBoneBlack::m_UartPaths[NUM_UART_CHANS] = { "/dev/ttyO0", "/dev/ttyO1", "/dev/ttyO4"};
unsigned long LinxBeagleBoneBlack::m_UartSupportedSpeeds[NUM_UART_SPEEDS] = {0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
unsigned long LinxBeagleBoneBlack::m_UartSupportedSpeedsCodes[NUM_UART_SPEEDS] = {B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200};

//SERVO
//None

/****************************************************************************************
**  Constructors /  Destructor
****************************************************************************************/
LinxBeagleBoneBlack::LinxBeagleBoneBlack()
{
	DeviceFamily = 0x06;	//Raspberry Pi Family Code
	DeviceID = 0x01;			//Raspberry Pi 2 Model B
	DeviceNameLen = DEVICE_NAME_LEN;	 
	DeviceName =  m_DeviceName;
 
	//LINX API Version
	LinxApiMajor = 2;
	LinxApiMinor = 1;
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
	AiRefExtMax = 0;
	
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
	NumCanChans = 0;
	CanChans = 0;
	
	//SERVO
	//None
	
	//------------------------------------- ANALOG -------------------------------------
	//Export Dev Tree Overlay For AI If It DNE And Open AI Handles
	bool dtoLoaded = false;
	if(!fileExists("/sys/bus/iio/devices/iio:device0"))
	{
		if(loadDto("BB-ADC"))
		{
			dtoLoaded = true;			
		}
		else
		{
			DebugPrintln("AI Fail - Failed To Load BB-ADC DTO");
		}
	}
	else
	{
		//DTO Already Loaded
		dtoLoaded = true;
	}
	
	if(dtoLoaded)
	{
		//Open AI Handles		
		for(int i=0; i<NUM_AI_CHANS; i++)
		{
			AiValuePaths[m_AiChans[i]] = m_AiValuePaths[i];
			AiValueHandles[m_AiChans[i]] = fopen(m_AiValuePaths[i].c_str(), "r+");
			
			if(AiValueHandles[m_AiChans[i]] <= 0)
			{
				DebugPrintln("AI Fail - Failed Open AI Channel Handle");
			}			
		}
	}
	
	//------------------------------------- DIGITAL -------------------------------------
	//Export GPIO - Set All Digital Handles To NULL
	for(int i=0; i<NUM_DIGITAL_CHANS; i++)
	{
		FILE* digitalExportHandle = fopen("/sys/class/gpio/export", "w");
		fprintf(digitalExportHandle, "%d", m_gpioChan[i]);
		fclose(digitalExportHandle);
		
		DigitalDirHandles[m_DigitalChans[i]] = NULL;
		DigitalValueHandles[m_DigitalChans[i]] = NULL;
		DigitalChannels[m_DigitalChans[i]] = m_gpioChan[i];
	}
	
	//------------------------------------- PWM -------------------------------------
	//Export PWM - Open Freq and Duty Cycle Handles	
	PwmDefaultFrequency = m_PwmDefaultFrequency;
	
	for(int i=0; i< NUM_PWM_CHANS; i++)
	{
		PwmDirPaths[m_PwmChans[i]] = m_PwmDirPaths[i];
		PwmDefaultFrequency = m_PwmDefaultFrequency;
		PwmFrequencies[m_PwmChans[i]] = m_PwmDefaultFrequency;
		
		//Load AM33xx_PWM DTO If No PWM Channels Have Been Exported Since Boot
		if(!fileExists(m_PwmDirPaths[i].c_str(), "period"))
		{
			if(!loadDto("am33xx_pwm"))
			{
				DebugPrintln("PWM Fail - Failed To Load am33xx_pwm DTO");
			}
			else if(!loadDto(m_PwmDtoNames[i].c_str()))
			{
				DebugPrint("PWM Fail - Failed To Load PWM DTO ");
				DebugPrintln(m_PwmDtoNames[i].c_str());
			}
			//Make Sure DTO Has Time To Load Before Opening Handles
			else if(!fileExists(m_PwmDirPaths[i].c_str(), "period", 3000))
			{
				DebugPrint("PWM Fail - PWM DTO Did Not Load Correctly: ");				
				DebugPrintln(m_PwmDirPaths[i].c_str());				
			}
		}
	}
	
	//------------------------------------- I2C -------------------------------------
	//Store I2C Master Paths In Map
	for(int i=0; i<NUM_I2C_CHANS; i++)
	{	
		I2cPaths[I2cChans[i]] = m_I2cPaths[i];
		I2cDtoNames[I2cChans[i]] = m_I2cDtoNames[i];
	}
	
	//------------------------------------- UART ------------------------------------
	for(int i=0; i<NUM_UART_CHANS; i++)
	{
		UartPaths[m_UartChans[i]] = m_UartPaths[i];
		UartHandles[m_UartChans[i]] = 0;
		UartDtoNames[m_UartChans[i]] = m_UartDtoNames[i];
	}
	
	
	//------------------------------------- SPI ------------------------------------
	
	//Load SPI Paths and DTO Names, Configure SPI Master Default Values	
	for(int i=0; i<NUM_SPI_CHANS; i++)
	{
		SpiDtoNames[SpiChans[i]] = m_SpiDtoNames[i];
				
		SpiBitOrders[SpiChans[i]] = MSBFIRST;		//MSB First
		SpiSetSpeeds[SpiChans[i]] = 4000000;
		SpiPaths[SpiChans[i]] = m_SpiPaths[i];
	}
	
	//If Debugging Is Enabled Call EnableDebug()
	#if DEBUG_ENABLED >= 0
		EnableDebug(DEBUG_ENABLED);
	#endif
}

//Destructor
LinxBeagleBoneBlack::~LinxBeagleBoneBlack()
{
	//Close AI Handles
	for(int i=0; i<NUM_AI_CHANS; i++)
	{
		fclose(AiValueHandles[i]);
	}
	
	//Close GPIO Handles
	for(int i=0; i<NUM_DIGITAL_CHANS; i++)
	{
		fclose(DigitalDirHandles[i]);
		fclose(DigitalValueHandles[i]);
	}
	
	//Close I2C Handles
	for(int i=0; i<NUM_I2C_CHANS; i++)
	{
		close(I2cHandles[i]);
	}
	
	//Close PWM Handles
	for(int i=0; i<NUM_PWM_CHANS; i++)
	{
		fclose(PwmPeriodHandles[i]);
		fclose(PwmDutyCycleHandles[i]);
	}
	
	//Close SPI Handles
	for(int i=0; i<NUM_SPI_CHANS; i++)
	{
		close(SpiHandles[i]);
	}
	
	//Close UART Handles
	for(int i=0; i<NUM_UART_CHANS; i++)
	{
		close(UartHandles[i]);
	}
}

/****************************************************************************************
**  Functions
****************************************************************************************/

