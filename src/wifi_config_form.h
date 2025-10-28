#pragma once

#include "ui.h"
#include "data_persistence.h"

class WifiConfigForm {
public:
    WifiConfigForm(UI &ui, DataPersistence dataPersistence);    
private:
    UI &ui_; 
    DataPersistence dataPersistence_;
    void WifiConfigFormCallback(const std::unordered_map<std::string,std::string>& data, uint8_t num);
};