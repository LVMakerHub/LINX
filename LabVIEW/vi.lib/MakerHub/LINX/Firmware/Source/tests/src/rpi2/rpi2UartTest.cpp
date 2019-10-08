#include <iostream>

#include "LinxDevice.h"
#include "LinxRaspberryPi.h"
#include "LinxRaspberryPi2B.h"

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


LinxRaspberryPi2B* LinxDev;

int main()
{
	cout << "\r\n.: Raspberry Pi 2 UART Test :.\r\n\r\n";
	
	LinxDev = new LinxRaspberryPi2B();
	
	unsigned long actualBaud = 0;
	unsigned char uartChan = 0;
	
	//Data
	unsigned char txbuffer[64] = "Hello World!";	
	unsigned char rxbuffer[64];
	unsigned char numBytesTx = 12;
	unsigned char numBytesRx = 0;
	
	cout << "Opening UART\r\n";
	LinxDev->UartOpen(uartChan, 115200, &actualBaud);
	
	unsigned char bytesAtPort = 0;
	while(1)
	{
		if(LinxDev->UartGetBytesAvailable(uartChan, &bytesAtPort) == LUART_AVAILABLE_FAIL)
		{
			cout << "Fail\n";
		}
		//cout << (unsigned int)bytesAtPort << " Bytes Available\n";
	}
	
	/*
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
		
	}	
	
	*/
	
	cout << "Closing UART\r\n";
	LinxDev->UartClose(uartChan);
	
	
	
	return 0;


	
}