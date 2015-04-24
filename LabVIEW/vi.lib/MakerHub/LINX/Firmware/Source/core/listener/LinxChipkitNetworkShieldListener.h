/****************************************************************************************
**  LINX header for ChipKIT Network Shield listener.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
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
#include "LinxSerialListener.h"
#include "utility\LinxListener.h"
#include "utility\LinxDevice.h"
#include "utility\LinxDEIPcKListener.h"

/****************************************************************************************
**  Type Defs
****************************************************************************************/		

/****************************************************************************************
**  Classes
****************************************************************************************/		
class LinxChipkitNetworkShieldListener : public LinxDEIPcKListener
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		LinxListenerState State;
		
		
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxChipkitNetworkShieldListener();		//Default Constructor
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/		
		virtual int Start(LinxDevice* linxDev);
		virtual int Start(LinxDevice* linxDev, unsigned char ip3, unsigned char ip2, unsigned char ip1, unsigned char ip0, unsigned short port);
		virtual int StartStage2(IPv4 deviceIpAddress, unsigned short port);
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

extern LinxChipkitNetworkShieldListener LinxEthernetConnection;

#endif //LINX_CHIPKITNETWORKSHIELD_LISTENER_H