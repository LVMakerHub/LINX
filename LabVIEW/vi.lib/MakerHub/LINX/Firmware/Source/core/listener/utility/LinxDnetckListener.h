/****************************************************************************************
**  LINX header for DNETcK listener.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_DNETCK_LISTENER_H
#define LINX_DNETCK_LISTENER_H

/****************************************************************************************
**  Defines
****************************************************************************************/		
#define LINX_TCP_BUFFER_SIZE 128

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include "LinxListener.h"
#include "LinxDevice.h"
#include <DNETcK.h> 

/****************************************************************************************
**  Type Defs
****************************************************************************************/		

/****************************************************************************************
**  Classes
****************************************************************************************/		
class LinxDnetckListener : public LinxListener
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		LinxListenerState State;		
		
		IPv4 IpAddress;
		unsigned short LinxTcpPort;
		
		TcpServer LinxTcpServer;
		TcpClient LinxTcpClient;
		int LinxTcpClientCount;		
		unsigned int LinxTcpStartTime;
		unsigned int LinxTcpTimeout;
		DNETcK::STATUS LinxTcpStatus;
		
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxDnetckListener();		//Default Constructor
		
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

#endif //LINX_DNETCK_LISTENER_H