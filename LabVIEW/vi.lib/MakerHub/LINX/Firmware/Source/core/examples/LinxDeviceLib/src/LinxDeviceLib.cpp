/****************************************************************************************
**  LINX Device Abstraction Layer (Helper Library)
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

LinxRaspberryPi2B* LinxDev;

//------------------------------------- Constructor / Destructor -------------------------------------
int LinxOpen()
{
	//Instantiate The LINX Device
	LinxDev = new LinxRaspberryPi2B();
	fprintf(stdout, "Lib is working...\n");
	return L_OK;
}

int LinxClose()
{
	delete LinxDev;
	return L_OK;
}


//------------------------------------- Digital -------------------------------------
int LinxDigitalWrite(unsigned char numChans, unsigned char* channels, unsigned char* values)
{
	return LinxDev->DigitalWrite(numChans, channels, values);
}

//------------------------------------- Analog -------------------------------------
