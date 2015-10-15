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
#include "LinxRaspberryPi.h"
#include "LinxRaspberryPi2B.h"


//------------------------------------- Raspberry Pi -------------------------------------
#if LINX_DEVICE_FAMILY == 4 
	#if LINX_DEVICE_ID == 3	//RPI 2 B
			#define LINXDEVICETYPE LinxRaspberryPi2B
	#endif
//------------------------------------- Beagle Bone -------------------------------------
#elif LINX_DEVICE_FAMILY == 6	
	#if LINX_DEVICE_ID == 1
			#define LINXDEVICETYPE LinxBeagleBoneBlack
	#endif
#endif

LINXDEVICETYPE* LinxDev;

//------------------------------------- Constructor / Destructor -------------------------------------
int LinxOpen()
{
	//Instantiate The LINX Device
	LinxDev = new LINXDEVICETYPE();
	fprintf(stdout, "Lib is working...\n");
	return L_OK;
}

int LinxClose()
{
	delete LinxDev;
	return L_OK;
}

int LinxDigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalWrite(numChans, channels, values);
}


//------------------------------------- Enumeration -------------------------------------
int LinxGetDeviceFamily()
{
	return LinxDev->DeviceFamily;
}

int LinxGetDeviceId()
{
	return LinxDev->DeviceId;
}

int LinxGetDeviceName(string* name)
{
	memcpy(name, LinxDev->DeviceName, LinxDev->DeviceNameLen);
	return L_OK;
}


//------------------------------------- Analog -------------------------------------
unsigned char LinxAiGetNumChans()
{
	return LinxDev->NumAiChans;
}

unsigned char LinxAoGetNumChans()
{
	return LinxDev->NumAoChans;
}

int LinxAiGetChans(unsigned char numChans, unsigned char* channels)
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

int LinxAoGetChans(unsigned char numChans, unsigned char* channels)
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

void LinxAiGetChans(const unsigned char* channels)
{
	channels = LinxDev->AiChans;
}

void LinxAoGetChans(const unsigned char* channels)
{
	channels = LinxDev->AoChans;
}

int LinxAnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->AnalogRead(numChans, channels, values);
}
	
int LinxAnalogSetRef(unsigned char mode, unsigned long voltage)
{
	return LinxDev->AnalogSetRef(mode, voltage);
}


//------------------------------------- Digital -------------------------------------
unsigned char LinxDigitalGetNumChans()
{
	return LinxDev->NumDigitalChans;
}

int LinxDigitalGetChans(unsigned char numChans, unsigned char* channels)
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

int LinxDigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalRead(numChans, channels, values);
}


//------------------------------------- I2C -------------------------------------
unsigned char LinxI2cGetNumChans()
{
	return LinxDev->NumI2cChans;
}

int LinxI2cGetChans(unsigned char numChans, unsigned char* channels)
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

int LinxI2cOpenMaster(unsigned char channel)
{
	return LinxDev->I2cOpenMaster(channel);
}

int LinxI2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return LinxDev->I2cSetSpeed(channel, speed, actualSpeed);
}

int LinxI2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxDev->I2cWrite(channel, slaveAddress, eofConfig, numBytes, sendBuffer);
}

int LinxI2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	return LinxDev->I2cRead(channel, slaveAddress, eofConfig, numBytes, timeout, recBuffer);
}

int LinxI2cClose(unsigned char channel)
{
	return LinxDev->I2cClose(channel);
}

		
//------------------------------------- PWM -------------------------------------
unsigned char LinxPwmGetNumChans()
{
	return LinxDev->NumPwmChans;
}

int LinxPwmGetChans(unsigned char numChans, unsigned char* channels)
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

int LinxPwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->PwmSetDutyCycle(numChans, channels, values);
}


//------------------------------------- SPI -------------------------------------
unsigned char LinxSpiGetNumChans()
{
	return LinxDev->NumSpiChans;
}

int LinxSpiGetChans(unsigned char numChans, unsigned char* channels)
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

int LinxSpiOpenMaster(unsigned char channel)
{
	return LinxDev->SpiOpenMaster(channel);
}

int LinxSpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	return LinxDev->SpiSetBitOrder(channel, bitOrder);
}

int LinxSpiSetMode(unsigned char channel, unsigned char mode)
{
	return LinxDev->SpiSetMode(channel, mode);
}

int LinxSpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return LinxDev->SpiSetSpeed(channel, speed, actualSpeed);
}

int LinxSpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	return LinxDev->SpiWriteRead(channel, frameSize, numFrames, csChan, csLL, sendBuffer, recBuffer);
}

		
//------------------------------------- UART -------------------------------------
unsigned char LinxUartGetNumChans()
{
	return LinxDev->NumUartChans;
}

int LinxUartGetChans(unsigned char numChans, unsigned char* channels)
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

int LinxUartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	return LinxDev->UartOpen(channel, baudRate, actualBaud);
}
int LinxUartSetBaudRate(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	return LinxDev->UartSetBaudRate(channel, baudRate, actualBaud);
}

int LinxUartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
{
	return LinxDev->UartGetBytesAvailable(channel, numBytes);
}

int LinxUartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	return LinxDev->UartRead(channel, numBytes, recBuffer, numBytesRead);
}

int LinxUartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	return LinxDev->UartWrite(channel, numBytes, sendBuffer);
}

int LinxUartClose(unsigned char channel)
{
	return LinxDev->UartClose(channel);
}
