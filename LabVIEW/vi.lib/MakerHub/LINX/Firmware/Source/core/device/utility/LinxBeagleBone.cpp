/****************************************************************************************
**  LINX - BeagleBone code
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
** 
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Defines
****************************************************************************************/		

/****************************************************************************************
**  Includes
****************************************************************************************/	
#include "LinxDevice.h"
#include "LinxBeagleBone.h"

#include <map>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <termios.h>	
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

using namespace std;

/****************************************************************************************
**  Variables
****************************************************************************************/		
//string DtoSlotsPath = "/sys/devices/bone_capemgr.9/slots";
//char* overlaySlotsPath = "/sys/devices/bone_capemgr.9/slots";

/****************************************************************************************
**  Constructors / Destructors 
****************************************************************************************/
LinxBeagleBone::LinxBeagleBone()
{
	//LINX API Version
	LinxApiMajor = 2;
	LinxApiMinor = 0;
	LinxApiSubminor = 0;
	
	//Check file system layout
	if(fileExists("/sys/devices/bone_capemgr.9/slots"))
	{
		//7.x Layout
		FilePathLayout = 7;
		DtoSlotsPath = "/sys/devices/bone_capemgr.9/slots";
	}
	else if(fileExists("/sys/devices/platform/bone_capemgr/slots"))
	{
		//8.x Layout
		FilePathLayout = 8;
		DtoSlotsPath = "/sys/devices/platform/bone_capemgr/slots";
	}
	else
	{
		//Unknown Layout		
		FilePathLayout = 0;
	}
	
	// TODO Load User Config Data From Non Volatile Storage
	//userId = NonVolatileRead(NVS_USERID) << 8 | NonVolatileRead(NVS_USERID + 1);
}

LinxBeagleBone::~LinxBeagleBone()
{

}
/****************************************************************************************
**  Private Functions
****************************************************************************************/

//Open Direction And Value Handles If They Are Not Already Open And Set Direction
int LinxBeagleBone::digitalSmartOpen(unsigned char numChans, unsigned char* channels)
{
	for(int i=0; i<numChans; i++)
	{		
		//Open Direction Handle If It Is Not Already		
		if(DigitalDirHandles[channels[i]] == NULL)
		{
			DebugPrint("Opening Digital Direction Handle For LINX DIO ");
			DebugPrint(channels[i], DEC);
			DebugPrint("(GPIO ");
			DebugPrint(DigitalChannels[channels[i]], DEC);
			DebugPrintln(")");
			
			char dirPath[64];
			sprintf(dirPath, "/sys/class/gpio/gpio%d/direction", DigitalChannels[channels[i]]);
			DigitalDirHandles[channels[i]] = fopen(dirPath, "r+w+");
			
			if(DigitalDirHandles[channels[i]] == NULL)
			{
				DebugPrintln("Digital Fail - Unable To Open Direction File Handles");
				return L_UNKNOWN_ERROR;
			}
		}
		
		//Open Value Handle If It Is Not Already		
		if(DigitalValueHandles[channels[i]] == NULL)
		{
			DebugPrintln("Opening Digital Value Handle");
			char valuePath[64];
			sprintf(valuePath, "/sys/class/gpio/gpio%d/value", DigitalChannels[channels[i]]);
			DigitalValueHandles[channels[i]] = fopen(valuePath, "r+w+");
			
			if(DigitalValueHandles[channels[i]] == NULL)
			{
				DebugPrintln("Digital Fail - Unable To Open Value File Handles");
				return L_UNKNOWN_ERROR;
			}
		}
	}
	return L_OK;
}

//Open Direction And Value Handles If They Are Not Already Open And Set Direction
int LinxBeagleBone::pwmSmartOpen(unsigned char numChans, unsigned char* channels)
{
	for(int i=0; i<numChans; i++)
	{		
		//Open Period Handle If It Is Not Already		
		if(PwmPeriodHandles[channels[i]] == NULL)
		{
			char periodPath[64];
			sprintf(periodPath, "%s%s", PwmDirPaths[channels[i]].c_str(), PwmPeriodFileName.c_str());
			DebugPrint("Opening ");
			DebugPrintln(periodPath);
			PwmPeriodHandles[channels[i]] = fopen(periodPath, "r+w+");
			
			//Initialize PWM Period
			fprintf(PwmPeriodHandles[channels[i]], "%lu", PwmDefaultPeriod);
			PwmPeriods[channels[i]] = PwmDefaultPeriod;
			fflush(PwmPeriodHandles[channels[i]]);
		}		
		
		//Open Duty Cycle Handle If It Is Not Already		
		if(PwmDutyCycleHandles[channels[i]] == NULL)
		{
			char dutyCyclePath[64];
			sprintf(dutyCyclePath, "%s%s", PwmDirPaths[channels[i]].c_str(), PwmDutyCycleFileName.c_str());
			DebugPrint("Opening ");
			DebugPrintln(dutyCyclePath);
			PwmDutyCycleHandles[channels[i]] = fopen(dutyCyclePath, "r+w+");
		}
	}
	return L_OK;		
}

//Return True If File Specified By path Exists.
bool LinxBeagleBone::fileExists(const char* path)
{
	struct stat buffer;   
	return (stat(path, &buffer) == 0); 
}

bool LinxBeagleBone::fileExists(const char* directory, const char* fileName)
{
	char fullPath[128];
	sprintf(fullPath, "%s%s", directory, fileName);				
	struct stat buffer;
	return (stat(fullPath, &buffer) == 0);  
}

bool LinxBeagleBone::fileExists(const char* directory, const char* fileName, unsigned long timeout)
{
	char fullPath[128];
	sprintf(fullPath, "%s%s", directory, fileName);				
	struct stat buffer;
	
	unsigned long startTime = GetMilliSeconds();
	while(GetMilliSeconds()-startTime < timeout)
	{
		if(stat(fullPath, &buffer) == 0)
		{
			DebugPrint("DTO Took ");
			DebugPrintln(GetMilliSeconds()-startTime < timeout, DEC);
			return true;
		}
	}
	DebugPrintln("Timeout");
	return false;
}

//Load Device Tree Overlay
bool LinxBeagleBone::loadDto(const char* dtoName)
{	
	FILE* slotsHandle = fopen(DtoSlotsPath.c_str(), "r+w+");
	if(slotsHandle != NULL)
	{
		fprintf(slotsHandle, "%s", dtoName);
		fclose(slotsHandle);
		return true;
	}
	else
	{
		DebugPrintln("Unable To Open slotsHandle");
	}
	
	return false;	
}


/****************************************************************************************
**  Public Functions
****************************************************************************************/
	
//--------------------------------------------------------ANALOG-------------------------------------------------------
int LinxBeagleBone::AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{

	//unsigned int analogValue = 0;
	unsigned char responseByteOffset = 0;
	unsigned char responseBitsRemaining = 8; 
	unsigned char dataBitsRemaining = AiResolution;
	fstream fs;	//AI File Handle
	
	values[responseByteOffset] = 0x00;    //Clear First	Response Byte   

	//Loop Over All AI channels In Command Packet
	for(int i=0; i<numChans; i++)
	{
		//Acquire AI Sample
		int aiVal = 0;
		AiValueHandles[channels[i]] = freopen(AiValuePaths[i].c_str(), "r+", AiValueHandles[channels[i]]);
		fscanf(AiValueHandles[channels[i]], "%u", &aiVal);
		
		/*
		fs.open(AiPaths[channels[i]], fstream::in);
		fs >> analogValue;
		fs.close();
		*/
		
		dataBitsRemaining = AiResolution;

		//Byte Packet AI Values In Response Packet
		while(dataBitsRemaining > 0)
		{
			*(values+responseByteOffset) |= ( ((unsigned int)aiVal>>(AiResolution - dataBitsRemaining)) << (8 - responseBitsRemaining) );
			//*(values+responseByteOffset) = 69;

			if(responseBitsRemaining > dataBitsRemaining)
			{
				//Current Byte Still Has Empty Bits
				responseBitsRemaining -= dataBitsRemaining;
				dataBitsRemaining = 0;
			}
			else
			{
				//Current Byte Full
				dataBitsRemaining = dataBitsRemaining - responseBitsRemaining;
				responseByteOffset++;
				responseBitsRemaining = 8;
				values[responseByteOffset] = 0x00;    //Clear Next Response Byte     
			}
		}
	}
	
	return L_OK;
}

int LinxBeagleBone::AnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned long* values)
{
	//Loop Over All AI channels In Command Packet
	for(int i=0; i<numChans; i++)
	{
		AiValueHandles[channels[i]] = freopen(AiValuePaths[i].c_str(), "r+", AiValueHandles[channels[i]]);
		fscanf(AiValueHandles[channels[i]], "%lu", values+i);
	}
	
	return L_OK;
}

int LinxBeagleBone::AnalogSetRef(unsigned char mode, unsigned long voltage)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

//--------------------------------------------------------DIGITAL-------------------------------------------------------
int LinxBeagleBone::DigitalSetDirection(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	if(digitalSmartOpen(numChans, channels) != L_OK)
	{
		DebugPrintln("Smart Open Failed");
		return L_UNKNOWN_ERROR;			
	}
	
	//Set Direction Only If Necessary
	//Avoid Divide By Zero By Doing First Iteration
	if( (values[0] & 0x01) == OUTPUT && DigitalDirs[channels[0]] != OUTPUT)
	{
		//Set As Output
		fprintf(DigitalDirHandles[channels[0]], "out");		
		fflush(DigitalDirHandles[channels[0]]);
		DigitalDirs[channels[0]] = OUTPUT;		
	}
	else if((values[0] & 0x01) == INPUT && DigitalDirs[channels[0]] != INPUT)
	{
		//Set As Input
		fprintf(DigitalDirHandles[channels[0]], "in");	
		fflush(DigitalDirHandles[channels[0]]);
		DigitalDirs[channels[0]] = INPUT;
	}
		
	//Set Directions
	for(int i=1; i<numChans; i++)
	{		
		if( (values[i] & 0x01) == OUTPUT && DigitalDirs[channels[i]] != OUTPUT)
		{
			//Set As Output
			fprintf(DigitalDirHandles[channels[i]], "out");		
			fflush(DigitalDirHandles[channels[i]]);
			DigitalDirs[channels[i]] = OUTPUT;				
		}
		else if( (values[i] & 0x01) == INPUT && DigitalDirs[channels[i]] != INPUT)
		{
			//Set As Input
			fprintf(DigitalDirHandles[channels[i]], "in");	
			fflush(DigitalDirHandles[channels[i]]);
			DigitalDirs[channels[i]] = INPUT;
		}
	}
	
	return L_OK;
}

int LinxBeagleBone::DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	//Generate Bit Packed Output Direction Array
	int numDirBytes = (int)ceil(numChans/8.0);		
	unsigned char directions[numDirBytes];
	for(int i=0; i< numDirBytes; i++)
	{
		directions[i] = 0xFF;
	}	
	
	if(DigitalSetDirection(numChans, channels, directions) != L_OK)
	{
		DebugPrintln("Digital Write Fail - Set Direction Failed");
	}
			
	for(int i=0; i<numChans; i++)
	{
		//Set Value
		if( ((values[i/8] >> i%8) & 0x01) == LOW)
		{
			fprintf(DigitalValueHandles[channels[i]], "0");
			fflush(DigitalValueHandles[channels[i]]);
		}
		else
		{
			fprintf(DigitalValueHandles[channels[i]], "1");
			fflush(DigitalValueHandles[channels[i]]);
		}
	}
		
	return L_OK;
}

int LinxBeagleBone::DigitalWrite(unsigned char channel, unsigned char value)
{
	return DigitalWrite(1, &channel, &value);
}

int LinxBeagleBone::DigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	//Generate Directions Array (waste some memory, save some CPU)
	unsigned char directions[numChans];
	
	for(int i=0; i< numChans; i++)
	{
		directions[i] = 0xFF;
	}
	
	if(DigitalSetDirection(numChans, channels, directions) != L_OK)
	{
		DebugPrintln("Digital Write Fail - Set Direction Failed");
	}
			
	for(int i=0; i<numChans; i++)
	{
		//Set Value
		if(values[i] == LOW)
		{
			fprintf(DigitalValueHandles[channels[i]], "0");
			fflush(DigitalValueHandles[channels[i]]);
		}
		else
		{
			fprintf(DigitalValueHandles[channels[i]], "1");
			fflush(DigitalValueHandles[channels[i]]);
		}
	}
		
	return L_OK;
}

int LinxBeagleBone::DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	//Generate Bit Packed Input Direction Array
	int numDirBytes = (int)ceil(numChans/8.0);		
	unsigned char directions[numDirBytes];
	for(int i=0; i< numDirBytes; i++)
	{
		directions[i] = 0x00;
	}
	
	//Set Directions To Inputs		
	if(DigitalSetDirection(numChans, channels, directions) != L_OK)
	{
		DebugPrintln("Digital Write Fail - Set Direction Failed");
	}
	
	unsigned char bitOffset = 8;
	unsigned char byteOffset = 0;
	unsigned char retVal = 0;
	int diVal = 0;
	
	//Loop Over channels To Read
	for(int i=0; i<numChans; i++)
	{
		
		//If bitOffset Is 0 We Have To Start A New Byte, Store Old Byte And Increment OFfsets
		if(bitOffset == 0)
		{
			//Insert retVal Into Response Buffer
			values[byteOffset] = retVal;
			retVal = 0x00;
			byteOffset++;
			bitOffset = 7;      
		}
		else
		{
			bitOffset--;
		}
		
		//Reopen Value Handle
		char valPath[64];
		sprintf(valPath, "/sys/class/gpio/gpio%d/value", DigitalChannels[channels[i]]);
		DigitalValueHandles[channels[i]] = freopen(valPath, "r+w+", DigitalValueHandles[channels[i]]);
		
		//Read From Next Pin
		fscanf(DigitalValueHandles[channels[i]], "%u", &diVal);
		
		//DebugPrint("Value = ");
		//DebugPrintln((unsigned char)diVal, DEC);
		
					
		retVal = retVal | ((unsigned char)diVal << bitOffset);	//Read Pin And Insert Value Into retVal
	}

	//Store Last Byte
	values[byteOffset] = retVal;
		
	return L_OK;
}

int LinxBeagleBone::DigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	//Generate Directions Array (waste some memory, save some CPU)
	unsigned char directions[numChans];
	
	for(int i=0; i< numChans; i++)
	{
		directions[i] = 0x00;
	}
	
	//Set Directions To Inputs		
	if(DigitalSetDirection(numChans, channels, directions) != L_OK)
	{
		DebugPrintln("Digital Write Fail - Set Direction Failed");
	}
	
	//Loop Over channels To Read
	for(int i=0; i<numChans; i++)
	{
		//Reopen Value Handle
		char valPath[64];
		sprintf(valPath, "/sys/class/gpio/gpio%d/value", DigitalChannels[channels[i]]);
		DigitalValueHandles[channels[i]] = freopen(valPath, "r+w+", DigitalValueHandles[channels[i]]);
		
		//Read From Next Pin
		fscanf(DigitalValueHandles[channels[i]], "%hhu", values+i);
	}
	return L_OK;
}

int LinxBeagleBone::DigitalWriteSquareWave(unsigned char channel, unsigned long freq, unsigned long duration)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxBeagleBone::DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned long timeout, unsigned long* width)
{
	return L_FUNCTION_NOT_SUPPORTED;
}


//--------------------------------------------------------PWM-------------------------------------------------------
int LinxBeagleBone::PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	//unsigned long period = 500000;		//Period Defaults To 500,000 nS.  To Do Update This When Support For Changing Period / Frequency Is Added
	unsigned long dutyCycle = 0;
	
	//Smart Open PWM Channels
	pwmSmartOpen(numChans, channels);
	
	for(int i=0; i<numChans; i++)
	{
		if(values[i] == 0)
		{				
			dutyCycle = 0;
		}
		else if(values[i] == 255)
		{
			dutyCycle = PwmPeriods[channels[i]];		
		}
		else
		{
			dutyCycle = PwmPeriods[channels[i]]*(values[i] / 255.0);
		}
		
		//Update Output
		DebugPrint("Setting Duty Cycle = ");
		DebugPrint(dutyCycle, DEC);
		fprintf(PwmDutyCycleHandles[channels[i]], "%lu", dutyCycle);	
		DebugPrint(" ... Duty Cycle Set ... ");
		fflush(PwmDutyCycleHandles[channels[i]]);
		DebugPrintln("Flushing.");
	}
	
	return L_OK;
}

int PwmSetFrequency(unsigned char numChans, unsigned char* channels, unsigned long* values)
{		
	return L_FUNCTION_NOT_SUPPORTED;
}

//--------------------------------------------------------SPI-------------------------------------------------------
int LinxBeagleBone::SpiOpenMaster(unsigned char channel)
{
	//Load SPI DTO If Necissary
	if(!fileExists(SpiPaths[channel].c_str()))
	{
		if(!loadDto(SpiDtoNames[channel].c_str()))		
		{
			DebugPrint("SPI Fail - Failed To Load SPI DTO");
			return  LSPI_OPEN_FAIL;
		}
	}
	
	SpiHandles[channel]= open(SpiPaths[channel].c_str(), O_RDWR);
	
	if(SpiHandles[channel] == 0)
	{
		return LSPI_OPEN_FAIL;
	}
	else
	{
		//Default To Mode 0 With No CS (LINX Uses GPIO When Performing Write)
		unsigned long spi_Mode = SPI_MODE_0;			
		if(ioctl(SpiHandles[channel], SPI_IOC_WR_MODE, &spi_Mode) < 0)					
		{
			DebugPrint("SPI Fail - Failed To Set SPI Mode - ");
			DebugPrintln(spi_Mode, BIN);
			return LSPI_OPEN_FAIL;
		}
		
		//Default Max Speed To 
		unsigned long speed = 25000;			
		if (ioctl(SpiHandles[channel], SPI_IOC_WR_MAX_SPEED_HZ, &speed) > 0)
		{			
			DebugPrint("SPI Fail - Failed To Set SPI Max Speed - ");
			DebugPrintln(speed, DEC);
			return LSPI_OPEN_FAIL;
		}			
		
	}
	return L_OK;
}

int LinxBeagleBone::SpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	SpiBitOrders[channel] = bitOrder;
	return L_OK;
}

int LinxBeagleBone::SpiSetMode(unsigned char channel, unsigned char mode)
{
	unsigned long spi_Mode = (unsigned long) mode;
	if(ioctl(SpiHandles[channel], SPI_IOC_WR_MODE, &spi_Mode) < 0)
	{
		DebugPrintln("Failed To Set SPI Mode");
		return  L_UNKNOWN_ERROR;
	}		
	return L_OK;
}

int LinxBeagleBone::SpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{	
	int index = 0;
	//Loop Over All Supported SPI Speeds
	for(index=0; index < NumSpiSpeeds; index++)
	{
			
			if(speed < *(SpiSupportedSpeeds+index))
			{
				index = index - 1; //Use Fastest Speed Below Target Speed
				break;
			}
			//If Target Speed Is Higher Than Max Speed Use Max Speed			
	}
	SpiSetSpeeds[channel] = *(SpiSupportedSpeeds+index);
	*actualSpeed = *(SpiSupportedSpeeds+index); 
	
	return L_OK;
}

int LinxBeagleBone::SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	unsigned char nextByte = 0;	//First Byte Of Next SPI Frame	

	//SPI Hardware Only Supports MSb First Transfer.  If  Configured for LSb First Reverse Bits In Software
	if( SpiBitOrders[channel] == LSBFIRST )
	{
		for(int i=0; i< frameSize*numFrames; i++)
		{			
			sendBuffer[i] = ReverseBits(sendBuffer[i]);
		}
	}
	
	struct spi_ioc_transfer transfer = {};
	
	//Set CS As Output And Make Sure CS Starts Idle	
	DigitalWrite(csChan, (~csLL & 0x01) );
	
	for(int i=0; i< numFrames; i++)
	{
		//Setup Transfer
		transfer.tx_buf = (unsigned long)(sendBuffer+nextByte);
		transfer.rx_buf = (unsigned long)(recBuffer+nextByte);
		transfer.len = frameSize;
		transfer.delay_usecs = 0;
		//transfer.speed_hz = SpiSetSpeeds[channel];
		transfer.speed_hz = 25000;
		transfer.bits_per_word = 8;
	
		//CS Active
		DigitalWrite(csChan, csLL);			
		
		//Transfer Data
		int retVal = ioctl(SpiHandles[channel], SPI_IOC_MESSAGE(1), &transfer);
		
		//CS Idle
		DigitalWrite(csChan, (~csLL & 0x01) );
		
		if (retVal < 0)
		{
			DebugPrintln("SPI Fail - Failed To Transfer Data");
			return  LSPI_TRANSFER_FAIL;
		}
		
		nextByte += frameSize;			
	}

	return L_OK;
}


//--------------------------------------------------------I2C-------------------------------------------------------
int LinxBeagleBone::I2cOpenMaster(unsigned char channel)
{
	//Export Dev Tree Overlay If Device DNE
	if(!fileExists(I2cPaths[channel].c_str()))
	{		
		DebugPrint("I2C - Loading DTO ");
		DebugPrintln(I2cDtoNames[channel].c_str());
		if(FilePathLayout == 7)
		{
			if(!loadDto(I2cDtoNames[channel].c_str()))		
			{
				DebugPrintln("I2C Fail - Failed To Load BB-I2C DTO");
				return  LI2C_OPEN_FAIL;
			}
		}
	}
	
	int handle = open(I2cPaths[channel].c_str(), O_RDWR);
	if (handle < 0)
	{
		DebugPrintln("I2C Fail - Failed To Open I2C Channel");
		return  LI2C_OPEN_FAIL;
	}
	else
	{
		I2cHandles[channel] = handle;
	}
	return L_OK;
}

int LinxBeagleBone::I2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxBeagleBone::I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	//Check EOF - Currently Only Support 0x00
	if(eofConfig != EOF_STOP)
	{
		DebugPrintln("I2C Fail - EOF Not Supported");
		return LI2C_EOF;	
	}
	
	//Set Slave Address
	if (ioctl(I2cHandles[channel], I2C_SLAVE, slaveAddress) < 0) 
	{			
		//Failed To Set Slave Address
		DebugPrintln("I2C Fail - Failed To Set Slave Address");
		return LI2C_SADDR;
	}
		
	//Write Data
	int retVal = write(I2cHandles[channel], sendBuffer, numBytes);
	if(retVal != numBytes)
	{
		DebugPrintln("I2C Fail - Failed To Write All Data");
		//return LI2C_WRITE_FAIL;
		return errno;
	}
	
	return L_OK;
}

int LinxBeagleBone::I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	//Check EOF - Currently Only Support 0x00
	if(eofConfig != EOF_STOP)
	{
		return LI2C_EOF;	
	}
	
	//Set Slave Address
	if (ioctl(I2cHandles[channel], I2C_SLAVE, slaveAddress) < 0) 
	{
		//Failed To Set Slave Address
		return LI2C_SADDR;
	}
	
	if(read(I2cHandles[channel], recBuffer, numBytes) < numBytes)
	{
		return LI2C_READ_FAIL;	
	}
	
	return L_OK;
}

int LinxBeagleBone::I2cClose(unsigned char channel)
{
	//Close I2C Channel
	if(close(I2cHandles[channel]) < 0)
	{
		return LI2C_CLOSE_FAIL;
	}
	
	return L_OK;
}


//--------------------------------------------------------UART-------------------------------------------------------
int LinxBeagleBone::UartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	
	DebugPrintln("UART Open");
	
	//Load DTO If Needed
	if(!fileExists(UartPaths[channel].c_str()))
	{
		if(!loadDto(UartDtoNames[channel].c_str()))	
		{
			DebugPrint("UART Fail - Failed To Load ");
			DebugPrint(UartDtoNames[channel].c_str());
			DebugPrintln(" DTO");
			return  LUART_OPEN_FAIL;
		}			
	}
	
	//Open UART	Handle If Not Already Open
	
if(UartHandles[channel] <= 0)
	{
		int handle = open(UartPaths[channel].c_str(),  O_RDWR);
			
		if (handle <= 0)
		{
			DebugPrint("UART Fail - Failed To Open UART Handle -  ");
			DebugPrintln(UartPaths[channel].c_str());
			return  LUART_OPEN_FAIL;
		}
		else
		{
			UartHandles[channel] = handle;
		}
	}
	/*else
	{
		DebugPrint("UART ");		
		DebugPrint(channel, DEC);
		DebugPrintln(" already Open.");		
	}*/
	
	if(UartSetBaudRate(channel, baudRate, actualBaud) != L_OK)
	{
		DebugPrintln("Failed to set baud rate");
	}
	
	return L_OK;
}

int LinxBeagleBone::UartSetBaudRate(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	//Get Closest Support Baud Rate Without Going Over

	//Loop Over All Supported SPI Speeds
	int index = 0;
	for(index=0; index < NumUartSpeeds; index++)
	{				
		if(baudRate < *(UartSupportedSpeeds+index))
		{		
			//Previous Index Was Closest Supported Baud Without Going Over, Index Will Be Decremented Accordingly Below.
			break;
		}	
	}
	
	//Once Loop Completes Index Is One Higher Than The Correct Baud, But Could Be Zero So Check And Decrement Accordingly
	//If The Entire Loop Runs Then index == NumUartSpeeds So Decrement It To Get Max Baud
	if(index != 0)
	{
		index = index -1;
	}
	
	//Store Actual Baud Used
	*actualBaud = (unsigned long) *(UartSupportedSpeeds+index);
	
	//Set Baud Rate
	struct termios options;	
	tcgetattr(UartHandles[channel], &options);
	
	options.c_cflag = *(UartSupportedSpeedsCodes+index) | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	
	tcflush(UartHandles[channel], TCIFLUSH);	
	tcsetattr(UartHandles[channel], TCSANOW, &options);
	
	return  L_OK;
}

int LinxBeagleBone::UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
{
	int bytesAtPort = -1;
	ioctl(UartHandles[channel], FIONREAD, &bytesAtPort);
	
	if(bytesAtPort < 0)
	{
		return LUART_AVAILABLE_FAIL;
	}
	else
	{
		*numBytes = (unsigned char) bytesAtPort;
	}
	return  L_OK;
}

int LinxBeagleBone::UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	//Check If Enough Bytes Are Available
	unsigned char bytesAvailable = -1;
	UartGetBytesAvailable(channel, &bytesAvailable);
	
	if(bytesAvailable >= numBytes)
	{
		//Read Bytes From Input Buffer
		int bytesRead = read(UartHandles[channel], recBuffer, numBytes);
		*numBytesRead = (unsigned char) bytesRead;
		
		if(bytesRead != numBytes)
		{
			return LUART_READ_FAIL;
		}		
	}
	return  L_OK;
}

int LinxBeagleBone::UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	int bytesSent = write(UartHandles[channel], sendBuffer, numBytes);	
	if(bytesSent != numBytes)
	{
		return LUART_WRITE_FAIL;
	}
	return  L_OK;
}

int LinxBeagleBone::UartClose(unsigned char channel)
{
	//Close UART Channel, Return OK or Error
	if (close(UartHandles[channel]) < 0)
	{
		return LUART_CLOSE_FAIL;
	}
	UartHandles[channel] = 0;
	return  L_OK;
}


//--------------------------------------------------------SERVO-------------------------------------------------------
int LinxBeagleBone::ServoOpen(unsigned char numChans, unsigned char* chans)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxBeagleBone::ServoSetPulseWidth(unsigned char numChans, unsigned char* chans, unsigned short* pulseWidths)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxBeagleBone::ServoClose(unsigned char numChans, unsigned char* chans)
{
	return L_FUNCTION_NOT_SUPPORTED;
}


//--------------------------------------------------------GENERAL-------------------------------------------------------
unsigned long LinxBeagleBone::GetMilliSeconds()
{
	timespec mTime;
	clock_gettime(CLOCK_MONOTONIC, &mTime);
	//return (mTime.tv_nsec / 1000000);
	return ( ((unsigned long) mTime.tv_sec * 1000) + mTime.tv_nsec / 1000000);
}

unsigned long LinxBeagleBone::GetSeconds()
{
	timespec mTime;
	clock_gettime(CLOCK_MONOTONIC, &mTime);	
	return mTime.tv_sec;
}

void LinxBeagleBone::DelayMs(unsigned long ms)
{
	usleep(ms * 1000);
}

void LinxBeagleBone::NonVolatileWrite(int address, unsigned char data)
{
	
}

unsigned char LinxBeagleBone::NonVolatileRead(int address)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

