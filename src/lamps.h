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
    };    
    std::unordered_map<std::string, LampData> lamps_;
    MainNavigatorForm &mainNavigatorForm_;
    DataPersistence &dataPersistence_;
    void LoadLampsFromPersistence();
    void SaveLampsToPersistence();    
    void LampActionCallback(const std::unordered_map<std::string,std::string>& data);
    void UpdateLampStatus(int pin, int value);
    void GetSwitchStatus(int pin, int &value);
    void SwitchRefresh();
};