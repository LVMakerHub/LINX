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

#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>	
#include <sys/ioctl.h>

/****************************************************************************************
**  Variables
****************************************************************************************/		

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
**  Public Functions
****************************************************************************************/
	
	//--------------------------------------------------------ANALOG-------------------------------------------------------
	int LinxBeagleBone::AnalogRead(unsigned char numPins, unsigned char* pins, unsigned char* values)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxBeagleBone::AnalogSetRef(unsigned char mode, unsigned long voltage)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}

	//--------------------------------------------------------DIGITAL-------------------------------------------------------
	int LinxBeagleBone::DigitalWrite(unsigned char numPins, unsigned char* pins, unsigned char* values)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxBeagleBone::DigitalRead(unsigned char numPins, unsigned char* pins, unsigned char* values)
	{
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
	int LinxBeagleBone::PwmSetDutyCycle(unsigned char numPins, unsigned char* pins, unsigned char* values)
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
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxBeagleBone::I2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxBeagleBone::I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxBeagleBone::I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxBeagleBone::I2cClose(unsigned char channel)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	
	//--------------------------------------------------------UART-------------------------------------------------------
	int LinxBeagleBone::UartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
	{
		
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