/****************************************************************************************
**  LINX - Wiring compatible device code
**
**  For more information see:           www.labviewhacker.com/linx
**  For support visit the forums at:    www.labviewhacker.com/forums/linx
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
#include "LinxWiringDevice.h"

#if ARDUINO_VERSION >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Servo.h>

/****************************************************************************************
**  Variables
****************************************************************************************/		

/****************************************************************************************
**  Constructors / Destructors 
****************************************************************************************/
LinxWiringDevice::LinxWiringDevice()
{
	//LINX API Version
	LinxApiMajor = 1;
	LinxApiMinor = 2;
	LinxApiSubminor = 0;
	
	//Load User Config Data From Non Volatile Storage
	userId = NonVolatileRead(NVS_USERID) << 8 | NonVolatileRead(NVS_USERID + 1);
	
}


/****************************************************************************************
**  Functions
****************************************************************************************/

void LinxWiringDevice::DelayMs(unsigned long ms)
{
	delay(ms);
} 

unsigned long LinxWiringDevice::GetMilliSeconds()
{
	return millis();
}

unsigned long LinxWiringDevice::GetSeconds()
{
	return (millis() / 1000);
}

//--------------------------------------------------------ANALOG-------------------------------------------------------

int LinxWiringDevice::AnalogRead(unsigned char numPins, unsigned char* pins, unsigned char* values)
{
	unsigned int analogValue = 0;
	unsigned char responseByteOffset = 0;
	unsigned char responseBitsRemaining = 8; 
	unsigned char dataBitsRemaining = AiResolution;
  
	values[responseByteOffset] = 0x00;    //Clear First	Response Byte   

	//Loop Over All AI Pins In Command Packet
	for(int i=0; i<numPins; i++)
	{
		analogValue = analogRead(pins[i]);	
		
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

int LinxWiringDevice::AnalogSetRef(unsigned char mode, unsigned long voltage)
{
	switch(mode)
	{
		case 0: //Default
			analogReference(DEFAULT);
			AiRefSet = AiRefDefault;
			break;
		case 1: //Internal
			if(NumAiRefIntVals > 0)
			{
				//Check If Internal AI Ref Value Is Supported
				for(int i=0; i<NumAiRefIntVals; i++)
				{				
					//Voltage Is Supported
					if(AiRefIntVals[i] == voltage)
					{
						analogReference(AiRefCodes[i]);
						AiRefSet = voltage;
						return L_OK;
					}
				}
				//Didn't Find Voltage
				return LANALOG_REF_VAL_ERROR;
			}
			else
			{
				//No Internal Voltages, So Internal Mode Not Supported
				return LANALOG_REF_MODE_ERROR;
			}			
			break;
		case 2: //External
			if(voltage >= AiRefExtMin && voltage <= AiRefExtMax)
			{
				analogReference(EXTERNAL);
				AiRefSet = voltage;
				return L_OK;
			}
			else
			{
				return LANALOG_REF_VAL_ERROR;
			}
			break;
		default:
			return LANALOG_REF_MODE_ERROR;
			break;	
	}
	return L_OK;
}

//--------------------------------------------------------DIGITAL-------------------------------------------------------

int LinxWiringDevice::DigitalWrite(unsigned char numPins, unsigned char* pins, unsigned char* values)
{
	for(int i=0; i<numPins; i++)
	{		
		pinMode(pins[i], OUTPUT);
		digitalWrite( pins[i], (values[i/8] >> i%8) & 0x01);
	}
	
	return L_OK;
}

int LinxWiringDevice::DigitalRead(unsigned char numPins, unsigned char* pins, unsigned char* values)
{
	unsigned char bitOffset = 8;
	unsigned char byteOffset = 0;
	unsigned char retVal = 0;
 
	//Loop Over Pins To Read
	for(int i=0; i<numPins; i++)
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
		
		//Read From Next Pin
		unsigned char pinNumber = pins[i];
			
		pinMode(pinNumber, INPUT);											//Set Pin As Input (Might Make This Configurable)    		
		retVal = retVal | (digitalRead(pinNumber) << bitOffset);	//Read Pin And Insert Value Into retVal
	}
	
	//Store Last Byte
	values[byteOffset] = retVal;
	
	return L_OK;
}


int LinxWiringDevice::DigitalWriteSquareWave(unsigned char channel, unsigned long freq, unsigned long duration)
{
	if(freq > 0)
	{
		pinMode(channel, OUTPUT);
		if(duration > 0)
		{
			tone(channel, freq, duration);
		}
		else
		{
			tone(channel, freq);
		}
	}
	else
	{
		noTone(channel);
	}	
	
	return L_OK;
}

int LinxWiringDevice::DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned long timeout, unsigned long* width)
{
	//Stimulus
	if(stimType == 1)
	{
		//High->Low->High
		pinMode(stimChan, OUTPUT);
		
		digitalWrite(stimChan, HIGH);
		delay(1);
		digitalWrite(stimChan, LOW);
		delay(1);
		digitalWrite(stimChan, HIGH);		
	}
	else if(stimType == 2)
	{
		//Low->High->Low
		pinMode(stimChan, OUTPUT);
		
		digitalWrite(stimChan, LOW);
		delay(1);
		digitalWrite(stimChan, HIGH);
		delay(1);
		digitalWrite(stimChan, LOW);		
	}
	
	//Read Pulse
	pinMode(respChan, INPUT);
	
	if(respType == 0)
	{
		*width = pulseIn(respChan, LOW, timeout);
	}
	else if(respType == 1)
	{
		*width = pulseIn(respChan, HIGH, timeout);
	}	
	
	return L_OK;	
}

//--------------------------------------------------------PWM-----------------------------------------------------------

int LinxWiringDevice::PwmSetDutyCycle(unsigned char numPins, unsigned char* pins, unsigned char* values)
{
	for(int i=0; i<numPins; i++)
	{		
		pinMode(pins[i], OUTPUT);
		analogWrite(pins[i], values[i]);
	}
	
	return L_OK;
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
	//Loop Over All Supported SPI Speeds (SPI Speeds Should Be Fastest -> Slowest)
	for(int index=0; index < NumSpiSpeeds; index++)
	{
			//If Target Speed Is greater or equal to the current supported speed use current supported speed (it's the fastest supported speed that is less or equal to the target)
			if(speed >= *(SpiSupportedSpeeds+index))
			{
				*actualSpeed = *(SpiSupportedSpeeds+index);
				SPI.setClockDivider(*(SpiSpeedCodes+index));
				break;
			}
			if(index == NumSpiSpeeds-1)
			{
				//Target speed is slower than slowest supported.  Use slowest supported
				*actualSpeed = *(SpiSupportedSpeeds+index);
				SPI.setClockDivider(*(SpiSpeedCodes+index));
			}
	}
	
	return L_OK;
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
	return L_OK;
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
			Wire.requestFrom(slaveAddress, numBytes, (uint8_t)0);
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
		Wire.requestFrom(slaveAddress, (uint8_t)numBytes);
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
	UartClose(channel);
	int retVal = UartOpen(channel, baudRate, actualBaud);
	return retVal;
}

int LinxWiringDevice::UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes)
{
	if(channel == 0)
	{		
		#if NUM_UART_CHANS > 0
			*numBytes = Serial.available();
		#endif
	}
	if(channel == 1)
	{
		#if NUM_UART_CHANS > 1
			*numBytes = Serial1.available();
		#endif
	}
	if(channel == 2)
	{
		#if NUM_UART_CHANS > 2
		*numBytes = Serial2.available();
		#endif
	}
	if(channel == 3)
	{
		#if NUM_UART_CHANS > 3
			*numBytes = Serial3.available();
		#endif
	}
	
	return L_OK;
}

int LinxWiringDevice::UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead)
{
	#if ARDUINO_VERSION >= 100
	
		if(channel == 0)
		{	
			#if NUM_UART_CHANS > 0
				*numBytesRead = Serial.readBytes((char*)recBuffer, numBytes);
			#endif
		}
		else if(channel == 1)
		{
			#if NUM_UART_CHANS > 1
				*numBytesRead = Serial1.readBytes((char*)recBuffer, numBytes);
			#endif
		}
		else if(channel == 2)
		{
			#if NUM_UART_CHANS > 2
				*numBytesRead = Serial2.readBytes((char*)recBuffer, numBytes);
			#endif
		}
		else if(channel == 3)
		{
			#if NUM_UART_CHANS > 3
				*numBytesRead = Serial3.readBytes((char*)recBuffer, numBytes);
			#endif
		}	
				
		if(*numBytesRead !=numBytes)
		{
			return LUART_READ_FAIL;
		}
		
		return L_OK;	
	
	#else
		for(int i=0; i<numBytes; i++)
		{
			int data = -1;
			
			if(channel == 0)
			{	
				#if NUM_UART_CHANS > 0
					data = Serial.read();
				#endif
			}
			else if(channel == 1)
			{
				#if NUM_UART_CHANS > 1
					data = Serial1.read();
				#endif
			}
			else if(channel == 2)
			{
				#if NUM_UART_CHANS > 2
					data = Serial2.read();
				#endif
			}
			else if(channel == 3)
			{
				#if NUM_UART_CHANS > 3
					data = Serial3.read();
				#endif
			}			
			
			if(data < 0)
			{
				return LUART_READ_FAIL;
			}
			else
			{
				*(recBuffer+i) = (char)data;
			}
			
			//Read All Bytes Without Error.  Return Num Bytes Read So Listener Can Pass It To PacketizeAndSend()
			*numBytesRead = numBytes;
		}
		
		return L_OK;
	#endif
}

int LinxWiringDevice::UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer)
{
	if(channel == 0)
	{		
		#if NUM_UART_CHANS > 0
			Serial.write(sendBuffer, numBytes);
		#endif
	}
	if(channel == 1)
	{
		#if NUM_UART_CHANS > 1
			Serial1.write(sendBuffer, numBytes);
		#endif
	}
	if(channel == 2)
	{
		#if NUM_UART_CHANS > 2
			Serial2.write(sendBuffer, numBytes);
		#endif
	}
	if(channel == 3)
	{
		#if NUM_UART_CHANS > 3
			Serial3.write(sendBuffer, numBytes);
		#endif
	}
	
	return L_OK;
}

int LinxWiringDevice::UartClose(unsigned char channel)
{
	if(channel == 0)
	{		
		#if NUM_UART_CHANS > 0
			Serial.end();
		#endif
	}
	if(channel == 1)
	{
		#if NUM_UART_CHANS > 1
			Serial1.end();
		#endif
	}
	if(channel == 2)
	{
		#if NUM_UART_CHANS > 2
		Serial2.end();
		#endif
	}
	if(channel == 3)
	{
		#if NUM_UART_CHANS > 3
			Serial3.end();
		#endif
	}
	return L_OK;
}

//--------------------------------------------------------SERVO----------------------------------------------------------
int LinxWiringDevice::ServoOpen(unsigned char numChans, unsigned char* chans)
{
	for(int i=0; i<numChans; i++)
	{
		unsigned char pin = chans[i];
		if(Servos[pin] == 0)
		{
			//Servo Not Yet Intialized On Specified Channel, Init
			Servos[pin] = new Servo();
			Servos[pin]->attach(pin);
			
			DebugPrint("Created New Servo On Channel ");
			DebugPrintln(pin, DEC);
		}
	}
	return L_OK;
}

int LinxWiringDevice::ServoSetPulseWidth(unsigned char numChans, unsigned char* chans, unsigned short* pulseWidths)
{
	
	for(int i=0; i<numChans; i++)
	{	
		
		DebugPrint("Servo ");
		DebugPrint((unsigned long)Servos[chans[i]], DEC);
		DebugPrint(" : ");
		DebugPrintln(pulseWidths[i], DEC);
		Servos[chans[i]]->writeMicroseconds(pulseWidths[i]);		
	}
	
	return L_OK;
}



int LinxWiringDevice::ServoClose(unsigned char numChans, unsigned char* chans)
{
	for(int i=0; i<numChans; i++)
	{
		Servos[chans[i]]->detach();
		Servos[chans[i]] = 0;
	}
	return L_OK;
}
		

//--------------------------------------------------------GENERAL----------------------------------------------------------
void LinxWiringDevice::NonVolatileWrite(int address, unsigned char data)
{
	EEPROM.write(address, data);
}

unsigned char LinxWiringDevice::NonVolatileRead(int address)
{
	return EEPROM.read(address);
}
