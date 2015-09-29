#include <stdio.h>
#include <iostream>

#include "LinxDevice.h"
#include "LinxBeagleBone.h"
#include "LinxBeagleBoneBlack.h"
#include "LinxLinuxTcpListener.h"

LinxBeagleBoneBlack* LinxDevice;


int main()
{
	fprintf(stdout, "\n\n ..:: LINX ::..\n\n");
	//Instantiate The LINX Device
	LinxDevice = new LinxBeagleBoneBlack();
	
	//The LINXT Listener Is Pre Instantiated, Call Start And Pass A Pointer To The LINX Device And The UART Channel To Listen On
	LinxTcpConnection.Start(LinxDevice, 44300);

	//Check for and process commands
	fprintf(stdout, "Starting LINX TCP Listener\n");

	while(1)
	{
		LinxTcpConnection.CheckForCommands();
	}

	return 0;
}