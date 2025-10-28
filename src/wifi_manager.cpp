#include "wifi_manager.h"

void wifiManager::wifiCreateAP(String ssid, String password)
{
    ismodeAp_ = true;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str(), password.c_str());  
}  

void wifiManager::wifiFinishStation()
{
    WiFi.disconnect(true); 
    WiFi.mode(WIFI_OFF);       
}

void wifiManager::wifiConnectAP(String ssid, String password)
{
    ismodeAp_ = false;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
}

void wifiManager::wifiFinishAP()
{
    WiFi.softAPdisconnect(true); // Desliga o Access Point
    WiFi.mode(WIFI_OFF);         // Desliga o WiFi completamente
}

wifiManager::WiFiStatus wifiManager::wifiGetConnectStatus()
{
    if (WiFi.status() == WL_CONNECTED) {
        WifiSts_ = CONNECTED;
    } else if (WiFi.status() == WL_DISCONNECTED) {
        WifiSts_ = DISCONNECTED;
    } else {
        WifiSts_ = CONNECTING;
    }
    return WifiSts_;
}

String wifiManager::wifiAPGetIP()
{
    IPAddress ip = WiFi.softAPIP();
    return ip.toString();
}