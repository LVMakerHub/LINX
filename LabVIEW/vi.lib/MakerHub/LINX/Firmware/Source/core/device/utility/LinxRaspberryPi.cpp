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

#include <fcntl.h>
#include <sstream>
#include<stdio.h>
#include <time.h>
#include <unistd.h>


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
**  Public Functions
****************************************************************************************/
	
	//--------------------------------------------------------ANALOG-------------------------------------------------------
	int LinxRaspberryPi::AnalogRead(unsigned char numPins, unsigned char* pins, unsigned char* values)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::AnalogSetRef(unsigned char mode, unsigned long voltage)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}

	//--------------------------------------------------------DIGITAL-------------------------------------------------------
	int LinxRaspberryPi::DigitalWrite(unsigned char numPins, unsigned char* pins, unsigned char* values)
	{
		for(int i=0; i<numPins; i++)
		{		
			GpioSetDir(pins[i], 1);
			GpioWrite( pins[i], (values[i/8] >> i%8) & 0x01);
		}
		
		return L_OK;
	}
	
	int LinxRaspberryPi::DigitalRead(unsigned char numPins, unsigned char* pins, unsigned char* values)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::DigitalWriteSquareWave(unsigned char channel, unsigned long freq, unsigned long duration)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned long timeout, unsigned long* width)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	
	//--------------------------------------------------------PWM-------------------------------------------------------
	int LinxRaspberryPi::PwmSetDutyCycle(unsigned char numPins, unsigned char* pins, unsigned char* values)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	
	//--------------------------------------------------------SPI-------------------------------------------------------
	int LinxRaspberryPi::SpiOpenMaster(unsigned char channel)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::SpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::SpiSetMode(unsigned char channel, unsigned char mode)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::SpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	
	//--------------------------------------------------------I2C-------------------------------------------------------
	int LinxRaspberryPi::I2cOpenMaster(unsigned char channel)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::I2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::I2cClose(unsigned char channel)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	
	//--------------------------------------------------------UART-------------------------------------------------------
	int LinxRaspberryPi::UartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::UartSetBaudRate(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::UartClose(unsigned char channel)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	
	//--------------------------------------------------------SERVO-------------------------------------------------------
	int LinxRaspberryPi::ServoOpen(unsigned char numChans, unsigned char* chans)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::ServoSetPulseWidth(unsigned char numChans, unsigned char* chans, unsigned short* pulseWidths)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	int LinxRaspberryPi::ServoClose(unsigned char numChans, unsigned char* chans)
	{
		return L_FUNCTION_NOT_SUPPORTED;
	}
	
	
	//--------------------------------------------------------GENERAL-------------------------------------------------------
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

/****************************************************************************************
**  Private Functions
****************************************************************************************/
	int LinxRaspberryPi::GpioExport (const unsigned char numGpioChans, const unsigned char*  gpioChans, int* digitalDirHandles, int* digitalValueHandles)
	{	
		//Open Export File
		int exportFile = open("/sys/class/gpio/export", O_RDWR);	
		
		char buff[3];	
		
		//Export GPIO Pins And Open Handles To Direction And Value
		for(int i=0; i<numGpioChans; i++)
		{		
			//Export GPIO Pin
			snprintf(buff, 3, "%d",  gpioChans[i]);
			write(exportFile, buff, 3);
			
			//Open And Save GPIO Dir Handle
			char dirFilePath [40];
			snprintf(dirFilePath, 40, "/sys/class/gpio/gpio%d/direction", gpioChans[i]);
			int handle = open(dirFilePath, O_RDWR);
			if( handle > 0)
			{
				digitalDirHandles[i] = handle;
			}
			else
			{
				//Unable To Open Dir Handle
				printf("Unable To Open %s\n", dirFilePath);
			}
			
			
			//Open And Save GPIO Value Handle
			char valueFilePath [40];
			snprintf(valueFilePath, 40, "/sys/class/gpio/gpio%d/value", gpioChans[i]);
			digitalValueHandles[i] = open(valueFilePath, O_RDWR);
		}

		//Close Export File
		close(exportFile);		
		
		return L_OK;
	}
	
	int LinxRaspberryPi::GpioUnexport(const unsigned char*  gpioChans, const unsigned char numGpioChans)
	{
		//Open Unexport File
		int unexportFile = open("/sys/class/gpio/unexport", O_RDWR);	
		
		char buff[3];	
		
		//Export GPIO Pins
		for(int i=0; i<numGpioChans; i++)
		{
			snprintf(buff, 3, "%d",  gpioChans[i]);
			write(unexportFile, buff, 3);
		}
		
		return L_OK;
	}
	
	int LinxRaspberryPi::GpioSetDir(unsigned char pin, unsigned char mode)
	{
		int pinIndex = GetDigitalChanIndex(pin);
		
		if(pinIndex < 0)
		{
			return LDIGITAL_PIN_DNE;
		}
		else
		{
			int dirHandle = DigitalDirHandles[pinIndex];
			
			if(mode == INPUT)
			{
				write(dirHandle, "in", 2);
			}
			else if (mode == OUTPUT)
			{
				write(dirHandle, "out", 3);
			}
			else
			{
				//TODO ERROR
			}
		}
			
		return L_OK;
	}
	
	int LinxRaspberryPi::GpioWrite(unsigned char pin, unsigned char val)
	{		
		int pinIndex = GetDigitalChanIndex(pin);
		
		if(pinIndex < 0)
		{
			return LDIGITAL_PIN_DNE;
		}
		else
		{
			int valueHandle = DigitalValueHandles[pinIndex];
			
			if(val == HIGH)
			{
				write(valueHandle, "1\00", 2);
			}
			else if (val == LOW)
			{
				write(valueHandle, "0\00", 2);
			}
			else
			{
				//TODO ERROR
			}
		}
			
		return L_OK;
	}
		
	int LinxRaspberryPi::GetDigitalChanIndex(unsigned char chanNum)
	{
		for(int i=0; i< NumDigitalChans; i++)
		{
			if(DigitalChans[i] == chanNum)
			{
				return i;
			}		
		}
		return -1;
	}





