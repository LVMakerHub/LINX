/****************************************************************************************
**  LINX Device Abstraction Layer (Helper Library)
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#include <stdio.h>
#include <iostream>
#include <string.h>

#include "LinxDevice.h"

//------------------------------------- Raspberry Pi -------------------------------------
#if LINX_DEVICE_FAMILY == 4 
	#if LINX_DEVICE_ID == 3	//RPI 2 B
			#define LINXDEVICETYPE LinxRaspberryPi2B
			#include "LinxRaspberryPi.h"
			#include "LinxRaspberryPi2B.h"
	#elif LINX_DEVICE_ID == 5	//RPI 5
			#define LINXDEVICETYPE LinxRaspberryPi5
			#include "LinxRaspberryPi.h"
			#include "LinxRaspberryPi5.h"
	#endif
//------------------------------------- Beagle Bone -------------------------------------
#elif LINX_DEVICE_FAMILY == 6	
	#if LINX_DEVICE_ID == 1
			#define LINXDEVICETYPE LinxBeagleBoneBlack
			#include "LinxBeagleBone.h"
			#include "LinxBeagleBoneBlack.h"
	#endif
#endif

LINXDEVICETYPE* LinxDev;

//------------------------------------- Constructor / Destructor -------------------------------------
extern "C" int LinxOpen()
{
	
	//Instantiate The LINX Device
	LinxDev = new LINXDEVICETYPE();
	return L_OK;
}

extern "C" int LinxClose()
{
	delete LinxDev;
	return L_OK;
}


//------------------------------------- Enumeration -------------------------------------
extern "C" int LinxGetDeviceFamily()
{
	return LinxDev->DeviceFamily;
}

extern "C" int LinxGetDeviceId()
{
	return LinxDev->DeviceId;
}

extern "C" int LinxGetDeviceName(char* name)
{
    int len = LinxDev->DeviceNameLen;
	if (len > 62) // buffer passed is max 64 bytes
		len = 62;
	memcpy(name, LinxDev->DeviceName, len);
    name[len+1] = '\0';
	return L_OK;
}


//------------------------------------- General -------------------------------------
extern "C" unsigned long LinxGetMilliSeconds()
{
	return LinxDev->GetMilliSeconds();
}


//------------------------------------- Analog -------------------------------------
extern "C" unsigned long LinxAiGetRefSetVoltage()
{
	return LinxDev->AiRefSet;
}

extern "C" unsigned long LinxAoGetRefSetVoltage()
{
	return LinxDev->AoRefSet;
}

extern "C" unsigned char LinxAiGetResolution()
{
	return LinxDev->AiResolution;
}

extern "C" unsigned char LinxAoGetResolution()
{
	return LinxDev->AoResolution;
}

extern "C" unsigned char LinxAiGetNumChans()
{
	return LinxDev->NumAiChans;
}

extern "C" unsigned char LinxAoGetNumChans()
{
	return LinxDev->NumAoChans;
}

extern "C" int LinxAiGetChans(unsigned char numChans, unsigned char* channels)
{
	if(numChans >= LinxDev->NumAiChans)
	{
		//Copy All Channels
		memcpy(channels, LinxDev->AiChans, LinxDev->NumAiChans);
		return LinxDev->NumAiChans;
	}
	else
	{
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, LinxDev->AiChans, numChans);
		return numChans;
	}
}

extern "C" int LinxAoGetChans(unsigned char numChans, unsigned char* channels)
{
	if(numChans >= LinxDev->NumAoChans)
	{
		//Copy All Channels
		memcpy(channels, LinxDev->AoChans, LinxDev->NumAoChans);
		return LinxDev->NumAoChans;
	}
	else
	{
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, LinxDev->AoChans, numChans);
		return numChans;
	}
}

extern "C" int LinxAnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->AnalogRead(numChans, channels, values);
}

extern "C" int LinxAnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned long* values)
{
	return LinxDev->AnalogReadNoPacking(numChans, channels, values);
}	


//------------------------------------- CAN -------------------------------------
extern "C" unsigned char LinxCanGetNumChans()
{
	return LinxDev->NumCanChans;
}
extern "C" int LinxCanGetChans(unsigned char numChans, unsigned char* channels)
{
	if(numChans >= LinxDev->NumCanChans)
	{
		//Copy All Channels
		memcpy(channels, LinxDev->CanChans, LinxDev->NumCanChans);
		return LinxDev->NumCanChans;
	}
	else
	{
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, LinxDev->CanChans, numChans);
		return numChans;
	}
}


//------------------------------------- Digital -------------------------------------
extern "C" unsigned char LinxDigitalGetNumChans()
{
	return LinxDev->NumDigitalChans;
}

extern "C" int LinxDigitalGetChans(unsigned char numChans, unsigned char* channels)
{
	if(numChans >= LinxDev->NumDigitalChans)
	{
		//Copy All Channels
		memcpy(channels, LinxDev->DigitalChans, LinxDev->NumDigitalChans);
		return LinxDev->NumDigitalChans;
	}
	else
	{
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, LinxDev->DigitalChans, numChans);
		return numChans;
	}
}

extern "C" int LinxDigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalRead(numChans, channels, values);
}

extern "C" int LinxDigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalReadNoPacking(numChans, channels, values);
}

extern "C" int LinxDigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalWrite(numChans, channels, values);
}

extern "C" int LinxDigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalWriteNoPacking(numChans, channels, values);
}



//------------------------------------- I2C -------------------------------------
extern "C" unsigned char LinxI2cGetNumChans()
{
	return LinxDev->NumI2cChans;
}

extern "C" int LinxI2cGetChans(unsigned char numChans, unsigned char* channels)
{
	if(numChans >= LinxDev->NumI2cChans)
	{
		//Copy All Channels
		memcpy(channels, LinxDev->I2cChans, LinxDev->NumI2cChans);
		return LinxDev->NumI2cChans;
	}
	else
	{
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, LinxDev->I2cChans, numChans);
		return numChans;
	}
}

extern "C" int LinxI2cOpenMaster(unsigned char channel)
{
	return LinxDev->I2cOpenMaster(channel);
}

extern "C" int LinxI2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return LinxDev->I2cSetSpeed(channel, speed, actualSpeed);
}

extern "C" int LinxI2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxDev->I2cWrite(channel, slaveAddress, eofConfig, numBytes, sendBuffer);
}

extern "C" int LinxI2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	return LinxDev->I2cRead(channel, slaveAddress, eofConfig, numBytes, timeout, recBuffer);
}

extern "C" int LinxI2cClose(unsigned char channel)
{
	return LinxDev->I2cClose(channel);
}

		
//------------------------------------- PWM -------------------------------------
extern "C" unsigned char LinxPwmGetNumChans()
{
	return LinxDev->NumPwmChans;
}

extern "C" int LinxPwmGetChans(unsigned char numChans, unsigned char* channels)
{
	if(numChans >= LinxDev->NumPwmChans)
	{
		//Copy All Channels
		memcpy(channels, LinxDev->PwmChans, LinxDev->NumPwmChans);
		return LinxDev->NumPwmChans;
	}
	else
	{
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, LinxDev->PwmChans, numChans);
		return numChans;
	}
}

extern "C" int LinxPwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->PwmSetDutyCycle(numChans, channels, values);
}


//------------------------------------- QE -------------------------------------
extern "C" unsigned char LinxQeGetNumChans()
{
	return LinxDev->NumQeChans;
}
extern "C" int LinxQeGetChans(unsigned char numChans, unsigned char* channels)
{
	if(numChans >= LinxDev->NumQeChans)
	{
		//Copy All Channels
		memcpy(channels, LinxDev->QeChans, LinxDev->NumQeChans);
		return LinxDev->NumQeChans;
	}
	else
	{
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, LinxDev->QeChans, numChans);
		return numChans;
	}
}

//------------------------------------- Servo -------------------------------------
extern "C" unsigned char LinxServoGetNumChans()
{
	return LinxDev->NumServoChans;
}

extern "C" int LinxServoGetChans(unsigned char numChans, unsigned char* channels)
{
	if(numChans >= LinxDev->NumServoChans)
	{
		//Copy All Channels
		memcpy(channels, LinxDev->ServoChans, LinxDev->NumServoChans); 
		return LinxDev->NumServoChans;
	}
	else
	{
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, LinxDev->ServoChans, numChans);
		return numChans;
	}
}


//------------------------------------- SPI -------------------------------------
extern "C" unsigned char LinxSpiGetNumChans()
{
	return LinxDev->NumSpiChans;
}

extern "C" int LinxSpiGetChans(unsigned char numChans, unsigned char* channels)
{
	if(numChans >= LinxDev->NumSpiChans)
	{
		//Copy All Channels
		memcpy(channels, LinxDev->SpiChans, LinxDev->NumSpiChans);
		return LinxDev->NumSpiChans;
	}
	else
	{
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, LinxDev->SpiChans, numChans);
		return numChans;
	}
}

extern "C" int LinxSpiOpenMaster(unsigned char channel)
{
	return LinxDev->SpiOpenMaster(channel);
}

extern "C" int LinxSpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	return LinxDev->SpiSetBitOrder(channel, bitOrder);
}

extern "C" int LinxSpiSetMode(unsigned char channel, unsigned char mode)
{
	return LinxDev->SpiSetMode(channel, mode);
}

extern "C" int LinxSpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return LinxDev->SpiSetSpeed(channel, speed, actualSpeed);
}

extern "C" int LinxSpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	return LinxDev->SpiWriteRead(channel, frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
}

		
//------------------------------------- UART -------------------------------------
extern "C" unsigned char LinxUartGetNumChans()
{
	return LinxDev->NumUartChans;
}

extern "C" int LinxUartGetChans(unsigned char numChans, unsigned char* channels)
{
	if(numChans >= LinxDev->NumUartChans)
	{
		//Copy All Channels
		memcpy(channels, LinxDev->UartChans, LinxDev->NumUartChans);
		return LinxDev->NumUartChans;
	}
	else
	{
		//Copy As Many Channels As Possible With Given Space
		memcpy(channels, LinxDev->UartChans, numChans);
		return numChans;
	}
}

extern "C" int LinxUartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	return LinxDev->UartOpen(channel, baudRate, actualBaud);
}
extern "C" int LinxUartSetBaudRate(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	return LinxDev->UartSetBaudRate(channel, baudRate, actualBaud);
}

extern "C" int LinxUartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
{
	return LinxDev->UartGetBytesAvailable(channel, numBytes);
}

extern "C" int LinxUartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	return LinxDev->UartRead(channel, numBytes, recBuffer, numBytesRead);
}

extern "C" int LinxUartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxDev->UartWrite(channel, numBytes, sendBuffer);
}

extern "C" int LinxUartClose(unsigned char channel)
{
	return LinxDev->UartClose(channel);
}
