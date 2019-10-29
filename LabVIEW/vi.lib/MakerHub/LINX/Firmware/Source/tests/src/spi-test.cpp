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
	cout << "\r\n.: SPI Test :.\r\n\r\n";
	
	//Open SPI Master
	unsigned char spiChannel = 0;
	unsigned char frameSize = 4;
	unsigned char numFrames = 2;
	unsigned char csChan = 61;
	unsigned char csLL = 0;	//Active Low
	unsigned char sendBuffer[8] = {0xAA, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
	unsigned char receiveBuffer[32]; 
	int retval;

	retval = LinxDev->SpiOpenMaster(spiChannel);
	fprintf(stdout, "SpiOpenMaster() returned %d\n", retval);
	fprintf(stdout, "SPI %d open.\n", spiChannel);
	
	//Transfer Data
	retval = LinxDev->SpiWriteRead(spiChannel, frameSize, numFrames, csChan, csLL, sendBuffer, receiveBuffer);
	fprintf(stdout, "SpiWriteRead() returned %d\n", retval);
	receiveBuffer[frameSize*numFrames] = 0;
	fprintf(stdout, "Write/Read Complete: %x %x %x %x %x %x %x %x\n", receiveBuffer[0], receiveBuffer[1],receiveBuffer[2],receiveBuffer[3],receiveBuffer[4],receiveBuffer[5],receiveBuffer[6],receiveBuffer[7]);
	
	for(int i=0; i< frameSize*numFrames; i++)
	{
		LinxDev->DebugPrint("[0x");
		LinxDev->DebugPrint(receiveBuffer[i], HEX);
		LinxDev->DebugPrint("] ");
	}
	LinxDev->DebugPrintln("");
	
	
}