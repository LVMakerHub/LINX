#include <iostream>
#include <cstring>

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

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		cerr << "Usage: " << argv[0] << " [PWM CHANNEL]" << " [DUTY CYCLE (0-255)]" << endl;
		return -1;
	}
	
	LinxDev = new LinxBeagleBoneBlack();
	
	//Check Channel
	unsigned char pwmChannel = atoi(argv[1]);	
	bool validChan = false;
	for (int i=0; i<LinxDev->NumPwmChans; i++)
	{		
		if(pwmChannel == LinxDev->PwmChans[i])
		{
			//Valid Channel
			validChan = true;
			break;
		}
	}
	
	if(!validChan)
	{
		cout << "Invalid PWM Channel.\r\n";
		return -1;
	}
	
	LinxDev = new LinxBeagleBoneBlack();
	cout << "\r\n.: PWM Set Duty Cycle Test :.\r\n\r\n";
	
	unsigned char pwmChan[1]; 
	pwmChan[0] = pwmChannel;

	unsigned char dutyCycle[1];
	dutyCycle[0] = atoi(argv[2]);
		
	LinxDev->PwmSetDutyCycle(1, pwmChan, dutyCycle);
	
	
	return 0;
}