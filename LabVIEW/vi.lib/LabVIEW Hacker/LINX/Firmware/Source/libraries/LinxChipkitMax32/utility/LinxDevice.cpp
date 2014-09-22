/****************************************************************************************
**  Includes
****************************************************************************************/
#include <stdio.h>
#include "LinxDevice.h"


/****************************************************************************************
**  Constructors
****************************************************************************************/
LinxDevice::LinxDevice()
{
	DeviceFamily = 0xFE;
	DeviceID = 0x00;
}

/****************************************************************************************
**  Functions
****************************************************************************************/

//Reverse The Order Of Bits In A Byte.  This Is Useful For SPI Hardware That Does Not Support Bit Order
unsigned char LinxDevice::ReverseBits(unsigned char b) 
{
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

//-------------------Default Empty Functions--------------------------------
void LinxDevice::EnableDebug(unsigned char channel)
{
	unsigned long actualBaud = 0;
	
	UartOpen(channel, 115200, &actualBaud);
	unsigned char debugString[19] = "Debugging Enabled\n";
	UartWrite(channel, 18, debugString);
}

void LinxDevice::DelayMs(unsigned long ms)
{

}

//Debug
void LinxDevice::DebugPrint(unsigned char numBytes, const char* message)
{
	#if DEBUG_ENABLED > 0
		UartWrite(DEBUG_ENABLED, numBytes, (unsigned char*) message);
		
		unsigned char newLine[3] = "\n\r";
		UartWrite(DEBUG_ENABLED, 2, newLine);
	#endif 
}

void LinxDevice::DebugPrint(const char *s)
{

}

void LinxDevice::DebugPrintPacket(unsigned char direction, const unsigned char* packetBuffer)
{
	#if DEBUG_ENABLED > 0
				
		if(direction == RX)
		{
			unsigned char recPrint[13] = "Received :: ";
			UartWrite(DEBUG_ENABLED, 12, recPrint);
		}
		else if(direction == TX)
		{
			unsigned char sendPrint[13] = "Sending  :: ";
			UartWrite(DEBUG_ENABLED, 12, sendPrint);			
		}
		
		for(int i=0; i<packetBuffer[1]; i++)
		{		
			unsigned long n = packetBuffer[i];
			char base = 16;
			unsigned char openBracket[2] = "[";
			UartWrite(DEBUG_ENABLED, 1, openBracket);			
			
			
			unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
			unsigned long i = 0;

			//Convert To HEX and Print
			if (n == 0) 
			{
				unsigned char zero[2] = "0";
				UartWrite(DEBUG_ENABLED, 1, zero);
			}
			else
			{

				while (n > 0) 
				{
					buf[i++] = n % base;
					n /= base;
				}

				unsigned char digit[2] = "0";
				for (; i > 0; i--)
				{
					digit[0] = (char) (buf[i - 1] < 10 ? '0' + buf[i - 1] : 'A' + buf[i - 1] - 10);
					UartWrite(DEBUG_ENABLED, 1, digit);
				}
			}
			
			unsigned char closeBracket[2] = "]";
			UartWrite(DEBUG_ENABLED, 1, closeBracket);
			
		}
		
	
		unsigned char newLine[3] = "\n\r";
		UartWrite(DEBUG_ENABLED, 2, newLine);
		if(direction == TX)
		{
			UartWrite(DEBUG_ENABLED, 2, newLine);
		}
	#endif
}
