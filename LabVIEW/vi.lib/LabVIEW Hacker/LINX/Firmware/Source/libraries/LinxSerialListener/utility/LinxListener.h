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
		LinxListenerState State;
		LinxListenerInterface Interface;
		unsigned char ListenerChan;
		
		unsigned char bufferSize;
		unsigned char recBuffer[LISTENER_BUFFER_SIZE];
		unsigned char sendBuffer[LISTENER_BUFFER_SIZE];
		
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
		
		
		int ProcessCommand(unsigned char* recBuffer, unsigned char* sendBuffer, LinxDevice& LinxDev);
		void PacketizeAndSend(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, unsigned int dataSize, int status);
		void StatusResponse(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, int status);
		void DataBufferResponse(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, const unsigned char* dataBuffer, unsigned char dataSize, int status);
		unsigned char ComputeChecksum(unsigned char* packetBuffer);
		bool ChecksumPassed(unsigned char* packetBuffer);
		
};


#endif //LINX_LISTENER_H