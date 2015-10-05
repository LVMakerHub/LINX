/****************************************************************************************
**  LINX Configurable Listener For Raspberry Pi 2 Model B
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
#include <string>
#include <stdlib.h> 

#include "LinxDevice.h"
#include "LinxRaspberryPi.h"
#include "LinxRaspberryPi2B.h"
#include "LinxSerialListener.h"
#include "LinxLinuxTcpListener.h"

//Helper Functions
int parseInputTokens(LinxDevice* linxDev, int argc, char* argv[]);
void printUsage(char* argv[], LinxDevice* linxDev);

#define NUMTOKENS 10
string tokens[NUMTOKENS] = {"-h", "-H", "--help", "--Help", "-v", "-V", "--version", "--Version", "-serial", "-tcp"};

int uartListenerPort = -1;
int tcpListenerPort = -1;

LinxRaspberryPi2B* LinxDev;

int main(int argc, char* argv[])
{
	//Instantiate The LINX Device
	LinxDev = new LinxRaspberryPi2B();
	
	if(parseInputTokens(LinxDev, argc, argv) >= 0)
	{	
		if(uartListenerPort > -1 && tcpListenerPort > -1)
		{
			while(1)
			{
				LinxSerialConnection.CheckForCommands();
				LinxTcpConnection.CheckForCommands();
			}
		}
		else if(uartListenerPort > -1)
		{
			while(1)
			{
				LinxSerialConnection.CheckForCommands();
			}
			
		}
		else if(tcpListenerPort > -1)
		{
			while(1)
			{
				LinxTcpConnection.CheckForCommands();
			}
		}
		else
		{
			cout << "No bus specified.\n";
			printUsage(argv, LinxDev);
			return -1;
		}
	}
	return 0;
}


//Helper function to parse input tokens and set variables or print output.
int parseInputTokens(LinxDevice* linxDev, int argc, char* argv[])
{
	cout << "\n";
	
	//Loop Over All Tokens
	for(int i=1; i<argc; i++)
	{
		//cout << argv[i];
		
		bool match = false;
		for(int j=0; j<NUMTOKENS; j++)
		{
			if(tokens[j].compare(argv[i]) == 0)
			{
				//cout << " - match\n";
				match = true;
				
				switch(j)
				{
					case 0: 	//-h
					case 1: 	//-H
					case 2:	//--help
					case 3: 	//--Help
						printUsage(argv, linxDev);
						return 0;
					case 4: 	//-v
					case 5: 	//-V
					case 6:	//--version
					case 7: 	//--Version
						cout << "\n"<< argv[0] << " version 2.1.0\n";
						return 0;
					case 8:	//-serial
						if(i+1 >= argc)
						{
							cout << "\n"<< "Missing port\n";
							printUsage(argv, linxDev);
							return -1;
						}
						else
						{
							uartListenerPort = atoi(argv[i+1]);
							bool validPort = false;
							
							for(int i=0; i<linxDev->NumUartChans; i++)
							{
								if(uartListenerPort == linxDev->UartChans[i])
								{	
									cout << "\n"<< "Listening on UART " << (unsigned short)uartListenerPort << "\n";
									LinxSerialConnection.Start(LinxDev, (unsigned char)uartListenerPort);
									validPort = true;
									break;
								}
							}
							if(!validPort)
							{
								//Invalid Port, Print Usage
								cout << "Invalid port.\n";
								printUsage(argv, linxDev);
								return -1;
							}
							break;
						}
						break;
					case 9:	//-tcp
						if(i+1 >= argc)
						{
							cout << "\n"<< "Missing port\n";
							printUsage(argv, linxDev);
							return -1;
						}
						else
						{
							tcpListenerPort = atoi(argv[i+1]);
							cout << "\n"<< "Listening on TCP Port " << (unsigned short)tcpListenerPort << "\n";
							LinxTcpConnection.Start(LinxDev, tcpListenerPort);
						}
						break;
					default:
						break;
				}
			}
		}
		if(!match)
		{
			//Either Argument or invalid token
		}
		
	}
	return 0;
}

//Helper function to print usage information.
void printUsage(char* argv[], LinxDevice* linxDev)
{
	cout << "\nusage: " << argv[0] << " -serial [port]\n";
	cout << "   or: " << argv[0] << " -serial [port] -tcp [port]\n";
	cout << "   or: " << argv[0] << " -tcp [port]\n\n";
	cout << "Available options are:\n";
	cout << "  -serial\t " << (int)linxDev->UartChans[0];
	for(int i = 1; i<linxDev->NumUartChans; i++)
	{
		cout << ", " << (int)linxDev->UartChans[i];
	}	
	cout << "\n";
	cout << "  -tcp  \t Any valid, unused TCP port. (ex 44300)\n\n";
}