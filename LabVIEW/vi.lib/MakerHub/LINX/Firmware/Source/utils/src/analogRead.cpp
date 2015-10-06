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
		cerr << "Usage: " << argv[0] << " AI_PIN" << endl;
		return -1;
	}
	
	LinxDev = new LinxBeagleBoneBlack();
	
	//Check Channel
	bool valid = false;
	for (int i=0; i<LinxDev->NumAiChans; i++)
	{
		if(atoi(argv[1]) == LinxDev->AiChans[i])
		{
			//Valid Channel
			valid = true;
			break;
		}
	}	
	
	if(valid == false)
	{		
		cerr << "Invalid AI Channel.  Valid Channels Are: ";
		fprintf(stderr, "%d", LinxDev->AiChans[0]);
		for (int i=1; i<LinxDev->NumAiChans; i++)
		{
			fprintf(stderr, ", %d", LinxDev->AiChans[i]);
		}		
		return -1;
	}
	
	cout << "\r\n.: analogRead Test :.\r\n\r\n";
	unsigned char aiVal[2];
	unsigned char chan[1];
	chan [0] = atoi(argv[1]);

	LinxDev->AnalogRead(1, chan, aiVal);
	
	unsigned int aiVoltage = (aiVal[1] <<8) | aiVal[0];
	
	fprintf(stdout, "%f", aiVoltage*0.00043945312);
	fprintf(stdout, "v \r\n", aiVoltage);
	
	return 0;
	
}