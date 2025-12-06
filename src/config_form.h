#pragma once

#include "command.h"
#include "ui.h"
#include "data_persistence.h"

class ConfigForm {
public:
    ConfigForm(UI &ui, Command& commandProcessor);    
private:
    UI &ui_; 
    Command& commandProcessor_;    
    void ConfigFormCallback(const std::unordered_map<std::string,std::string>& data, uint8_t num);
    
};