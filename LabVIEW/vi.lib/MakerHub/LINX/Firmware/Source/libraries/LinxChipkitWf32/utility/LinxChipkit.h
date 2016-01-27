/****************************************************************************************
**  LINX header for chipKIT family of devices.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_CHIPKIT_H
#define LINX_CHIPKIT_H

/****************************************************************************************
**  Defines
****************************************************************************************/
#include "LinxDevice.h"
#include "LinxWiringDevice.h"
	
class LinxChipkit : public LinxWiringDevice
{
	public:	
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		
		/****************************************************************************************
		**  Constructors /  Destructor
		****************************************************************************************/
		LinxChipkit();
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		//WS2812
		virtual int Ws2812Open(unsigned short numLeds, unsigned char dataChan);
		virtual int Ws2812WriteOnePixel(unsigned short pixelIndex, unsigned char red, unsigned char green, unsigned char blue, unsigned char refresh);
		virtual int Ws2812WriteNPixels(unsigned short startPixel, unsigned short numPixels, unsigned char* data, unsigned char refresh);
		virtual int Ws2812Refresh();
		virtual int Ws2812Close();
		
	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
				
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
	
};

#endif //LINX_CHIPKIT_H