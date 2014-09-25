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

int LinxListener::CheckForCommands()
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


int LinxListener::ProcessCommand(unsigned char* commandPacketBuffer, unsigned char* responsePacketBuffer)
{
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
		
	//case 0x0001: //TODO Flush Linx Connection Buffer
	 
	case 0x0003: // Get Device ID     
		responsePacketBuffer[5] = LinxDev->DeviceFamily;
		responsePacketBuffer[6] = LinxDev->DeviceID;    
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 2, L_OK); 
		break;	
		
	case 0x0004: //Get LINX API Version
		responsePacketBuffer[5] = LinxDev->LinxApiMajor;
		responsePacketBuffer[6] = LinxDev->LinxApiMinor;
		responsePacketBuffer[7] = LinxDev->LinxApiSubminor;   
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 3, L_OK); 
		break;
		
	case 0x0005: //Get UART Max Baud
		responsePacketBuffer[5] = (LinxDev->UartMaxBaud>>24) & 0xFF;
		responsePacketBuffer[6] = (LinxDev->UartMaxBaud>>16) & 0xFF;
		responsePacketBuffer[7] = (LinxDev->UartMaxBaud>>8) & 0xFF;
		responsePacketBuffer[8] = LinxDev->UartMaxBaud & 0xFF;
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, L_OK); 
		break;
	
	case 0x0006: //Set UART Listener Interface Max Baud
	{
		unsigned long targetBaud = (unsigned long)((commandPacketBuffer[6] << 24) | (commandPacketBuffer[7] << 16) | (commandPacketBuffer[8] << 8) | commandPacketBuffer[9]);
		unsigned long actualBaud = 0;
		status = LinxDev->UartSetBaudRate(ListenerChan, targetBaud, &actualBaud);
		delay(1000);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
		break;
	}		
		
	//case 0x0007: //TODO Get Max Packet Size
	
	case 0x0008: // Get DIO Channels
		DataBufferResponse(commandPacketBuffer, responsePacketBuffer, LinxDev->DigitalChans, LinxDev->NumDigitalChans, L_OK);
		break;
	
	case 0x0009: // Get AI Channels
        DataBufferResponse(commandPacketBuffer, responsePacketBuffer, LinxDev->AiChans, LinxDev->NumAiChans, L_OK);
       break;
	   
    case 0x000A: // Get AO Channels
        DataBufferResponse(commandPacketBuffer, responsePacketBuffer, LinxDev->AoChans, LinxDev->NumAoChans, L_OK);
       break;
	   
    case 0x000B: // Get PWM Channels
        DataBufferResponse(commandPacketBuffer, responsePacketBuffer, LinxDev->PwmChans, LinxDev->NumPwmChans, L_OK);
       break;
	   
    case 0x000C: // Get QE Channels
        DataBufferResponse(commandPacketBuffer, responsePacketBuffer, LinxDev->QeChans, LinxDev->NumQeChans, L_OK);
       break;
	   
    case 0x000D: // Get UART Channels
       DataBufferResponse(commandPacketBuffer, responsePacketBuffer, LinxDev->UartChans, LinxDev->NumUartChans, L_OK);
       break;
	   
    case 0x000E: // Get I2C Channels
       DataBufferResponse(commandPacketBuffer, responsePacketBuffer, LinxDev->I2cChans, LinxDev->NumI2cChans, L_OK);
       break;
	   
    case 0x000F: // Get SPI Channels
       DataBufferResponse(commandPacketBuffer, responsePacketBuffer, LinxDev->SpiChans, LinxDev->NumSpiChans, L_OK);
       break;
	   
    case 0x0010: // Get CAN Channels
        DataBufferResponse(commandPacketBuffer, responsePacketBuffer, LinxDev->CanChans, LinxDev->NumCanChans, L_OK);
       break;
		
	case 0x0011: // Disconnect
		status = L_DISCONNECT;
		break;
		
	//case 0x0012: //TODO Set Device User Id	
	//case 0x0013: //TODO Get Device User Id
	
	//case 0x0014: //TODO Set Device Ethernet IP
	//case 0x0015: //TODO Get Device Ethernet IP	
	//case 0x0016: //TODO Set Device Ethernet Port
	//case 0x0017: //TODO Get Device Ethernet Port
	
	//case 0x0018: //TODO Set Device WIFI IP
	//case 0x0019: //TODO Get Device WIFI IP	
	//case 0x001A: //TODO Set Device WIFI Port
	//case 0x001B: //TODO Get Device WIFI Port
	//case 0x001C: //TODO Set Device WIFI SSID
	//case 0x001D: //TODO Get Device WIFI SSID
	//case 0x001E: //TODO Set Device WIFI Security Type
	//case 0x001F: //TODO Get Device WIFI Security Type
	//case 0x0020: //TODO Set Device WIFI Password
	//case 0x0021: //TODO Get Device WIFI Password
	
	//case 0x0022: //TODO Set Device Max Baud
	//case 0x0023: //TODO Get Device Max Baud
	
	case 0x0024: // Get Device Name
		DataBufferResponse(commandPacketBuffer, responsePacketBuffer, LinxDev->DeviceName, LinxDev->DeviceNameLen, L_OK);
		break;
	
	/****************************************************************************************
	**  Digital I/O
	****************************************************************************************/	
	//case 0x0040: //TODO Set Pin Mode
	
	case 0x0041: // Digital Write
		LinxDev->DigitalWrite(commandPacketBuffer[6], &commandPacketBuffer[7], &commandPacketBuffer[7+commandPacketBuffer[6]]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
		break;
		
	case 0x0042: // Digital Read
		LinxDev->DigitalRead((commandPacketBuffer[1]-7), &commandPacketBuffer[6], &responsePacketBuffer[5]);
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, (commandPacketBuffer[1]-7), status); 
		break;
		
	//case 0x0043: //TODO Write Square Wave
	//case 0x0044: //TODO Read Pulse Width
		
	/****************************************************************************************
	**  Analog I/O
	****************************************************************************************/	
	//case 0x0060: //TODO Set AI Ref Voltage
	
	case 0x0061: // Get AI Reference Voltage
		responsePacketBuffer[5] = (LinxDev->AiRef>>24) & 0xFF;		//AIREF MSB
		responsePacketBuffer[6] = (LinxDev->AiRef>>16) & 0xFF;		//...
		responsePacketBuffer[7] = (LinxDev->AiRef>>8) & 0xFF;			//...
		responsePacketBuffer[8] = LinxDev->AiRef & 0xFF;					//AIREF LSB
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status); 
		break;
		
	//case 0x0062: //TODO Set AI Resolution
	//case 0x0063: //TODO Get AI Resolution
	
	case 0x0064: // Analog Read
	{
		responsePacketBuffer[5] = LinxDev->AiResolution;
		LinxDev->AnalogRead((commandPacketBuffer[1]-7), &commandPacketBuffer[6], &responsePacketBuffer[6]);	
		unsigned int numDataBits = ((commandPacketBuffer[1]-7) * LinxDev->AiResolution);
		unsigned char numResponseDataBytes = numDataBits / 8;
				
		LinxDev->DebugPrint("numDataBits = ");
		LinxDev->DebugPrintln(numDataBits, DEC);
		
		LinxDev->DebugPrint("numResponseDataBytes = ");		
		LinxDev->DebugPrintln(numResponseDataBytes, DEC);
		
		if( (numDataBits % 8) != 0)
		{
			//Partial Byte Included, Increment Total
			numResponseDataBytes++;
			LinxDev->DebugPrintln("Partial Byte");		
		}
		
		LinxDev->DebugPrint("numResponseDataBytes = ");		
		LinxDev->DebugPrintln(numResponseDataBytes, DEC);
		
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, numResponseDataBytes+1, status); 
		break;
	}
	
	//case 0x0065: //TODO Analog Write
		
	/****************************************************************************************
	** UART
	****************************************************************************************/	
	case 0x00C0: // UART Open
	{
		unsigned long targetBaud = (unsigned long)((commandPacketBuffer[7] << 24) | (commandPacketBuffer[8] << 16) | (commandPacketBuffer[9] << 8) | commandPacketBuffer[10]);
		unsigned long actualBaud = 0;
		
		status = LinxDev->UartOpen(commandPacketBuffer[6], targetBaud, &actualBaud);
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
		status = LinxDev->UartSetBaudRate(commandPacketBuffer[6], targetBaud, &actualBaud);
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
		status = LinxDev->UartGetBytesAvailable(commandPacketBuffer[6], &numBytes);
		responsePacketBuffer[5] = numBytes;	
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 1, status); 		
		break;
	}
	case 0x00C3: // UART Read
	{
		unsigned char numBytesRead = 0;
		status = LinxDev->UartRead(commandPacketBuffer[6], commandPacketBuffer[7], &responsePacketBuffer[5], &numBytesRead);
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, numBytesRead, status); 		
		break;
	}
	case 0x00C4: // UART Write
	{
		status = LinxDev->UartWrite(commandPacketBuffer[6], (commandPacketBuffer[1]-8), &commandPacketBuffer[7]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);	
		break;
	}
	case 0x00C5: // UART Close
	{
		status = LinxDev->UartClose(commandPacketBuffer[6]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);	
		break;
	}
	
	/****************************************************************************************
	** I2C
	****************************************************************************************/	
	case 0x00E0: // I2C Open Master
		status = LinxDev->I2cOpenMaster(commandPacketBuffer[6]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
		break;
	case 0x00E1: // I2C Set Speed
	{
		unsigned long targetSpeed = (unsigned long)((commandPacketBuffer[7] << 24) | (commandPacketBuffer[8] << 16) | (commandPacketBuffer[9] << 8) | commandPacketBuffer[10]);
		unsigned long actualSpeed = 0;
		status = LinxDev->I2cSetSpeed(commandPacketBuffer[6], targetSpeed, &actualSpeed);
		
		//Build Response Packet
		responsePacketBuffer[5] = (actualSpeed>>24) & 0xFF;		//Actual Speed MSB
		responsePacketBuffer[6] = (actualSpeed>>16) & 0xFF;		//...
		responsePacketBuffer[7] = (actualSpeed>>8) & 0xFF;			//...
		responsePacketBuffer[8] = actualSpeed & 0xFF;					//Actual Speed LSB		
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status); 
		break;
	}
	case 0x00E2: // I2C Write
		status = LinxDev->I2cWrite(commandPacketBuffer[6], commandPacketBuffer[7], commandPacketBuffer[8], (commandPacketBuffer[1]-10), &commandPacketBuffer[9]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
		break;
	case 0x00E3: // I2C Read
		status = LinxDev->I2cRead(commandPacketBuffer[6], commandPacketBuffer[7], commandPacketBuffer[11], commandPacketBuffer[8],((commandPacketBuffer[9]<<8) | commandPacketBuffer[10]), &responsePacketBuffer[5]);
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, commandPacketBuffer[8], status); 		
		break;
	case 0x00E4: // I2C Close
		status = LinxDev->I2cClose((commandPacketBuffer[6]));
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
		break;
		
	/****************************************************************************************
	** SPI
	****************************************************************************************/	
	case 0x0100: // SPI Open Master
		LinxDev->SpiOpenMaster(commandPacketBuffer[6]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
	case 0x0101: // SPI Set Bit Order
		LinxDev->SpiSetBitOrder(commandPacketBuffer[6], commandPacketBuffer[7]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
		
	case 0x0102: // SPI Set Clock Frequency				
	{
		unsigned long targetSpeed = (unsigned long)((commandPacketBuffer[7] << 24) | (commandPacketBuffer[8] << 16) | (commandPacketBuffer[9] << 8) | commandPacketBuffer[10]);
		unsigned long actualSpeed = 0;
		status = LinxDev->SpiSetSpeed( commandPacketBuffer[6], targetSpeed, &actualSpeed );
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
		status = LinxDev->SpiSetMode(commandPacketBuffer[6], commandPacketBuffer[7]);
		
		//Build Response Packet
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, status); 
		break;	
	}
	case 0x0107: // SPI Write Read
	{
		status = LinxDev->SpiWriteRead(commandPacketBuffer[6], commandPacketBuffer[7], (commandPacketBuffer[1]-11)/commandPacketBuffer[7], commandPacketBuffer[8], commandPacketBuffer[9], &commandPacketBuffer[10], &responsePacketBuffer[5]);
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, commandPacketBuffer[7]*commandPacketBuffer[8], status); 
		break;
	}
	
	default: //Default Case
		StatusResponse(commandPacketBuffer, responsePacketBuffer, (int)L_FUNCTION_NOT_SUPPORTED);
		break;		
	}
	
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