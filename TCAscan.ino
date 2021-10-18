    // --------------------------------------
// TCA i2c_mux address_scanner
//
// Version 1
// based on one of I2C bus scanner scketch found somewhere on the internet,
// collecting answering TCA chip adddresses into an array, and walk trough on all MUX all channel.
// Wifi and OTA started for ESP32 / ESP8266
// wifi and ota copnfiguration in private.h
//
// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.
// TCA reset/interrupt pins not supported (yet).
//
// This program uses serail port at 112500 baud for output. Wifi only for OTA update only.
// version 0.4


#include <Wire.h>
#include "private.h"                                     // contains users, passwords, hostname configs. with this I don't keep any sensitive information int the code.
#include "lib/wifi_helper/wifi_helper.cpp"               // gets information from private.h definitions, and starts wifi AP, and Client for OTA (OTA not touched here.)
#include "lib/Arduino_Otahelper/Arduino_Otahelper.cpp"   // function for prepare ArduinoOTA.
#include "lib//TCA/TCA.cpp"

bool mainBusScanNotDone = true;
int tcaaddr[8]={0,0,0,0,0,0,0,0};   // this will keep possible TCA adresses
int i=0;                            // tcaaddr pointer
byte error, address;
extern int nDevices =0;                    // Device count
  
void waitForSerialKeyPress()
  {
  //  
  // Special thanks for "Magyar Arduino csoport" FB group members who inspired/helped with this function code.
  //
  Serial.println("Press any key to continue...");
  Serial.flush();
  while (!Serial.available())
    {
    yield();                // need for ESP. this makes a "gap int he code" to let run WiFi
    ArduinoOTA.handle();    // let run OTA while waiting for keypress
  }
  while (Serial.available()) 
    {
    Serial.read();
    yield();                // need for ESP. this makes a "gap int he code" to let run WiFi
    ArduinoOTA.handle();    // let run OTA while waiting for keypress
  }
}

 void scan(int tcaaddr[8]) {
  ArduinoOTA.handle();
  int nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    ArduinoOTA.handle();
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (address<16) Serial.print("0");
    Serial.print(address,HEX);Serial.print("\r");
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
      // add MUX address to the tcaaddr array if address answering.
      // still not checking if MUX or not.
      if ( mainBusScanNotDone and  (address > 111 and address < 120) )   
        {
        Serial.println("Possible I2C MUX found. Adding to MUX addresses.");
        tcaaddr[i]=address;
        i++;
        }
      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16){ Serial.print("0"); }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    {
    Serial.println("Bus scan done.\n");
    Serial.println("Found "+ String(nDevices));
    }
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);             // wait for serial monitor, some microcontroller need it.
  
  Wire.begin();
  
  // this 2 line just for start OTA.
  wifi_init();   // init wifiAP and wificlient from "wifi_helper.cpp"
  ota_init();    // ArduinoOTA basic initalize from "Arduino_Otahelper.cpp". After this just use "ArduinoOTA.handle();" in the code to keep run OTA.
}


void loop()
{
  int tcaaddr[8]={0,0,0,0,0,0,0,0};
  byte error, address;
  Serial.println("\nI2C Scanner\n");
  Serial.println("Scanning...");
  scan((int *) &tcaaddr);
  mainBusScanNotDone = false;    // turn off TCA adressess re-adding to array. I'ts already done once.
  Serial.println("Main bus scan done.\n");

  if (!tcaaddr[0] == 0)   
    {   
    Serial.println("---------------------------------------------------------");
    Serial.println("Listing possible I2C MUX addresses:");
    for (int i=0; i<sizeof tcaaddr/sizeof tcaaddr[0]; i++)
      {
      int s = tcaaddr[i];
      if (s != 0)
        {
        Serial.print ("0x");
        Serial.println(s,HEX);
        }
      }
    Serial.println("---------------------------------------------------------");
    Serial.println("Repeat Scan with cannel canhges.\n");
    
    for (int i=0; i<sizeof tcaaddr/sizeof tcaaddr[0]; i++)
      {
      int s = tcaaddr[i];
      if (s != 0)
        {
        TCA tca1(s);
        tca1.begin();
        if (tca1.isConnected()) 
           {
           Serial.println ("TCA connected Try scanning on all channel."); 
           for (int Ch=0; Ch <= tca1.getMaxCh(); Ch++)
             {
             Serial.print ("Address: 0x");
             Serial.print(s,HEX);
             Serial.println("  Cahnnel: " + String(Ch) + " Scanning...");
             tca1.selectCh(Ch);
             scan((int *) &tcaaddr);
             Serial.println("---------------------------------------------------------");
             }
           }
         }
       }
    }     
  Serial.println("Program complete.");
    
  waitForSerialKeyPress();
  mainBusScanNotDone = true;
}
