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
#include <DHT.h>

//Include Device Specific Header From Sketch>>Import Library (In This Case LinxChipkitMax32.h)
//Also Include Desired LINX Listener From Sketch>>Import Library (In This Case LinxSerialListener.h)
#include <LinxChipkitWf32.h>
#include <LinxSerialListener.h>

#define E channels[0]
#define D channels[1]
#define DP channels[2]
#define C channels[3]
#define G channels[4]
#define Anode4 channels[5]
#define B channels[6]
#define Anode3 channels[7]
#define Anode2 channels[8]
#define F channels[9]
#define A channels[10]
#define Anode1 channels[11]

#define row5 channels[0]
#define row7 channels[1]
#define col2 channels[2]
#define col3 channels[3]
#define row8 channels[4]
#define col5 channels[5]
#define row6 channels[6]
#define row3 channels[7]
#define row1 channels[8]
#define col4 channels[9]
#define col6 channels[10]
#define row4 channels[11]
#define col1 channels[12]
#define row2 channels[13]
#define col7 channels[14]
#define col8 channels[15]

int updateDisplay();
int changeChannels();
int dht();
unsigned char channels[16] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char hue[8] = {
  0, 0, 0, 0, 0, 0, 0, 0};
int activateTask = -1;

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
  LinxSerialConnection.AttachCustomCommand(2, dht);
  LinxSerialConnection.AttachPeriodicTask(periodicTaskMain);
}

void loop()
{
  //Listen For New Packets From LabVIEW
  LinxSerialConnection.CheckForCommands();

  //Your Code Here, But It will Slow Down The Connection With LabVIEW
}

int changeValue(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response)
{

  for(int i=0; i<numInputBytes; i++)
  {
    hue[i] = input[i];
  }

  *numResponseBytes = 0;
  return 0;
}

int changeChannels(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response)
{

  for(int i=0; i<numInputBytes; i++)
  {
    channels[i] = input[i];
  }

  *numResponseBytes = 0;
  activateTask = input[numInputBytes-1];
  return 0;
}

int periodicTaskMain(unsigned char* input, unsigned char* output)
{
  if (channels[0] != 0)
  {
    if (activateTask == 0)
    {
      return displayMatrix(input, output);
    }
    if (activateTask == 1)
    {
      return updateDisplay(input, output);
    }
    else
    {
      return 0;
    }
  }
  else
  {
    return 0;
  }
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

  char rows[8] = {
    row1, row2, row3, row4, row5, row6, row7, row8  };
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

int dht(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response)
{
  if (numInputBytes != 1)
  {
    return 1;
  }

  DHT dht(input[0], DHT11);

  dht.begin();
  delay(1000);

  unsigned char h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  unsigned char t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  unsigned char f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    return 1;
  }

  unsigned char hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  unsigned char hic = dht.computeHeatIndex(t, h, false);

  response[0] = h;
  response[1] = t;
  response[2] = f;
  response[3] = hic;
  response[4] = hif;

  *numResponseBytes = 5;

  return 0;
}



