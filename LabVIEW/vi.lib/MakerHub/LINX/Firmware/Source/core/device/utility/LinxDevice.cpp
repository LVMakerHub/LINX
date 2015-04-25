/****************************************************************************************
**  LINX - Generic LINX device.
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
**  Public Functions
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
	DebugPrintln("Debugging Enabled");
}

void LinxDevice::DelayMs(unsigned long ms)
{

}

void LinxDevice::DebugPrintPacket(unsigned char direction, const unsigned char* packetBuffer)
{
	#if DEBUG_ENABLED > 0				
		if(direction == RX)
		{		
			DebugPrint("Received :: ");
		}
		else if(direction == TX)
		{
			DebugPrint("Sending  :: ");
		}
		
		for(int i=0; i<packetBuffer[1]; i++)
		{		
			DebugPrint("[");	
			DebugPrint(packetBuffer[i], HEX);
			DebugPrint("]");	
		}		
		DebugPrintln();		
		
		if(direction == TX)
		{
			//Add Second New Line After TX
			DebugPrintln();		
		}
	#endif
}


// ---------------- UART Functions ------------------ 

void LinxDevice::UartWrite(unsigned char channel, unsigned char b)
{
	UartWrite(channel, 1, (unsigned char*)&b);
}

void LinxDevice::UartWrite(unsigned char channel, const char *s)
{
	while (*s)
	{
		UartWrite(channel, *s++);
	}
}

void LinxDevice::UartWrite(unsigned char channel, char c)
{
	UartWrite(channel, (unsigned char) c);
}

void LinxDevice::UartWrite(unsigned char channel, int n)
{
	UartWrite(channel, (long) n);
}

void LinxDevice::UartWrite(unsigned char channel, unsigned int n)
{
	UartWrite(channel, (unsigned long) n);
}

void LinxDevice::UartWrite(unsigned char channel, long n)
{
	if (n < 0) 
	{
		UartWrite(channel, '-');
		n = -n;
	}
	UartWriteNumber(channel, n, 10);
}

void LinxDevice::UartWrite(unsigned char channel, unsigned long n)
{
	UartWriteNumber(channel , n, 10);
}

void LinxDevice::UartWrite(unsigned char channel, long n, int base)
{
	if (base == 0)
	{
		UartWrite(channel, (char) n);
	}
	else if (base == 10)
	{
		UartWrite(channel, n);
	}
	else
	{
		UartWriteNumber(channel , n, base);
	}
}

void LinxDevice::UartWriteln(unsigned char channel)
{
	UartWrite(channel, '\r');
	UartWrite(channel, '\n');  
}

void LinxDevice::UartWriteln(unsigned char channel, char c)
{
	UartWrite(channel, c);
	UartWriteln(channel);  
}

void LinxDevice::UartWriteln(unsigned char channel, const char c[])
{
	UartWrite(channel, c);
	UartWriteln(channel);
}

void LinxDevice::UartWriteln(unsigned char channel, unsigned char b)
{
	UartWrite(channel, b);
	UartWriteln(channel);
}

void LinxDevice::UartWriteln(unsigned char channel, int n)
{
	UartWrite(channel, n);
	UartWriteln(channel);
}

void LinxDevice::UartWriteln(unsigned char channel, long n)
{
	UartWrite(channel, n);
	UartWriteln(channel);  
}

void LinxDevice::UartWriteln(unsigned char channel, unsigned long n)
{
	UartWrite(channel, n);
	UartWriteln(channel);  
}

void LinxDevice::UartWriteln(unsigned char channel, long n, int base)
{
	UartWrite(channel, n, base);
	UartWriteln(channel);
}

//----------------- WS2812 Functions -----------------------------
int LinxDevice::WS2812Open(unsigned short numLeds, unsigned char dataChan)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxDevice::WS2812WriteOnePixel(unsigned short pixelIndex, unsigned char red, unsigned char green, unsigned char blue, unsigned char refresh)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxDevice::WS2812WriteNPixels(unsigned short startPixel, unsigned short endPixel, unsigned char* data, unsigned char refresh)
{
	return L_FUNCTION_NOT_SUPPORTED;
}

int LinxDevice::WS2812Refresh()
{
	return L_FUNCTION_NOT_SUPPORTED;
}



//----------------- DEBUG Functions -----------------------------
void  LinxDevice::DebugPrint(char c)
{
	#if DEBUG_ENABLED > 0	
		UartWrite(DEBUG_ENABLED, c);
	#endif
}

void  LinxDevice::DebugPrint(const char s[])
{
	#if DEBUG_ENABLED > 0	
		UartWrite(DEBUG_ENABLED, s);
	#endif
}

void  LinxDevice::DebugPrint(unsigned char c)
{
	#if DEBUG_ENABLED > 0	
		UartWrite(DEBUG_ENABLED, c);
	#endif
}

void  LinxDevice::DebugPrint(int n)
{
	#if DEBUG_ENABLED > 0	
		UartWrite(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrint(unsigned int n)
{
	#if DEBUG_ENABLED > 0	
		UartWrite(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrint(long n)
{
	#if DEBUG_ENABLED > 0	
		UartWrite(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrint(unsigned long n)
{
	#if DEBUG_ENABLED > 0	
		UartWrite(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrint(long n, int base)
{
	#if DEBUG_ENABLED > 0	
		UartWrite(DEBUG_ENABLED, n, base);
	#endif
}

void  LinxDevice::DebugPrintln()
{
	#if DEBUG_ENABLED > 0	
		UartWriteln(DEBUG_ENABLED);
	#endif
}

void  LinxDevice::DebugPrintln(char c)
{
	#if DEBUG_ENABLED > 0	
		UartWriteln(DEBUG_ENABLED, c);
	#endif
}

void  LinxDevice::DebugPrintln(const char s[])
{
	#if DEBUG_ENABLED > 0	
		UartWriteln(DEBUG_ENABLED, s);
	#endif
}

void  LinxDevice::DebugPrintln(unsigned char c)
{
	#if DEBUG_ENABLED > 0	
		UartWriteln(DEBUG_ENABLED, c);
	#endif
}

void  LinxDevice::DebugPrintln(int n)
{	
	#if DEBUG_ENABLED > 0	
		UartWriteln(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrintln(long n)
{
	#if DEBUG_ENABLED > 0	
		UartWriteln(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrintln(unsigned long n)
{
	#if DEBUG_ENABLED > 0	
		UartWriteln(DEBUG_ENABLED, n);
	#endif
}

void  LinxDevice::DebugPrintln(long n, int base)
{
	#if DEBUG_ENABLED > 0	
		UartWriteln(DEBUG_ENABLED, n, base);
	#endif
}


/****************************************************************************************
**  Private Functions
****************************************************************************************/

void LinxDevice::UartWriteNumber(unsigned char channel, unsigned long n, unsigned char base)
{
	unsigned char buf[8 * sizeof(long)]; // Assumes 8-bit chars. 
	unsigned long i = 0;

	if (n == 0) 
	{
		UartWrite(channel, '0');
		return;
	} 

	while (n > 0) 
	{
		buf[i++] = n % base;
		n /= base;
	}

	for (; i > 0; i--)
	{
		UartWrite(channel, (char) (buf[i - 1] < 10 ? '0' + buf[i - 1] : 'A' + buf[i - 1] - 10));
	}
}
