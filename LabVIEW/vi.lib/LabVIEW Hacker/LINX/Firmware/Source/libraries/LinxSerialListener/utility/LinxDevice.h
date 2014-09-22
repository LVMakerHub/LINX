#ifndef LINX_DEVICE_H
#define LINX_DEVICE_H

/****************************************************************************************
**  Includes
****************************************************************************************/		

/****************************************************************************************
**  Defines
****************************************************************************************/		
//GPIO
#ifndef INPUT
	#define INPUT 0x00
#endif

#ifndef  OUTPUT
	#define OUTPUT 0x01
#endif

#ifndef HIGH
	#define HIGH 0x01
#endif

#ifndef LOW
	#define LOW 0x00
#endif

//SPI
#ifndef LSBFIRST
	#define LSBFIRST 0
#endif

#ifndef MSBFIRST
	#define MSBFIRST 1
#endif

//I2C
#define EOF_STOP 0
#define EOF_RESTART 1
#define EOF_RESTART_NOSTOP 2
#define EOF_NOSTOP 3

//DEBUG
#define TX 0
#define RX 1

/****************************************************************************************
**  Typedefs
****************************************************************************************/		
enum LinxStatus {L_OK, L_FUNCTION_NOT_SUPPORTED, L_REQUEST_RESEND, L_UNKNOWN_ERROR, L_DISCONNECT};

typedef enum DioStatus
{
	LDIGITAL_PIN_DNE=128, 
}DioStatus;


typedef enum SPIStatus
{
	
}SPIStatus;

typedef enum I2CStatus
{
	LI2C_SADDR=128, 
	LI2C_EOF, 
	LI2C_WRITE_FAIL, 	
	LI2C_READ_FAIL, 
	LI2C_CLOSE_FAIL,
	LI2C_OPEN_FAIL
}I2CStatus;

typedef enum UartStatus
{
	LUART_OPEN_FAIL=128, 
	LUART_SET_BAUD_FAIL, 
	LUART_AVAILABLE_FAIL, 
	LUART_READ_FAIL, 
	LUART_WRITE_FAIL, 
	LUART_CLOSE_FAIL
}UartStatus;

class LinxDevice
{
	public:	
		/****************************************************************************************
		**  Member Variables
		****************************************************************************************/				
		
		//Device ID
		unsigned char DeviceFamily;
		unsigned char DeviceID;
		unsigned char DeviceNameLen;
		const unsigned char* DeviceName;
		
		//LINX API Version
		unsigned char LinxApiMajor;
		unsigned char LinxApiMinor;
		unsigned char LinxApiSubminor;
		
		//----Peripherals----
		
		//DIO		
		unsigned char NumDigitalChans;
		const unsigned char*  DigitalChans;
		
		//AI
		unsigned char NumAiChans;
		const unsigned char* AiChans;
		unsigned char AiResolution;
		unsigned long AiRef;
		
		//AO
		unsigned char NumAoChans;
		const unsigned char* AoChans;
		
		//PWM
		unsigned char NumPwmChans;
		const unsigned char* PwmChans;
		
		//QE
		unsigned char NumQeChans;
		const unsigned char* QeChans;
		
		//UART
		unsigned char NumUartChans;
		const unsigned char* UartChans;
		unsigned long UartMaxBaud;
		
		//I2C
		unsigned char NumI2cChans;
		const unsigned char* I2cChans;
		
		//SPI
		unsigned char NumSpiChans;
		const unsigned char* SpiChans;
		
		//CAN
		unsigned char NumCanChans;
		const unsigned char* CanChans;
		
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxDevice();
			
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		//DIGITAL
		virtual int DigitalWrite(unsigned char numPins, unsigned char* pins, unsigned char* values) = 0;
		
		//SPI
		virtual int SpiOpenMaster(unsigned char channel) = 0;
		virtual int SpiSetBitOrder(unsigned char channel, unsigned char bitOrder) = 0;
		virtual int SpiSetMode(unsigned char channel, unsigned char mode) = 0;
		virtual int SpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed) = 0;
		virtual int SpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer) = 0;
		
		//I2C
		virtual int I2cOpenMaster(unsigned char channel) = 0;
		virtual int I2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed) = 0;
		virtual int I2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer) = 0;
		virtual int I2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer) = 0;		
		virtual int I2cClose(unsigned char channel) = 0;
		
		//UART
		virtual int UartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud) = 0;
		virtual int UartSetBaudRate(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud) = 0;
		virtual int UartGetBytesAvailable(unsigned char channel, unsigned char *numBytes) = 0;
		virtual int UartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead) = 0;
		virtual int UartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer) = 0;
		virtual int UartClose(unsigned char channel) = 0;
				
		//General - 
		unsigned char ReverseBits(unsigned char b);
		virtual unsigned long GetMilliSeconds() = 0;
		virtual unsigned long GetSeconds() = 0;
		virtual void DelayMs(unsigned long ms);
		
		//Debug
		virtual void EnableDebug(unsigned char channel);
		virtual void DebugPrint(const char[]);
		virtual void DebugPrint(unsigned char numBytes, const char* message);		
		virtual void DebugPrintPacket(unsigned char direction, const unsigned char* packetBuffer);
				
	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
				
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
};
#endif //LINX_DEVICE_H
