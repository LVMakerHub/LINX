/****************************************************************************************
**  LINX ChipKIT Network Shield listener code.
**
**  For more information see:           www.labviewhacker.com/linx
**  For support visit the forums at:    www.labviewhacker.com/forums/linx
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

#include "utility\LinxDevice.h"
#include "utility\LinxListener.h"
#include "utility\LinxDnetckListener.h"
#include "LinxChipkitNetworkShieldListener.h"

#include <DNETcK.h> 

/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxChipkitNetworkShieldListener::LinxChipkitNetworkShieldListener()
{
	State = START;
	
	LinxTcpClientCount = 0;
	LinxTcpStartTime = 0;
	LinxTcpTimeout = 5000;
	LinxTcpPort = 0;			//Default Port This Will Be Overwritten When Users Calls Stat()
}

/****************************************************************************************
**  Functions
****************************************************************************************/
int LinxChipkitNetworkShieldListener::Start(LinxDevice* linxDev)
{
	LinxDev = linxDev;
	LinxDev->DebugPrintln("Network Ethernet Stack :: Starting With NVS Data");
	
	//Load Stored Ethernet Config Values	
	IPv4 deviceIpAddress = {LinxDev->NonVolatileRead(NVS_ETHERNET_IP), LinxDev->NonVolatileRead(NVS_ETHERNET_IP+1), LinxDev->NonVolatileRead(NVS_ETHERNET_IP+2), LinxDev->NonVolatileRead(NVS_ETHERNET_IP+3)};
	LinxDev->ethernetIp = deviceIpAddress.rgbIP[0]<<24 | deviceIpAddress.rgbIP[1] <<16 | deviceIpAddress.rgbIP[2] << 8 | deviceIpAddress.rgbIP[3];
	unsigned short port = (LinxDev->NonVolatileRead(NVS_ETHERNET_PORT) << 8) +  (LinxDev->NonVolatileRead(NVS_ETHERNET_PORT+1)) ;
		
	StartStage2(deviceIpAddress, port);
}

int LinxChipkitNetworkShieldListener::Start(LinxDevice* linxDev, unsigned char ip3, unsigned char ip2, unsigned char ip1, unsigned char ip0, unsigned short port)
{	
	LinxDev = linxDev;
	
	LinxDev->DebugPrintln("Network Ethernet Stack :: Starting With Static Configuration");		
	
	unsigned long ethernetIP = (ip3<<24) | (ip2<<16) | (ip1<<8) | ip0;	
	IPv4 m_IpAddress = {((ethernetIP>>24)&0xFF), ((ethernetIP>>16)&0xFF), ((ethernetIP>>8)&0xFF), (ethernetIP&0xFF)};
	
	LinxTcpPort = port;
	
	StartStage2(m_IpAddress, LinxTcpPort);
		
	return L_OK;
}

int LinxChipkitNetworkShieldListener::StartStage2(IPv4 deviceIpAddress, unsigned short port)
{
	//Ethernet Config Debug Info
	LinxDev->DebugPrintln("");
	LinxDev->DebugPrintln("");
	LinxDev->DebugPrintln(".: LINX ETHERNET SETTINNGS :.");
	
	LinxDev->DebugPrint("IP Address : ");
	LinxDev->DebugPrint(deviceIpAddress.rgbIP[0], DEC);
	LinxDev->DebugPrint(".");
	LinxDev->DebugPrint(deviceIpAddress.rgbIP[1], DEC);
	LinxDev->DebugPrint(".");
	LinxDev->DebugPrint(deviceIpAddress.rgbIP[2], DEC);
	LinxDev->DebugPrint(".");
	LinxDev->DebugPrintln(deviceIpAddress.rgbIP[3], DEC);
	
	LinxDev->DebugPrint("Port       : ");
	LinxDev->DebugPrintln(port, DEC);
	
	LinxDev->DebugPrintln("");
	LinxDev->DebugPrintln("");
	
	//Start Ethernet Stack
	DNETcK::begin(deviceIpAddress);
	 
	//Listen For Connection On Specified Port
	if(LinxTcpServer.startListening(port))
	{
		//Started Listening On Port
		State = LISTENING;
	}
	else
	{
		//Unable To Listen On Specified Port
		State = EXIT;        
	}
	
	return L_OK;
}

int LinxChipkitNetworkShieldListener::Listen()
{
	
	//LinxDev->DebugPrintln("Network Stack :: Listening");
	
	//Listen For Clients	
	if(LinxTcpServer.isListening(&LinxTcpStatus))
	{
		//New Connection Available
		State = AVAILABLE;
		
	}
	else if(DNETcK::isStatusAnError(LinxTcpStatus))
	{
		State = EXIT;
	}
	//If No New Connection Is Available And No Error Occurred State In Listening State.
	
	return 0;
}

int LinxChipkitNetworkShieldListener::Available()
{
	//LinxDev->DebugPrintln("Network Stack :: Available");
	
	if( (LinxTcpClientCount = LinxTcpServer.availableClients()) > 0)
	{
		State = ACCEPT;
	}
	else
	{
		//No New Connection, Nothing To Do Here, Could Debug Print
	}
	
	return 0;
}

int LinxChipkitNetworkShieldListener::Accept()
{
	//LinxDev->DebugPrintln("Network Stack :: Accept");

	//Close Any Previous Connections Just In Case
	LinxTcpClient.close();

	
	if(LinxTcpServer.acceptClient(&LinxTcpClient))
	{
		State = CONNECTED;
		LinxTcpStartTime = (unsigned) millis();
		LinxDev->DebugPrint("Network Stack :: Client Connected");
	}
	else
	{
		State = CLOSE;
		LinxDev->DebugPrintln("Network Stack :: Failed To Connect Client");
	}
	return 0;
}

int LinxChipkitNetworkShieldListener::Connected()
{
	//Read Ethernet TCP Bytes
	//LinxDev->DebugPrintln("Network Stack :: Connected");
	
	//If There Are Bytes Available Have A Look, If Not Loop (Remain In Read Unless Timeout)
	if(LinxTcpClient.available() > 0)
	{
		//Read First Byte, Check If It Is SoF (0xFF)
		if ( (recBuffer[0] = LinxTcpClient.readByte()) == 0xFF)
		{               
			//LinxDev->DebugPrintln("Network Stack :: SoF Received");
			
			//SoF Received, Reset LinxTcpStartTime
			LinxTcpStartTime = (unsigned)millis();

			//SoF Received Wait For Packet Size
			while(LinxTcpClient.available() < 1)
			{
				if( ((unsigned)millis() - LinxTcpStartTime) > LinxTcpTimeout)
				{
					State = CLOSE;
					//LinxDev->DebugPrintln("Network Stack :: Rx Timeout (0)");
				}             
			}
			
			recBuffer[1] = LinxTcpClient.readByte();
			
			//LinxDev->DebugPrintln("Network Stack :: Packet Size Received");

			//Read Remainder Of Packet As It Comes In
			for (int i=0; i< recBuffer[1]-2; i++)
			{
				while(LinxTcpClient.available() < 1)
				{
					if( ((unsigned)millis() - LinxTcpStartTime) > LinxTcpTimeout)
					{
						State = CLOSE;
						//LinxDev->DebugPrintln("Network Stack :: Rx Timeout (1)");
					}
				} 
				recBuffer[i+2] = LinxTcpClient.readByte();
			}
			//LinxDev->DebugPrintln("Network Stack :: Full Packet Received");

			//Checksum
			if(ChecksumPassed(recBuffer))
			{			
				//LinxDev->DebugPrintln("Network Stack :: Packet Checksum Passed");
				
				//Process Command And Respond
				ProcessCommand(recBuffer, sendBuffer);
				LinxTcpClient.writeStream(sendBuffer, sendBuffer[1]);            
			}
			else
			{
				//LinxDev->DebugPrintln("Network Stack :: Checksum Failed");       
			}         
		}
		else
		{
			State = CLOSE;
			//LinxDev->DebugPrintln("Network Stack :: SoF Failed");       
		}

		//Data Received, Reset Timeout
		LinxTcpStartTime = (unsigned)millis();
	}
	
	//Check For Timeout
	else if( ((unsigned)millis() - LinxTcpStartTime) > LinxTcpTimeout)
	{
		State = CLOSE;
		LinxDev->DebugPrintln("Network Stack :: Ethernet Timeout");             
	}
	else
	{
		//No Data To Read But No Timeout Either, Loop To Read Again
		State = CONNECTED;
		//LinxTcpStartTime = (unsigned) millis();
		
		LinxDev->DebugPrintln(((unsigned)millis() - LinxTcpStartTime), DEC);  
		LinxDev->DebugPrintln(" mS With No Data");      
	} 
	
	return 0;
}

int LinxChipkitNetworkShieldListener::Close()
{
	//Close TCP Connection, Return To Listening State
	LinxDev->DebugPrintln("Network Stack :: Close");             
	LinxTcpClient.close();
	State = LISTENING;
	return 0;
}

int LinxChipkitNetworkShieldListener::Exit()
{
	LinxDev->DebugPrintln("Network Stack :: Exit And Restart");   
	LinxTcpServer.close();
	LinxTcpClient.close();
	State = START;
	return 0;
}

int LinxChipkitNetworkShieldListener::CheckForCommands()
{
	switch(State)
	{				
		case START:    
			Start(LinxDev, IpAddress.rgbIP[3], IpAddress.rgbIP[2], IpAddress.rgbIP[1], IpAddress.rgbIP[0], LinxTcpPort);
			break;
		case LISTENING:    
			Listen();
			break;
		case AVAILABLE:    
			Available();
			break;
		case ACCEPT:    
			Accept();
			break;
		case CONNECTED:    
			Connected();
			break;
		case CLOSE:    			
			Close();
			State = START;
			break;	
		case EXIT:
			Exit();
			break;				
	}
	
	//Every Iteration Run Periodic Network Tasks
	 DNETcK::periodicTasks(); 
	
	return 0;
}

// Pre Instantiate Object
LinxChipkitNetworkShieldListener LinxEthernetConnection;