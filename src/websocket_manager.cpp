
#include "websocket_manager.h"

WebsocketManager::WebsocketManager(int Port, wifiManager &wifiMgr) : webSocket(Port),  wifiMgr_(wifiMgr)
{
   
}

void WebsocketManager::setEventCallback(void (*callback)(uint8_t, WStype_t, uint8_t *, size_t)) 
{
    webSocket.onEvent(callback);
}

void WebsocketManager::TaskRefresh() {

 static wifiManager::WiFiStatus lastWifiSts = wifiManager::WiFiStatus::UNKNOWN;
    static bool webSocketEnabled = false;
    static bool serverInitialized = false;

    if (wifiMgr_.ismodeAp_) {
        if(!serverInitialized) {            
            webSocket.begin();
            serverInitialized = true;
            webSocketEnabled = true;                        
        }        
    }
    else if(lastWifiSts != wifiMgr_.wifiGetConnectStatus()) {
        lastWifiSts = wifiMgr_.wifiGetConnectStatus();
        if(lastWifiSts == wifiManager::WiFiStatus::CONNECTED) {            
            if(!serverInitialized) {            
                webSocket.begin();
                serverInitialized = true;                        
            }   
            webSocketEnabled = true;                        
        } else if(lastWifiSts == wifiManager::WiFiStatus::DISCONNECTED) {
            webSocketEnabled = false;            
        }
    }

    if (webSocketEnabled)
        webSocket.loop();    
}

void WebsocketManager::sendTXT(uint8_t num, const char * payload) 
{
    webSocket.sendTXT(num, payload);
}