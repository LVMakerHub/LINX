/****************************************************************************************
**  Generic LINX listener header.
**
**  For more information see:           www.labviewhacker.com/linx
**  For support visit the forums at:    www.labviewhacker.com/forums/linx
**  
**  Written By Sam Kristoff
**
** MIT license.
****************************************************************************************/	

#ifndef LINX_LISTENER_H
#define LINX_LISTENER_H

/****************************************************************************************
** Defines
****************************************************************************************/
#define LISTENER_BUFFER_SIZE 128

/****************************************************************************************
** Includes
****************************************************************************************/
#include "LinxDevice.h"

/****************************************************************************************
** Enums
****************************************************************************************/
enum LinxListenerState
{
	START, 
	LISTENING, 
	AVAILABLE, 
	ACCEPT, 
	CONNECTED, 
	CLOSE, 
	EXIT
};

enum LinxListenerInterface
{
	UART, 
	TCP	
};

class LinxListener
{
	public:	
		/****************************************************************************************
		**  Variables
		****************************************************************************************/
		LinxDevice* LinxDev;
		LinxListenerState State;
		LinxListenerInterface Interface;
		unsigned char ListenerChan;
		
		unsigned char bufferSize;
		unsigned char recBuffer[LISTENER_BUFFER_SIZE];
		unsigned char sendBuffer[LISTENER_BUFFER_SIZE];
		
		int (*customCommands[64])(unsigned char, unsigned char*, unsigned char*, unsigned char*);
		
		
		/****************************************************************************************
		**  Constructors
		****************************************************************************************/
		LinxListener();
		
		/****************************************************************************************
		** Functions
		****************************************************************************************/
		virtual int Start();			//Start Listener
		virtual int Listen();			//Listen For Connection
		virtual int Available();		//New Client Connection Available
		virtual int Accept();		//Accept New Client Connection
		virtual int Connected();	//Connected To Client
		virtual int Close();			//Close Client Connection
		virtual int Exit();			//Stop Listening, Close And Exit
		
		void AttachCustomCommand(unsigned short commandNumber, int (*function)(unsigned char, unsigned char*, unsigned char*, unsigned char*) );
		virtual int CheckForCommands();		//Execute Listener State Machine
				
		int ProcessCommand(unsigned char* recBuffer, unsigned char* sendBuffer);
		void PacketizeAndSend(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, unsigned int dataSize, int status);
		void StatusResponse(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, int status);
		void DataBufferResponse(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, const unsigned char* dataBuffer, unsigned char dataSize, int status);
		unsigned char ComputeChecksum(unsigned char* packetBuffer);
		bool ChecksumPassed(unsigned char* packetBuffer);
		
};


#endif //LINX_LISTENER_H