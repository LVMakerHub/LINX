/****************************************************************************************
**  LINX serial listener code.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

/****************************************************************************************
**  Includes
****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utility/LinxDevice.h"

#include "utility/LinxListener.h"
#include "LinxSerialListener.h"

/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxSerialListener::LinxSerialListener()
{
	State = START;
	Interface = UART;
}

/****************************************************************************************
**  Functions
****************************************************************************************/
int LinxSerialListener::Start(LinxDevice* linxDev, unsigned char uartChan)
{	
	LinxDev = linxDev;
	
	recBuffer = (unsigned char*) malloc(LinxDev->ListenerBufferSize);
	sendBuffer = (unsigned char*) malloc(LinxDev->ListenerBufferSize);
	
	LinxDev->DebugPrintln("Starting Listener...\n");
	
	ListenerChan = uartChan;
	unsigned long acutalBaud = 0;
	ListenerChan = uartChan;
	LinxDev->UartOpen(ListenerChan, 9600, &acutalBaud);
	
	State = CONNECTED;
}

int LinxSerialListener::Connected()
{
	unsigned char bytesAvailable = 0;	
	
	//Check How Many Bytes Received, Need At Least 2 To Get SoF And Packet Size
	LinxDev->UartGetBytesAvailable(ListenerChan, &bytesAvailable);
	
	//LinxDev->DebugPrint("Received ");
	//LinxDev->DebugPrint(bytesAvailable, DEC);
	//LinxDev->DebugPrintln(" bytes");
	
	if(bytesAvailable >= 2)
	{
		//Check for valid SoF
		unsigned char bytesRead = 0;
		LinxDev->UartRead(ListenerChan, 2, recBuffer, &bytesRead);	
		
		if(recBuffer[0] == 0xFF)
		{
			//SoF is valid. Check If Entire Packet Has Been Received
			LinxDev->UartGetBytesAvailable(ListenerChan, &bytesAvailable);
		   
			if(bytesAvailable < (recBuffer[1] - 2) )
			{
				//Wait For More Bytes	
				int timeout = 100;
				while(bytesAvailable < (recBuffer[1] - 2) )
				{
					LinxDev->UartGetBytesAvailable(ListenerChan, &bytesAvailable);
					
					if(timeout <= 0)
					{						
						//Flush
						LinxDev->UartGetBytesAvailable(ListenerChan, &bytesAvailable);
						LinxDev->UartRead(ListenerChan, bytesAvailable, recBuffer, &bytesRead);
						return -1;
					}
					LinxDev->DelayMs(1);
					timeout--;
				}
			}
						
			//Full Packet Received			
			LinxDev->UartRead(ListenerChan, (recBuffer[1] - 2), (recBuffer+2), &bytesRead);			
									
			//Full Packet Received - Compute Checksum - Process Packet If Checksum Passes
			if(ChecksumPassed(recBuffer))
			{		
				LinxDev->DebugPrintPacket(RX, recBuffer);
				
				//Process Packet
				int status = ProcessCommand(recBuffer, sendBuffer);
				if(status == L_DISCONNECT)
				{
					State = CLOSE;
				}
			
				//Send Response Packet 
				LinxDev->DebugPrintPacket(TX, sendBuffer);
				LinxDev->UartWrite(ListenerChan, sendBuffer[1], sendBuffer);		
			}
			else
			{
				//Flush
				LinxDev->UartGetBytesAvailable(ListenerChan, &bytesAvailable);
				LinxDev->UartRead(ListenerChan, bytesAvailable, recBuffer, &bytesRead);
			}
		}
		else
		{			
			//Flush
			LinxDev->UartGetBytesAvailable(ListenerChan, &bytesAvailable);
			LinxDev->UartRead(ListenerChan, bytesAvailable, recBuffer, &bytesRead); 
		}
	}
	else
	{
		//No New Packet
		if (periodicTasks[0] != NULL)
		{
			periodicTasks[0](0,0);
		}
		else
		{
			#ifdef LINX_DEVICE_FAMILY=4 | LINX_DEVICE_FAMILY=6
			LinxDev->DelayMs(30);
			#endif
		}
	}
	
	return 0;
}

int LinxSerialListener::Close()
{
	LinxDev->UartClose(ListenerChan);
	State = START;
	return 0;
}

int LinxSerialListener::Exit()
{
	return -1;
}

int LinxSerialListener::CheckForCommands()
{
	switch(State)
	{				
		case START:  
			Start(LinxDev, ListenerChan);
			break;
		case CONNECTED:  
			//LinxDev->DebugPrintln("State - Connected");
			Connected();
			break;
		case CLOSE:    			
			LinxDev->DebugPrintln("State - Close");
			Close();
			break;	
		case EXIT:
			LinxDev->DebugPrintln("State - Exit");
			Exit();
			break;				
	}
}

// Pre Instantiate Object
LinxSerialListener LinxSerialConnection = LinxSerialListener();