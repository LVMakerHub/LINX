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
	cout << "\r\n.: UART Loopback Test :.\r\n\r\n";
	
	unsigned long actualBaud = 0;
	unsigned char uartChan = 1;
	
	//Data
	unsigned char txbuffer[64] = "Hello World!";	
	unsigned char rxbuffer[64];
	unsigned char numBytesTx = 12;
	unsigned char numBytesRx = 0;
	
	cout << "Opening UART\r\n";
	LinxDev->UartOpen(uartChan, 115200, &actualBaud);
	
	cout << "Writing Data\r\n";	
	LinxDev->UartWrite(uartChan, numBytesTx, txbuffer);
	
	cout << "Reading Data\r\n";
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
		
		cout << "Closing UART\r\n";
		LinxDev->UartClose(uartChan);
	}
	
	return 0;
}