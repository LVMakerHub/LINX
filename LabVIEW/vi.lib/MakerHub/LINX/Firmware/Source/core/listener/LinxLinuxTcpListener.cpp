/****************************************************************************************
**  LINX Linux TCP listener code.
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
#include "utility/LinxDevice.h"
#include "utility/LinxListener.h"
#include "LinxLinuxTcpListener.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxLinuxTcpListener::LinxLinuxTcpListener()
{
	State = START;
	Interface = TCP;
	TcpPort = 44300;
	TcpTimeout.tv_sec = 10;		//Set Socket Time-out To Default Value
}

/****************************************************************************************
**  Functions
****************************************************************************************/

int LinxLinuxTcpListener::Start(LinxDevice* linxDev, unsigned short port)
{
	LinxDev = linxDev;

	recBuffer = (unsigned char*) malloc(LinxDev->ListenerBufferSize);
	sendBuffer = (unsigned char*) malloc(LinxDev->ListenerBufferSize);

	LinxDev->DebugPrintln("Starting Linux TCP Listener...");
	
	//Create the TCP socket
	if((ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
	{
		LinxDev->DebugPrintln("Failed To Create Socket");
		State = EXIT;
		return -1;
	}
	else
	{
		LinxDev->DebugPrintln("Successfully Created Socket");
	}

	//Construct the server sockaddr_in structure
	TcpPort = port;														//Store Specified Port In Case It's Needed Later
	memset(&TcpServer, 0, sizeof(TcpServer));			//Clear Struct
	TcpServer.sin_family = AF_INET;							//Internet/IP
	TcpServer.sin_addr.s_addr = htonl(INADDR_ANY); 	//Incoming Addr
	TcpServer.sin_port = htons(port);							//Server Port
	
	//Bind the server socket
	if( bind(ServerSocket, (struct sockaddr *) &TcpServer, sizeof(TcpServer)) < 0)
	{
		LinxDev->DebugPrintln("Failed To Bind Sever Socket");
		State = EXIT;
		return -1;
	}
	else
	{
		LinxDev->DebugPrintln("Successfully Bound Sever Socket");
	}
	
	//Listen on the server socket
	if(listen(ServerSocket, MAX_PENDING_CONS) < 0)
	{
		LinxDev->DebugPrintln("Failed To Start Listening On Sever Socket");
		State = EXIT;
		return -1;
	}
	else
	{
		LinxDev->DebugPrintln("Successfully Started Listening On Sever Socket");
		State = LISTENING;
	}
	
	return 0;
}

int LinxLinuxTcpListener::Listen()
{
	LinxDev->DebugPrintln("Waiting For Client Connection\n");
	
	unsigned int clientlen = sizeof(TcpClient);
	
	if( (ClientSocket = accept(ServerSocket, (struct sockaddr *) &TcpClient, &clientlen)) < 0)
	{
		("Failed To Accept Client Connection\n");
		State = EXIT;
		return -1;
	}
	else
	{		
		if ( setsockopt (ClientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&TcpTimeout, sizeof(TcpTimeout)) < 0)
		{
			LinxDev->DebugPrintln("Failed To Set Socket Receive Time-out\n");
			return -1;
		}
		else
		{	
			TcpUpdateTime = LinxDev->GetSeconds();
			State = CONNECTED;
			LinxDev->DebugPrintln(inet_ntoa(TcpClient.sin_addr));
			LinxDev->DebugPrintln("Successfully Connected\n");
		}		
	}
	return 0;	
}

int LinxLinuxTcpListener::Connected()
{	
	int received = -1;
	unsigned char packetSize = 0;
	errno = 0;
	
	//Clear SoF
	recBuffer[0] = 0;
	
	//Watch For New Packet
	received = peek(recBuffer, LinxDev->ListenerBufferSize);
	
	//Wait For At Least First Two Bytes Of Packet
	if(received >= 2)
	{
		//Check SoF and Packet Size
		if(recBuffer[0] == 0xFF)
		{
			//Valid SoF, Check Packet Size
			packetSize = recBuffer[1];
			if(packetSize < received )
			{
				//Partial Packet, Make Sure Packet Size Will Fit In Buffer, If It Will Loop To Wait For Remainder Of Packet
				if(packetSize > LinxDev->ListenerBufferSize)
				{
					LinxDev->DebugPrintln("Packet Size Too Large For Buffer");
					State = EXIT;
					return -1;
				}
				return 0;
			}			
			else
			{
				//Full Packet In Receive Buffer
				if( received = read(ClientSocket, recBuffer, packetSize) < 0 )
				{
					//Failed To Read Packet From Buffer
					LinxDev->DebugPrintln("Failed To Read Packet From Buffer");
					State = EXIT;
					return -1;				
				}
				else
				{
					//Check Checksum
					if(ChecksumPassed(recBuffer))
					{				
						LinxDev->DebugPrintPacket(RX, recBuffer);
						
						//Process Packet Handle Any Networking Packets
						int status = ProcessCommand(recBuffer, sendBuffer);
						if(status == L_DISCONNECT)
						{
							//Host Disconnected.  Listen For New Connection														
							LinxDev->DebugPrintln("Disconnect");
							State = LISTENING;							
						}
										
						
						//Send Response Packet
						LinxDev->DebugPrintPacket(TX, sendBuffer);
						unsigned char bytesToSend = sendBuffer[1];
						if( send(ClientSocket, sendBuffer, bytesToSend, 0) != bytesToSend)
						{
							LinxDev->DebugPrintln("Failed To Send Response Packet");
							State = EXIT;
							return -1;
						}
						return 0;
					}
					else
					{
						//Checksum Failed
						LinxDev->DebugPrintln("Checksum Failed");
						recv(ClientSocket, recBuffer, LinxDev->ListenerBufferSize, MSG_DONTWAIT);
					}
				}
			}
		}
		else
		{
			//Bad SoF, Flush Socket
			LinxDev->DebugPrintln("Bad SoF");
			recv(ClientSocket, recBuffer, LinxDev->ListenerBufferSize, MSG_DONTWAIT);
			printf("Got %s\n", recBuffer);
		}
	}
}
 
 
int LinxLinuxTcpListener::Exit()
{
	close(ServerSocket);
	close(ClientSocket);
	State = LISTENING;
	
	return 0;
}

int LinxLinuxTcpListener::Close()
{
	State = LISTENING;
	
	return 0;
}


int LinxLinuxTcpListener::peek(unsigned char * recBuffer, int bufferSize)
{
	int peekReceived = -1;
	errno = 0;
	
	if( (peekReceived = recv(ClientSocket, recBuffer, bufferSize, MSG_PEEK)) < 0)
	{
		//Time-out Or Error
		if(errno == EWOULDBLOCK)
		{
			//Time-out Waiting For Data
			LinxDev->DebugPrintln("Time-out Waiting For Data");			
		}			
		else
		{
			State = EXIT;
			return peekReceived;
		}
	}
	else	 if(peekReceived == 0)
	{		
		//Client Disconnected
		LinxDev->DebugPrintln("Client Disconnected");			
		State = LISTENING;		
		return peekReceived;		
	}
	else
	{
		//Data Received
		return peekReceived;		
	}
}

int LinxLinuxTcpListener::CheckForCommands()
{	
	switch(State)
	{				
		case START:
			LinxDev->DebugPrintln("State - Start");
			Start(LinxDev, TcpPort);			
			break;
		case LISTENING:  
			LinxDev->DebugPrintln("State - Listening");
			Listen();
			break;
		case CONNECTED:  
			//LinxDev->DebugPrintln("State - Connected");
			Connected();
			break;
		case CLOSE:    			
			LinxDev->DebugPrintln("State - Close");
			Close();
			break;	
		case EXIT:
			LinxDev->DebugPrintln("State - Exit");
			Exit();
			exit(-1);
			break;				
	}
	return L_OK;
}

// Pre Instantiate Object
LinxLinuxTcpListener LinxTcpConnection = LinxLinuxTcpListener();
