/****************************************************************************************	
**  This is example LINX firmware for use with the chipKIT WF32 with the WIFI
**  interface enabled.
**
**  For more information see:           www.labviewhacker.com/linx
**  For support visit the forums at:    www.labviewhacker.com/forums/linx
**  
**  Written By Sam Kristoff
**
**  BSD2 License.
****************************************************************************************/

//Include All Peripheral Libraries Used By LINX
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Servo.h>

#include <WiFiShieldOrPmodWiFi_G.h>
#include <DNETcK.h>
#include <DWIFIcK.h>

//Include Device Sepcific Header From Sketch>>Import Library (In This Case LinxChipkitMax32.h)
//Also Include Desired LINX Listener From Sketch>>Import Library 
#include <LinxChipkitWf32.h>
#include <LinxChipkitWifiListener.h>
 
//Create A Pointer To The LINX Device Object We Instantiate In Setup()
LinxChipkitWf32* LinxDevice;

//Initialize LINX Device And Listener
void setup()
{
  //Instantiate The LINX Device
  LinxDevice = new LinxChipkitWf32();
  
  //The LINX Serial Listener Is Included In WIFI Listener And Pre Instantiated.  This Is Necissary For Configuring Wifi Settings.
  LinxSerialConnection.Start(LinxDevice, 0);  
  
  //Start Wifi Listener.  Settings (IP, SSID, etc) Will Be Read From Non Volatile Storage And Can Be Set Using LINX VIs Via USB
  LinxWifiConnection.Start(LinxDevice);  
}

void loop()
{
  //Listen For New Packets From LabVIEW
  LinxWifiConnection.CheckForCommands();
  LinxSerialConnection.CheckForCommands();
  
  //Your Code Here, But It will Slow Down The Connection With LabVIEW
}


