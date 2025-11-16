#pragma once

#include "main_navigator_form.h"
#include "data_persistence.h"

class Lamps {
public:
    Lamps(MainNavigatorForm &mainNavigatorForm, DataPersistence &dataPersistence);  
    void TaskRefresh();
    
private:
    struct LampData {       
        int brightness;
        int lampPort;
        int switchPort;
        int switchStatus;
        int lampInvert;
        int switchInvert; 
        int timer;
        int timerCounter;          
    };    
    std::unordered_map<std::string, LampData> lamps_;
    MainNavigatorForm &mainNavigatorForm_;
    DataPersistence &dataPersistence_;
    std::unordered_map<std::string,std::string> lampData_;
    std::vector<std::string> lampNameList;
    void LoadLampsFromPersistence();
    void SaveLampsToPersistence();    
    void LampActionCallback(const std::unordered_map<std::string,std::string>& data);
    std::unordered_map<std::string,std::string>& LampUpdateCb(std::string deviceName);
    void UpdateLampStatus(int pin, int value, int lampInvert); 
    void GetSwitchStatus(int pin, int &value, int switchInvert);
    void SwitchRefresh();
};