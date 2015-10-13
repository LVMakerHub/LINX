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

//------------------------------------- Constructor/Destructor -------------------------------------
int LinxOpen();
int LinxClose();


//------------------------------------- Enumeration -------------------------------------
unsigned char LinxGetDeviceFamily();
unsigned char LinxGetDeviceId();
int LinxGetDeviceName(string* name);



/*
Channels - DI, DO, AI, AO, PWM, QE, UART, I2C, SPI, CAN, Servo
AI Ref
AO Reg Volt
AO RES
*/


//------------------------------------- Analog -------------------------------------
//------------------------------------- I2C -------------------------------------
unsigned char LinxAiGetNumChans();
unsigned char LinxAoGetNumChans();

int LinxAnalogRead(unsigned char numChans, unsigned char* channels, unsigned char* values);
int LinxAnalogSetRef(unsigned char mode, unsigned long voltage);


//------------------------------------- Digital -------------------------------------
unsigned char LinxDigitalGetNumChans();

int LinxDigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values);
int LinxDigitalRead(unsigned char numChans, unsigned char* channels, unsigned char* values);


//------------------------------------- I2C -------------------------------------
unsigned char LinxI2cGetNumChans();

int LinxI2cOpenMaster(unsigned char channel);
int LinxI2cSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed);
int LinxI2cWrite(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned char* sendBuffer);
int LinxI2cRead(unsigned char channel, unsigned char slaveAddress, unsigned char eofConfig, unsigned char numBytes, unsigned int timeout, unsigned char* recBuffer);		
int LinxI2cClose(unsigned char channel);
		
		
//------------------------------------- PWM -------------------------------------
unsigned char LinxPwmGetNumChans();

int LinxPwmSetDutyCycle(unsigned char numChans, unsigned char* channels, unsigned char* values);


//------------------------------------- SPI -------------------------------------
unsigned char LinxSpiGetNumChans();
int LinxSpiOpenMaster(unsigned char channel);
int LinxSpiSetBitOrder(unsigned char channel, unsigned char bitOrder);
int LinxSpiSetMode(unsigned char channel, unsigned char mode);
int LinxSpiSetSpeed(unsigned char channel, unsigned long speed, unsigned long* actualSpeed);
int LinxSpiWriteRead(unsigned char channel, unsigned char frameSize, unsigned char numFrames, unsigned char csChan, unsigned char csLL, unsigned char* sendBuffer, unsigned char* recBuffer);
		
//------------------------------------- UART -------------------------------------
//Use VISA


#endif //LINX_DEVICELIB_H