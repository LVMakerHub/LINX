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
**  Functions
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
		return L_FUNCTION_NOT_SUPPORTED;
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
	