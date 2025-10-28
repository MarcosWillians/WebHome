#include <Arduino.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <ArduinoJson.h>  // para DynamicJsonDocument
#include "ui.h"

UI* UI::instance_ = nullptr;

UI::UI(WebsocketManager &webSocketMgr)
  : webSocketMgr_(webSocketMgr) 
{
}


void UI::WebSocketEventCb(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
    if (instance_) {
        instance_->HandleSocketEventCb(num, type, payload, length);
    }
}



void UI::HandleSocketEventCb(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
    if (type != WStype_TEXT) return;

    StaticJsonDocument<500> doc;
    DeserializationError error = deserializeJson(doc, payload, length);

    if (error) {
        Serial.println("Error parsing JSON");
        return;
    }

    std::unordered_map<std::string,std::string> dataJsonMap ;
    JsonObject obj = doc.as<JsonObject>();
    for (JsonPair kv : obj) {
        dataJsonMap[kv.key().c_str()] = kv.value().as<const char*>();
    }

    if (dataJsonMap.find("FormID") == dataJsonMap.end()) {
        Serial.println("JSON data fields missing");
        return;
    }
    
    currentFormId_ = dataJsonMap["FormID"] ;

    if(registeredForms_.find(currentFormId_) != registeredForms_.end()){
            if (registeredForms_[currentFormId_].callback) {
                registeredForms_[currentFormId_].callback(dataJsonMap, num);
        }
    }
}

void UI::SendData(const std::string formId, std::vector<std::unordered_map<std::string,std::string>> &data, uint8_t num) 
{   
    if(formId != currentFormId_){
        Serial.println("FormID mismatch. Data not sent.");
        return;
    }

    DynamicJsonDocument doc(1024);

    JsonArray arr = doc.to<JsonArray>();  

    for (auto& item : data) {
        JsonObject obj = arr.createNestedObject();  
        for (auto& kv : item) {
            obj[kv.first.c_str()] = kv.second.c_str();
        }
    }

    String jsonStr;
    serializeJson(doc, jsonStr);

    webSocketMgr_.sendTXT(num, jsonStr.c_str());   
}



void UI::RegisterForm(FormInfo info) 
{    
    registeredForms_[info.FormID] = info;
}

// Adicione ao .h:
// private:
//     WebsocketManager &webSocketMgr_;

void UI::setInstance(UI* instance)
{
    instance_ = instance;
}