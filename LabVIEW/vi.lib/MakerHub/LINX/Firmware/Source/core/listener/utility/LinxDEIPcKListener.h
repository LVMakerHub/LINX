/****************************************************************************************
**  LINX header for DEIPcK listener.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_DEIPcK_LISTENER_H
#define LINX_DEIPcK_LISTENER_H

/****************************************************************************************
**  Defines
****************************************************************************************/		
#define LINX_TCP_BUFFER_SIZE 128

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include "LinxListener.h"
#include "LinxDevice.h"
#include <DEIPcK.h> 

/****************************************************************************************
**  Type Defs
****************************************************************************************/		

/****************************************************************************************
**  Classes
****************************************************************************************/		
class LinxDEIPcKListener : public LinxListener
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		//LinxListenerState State;		
		
		IPv4 IpAddress;
		unsigned short LinxTcpPort;
		
		TCPServer LinxTcpServer;
		TCPSocket LinxTcpClient;
		TCPSocket * LinxTcpClientPtr;
		int LinxTcpClientCount;		
		unsigned int LinxTcpStartTime;
		unsigned int LinxTcpTimeout;
		IPSTATUS LinxTcpStatus;
		
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxDEIPcKListener();		//Default Constructor
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/		
		virtual int Start(LinxDevice* linxDev);
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

#endif //LINX_DEIPcK_LISTENER_H