/****************************************************************************************
**  LINX TCP  Listener For BeagleBone Black
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#include <stdio.h>
#include <iostream>

#include "LinxDevice.h"
#include "LinxBeagleBone.h"
#include "LinxBeagleBoneBlack.h"
#include "LinxLinuxTcpListener.h"

#define LISTENER_TCP_PORT 0

LinxBeagleBoneBlack* LinxDevice;

int main()
{
	fprintf(stdout, "\n\n ..:: LINX ::..\n\n");
	
	//Instantiate The LINX Device
	LinxDevice = new LinxBeagleBoneBlack();
	
	//The LINXT Listener Is Pre Instantiated, Call Start And Pass A Pointer To The LINX Device And The UART Channel To Listen On
	LinxTcpConnection.Start(LinxDevice, LISTENER_TCP_PORT);

	fprintf(stdout, "Listening On TCP Port %d.\n", LISTENER_TCP_PORT);

	//Check for and process commands
	while(1)
	{
		LinxTcpConnection.CheckForCommands();
	}

	return 0;
}