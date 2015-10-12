#include <stdio.h>
#include <iostream>

#include "LinxDevice.h"
#include "LinxRaspberryPi.h"
#include "LinxRaspberryPi2B.h"

#include "LinxDeviceLib.h"

int main()
{
	fprintf(stdout, "\nHello World\n\n");
	
	LinxOpen();
	
	unsigned char pins[1] = {7};
	unsigned char vals[1] = {0xFF};
	
	LinxDigitalWrite(1, &pins, &vals);
	
	LinxClose();
}