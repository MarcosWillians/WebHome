#include "config_form.h"
#include <unordered_map>
#include <Arduino.h>
#include <iostream>
    
ConfigForm::ConfigForm(UI &ui, Command& commandProcessor) : ui_(ui), commandProcessor_(commandProcessor)  
{
    UI::FormInfo CfgForm;
    CfgForm.FormID = "ConfigForm";    
    CfgForm.callback = [this](const std::unordered_map<std::string,std::string>& data, uint8_t num) {
        this->ConfigFormCallback(data, num);
    };
    ui_.RegisterForm(CfgForm);         
}


void ConfigForm::ConfigFormCallback(const std::unordered_map<std::string,std::string>& data, uint8_t num)
{  
    std::vector<std::unordered_map<std::string,std::string>> response;    
    if(data.find("action") != data.end()){               
        std::string action = data.at("action");
        if(action == "command"){        
            std::string cmdInput = data.at("comand");
            std::string response = commandProcessor_.ProcessCommand(cmdInput);
            ui_.SendTxt("ConfigForm", response, 0);
        }
    }
}
