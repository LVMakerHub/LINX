/****************************************************************************************
**  LINX Serial Listener For Raspberry Pi 5
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Ken Sharp
**
** BSD2 License.
****************************************************************************************/	

#include <stdio.h>
#include <iostream>

#include "LinxDevice.h"
#include "LinxRaspberryPi.h"
#include "LinxRaspberryPi5.h"
#include "LinxSerialListener.h"

#define LISTENER_UART_PORT 0

LinxRaspberryPi5* LinxDevice;


int main()
{
	fprintf(stdout, "\n\n ..:: LINX ::..\n\n");
	
	//Instantiate The LINX Device
	LinxDevice = new LinxRaspberryPi5();
	
	//The LINXT Listener Is Pre Instantiated, Call Start And Pass A Pointer To The LINX Device And The UART Channel To Listen On
	LinxSerialConnection.Start(LinxDevice, LISTENER_UART_PORT);

	fprintf(stdout, "Listening On UART %d.\n", LISTENER_UART_PORT);

	//Check for and process commands
	while(1)
	{
		LinxSerialConnection.CheckForCommands();
	}

	return 0;
}