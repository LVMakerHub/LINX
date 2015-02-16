/****************************************************************************************
**  LINX ChipKIT Wifi listener code.
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
int LinxChipkitWifiListener::SetSsid(const char ssid[])
{
	//Copy User SSID Into Member Var
	for(int  i=0; i<32; i++)
	{
		LinxWifiSsid[i] = ssid[i];
	}	
	return L_OK;
}

int LinxChipkitWifiListener::SetSecurity(SecurityType securityType)
{
	LinxWifiSecurity = securityType;
	return L_OK;
}

int LinxChipkitWifiListener::SetPassphrase(const char pw[])
{
	//Copy User PW Into Member Var
	for(int  i=0; i<32; i++)
	{
		LinxWifiPw[i] = pw[i];
	}	
	return L_OK;
}

//Start With IP And Port Saved In NVS
int LinxChipkitWifiListener::Start(LinxDevice* linxDev)
{
	LinxDev = linxDev;
	LinxDev->DebugPrintln("Network Wifi Stack :: Starting With NVS Data");
	
	//Load Stored WIFI Values
	int ssidSize = LinxDev->NonVolatileRead(NVS_WIFI_SSID_SIZE);
	for(int i=0; i<ssidSize; i++)
	{
		LinxWifiSsid[i] = LinxDev->NonVolatileRead(NVS_WIFI_SSID + i);
	}
	
	int pwSize =LinxDev->NonVolatileRead(NVS_WIFI_PW_SIZE);
	for(int i=0; i<pwSize; i++)
	{
		LinxWifiPw[i] = LinxDev->NonVolatileRead(NVS_WIFI_PW + i);
	}
	
	LinxWifiSecurity = (SecurityType)LinxDev->NonVolatileRead(NVS_WIFI_SECURITY_TYPE);
	
	IPv4 deviceIpAddress = {LinxDev->NonVolatileRead(NVS_WIFI_IP), LinxDev->NonVolatileRead(NVS_WIFI_IP+1), LinxDev->NonVolatileRead(NVS_WIFI_IP+2), LinxDev->NonVolatileRead(NVS_WIFI_IP+3)};
	LinxDev->WifiIp = deviceIpAddress.rgbIP[0]<<24 | deviceIpAddress.rgbIP[1] <<16 | deviceIpAddress.rgbIP[2] << 8 | deviceIpAddress.rgbIP[3];
	unsigned short port = (LinxDev->NonVolatileRead(NVS_WIFI_PORT) << 8) +  (LinxDev->NonVolatileRead(NVS_WIFI_PORT+1)) ;
	
	
	
	StartStage2(deviceIpAddress, port);
	
	return L_OK;
}

int LinxChipkitWifiListener::Start(LinxDevice* linxDev, unsigned char ip3, unsigned char ip2, unsigned char ip1, unsigned char ip0, unsigned short port)
{
	LinxDev = linxDev;
	
	LinxDev->DebugPrintln("Network Wifi Stack :: Starting With Fixed IP Address");
		
	IPv4 deviceIpAddress = {ip3, ip2, ip1, ip0};
	
	StartStage2(deviceIpAddress, port);
	
	return L_OK;
	
}


//Common Part Of Network Stack Bring Up
int LinxChipkitWifiListener::StartStage2(IPv4 deviceIpAddress, unsigned short port)
{
	
	LinxTcpPort = port;

	int LinxWifiConnectionId = DWIFIcK::INVALID_CONNECTION_ID;

	//SSID
	const char * szSsid; 
	szSsid = LinxWifiSsid;

	//PW
	const char * szPassPhrase;
	szPassPhrase = LinxWifiPw;  

	DWIFIcK::WPA2KEY key;
	
	for(int i=0; i<64; i++)
	{
		key.rgbKey[i] = LinxWifiPw[i];
	}
	
	//LINX WIFI DEBUG INFO
	LinxDev->DebugPrintln("");
	LinxDev->DebugPrintln("");
	LinxDev->DebugPrintln(".: LINX WIFI SETTINNGS :.");
	
	LinxDev->DebugPrint("IP Address : ");
	LinxDev->DebugPrint(deviceIpAddress.rgbIP[0], DEC);
	LinxDev->DebugPrint(".");
	LinxDev->DebugPrint(deviceIpAddress.rgbIP[1], DEC);
	LinxDev->DebugPrint(".");
	LinxDev->DebugPrint(deviceIpAddress.rgbIP[2], DEC);
	LinxDev->DebugPrint(".");
	LinxDev->DebugPrintln(deviceIpAddress.rgbIP[3], DEC);
	
	LinxDev->DebugPrint("Port       : ");
	LinxDev->DebugPrintln(LinxTcpPort, DEC);
	
	LinxDev->DebugPrint("SSID       : ");
	LinxDev->DebugPrintln(szSsid);
	
	LinxDev->DebugPrint("Security   : ");
	switch(LinxWifiSecurity)
	{
		case NONE:
			//No Security		
			LinxDev->DebugPrintln("None");
			break;
		case WPA2_PASSPHRASE:
			//WPA2 Passphrase
				LinxDev->DebugPrintln("WPA2 Passphrase");
			break;
		case WPA2_KEY:
			//WPA2 Key  --Untested--
				LinxDev->DebugPrintln("WPA2 Key");
			break;
		case WEP40:
				LinxDev->DebugPrintln("WEP40 - Not Implemented");
			break;
		case WEO104:
				LinxDev->DebugPrintln("WEP104 - Not Implemented");
			break;
		default:			
			break;     
	}
	
	LinxDev->DebugPrint("Passphrase : ");
	LinxDev->DebugPrintln(szPassPhrase);	
	
	LinxDev->DebugPrintln("");
	LinxDev->DebugPrintln("");	

	switch(LinxWifiSecurity)
	{
		case NONE:
			//No Security		
			LinxWifiConnectionId = DWIFIcK::connect(szSsid, &LinxTcpStatus);
			break;
		case WPA2_PASSPHRASE:
			//WPA2 Passphrase
			LinxWifiConnectionId = DWIFIcK::connect(szSsid, szPassPhrase, &LinxTcpStatus);
			break;
		case WPA2_KEY:
			//WPA2 Key  --Untested--
			LinxWifiConnectionId = DWIFIcK::connect(szSsid, szPassPhrase, &LinxTcpStatus);
			break;
		case WEP40:
			//TODO
			break;
		case WEO104:
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
	
	 //Listen For Connection On Specified Port
	if(LinxTcpServer.startListening(LinxTcpPort))
	{
		State = LISTENING;
		  
		LinxDev->DebugPrint("WIFI Starting To Listen On Port ");
		LinxDev->DebugPrintln(LinxTcpPort, DEC);
	}
	else
	{
		LinxDev->DebugPrint("Unable To Start Listenging On Wifi Port ");
		LinxDev->DebugPrintln(LinxTcpPort);
		
		State = EXIT;        
	}
		
	return L_OK;
}

int LinxChipkitWifiListener::Listen()
{
	if( (LinxTcpClientCount = LinxTcpServer.availableClients()) > 0)
	{
		//Should Move To Available But There Is Nothing To Do There For Now
		State = ACCEPT;		
	}
	else
	{
		//No New Clients
	}	
	
	return L_OK;
}

int LinxChipkitWifiListener::Available()
{
	//Not Used
	return L_OK;
}

int LinxChipkitWifiListener::Accept()
{
	//Accept The Client Connection
	
	//Close Any Previous Connections Just In Case
	LinxTcpClient.close();

	if(LinxTcpServer.acceptClient(&LinxTcpClient))
	{
		State = CONNECTED;
		LinxTcpStartTime= (unsigned) LinxDev->GetMilliSeconds();		 
		LinxDev->DebugPrintln("Client Connected...");		
	}
	else
	{
		State = CLOSE;		
		LinxDev->DebugPrintln("Failed To Accept Wifi Connection...");
		LinxDev->DebugPrint("Wifi Status => ");         
		LinxDev->DebugPrintln(LinxTcpStatus, DEC);		
	}
	
	return L_OK;
}

int LinxChipkitWifiListener::Connected()
{
	//Read Wifi TCP Bytes	
	
	//If There Are Bytes Available Have A Look, If Not Loop (Remain In Read Unless Timeout)
	if(LinxTcpClient.available() > 0)
	{
		//Read First Byte, Check If It Is SoF (0xFF)
		if ( (recBuffer[0] = LinxTcpClient.readByte()) == 0xFF)
		{               
			LinxDev->DebugPrintln("Network Stack :: SoF Received");
			
			//SoF Received, Reset LinxTcpStartTime
			LinxTcpStartTime = (unsigned)millis();

			//SoF Received Wait For Packet Size
			while(LinxTcpClient.available() < 1)
			{
				if( ((unsigned)millis() - LinxTcpStartTime) > LinxTcpTimeout)
				{
					State = CLOSE;
					LinxDev->DebugPrintln("Network Stack :: Rx Timeout (0)");
				}             
			}
			
			recBuffer[1] = LinxTcpClient.readByte();
			
			LinxDev->DebugPrintln("Network Stack :: Packet Size Received");

			//Read Remainder Of Packet As It Comes In
			for (int i=0; i< recBuffer[1]-2; i++)
			{
				while(LinxTcpClient.available() < 1)
				{
					if( ((unsigned)millis() - LinxTcpStartTime) > LinxTcpTimeout)
					{
						State = CLOSE;
						LinxDev->DebugPrintln("Network Stack :: Rx Timeout (1)");
					}
				} 
				recBuffer[i+2] = LinxTcpClient.readByte();
			}
			LinxDev->DebugPrintln("Network Stack :: Full Packet Received");

			//Checksum
			if(ChecksumPassed(recBuffer))
			{			
				LinxDev->DebugPrintln("Network Stack :: Packet Checksum Passed");
				
				//Process Command And Respond
				ProcessCommand(recBuffer, sendBuffer);
				LinxTcpClient.writeStream(sendBuffer, sendBuffer[1]);            
			}
			else
			{
				LinxDev->DebugPrintln("Network Stack :: Checksum Failed");       
			}         
		}
		else
		{
			State = CLOSE;
			LinxDev->DebugPrintln("Network Stack :: SoF Failed");       
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
	
	return L_OK;
}

int LinxChipkitWifiListener::Close()
{
	//Close TCP Connection, Return To Listening State	
	LinxDev->DebugPrintln("Closing Wifi TCP Connection...");
	LinxTcpClient.close();
	State = LISTENING;

	return L_OK;
}

int LinxChipkitWifiListener::Exit()
{
	//Something Went Wrong.  Try To Close Connection And Quit.
	LinxTcpClient.close();
	LinxTcpServer.close();
	State = START;	
	
	LinxDev->DebugPrintln("Restarting Wifi Server...");
	
	return L_OK;
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