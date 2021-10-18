#include "Arduino_Otahelper.h"

void ota_init()
{
  Serial.println("Setup Arduino OTA");
  
  ArduinoOTA.setHostname(OTA_HOST);
  
  ArduinoOTA.onStart([]() { // switch off all the things during upgrade
   Serial.println("\nStart OTA UPDATE.");
  });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
    Serial.println("\nEnd OTA UPDATE.");
    delay(2000);
    });

  ArduinoOTA.onError([](ota_error_t error) {
    (void)error;
    Serial.println("\nERROR OTA UPDATE.");
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)         Serial.println("OTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR)   Serial.println("OTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive Failed");
    else if (error == OTA_END_ERROR)     Serial.println("OTA End Failed");
    delay(2000);
    ESP.restart();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  /* start the OTA server */
  ArduinoOTA.begin();
  }
