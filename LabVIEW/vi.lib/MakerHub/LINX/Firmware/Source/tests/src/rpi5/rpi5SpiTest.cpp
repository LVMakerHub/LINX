#include <iostream>

#include "LinxDevice.h"
#include "LinxRaspberryPi.h"
#include "LinxRaspberryPi5.h"

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


LinxRaspberryPi5* LinxDev;

int main()
{
	LinxDev = new LinxRaspberryPi5();
	cout << "\r\n.: SPI Test :.\r\n\r\n";
	
	//LinxDev->DigitalWrite(7, 0xFF);
	
	
	//Open SPI Master
	unsigned char spiChannel = 0;
	unsigned char frameSize = 4;
	unsigned char numFrames = 2;
	unsigned char csChan = 7;
	unsigned char csLL = 0;	//Active Low
	unsigned char sendBuffer[8] = {0xAA, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
	unsigned char receiveBuffer[32]; 
	
	LinxDev->SpiOpenMaster(spiChannel);
	fprintf(stdout, "SPI %d open.\n", spiChannel);
	
	//Transfer Data
	int iterations = 10;
	for(int i=0; i<iterations; i++)
	{
		LinxDev->SpiWriteRead(spiChannel, frameSize, numFrames, csChan, csLL, sendBuffer, receiveBuffer);
		//receiveBuffer[frameSize*numFrames] = 0;
		//fprintf(stdout, "Write/Read Complete: %s\n", receiveBuffer);
		
		for(int i=0; i< frameSize*numFrames; i++)
		{
			LinxDev->DebugPrint("[0x");
			LinxDev->DebugPrint(receiveBuffer[i], HEX);
			LinxDev->DebugPrint("] ");
		}
		LinxDev->DebugPrintln("");
	}
	
}
	
	