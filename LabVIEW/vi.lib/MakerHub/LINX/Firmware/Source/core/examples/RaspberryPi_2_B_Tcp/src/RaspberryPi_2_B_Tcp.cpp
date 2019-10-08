/****************************************************************************************
**  LINX TCP Listener For Raspberry Pi 2 Model B
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
#include "LinxRaspberryPi.h"
#include "LinxRaspberryPi2B.h"
#include "LinxLinuxTcpListener.h"

#define LISTENER_TCP_PORT 44300

LinxRaspberryPi2B* LinxDev;

int main()
{
	fprintf(stdout, "\n\n ..:: LINX ::..\n\n");
	
	//Instantiate The LINX Device
	LinxDev = new LinxRaspberryPi2B();
	
	//The LINXT Listener Is Pre Instantiated, Call Start And Pass A Pointer To The LINX Device And The UART Channel To Listen On
	LinxTcpConnection.Start(LinxDev, LISTENER_TCP_PORT);

	fprintf(stdout, "Listening On TCP Port %d.\n", LISTENER_TCP_PORT);
	
	//Check for and process commands
	while(1)
	{
		LinxTcpConnection.CheckForCommands();
	}

	return 0;
}