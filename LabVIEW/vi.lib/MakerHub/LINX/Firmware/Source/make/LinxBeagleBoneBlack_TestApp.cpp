#include <iostream>

#include "LinxDevice.h"
#include "LinxBeagleBone.h"
#include "LinxBeagleBoneBlack.h"

#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>	
#include <sys/ioctl.h>

using namespace std;


LinxBeagleBoneBlack* LinxDev;

int main()
{
	LinxDev = new LinxBeagleBoneBlack();
	cout << "\r\nHello World!\r\n\r\n";
	
	
	//Delay Test
	/**************************************************************************
	for(int i=0; i<10; i++)
	{	
		
		std::cout << LinxDev->GetMilliSeconds();
		LinxDev->DelayMs(100);
		std::cout << "\r\n";
	}
	**************************************************************************/
	
	unsigned long actualBaud = 0;
	
	
	
	int x = open("/dev/ttyO0\00",  O_RDWR | O_NDELAY);
	write(x, "12345", 4);	
	close(x);
	
	LinxDev->UartOpen(0, 9600, &actualBaud);
	cout << "Open!\r\n";
	
	unsigned char buffer[64] = "BBB UART Test";
	
	LinxDev->UartWrite(0, 13, buffer);
	
	LinxDev->UartClose(0);
	
}