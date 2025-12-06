
#include "command.h"

#include <iostream>
#include <sstream>
#include <string>

void Command::RegisterCommandCallBack(std::string command, CommandCallback cb)
{
    callbacks_[command] = cb;
}

std::string Command::ParseCommand(const std::string& command)
{    
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd; 
    command_.clear();    
    std::string token;
    while (iss >> token) {
        if (token.rfind("-", 0) == 0) { // começa com '-'
            std::string argName = token.substr(1); // remove o '-'
            std::string value;
            
            if (iss.peek() != EOF) {
                std::streampos pos = iss.tellg();
                iss >> value;
                if (value.rfind("-", 0) == 0) {                    
                    iss.seekg(pos); 
                    value = "";
                }
            }
            command_[argName] = value;
        }
    }
    return cmd;
}

std::string Command::ProcessCommand(std::string& command)
{  
    std::string cmdRet;
    auto cmd = ParseCommand(command);
    auto it = callbacks_.find(cmd);
    if (it != callbacks_.end()){
        cmdRet = it->second(command_);
    }
    return cmdRet;
}
