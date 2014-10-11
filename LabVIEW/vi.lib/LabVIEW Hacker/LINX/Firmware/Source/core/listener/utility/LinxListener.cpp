/****************************************************************************************
** Includes
****************************************************************************************/
#include <stdio.h>

#include "LinxListener.h"
#include "LinxDevice.h"

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
	//case 0x0002: //TODO Reset
	 
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
		LinxDev->DelayMs(1000);
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
		
	case 0x0012: //Set Device User Id	
		LinxDev->userId = commandPacketBuffer[6] << 8 | commandPacketBuffer[7];		
		LinxDev->NonVolatileWrite(NVS_USERID, commandPacketBuffer[6]);
		LinxDev->NonVolatileWrite(NVS_USERID+1, commandPacketBuffer[7]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
		
	case 0x0013: //Get Device User Id
		responsePacketBuffer[5] = (LinxDev->userId >> 8) & 0xFF;
		responsePacketBuffer[6] = LinxDev->userId & 0xFF;
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 2, L_OK);
		break;
	
	
	case 0x0014: //Set Device Ethernet IP
		LinxDev->ethernetIp = (commandPacketBuffer[6]<<24) | (commandPacketBuffer[7]<<16) | (commandPacketBuffer[8]<<8) | (commandPacketBuffer[9]);
		LinxDev->NonVolatileWrite(NVS_ETHERNET_IP, commandPacketBuffer[6]);
		LinxDev->NonVolatileWrite(NVS_ETHERNET_IP+1, commandPacketBuffer[7]);
		LinxDev->NonVolatileWrite(NVS_ETHERNET_IP+2, commandPacketBuffer[8]);
		LinxDev->NonVolatileWrite(NVS_ETHERNET_IP+3, commandPacketBuffer[9]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
		
	case 0x0015: //Get Device Ethernet IP	
		responsePacketBuffer[5] = ((LinxDev->ethernetIp>>24) & 0xFF);	//Ethernet IP MSB  
		responsePacketBuffer[6] = ((LinxDev->ethernetIp>>16) & 0xFF);	//Ethernet IP ...
		responsePacketBuffer[7] = ((LinxDev->ethernetIp>>8) & 0xFF);	//Ethernet IP ...
		responsePacketBuffer[8] = ((LinxDev->ethernetIp) & 0xFF);		//Ethernet IP LSB  
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, L_OK);
		break;
		
	case 0x0016: //Set Device Ethernet Port
		LinxDev->ethernetPort = ((commandPacketBuffer[6]<<8) | (commandPacketBuffer[7]));
		LinxDev->NonVolatileWrite(NVS_ETHERNET_PORT, commandPacketBuffer[6]);
		LinxDev->NonVolatileWrite(NVS_ETHERNET_PORT+1, commandPacketBuffer[7]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
		
	case 0x0017: //Get Device Ethernet Port
		responsePacketBuffer[5] = ((LinxDev->ethernetPort>>8) & 0xFF);	//Ethernet PORT MSB
		responsePacketBuffer[6] = (LinxDev->ethernetPort & 0xFF);		//Ethernet PORT LSB
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 2, L_OK);
		break;
	
	case 0x0018: //Set Device WIFI IP
		LinxDev->wifiIp = (commandPacketBuffer[6]<<24) | (commandPacketBuffer[7]<<16) | (commandPacketBuffer[8]<<8) | (commandPacketBuffer[9]);
		LinxDev->NonVolatileWrite(NVS_WIFI_IP, commandPacketBuffer[6]);
		LinxDev->NonVolatileWrite(NVS_WIFI_IP+1, commandPacketBuffer[7]);
		LinxDev->NonVolatileWrite(NVS_WIFI_IP+2, commandPacketBuffer[8]);
		LinxDev->NonVolatileWrite(NVS_WIFI_IP+3, commandPacketBuffer[9]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
		
	case 0x0019: //Get Device WIFI IP	
		responsePacketBuffer[5] = ((LinxDev->wifiIp>>24) & 0xFF);                   //WIFI IP MSB
		responsePacketBuffer[6] = ((LinxDev->wifiIp>>16) & 0xFF);                   //WIFI IP ...
		responsePacketBuffer[7] = ((LinxDev->wifiIp>>8) & 0xFF);                    //WIFI IP ...
		responsePacketBuffer[8] = ((LinxDev->wifiIp) & 0xFF);                       //WIFI IP LSB  
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, L_OK);
		break;
		
	case 0x001A: //Set Device WIFI Port
		LinxDev->wifiPort = ((commandPacketBuffer[6]<<8) | (commandPacketBuffer[7]));
		LinxDev->NonVolatileWrite(NVS_WIFI_PORT, commandPacketBuffer[6]);
		LinxDev->NonVolatileWrite(NVS_WIFI_PORT+1, commandPacketBuffer[7]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
		
	case 0x001B: //Get Device WIFI Port
		responsePacketBuffer[5] = ((LinxDev->wifiPort>>8) & 0xFF);                  //WIFI PORT MSB
		responsePacketBuffer[6] = (LinxDev->wifiPort & 0xFF);                       //WIFI PORT LSB
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 2, L_OK);
		break;
		
	case 0x001C: //Set Device WIFI SSID
		 //Update Ssid Size In RAM And NVS
		if(commandPacketBuffer[6] > 32)
		{
			LinxDev->wifiSsidSize = 32;
			LinxDev->NonVolatileWrite(NVS_WIFI_SSID_SIZE, 32);
		}
		else
		{
			LinxDev->wifiSsidSize = commandPacketBuffer[6];
			LinxDev->NonVolatileWrite(NVS_WIFI_SSID_SIZE, commandPacketBuffer[6]);
		}

		//Update SSID Value In RAM And NVS
		for(int i=0; i<LinxDev->wifiSsidSize; i++)
		{
			LinxDev->wifiSsid[i] = commandPacketBuffer[7+i];
			LinxDev->NonVolatileWrite(NVS_WIFI_SSID_SIZE+i, commandPacketBuffer[7+i]);    
		}
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
		
	case 0x001D: //Get Device WIFI SSID
		responsePacketBuffer[5] = LinxDev->wifiSsidSize;	//SSID SIZE

		for(int i=0; i<LinxDev->wifiSsidSize; i++)
		{
			responsePacketBuffer[i+6] = LinxDev->wifiSsid[i];
		}
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, LinxDev->wifiSsidSize, L_OK);
		break;
		
	case 0x001E: //Set Device WIFI Security Type
		LinxDev->wifiSecurity = commandPacketBuffer[6];
		LinxDev->NonVolatileWrite(NVS_WIFI_SECURITY_TYPE, commandPacketBuffer[6]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
	
	case 0x001F: //Get Device WIFI Security Type
		responsePacketBuffer[5] = LinxDev->wifiSecurity;
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 1, L_OK);
		break;
		
	case 0x0020: //Set Device WIFI Password
		//Update PW Size In RAM And NVS
		if(commandPacketBuffer[6] > 64)
		{
			LinxDev->wifiPwSize = 64;
			LinxDev->NonVolatileWrite(NVS_WIFI_PW_SIZE, 64);
		}
		else
		{
			LinxDev->wifiPwSize = commandPacketBuffer[6];
			LinxDev->NonVolatileWrite(NVS_WIFI_PW_SIZE, commandPacketBuffer[6]);
		}  

		//Update PW Value In RAM And NVS
		for(int i=0; i<LinxDev->wifiPwSize; i++)
		{
			LinxDev->wifiPw[i] = commandPacketBuffer[7+i];
			LinxDev->NonVolatileWrite(NVS_WIFI_PW+i, commandPacketBuffer[i+7]);    
		}
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
		
	//case 0x0021: //TODO Get Device WIFI Password - Intentionally Not Implemented For Security Reasons.
		
	case 0x0022: //Set Device Max Baud
		LinxDev->serialInterfaceMaxBaud = (unsigned long)(((unsigned long)commandPacketBuffer[6]<<24) | ((unsigned long)commandPacketBuffer[7]<<16) | ((unsigned long)commandPacketBuffer[8]<<8) | ((unsigned long)commandPacketBuffer[9]));
		LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD, commandPacketBuffer[6]);
		LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD+1, commandPacketBuffer[7]);
		LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD+2, commandPacketBuffer[8]);
		LinxDev->NonVolatileWrite(NVS_SERIAL_INTERFACE_MAX_BAUD+3, commandPacketBuffer[9]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, L_OK);
		break;
	case 0x0023: //Get Device Max Baud
	
		responsePacketBuffer[5] = ((LinxDev->serialInterfaceMaxBaud>>24) & 0xFF);   //WIFI IP MSB
		responsePacketBuffer[6] = ((LinxDev->serialInterfaceMaxBaud>>16) & 0xFF);   //WIFI IP ...
		responsePacketBuffer[7] = ((LinxDev->serialInterfaceMaxBaud>>8) & 0xFF);    //WIFI IP ...
		responsePacketBuffer[8] = ((LinxDev->serialInterfaceMaxBaud) & 0xFF);       //WIFI IP LSB   
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, 4, L_OK);
		break;
	
	case 0x0024: // Get Device Name
		DataBufferResponse(commandPacketBuffer, responsePacketBuffer, LinxDev->DeviceName, LinxDev->DeviceNameLen, L_OK);
		break;
	
	//---0x0025 to 0x003F Reserved---
	
	/****************************************************************************************
	**  Digital I/O
	****************************************************************************************/	
	//case 0x0040: //TODO Set Pin Mode
	
	case 0x0041: // Digital Write
		status = LinxDev->DigitalWrite(commandPacketBuffer[6], &commandPacketBuffer[7], &commandPacketBuffer[7+commandPacketBuffer[6]]);
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
		break;
		
	case 0x0042: // Digital Read
		status = LinxDev->DigitalRead((commandPacketBuffer[1]-7), &commandPacketBuffer[6], &responsePacketBuffer[5]);
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, (commandPacketBuffer[1]-7), status); 
		break;
		
	//case 0x0043: //TODO Write Square Wave
	//case 0x0044: //TODO Read Pulse Width
		
	//---0x0045 to 0x005F Reserved---
	
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
		status = LinxDev->AnalogRead((commandPacketBuffer[1]-7), &commandPacketBuffer[6], &responsePacketBuffer[6]);	
		unsigned int numDataBits = ((commandPacketBuffer[1]-7) * LinxDev->AiResolution);
		unsigned char numResponseDataBytes = numDataBits / 8;
				
		if( (numDataBits % 8) != 0)
		{
			//Partial Byte Included, Increment Total
			numResponseDataBytes++;
		}
		
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, numResponseDataBytes+1, status); 
		break;
	}
	
	//case 0x0065: //TODO Analog Write
	
	//---0x0066 to 0x007F Reserved---
	
	/****************************************************************************************
	** PWM
	****************************************************************************************/	
	
	//case 0x0080: //TODO PWM Open
	//case 0x0081: //TODO PWM Set Mode
	//case 0x0082: //TODO PWM Set Frequency
	
	case 0x0083: //PWM Set Duty Cycle	
		status = LinxDev->PwmSetDutyCycle(commandPacketBuffer[6], &commandPacketBuffer[7], &commandPacketBuffer[commandPacketBuffer[6] + 7] );
		StatusResponse(commandPacketBuffer, responsePacketBuffer, status);
		break;
	
	//case 0x0084: //TODO PWM Close	
	
	/****************************************************************************************
	** QE
	****************************************************************************************/		
	
	//---0x00A0 to 0x00BF Reserved---
	
	
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
	
	//---0x00C6 to 0x00DF Reserved---
	
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
		
	//---0x00E5 to 0x00FF Reserved---
		
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
	
	//case 0x0104: //LEGACY - SPI Set Frame Size
	//case 0x0105: //LEGACY - SPI Set CS Logic Level
	//case 0x0106: //LEGACY - SPI Set CS Channel
	
	case 0x0107: // SPI Write Read
	{
		status = LinxDev->SpiWriteRead(commandPacketBuffer[6], commandPacketBuffer[7], (commandPacketBuffer[1]-11)/commandPacketBuffer[7], commandPacketBuffer[8], commandPacketBuffer[9], &commandPacketBuffer[10], &responsePacketBuffer[5]);
		PacketizeAndSend(commandPacketBuffer, responsePacketBuffer, commandPacketBuffer[7]*commandPacketBuffer[8], status); 
		break;
	}
	
	//---0x0085 to 0x009F Reserved---
	
	/****************************************************************************************
	** CAN
	****************************************************************************************/	
	
	//---0x0120 to 0x013F Reserved---
	
	/****************************************************************************************
	** User Commands
	****************************************************************************************/	
	
	//---0xFC00 to 0xFFFF---
	
	/****************************************************************************************
	** Default
	****************************************************************************************/	
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