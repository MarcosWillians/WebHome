#pragma once

#include "ui.h"
#include "data_persistence.h"
#include <functional>

class MainNavigatorForm {
public:
    using DeviceCallback = std::function<void(const std::unordered_map<std::string,std::string>& data)>;
    MainNavigatorForm(UI &ui, DataPersistence &dataPersistence);
    void RegiterDeviceCallback(std::string deviceName, DeviceCallback callback);   
    void UpdateDevice(const std::string &deviceType, std::unordered_map<std::string,std::string>& data);

    void DrawLampDevices(); 
    void SendFormUpdate();
private:        
    DataPersistence &dataPersistence_;
    std::vector<std::unordered_map<std::string,std::string>> response_buffer_;
    UI &ui_;

    std::unordered_map<std::string,DeviceCallback> deviceCallBack;    
    void MainFormCallback(const std::unordered_map<std::string,std::string>& data, uint8_t num);   

};