#pragma once

#include "ui.h"
#include "data_persistence.h"
#include <functional>

class MainNavigatorForm {
public:
    using DeviceCallback = std::function<void(const std::unordered_map<std::string,std::string>& data)>;
    using DeviceUpdateCb = std::function<std::unordered_map<std::string,std::string>&(std::string deviceName)>;
    MainNavigatorForm(UI &ui, DataPersistence &dataPersistence);
    void RegiterDeviceCallback(std::string deviceName, DeviceCallback callback);   
    void RegiterDeviceUpdateCb(std::string deviceName, DeviceUpdateCb callback); 
    void RegisterDeviceNameList(std::string deviceType, std::vector<std::string> &deviceNames);
    void UpdateDevice(const std::string &deviceType, std::unordered_map<std::string,std::string>& data);

    void DrawScreenEntities(); 
    void SendFormUpdate();
private:        
    DataPersistence &dataPersistence_;
    std::vector<std::unordered_map<std::string,std::string>> response_buffer_;
    
    UI &ui_;
    uint8_t num_;
    std::unordered_map<std::string,DeviceCallback> deviceCallBack;  
    std::unordered_map<std::string,DeviceUpdateCb> deviceUpdateCb;   
    std::unordered_map<std::string,std::vector<std::string>> registeredDevicenameList_;
    void MainFormCallback(const std::unordered_map<std::string,std::string>& data, uint8_t num);   

};