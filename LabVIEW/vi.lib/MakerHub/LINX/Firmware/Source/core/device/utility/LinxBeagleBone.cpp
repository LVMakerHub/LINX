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
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <termios.h>	
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

using namespace std;

/****************************************************************************************
**  Variables
****************************************************************************************/		
const char* overlaySlotsPath = "/sys/devices/bone_capemgr.9/slots";

/****************************************************************************************
**  Constructors / Destructors 
****************************************************************************************/
LinxBeagleBone::LinxBeagleBone()
{
	//LINX API Version
	LinxApiMajor = 2;
	LinxApiMinor = 0;
	LinxApiSubminor = 0;
	
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
int LinxBeagleBone::digitalSmartOpen(unsigned char numChans, unsigned char* channels, unsigned char direction)
{
	for(int i=0; i<numChans; i++)
	{
		
		//Open Direction Handle If It Is Not Already
		//char dirPath[64];
		//sprintf(dirPath, "/sys/class/gpio/gpio%d/direction", channels[i]);
		
		if(DigitalDirHandles[channels[i]] == 0)
		{			
			//DigitalDirHandles[channels[i]] = open(dirPath, O_RDWR);
			DigitalDirHandles[channels[i]] = open("/sys/class/gpio/gpio66/direction", O_RDWR);
			if(DigitalDirHandles[channels[i]] == 0)
			{
				DebugPrintln("Digital Fail - Unable To Open Direction File Handles");
				return L_UNKNOWN_ERROR;
			}
		}
		
		//Open Value Handle If It Is Not Already
		//char valuePath[64];
		//sprintf(valuePath, "/sys/class/gpio/gpio%d/value", channels[i]);
		
		if(DigitalValueHandles[channels[i]] == 0)
		{
			//DigitalValueHandles[channels[i]] = open(valuePath, O_RDWR);
			DigitalValueHandles[channels[i]] = open( "/sys/class/gpio/gpio66/value", O_RDWR);
			if(DigitalValueHandles[channels[i]] == 0)
			{
				DebugPrintln("Digital Fail - Unable To Open Value File Handles");
				return L_UNKNOWN_ERROR;
			}
		}
		
		//Set Direction
		if(direction == OUTPUT)
		{
			//Set As Output
			write(DigitalDirHandles[channels[i]], "out", 3);			
		}
		else
		{
			//Set As Input
			write(DigitalDirHandles[channels[i]], "in", 2);	
		}
	}
	return L_OK;
}


/****************************************************************************************
**  Public Functions
****************************************************************************************/
	//--------------------------------------------------------HELPERS-------------------------------------------------------
	bool LinxBeagleBone::FileExists(const char* path)
	{
		struct stat buffer;   
		return (stat(path, &buffer) == 0); 
	}
	
	bool LinxBeagleBone::LoadDto(const char* dtoName, int dtoNameSize)
	{
		int handle = open(overlaySlotsPath,  O_RDWR | O_NDELAY);
		if(handle > 0)
		{
			write(handle, dtoName, dtoNameSize);
			close(handle);
			return true;
		}
		else
		{
			return false;
		}
	}
	
	
	
	/*
	unsigned char LinxBeagleBone::GetGpioIndex(unsigned char channel)
	{
		for(int i=0; i< NumDigitalChans; i++)
		{
			if(DigitalChans[i] == channel)
			{
				return i;
			}		
		}
		DebugPrintln("GPIO Fail - Unable To Find Channel Index");
		return -1;
	}
	*/
		
	
	//--------------------------------------------------------ANALOG-------------------------------------------------------
	int LinxBeagleBone::AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
	{
	
		unsigned int analogValue = 0;
		unsigned char responseByteOffset = 0;
		unsigned char responseBitsRemaining = 8; 
		unsigned char dataBitsRemaining = AiResolution;
		fstream fs;	//AI File Handle
		
		values[responseByteOffset] = 0x00;    //Clear First	Response Byte   

		//Loop Over All AI channels In Command Packet
		for(int i=0; i<numChans; i++)
		{
			//Acquire AI Sample
			
			fs.open(AiPaths[channels[i]], fstream::in);
			fs >> analogValue;
			fs.close();
			
			
			//TODO analogValue = AiHandles[i];
			
			dataBitsRemaining = AiResolution;

			//Byte Packet AI Values In Response Packet
			while(dataBitsRemaining > 0)
			{
				*(values+responseByteOffset) |= ( (analogValue>>(AiResolution - dataBitsRemaining)) << (8 - responseBitsRemaining) );
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
	
	int LinxBeagleBone::AnalogSetRef(unsigned char mode, unsigned long voltage)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}

	//--------------------------------------------------------DIGITAL-------------------------------------------------------
	int LinxBeagleBone::DigitalSetDirection(unsigned char numChans, unsigned char* channels, unsigned char* values)
	{		
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxBeagleBone::DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
	{
		if(digitalSmartOpen(numChans, channels, OUTPUT) != L_OK)
		{
			DebugPrintln("Smart Open Failed");
			return L_UNKNOWN_ERROR;			
		}
				
		for(int i=0; i<numChans; i++)
		{
			//Set Value
			if( ((values[i/8] >> i%8) & 0x01) == 0)
			{
				write(DigitalValueHandles[channels[i]], "0", 1);
			}
			else
			{
				write(DigitalValueHandles[channels[i]], "1", 1);
			}
		}
			
		return L_OK;
	}
	
	int LinxBeagleBone::DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
	{
		for(int i=0; i<numChans; i++)
		{
			//Set As Output
			write(DigitalDirHandles[channels[i]], "out", 3);
		}
		return L_FUNCTION_NOT_SUPPORTED;
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
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	
	//--------------------------------------------------------SPI-------------------------------------------------------
	int LinxBeagleBone::SpiOpenMaster(unsigned char channel)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxBeagleBone::SpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxBeagleBone::SpiSetMode(unsigned char channel, unsigned char mode)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxBeagleBone::SpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxBeagleBone::SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	
	//--------------------------------------------------------I2C-------------------------------------------------------
	int LinxBeagleBone::I2cOpenMaster(unsigned char channel)
	{
		//Export Dev Tree Overlay If Device DNE
		if(!FileExists(I2cPaths[channel]))
		{
			switch(channel)
			{
				case 0:
					//Internal I2C, Do Nothing
					break;
				case 1:
					if(!LoadDto("BB-I2C1", 7))		
					{
						DebugPrintln("I2C Fail - Failed To Load BB-I2C DTO");
						return  LI2C_OPEN_FAIL;
					}
					break;
				case 2:
					if(!LoadDto("BB-I2C2", 7))
					{
						DebugPrintln("I2C Fail - Failed To Load BB-I2C DTO");
						return  LI2C_OPEN_FAIL;
					}
					break;
				default:
					return  LI2C_OPEN_FAIL;
					break;
			}
		}
		
		int handle = open(I2cPaths[channel], O_RDWR);
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
		if(write(I2cHandles[channel], sendBuffer, numBytes) != numBytes)
		{
			DebugPrintln("I2C Fail - Failed To Write All Data");
			return LI2C_WRITE_FAIL;
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
		//Export Dev Tree Overlay If Device DNE
		if(!FileExists(UartPaths[channel]))
		{
			switch(channel)
			{
				case 0:
					if(!LoadDto("BB-UART0", 8))		
					{
						DebugPrintln("UART Fail - Failed To Load BB-UART0 DTO");
						return  LUART_OPEN_FAIL;
					}
					break;
				case 1:
					if(!LoadDto("BB-UART1", 8))		
					{
						DebugPrintln("UART Fail - Failed To Load BB-UART1 DTO");
						return  LUART_OPEN_FAIL;
					}
					break;
				case 2:
					if(!LoadDto("BB-UART2", 8))		
					{
						DebugPrintln("UART Fail - Failed To Load BB-UART2 DTO");
						return  LUART_OPEN_FAIL;
					}
					break;
				case 3:
					if(!LoadDto("BB-UART3", 8))		
					{
						DebugPrintln("UART Fail - Failed To Load BB-UART3 DTO");
						return  LUART_OPEN_FAIL;
					}
					break;
				case 4:
					if(!LoadDto("BB-UART4", 8))		
					{
						DebugPrintln("UART Fail - Failed To Load BB-UART4 DTO");
						return  LUART_OPEN_FAIL;
					}
					break;
				case 5:
					if(!LoadDto("BB-UART5", 8))		
					{
						DebugPrintln("UART Fail - Failed To Load BB-UART5 DTO");
						return  LUART_OPEN_FAIL;
					}
					break;
				default:
					return  LUART_OPEN_FAIL;
					break;
			}
		}
		
		//Open UART	
		int handle = open(UartPaths[channel],  O_RDWR | O_NDELAY);
				
		if (handle <= 0)
		{
			//DEBUG((char*)"Failed To Open UART Channel");
			return  LUART_OPEN_FAIL;
		}	
		else
		{
			UartHandles[channel] = handle;
		}
		
		UartSetBaudRate(channel, baudRate, actualBaud);
		
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
				index = index - 1; //Use Fastest Baud Less Or Equal To Target Baud
				break;
			}
			//If Target Baud Is Higher Than Max Baud Use Max Baud			
		}
		
		//Store Actual Baud Used
		*actualBaud = (unsigned long) *(UartSupportedSpeeds+index);
		// DEBUG printf("Baud = %d\n", *(UartSupportedSpeeds+index));
		
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
	
	