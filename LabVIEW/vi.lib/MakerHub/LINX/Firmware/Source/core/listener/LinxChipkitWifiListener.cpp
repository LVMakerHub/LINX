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

#include <DEIPcK.h> 
#include <DEWFcK.h> 
/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxChipkitWifiListener::LinxChipkitWifiListener()
{
	State = START;
	
	wifiState = INIT;
	LinxWifiConnectStatus = DEWFcK::INVALID_CONNECTION_ID;
	
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
	LinxDev->WifiIp = deviceIpAddress.u8[0]<<24 | deviceIpAddress.u8[1] <<16 | deviceIpAddress.u8[2] << 8 | deviceIpAddress.u8[3];
	unsigned short port = (LinxDev->NonVolatileRead(NVS_WIFI_PORT) << 8) +  (LinxDev->NonVolatileRead(NVS_WIFI_PORT+1)) ;
		
	PrintWifiInfo(deviceIpAddress, port);
	
	State = START;
	
	return L_OK;
}

int LinxChipkitWifiListener::Start(LinxDevice* linxDev, unsigned char ip3, unsigned char ip2, unsigned char ip1, unsigned char ip0, unsigned short port)
{
	LinxDev = linxDev;
	
	LinxDev->DebugPrintln("Network Wifi Stack :: Starting With Fixed IP Address");
		
	IPv4 deviceIpAddress = {ip3, ip2, ip1, ip0};
	
	PrintWifiInfo(deviceIpAddress, port);
	
	State = START;
	
	LinxDev->DebugPrint("Connecting To Wifi Network");
	return L_OK;
	
}

int LinxChipkitWifiListener::PrintWifiInfo(IPv4 deviceIpAddress, unsigned short port)
{
	LinxTcpPort = port;	

	//SSID
	const char * szSsid; 
	szSsid = LinxWifiSsid;

	//PW
	const char * szPassPhrase;
	szPassPhrase = LinxWifiPw;  
		
	//LINX WIFI DEBUG INFO
	LinxDev->DebugPrintln("");
	LinxDev->DebugPrintln("");
	LinxDev->DebugPrintln(".: LINX WIFI SETTINNGS :.");
	
	LinxDev->DebugPrint("IP Address : ");
	LinxDev->DebugPrint(deviceIpAddress.u8[0], DEC);
	LinxDev->DebugPrint(".");
	LinxDev->DebugPrint(deviceIpAddress.u8[1], DEC);
	LinxDev->DebugPrint(".");
	LinxDev->DebugPrint(deviceIpAddress.u8[2], DEC);
	LinxDev->DebugPrint(".");
	LinxDev->DebugPrintln(deviceIpAddress.u8[3], DEC);
	
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
	
	return L_OK;
}

	

int LinxChipkitWifiListener::Init()
{
	//This case will be called repeatedly until the device connects to the network and starts listening.
	/*
	switch(wifiState)
	{
		case INIT:
			LinxWifiConnectStatus = DEWFcK::INVALID_CONNECTION_ID;
			wifiState = CONNECT;			
		break;
		
		case CONNECT:
			
		break;
	}
	*/
	
	

	//SSID
	const char * szSsid; 
	szSsid = LinxWifiSsid;

	//PW
	const char * szPassPhrase;
	szPassPhrase = LinxWifiPw;  

	char key[64];
	
	for(int i=0; i<64; i++)
	{
		key[i] = LinxWifiPw[i];
	}
		
	switch(LinxWifiSecurity)
	{
		case NONE:
			//No Security		
			LinxWifiConnectStatus = deIPcK.wfConnect(szSsid, &LinxTcpStatus);
			break;
		case WPA2_PASSPHRASE:
			//WPA2 Passphrase
			LinxWifiConnectStatus = deIPcK.wfConnect(szSsid, szPassPhrase, &LinxTcpStatus);
			break;
		case WPA2_KEY:
			//WPA2 Key  --Untested--
			LinxWifiConnectStatus = deIPcK.wfConnect(szSsid, szPassPhrase, &LinxTcpStatus);
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

	//Connect To Network
	if(LinxWifiConnectStatus)
	{
		IPv4 deviceIpAddress;
		deviceIpAddress.u8[0] = LinxDev->WifiIp>>24 & 0xFF;
		deviceIpAddress.u8[1] = LinxDev->WifiIp>>16 & 0xFF;
		deviceIpAddress.u8[2] = LinxDev->WifiIp>>8 & 0xFF;
		deviceIpAddress.u8[3] = LinxDev->WifiIp & 0xFF;
		LinxDev->DebugPrintln("");
		LinxDev->DebugPrintln("Connected To Wifi Network");
		deIPcK.begin(deviceIpAddress);
		
		//Listen For Connection On Specified Port
		if(deIPcK.tcpStartListening(LinxTcpPort, LinxTcpServer))
		{		
			LinxDev->DebugPrintln("Adding Socket...");
			LinxTcpServer.addSocket(LinxTcpClient);
			State = AVAILABLE;			
		}
	}
	else if(IsIPStatusAnError(LinxTcpStatus))
	{
		LinxDev->DebugPrint("Unable To Connect, Status ");
		LinxDev->DebugPrintln(LinxTcpStatus, DEC);
		State = CLOSE;
	}
	else
	{
		LinxDev->DebugPrint(".");	
	}
	//LINXDev->delay(100);	

}


int LinxChipkitWifiListener::Listen()
{	
	if(LinxTcpServer.isListening() > 0)
	{
		State = AVAILABLE;
	}
			
	return L_OK;
}

int LinxChipkitWifiListener::Available()
{
	if((LinxTcpServer.availableClients() > 0))
	{
		LinxDev->DebugPrintln("Available Client");
		State = ACCEPT;
	}
	return L_OK;
}

int LinxChipkitWifiListener::Accept()
{
	if((LinxTcpClientPtr = LinxTcpServer.acceptClient()) != NULL && LinxTcpClientPtr->isConnected())
	{
		LinxDev->DebugPrintln("Client Connected");
		State = CONNECTED;
		LinxTcpStartTime = (unsigned)millis();
	}
	else
	{
		State = CLOSE;
	}
	
	return L_OK;
}

int LinxChipkitWifiListener::Connected()
{
	//Read Wifi TCP Bytes
	
	
	//If There Are Bytes Available Have A Look, If Not Loop (Remain In Read Unless Timeout)
	if(LinxTcpClientPtr->available() > 0)
	{
		//Read First Byte, Check If It Is SoF (0xFF)
		if ( (recBuffer[0] = LinxTcpClientPtr->readByte()) == 0xFF)
		{
			//LinxDev->DebugPrintln("Network Stack :: SoF Received");
			
			//SoF Received, Reset LinxTcpStartTime
			LinxTcpStartTime = (unsigned)millis();

			//SoF Received Wait For Packet Size
			while(LinxTcpClientPtr->available() < 1)
			{
				if( ((unsigned)millis() - LinxTcpStartTime) > LinxTcpTimeout)
				{
					State = CLOSE;
					LinxDev->DebugPrintln("Network Stack :: Rx Timeout (0)");
					break;
				}				
			}
			
			recBuffer[1] = LinxTcpClientPtr->readByte();
			
			//LinxDev->DebugPrint("Network Stack :: Packet Size Received : ");
			//LinxDev->DebugPrintln(recBuffer[1], HEX);
			
			//Wait For Rest Of Packet
			while(LinxTcpClientPtr->available() < recBuffer[1]-2)
			{
				if( ((unsigned)millis() - LinxTcpStartTime) > LinxTcpTimeout)
				{
					State = CLOSE;
					LinxDev->DebugPrintln("Network Stack :: Rx Timeout (1)");
					break;
				}				
			}
			
			//Read Packet
			LinxTcpClientPtr->readStream(&recBuffer[2],  recBuffer[1]-2);
			
			
			//DEBUG PRINT PACKET			
			LinxDev->DebugPrint("RX <= ");
			for(int i=0; i<recBuffer[1]; i++)
			{
				LinxDev->DebugPrint("[");
				LinxDev->DebugPrint(recBuffer[i], HEX);
				LinxDev->DebugPrint("] ");
			}
			LinxDev->DebugPrintln("");
			

			//Checksum
			if(ChecksumPassed(recBuffer))
			{			
				//LinxDev->DebugPrintln("Network Stack :: Packet Checksum Passed");
				
				//Process Command And Respond
				ProcessCommand(recBuffer, sendBuffer);
				LinxTcpClientPtr->writeStream(sendBuffer, sendBuffer[1]);  
				
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
		LinxDev->DebugPrintln("Network Stack :: Wifi Timeout");             
	}
	else
	{
		//No Data To Read But No Timeout Either, Loop To Read Again
		//State = CONNECTED;
		//Serial.println("No Packet...");
		//LinxTcpStartTime = (unsigned) millis();
		
		//LinxDev->DebugPrintln(((unsigned)millis() - LinxTcpStartTime), DEC);  
		//LinxDev->DebugPrintln(" mS With No Data");      
	} 
	
	return L_OK;
}

int LinxChipkitWifiListener::Close()
{
	//Close TCP Connection, Return To Listening State	
	LinxDev->DebugPrintln("Closing Wifi TCP Connection...");
	LinxTcpClientPtr->close();
	LinxTcpServer.addSocket(*LinxTcpClientPtr);
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
			//Start(LinxDev, IpAddress.u8[3], IpAddress.u8[2], IpAddress.u8[1], IpAddress.u8[0], LinxTcpPort);
			Init();
			//LinxDev->DebugPrintln("..........START..........");
			break;
		case LISTENING:    
			Listen();
			//LinxDev->DebugPrintln("..........LISTENING..........");
			break;
		case AVAILABLE:    
			Available();
			//LinxDev->DebugPrintln("..........AVAILABLE..........");
			break;
		case ACCEPT:    
			Accept();
			//LinxDev->DebugPrintln("..........ACCEPT..........");
			break;
		case CONNECTED:    
			Connected();
			LinxDev->DebugPrintln("..........CONNECTED..........");
			break;
		case CLOSE:    			
			Close();
			//LinxDev->DebugPrintln("..........CLOSE..........");
			break;	
		case EXIT:
			Exit();
			break;				
	}
	
	//Every Iteration Run Periodic Network Tasks
	 DEIPcK::periodicTasks(); 
	
	return L_OK;
}

// Pre Instantiate Object
LinxChipkitWifiListener LinxWifiConnection;