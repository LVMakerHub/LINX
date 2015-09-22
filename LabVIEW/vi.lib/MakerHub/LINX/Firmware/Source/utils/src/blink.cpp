#include <stdio.h>
#include <stdlib.h> 
#include <iostream>

#include "LinxDevice.h"
#include "LinxBeagleBone.h"
#include "LinxBeagleBoneBlack.h"

using namespace std;


LinxBeagleBoneBlack* LinxDev;

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		cerr << "Usage: " << argv[0] << " DIO_PIN" << endl;
		return -1;
	}
	
	LinxDev = new LinxBeagleBoneBlack(); 
	
	//Check Channel
	bool valid = false;
	for (int i=0; i<LinxDev->NumDigitalChans; i++)
	{
		if(atoi(argv[1]) == LinxDev->DigitalChans[i])
		{
			//Valid Channel
			valid = true;
			break;
		}
	}	
	
	if(valid == false)
	{		
		cerr << "Invalid DIO Channel.  Valid Channels Are: ";
		fprintf(stderr, "%d", LinxDev->DigitalChans[0]);
		for (int i=1; i<LinxDev->NumDigitalChans; i++)
		{
			fprintf(stderr, ", %d", LinxDev->DigitalChans[i]);
		}		
		return -1;
	}
	
	cout << "\r\n.: DIO Test :.\r\n\r\n";
	
	unsigned char chan[1];
	chan [0] = atoi(argv[1]);
	unsigned char onVal[1] = {0xFF};
	unsigned char offVal[1] = {0x00};
	
	for(int i=0; i<3; i++)
	{
		LinxDev->DigitalWrite(1, chan, onVal);
		usleep(150000);
		LinxDev->DigitalWrite(1, chan, offVal);
		usleep(150000);
	}

	return 0;
	
}