/****************************************************************************************	
**  This is example LINX firmware for use with the chipKIT WF32 with the LED matrix 
**  from the LabVIEW Interaction Parts Kit
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sudharsan Sukumar
**
**  BSD2 License.
****************************************************************************************/

//Include All Peripheral Libraries Used By LINX
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Servo.h>
#include <WS2812.h>

//Include Device Specific Header From Sketch>>Import Library (In This Case LinxChipkitMax32.h)
//Also Include Desired LINX Listener From Sketch>>Import Library (In This Case LinxSerialListener.h)
#include <LinxChipkitWf32.h>
#include <LinxSerialListener.h>

#define col1 channels[0]
#define col2 channels[1]
#define col3 channels[2]
#define col4 channels[3]
#define col5 channels[4]
#define col6 channels[5]
#define col7 channels[6]
#define col8 channels[7]
   
#define row1 channels[8]
#define row2 channels[9]
#define row3 channels[10]
#define row4 channels[11]
#define row5 channels[12]
#define row6 channels[13]
#define row7 channels[14]
#define row8 channels[15]

int displayMatrix();
int changeChannels();
int changeValue();
int activateTask = 0;
unsigned char hue[8] = {255, 255, 255, 255, 255, 255, 255, 255};
unsigned char channels[16] = {26, 27, 28, 29, 30, 31, 32, 33, 3, 4, 5, 6, 7, 8, 9, 11};
 
//Create A Pointer To The LINX Device Object We Instantiate In Setup()
LinxChipkitWf32* LinxDevice;

//Initialize LINX Device And Listener
void setup()
{
  
  //Instantiate The LINX Device
  LinxDevice = new LinxChipkitWf32();
  
  //The LINXT Listener Is Pre Instantiated, Call Start And Pass A Pointer To The LINX Device And The UART Channel To Listen On
  LinxSerialConnection.Start(LinxDevice, 0);  
  
  LinxSerialConnection.AttachCustomCommand(0, changeValue);
  LinxSerialConnection.AttachCustomCommand(1, changeChannels);
  LinxSerialConnection.AttachPeriodicTask(displayMatrix);
}

void loop()
{
  //Listen For New Packets From LabVIEW
  LinxSerialConnection.CheckForCommands();
  
  //Your Code Here, But It will Slow Down The Connection With LabVIEW
}

int changeChannels(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response)
{
   
  for(int i=0; i<16; i++)
  {
    channels[i] = input[i];
  }
   
  *numResponseBytes = 0;
  activateTask = 1;
  return 0;
}

int changeValue(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response)
{
   
  for(int i=0; i<8; i++)
  {
    hue[i] = input[i];
  }
   
  *numResponseBytes = 0;
  return 0;
}

int displayMatrix(unsigned char* input, unsigned char* output)
{ 
  pinMode(col1, OUTPUT);
  pinMode(col2, OUTPUT);
  pinMode(col3, OUTPUT);
  pinMode(col4, OUTPUT);
  pinMode(col5, OUTPUT);
  pinMode(col6, OUTPUT);
  pinMode(col7, OUTPUT);
  pinMode(col8, OUTPUT);
  pinMode(row1, OUTPUT);
  pinMode(row2, OUTPUT);
  pinMode(row3, OUTPUT);
  pinMode(row4, OUTPUT);
  pinMode(row5, OUTPUT);
  pinMode(row6, OUTPUT);
  pinMode(row7, OUTPUT);
  pinMode(row8, OUTPUT);
  
  char rows[8] = {row1, row2, row3, row4, row5, row6, row7, row8};
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(col1, LOW);
    digitalWrite(col2, LOW);
    digitalWrite(col3, LOW);
    digitalWrite(col4, LOW);
    digitalWrite(col5, LOW);
    digitalWrite(col6, LOW);
    digitalWrite(col7, LOW);
    digitalWrite(col8, LOW);
    
    digitalWrite(col8, hue[i] & 1);
    digitalWrite(col7, hue[i] >> 1 & 1);
    digitalWrite(col6, hue[i] >> 2 & 1);
    digitalWrite(col5, hue[i] >> 3 & 1);
    digitalWrite(col4, hue[i] >> 4 & 1);
    digitalWrite(col3, hue[i] >> 5 & 1);
    digitalWrite(col2, hue[i] >> 6 & 1);
    digitalWrite(col1, hue[i] >> 7 & 1);
    
    
    digitalWrite(rows[i], LOW);
    delayMicroseconds(500);
    digitalWrite(rows[i], HIGH);
   
  }
  
  
  return 0;
}

