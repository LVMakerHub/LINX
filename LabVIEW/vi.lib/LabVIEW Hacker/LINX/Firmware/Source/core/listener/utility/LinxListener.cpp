/****************************************************************************************
** Includes
****************************************************************************************/
#include <stdio.h>

#include "LinxListener.h"


//DEBUG
//This was added to get delay() in cmd 0006.  Delay should be added at device level
#include <WProgram.h>

/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxListener::LinxListener()
{
	State = START;
	bufferSize = LISTENER_BUFFER_SIZE;
}

/****************************************************************************************
** Functions
****************************************************************************************/
int LinxListener::Start()
{
	return -1;
}

int LinxListener::Listen()
{ 
	return -1;
}

int LinxListener::Available()
{
	return -1;
}

int LinxListener::Accept()
{
	return -1;
}
int LinxListener::Connected()
{
	return -1;
}

int LinxListener::Close()
{
	return -1;
}

int LinxListener::Exit()
{
	return -1;
}



unsigned char LinxListener::ComputeChecksum(unsigned char* packetBuffer)
{  
  unsigned char checksum = 0;
  
  //Sum All Bytes In The Packet Except The Last (Checksum Byte)
  for(int i=0; i<(packetBuffer[1] - 1); i++)
  {
    checksum += packetBuffer[i];
  }  
  return checksum; 
}


bool LinxListener::ChecksumPassed(unsigned char* packetBuffer)
{
  return (ComputeChecksum(packetBuffer) == packetBuffer[packetBuffer[1]-1]);
}


void LinxListener::StatusResponse(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, int status)
{
	PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 0, status); 
}


int LinxListener::ProcessCommand(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, LinxDevice &linxDev)
{
	//DEBUG linxDev.DebugPrintCmdPacket(commandPacketBuffer);
		
	//Store Some Local Values For Convenience
	unsigned char commandLength = commandPacketBuffer[1];
	unsigned int command = commandPacketBuffer[4] << 8 | commandPacketBuffer[5];
	
	int status = L_OK;
	
	switch(command)
	{
	
	/************************************************************************************
    * SYSTEM COMMANDS
    ************************************************************************************/
	case 0x0000: // Sync Packet        
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
	 
	case 0x0003: // Get Device ID     
		responsePacketBuffer[5] = linxDev.DeviceFamily;
		responsePacketBuffer[6] = linxDev.DeviceID;    
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 2, L_OK); 
		break;	
		
	case 0x0004: //Get LINX API Version
		responsePacketBuffer[5] = linxDev.LinxApiMajor;
		responsePacketBuffer[6] = linxDev.LinxApiMinor;
		responsePacketBuffer[7] = linxDev.LinxApiSubminor;   
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 3, L_OK); 
		break;
		
	case 0x0005: //Get UART Max Baud
		responsePacketBuffer[5] = (linxDev.UartMaxBaud>>24) & 0xFF;
		responsePacketBuffer[6] = (linxDev.UartMaxBaud>>16) & 0xFF;
		responsePacketBuffer[7] = (linxDev.UartMaxBaud>>8) & 0xFF;
		responsePacketBuffer[8] = linxDev.UartMaxBaud & 0xFF;
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, L_OK); 
		break;
	
	case 0x0006: //Set UART Listener Interface Max Baud
	{
		unsigned long targetBaud = (unsigned long)((commandPacketBuffer[6] << 24) | (commandPacketBuffer[7] << 16) | (commandPacketBuffer[8] << 8) | commandPacketBuffer[9]);
		unsigned long actualBaud = 0;
		//DEBUG linxDev.DebugPrint((char*)"Changing Listener Baud\n");
		status = linxDev.UartSetBaudRate(ListenerChan, targetBaud, &actualBaud);
		delay(1000);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
		//DEBUG linxDev.DebugPrint((char*)"Set Baud\n");
		break;
	}		
			
	case 0x0008: // Get DIO Channels
		DataBufferResponse(commandPacketBuffer, responsePacketBuffer, linxDev.DigitalChans, linxDev.NumDigitalChans, L_OK);
		break;
	
	case 0x0009: // Get AI Channels
        DataBufferResponse(commandPacketBuffer, responsePacketBuffer, linxDev.AiChans, linxDev.NumAiChans, L_OK);
       break;
	   
    case 0x000A: // Get AO Channels
        DataBufferResponse(commandPacketBuffer, responsePacketBuffer, linxDev.AoChans, linxDev.NumAoChans, L_OK);
       break;
	   
    case 0x000B: // Get PWM Channels
        DataBufferResponse(commandPacketBuffer, responsePacketBuffer, linxDev.PwmChans, linxDev.NumPwmChans, L_OK);
       break;
	   
    case 0x000C: // Get QE Channels
        DataBufferResponse(commandPacketBuffer, responsePacketBuffer, linxDev.QeChans, linxDev.NumQeChans, L_OK);
       break;
	   
    case 0x000D: // Get UART Channels
       DataBufferResponse(commandPacketBuffer, responsePacketBuffer, linxDev.UartChans, linxDev.NumUartChans, L_OK);
       break;
	   
    case 0x000E: // Get I2C Channels
       DataBufferResponse(commandPacketBuffer, responsePacketBuffer, linxDev.I2cChans, linxDev.NumI2cChans, L_OK);
       break;
	   
    case 0x000F: // Get SPI Channels
       DataBufferResponse(commandPacketBuffer, responsePacketBuffer, linxDev.SpiChans, linxDev.NumSpiChans, L_OK);
       break;
	   
    case 0x0010: // Get CAN Channels
        DataBufferResponse(commandPacketBuffer, responsePacketBuffer, linxDev.CanChans, linxDev.NumCanChans, L_OK);
       break;
		
	case 0x0011: // Disconnect
		status = L_DISCONNECT;
		break;
		
	case 0x0024: // Get Device Name
		DataBufferResponse(commandPacketBuffer, responsePacketBuffer, linxDev.DeviceName, linxDev.DeviceNameLen, L_OK);
		break;
	
	/****************************************************************************************
	**  Digital I/O
	****************************************************************************************/	
	case 0x0041: // Get Device Name
		linxDev.DigitalWrite(commandPacketBuffer[6], &commandPacketBuffer[7], &commandPacketBuffer[7+commandPacketBuffer[6]]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
		break;
		
	/****************************************************************************************
	**  Analog I/O
	****************************************************************************************/	
	case 0x0061: // Get AI Reference Voltage
		responsePacketBuffer[5] = (linxDev.AiRef>>24) & 0xFF;		//AIREF MSB
		responsePacketBuffer[6] = (linxDev.AiRef>>16) & 0xFF;		//...
		responsePacketBuffer[7] = (linxDev.AiRef>>8) & 0xFF;			//...
		responsePacketBuffer[8] = linxDev.AiRef & 0xFF;					//AIREF LSB
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status); 
		break;
		
	/****************************************************************************************
	** UART
	****************************************************************************************/	
	case 0x00C0: // UART Open
	{
		//DEBUG linxDev.DebugPrint((char*)"UART Open Command");
		unsigned long targetBaud = (unsigned long)((commandPacketBuffer[7] << 24) | (commandPacketBuffer[8] << 16) | (commandPacketBuffer[9] << 8) | commandPacketBuffer[10]);
		unsigned long actualBaud = 0;
		
		status = linxDev.UartOpen(commandPacketBuffer[6], targetBaud, &actualBaud);
		//DEBUG linxDev.DebugPrint((char*)"UART Open Command Returned...\n");
		responsePacketBuffer[5] = (actualBaud>>24) & 0xFF;												//actualBaud MSB
		responsePacketBuffer[6] = (actualBaud>>16) & 0xFF;												//...
		responsePacketBuffer[7] = (actualBaud>>8) & 0xFF;												//...
		responsePacketBuffer[8] = actualBaud & 0xFF;															//actualBaud LSB
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status); 
		break;
	}
	case 0x00C1: // UART Set Buad Rate
	{
		unsigned long targetBaud = (unsigned long)((commandPacketBuffer[7] << 24) | (commandPacketBuffer[8] << 16) | (commandPacketBuffer[9] << 8) | commandPacketBuffer[10]);
		unsigned long actualBaud = 0;
		status = linxDev.UartSetBaudRate(commandPacketBuffer[6], targetBaud, &actualBaud);
		responsePacketBuffer[5] = (actualBaud>>24) & 0xFF;												//actualBaud MSB
		responsePacketBuffer[6] = (actualBaud>>16) & 0xFF;												//...
		responsePacketBuffer[7] = (actualBaud>>8) & 0xFF;												//...
		responsePacketBuffer[8] = actualBaud & 0xFF;															//actualBaud LSB
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status); 
		break;
	}
	case 0x00C2: // UART Get Bytes Available
	{
		unsigned char numBytes;
		status = linxDev.UartGetBytesAvailable(commandPacketBuffer[6], &numBytes);
		responsePacketBuffer[5] = numBytes;	
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 1, status); 		
		break;
	}
	case 0x00C3: // UART Read
	{
		unsigned char numBytesRead;
		status = linxDev.UartRead(commandPacketBuffer[6], commandPacketBuffer[7], &responsePacketBuffer[5], &numBytesRead);
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, numBytesRead, status); 		
		break;
	}
	case 0x00C4: // UART Write
	{
		status = linxDev.UartWrite(commandPacketBuffer[6], (commandPacketBuffer[1]-8), &commandPacketBuffer[7]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);	
		break;
	}
	case 0x00C5: // UART Close
	{
		status = linxDev.UartClose(commandPacketBuffer[6]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);	
		break;
	}
	
	/****************************************************************************************
	** I2C
	****************************************************************************************/	
	case 0x00E0: // I2C Open Master
		status = linxDev.I2cOpenMaster(commandPacketBuffer[6]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
		break;
	case 0x00E1: // I2C Set Speed
	{
		unsigned long targetSpeed = (unsigned long)((commandPacketBuffer[7] << 24) | (commandPacketBuffer[8] << 16) | (commandPacketBuffer[9] << 8) | commandPacketBuffer[10]);
		unsigned long actualSpeed = 0;
		status = linxDev.I2cSetSpeed(commandPacketBuffer[6], targetSpeed, &actualSpeed);
		
		//Build Response Packet
		responsePacketBuffer[5] = (actualSpeed>>24) & 0xFF;		//Actual Speed MSB
		responsePacketBuffer[6] = (actualSpeed>>16) & 0xFF;		//...
		responsePacketBuffer[7] = (actualSpeed>>8) & 0xFF;			//...
		responsePacketBuffer[8] = actualSpeed & 0xFF;					//Actual Speed LSB		
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status); 
		break;
	}
	case 0x00E2: // I2C Write
		status = linxDev.I2cWrite(commandPacketBuffer[6], commandPacketBuffer[7], commandPacketBuffer[8], (commandPacketBuffer[1]-10), &commandPacketBuffer[9]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
		break;
	case 0x00E3: // I2C Read
		status = linxDev.I2cRead(commandPacketBuffer[6], commandPacketBuffer[7], commandPacketBuffer[11], commandPacketBuffer[8],((commandPacketBuffer[9]<<8) | commandPacketBuffer[10]), &responsePacketBuffer[5]);
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, commandPacketBuffer[8], status); 		
		break;
	case 0x00E4: // I2C Close
		status = linxDev.I2cClose((commandPacketBuffer[6]));
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
		break;
		
	/****************************************************************************************
	** SPI
	****************************************************************************************/	
	case 0x0100: // SPI Open Master
		linxDev.SpiOpenMaster(commandPacketBuffer[6]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
	case 0x0101: // SPI Set Bit Order
		linxDev.SpiSetBitOrder(commandPacketBuffer[6], commandPacketBuffer[7]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
		
	case 0x0102: // SPI Set Clock Frequency				
	{
		unsigned long targetSpeed = (unsigned long)((commandPacketBuffer[7] << 24) | (commandPacketBuffer[8] << 16) | (commandPacketBuffer[9] << 8) | commandPacketBuffer[10]);
		unsigned long actualSpeed = 0;
		status = linxDev.SpiSetSpeed( commandPacketBuffer[6], targetSpeed, &actualSpeed );
		//Build Response Packet
		responsePacketBuffer[5] = (actualSpeed>>24) & 0xFF;		//Actual Speed MSB
		responsePacketBuffer[6] = (actualSpeed>>16) & 0xFF;		//...
		responsePacketBuffer[7] = (actualSpeed>>8) & 0xFF;			//...
		responsePacketBuffer[8] = actualSpeed & 0xFF;					//Actual Speed LSB		
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, L_OK); 
		break;		
	}	
	case 0x0103: // SPI Set Mode
	{
		//Set SPI Mode
		status = linxDev.SpiSetMode(commandPacketBuffer[6], commandPacketBuffer[7]);
		
		//Build Response Packet
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status); 
		break;	
	}
	case 0x0107: // SPI Write Read
	{
		status = linxDev.SpiWriteRead(commandPacketBuffer[6], commandPacketBuffer[7], (commandPacketBuffer[1]-11)/commandPacketBuffer[7], commandPacketBuffer[8], commandPacketBuffer[9], &commandPacketBuffer[10], &responsePacketBuffer[5]);
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, commandPacketBuffer[7]*commandPacketBuffer[8], status); 
		break;
	}
	
	default: //Default Case
		StatusResponse(commandPacketBuffer, responsePacketBuffer, (int)L_FUNCTION_NOT_SUPPORTED);
		break;		
	}
	
	//Print Response Packet If Debugging Is Enabled
	//DEBUGlinxDev.DebugPrintResPacket(responsePacketBuffer);	
	
	return status;
}


void LinxListener::PacketizeAndSend(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, unsigned int dataSize,  int status)
{
	//Load Header
	responsePacketBuffer[0] = 0xFF;                                 //SoF
	responsePacketBuffer[1] = dataSize+6; 						//PACKET SIZE
	responsePacketBuffer[2] = commandPacketBuffer[2];	//PACKET NUM (MSB)
	responsePacketBuffer[3] = commandPacketBuffer[3];	//PACKET NUM (LSB)
	//Make Sure Status Is Valid
	if(status >= 0 && status <= 255)
	{
		responsePacketBuffer[4] = (unsigned char)status;	//Status
	}
	else
	{
		responsePacketBuffer[4] = (unsigned char)L_UNKNOWN_ERROR;	//Status
	}
	
	//Compute And Load Checksum
	responsePacketBuffer[dataSize+5] = ComputeChecksum(responsePacketBuffer);	
}

void LinxListener::DataBufferResponse(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer, const unsigned char* dataBuffer, unsigned char dataSize, int status)
{
	
	//Copy Data Into Response Buffer
	for(int i=0; i<dataSize; i++)
	{
		responsePacketBuffer[i+5] = dataBuffer[i];
	}
	
	PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, dataSize, status);
}