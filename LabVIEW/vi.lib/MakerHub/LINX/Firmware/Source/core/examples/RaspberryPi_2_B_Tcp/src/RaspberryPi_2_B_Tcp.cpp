#include <stdio.h>
#include <iostream>

#include "LinxDevice.h"
#include "LinxRaspberryPi.h"
#include "LinxRaspberryPi2B.h"
#include "LinxLinuxTcpListener.h"

LinxRaspberryPi2B* LinxDev;


int main()
{
	fprintf(stdout, "\n\n ..:: LINX ::..\n\n");
	//Instantiate The LINX Device
	LinxDev = new LinxRaspberryPi2B();
	
	//The LINXT Listener Is Pre Instantiated, Call Start And Pass A Pointer To The LINX Device And The UART Channel To Listen On
	LinxTcpConnection.Start(LinxDev, 44300);

	//Check for and process commands
	fprintf(stdout, "Starting LINX TCP Listener\n");
	
	LinxDev->DebugPrintln("Hello From LINX");

	while(1)
	{
		LinxTcpConnection.CheckForCommands();
	}

	return 0;
}