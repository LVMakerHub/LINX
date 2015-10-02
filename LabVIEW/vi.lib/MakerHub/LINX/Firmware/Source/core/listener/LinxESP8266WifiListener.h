/****************************************************************************************
**  LINX header for ESP8266 Wifi listener.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Ken Sharp
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_CHIPKITNETWORKSHIELD_LISTENER_H
#define LINX_CHIPKITNETWORKSHIELD_LISTENER_H

/****************************************************************************************
**  Defines
****************************************************************************************/		
#define LINX_SERIAL_BUFFER_SIZE 128

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include "utility/LinxListener.h"
#include "utility/LinxDevice.h"

/****************************************************************************************
**  Type Defs
****************************************************************************************/		
enum SecurityType {NONE, WPA2_PASSPHRASE, WPA2_KEY, WEP40, WEO104};
enum WifiStartState {sINIT, sCONNECT, sLISTEN};

/****************************************************************************************
**  Classes
****************************************************************************************/		
class LinxESP8266WifiListener : public LinxListener
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		LinxListenerState State;
		WifiStartState wifiState;
		int LinxWifiConnectStatus;
		
		unsigned long LinxWifiIp;
		unsigned short LinxWifiPort;
		unsigned char LinxWifiSsidSize;
		char LinxWifiSsid[32];
		SecurityType LinxWifiSecurity;
		unsigned char LinxWifiPwSize;
		char LinxWifiPw[64];
		
		unsigned char wifiServerFail;
		
		unsigned int LinxWifiStartTime;
		unsigned int LinxWifiTimeout;
		
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxESP8266WifiListener();		//Default Constructor
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/		
		virtual int SetSsid(const char ssid[]);
		virtual int SetSecurity(SecurityType securityType);
		virtual int SetPassphrase(const char pw[]);
		
		virtual int Start(LinxDevice* linxDev);
		virtual int Start(LinxDevice* linxDev, unsigned char ip3, unsigned char ip2, unsigned char ip1, unsigned char ip0, unsigned short port);
		virtual int PrintWifiInfo();
		virtual int Init();
		virtual int Listen();			
		virtual int Available();	
		virtual int Accept();				
		virtual int Connected();			
		virtual int Close();			
		virtual int Exit();
		
		virtual int CheckForCommands();
		
	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		WiFiServer* m_pWifiSvr;
		WiFiClient  m_WifiClient;
				
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
	
};

extern LinxESP8266WifiListener LinxWifiConnection;

#endif //LINX_CHIPKITNETWORKSHIELD_LISTENER_H
