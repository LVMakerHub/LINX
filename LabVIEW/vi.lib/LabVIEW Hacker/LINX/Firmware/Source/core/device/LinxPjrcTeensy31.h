/****************************************************************************************
**  LINX header for PJRC Teensy 3.1
**
**  For more information see:           www.labviewhacker.com/linx
**  For support visit the forums at:    www.labviewhacker.com/forums/linx
**  
**  Written By Sam Kristoff
**
** MIT license.
****************************************************************************************/	

#ifndef LINX_PJRC_TEENSY31_H
#define LINX_PJRC_TEENSY31_H

/****************************************************************************************
**  Defines
****************************************************************************************/	
#define DEVICE_NAME_LEN 16

#define NUM_AI_CHANS 21
#define AI_RES_BITS 12
#define AI_REFV 3300000

#define NUM_DIGITAL_CHANS 24

#define NUM_PWM_CHANS 12

#define NUM_SPI_CHANS 1
#define NUM_SPI_SPEEDS 7

#define NUM_I2C_CHANS 2

#define NUM_UART_CHANS 3
#define NUM_UART_SPEEDS 13

/****************************************************************************************
**  Includes
****************************************************************************************/	
#include "utility/LinxDevice.h"
#include "utility/LinxWiringDevice.h"
#include "utility/LinxPjrc.h"
	
class LinxPjrcTeensy31 : public LinxPjrc
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
		LinxPjrcTeensy31();
		
		~LinxPjrcTeensy31();
			
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


#endif //LINX_PJRC_TEENSY31_H
