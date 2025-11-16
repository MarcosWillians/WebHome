#include "main_navigator_form.h"
#include <iostream>

MainNavigatorForm::MainNavigatorForm(UI &ui, DataPersistence &dataPersistence) : ui_(ui), dataPersistence_(dataPersistence)
{
    UI::FormInfo MainForm;
    MainForm.FormID = "MainForm";    
    MainForm.callback = [this](const std::unordered_map<std::string,std::string>& data, uint8_t num) {
        this->MainFormCallback(data, num);
    };
    ui_.RegisterForm(MainForm);
}

void MainNavigatorForm::RegiterDeviceCallback(std::string deviceName, DeviceCallback callback)
{
    deviceCallBack[deviceName] = callback;
}

void MainNavigatorForm::RegiterDeviceUpdateCb(std::string deviceName, DeviceUpdateCb callback)
{    
    deviceUpdateCb[deviceName] = callback;
}

void MainNavigatorForm::MainFormCallback(const std::unordered_map<std::string,std::string>& data, uint8_t num)
{    
    num_= num;
    if(data.find("action") != data.end()){
        std::string action = data.at("action");
        if(action == "load_form"){        
            DrawScreenEntities(); 
        }
        else if(action == "lamp_action"){ 
            auto it = deviceCallBack.find("lamps");
            if (it != deviceCallBack.end()) {
                it->second(data);
            } 
        }
    }
}

void MainNavigatorForm::DrawScreenEntities(){    
    for(const auto &it : deviceUpdateCb){
        std::string deviceType = it.first;
        auto itNames = registeredDevicenameList_.find(deviceType);
        if(itNames == registeredDevicenameList_.end()){
            continue;
        }
        for (auto deviceName : itNames->second) {
            response_buffer_.clear();
            std::unordered_map<std::string,std::string> deviceData = it.second(deviceName);
            response_buffer_.push_back(deviceData);
            ui_.SendData("MainForm",response_buffer_, num_);
        }         
    }    
}

void MainNavigatorForm::SendFormUpdate()
{    
    ui_.SendData("MainForm",response_buffer_, 0); 
    response_buffer_.clear(); 
}


void MainNavigatorForm::RegisterDeviceNameList(std::string deviceType, std::vector<std::string> &deviceNames)
{
    registeredDevicenameList_[deviceType] = deviceNames;    
}