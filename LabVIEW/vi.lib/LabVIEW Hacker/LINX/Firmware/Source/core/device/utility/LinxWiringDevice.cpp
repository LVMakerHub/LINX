/****************************************************************************************
**  Defines
****************************************************************************************/		

/****************************************************************************************
**  Includes
****************************************************************************************/	
#if DEVICE_FAMILY == 2	//Arduino
	#include <Arduino.h>
#endif

#if DEVICE_FAMILY == 1	//ChipKIT
	#include <WProgram.h>
#endif

#include <SPI.h>
#include <Wire.h>
//#include <Serial.h>

#include "LinxDevice.h"
#include "LinxWiringDevice.h"

//This Makes It Easy For IDE Users To Define Necissary Settings In One Place
//When Using Make Files Define LINXCONFIG To Ignore Config.h File
#ifndef LINXCONFIG
	#include "../LinxConfig.h"
#endif

/****************************************************************************************
**  Variables
****************************************************************************************/		

/****************************************************************************************
**  Constructors / Destructors 
****************************************************************************************/
LinxWiringDevice::LinxWiringDevice( )
{
	//LINX API Version
	LinxApiMajor = 1;
	LinxApiMinor = 2;
	LinxApiSubminor = 0;	
}


/****************************************************************************************
**  Functions
****************************************************************************************/
void LinxWiringDevice::DelayMs(unsigned long ms)
{
	delay(ms);
} 
		
//Debug
void LinxWiringDevice::DebugPrint(unsigned char numBytes, const char* message)
{	
	#if DEBUG_ENABLED == 1
		for(int i=0; i<numBytes; i++)
		{			
			Serial1.print(message[i]);
		}
		Serial1.print("\n\r");
	#endif 
}

void LinxWiringDevice::DebugPrint(const char *s)
{		
	#if DEBUG_ENABLED == 1
		Serial1.print(s);
	#endif 
}

void LinxWiringDevice::DebugPrintPacket(unsigned char direction, const unsigned char* packetBuffer)
{
	#if DEBUG_ENABLED == 1
		if(direction == RX)
		{
			Serial1.print("Received :: ");
		}
		else if(direction == TX)
		{
			Serial1.print("Sending  :: ");
		}
		for(int i=0; i<packetBuffer[1]; i++)
		{			
			Serial1.print("[");
			Serial1.print(packetBuffer[i], HEX);
			Serial1.print("]");
		}
		Serial1.print("\n\r");
		if(direction == TX)
		{
			Serial1.print("\n\r");	//Print Extra New Line After TX
		}
	#endif
}

unsigned long LinxWiringDevice::GetMilliSeconds()
{
	return millis();
}

unsigned long LinxWiringDevice::GetSeconds()
{
	return (millis() / 1000);
}
//--------------------------------------------------------DIGITAL-------------------------------------------------------

int LinxWiringDevice::DigitalWrite(unsigned char numPins, unsigned char* pins, unsigned char* values)
{
	for(int i=0; i<numPins; i++)
	{		
		pinMode(pins[i], OUTPUT);
		digitalWrite( pins[i], (values[i/8] >> i%8) & 0x01);
	}
	
	return 0;
}

//--------------------------------------------------------SPI-----------------------------------------------------------
int LinxWiringDevice::SpiOpenMaster(unsigned char channel)
{
	SPI.begin();
	return 0;
}

int LinxWiringDevice::SpiSetBitOrder(unsigned char channel, unsigned char bitOrder)
{
	SPI.setBitOrder(bitOrder);
	return 0;
}

int LinxWiringDevice::SpiSetMode(unsigned char channel, unsigned char mode)
{
	 SPI.setDataMode(mode);
	return 0;
}

int LinxWiringDevice::SpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
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

			SPI.setClockDivider(*(SpiSupportedSpeeds+index));
	}
	return 0;
}

int LinxWiringDevice::SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer)
{
	//Set CS Pin As DO
	pinMode(csChan, OUTPUT);
	
	//Set CS Pin Before Starting SPI Transfer
	digitalWrite(csChan, (~csLL & 0x01) );  

	//Loop Over Frames
	for(int i=0; i<numFrames; i++)
	{
		//Loop Over Bytes In Frame
		for(int j=0; j<frameSize; j++)
		{
			//Transfer Data
			unsigned char byteNum = (i*frameSize) + j;
			recBuffer[byteNum] = SPI.transfer(sendBuffer[byteNum]);
		}
		
		//Frame Complete, Toggle CS
		digitalWrite(csChan, (csLL & 0x01) );
	}		
	return 0;
}

//--------------------------------------------------------I2C-----------------------------------------------------------

//Helper To Deal With Arduino API Changes
void LinxWireWrite(unsigned char data)
{
  #if ARDUINO_VERSION < 100
    Wire.send(data);
  #else
    Wire.write(data);
  #endif 
}

int LinxWiringDevice::I2cOpenMaster(unsigned char channel)
{
	if(*(I2cRefCount+channel) > 0)
	{
		//Channel Already Open, Increment Ref Count
		*(I2cRefCount+channel) = *(I2cRefCount+channel)+1;	
	}
	else
	{
		//Channel Not Yet Open, Open And Set Refcount = 1
		 Wire.begin();			//TODO ONLY SUPPORT ONE CHANNEL ATM
	}
	return 0;
}

int LinxWiringDevice::I2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed)
{
	return 0;
}

int LinxWiringDevice::I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer)
{ 
	#if ARDUINO_VERSION >= 100
		Wire.beginTransmission(slaveAddress);
		Wire.write(sendBuffer, numBytes);
		
		if(eofConfig == EOF_STOP)
		{
			Wire.endTransmission(true);
		}
		else if (eofConfig == EOF_RESTART)
		{
			Wire.endTransmission(false);
		}
		else
		{
			//EOF Not Supported, Stop Bus
			Wire.endTransmission(true);
			return LI2C_EOF;
		}	
	return L_OK;
	#else
		if(eofConfig != EOF_STOP)
		{
			//EOF Not Supported, Stop Bus
			return LI2C_EOF;
		}
		Wire.beginTransmission(slaveAddress);
		for(int i=0; i<numBytes; i++)
		{
			Wire.send(*(sendBuffer+i));
		}
		Wire.endTransmission();
		return 0;
	#endif	
}

int LinxWiringDevice::I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer)
{
	#if ARDUINO_VERSION >= 100
		if(eofConfig == EOF_STOP)
		{
			Wire.requestFrom(slaveAddress, numBytes, (uint8_t)1);
		}
		else if (eofConfig == EOF_RESTART)
		{
			Wire.requestFrom(slaveAddress, numBytes, 0);
		}
		else
		{
			//EOF Not Supported		
			return LI2C_EOF;
		}
	#else
		if(eofConfig != EOF_STOP)
		{
			//EOF Not Supported		
			return LI2C_EOF;
		}
		Wire.requestFrom(slaveAddress, numBytes);
	#endif
		
		//Wait For Data, Potentially Timeout
		unsigned long tickCount = millis();
		while(Wire.available() < numBytes)
		{
			if( (millis() - tickCount) > timeout)
			{
				return LI2C_READ_FAIL;
			}
		}
		
		//Data Read, Read And Return
		for(int i=0; i<numBytes; i++)
		{
			#if ARDUINO_VERSION >= 100
				*(recBuffer+i) = Wire.read();
			#else
				*(recBuffer+i) = Wire.receive();
			#endif			
		}	
		return L_OK;
}

int LinxWiringDevice::I2cClose(unsigned char channel)
{
	//Function Not Supported
	return L_FUNCTION_NOT_SUPPORTED;
}
		
//--------------------------------------------------------UART----------------------------------------------------------
int LinxWiringDevice::UartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	int index = 0;
	
	for(index=0; index < NumUartSpeeds; index++)
	{
			if(baudRate < *(UartSupportedSpeeds+index))
			{		
				//Previous Index Was Closest Supported Baud Without Going Over
				break;
			}			
	}
	
	//Once Loop Complets Index Is One Higher Than The Correct Baud, But Could Be Zero So Check And Decrement Accordingly
	//If The Entire Loop Runs Then index == NumUartSpeeds So Decrement It To Get Max Baud...Is This Specific To gcc-pic32?
	if(index != 0)
	{
		index = index -1;
	}
	if(channel == 0)
	{		
		#if NUM_UART_CHANS > 0
			Serial.begin(*(UartSupportedSpeeds+index));
			*actualBaud = *(UartSupportedSpeeds+index);
		#endif
	}
	if(channel == 1)
	{
		#if NUM_UART_CHANS > 1
			Serial1.begin(*(UartSupportedSpeeds+index));
			*actualBaud = *(UartSupportedSpeeds+index);
		#endif
	}
	if(channel == 2)
	{
		#if NUM_UART_CHANS > 2
			Serial2.begin(*(UartSupportedSpeeds+index));
			*actualBaud = *(UartSupportedSpeeds+index);
		#endif
	}
	if(channel == 3)
	{
		#if NUM_UART_CHANS > 3
			Serial3.begin(*(UartSupportedSpeeds+index));
			*actualBaud = *(UartSupportedSpeeds+index);
		#endif
	}
	
	return L_OK;
}

int LinxWiringDevice::UartSetBaudRate(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud)
{
	Serial.end();
	int retVal = UartOpen(channel, baudRate, actualBaud);
	return retVal;
}

int LinxWiringDevice::UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
{
	*numBytes = Serial.available();
	return 0;
}

int LinxWiringDevice::UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	#if ARDUINO_VERSION >= 100
		*numBytesRead = Serial.readBytes((char*)recBuffer, numBytes);
		
		if(*numBytesRead !=numBytes)
		{
			return LUART_READ_FAIL;
		}
		return 0;
	#else
		for(int i=0; i<numBytes; i++)
		{
			int data = Serial.read();
			if(data < 0)
			{
				return LUART_READ_FAIL;
			}
			else
			{
				*(recBuffer+i) = (char)data;
			}
		}
		return 0;
	#endif
}

int LinxWiringDevice::UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	#if ARDUINO_VERSION >= 100
		unsigned char bytesSent = Serial.write(sendBuffer, numBytes);
		if(bytesSent != numBytes)
		{
			return LUART_WRITE_FAIL;
		}
	#else
		Serial.write(sendBuffer, numBytes);
	#endif	
	return 0;
}

int LinxWiringDevice::UartClose(unsigned char channel)
{
	Serial.end();
	return 0;
}
