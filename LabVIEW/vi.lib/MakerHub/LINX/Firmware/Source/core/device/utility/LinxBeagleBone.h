/****************************************************************************************
**  LINX header for BeagleBone family
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_BEAGLEBONE_H
#define LINX_BEAGLEBONE_H

/****************************************************************************************
**  Defines
****************************************************************************************/		
#define DIGITAL_PIN_LEN 3
#define PWM_PATH_LEN 64
#define PWM_DTO_NAME_LEN 32
#define AI_PATH_LEN 64
#define SPI_PATH_LEN 64
#define I2C_PATH_LEN 64
#define UART_PATH_LEN 64

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include "LinxDevice.h"
#include <stdio.h>
#include <map>
#include <string>

using namespace std;

/****************************************************************************************
**  Variables
****************************************************************************************/		

class LinxBeagleBone : public LinxDevice
{
	public:	
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		//DIO
		map<unsigned char, FILE*> DigitalDirHandles;							//File Handles For Digital Pin Directions
		map<unsigned char, FILE*> DigitalValueHandles;						//File Handles For Digital Pin Values
		
		//PWM
		map<unsigned char, FILE*> PwmPeriodHandles;						//File Handles For PWM Period Values
		map<unsigned char, FILE*> PwmDutyCycleHandles;				//File Handles For PWM Duty Cycle Values		
		const char (*PwmDirPaths)[PWM_PATH_LEN];							//Path To PWM Directories
		const char (*PwmDtoNames)[PWM_DTO_NAME_LEN];				//PWM Device Tree Overlay Names
		
		unsigned char NumAiRefIntVals;												//Number Of Internal AI Reference Voltages
		const unsigned long* AiRefIntVals;											//Supported AI Reference Voltages (uV)
		const int* AiRefCodes;															//AI Ref Values (AI Ref Macros In Wiring Case)
		
		unsigned long AiRefExtMin;														//Min External AI Ref Value (uV)
		unsigned long AiRefExtMax;					   								 //Max External AI Ref Value (uV)		
		
		int* AiHandles;																		//AI File Handles
		const char (*AiPaths)[AI_PATH_LEN];										//AI Channel File Paths
		
		unsigned char NumUartSpeeds;												//Number Of Support UART Buads
		unsigned long* UartSupportedSpeeds;										//Supported UART Bauds Frequencies
		const char (*UartPaths)[UART_PATH_LEN];  							//UART Channel File Paths
		int* UartHandles;																	//UART File Handles
		unsigned long* UartSupportedSpeedsCodes;							//Supported UART Baud Divider Codes
		
		map<unsigned char, string> SpiDtoNames;  							//Device Tree Overlay Names For SPI Master(s)
		map<unsigned char, string> SpiPaths;  									//File Paths For SPI Master(s)		
		map<unsigned char, int> SpiHandles;										//File Handles For SPI Master(s)
		unsigned char NumSpiSpeeds;												//Number Of Supported SPI Speeds
		unsigned long* SpiSupportedSpeeds;										//Supported SPI Clock Frequencies
		int* SpiSpeedCodes;																//SPI Speed Values (Clock Divider Macros In Wiring Case)
		map<unsigned char, unsigned char> SpiBitOrders;					//Stores Bit Orders For SPI Channels (LSBFIRST / MSBFIRST)
		map<unsigned char, unsigned long> SpiSetSpeeds; 				//Stores The Set Clock Rate Of Each SPI Channel
		
		map<unsigned char, string> I2cPaths;										//File Paths For I2C Master(s)
		map<unsigned char, int> I2cHandles;										//File Handles For I2C Master(s)
		map<unsigned char, string> I2cDtoNames;								//Device Tree Overlay Names For I2C Master(s)
		unsigned char* I2cRefCount;													//Number Opens - Closes On I2C Channel
		
		
		
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxBeagleBone();
		~LinxBeagleBone();
		
			
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		//Analog
		virtual int AnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int AnalogSetRef(unsigned char mode, unsigned long voltage);
		
		//DIGITAL
		virtual int DigitalSetDirection(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int DigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int DigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
		virtual int DigitalWriteSquareWave(unsigned char channel, unsigned long freq, unsigned long duration);
		virtual int DigitalReadPulseWidth(unsigned char stimChan, unsigned char stimType, unsigned char respChan, unsigned char respType, unsigned long timeout, unsigned long* width);
		
		//PWM
		virtual int PwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values);
		
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
		
	protected:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
				
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		virtual int digitalSmartOpen(unsigned char numChans, unsigned char* channels);
		bool fileExists(const char* path);
		bool fileExists(const char* directory, const char* fileName);
		bool fileExists(const char* directory, const char* fileName, unsigned long timout);
		bool loadDto(const char* dtoName);
};
		
#endif //LINX_BEAGLEBONE_H