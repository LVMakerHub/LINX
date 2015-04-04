#include <iostream>

#include "LinxDevice.h"
#include "LinxRaspberryPi.h"


LinxRaspberryPi* myPi;

int main()
{
	myPi = new LinxRaspberryPi();
	std::cout << "\r\nHello World!\r\n\r\n";
	
	for(int i=0; i<10; i++)
	{	
		std::cout << myPi->GetMilliSeconds();
		myPi->DelayMs(100);
		std::cout << "\r\n";
	}
}