#include <iostream>

#include "LinxDevice.h"
#include "LinxBeagleBone.h"
#include "LinxBeagleBoneBlack.h"

#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h> 
#include <fstream>
#include <termios.h>	
#include <sstream>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <map>


using namespace std;

LinxBeagleBoneBlack* LinxDev;

int main()
{
	LinxDev = new LinxBeagleBoneBlack();
	cout << "\r\n.: I2C Test :.\r\n\r\n";
	
	unsigned char i2cChan = 2;
	unsigned char slaveAddress = 0x1D;
	unsigned char data[32] = {0x27, 0x01, 0x02, 0x03};
	
	//Open I2C Master
	LinxDev->I2cOpenMaster(i2cChan);
	fprintf(stdout, "I2C %d open.\n", i2cChan);
	
	//Write Data
	unsigned char powerOnCmd[2] = {0x20, 0x0f};
	unsigned char resolutionCmd[2] = {0x23, 0x30};
	fprintf(stdout, "Writing %d bytes to slave address %d.\n", 1, slaveAddress);
	LinxDev->I2cWrite(i2cChan, slaveAddress, EOF_STOP, 1, data);
	
	
	//Read Data
	fprintf(stdout, "Reading %d bytes from slave address %d.\n", 1, slaveAddress);
	LinxDev->I2cRead(i2cChan, slaveAddress, EOF_STOP, 1, 100, data);
	
	//Close
	LinxDev->I2cClose(i2cChan);
	fprintf(stdout, "I2C %d closed.\n", i2cChan);
	
	//Print Results
	LinxDev->DebugPrintln(data[0], HEX);
	
	
}