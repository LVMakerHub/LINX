/****************************************************************************************
**  LINX header for Raspberry Pi 2B
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Ken Sharp
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

#define NUM_CAN_CHANS 0

#define NUM_DIGITAL_CHANS 17

#define NUM_PWM_CHANS 0

#define NUM_SPI_CHANS 1
#define NUM_SPI_SPEEDS 13

#define NUM_I2C_CHANS 1

#define NUM_UART_CHANS 1
#define NUM_UART_SPEEDS 18

#define NUM_SERVO_CHANS 0

#define PI_OS_GPIO_DIRECTION 3

/****************************************************************************************
**  Includes
****************************************************************************************/	
#include "utility/LinxDevice.h"
#include "utility/LinxRaspberryPi.h"
#include <string>
#include <map>

using namespace std;
	
class LinxRaspberryPi2B : public LinxRaspberryPi
{
	public:	
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		//System
		static const unsigned char m_DeviceName[DEVICE_NAME_LEN];
		
		//AI		
		//None
		
		//AO
		//None
		
		//CAN
		//None
		
		//DIGITAL
		static const unsigned char m_DigitalChans[NUM_DIGITAL_CHANS];
		static const unsigned int m_gpioChan[NUM_DIGITAL_CHANS];
		
		//PWM
		//None
		
		//SPI
		static const unsigned char m_SpiChans[NUM_SPI_CHANS];
		static int m_SpiHandles[NUM_SPI_CHANS];
		static unsigned long m_SpiSupportedSpeeds[NUM_SPI_SPEEDS];
		static int m_SpiSpeedCodes[NUM_SPI_SPEEDS];
				
		//I2C
		static unsigned char m_I2cChans[NUM_I2C_CHANS];
		static unsigned char m_I2cRefCount[NUM_I2C_CHANS];		
		
		//UART
		static unsigned char m_UartChans[NUM_UART_CHANS];
		static unsigned long m_UartSupportedSpeeds[NUM_UART_SPEEDS];
		static unsigned long m_UartSupportedSpeedsCodes[NUM_UART_SPEEDS];
		static int m_UartHandles[NUM_UART_CHANS];
		static string m_UartPaths[NUM_UART_CHANS];

		
		//Servo		
		//None
		
		/****************************************************************************************
		**  Constructors /  Destructor
		****************************************************************************************/
		LinxRaspberryPi2B();
		virtual ~LinxRaspberryPi2B();
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
				
	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		int m_gpioBase;

		/****************************************************************************************
		**  Functions
		****************************************************************************************/
		int getGpioBase();		
};

#endif //LINX_RASPBERRYPI5
