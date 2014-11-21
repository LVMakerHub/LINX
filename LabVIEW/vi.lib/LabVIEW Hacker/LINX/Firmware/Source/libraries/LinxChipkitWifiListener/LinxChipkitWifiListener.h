/****************************************************************************************
**  LINX header for ChipKIT Wifi listener.
**
**  For more information see:           www.labviewhacker.com/linx
**  For support visit the forums at:    www.labviewhacker.com/forums/linx
**  
**  Written By Sam Kristoff
**
** MIT license.
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
#include "utility\LinxListener.h"
#include "utility\LinxDevice.h"
#include "utility\LinxDnetckListener.h"

/****************************************************************************************
**  Type Defs
****************************************************************************************/		

/****************************************************************************************
**  Classes
****************************************************************************************/		
class LinxChipkitWifiListener : public LinxDnetckListener
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		LinxListenerState State;
		
		unsigned long LinxWifiIp;
		unsigned short LinxWifiPort;
		unsigned char LinxWifiSsidSize;
		char LinxWifiSsid[32];
		unsigned char LinxWifiSecurity;
		unsigned char LinxWifiPwSize;
		char LinxWifiPw[64];
		
		unsigned char wifiServerFail;
		
		
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxChipkitWifiListener();		//Default Constructor
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/		
		virtual int Start(LinxDevice* linxDev, unsigned char ip3, unsigned char ip2, unsigned char ip1, unsigned char ip0, unsigned short port);
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
				
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
	
};

extern LinxChipkitWifiListener LinxWifiConnection;

#endif //LINX_CHIPKITNETWORKSHIELD_LISTENER_H