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
	if(argc < 4)
	{
		cerr << "Usage: " << argv[0] << " [UART CHANNEL]" << " [BAUD RATE]" << " [DATA]"<< endl;
		return -1;
	}
	
	LinxDev = new LinxBeagleBoneBlack();
	
	//Check Channel
	bool valid = false;
	for (int i=0; i<LinxDev->NumUartChans; i++)
	{
		if(atoi(argv[1]) == LinxDev->UartChans[i])
		{
			//Valid Channel
			valid = true;
			break;
		}
	}	
	
	
	LinxDev = new LinxBeagleBoneBlack();
	cout << "\r\n.: UART Loopback Test :.\r\n\r\n";
	
	unsigned char uartChan = atoi(argv[1]);;
	unsigned long actualBaud = 0;
	
	//Data
	//unsigned char txbuffer[128];	
	unsigned char rxbuffer[128];
	unsigned char numBytesTx = strlen(argv[3]);
	unsigned char numBytesRx = 0;
	unsigned long buadRate = atoi(argv[2]);
	

	
	cout << "Opening UART\r\n";
	LinxDev->UartOpen(uartChan, buadRate, &actualBaud);
	
	fprintf(stdout, "Writing %d bytes...\n", numBytesTx);
	LinxDev->UartWrite(uartChan, numBytesTx, (unsigned char*)argv[3]);
	
	cout << "Reading Data..\r\n";
	LinxDev->UartRead(uartChan, numBytesTx, rxbuffer, &numBytesRx);
	
	if(numBytesRx != numBytesTx)
	{
		fprintf(stdout, "\r\nFail - Sent %d bytes but read %d bytes.\r\n\r\n", numBytesTx, numBytesRx);
	}
	else
	{
		rxbuffer[numBytesTx] = 0;
		fprintf(stdout, "\r\nSuccess - '");
		fprintf(stdout, "%s", rxbuffer);
		fprintf(stdout, "'\r\n\r\n");
		
		cout << "Closing UART.\r\n";
		LinxDev->UartClose(uartChan);
	}
	
	return 0;
}