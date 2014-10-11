#ifndef LINX_WIRING_H
#define LINX_WIRING_H

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include "LinxDevice.h"

/****************************************************************************************
**  Variables
****************************************************************************************/		

class LinxWiringDevice : public LinxDevice
{
	public:	
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		unsigned char NumUartSpeeds;					//Number Of Support UART Buads
		unsigned long* UartSupportedSpeeds;				//Supported UART Bauds Frequencies
		
		unsigned char NumSpiSpeeds;						//Number Of Supported SPI Speeds
		unsigned long* SpiSupportedSpeeds;				//Supported SPI Clock Frequencies
		int* SpiSpeedCodes;								//SPI Speed Values (Clock Divider Macros In Wiring Case)
		
		unsigned char* I2cRefCount;						//Number Opens - Closes On I2C Channel
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxWiringDevice();
			
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		
		//Analog
		virtual int AnalogRead(unsigned char numPins, unsigned char* pins, unsigned char* values);
		
		//DIGITAL
		virtual int DigitalWrite(unsigned char numPins, unsigned char* pins, unsigned char* values);
		virtual int DigitalRead(unsigned char numPins, unsigned char* pins, unsigned char* values);
		
		//PWM
		virtual int PwmSetDutyCycle(unsigned char numPins, unsigned char* pins, unsigned char* values);
		
		//SPI
		virtual int SpiOpenMaster(unsigned char channel);
		virtual int SpiSetBitOrder(unsigned char channel, unsigned char bitOrder);
		virtual int SpiSetMode(unsigned char channel, unsigned char mode);
		virtual int SpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed);
		virtual int SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
		
		//I2C
		virtual int I2cOpenMaster(unsigned char channel);
		virtual int I2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed);
		virtual int I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer);
		virtual int I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer);		
		virtual int I2cClose(unsigned char channel);
		
		//UART
		virtual int UartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud);
		virtual int UartSetBaudRate(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud);
		virtual int UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes);
		virtual int UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead);
		virtual int UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer);
		virtual int UartClose(unsigned char channel);
		
		//General - 
		virtual unsigned long GetMilliSeconds();
		virtual unsigned long GetSeconds();
		virtual void DelayMs(unsigned long ms);
		virtual void NonVolatileWrite(int address, unsigned char data);
		virtual unsigned char NonVolatileRead(int address);
		
		//Debug
		
		
	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
				
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		
	private:
	/****************************************************************************************
	**  Functions
	****************************************************************************************/
	void LinxWireWrite();
				
};


#endif //LINX_WIRING_H
