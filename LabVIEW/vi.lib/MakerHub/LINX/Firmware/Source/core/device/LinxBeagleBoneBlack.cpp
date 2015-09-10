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

#include "utility/LinxDevice.h"
#include "utility/LinxBeagleBone.h"
#include "LinxBeagleBoneBlack.h"

/****************************************************************************************
**  Member Variables
****************************************************************************************/
//System
const unsigned char LinxBeagleBoneBlack::m_DeviceName[DEVICE_NAME_LEN] = "BeagleBone Black";

//AI
const unsigned char LinxBeagleBoneBlack::m_AiChans[NUM_AI_CHANS] = {0, 1, 2, 3, 4, 5, 6, 7};
const char LinxBeagleBoneBlack::m_AiPaths[NUM_AI_CHANS][AI_PATH_LEN] = {"/sys/bus/iio/devices/iio:device0/in_voltage0_raw", "/sys/bus/iio/devices/iio:device0/in_voltage1_raw", "/sys/bus/iio/devices/iio:device0/in_voltage2_raw", "/sys/bus/iio/devices/iio:device0/in_voltage3_raw", "/sys/bus/iio/devices/iio:device0/in_voltage4_raw", "/sys/bus/iio/devices/iio:device0/in_voltage5_raw", "/sys/bus/iio/devices/iio:device0/in_voltage6_raw", "/sys/bus/iio/devices/iio:device0/in_voltage7_raw"};
int LinxBeagleBoneBlack::m_AiHandles[NUM_AI_CHANS];
const unsigned long LinxBeagleBoneBlack::m_AiRefIntVals[NUM_AI_INT_REFS] = {};
const int LinxBeagleBoneBlack::m_AiRefCodes[NUM_AI_INT_REFS] = {};

//AO
//None

//DIGITAL
const unsigned char LinxBeagleBoneBlack::m_DigitalChans[NUM_DIGITAL_CHANS] = {2, 3, 4, 5, 7, 8, 9, 10, 11, 14, 15, 20, 22, 23, 26, 27, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 44, 45, 46, 47, 48, 49, 50, 51, 60, 61, 62, 63, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 86, 87, 88, 89, 110, 111, 112, 113, 115, 117};

//PWM
const unsigned char LinxBeagleBoneBlack::m_PwmChans[NUM_PWM_CHANS] = {13, 19, 60, 62};
const char LinxBeagleBoneBlack::m_PwmDirPaths[NUM_PWM_CHANS][PWM_PATH_LEN] = {"/sys/devices/ocp.3/pwm_test_P8_13.16/", "/sys/devices/ocp.3/pwm_test_P8_19.17/", "/sys/devices/ocp.3/pwm_test_P9_14.18/", "/sys/devices/ocp.3/pwm_test_P9_16.19/"};
const char LinxBeagleBoneBlack::m_PwmDtoNames[NUM_PWM_CHANS][PWM_DTO_NAME_LEN] = {"bone_pwm_P8_13", "bone_pwm_P8_19", "bone_pwm_P9_14", "bone_pwm_P9_16"};

//QE
//None

//SPI
const unsigned char LinxBeagleBoneBlack::m_SpiChans[NUM_SPI_CHANS] = {0, 1};
int  LinxBeagleBoneBlack::m_SpiHandles[NUM_SPI_CHANS];
//const char LinxBeagleBoneBlack::m_SpiPaths[NUM_SPI_CHANS][SPI_PATH_LEN] = { "/dev/spidev0.0\00"};	//TODO ADD SECOND CHANNEL
unsigned long LinxBeagleBoneBlack::m_SpiSupportedSpeeds[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};
int LinxBeagleBoneBlack::m_SpiSpeedCodes[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};

//I2C
unsigned char LinxBeagleBoneBlack::m_I2cChans[NUM_I2C_CHANS] = {1, 2};
int LinxBeagleBoneBlack::m_I2cHandles[NUM_I2C_CHANS];
const char LinxBeagleBoneBlack::m_I2cPaths[NUM_I2C_CHANS][I2C_PATH_LEN] = {"/dev/i2c-0\00", "/dev/i2c-2\00", "/dev/i2c-1\00" };		//Out of order numbering is correct for BBB!!
unsigned char LinxBeagleBoneBlack::m_I2cRefCount[NUM_I2C_CHANS];			

//UART
unsigned char LinxBeagleBoneBlack::m_UartChans[NUM_UART_CHANS] = {0, 1, 2, 3, 4, 5};
int LinxBeagleBoneBlack::m_UartHandles[NUM_UART_CHANS];
const char LinxBeagleBoneBlack::m_UartPaths[NUM_UART_CHANS][UART_PATH_LEN] = { "/dev/ttyO0\00", "/dev/ttyO1\00", "/dev/ttyO2\00", "/dev/ttyO3\00", "/dev/ttyO4\00", "/dev/ttyO5\00"};
unsigned long LinxBeagleBoneBlack::m_UartSupportedSpeeds[NUM_UART_SPEEDS] = {0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400};
unsigned long LinxBeagleBoneBlack::m_UartSupportedSpeedsCodes[NUM_UART_SPEEDS] = {B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400};

//SERVO
//Servo* LinxBeagleBoneBlack::m_Servos[NUM_SERVO_CHANS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};			//Initialize To Null Pointers

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
	LinxApiMinor = 0;
	LinxApiSubminor = 0; 
	
	//DIGITAL
	NumDigitalChans = NUM_DIGITAL_CHANS;			
	DigitalChans = m_DigitalChans;
		
	//AI
	NumAiChans = NUM_AI_CHANS;
	AiPaths = m_AiPaths;
	AiHandles = m_AiHandles;
	
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
	PwmDirPaths = m_PwmDirPaths;
	PwmDtoNames = m_PwmDtoNames;
		
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
	UartPaths = m_UartPaths;
	UartHandles = m_UartHandles;

	//I2C
	NumI2cChans = NUM_I2C_CHANS;	
	I2cChans = m_I2cChans;
	I2cRefCount = m_I2cRefCount;
	I2cPaths = m_I2cPaths;
	I2cHandles = m_I2cHandles;
		
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
	//NumServoChans = NUM_SERVO_CHANS;	
	//ServoChans = m_DigitalChans;
	//Servos = m_Servos;
	
	//If Debuging Is Enabled Call EnableDebug()
	#ifdef DEBUG_ENABLED
		EnableDebug(DEBUG_ENABLED);
	#endif
	
	//---------------------------- ANALOG ----------------------------
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
			int handle = open(AiPaths[i], O_RDWR);
			if(handle < 0)
			{
				DebugPrintln("AI Fail - Failed Open AI Channel Handle");
			}
			else
			{
				AiHandles[i] = handle;
			}
		}
	}
	
	//---------------------------- DIGITAL ----------------------------
	//Export GPIO - Set All Digital Handles To NULL
	for(int i=0; i<NUM_DIGITAL_CHANS; i++)
	{
		FILE* digitalExportHandle = fopen("/sys/class/gpio/export", "w");
		fprintf(digitalExportHandle, "%d", DigitalChans[i]);
		fclose(digitalExportHandle);
		
		DigitalDirHandles[DigitalChans[i]] = NULL;
		DigitalValueHandles[DigitalChans[i]] = NULL;
	}
	
	//---------------------------- PWM ----------------------------
	//Export PWM - Open Freq and Duty Cycle Handles
	
	//Load AM33xx_PWM DTO If No PWM Channels Have Been Exported Since Boot
	if(!fileExists(PwmDirPaths[0], "period"))
	{
		loadDto("am33xx_pwm");
	}
		
	for(int i=0; i< NUM_PWM_CHANS; i++)
	{
		//Load DTO If Necessary
		if(!fileExists(PwmDirPaths[i], "period"))
		{
			DebugPrintln("Loading PWM DTO");
			if(!loadDto(PwmDtoNames[i]))
			{
				DebugPrint("PWM Fail - Failed To Load PWM DTO ");
				DebugPrintln(PwmDtoNames[i]);
			}
			else
			{
				//Wait for DTO to Load
				fileExists(PwmDirPaths[0], "period", 3000); 
				
				//DTO Successfully Loaded, Open PWM Handles
				char path[128];
				sprintf(path, "%s%s", PwmDirPaths[i], "period");
				DebugPrint("Opening ");
				DebugPrintln(path);
				PwmPeriodHandles[PwmChans[i]] = fopen(path, "r+w+");
				DebugPrintln("Period Handle Open... ");
				
				sprintf(path, "%s%s", PwmDirPaths[i], "duty");
				PwmDutyCycleHandles[PwmChans[i]] = fopen(path, "r+w+");
				DebugPrintln("Period Duty Cylce Handle Open... ");
			}
		}
		else
		{
			DebugPrintln("PWM DTO Already Loaded");
			//DTO Was Already Loaded, Open PWM Handles
			char path[128];
			sprintf(path, "%s%s", PwmDirPaths[i], "period");
			PwmPeriodHandles[PwmChans[i]] = fopen(path, "r+w+");
			
			sprintf(path, "%s%s", PwmDirPaths[i], "duty");
			PwmDutyCycleHandles[PwmChans[i]] = fopen(path, "r+w+");
		}
	
		//Set Defaults
		FILE* handle;
		char path[128];
		
		sprintf(path, "%s%s", PwmDirPaths[i], "polarity");
		handle = fopen(path, "r+w+");
		fprintf(handle, "0");		//Set To Non Inverting (ie Duty Cycle = % On)
		fclose(handle);
			
		sprintf(path, "%s%s", PwmDirPaths[i], "duty");
		handle = fopen(path, "r+w+");
		fprintf(handle, "0");		//Turn PWM Output Off
		fclose(handle);
	}
	
}

//Destructor
LinxBeagleBoneBlack::~LinxBeagleBoneBlack()
{
	//Close AI Handles
	for(int i=0; i<NUM_AI_CHANS; i++)
	{
		close(AiHandles[i]);
	}
	
	//Close GPIO Handles
	for(int i=0; i<NUM_DIGITAL_CHANS; i++)
	{
		fclose(DigitalDirHandles[i]);
		fclose(DigitalValueHandles[i]);
	}	
}

/****************************************************************************************
**  Functions
****************************************************************************************/

