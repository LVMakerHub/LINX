/****************************************************************************************	
**  This is example LINX firmware for use with the ESP8266
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Ken Sharp
**
**  BSD2 License.
****************************************************************************************/

//Include All Peripheral Libraries Used By LINX
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Servo.h>
#include <ESP8266WiFi.h>

//Include Device Specific Header From Sketch>>Import Library (In This Case LinxChipkitMax32.h)
//Also Include Desired LINX Listener From Sketch>>Import Library 
#include <LinxESP8266.h>
#include <LinxESP8266WifiListener.h>
 
//Create A Pointer To The LINX Device Object We Instantiate In Setup()
LinxESP8266* LinxDevice;

//Initialize LINX Device And Listener
void setup()
{
  //Instantiate The LINX Device
  LinxDevice = new LinxESP8266();
  
  //The LINX Serial Listener Is Included In WIFI Listener And Pre Instantiated.  This Is Necessary For Configuring Wifi Settings.
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
