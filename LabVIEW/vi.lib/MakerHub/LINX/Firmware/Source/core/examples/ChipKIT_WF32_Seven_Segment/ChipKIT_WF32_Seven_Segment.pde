/****************************************************************************************	
**  This is example LINX firmware for use with the chipKIT WF32 for the Seven Segment 
**  Display from the LabVIEW Interaction Parts Kit
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

#define A channels[0]
#define F channels[1]
#define G channels[2]
#define B channels[3]
#define E channels[4]
#define D channels[5]
#define C channels[6]
#define DP channels[7]
   
#define Anode1 channels[8]
#define Anode2 channels[9]
#define Anode3 channels[10]
#define Anode4 channels[11]

int updateDisplay();
int changeChannels();
unsigned char channels[12] = {5, 4, 33, 3, 29, 30, 32, 31, 9, 11, 12, 13};
unsigned char hue[2] = {1, 15};
int activateTask = 0;
 
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
  LinxSerialConnection.AttachPeriodicTask(updateDisplay);
}

void loop()
{
  //Listen For New Packets From LabVIEW
  LinxSerialConnection.CheckForCommands();
  
  //Your Code Here, But It will Slow Down The Connection With LabVIEW
}

int changeValue(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response)
{
   
  for(int i=0; i<2; i++)
  {
    hue[i] = input[i];
  }
   
  *numResponseBytes = 0;
  return 0;
}

int changeChannels(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response)
{
   
  for(int i=0; i<12; i++)
  {
    channels[i] = input[i];
  }
   
  *numResponseBytes = 0;
  activateTask = 1;
  return 0;
}

int updateDisplay(unsigned char* input, unsigned char* output)
{
  unsigned int digitCounter = (hue[0] << 8) | hue[1];
  unsigned int toDisplay = (hue[0] << 8) | hue[1];
  unsigned int digit = 1;
  unsigned int length = 0;
  
  pinMode(Anode1, OUTPUT);
  pinMode(Anode2, OUTPUT);
  pinMode(Anode3, OUTPUT);
  pinMode(Anode4, OUTPUT);
  
  pinMode(A,OUTPUT);
  pinMode(B,OUTPUT);
  pinMode(C,OUTPUT);
  pinMode(D,OUTPUT);
  pinMode(E,OUTPUT);
  pinMode(F,OUTPUT);
  pinMode(G,OUTPUT);
  pinMode(DP,OUTPUT);
  
  if (toDisplay == 0)
  {
    length = 1;
  }
  else
  {
    
    while (digitCounter != 0) 
      { 
        digitCounter /= 10; 
        length++; 
      }
  }
  for (int i=0; i<length; i++) {
    digit=4-i;
    switch(digit){
   
      case 1:
       
      digitalWrite(Anode1,HIGH);
      digitalWrite(Anode2,LOW);
      digitalWrite(Anode3,LOW);
      digitalWrite(Anode4,LOW);
      decodeAndWrite((toDisplay/1000)%10, channels);
      break;
       
      case 2:
       
      digitalWrite(Anode1,LOW);
      digitalWrite(Anode2,HIGH);
      digitalWrite(Anode3,LOW);
      digitalWrite(Anode4,LOW);
      decodeAndWrite((toDisplay/100)%10, channels);
      break;
       
      case 3:
       
      digitalWrite(Anode1,LOW);
      digitalWrite(Anode2,LOW);
      digitalWrite(Anode3,HIGH);
      digitalWrite(Anode4,LOW);
      decodeAndWrite((toDisplay/10)%10, channels);
      break;
         
      case 4:
       
      digitalWrite(Anode1,LOW);
      digitalWrite(Anode2,LOW);
      digitalWrite(Anode3,LOW);
      digitalWrite(Anode4,HIGH);
      decodeAndWrite(toDisplay%10, channels);
      break;
       
      default:
      //too many digits given!
      digitalWrite(Anode1,LOW);
      digitalWrite(Anode2,LOW);
      digitalWrite(Anode3,LOW);
      digitalWrite(Anode4,LOW);
      break;
 
      }
     delayMicroseconds(500);
  }
  return 0;
}

void decodeAndWrite(unsigned int digitValue, unsigned char* channels){
 // Write values all high to turn off the previous number
   
  digitalWrite(A,HIGH);
  digitalWrite(B,HIGH);
  digitalWrite(C,HIGH);
  digitalWrite(D,HIGH);
  digitalWrite(E,HIGH);
  digitalWrite(F,HIGH);
  digitalWrite(G,HIGH);
  digitalWrite(DP,HIGH);
  
  switch(digitValue){
    //  cases for each digit
     
    case 0:
     
    digitalWrite(A,LOW);
    digitalWrite(F,LOW);
    digitalWrite(E,LOW);
    digitalWrite(D,LOW);
    digitalWrite(C,LOW);
    digitalWrite(B,LOW);  
    break;
     
    case 1:
    
    digitalWrite(C,LOW);
    digitalWrite(B,LOW);
    break;
     
    case 2:
     
    digitalWrite(A,LOW);
    digitalWrite(G,LOW);
    digitalWrite(E,LOW);
    digitalWrite(D,LOW);
    digitalWrite(B,LOW);
    break;
     
    case 3:
     
    digitalWrite(C,LOW);
    digitalWrite(B,LOW);
    digitalWrite(A,LOW);
    digitalWrite(G,LOW);
    digitalWrite(D,LOW);
    break;
     
    case 4:
     
    digitalWrite(F,LOW);
    digitalWrite(B,LOW);
    digitalWrite(G,LOW);
    digitalWrite(C,LOW);
    break;
     
    case 5:
     
    digitalWrite(A,LOW);
    digitalWrite(F,LOW);
    digitalWrite(G,LOW);
    digitalWrite(C,LOW);
    digitalWrite(D,LOW);
    break;
     
    case 6:
     
    digitalWrite(C,LOW);
    digitalWrite(G,LOW);
    digitalWrite(E,LOW);
    digitalWrite(D,LOW);
    digitalWrite(F,LOW);  
    break;
     
    case 7:
     
    digitalWrite(A,LOW);
    digitalWrite(B,LOW);
    digitalWrite(C,LOW);
    break;
     
    case 8:
     
    digitalWrite(A,LOW);
    digitalWrite(F,LOW);
    digitalWrite(B,LOW);
    digitalWrite(G,LOW);
    digitalWrite(C,LOW);
    digitalWrite(D,LOW);
    digitalWrite(E,LOW);  
    break;
     
    case 9:
     
    digitalWrite(A,LOW);
    digitalWrite(F,LOW);
    digitalWrite(B,LOW);
    digitalWrite(G,LOW);
    digitalWrite(C,LOW);
    break;
     
    default:
    // if unknown value, display nothing
    break;
     
  }
}


