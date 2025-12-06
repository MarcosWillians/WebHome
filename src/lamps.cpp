
#include "lamps.h"
#include <iostream>

Lamps::Lamps(MainNavigatorForm &mainNavigatorForm, DataPersistence &dataPersistence, Command& commandProcessor) : mainNavigatorForm_(mainNavigatorForm), dataPersistence_(dataPersistence), commandProcessor_(commandProcessor) 
{    
    MainNavigatorForm::DeviceCallback lampCallback = [this](const std::unordered_map<std::string,std::string>& data) {
        this->LampActionCallback(data);
    };
    mainNavigatorForm_.RegiterDeviceCallback("lamps", lampCallback);   

    LoadLampsFromPersistence();

    MainNavigatorForm::DeviceUpdateCb lampUpdateCb = [this](std::string deviceName) -> std::unordered_map<std::string,std::string>& {
        return this->LampUpdateCb(deviceName);       
    };
    mainNavigatorForm_.RegiterDeviceUpdateCb("lamps", lampUpdateCb);
    mainNavigatorForm_.RegisterDeviceNameList("lamps", lampNameList);

    Command::CommandCallback cmdCb = [this](const std::unordered_map<std::string,std::string>& data) -> std::string {
        std::string response;
        response = LampCommandCallback(data);
        return response;
    };
    commandProcessor_.RegisterCommandCallBack("lamp", cmdCb);

}

std::string Lamps::LampCommandCallback(const std::unordered_map<std::string,std::string>& data)
{
    std::string response = "Missing arguments. Please insert '-addlamp' to add a new lamp device.\n";
   
    if (data.find("addlamp") != data.end()){
        std::vector<std::string> lamps;
        dataPersistence_.ReadDocument("lamp");
        if (dataPersistence_.GetEntry("lamps", lamps)) {        
            std::string lampsearch = data.at("addlamp");
            for (const auto &lamp : lamps) {
                if (lamp == lampsearch ){                
                    response = "Device already exists. Please use '-updatelamp' to update the device configuration.\n";
                    return response;
                }
            }
            lamps.push_back(lampsearch);
            dataPersistence_.AddOrUpdateEntry("lamps", lamps);
            dataPersistence_.SaveDocument();  

            std::string lampfile = "lamp_device_" + lampsearch;
            dataPersistence_.CreateDocument(lampfile);                     
            if (UpdateLampConfig(lampsearch, data, response)) {                
                dataPersistence_.SaveDocument();                
            }  
        }
    }
    else if (data.find("updatelamp") != data.end()){
        std::string lampsearch = data.at("updatelamp");
        UpdateLampConfig(lampsearch, data, response);        
    }

    return response;
}

bool Lamps::UpdateLampConfig(const std::string& lampName, const std::unordered_map<std::string,std::string>& data, std::string& response)
{
  
    std::string lampfile = "lamp_device_" + lampName;            
    if (!dataPersistence_.ReadDocument(lampfile)) {
        response = "Device not found.";
        return false;
    }

    for (const auto& kv : data) {
        dataPersistence_.AddOrUpdateEntry(kv.first, kv.second);       
    }

    bool retGet;
    std::string lampPort;
    retGet = dataPersistence_.GetEntry("lampport", lampPort);
    if(!retGet || lampPort.empty()){
        response = "Missing '-lampport' configuration.\n";
        return false;
    }

    std::string lampInvert;
    retGet = dataPersistence_.GetEntry("lampinvert", lampInvert);
    if(!retGet || lampInvert.empty()){
        response = "Missing '-lampinvert' configuration.\n";
        return false;
    }

    std::string switchPort;
    retGet = dataPersistence_.GetEntry("switchport", switchPort);
    if(!retGet || switchPort.empty()){
        response = "Missing '-switchport' configuration.\n";
        return false;
    }

    std::string switchInvert;
    retGet = dataPersistence_.GetEntry("switchinvert", switchInvert);
    if(!retGet || switchInvert.empty()){
        response = "Missing '-switchinvert' configuration.\n";
        return false;
    }

    std::string timer;
    retGet = dataPersistence_.GetEntry("timer", timer);
    if(!retGet || timer.empty()){
        dataPersistence_.AddOrUpdateEntry("timer", -1);
    }

    dataPersistence_.SaveDocument();
    response = "Device configuration updated.";
    return true;
}


void Lamps::LampActionCallback(const std::unordered_map<std::string,std::string>& data)
{    
    auto dataIt = data.find("name");
    auto valueIt = data.find("value");
    if(dataIt != data.end() && valueIt != data.end()){
        auto lampDataIt = lamps_.find(dataIt->second);
        if(lampDataIt != lamps_.end()){ 
            lampDataIt->second.brightness = std::stoi(valueIt->second);            
            UpdateLampStatus(lampDataIt->second.lampPort, lampDataIt->second.brightness, lampDataIt->second.lampInvert);
            //SaveLampsToPersistence();
            mainNavigatorForm_.DrawScreenEntities();            
            if ((lampDataIt->second.brightness > 0)  && (lampDataIt->second.timer != 0)){                
                lampDataIt->second.timerCounter = lampDataIt->second.timer; 
            }  
        }
    }
}

std::unordered_map<std::string,std::string>& Lamps::LampUpdateCb(std::string deviceName)
{    
    lampData_.clear();
    auto it = lamps_.find(deviceName);
    if(it != lamps_.end()){        
        lampData_["type"] = "lampControl";
        lampData_["name"] = deviceName;
        lampData_["value"] = std::to_string(it->second.brightness);        
    }
    return lampData_;  
}


void Lamps::UpdateLampStatus(int pin, int value, int lampInvert) 
{    
    pinMode(pin,OUTPUT);
    if(lampInvert)
        digitalWrite(pin, value ? LOW : HIGH);            
    else
        digitalWrite(pin, value ? HIGH : LOW);
}

void Lamps::GetSwitchStatus(int pin, int &value, int switchInvert)
{
    pinMode(pin,INPUT);
    value = digitalRead(pin) * 100;

    if(switchInvert){
        if (value)
            value = 0;
        else
            value = 100;
    }
    else {
        if (value)
            value = 100;
        else
            value = 0;
    }
}

void Lamps::TaskRefresh()
{
    unsigned long currentTime = millis();
    unsigned long interval = 10; // interval milliseconds
    static unsigned long lastTime = 0;
    static int cont1S = 0;    
    if (currentTime - lastTime >= interval) {
        lastTime = currentTime;
        cont1S++;
        if(cont1S >= 10){ //1 second    
            cont1S = 0;
            for(auto &pair : lamps_){
                if(pair.second.timer > 0){
                    if (pair.second.timerCounter > 0)                    { 
                        pair.second.timerCounter--;
                        if(pair.second.timerCounter == 0){
                            pair.second.brightness = 0;
                            pair.second.timerCounter = -1;
                            UpdateLampStatus(pair.second.lampPort, pair.second.brightness, pair.second.lampInvert);
                            mainNavigatorForm_.DrawScreenEntities();                        
                        }
                    }
                }
            }           
        }
        SwitchRefresh();
    }
}

void Lamps::LoadLampsFromPersistence()
{
    std::vector<std::string> lamps;
    dataPersistence_.ReadDocument("lamp");
    if (dataPersistence_.GetEntry("lamps", lamps)) {
        for (const auto &lamp : lamps) {
            lampNameList.push_back(lamp);
            std::string lampfile = "lamp_device_" + lamp;
            dataPersistence_.ReadDocument(lampfile);
            LampData lampData;
            dataPersistence_.GetEntry("lampport", lampData.lampPort);
            dataPersistence_.GetEntry("switchport", lampData.switchPort);
            dataPersistence_.GetEntry("lampinvert", lampData.lampInvert);
            dataPersistence_.GetEntry("switchinvert", lampData.switchInvert);
            dataPersistence_.GetEntry("timer", lampData.timer);
            lampData.brightness = 0;
            lampData.switchStatus = 0;
            lamps_[lamp] = lampData;              
            UpdateLampStatus(lampData.lampPort, lampData.brightness, lampData.lampInvert);
        }
    }
}

void Lamps::SaveLampsToPersistence()
{
    std::vector<std::string> lamps;
    dataPersistence_.ReadDocument("lamp");
    if (dataPersistence_.GetEntry("lamps", lamps)) {
        for (const auto &lamp : lamps) {
            std::string lampfile = "lamp_device_" + lamp;
            dataPersistence_.ReadDocument(lampfile);           
            auto lamps_It = lamps_.find(lamp);
            if(lamps_It != lamps_.end()){   
                dataPersistence_.AddOrUpdateEntry("value", lamps_It->second.brightness);
                dataPersistence_.SaveDocument();
            }   
        }
    }
}

void Lamps::SwitchRefresh()
{
    for (auto &pair : lamps_) {
        int switchValue = 0;
        GetSwitchStatus(pair.second.switchPort, switchValue, pair.second.switchInvert);
        if(switchValue != pair.second.switchStatus){
            pair.second.brightness = switchValue;
            pair.second.switchStatus = switchValue;
            UpdateLampStatus(pair.second.lampPort, switchValue, pair.second.lampInvert);            
            mainNavigatorForm_.DrawScreenEntities();
            if ((pair.second.brightness > 0)  && (pair.second.timer != 0)){                
                pair.second.timerCounter = pair.second.timer; 
            }    
        }
    }
}
