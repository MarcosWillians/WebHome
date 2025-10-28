#pragma once

#include <WebSocketsServer.h>
#include "wifi_manager.h"

class WebsocketManager {
public:    
    WebsocketManager(int Port, wifiManager &wifiMgr);
    void setEventCallback(void (*callback)(uint8_t, WStype_t, uint8_t *, size_t));
    void sendTXT(uint8_t num, const char * payload);
    void TaskRefresh();  
private:
    WebSocketsServer webSocket;
    wifiManager &wifiMgr_;
};  