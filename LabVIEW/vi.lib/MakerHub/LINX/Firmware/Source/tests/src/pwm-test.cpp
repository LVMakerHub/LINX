
#include "LinxDevice.h"
#include "LinxBeagleBone.h"
#include "LinxBeagleBoneBlack.h"

#include <iostream>
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
	cout << "\r\n.: PWM Test :.\r\n\r\n";
	
	unsigned char numChans = 4;
	unsigned char chans[4] = {13, 19, 60, 62};
	unsigned char vals[4] = {0, 64, 128, 192};
	
	LinxDev->PwmSetDutyCycle(numChans, chans, vals);
	
	return 0;
}