/****************************************************************************************
**  Includes
****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utility\LinxDevice.h"

#include "utility\LinxListener.h"
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
int LinxSerialListener::Start(LinxDevice &linxDev, unsigned char uartChan)
{
	ListenerChan = uartChan;
	unsigned long acutalBaud = 0;
	ListenerChan = uartChan;
	linxDev.UartOpen(ListenerChan, 9600, &acutalBaud);
	
	State = CONNECTED;
}

int LinxSerialListener::Connected(LinxDevice &linxDev)
{
	unsigned char bytesAvailable = 0;	
	
	//Check How Many Bytes Received, Need At Least 2 To Get SoF And Packet Size
	linxDev.UartGetBytesAvailable(ListenerChan, &bytesAvailable);
	
	if(bytesAvailable > 0)
	{
		Serial1.print("Bytes At Port:  ");	
		Serial1.println(bytesAvailable, DEC);
	}
	
	if(bytesAvailable >= 2)
	{	
		//DEBUGDEBUG(">2 Bytes Available");
		//Check for valid SoF
		unsigned char bytesRead = 0;
		linxDev.UartRead(ListenerChan, 2, recBuffer, &bytesRead);	
		
		//DEBUG
		Serial1.print("Got Frist Two ");
		Serial1.print("[");
		Serial1.print(recBuffer[0], HEX);
		Serial1.print("][");
		Serial1.print(recBuffer[1], HEX);
		Serial1.print("] ");
		Serial1.println("");
		
		if(recBuffer[0] == 0xFF)
		{
			//SoF is valid. Check If Entire Packet Has Been Received
			linxDev.UartGetBytesAvailable(ListenerChan, &bytesAvailable);
		   
			if(bytesAvailable < (recBuffer[1] - 2) )
			{
				//DEBUGDEBUG("Waiting for rest of packet\n");
				//Wait For More Bytes	
				int timeout = 100;
				while(bytesAvailable < (recBuffer[1] - 2) )
				{
					//DEBUGDEBUG("WAITING!!!");
					linxDev.UartGetBytesAvailable(ListenerChan, &bytesAvailable);
					
					//DEBUG
					Serial1.print("Bytes At Port:  ");	
					Serial1.println(bytesAvailable, DEC);
					
					if(timeout <= 0)
					{
						//DEBUG
						Serial1.println("Timeout Waiting For Rest Of Packet");
						//Flush
						linxDev.UartGetBytesAvailable(ListenerChan, &bytesAvailable);
						linxDev.UartRead(ListenerChan, bytesAvailable, recBuffer, &bytesRead);
						return -1;
					}
					delay(1);
					timeout--;
				}
			}
						
			//Full Packet Received			
			linxDev.UartRead(ListenerChan, (recBuffer[1] - 2), (recBuffer+2), &bytesRead);
			
			//DEBUG
			Serial1.print("Received ");
			for(int i=0; i< recBuffer[1]; i++)
			{
				Serial1.print("[");
				Serial1.print(recBuffer[i], HEX);
				Serial1.print("] ");
			}
			Serial1.println("");
						
			//Full Packet Received - Compute Checksum - Process Packet If Checksum Passes
			if(ChecksumPassed(recBuffer))
			{		
				//DEBUGDEBUG("Checksum Passed\n");
				//Process Packet
				ProcessCommand(recBuffer, sendBuffer, linxDev);
				
				//DEBUG
				Serial1.print("Sending ");
				for(int i=0; i< sendBuffer[1]; i++)
				{
					Serial1.print("[");
					Serial1.print(sendBuffer[i], HEX);
					Serial1.print("] ");
				}
				Serial1.println("");
			
				//Send Response Packet      
				linxDev.UartWrite(ListenerChan, sendBuffer[1], sendBuffer);		
			}
			else
			{         
				//DEBUG
				Serial1.println("Checksum Failed");
				//Flush
				linxDev.UartGetBytesAvailable(ListenerChan, &bytesAvailable);
				linxDev.UartRead(ListenerChan, bytesAvailable, recBuffer, &bytesRead);
			}
		}
		else
		{
			//DEBUG
			Serial1.println("SoF Failed");
			//Flush
			linxDev.UartGetBytesAvailable(ListenerChan, &bytesAvailable);
			linxDev.UartRead(ListenerChan, bytesAvailable, recBuffer, &bytesRead); 
		}
	}
	
	return 0;
}

int LinxSerialListener::Close()
{
	return -1;
}

int LinxSerialListener::Exit()
{
	return -1;
}
