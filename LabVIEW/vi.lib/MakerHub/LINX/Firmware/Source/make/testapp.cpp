#include <iostream>

#include "LinxDevice.h"
#include "LinxRaspberryPi.h"
#include "LinxRaspberryPi2B.h"


LinxRaspberryPi2B* LinxDev;

int main()
{
	LinxDev = new LinxRaspberryPi2B();
	std::cout << "\r\nHello World!\r\n\r\n";
	
	for(int i=0; i<10; i++)
	{	
		
		std::cout << LinxDev->GetMilliSeconds();
		LinxDev->DelayMs(100);
		std::cout << "\r\n";
	}
}