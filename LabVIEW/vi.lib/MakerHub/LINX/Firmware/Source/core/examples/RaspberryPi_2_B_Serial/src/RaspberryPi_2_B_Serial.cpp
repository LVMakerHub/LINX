#include <stdio.h>
#include <iostream>

#include "LinxDevice.h"
#include "LinxRaspberryPi.h"
#include "LinxRaspberryPi2B.h"
#include "LinxSerialListener.h"

#define ListenerUartChan 0

LinxRaspberryPi2B* LinxDevice;


int main()
{
	fprintf(stdout, "\n\n ..:: LINX ::..\n\n");
	//Instantiate The LINX Device
	LinxDevice = new LinxRaspberryPi2B();
	
	//The LINXT Listener Is Pre Instantiated, Call Start And Pass A Pointer To The LINX Device And The UART Channel To Listen On
	LinxSerialConnection.Start(LinxDevice, ListenerUartChan);

	//Check for and process commands
	fprintf(stdout, "Listening On UART %d.\n", ListenerUartChan);

	while(1)
	{
		LinxSerialConnection.CheckForCommands();
	}

	return 0;
}