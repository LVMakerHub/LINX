#include <iostream>

#include "LinxDevice.h"
#include "LinxBeagleBone.h"
#include "LinxBeagleBoneBlack.h"

#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h> 
#include <fstream>
#include <termios.h>	
#include <sstream>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <map>


using namespace std;


LinxBeagleBoneBlack* LinxDev;

int main()
{
	LinxDev = new LinxBeagleBoneBlack();
	cout << "\r\nHello World!\r\n\r\n";
	
	
	//Delay Test
	/**************************************************************************
	for(int i=0; i<10; i++)
	{	
		
		std::cout << LinxDev->GetMilliSeconds();
		LinxDev->DelayMs(100);
		std::cout << "\r\n";
	}
	**************************************************************************/
		
	
	//UART Test
	/**************************************************************************
	
	unsigned long actualBaud = 0;
	unsigned char uartChan = 5;
	
	LinxDev->UartOpen(uartChan, 115200, &actualBaud);
	cout << "Open!\r\n";
	
	unsigned char buffer[64] = "BBB UART Test\r\n";
	
	LinxDev->UartWrite(uartChan, 15, buffer);
	
	LinxDev->UartClose(uartChan);
	**************************************************************************/
	
	//I2C Test
	/**************************************************************************
	
	unsigned char i2cChan = 1;
	LinxDev->I2cOpenMaster(i2cChan);
	unsigned char data[4] = {0x0D, 0x00, 0xFF, 0x00};
	LinxDev->I2cWrite(i2cChan, 0x1D, EOF_STOP, 1, data);
	LinxDev->I2cRead(i2cChan, 0x1D, EOF_STOP, 1, 100, data);
	LinxDev->DebugPrintln(data[0], HEX);
	LinxDev->I2cClose(i2cChan);
	**************************************************************************/
	
	//AI Read Test
	
	/*
	int x = open("/sys/bus/iio/devices/iio:device0/in_voltage0_raw",  O_RDWR | O_NDELAY);
	char aiBuffer[4];
	for(int i=0; i<10000; i++)
	{
		//char aiBuffer[4];
		read(x, aiBuffer, 4);
		LinxDev->DebugPrintln(atoi(aiBuffer), DEC);
		
		
	}	
	close(x);
	
	
	int analogValue = 0;
	fstream fs;
	fs.open("/sys/bus/iio/devices/iio:device0/in_voltage0_raw", fstream::in);
	for(int i=0; i<1000; i++)
	{		
		fs.open("/sys/bus/iio/devices/iio:device0/in_voltage0_raw", fstream::in);
		fs >> analogValue;
		LinxDev->DebugPrintln(analogValue, DEC);
	}
	fs.close();
	*/
	
	/*
	unsigned char channels[1] = {0};
	unsigned char values[2];
	
	unsigned long startTime = LinxDev->GetMilliSeconds();
	LinxDev->DebugPrintln(startTime, DEC);
	for(int i=0; i<10000; i++)
	{
		LinxDev->AnalogRead(1, channels, values);
		//unsigned int val = values[1] << 8 | values [0];
		//LinxDev->DebugPrint(val, DEC);
	}
	unsigned long endTime = LinxDev->GetMilliSeconds();
	LinxDev->DebugPrintln(endTime, DEC);
	LinxDev->DebugPrint("10000 Samples In ");
	LinxDev->DebugPrint((endTime-startTime)/1000, DEC);
	LinxDev->DebugPrint("Seconds. ");
	LinxDev->DebugPrint(10000/((endTime-startTime)/1000), DEC);
	LinxDev->DebugPrint("Samples Per Second.");
	*/
	//Export TTY4
	//int x = open("/sys/devices/bone_capemgr.9/slots",  O_RDWR | O_NDELAY);
	//write(x, "BB-UART4", 8);
	//close(x);
	
	/*******
		int handle;
		string basePath = "/sys/class/gpio/gpio";
		
		unsigned long startTime = LinxDev->GetMilliSeconds();
		LinxDev->DebugPrintln(startTime, DEC);
		
		char pathBuffer[64];
		sprintf(pathBuffer, "/sys/class/gpio/gpio%d/value", 66);
		
		
		handle = open(pathBuffer, O_RDWR);
		for(int i=0; i<10; i++)
		{			
			write(handle, "1", 1);
			usleep(250000);
			write(handle, "0", 1);
			usleep(250000);
			
		}
		close(handle);
		unsigned long endTime = LinxDev->GetMilliSeconds();
		LinxDev->DebugPrintln(endTime, DEC);
		LinxDev->DebugPrint("10000 Samples In ");
		LinxDev->DebugPrint((endTime-startTime), DEC);
		LinxDev->DebugPrint("mS. ");
		//LinxDev->DebugPrint(10000/((endTime-startTime)/1000), DEC);
		//LinxDev->DebugPrint("Samples Per Second.");
	*********/
	
	//map<unsigned char, int> DigitalValueHandle = {{0, 0}, {1, 1}};
	
	//DigitalValueHandle[0] = 27;
	
	//LinxDev->DebugPrintln(DigitalValueHandle[0], DEC);
	
	
	/****************************** BLINK!!!
	unsigned char chans[1] = {66};
	unsigned char on[1] = {0xFF};
	unsigned char off[1] = {0x00};
	
	for(int i=0; i<10; i++)
	{
		LinxDev->DigitalWrite(1, chans, on);
		usleep(250000);
		LinxDev->DigitalWrite(1, chans, off);
		usleep(250000);
	}
	*********************************/
	
	
	
	///*DI TEST
	unsigned char inChans[1] = {69};
	unsigned char outChans[1] = {66};
	
	unsigned char vals[1];
	
	unsigned long startTime = LinxDev->GetMilliSeconds();
	LinxDev->DebugPrintln(startTime, DEC);
	for(int i=0; i<100000; i++)  
	{			
		LinxDev->DigitalRead(1, inChans, vals);
		//vals[0] = (vals[0] >> 7) & 0x01;
		//LinxDev->DigitalWrite(1, outChans, vals);
		//LinxDev->DebugPrintln(vals[0], DEC);
		//usleep(1000);
	}
	unsigned long endTime = LinxDev->GetMilliSeconds();
	LinxDev->DebugPrintln(endTime, DEC);
	LinxDev->DebugPrint("100000 Samples In ");
	LinxDev->DebugPrint((endTime-startTime), DEC);
	LinxDev->DebugPrint("mS. ");
	//LinxDev->DebugPrint(10000/((endTime-startTime)/1000), DEC);
	//LinxDev->DebugPrint("Samples Per Second.");
	
	
	//*///DI TEST
	
	/* Simple DI Read
	unsigned char diVal;
	FILE* handle = fopen("/sys/class/gpio/gpio69/value", "r+w+");
	fscanf(handle, "%u", &diVal);
	
	LinxDev->DebugPrintln(diVal, DEC);
	*/// Simple DI Read


	/****************************** Timed Blink *
	unsigned char chans[2] = {66, 69};
	
	
	//Warm Up
	LinxDev->DigitalWrite(2, chans, on);
		
	unsigned long startTime = LinxDev->GetMilliSeconds();
	LinxDev->DebugPrintln(startTime, DEC);
	
	for(int i=0; i<100000; i++)  
	{			
		LinxDev->DigitalWrite(2, chans, on);	
		//usleep(250000);
		LinxDev->DigitalWrite(2, chans, off);		
		//usleep(250000);
	}
	
	
	unsigned long endTime = LinxDev->GetMilliSeconds();
	LinxDev->DebugPrintln(endTime, DEC);
	LinxDev->DebugPrint("2000000 Samples In ");
	LinxDev->DebugPrint((endTime-startTime), DEC);
	LinxDev->DebugPrint("mS. ");
	//LinxDev->DebugPrint(10000/((endTime-startTime)/1000), DEC);
	//LinxDev->DebugPrint("Samples Per Second.");
	****************************** Timed Blink */
	
	
	//int x = open("/sys/class/gpio/gpio69/direction", O_RDWR);
	//write(x, "out", 3);	
	//close(x);
	
	//FILE* y = fopen("/sys/class/gpio/gpio69/direction", "w");
	//fprintf(y, "out");
	//fclose(y);
	
	/*
	FILE* y = fopen("/sys/class/gpio/gpio66/value", "w");
	fprintf(y, "1");
	fflush(y);
	usleep(250000);
	fprintf(y, "0");
	fflush(y);
	usleep(250000);
	fprintf(y, "1");
	fflush(y);
	usleep(250000);
	fprintf(y, "0");
	fflush(y);
	usleep(250000);
	*/
	
	
}