#ifndef WIFI_HELPER_H
 #define WIFI_HELPER_H
 
 #ifdef ESP32 
   #include <WiFi.h>
 #else 
   #include <ESP8266WiFi.h>
   #include <ESP8266mDNS.h>
 #endif

 #ifndef AP_SSID
   #pragma No AP_SSID configured
   #define AP_SSID Arduino-SoftAP      // Set AP default SSID
 #else
   #define AP_ON
 #endif
 #ifndef AP_PASS
   #pragma No AP_PASS configured
   #define AP_PASS Arduino             //set AP default password
 #endif
 
 #ifndef CLIENTSSID
   #pragma No CLIENTSSID configured
   #define CLIENTSSID ""  
 #endif
 #ifndef CLIENTPASSWORD
   #pragma No CLIENTPASSWORD configured
   #define CLIENTPASSWORD "" 
 #endif

 #include <WiFiUdp.h>

 void wifi_init();
 
 int _wifiConnectRetry = 3;

#endif
