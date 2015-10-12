#include <stdio.h>
#include <iostream>

#include "LinxDevice.h"
#include "LinxRaspberryPi.h"
#include "LinxRaspberryPi2B.h"

#include "Linx_Device_Lib.h"

int main()
{
	fprintf(stdout, "\nHello World\n\n");
	LinxOpen();
}