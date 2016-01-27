/****************************************************************************************
**  Header file for LINX Device Lib (Shared Library)
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_DEVICELIB_H
#define LINX_DEVICELIB_H

#include <string>

using namespace std;
//------------------------------------- Constructor/Destructor -------------------------------------
extern "C" int LinxOpen();
extern "C" int LinxClose();


//------------------------------------- Enumeration -------------------------------------
extern "C" unsigned char LinxGetDeviceFamily();
extern "C" unsigned char LinxGetDeviceId();
extern "C" int LinxGetDeviceName(string* name);


//------------------------------------- General -------------------------------------
extern "C" unsigned long LinxGetMilliSeconds();


//------------------------------------- Analog -------------------------------------

extern "C" unsigned long LinxAiGetRefSetVoltage();
extern "C" unsigned long LinxAoGetRefSetVoltage();
extern "C" unsigned long LinxAiGetResolution();
extern "C" unsigned long LinxAoGetResolution();
extern "C" unsigned char LinxAiGetNumChans();
extern "C" unsigned char LinxAoGetNumChans();
extern "C" int LinxAiGetChans(unsigned char numChans, unsigned char* channels);
extern "C" int LinxAoGetChans(unsigned char numChans, unsigned char* channels);
extern "C" int LinxAnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
extern "C" int LinxAnalogReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned long* values);


//------------------------------------- CAN -------------------------------------
extern "C" unsigned char LinxCanGetNumChans();
extern "C" int LinxCanGetChans(unsigned char numChans, unsigned char* channels);


//------------------------------------- Digital -------------------------------------
extern "C" unsigned char LinxDigitalGetNumChans();
extern "C" int LinxDigitalGetChans(unsigned char numChans, unsigned char* channels);
extern "C" int LinxDigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values);
extern "C" int LinxDigitalWriteNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);
extern "C" int LinxDigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
extern "C" int LinxDigitalReadNoPacking(unsigned char numChans, unsigned char* channels, unsigned char* values);


//------------------------------------- I2C -------------------------------------
extern "C" unsigned char LinxI2cGetNumChans();
extern "C" int LinxI2cGetChans(unsigned char numChans, unsigned char* channels);
extern "C" int LinxI2cOpenMaster(unsigned char channel);
extern "C" int LinxI2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed);
extern "C" int LinxI2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer);
extern "C" int LinxI2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer);		
extern "C" int LinxI2cClose(unsigned char channel);
		
		
//------------------------------------- PWM -------------------------------------
extern "C" unsigned char LinxPwmGetNumChans();
extern "C" int LinxPwmGetChans(unsigned char numChans, unsigned char* channels);
extern "C" int LinxPwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values);

//------------------------------------- QE -------------------------------------
extern "C" unsigned char LinxQeGetNumChans();
extern "C" int LinxQeGetChans(unsigned char numChans, unsigned char* channels);


//------------------------------------- Servo -------------------------------------
extern "C" unsigned char LinxServoGetNumChans();
extern "C" int LinxServoGetChans(unsigned char numChans, unsigned char* channels);


//------------------------------------- SPI -------------------------------------
extern "C" unsigned char LinxSpiGetNumChans();
extern "C" int LinxSpiGetChans(unsigned char numChans, unsigned char* channels);
extern "C" int LinxSpiOpenMaster(unsigned char channel);
extern "C" int LinxSpiSetBitOrder(unsigned char channel, unsigned char bitOrder);
extern "C" int LinxSpiSetMode(unsigned char channel, unsigned char mode);
extern "C" int LinxSpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed);
extern "C" int LinxSpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
		
//------------------------------------- UART -------------------------------------
extern "C" unsigned char LinxUartGetNumChans();
extern "C" int LinxUartGetChans(unsigned char numChans, unsigned char* channels);
extern "C" int LinxUartOpen(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud);
extern "C" int LinxUartSetBaudRate(unsigned char channel, unsigned long baudRate, unsigned long* actualBaud);
extern "C" int LinxUartGetBytesAvailable(unsigned char channel, unsigned char *numBytes);
extern "C" int LinxUartRead(unsigned char channel, unsigned char numBytes, unsigned char* recBuffer, unsigned char* numBytesRead);
extern "C" int LinxUartWrite(unsigned char channel, unsigned char numBytes, unsigned char* sendBuffer);
extern "C" int LinxUartClose(unsigned char channel);

#endif //LINX_DEVICELIB_H