
#include "lamps.h"

Lamps::Lamps(MainNavigatorForm &mainNavigatorForm, DataPersistence &dataPersistence) : mainNavigatorForm_(mainNavigatorForm), dataPersistence_(dataPersistence)
{    
    MainNavigatorForm::DeviceCallback lampCallback = [this](const std::unordered_map<std::string,std::string>& data) {
        this->LampActionCallback(data);
    };
    mainNavigatorForm_.RegiterDeviceCallback("lamps", lampCallback);   
    LoadLampsFromPersistence();
}

void Lamps::LampActionCallback(const std::unordered_map<std::string,std::string>& data)
{    
    auto dataIt = data.find("name");
    auto valueIt = data.find("value");
    if(dataIt != data.end() && valueIt != data.end()){
        auto lampDataIt = lamps_.find(dataIt->second);
        if(lampDataIt != lamps_.end()){ 
            lampDataIt->second.brightness = std::stoi(valueIt->second);            
            UpdateLampStatus(lampDataIt->second.lampPort, lampDataIt->second.brightness);
            SaveLampsToPersistence();
            mainNavigatorForm_.DrawLampDevices();
            mainNavigatorForm_.SendFormUpdate(); 
        }
    }
}

void Lamps::UpdateLampStatus(int pin, int value) 
{    
    pinMode(pin,OUTPUT);
    digitalWrite(pin, value ? HIGH : LOW);    
}

void Lamps::GetSwitchStatus(int pin, int &value)
{
    pinMode(pin,INPUT);
    value = digitalRead(pin) * 100;

}

void Lamps::TaskRefresh()
{
    unsigned long currentTime = millis();
    unsigned long interval = 100; // interval milliseconds
    static unsigned long lastTime = 0;
    if (currentTime - lastTime >= interval) {
        lastTime = currentTime;  
        SwitchRefresh();
    }
}

void Lamps::LoadLampsFromPersistence()
{
    std::vector<std::string> lamps;
    dataPersistence_.ReadDocument("lamp");
    if (dataPersistence_.GetEntry("lamps", lamps)) {
        for (const auto &lamp : lamps) {
            std::string lampfile = "lamp_device_" + lamp;
            dataPersistence_.ReadDocument(lampfile);
            std::string value;
            if(dataPersistence_.GetEntry("value", value)){
                int lampport;
                if(dataPersistence_.GetEntry("lampport", lampport)){
                    int switchport;
                    if(dataPersistence_.GetEntry("switchport", switchport)){
                        LampData lampData;
                        lampData.brightness = std::stoi(value);
                        lampData.lampPort = lampport;
                        lampData.switchPort = switchport;
                        lamps_[lamp] = lampData;
                        UpdateLampStatus(lampport, lampData.brightness);
                    }
                }
            }
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
        GetSwitchStatus(pair.second.switchPort, switchValue);
        if(switchValue != pair.second.switchStatus){
            pair.second.brightness = switchValue;
            pair.second.switchStatus = switchValue;
            UpdateLampStatus(pair.second.lampPort, switchValue);
            SaveLampsToPersistence();
            mainNavigatorForm_.DrawLampDevices();
            mainNavigatorForm_.SendFormUpdate();            
        }
    }
}
