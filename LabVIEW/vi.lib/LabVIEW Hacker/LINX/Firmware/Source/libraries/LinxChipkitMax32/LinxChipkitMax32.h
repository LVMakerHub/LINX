/****************************************************************************************
**  LINX header for chipKIT Max32
**
**  For more information see:           www.labviewhacker.com/linx
**  For support visit the forums at:    www.labviewhacker.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_CHIPKIT_MAX32_H
#define LINX_CHIPKIT_MAX32_H

/****************************************************************************************
**  Defines
****************************************************************************************/	
#define DEVICE_NAME_LEN 14

#define NUM_AI_CHANS 16
#define AI_RES_BITS 10
#define AI_REFV 3300000

#define NUM_DIGITAL_CHANS 86

#define NUM_PWM_CHANS 5

#define NUM_SPI_CHANS 1
#define NUM_SPI_SPEEDS 7

#define NUM_I2C_CHANS 1

#define NUM_UART_CHANS 4
#define NUM_UART_SPEEDS 11

/****************************************************************************************
**  Includes
****************************************************************************************/	
#include "utility/LinxDevice.h"
#include "utility/LinxWiringDevice.h"
#include "utility/LinxChipkit.h"
	
class LinxChipkitMax32 : public LinxChipkit
{
	public:	
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		//System
		static const unsigned char m_DeviceName[DEVICE_NAME_LEN];
		
		//AI
		static const unsigned char m_AiChans[NUM_AI_CHANS];
		
		//AI
		//None
		
		//DIGITAL
		static const unsigned char m_DigitalChans[NUM_DIGITAL_CHANS];	
		
		//PWM
		static const unsigned char m_PwmChans[NUM_PWM_CHANS];			
		
		//SPI
		static const unsigned char m_SpiChans[NUM_SPI_CHANS];
		static unsigned long m_SpiSupportedSpeeds[NUM_SPI_SPEEDS];
		static int m_SpiSpeedCodes[NUM_SPI_SPEEDS];
		
		//I2C
		static unsigned char m_I2cChans[NUM_I2C_CHANS];
		static unsigned char m_I2cRefCount[NUM_I2C_CHANS];						
		
		//UART
		static unsigned char m_UartChans[NUM_UART_CHANS];
		static unsigned long m_UartSupportedSpeeds[NUM_UART_SPEEDS];
		
		/****************************************************************************************
		**  Constructors /  Destructor
		****************************************************************************************/
		LinxChipkitMax32();
		
		~LinxChipkitMax32();
			
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		
		
	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
				
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		
		
};


#endif //LINX_CHIPKIT_MAX32_H
