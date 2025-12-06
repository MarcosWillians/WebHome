
#pragma once

#include "ui.h"
#include "data_persistence.h"
#include <unordered_map> 

class Command {
public:

    using CommandCallback = std::function<std::string(const std::unordered_map<std::string,std::string>& data)>;
    void RegisterCommandCallBack(std::string command, CommandCallback cb);
    std::string ProcessCommand(std::string& command);
private:
    std::unordered_map<std::string,std::string> command_;
    std::unordered_map<std::string,CommandCallback> callbacks_;
    std::string ParseCommand(const std::string& command);

};