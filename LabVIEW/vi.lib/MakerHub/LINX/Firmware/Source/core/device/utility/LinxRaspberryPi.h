/****************************************************************************************
**  LINX header for Raspberry Pi
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_RASPBERRYPI_H
#define LINX_RASPBERRYPI_H

/****************************************************************************************
**  Defines
****************************************************************************************/		
#define DIGITAL_PIN_LEN 3
#define SPI_PATH_LEN 64
#define I2C_PATH_LEN 64
#define UART_PATH_LEN 64

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include "LinxDevice.h"


/****************************************************************************************
**  Variables
****************************************************************************************/		

class LinxRaspberryPi : public LinxDevice
{
	public:	
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		//DIO
		int* DigitalDirHandles;								//File Handles For Digital Pin Directions
		int* DigitalValueHandles;							//File Handles For Digital Pin Values
		
		unsigned char NumAiRefIntVals;					//Number Of Internal AI Reference Voltages
		const unsigned long* AiRefIntVals;				//Supported AI Reference Voltages (uV)
		const int* AiRefCodes;								//AI Ref Values (AI Ref Macros In Wiring Case)
		
		unsigned long AiRefExtMin;							//Min External AI Ref Value (uV)
		unsigned long AiRefExtMax;					    //Max External AI Ref Value (uV)		
		
		unsigned char NumUartSpeeds;					//Number Of Support UART Buads
		unsigned long* UartSupportedSpeeds;			//Supported UART Bauds Frequencies
		
		unsigned char NumSpiSpeeds;					//Number Of Supported SPI Speeds
		unsigned long* SpiSupportedSpeeds;			//Supported SPI Clock Frequencies
		int* SpiSpeedCodes;									//SPI Speed Values (Clock Divider Macros In Wiring Case)
		
		unsigned char* I2cRefCount;						//Number Opens - Closes On I2C Channel
		
		//Servo** Servos;										//Array Servo Pointers
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxRaspberryPi();
		~LinxRaspberryPi();
		
			
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		
		//Analog
		virtual int AnalogRead(unsigned char numPins, unsigned char* pins, unsigned char* values);
		virtual int AnalogSetRef(unsigned char mode, unsigned long voltage);
		
		//DIGITAL
		virtual int DigitalWrite(unsigned char numPins, unsigned char* pins, unsigned char* values);
		virtual int DigitalRead(unsigned char numPins, unsigned char* pins, unsigned char* values);
		virtual int DigitalWriteSquareWave(unsigned char channel, unsigned long freq, unsigned long duration);
		virtual int DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned long timeout, unsigned long* width);
		
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
		
		//Servo
		virtual int ServoOpen(unsigned char numChans, unsigned char* chans);
		virtual int ServoSetPulseWidth(unsigned char numChans, unsigned char* chans, unsigned short* pulseWidths);
		virtual int ServoClose(unsigned char numChans, unsigned char* chans);		
		
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
	virtual int GpioExport (const unsigned char numGpioChans, const unsigned char*  gpioChans, int* digitalDirHandles, int* digitalValueHandles);
	virtual int GpioUnexport(const unsigned char*  gpioChans, const unsigned char numGpioChans);
	virtual int GpioSetDir(unsigned char pin, unsigned char mode);
	virtual int GpioWrite(unsigned char pin, unsigned char val);
	virtual int GetDigitalChanIndex(unsigned char chanNum);
			
	private:
	/****************************************************************************************
	**  Functions
	****************************************************************************************/
	
				
};


#endif //LINX_RASPBERRYPI_H