#include "main_navigator_form.h"

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

void MainNavigatorForm::MainFormCallback(const std::unordered_map<std::string,std::string>& data, uint8_t num)
{    
    if(data.find("action") != data.end()){

        std::string action = data.at("action");
        if(action == "load_form"){
            Serial.println("load_form");
            response_buffer_.clear();
            DrawLampDevices();            
            ui_.SendData("MainForm",response_buffer_, num);
        }
        else if(action == "lamp_action"){ 
            auto it = deviceCallBack.find("lamps");
            if (it != deviceCallBack.end()) {
                it->second(data);
            } 
        }
    }
}

void MainNavigatorForm::DrawLampDevices(){    
    std::vector<std::string> lamps;
    dataPersistence_.ReadDocument("lamp");
    if (dataPersistence_.GetEntry("lamps", lamps)) {
        for (const auto &lamp : lamps) {
            std::string lampfile = "lamp_device_" + lamp;
            dataPersistence_.ReadDocument(lampfile);
            std::string value;
            if(dataPersistence_.GetEntry("value", value)){
                Serial.println(value.c_str());
                response_buffer_.push_back({{"type", "lampControl"},{"value",value},{"name",lamp}});
            }
            else{
                Serial.println("Error reading lamp value");
            }               
        }
    }
}

void MainNavigatorForm::SendFormUpdate()
{    
    ui_.SendData("MainForm",response_buffer_, 0); 
    response_buffer_.clear(); 
}

void MainNavigatorForm::UpdateDevice(const std::string &deviceType, std::unordered_map<std::string,std::string>& data)
{

}