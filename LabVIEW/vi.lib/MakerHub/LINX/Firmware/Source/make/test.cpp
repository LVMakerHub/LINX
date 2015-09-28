#include <map>
#include <fcntl.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <termios.h>	
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

using namespace std;

int main()
{
	
	fprintf(stdout, "Begin...\n");
	
	int diVal = 0;
	
	FILE* handle = fopen("/sys/class/gpio/gpio66/value", "r+w+");
	fprintf(stdout, "File Open..%d\n", handle);
	
	fscanf(handle, "%u", &diVal);
	fprintf(stdout, "Data Read..%d\n", handle);
	fprintf(stdout, "Value = %d\n", diVal);
	
	//fclose(handle);
	fprintf(stdout, "File Closed..%d\n", handle);
	
	return 0;
}