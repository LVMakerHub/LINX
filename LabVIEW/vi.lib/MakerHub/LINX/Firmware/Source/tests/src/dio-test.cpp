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
	cout << "\r\n.: DIO Test :.\r\n\r\n";
	
	unsigned char chan[1] = {8};
	unsigned char onVal[1] = {0xFF};
	unsigned char offVal[1] = {0x00};
	
	for(int i=0; i<3; i++)
	{
		LinxDev->DigitalWrite(1, chan, onVal);
		usleep(500000);
		LinxDev->DigitalWrite(1, chan, offVal);
		usleep(500000);
	}

	
}