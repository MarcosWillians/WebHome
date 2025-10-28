#pragma once

#include <WiFi.h>

class wifiManager {
public:    
    enum WiFiStatus { 
    UNKNOWN,
    DISCONNECTED, 
    CONNECTED, 
    CONNECTING} 
    WifiSts_ = DISCONNECTED;

    bool ismodeAp_ = false;
    void wifiCreateAP(String ssid, String password);    
    void wifiFinishStation();
    void wifiConnectAP(String ssid, String password);
    String wifiAPGetIP();
    WiFiStatus wifiGetConnectStatus();
    void wifiFinishAP();
};