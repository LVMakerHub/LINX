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

#define I2C_CHAN 1

LinxBeagleBoneBlack* LinxDev;

int main()
{
	LinxDev = new LinxBeagleBoneBlack();
	cout << "\r\n.: I2C Test :.\r\n\r\n";
	
	unsigned char i2cChan = 2;
	unsigned char slaveAddress = 0xD1;
	unsigned char data[32] = {0x00, 0x01, 0x02, 0x03};
	
	//Open I2C Master
	LinxDev->I2cOpenMaster(i2cChan);
	
	//Write Data
	unsigned char powerOnCmd[2] = {0x20, 0x0f};
	unsigned char resolutionCmd[2] = {0x23, 0x30};
	LinxDev->I2cWrite(i2cChan, slaveAddress, EOF_STOP, 1, data);
	
	//Read Data
	LinxDev->I2cRead(i2cChan, slaveAddress, EOF_STOP, 1, 100, data);
	
	//Close
	LinxDev->I2cClose(i2cChan);
	
	//Print Results
	LinxDev->DebugPrintln(data[0], HEX);
	
	
}