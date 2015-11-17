/****************************************************************************************
**  LINX - Raspberry Pi code
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
#include "LinxRaspberryPi.h"

#include <map>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <termios.h>	
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

#include <cerrno>

using namespace std;

/****************************************************************************************
**  Variables
****************************************************************************************/		

/****************************************************************************************
**  Constructors / Destructors 
****************************************************************************************/
LinxRaspberryPi::LinxRaspberryPi()
{
	//LINX API Version
	LinxApiMajor = 2;
	LinxApiMinor = 0;
	LinxApiSubminor = 0;
	
	// TODO Load User Config Data From Non Volatile Storage
	//userId = NonVolatileRead(NVS_USERID) << 8 | NonVolatileRead(NVS_USERID + 1);
	
}

LinxRaspberryPi::~LinxRaspberryPi()
{
		
}

/****************************************************************************************
**  Private Functions
****************************************************************************************/
int LinxRaspberryPi::digitalSmartOpen(unsigned char numChans, unsigned char* channels)
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
int LinxRaspberryPi::pwmSmartOpen(unsigned char numChans, unsigned char* channels)
{
	return L_FUNCTION_NOT_SUPPORTED;
}	

//Return True If File Specified By path Exists.
bool LinxRaspberryPi::fileExists(const char* path)
{
	struct stat buffer;   
	return (stat(path, &buffer) == 0); 
}

bool LinxRaspberryPi::fileExists(const char* directory, const char* fileName)
{
	char fullPath[128];
	sprintf(fullPath, "%s%s", directory, fileName);				
	struct stat buffer;
	return (stat(fullPath, &buffer) == 0);  
}

bool LinxRaspberryPi::fileExists(const char* directory, const char* fileName, unsigned long timeout)
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

/****************************************************************************************
**  Public Functions
****************************************************************************************/

//------------------------------------- Analog -------------------------------------
int LinxRaspberryPi::AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxRaspberryPi::AnalogSetRef(unsigned char mode, unsigned long voltage)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

		
//------------------------------------- Digital -------------------------------------
int LinxRaspberryPi::DigitalSetDirection(unsigned char numChans, unsigned char* channels, unsigned char* values)
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
		if( ((values[i/8] >> i%8) & 0x01) == OUTPUT && DigitalDirs[channels[i]] != OUTPUT)
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

int LinxRaspberryPi::DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
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

int LinxRaspberryPi::DigitalWrite(unsigned char channel, unsigned char value)
{
	return DigitalWrite(1, &channel, &value);
}

int LinxRaspberryPi::DigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
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

int LinxRaspberryPi::DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
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
					
		retVal = retVal | ((unsigned char)diVal << bitOffset);	//Read Pin And Insert Value Into retVal
	}

	//Store Last Byte
	values[byteOffset] = retVal;
		
	return L_OK;
}

int LinxRaspberryPi::DigitalRead(unsigned char channel, unsigned char* value)
{
	return DigitalRead(1, &channel, value);
}

int LinxRaspberryPi::DigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
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

int LinxRaspberryPi::DigitalWriteSquareWave(unsigned char channel, unsigned long freq, unsigned long duration)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxRaspberryPi::DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned long timeout, unsigned long* width)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

		
//------------------------------------- PWM -------------------------------------
int LinxRaspberryPi::PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

		
//------------------------------------- SPI -------------------------------------
int LinxRaspberryPi::SpiOpenMaster(unsigned char channel)
{
	//fprintf(stdout, "SpiOpen(%d)\n", channel);
	SpiHandles[channel]= open(SpiPaths[channel].c_str(), O_RDWR);
	
	if(SpiHandles[channel] == 0)
	{
		//fprintf(stdout, "SPI OPEN FAIL");
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
			
			//fprintf(stdout, "SPI OPEN FAIL");
			return LSPI_OPEN_FAIL;			
		}
		
		//Open With Default Clock Speed
		if (ioctl(SpiHandles[channel], SPI_IOC_WR_MAX_SPEED_HZ, &SpiDefaultSpeed) < 0)
		{			
			DebugPrint("SPI Fail - Failed To Set SPI Max Speed - ");
			DebugPrintln(SpiDefaultSpeed, DEC);
			
			//fprintf(stdout, "SPI OPEN FAIL");
			return LSPI_OPEN_FAIL;			
		}
	}
	return L_OK;
}

int LinxRaspberryPi::SpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	SpiBitOrders[channel] = bitOrder;
	return L_OK;
}

int LinxRaspberryPi::SpiSetMode(unsigned char channel, unsigned char mode)
{
	unsigned long spi_Mode = (unsigned long) mode;
	if(ioctl(SpiHandles[channel], SPI_IOC_WR_MODE, &spi_Mode) < 0)
	{
		DebugPrintln("Failed To Set SPI Mode");
		return  L_UNKNOWN_ERROR;
	}
		
	return L_OK;
}

int LinxRaspberryPi::SpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
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

int LinxRaspberryPi::SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
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
		
		struct spi_ioc_transfer transfer;
		
		//Set CS As Output And Make Sure CS Starts Idle	
		DigitalWrite(csChan, (~csLL & 0x01) );
		
		for(int i=0; i< numFrames; i++)
		{
			//Setup Transfer
			transfer.tx_buf = (unsigned long)(sendBuffer+nextByte);
			transfer.rx_buf = (unsigned long)(recBuffer+nextByte);
			transfer.len = frameSize;
			transfer.delay_usecs = 0;
			transfer.speed_hz = SpiSetSpeeds[channel];
			//transfer.speed_hz = 25000;
			transfer.bits_per_word = 8;
			transfer.pad = 0;
		
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
		
//------------------------------------- I2C -------------------------------------
int LinxRaspberryPi::I2cOpenMaster(unsigned char channel)
{	
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

int LinxRaspberryPi::I2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxRaspberryPi::I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	//Check EOF - Currently Only Support 0x00
	if(eofConfig != EOF_STOP)
	{
		DebugPrintln("I2C Fail - EOF Not Supported");
		return LI2C_EOF;	
	}
	
	//Set Slave Address
	if(ioctl(I2cHandles[channel], I2C_SLAVE, slaveAddress) < 0) 
	{			
		//Failed To Set Slave Address
		DebugPrintln("I2C Fail - Failed To Set Slave Address");
		return LI2C_SADDR;
	}
		
	//Write Data
	if(write(I2cHandles[channel], sendBuffer, numBytes) != numBytes)
	{
		DebugPrintln("I2C Fail - Failed To Write All Data");
		return LI2C_WRITE_FAIL;
	}
	
	return L_OK;
}

int LinxRaspberryPi::I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
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
	
int LinxRaspberryPi::I2cClose(unsigned char channel)
{
	//Close I2C Channel
	if(close(I2cHandles[channel]) < 0)
	{
		return LI2C_CLOSE_FAIL;
	}
	
	return L_OK;
}

		
//------------------------------------- UART -------------------------------------
int LinxRaspberryPi::UartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	//Open UART	Handle If Not Already Open	
	if(UartHandles[channel] <= 0)
	{
		int handle = open(UartPaths[channel].c_str(),  O_RDWR);
			
		if(handle <= 0)
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
	
	if(UartSetBaudRate(channel, baudRate, actualBaud) != L_OK)
	{
		DebugPrintln("Failed to set baud rate");
	}
	
	return L_OK;
}

int LinxRaspberryPi::UartSetBaudRate(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
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

int LinxRaspberryPi::UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
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

int LinxRaspberryPi::UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
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

int LinxRaspberryPi::UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	int bytesSent = write(UartHandles[channel], sendBuffer, numBytes);	
	if(bytesSent != numBytes)
	{
		return LUART_WRITE_FAIL;
	}
	
	return  L_OK;
}

int LinxRaspberryPi::UartClose(unsigned char channel)
{
	//Close UART Channel, Return OK or Error
	if (close(UartHandles[channel]) < 0)
	{
		return LUART_CLOSE_FAIL;
	}
	UartHandles[channel] = 0;
	
	return  L_OK;
	
}

		
//------------------------------------- Servo -------------------------------------
int LinxRaspberryPi::ServoOpen(unsigned char numChans, unsigned char* channels)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxRaspberryPi::ServoSetPulseWidth(unsigned char numChans, unsigned char* channels, unsigned short* pulseWidths)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxRaspberryPi::ServoClose(unsigned char numChans, unsigned char* channels)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

				
//------------------------------------- General -------------------------------------
unsigned long LinxRaspberryPi::GetMilliSeconds()
{
	timespec mTime;
	clock_gettime(CLOCK_MONOTONIC, &mTime);
	//return (mTime.tv_nsec / 1000000);
	return ( ((unsigned long) mTime.tv_sec * 1000) + mTime.tv_nsec / 1000000);
}

unsigned long LinxRaspberryPi::GetSeconds()
{
	timespec mTime;
	clock_gettime(CLOCK_MONOTONIC, &mTime);	
	return mTime.tv_sec;
}

void LinxRaspberryPi::DelayMs(unsigned long ms)
{
	usleep(ms * 1000);
}

void LinxRaspberryPi::NonVolatileWrite(int address, unsigned char data)
{
	
}

unsigned char LinxRaspberryPi::NonVolatileRead(int address)
{
	return L_FUNCTION_NOT_SUPPORTED;
}




