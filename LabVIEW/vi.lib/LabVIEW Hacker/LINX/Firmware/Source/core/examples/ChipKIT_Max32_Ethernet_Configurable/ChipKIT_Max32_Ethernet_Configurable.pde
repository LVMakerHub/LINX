/****************************************************************************************	
**  This is example LINX firmware for use with the chipKIT Max32 with the ethernet
**  interface enabled.  This firware does not specify the IP address or Port which must be
**  set using LINX VIs (for example using the firmware wizard).
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
  
  //The LINX Serial Listener Is Included In NetworkShield Listener And Pre Instantiated.  This Is Necissary For Configuring Ethernet Settings Using LabVIEW.
  LinxSerialConnection.Start(LinxDevice, 0);  
  
  //The LINX Listener Is Pre Instantiated, Call Start And Pass A Pointer To The LINX Device, IP Address and Port To Listen On
  LinxEthernetConnection.Start(LinxDevice);  
}

void loop()
{
  //Listen For New Packets From LabVIEW
  LinxEthernetConnection.CheckForCommands();
  LinxSerialConnection.CheckForCommands();
  
  //Your Code Here, But It will Slow Down The Connection With LabVIEW
}


