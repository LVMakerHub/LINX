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

LinxRaspberryPi2B* LinxDev;

//------------------------------------- Constructor / Destructor -------------------------------------
int LinxOpen()
{
	//Instantiate The LINX Device
	LinxDev = new LinxRaspberryPi2B();
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

int LinxDigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalRead(numChans, channels, values);
}


//------------------------------------- I2C -------------------------------------
unsigned char LinxI2cGetNumChans()
{
	return LinxDev->NumI2cChans;
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

int LinxPwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->PwmSetDutyCycle(numChans, channels, values);
}


//------------------------------------- SPI -------------------------------------
unsigned char LinxSpiGetNumChans()
{
	return LinxDev->NumSpiChans;
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
//Use VISA
