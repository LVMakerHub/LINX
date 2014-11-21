/****************************************************************************************
**  LINX ChipKIT Wifi listener code.
**
**  For more information see:           www.labviewhacker.com/linx
**  For support visit the forums at:    www.labviewhacker.com/forums/linx
**  
**  Written By Sam Kristoff
**
** MIT license.
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
#include "LinxChipkitWifiListener.h"

#include <DNETcK.h> 
#include <DWIFIcK.h> 
/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxChipkitWifiListener::LinxChipkitWifiListener()
{
	State = START;
	
	unsigned char wifiServerFail = 0;
	
}

/****************************************************************************************
**  Functions
****************************************************************************************/
int LinxChipkitWifiListener::Start(LinxDevice* linxDev, unsigned char ip3, unsigned char ip2, unsigned char ip1, unsigned char ip0, unsigned short port)
{
	
	LinxDev = linxDev;
	
	LinxDev->DebugPrintln("Network Wifi Stack :: Starting");
		
	
	unsigned long deviceIp = (ip3<<24) | (ip2<<16) | (ip1<<8) | ip0;	
	IPv4 deviceIpAddress = {((deviceIp>>24)&0xFF), ((deviceIp>>16)&0xFF), ((deviceIp>>8)&0xFF), (deviceIp&0xFF)};
	
	LinxTcpPort = port;
		
	
	int LinxWifiConnectionId = DWIFIcK::INVALID_CONNECTION_ID;

	//SSID
	const char * szSsid; 
	szSsid = LinxWifiSsid;

	//PW
	const char * szPassPhrase;
	szPassPhrase = LinxWifiPw;  

	DWIFIcK::WPA2KEY key;
	
	for(int i=0; i<32; i++)
	{
		key.rgbKey[i] = LinxWifiPw[i];
	}

	switch(LinxWifiSecurity)
	{
		case 0x00:
			//No Security		
			LinxWifiConnectionId = DWIFIcK::connect(szSsid, &LinxTcpStatus);
			break;
		case 0x01:
			//WPA2 Passphrase       
			LinxWifiConnectionId = DWIFIcK::connect(szSsid, szPassPhrase, &LinxTcpStatus);
			break;
		case 0x02:
			//WPA2 Key  --Untested--
			LinxWifiConnectionId = DWIFIcK::connect(szSsid, szPassPhrase, &LinxTcpStatus);
			break;
		case 0x03:
			//TODO
			break;
		case 0x04:
			//TODO
			break;
		default:			
			break;     
	}

	if(LinxWifiConnectionId != DWIFIcK::INVALID_CONNECTION_ID)
	{	
		LinxDev->DebugPrintln("Connecting To Wifi Network");		
		
		if(DNETcK::isInitialized(&LinxTcpStatus))
		{			
			LinxDev->DebugPrintln("TCP Stack Initialized");			
			State = LISTENING;
		}
		else if(DNETcK::isStatusAnError(LinxTcpStatus))
		{			
			LinxDev->DebugPrint("Error in initializing, status: ");
			LinxDev->DebugPrintln(LinxTcpStatus, DEC);			
			State = EXIT;
		}		
	}
	else
	{   
		LinxDev->DebugPrint("Unable to connection, status: ");
		LinxDev->DebugPrintln(LinxTcpStatus, DEC);
		State = EXIT;
	}
	
	// intialize the stack with a static IP
	DNETcK::begin(deviceIpAddress); 
		
	return 0;
}

int LinxChipkitWifiListener::Listen()
{
	
	
	return 0;
}

int LinxChipkitWifiListener::Available()
{
	
	return 0;
}

int LinxChipkitWifiListener::Accept()
{
	
	return 0;
}

int LinxChipkitWifiListener::Connected()
{
	
	return 0;
}

int LinxChipkitWifiListener::Close()
{
	

	return 0;
}

int LinxChipkitWifiListener::Exit()
{
	
	return 0;
}

int LinxChipkitWifiListener::CheckForCommands()
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
LinxChipkitWifiListener LinxWifiConnection;