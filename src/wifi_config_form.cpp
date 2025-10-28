
#include "wifi_config_form.h"
#include <unordered_map>
#include <Arduino.h>

WifiConfigForm::WifiConfigForm(UI &ui, DataPersistence dataPersistence) : ui_(ui), dataPersistence_(dataPersistence)  
{
    UI::FormInfo CfgForm;
    CfgForm.FormID = "WifiConfigForm";    
    CfgForm.callback = [this](const std::unordered_map<std::string,std::string>& data, uint8_t num) {
        this->WifiConfigFormCallback(data, num);
    };
    ui_.RegisterForm(CfgForm);   
}

void WifiConfigForm::WifiConfigFormCallback(const std::unordered_map<std::string,std::string>& data, uint8_t num)
{  
    std::vector<std::unordered_map<std::string,std::string>> response;
    Serial.println("WifiConfigFormCallback()");
    if(data.find("action") != data.end()){
        std::string ip;
        std::string gateway;
        std::string subnet;
        std::string pass;
        std::string ssid;
        dataPersistence_.ReadDocument("wifi_config");         
        std::string action = data.at("action");
        if(action == "load_form"){
            Serial.println("load_form");
            dataPersistence_.GetEntry("ip", ip);
            dataPersistence_.GetEntry("gateway", gateway);
            dataPersistence_.GetEntry("subnet", subnet);
            dataPersistence_.GetEntry("ssid", ssid);
            dataPersistence_.GetEntry("pass", pass);  
            response.push_back({{"type", "formInput"},{"ipvalue",ip},{"gateway",gateway},{"subnet",subnet},{"ssid",ssid},{"pass",pass}});            
            ui_.SendData("WifiConfigForm", response, num);
        }
        else if(action == "savewifiConfig"){            
            dataPersistence_.AddOrUpdateEntry("ip", data.at("ip"));
            dataPersistence_.AddOrUpdateEntry("gateway", data.at("gateway"));
            dataPersistence_.AddOrUpdateEntry("subnet", data.at("subnet"));
            dataPersistence_.AddOrUpdateEntry("ssid", data.at("ssid"));
            dataPersistence_.AddOrUpdateEntry("pass", data.at("pass"));                        
            dataPersistence_.SaveDocument();
        }
    }
}
