#pragma once
#include <Arduino.h>
#include <unordered_map>
#include <functional>
#include <vector>
#include "websocket_manager.h"

class UI {
public:
    using FormCallback = std::function<void(const std::unordered_map<std::string,std::string>& data, uint8_t num)>;
    
    struct FormInfo {
        std::string FormID;        
        FormCallback callback;        
    };  
    
    UI(WebsocketManager &webSocketMgr);
    static void WebSocketEventCb(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
    void HandleSocketEventCb(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
    static void setInstance(UI* instance);       
    void SendData(const std::string formId, std::vector<std::unordered_map<std::string,std::string>> &data, uint8_t num);
    void RegisterForm(FormInfo info);

private:
    WebsocketManager& webSocketMgr_;
    std::unordered_map<std::string,FormInfo> registeredForms_;
    static UI* instance_;
    std::string currentFormId_;
};

