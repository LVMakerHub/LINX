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
#include <termios.h>	
#include <map>
#include <string.h>
#include <dirent.h>

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
const unsigned char LinxBeagleBoneBlack::m_AiChans[NUM_AI_CHANS] = {0, 1, 2, 3, 4, 5, 6};
const string LinxBeagleBoneBlack::m_AiValuePaths[NUM_AI_CHANS] = {"/sys/bus/iio/devices/iio:device0/in_voltage0_raw", "/sys/bus/iio/devices/iio:device0/in_voltage1_raw", "/sys/bus/iio/devices/iio:device0/in_voltage2_raw", "/sys/bus/iio/devices/iio:device0/in_voltage3_raw", "/sys/bus/iio/devices/iio:device0/in_voltage4_raw", "/sys/bus/iio/devices/iio:device0/in_voltage5_raw", "/sys/bus/iio/devices/iio:device0/in_voltage6_raw"};
//int LinxBeagleBoneBlack::m_AiHandles[NUM_AI_CHANS];
const unsigned long LinxBeagleBoneBlack::m_AiRefIntVals[NUM_AI_INT_REFS] = {};
const int LinxBeagleBoneBlack::m_AiRefCodes[NUM_AI_INT_REFS] = {};

//AO
//None

//DIGITAL
const unsigned char LinxBeagleBoneBlack::m_DigitalChans[NUM_DIGITAL_CHANS] = { 7, 8, 9, 10, 11, 12, 15, 16, 17, 18, 26,          58, 61, 69, 73, 76};
const unsigned char LinxBeagleBoneBlack::m_gpioChan[NUM_DIGITAL_CHANS] =     {66, 67, 69, 68, 45, 44, 47, 46, 27, 65, 61,      60, 48, 49, 115, 112};

//PWM - Default to 7.x Layout, Updated B
unsigned char LinxBeagleBoneBlack::m_PwmChans[NUM_PWM_CHANS] = {13, 19, 60, 62};
//unsigned char LinxBeagleBoneBlack::m_PwmChips[NUM_PWM_CHANS] = {6, 5, 3, 4};
//unsigned long m_PwmDefaultPeriod = 500000;
//string LinxBeagleBoneBlack::m_PwmExportPaths = "/sys/class/pwm/export";
//string LinxBeagleBoneBlack::m_PwmDirPaths[NUM_PWM_CHANS] = {"/sys/class/pwm/pwm6/", "/sys/class/pwm/pwm5/", "/sys/class/pwm/pwm3/", "/sys/class/pwm/pwm4/"};
//string LinxBeagleBoneBlack::m_PwmDtoNames[NUM_PWM_CHANS] = {"bone_pwm_P8_13", "bone_pwm_P8_19", "bone_pwm_P9_14", "bone_pwm_P9_16"};


//QE
//None

//SPI
string m_SpiPaths[NUM_SPI_CHANS] = { "/dev/spidev1.1"};
string m_SpiDtoNames[NUM_SPI_CHANS] = { "BB-SPIDEV0"};
unsigned char LinxBeagleBoneBlack::m_SpiChans[NUM_SPI_CHANS] = {0};
unsigned long LinxBeagleBoneBlack::m_SpiSupportedSpeeds[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};
int LinxBeagleBoneBlack::m_SpiSpeedCodes[NUM_SPI_SPEEDS] = {7629, 15200, 30500, 61000, 122000, 244000, 488000, 976000, 1953000, 3900000, 7800000, 15600000, 31200000};

//I2C
unsigned char LinxBeagleBoneBlack::m_I2cChans[NUM_I2C_CHANS] = {2};
unsigned char LinxBeagleBoneBlack::m_I2cRefCount[NUM_I2C_CHANS];
string m_I2cPaths[NUM_I2C_CHANS] = {"/dev/i2c-1" };		//Out of order numbering is correct for BBB!!
string m_I2cDtoNames[NUM_I2C_CHANS] = {"BB-I2C2" };

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
	DeviceFamily = 0x06;	//TI Family Code
	DeviceId = 0x01;			//BeagleBone Black
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
	
	AiChans = m_AiChans;
	AiResolution = AI_RES_BITS;
	
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
	AoResolution = 0;
    AoRefDefault = 0;
	AoRefSet = 0;
	
	//PWM
	//Shared Non Varying Components
	
	unsigned long m_PwmDefaultPeriod = 500000;	
	string m_PolarityFileName = "polarity";
	
	//7.x Only
	string m_PwmDtoNames[NUM_PWM_CHANS] = {"bone_pwm_P8_13", "bone_pwm_P8_19", "bone_pwm_P9_14", "bone_pwm_P9_16"};
		
	//8.x Only
	string m_PwmMuxPaths[NUM_PWM_CHANS] = {"/sys/devices/platform/ocp/ocp:P8_13_pinmux/state", "/sys/devices/platform/ocp/ocp:P8_19_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_14_pinmux/state", "/sys/devices/platform/ocp/ocp:P9_16_pinmux/state"};
	
	//Shared, Varying Components - Default To 7.x
	string m_PwmExportPaths[NUM_PWM_CHANS] = {"/sys/class/pwm/export", "/sys/class/pwm/export", "/sys/class/pwm/export", "/sys/class/pwm/export"};
	unsigned char m_PwmExportVal[NUM_PWM_CHANS] = {6, 5, 3, 4};
	string m_PwmDirPaths[NUM_PWM_CHANS] = {"/sys/class/pwm/pwm6/", "/sys/class/pwm/pwm5/", "/sys/class/pwm/pwm3/", "/sys/class/pwm/pwm4/"};
	string m_DutyCycleFileName = "duty_ns";
	string m_PeriodFileName = "period_ns";
	m_EnableFileName = "run";


	//Update to 8.x layout if necessary
	if(FilePathLayout == 8)
	{
		string pwmBasePath = "/sys/class/pwm/";
		
		string PwmP8ChipDir = "";	
		string PwmP9ChipDir = "";
		
		DIR* pwmDirHandle = opendir(pwmBasePath.c_str());

		dirent* dp;

		//Loop Over All Dirs In PWM Base Dir
		while(pwmDirHandle)
		{
			if ((dp = readdir(pwmDirHandle)) != NULL)
			{
				//Make Sure Dir Is Not . or ..
				if((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0))
				{
					//PWM Chip Dir, Check Where Symlink Points
					char pwmChipSymlink[128] ;
					char pwmChipSymlinkTarget[128] ;
					
					sprintf(pwmChipSymlink, "%s%s", pwmBasePath.c_str(), dp->d_name);
					readlink(pwmChipSymlink, pwmChipSymlinkTarget, 128);
					
					//Parse DTO Address
					//fprintf(stdout, pwmChipSymlinkTarget);	//Debug
					//fprintf(stdout, "\n");	//Debug
					char* token = strtok(pwmChipSymlinkTarget, "\\/");
					while(token != NULL)
					{
						unsigned int val = 0;
						sscanf(token, "%u", &val);
						//printf("%u\n", val);
						if(val == 48304200)
						{
							PwmP8ChipDir = pwmChipSymlink;
						}
						else if(val == 48302200)
						{
							PwmP9ChipDir = pwmChipSymlink;
						}
						token = strtok(NULL, "\\/");
					}
				}
			}
			else
			{
				closedir(pwmDirHandle);
				break;
			}
		}
				
		//Build  PWM Paths
		m_PwmExportVal[0] = 1;
		m_PwmExportVal[1] = 0;
		m_PwmExportVal[2] = 0;
		m_PwmExportVal[3] = 1;
		
		m_PwmDirPaths[0] = PwmP8ChipDir + "/pwm1/";
		m_PwmDirPaths[1] = PwmP8ChipDir + "/pwm0/";
		m_PwmDirPaths[2] = PwmP9ChipDir + "/pwm0/";
		m_PwmDirPaths[3] = PwmP9ChipDir + "/pwm1/";
		
		m_PwmExportPaths[0] = PwmP8ChipDir + "/export";
		m_PwmExportPaths[1] = PwmP8ChipDir + "/export";
		m_PwmExportPaths[2] = PwmP9ChipDir + "/export";
		m_PwmExportPaths[3] = PwmP9ChipDir + "/export";
		
		m_DutyCycleFileName = "duty_cycle";
		m_PeriodFileName = "period";
		m_EnableFileName = "enable";
	}
	
	PwmDutyCycleFileName = m_DutyCycleFileName;
	PwmPeriodFileName = m_PeriodFileName;
	PwmEnableFileName = m_EnableFileName;
		
	NumPwmChans = NUM_PWM_CHANS;
	PwmChans = m_PwmChans;
	PwmDefaultPeriod = m_PwmDefaultPeriod;
		
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
	NumServoChans = 0;
	
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
		if(digitalExportHandle != NULL)
		{
			fprintf(digitalExportHandle, "%d", m_gpioChan[i]);
			fclose(digitalExportHandle);
		}
		
		DigitalDirHandles[m_DigitalChans[i]] = NULL;
		DigitalValueHandles[m_DigitalChans[i]] = NULL;
		DigitalChannels[m_DigitalChans[i]] = m_gpioChan[i];
	}
	
	//------------------------------------- PWM -------------------------------------
	
	//PWM General Initialization
	if(FilePathLayout == 7)
	{
		//Load AM33xx_PWM DTO If No PWM Channels Have Been Exported Since Boot
		if(!fileExists(m_PwmDirPaths[NUM_PWM_CHANS-1].c_str(), m_PeriodFileName.c_str()))
		{
			if(!loadDto("am33xx_pwm"))
			{
				DebugPrintln("PWM Fail - Failed To Load am33xx_pwm DTO");
			}
			
			//Export PWM Channels Before Loading Channel Specific DTOs Below
			for(int i=0; i< NUM_PWM_CHANS; i++)
			{
				//Export PWM Channels
				if(!fileExists(m_PwmDirPaths[i].c_str(), m_PeriodFileName.c_str()))
				{
					FILE* pwmExportHandle = fopen(m_PwmExportPaths[i].c_str(), "w");
					if(pwmExportHandle != NULL)
					{
						fprintf(pwmExportHandle, "%u", m_PwmExportVal[i]);
						fclose(pwmExportHandle);
					}
					else
					{
						DebugPrintln("PWM Fail - Unable to open pwmExportHandle");
					}

					//Set Default Period Only First Time
					char periodPath[64];
					sprintf(periodPath, "%s%s", m_PwmDirPaths[i].c_str(), m_PeriodFileName.c_str());			
					
					FILE* pwmPeriodleHandle = fopen(periodPath, "r+w+");
					if(pwmPeriodleHandle != NULL)
					{
						fprintf(pwmPeriodleHandle, "%lu", m_PwmDefaultPeriod);
						fclose(pwmPeriodleHandle);							
					}
					else
					{
						DebugPrintln("PWM Fail - Unable to open pwmPeriodHandle");
					}
				}
			}
		}
	}
	else if(FilePathLayout == 8)
	{
		//Set Mux to PWM
		for(int i=0; i<NUM_PWM_CHANS; i++)
		{
			FILE* pwmMuxHandle = fopen(m_PwmMuxPaths[i].c_str(), "r+w+");
			if(pwmMuxHandle != NULL)
			{
				fprintf(pwmMuxHandle, "pwm");
				fclose(pwmMuxHandle);							
			}
		}
	}
	
	//For Each Pin Init
	for(int i=0; i<NUM_PWM_CHANS; i++)
	{
		//Store Default Values
		PwmDirPaths[m_PwmChans[i]] = m_PwmDirPaths[i];
		PwmPeriods[m_PwmChans[i]] = m_PwmDefaultPeriod;
		
		//7.x Per Pin Init
		if(FilePathLayout == 7)
		{
			//Load Chip Specific PWM DTO If Not Already Loaded
			if(!loadDto(m_PwmDtoNames[i].c_str()))
			{
				DebugPrint("PWM Fail - Failed To Load PWM DTO ");
				DebugPrintln(m_PwmDtoNames[i].c_str());
			}
			
			//Make Sure DTO Has Time To Load Before Opening Handles
			else if(!fileExists(m_PwmDirPaths[i].c_str(), "period_ns", 3000))
			{
				DebugPrint("PWM Fail - PWM DTO Did Not Load Correctly: ");				
				DebugPrintln(m_PwmDirPaths[i].c_str());				
			}		
		}
		//Export PWM Chans.  If 7.x layout this is done above.  This should probably be moved.
		if(FilePathLayout == 8)
		{
			//Export PWM Channels
			if(!fileExists(m_PwmDirPaths[i].c_str(), m_PeriodFileName.c_str()))
			{
				FILE* pwmExportHandle = fopen(m_PwmExportPaths[i].c_str(), "w");
				if(pwmExportHandle != NULL)
				{
					fprintf(pwmExportHandle, "%u", m_PwmExportVal[i]);
					fclose(pwmExportHandle);
				}
				else
				{
					DebugPrintln("PWM Fail - Unable to open pwmExportHandle");
				}

				//Set Default Period Only First Time
				char periodPath[64];
				sprintf(periodPath, "%s%s", m_PwmDirPaths[i].c_str(), m_PeriodFileName.c_str());
				
				FILE* pwmPeriodleHandle = fopen(periodPath, "r+w+");
				if(pwmPeriodleHandle != NULL)
				{
					fprintf(pwmPeriodleHandle, "%lu", m_PwmDefaultPeriod);
					fclose(pwmPeriodleHandle);							
				}
				else
				{
					DebugPrintln("PWM Fail - Unable to open pwmPeriodHandle");
				}
			}
		}
		
		//Set Polarity To 0 So PWM Value Corresponds To 'Percent On' Rather Than 'Percent Off'
		char polarityPath[64];
		sprintf(polarityPath, "%s%s", PwmDirPaths[m_PwmChans[i]].c_str(), m_PolarityFileName.c_str());
		
		FILE* pwmPolarityHandle = fopen(polarityPath, "w");
		if(pwmPolarityHandle != NULL)
		{
			fprintf(pwmPolarityHandle, "0");
			fclose(pwmPolarityHandle);
		}
		else
		{
			DebugPrint("PWM Fail - Unable to open pwmPolarityHandle");				
		}
			
		//Set Default Duty Cycle To 0	
		char dutyCyclePath[64];
		sprintf(dutyCyclePath, "%s%s", PwmDirPaths[m_PwmChans[i]].c_str(), m_DutyCycleFileName.c_str());			
		
		FILE* pwmDutyCycleHandle = fopen(dutyCyclePath, "r+w+");
		if(pwmDutyCycleHandle != NULL)
		{
			fprintf(pwmDutyCycleHandle, "0");
			fclose(pwmDutyCycleHandle);
		}
		else
		{
			DebugPrint("PWM Fail - Unable to open pwmDutyCycleHandle");				
		}		
		
		//Turn On PWM		
		char enablePath[64];
		sprintf(enablePath, "%s%s", PwmDirPaths[m_PwmChans[i]].c_str(), m_EnableFileName.c_str());			
		FILE* pwmEnableHandle = fopen(enablePath, "r+w+");
		if(pwmEnableHandle != NULL)
		{
			fprintf(pwmEnableHandle, "1");
			fclose(pwmEnableHandle);		
		}
		else
		{
			DebugPrint("PWM Fail - Unable to open pwmEnableHandle");				
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
		if(AiValueHandles[m_AiChans[i]] != 0)
		{
			fclose(AiValueHandles[m_AiChans[i]]);
		}
	}
	
	//Close GPIO Handles If Open
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
	
	//Close I2C Handles If Open
	for(int i=0; i<NUM_I2C_CHANS; i++)
	{
		if(I2cHandles[m_I2cChans[i]] != 0)
		{	
			close(I2cHandles[m_I2cChans[i]]);
		}
	}
	
	//Close PWM Handles If Open
	for(int i=0; i<NUM_PWM_CHANS; i++)
	{
		if(PwmPeriodHandles[m_PwmChans[i]] != NULL)
		{
			fclose(PwmPeriodHandles[m_PwmChans[i]]);
		}
		if(PwmDutyCycleHandles[m_PwmChans[i]] != NULL)
		{
			fprintf(PwmDutyCycleHandles[m_PwmChans[i]], "0");
			fclose(PwmDutyCycleHandles[m_PwmChans[i]]);
		}
		
		//Turn Off PWM		
		char enablePath[64];
		sprintf(enablePath, "%s%s", m_PwmDirPaths[i].c_str(), m_EnableFileName.c_str());
		FILE* pwmEnableHandle = fopen(enablePath, "r+w+");
		if(pwmEnableHandle != NULL)
		{
			fprintf(pwmEnableHandle, "0");
			fclose(pwmEnableHandle);
		}
		else
		{
			DebugPrint("PWM Fail - Unable to open pwmEnableHandle");
		}
	}
	
	
	//Close SPI Handles If Open
	for(int i=0; i<NUM_SPI_CHANS; i++)
	{
		if(SpiHandles[m_SpiChans[i]] != 0)
		{	
			close(SpiHandles[m_SpiChans[i]]);
		}
	}
	
	//Close UART Handles If Open
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

