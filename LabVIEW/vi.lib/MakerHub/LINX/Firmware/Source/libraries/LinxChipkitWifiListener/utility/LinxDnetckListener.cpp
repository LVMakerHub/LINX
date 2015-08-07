/****************************************************************************************
**  LINX DEIPcK listener code.
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

#include "LinxDevice.h"
#include "LinxListener.h"

#include "LinxDnetckListener.h"
#include <DEIPcK.h> 

/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxDEIPcKListener::LinxDEIPcKListener()
{
	State = START;
	Interface = TCP;
	
	LinxTcpClientCount = 0;
	LinxTcpStartTime = 0;
	LinxTcpTimeout = 5000;
	
	
	LinxListenerState State;		
	
	TCPServer LinxTcpServer;
	TCPSocket LinxTcpClient;
	TCPSocket * LinxTcpClientPtr = NULL;		
}

/****************************************************************************************
**  Functions
****************************************************************************************/
int LinxDEIPcKListener::Start(LinxDevice* linxDev)
{
	LinxDev = linxDev;
}

int LinxDEIPcKListener::Listen()
{
	return -1;
}
		
int LinxDEIPcKListener::Available()
{
	return -1;
}

int LinxDEIPcKListener::Accept()
{
	return -1;
}


int LinxDEIPcKListener::Connected()
{
	return -1;
}

int LinxDEIPcKListener::Close()
{
	return -1;
}

int LinxDEIPcKListener::Exit()
{
	return -1;
}

int LinxDEIPcKListener::CheckForCommands()
{
	return -1;
}
