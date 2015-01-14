/****************************************************************************************	
**  This is example LINX firmware for use with the chipKIT Max32 with the ethernet
**  interface enabled.
**
**  For more information see:           www.labviewhacker.com/linx
**  For support visit the forums at:    www.labviewhacker.com/forums/linx
**  
**  Written By Sam Kristoff
**
**  BSD2 license.
****************************************************************************************/

//Include All Peripheral Libraries Used By LINX
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Servo.h>

#include <NetworkShield.h> 
#include <DNETcK.h>

//Include Device Sepcific Header From Sketch>>Import Library (In This Case LinxChipkitMax32.h)
//Also Include Desired LINX Listener From Sketch>>Import Library (In This Case LinxSerialListener.h)
#include <LinxChipkitMax32.h>
#include <LinxChipkitNetworkShieldListener.h>
 
//Create A Pointer To The LINX Device Object We Instantiate In Setup()
LinxChipkitMax32* LinxDevice;

//Initialize LINX Device And Listener
void setup()
{
  //Instantiate The LINX Device
  LinxDevice = new LinxChipkitMax32();
  
  //The LINX Listener Is Pre Instantiated, Call Start And Pass A Pointer To The LINX Device, IP Address and Port To Listen On
  LinxEthernetConnection.Start(LinxDevice, 192, 168, 1, 129, 44300);  
}

void loop()
{
  //Listen For New Packets From LabVIEW
  LinxEthernetConnection.CheckForCommands();
  
  //Your Code Here, But It will Slow Down The Connection With LabVIEW
}


