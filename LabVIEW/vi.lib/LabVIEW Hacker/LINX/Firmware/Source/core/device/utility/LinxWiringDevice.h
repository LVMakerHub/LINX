#ifndef LINX_WIRING_H
#define LINX_WIRING_H

/****************************************************************************************
**  Includes
****************************************************************************************/		

#if DEVICE_FAMILY == 2	//Arduino
	#include <Arduino.h>
#endif

#if DEVICE_FAMILY == 1	//ChipKIT
	#include <WProgram.h>
#endif

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
		unsigned char NumUartSpeeds;						///Number Of Support UART Buads
		unsigned long* UartSupportedSpeeds;				//Supported UART Bauds Frequencies
		
		unsigned char NumSpiSpeeds;						//Number Of Supported SPI Speeds
		unsigned long* SpiSupportedSpeeds;				//Supported SPI Clock Frequencies
		int* SpiSpeedCodes;										//SPI Speed Values (Clock Divider Macros In Wiring Case)
		
		unsigned char* I2cRefCount;							//Number Opens - Closes On I2C Channel
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxWiringDevice();
			
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		
		//DIGITAL
		virtual int DigitalWrite(unsigned char numPins, unsigned char* pins, unsigned char* values);
		
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
		
		//Debug
		virtual void DebugPrint(const char[]);
		virtual void DebugPrint(unsigned char numBytes, const char* message);
		virtual void DebugPrintPacket(unsigned char direction, const unsigned char* packetBuffer);
		
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
