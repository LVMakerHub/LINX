/****************************************************************************************
**  LINX DNETcK listener code.
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

#include "LinxDevice.h"
#include "LinxListener.h"

#include "LinxDnetckListener.h"
#include <DNETcK.h> 

/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxDnetckListener::LinxDnetckListener()
{
	State = START;
	Interface = TCP;
	
	LinxTcpClientCount = 0;
	LinxTcpStartTime = 0;
	LinxTcpTimeout = 5000;
		
}

/****************************************************************************************
**  Functions
****************************************************************************************/
int LinxDnetckListener::Start(LinxDevice* linxDev)
{
	LinxDev = linxDev;
}

int LinxDnetckListener::Listen()
{
	return -1;
}
		
int LinxDnetckListener::Available()
{
	return -1;
}

int LinxDnetckListener::Accept()
{
	return -1;
}


int LinxDnetckListener::Connected()
{
	return -1;
}

int LinxDnetckListener::Close()
{
	return -1;
}

int LinxDnetckListener::Exit()
{
	return -1;
}

int LinxDnetckListener::CheckForCommands()
{
	return -1;
}
