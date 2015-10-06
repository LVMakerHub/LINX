/****************************************************************************************
**  LINX header for BeagleBone Black
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
** BSD2 License.
****************************************************************************************/	

#ifndef LINX_BEAGLEBONEBLACK_H
#define LINX_BEAGLEBONEBLACK_H

/****************************************************************************************
**  Defines
****************************************************************************************/	
#define DEVICE_NAME_LEN 17

#define NUM_AI_CHANS 8
#define AI_RES_BITS 12
#define AI_REFV 1800000
#define NUM_AI_INT_REFS 0

#define NUM_DIGITAL_CHANS 16

#define NUM_PWM_CHANS 4

#define NUM_SPI_CHANS 1
#define NUM_SPI_SPEEDS 13

#define NUM_I2C_CHANS 2

#define NUM_UART_CHANS 3
#define NUM_UART_SPEEDS 18

#define NUM_SERVO_CHANS 0

/****************************************************************************************
**  Includes
****************************************************************************************/	
#include "utility/LinxDevice.h"
#include "utility/LinxBeagleBone.h"
#include <map>
#include <string>

using namespace std;
	
class LinxBeagleBoneBlack : public LinxBeagleBone
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
		static FILE* m_AiValueHandles[NUM_AI_CHANS];		
		static const string m_AiValuePaths[NUM_AI_CHANS];
		
		//AO
		//None
		
		//DIGITAL
		static const unsigned char m_DigitalChans[NUM_DIGITAL_CHANS];
		static const unsigned char m_gpioChan[NUM_DIGITAL_CHANS];
		
		//PWM
		static const unsigned char m_PwmChans[NUM_PWM_CHANS];
		static const string m_PwmDirPaths[NUM_PWM_CHANS];
		static const string m_PwmDtoNames[NUM_PWM_CHANS];
		
		//SPI
		static unsigned char m_SpiChans[NUM_SPI_CHANS];
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
		//none
		
		/****************************************************************************************
		**  Constructors /  Destructor
		****************************************************************************************/
		LinxBeagleBoneBlack();
		~LinxBeagleBoneBlack();
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
				
	private:
		/****************************************************************************************
		**  Variables
		****************************************************************************************/		
		const char* overlaySlotsPath;		//The overlay slot export path
		
		/****************************************************************************************
		**  Functions
		****************************************************************************************/
				
};

#endif //LINX_BEAGLEBONEBLACK_H