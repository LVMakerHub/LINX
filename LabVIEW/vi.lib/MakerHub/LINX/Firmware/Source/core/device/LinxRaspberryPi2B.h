/****************************************************************************************
**  LINX header for Raspberry Pi 2 Model B
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_RASPBERRYPI2B_H
#define LINX_RASPBERRYPI2B_H

/****************************************************************************************
**  Defines
****************************************************************************************/	
#define DEVICE_NAME_LEN 23

#define NUM_AI_CHANS 0
#define AI_RES_BITS 0
#define AI_REFV 0
#define NUM_AI_INT_REFS 0

#define NUM_DIGITAL_CHANS 26

#define NUM_PWM_CHANS 0

#define NUM_SPI_CHANS 2
#define NUM_SPI_SPEEDS 13

#define NUM_I2C_CHANS 2

#define NUM_UART_CHANS 1
#define NUM_UART_SPEEDS 19

#define NUM_SERVO_CHANS 0

/****************************************************************************************
**  Includes
****************************************************************************************/	
#include "utility/LinxDevice.h"
#include "utility/LinxRaspberryPi.h"
	
class LinxRaspberryPi2B : public LinxRaspberryPi
{
	public:	
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		//System
		static const unsigned char m_DeviceName[DEVICE_NAME_LEN];
		
		//AI
		static const unsigned char m_AiChans[NUM_AI_CHANS];
		static const unsigned long m_AiRefIntVals[NUM_AI_INT_REFS];
		static const int m_AiRefCodes[NUM_AI_INT_REFS];
		
		//AO
		//None
		
		//DIGITAL
		static const unsigned char m_DigitalChans[NUM_DIGITAL_CHANS];
		static int m_DigitalDirHandles[NUM_DIGITAL_CHANS];
		static int m_DigitalValueHandles[NUM_DIGITAL_CHANS];
		
		//PWM
		static const unsigned char m_PwmChans[NUM_PWM_CHANS];			
		
		//SPI
		static const unsigned char m_SpiChans[NUM_SPI_CHANS];
		static const char m_SpiPaths[NUM_SPI_CHANS][SPI_PATH_LEN];
		static unsigned long m_SpiSupportedSpeeds[NUM_SPI_SPEEDS];
		static int m_SpiSpeedCodes[NUM_SPI_SPEEDS];
		static int m_SpiHandles[NUM_SPI_CHANS];
		
		//I2C
		static unsigned char m_I2cChans[NUM_I2C_CHANS];
		static unsigned char m_I2cRefCount[NUM_I2C_CHANS];	
	    static int m_I2cHandles[NUM_I2C_CHANS];		
		static const char m_I2cPaths[NUM_I2C_CHANS][I2C_PATH_LEN];
		
		//UART
		static unsigned char m_UartChans[NUM_UART_CHANS];
		static unsigned long m_UartSupportedSpeeds[NUM_UART_SPEEDS];
		static unsigned long m_UartSupportedSpeedsCodes[NUM_UART_SPEEDS];
		static int m_UartHandles[NUM_UART_CHANS];
		static const char m_UartPaths[NUM_UART_CHANS][UART_PATH_LEN];

		
		//Servo		
		//static const unsigned char m_ServoChans[NUM_SERVO_CHANS];
		//static Servo* m_Servos[NUM_SERVO_CHANS];
		
		/****************************************************************************************
		**  Constructors /  Destructor
		****************************************************************************************/
		LinxRaspberryPi2B();
		
		~LinxRaspberryPi2B();
			
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


#endif //LINX_RASPBERRYPI2B
