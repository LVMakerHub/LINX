/****************************************************************************************
**  LINX - chipKIT Family
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
#include "LinxChipkit.h"


/****************************************************************************************
**  Constructors /  Destructor
****************************************************************************************/
LinxChipkit::LinxChipkit()
{
	DeviceFamily = 0x01;	//Digilent Family Code
}

//----------------- WS2812 Functions -----------------------------
	
#ifdef _BOARD_WF32_ | _BOARD_MEGA_
	
	#include <WS2812.h>
	
	WS2812 m_ws2812;
	WS2812::GRB* m_WS2812Buffer;
	uint8_t* m_rgbPatternBuffer;
	unsigned short m_numPixels = 0;
#endif // _BOARD_WF32_ | _BOARD_MEGA_


int LinxChipkit::WS2812Open(unsigned short numLeds, unsigned char dataChan)
{
	#ifdef _BOARD_WF32_ | _BOARD_MEGA_
		m_numPixels = numLeds;
		m_WS2812Buffer = (WS2812::GRB*)malloc(numLeds * sizeof(WS2812::GRB));
		m_rgbPatternBuffer = (uint8_t*)malloc(CBWS2812PATBUF(numLeds));

		
		
		//Check If Memory Was Allocated Successfully
		if(m_WS2812Buffer == 0 || m_rgbPatternBuffer == 0)
		{
			return 0x81;
		}
		else
		{
			m_ws2812.begin(numLeds, m_rgbPatternBuffer, CBWS2812PATBUF(numLeds), false);
		}
		
		return L_OK;
	#else
		return L_FUNCTION_NOT_SUPPORTED;
	#endif
}

int LinxChipkit::WS2812WriteOnePixel(unsigned short pixelIndex, unsigned char red, unsigned char green, unsigned char blue, unsigned char refresh)
{
	#ifdef _BOARD_WF32_ | _BOARD_MEGA_
		m_WS2812Buffer[pixelIndex].red = red;
		m_WS2812Buffer[pixelIndex].green = green;
		m_WS2812Buffer[pixelIndex].blue = blue;
		
		if(refresh != 0)
		{
			while(!m_ws2812.updateLEDs(m_WS2812Buffer));
		}
		
		return L_OK;
	#else
		return L_FUNCTION_NOT_SUPPORTED;
	#endif
}

int LinxChipkit::WS2812WriteNPixels(unsigned short startPixel, unsigned short numPixels, unsigned char* data, unsigned char refresh)
{
	#ifdef _BOARD_WF32_ | _BOARD_MEGA_
		if(startPixel+numPixels <= m_numPixels)
		{
			memcpy(&m_WS2812Buffer[startPixel], data, sizeof(WS2812::GRB)*numPixels);
		}
		else
		{
			memcpy(&m_WS2812Buffer[startPixel], data, sizeof(WS2812::GRB)*m_numPixels-startPixel);
		}
		
		if(refresh != 0)
		{
			while(!m_ws2812.updateLEDs(m_WS2812Buffer));
		}
		
		return L_OK;
	#else
		return L_FUNCTION_NOT_SUPPORTED;
	#endif
}

int LinxChipkit::WS2812Refresh()
{
	#ifdef _BOARD_WF32_ | _BOARD_MEGA_
		while(!m_ws2812.updateLEDs(m_WS2812Buffer));

		return L_OK;
	#else
		return L_FUNCTION_NOT_SUPPORTED;
	#endif
}

 int LinxChipkit::WS2812Close()
 {
	#ifdef _BOARD_WF32_ | _BOARD_MEGA_
		free(m_WS2812Buffer);
		free(m_rgbPatternBuffer);
		
		return L_OK;
	#else
		return L_FUNCTION_NOT_SUPPORTED;
	#endif
		
 }

