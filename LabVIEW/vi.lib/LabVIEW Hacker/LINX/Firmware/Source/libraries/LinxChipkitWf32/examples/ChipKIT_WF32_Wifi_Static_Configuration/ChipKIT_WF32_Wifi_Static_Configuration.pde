/****************************************************************************************	
**  This is example LINX firmware for use with the chipKIT WF32 with the WIFI
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

#include <WiFiShieldOrPmodWiFi_G.h>
#include <DNETcK.h>
#include <DWIFIcK.h>

//Include Device Sepcific Header From Sketch>>Import Library (In This Case LinxChipkitMax32.h)
//Also Include Desired LINX Listener From Sketch>>Import Library (In This Case LinxSerialListener.h)
#include <LinxChipkitWf32.h>
#include <LinxChipkitWifiListener.h>
 
//Create A Pointer To The LINX Device Object We Instantiate In Setup()
LinxChipkitWf32* LinxDevice;

//Initialize LINX Device And Listener
void setup()
{
  //Instantiate The LINX Device
  LinxDevice = new LinxChipkitWf32();
  
  //The LINX Listener Is Pre Instantiated.  
  //Set SSID (Network Name), Security Type, Passphrase/Key, And Call Start With Desired Device IP and Port
  LinxWifiConnection.SetSsid("YOUR_NETWORK_NAME");
  LinxWifiConnection.SetSecurity(WPA2_PASSPHRASE);    //NONE, WPA2_PASSPHRASE, WPA2_KEY, WEP40, WEO104
  LinxWifiConnection.SetPassphrase("PASSPHRASE");    //NONE, WPA2_PASSPHRASE, WPA2_KEY, WEP40, WEO104
  LinxWifiConnection.Start(LinxDevice, 192, 168, 1, 128, 44300);  //Start With Fixed IP and Port.  When Using This Method You Cannot Update The IP/Port Using LINX VIs
}

void loop()
{
  //Listen For New Packets From LabVIEW
  LinxWifiConnection.CheckForCommands();
  
  //Your Code Here, But It will Slow Down The Connection With LabVIEW
}


