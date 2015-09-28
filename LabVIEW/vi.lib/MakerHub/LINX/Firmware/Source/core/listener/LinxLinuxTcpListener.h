/****************************************************************************************
**  LINX header for Linux TCP listener.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_LINUX_TCP_LISTENER_H
#define LINX_LINUX_TCP_LISTENER_H


#ifndef MAX_PENDING_CONS
	#define MAX_PENDING_CONS 2
#endif

/****************************************************************************************
**  Includes
****************************************************************************************/		
#include "utility/LinxListener.h"
#include "utility/LinxDevice.h"

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>


/****************************************************************************************
**  Classes
****************************************************************************************/		
class LinxLinuxTcpListener : public LinxListener
{
	public:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		unsigned long TcpUpdateTime;
		struct timeval TcpTimeout;
		
		unsigned short TcpPort;
		int ServerSocket;
		int ClientSocket;		
	
		struct sockaddr_in TcpServer;
		struct sockaddr_in TcpClient;
		
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxLinuxTcpListener();		//Default Constructor
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/		
		int Start(LinxDevice* linxDev, unsigned short port);
		int Listen();
		int Connected();
		int Close();
		int Exit();
		
		virtual int CheckForCommands();
		
	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
				
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		int peek(unsigned char * recBuffer, int bufferSize);
};

extern LinxLinuxTcpListener LinxTcpConnection;

#endif //LINX_LINUX_TCP_LISTENER_H