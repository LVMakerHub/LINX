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
	unsigned char frameSize = 8;
	unsigned char numFrames = 1;
	unsigned char csChan = 66;
	unsigned char csLL = 0;	//Active Low
	unsigned char sendBuffer[8] = {0xAA, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
	unsigned char receiveBuffer[8]; 
	
	LinxDev->SpiOpenMaster(spiChannel);
	
	//Transfer Data
	LinxDev->SpiWriteRead(spiChannel, frameSize, numFrames, csChan, csLL, sendBuffer, receiveBuffer);
	
	for(int i=0; i< frameSize*numFrames; i++)
	{
		LinxDev->DebugPrint("[0x");
		LinxDev->DebugPrint(receiveBuffer[i], HEX);
		LinxDev->DebugPrint("] ");
	}
	LinxDev->DebugPrintln("");
	
	
}