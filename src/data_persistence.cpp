#include "data_persistence.h"
#include <string>     
#include <cstdlib>     // std::stof

DataPersistence::DataPersistence(FsManager &fsManager) : fsManager_(fsManager)
{
}
    
void DataPersistence::ReadDocument(const std::string &document)
{
    filepath_ = "/data/" + String(document.c_str()) + ".txt";
    File file = fsManager_.openFile(filepath_, "r+");
    if (!file) {       
        file = fsManager_.openFile(filepath_, "w+"); 
        return;
    }
    dataMap_.clear();
    std::string currentKey;    
    while (file.available()) {
        String linha = file.readStringUntil('\n');
        linha.trim(); // Remove espaços e quebras de linha no início/fim
        std::string linha_std = linha.c_str(); // Converte para std::string
        if (!currentKey.empty()) {
            dataMap_[currentKey] = linha_std;
            currentKey.clear();
        }
        else {
            currentKey = linha_std; 
        }
    } 
    fsManager_.closeFile(file);
}

void DataPersistence::AddOrUpdateEntry(const std::string &key, const std::string &value)
{
    dataMap_[key] = value;
}

void DataPersistence::AddOrUpdateEntry(const std::string &key, int value)
{
    dataMap_[key] = std::to_string(value);
}

void DataPersistence::AddOrUpdateEntry(const std::string &key, float value)
{
    dataMap_[key] = std::to_string(value);
}

void  DataPersistence::AddOrUpdateEntry(const std::string &key, std::vector<std::string> &value)
{
    std::string combined;
    for (const auto &val : value) {
        if (!combined.empty()) {
            combined += ","; 
        }
        combined += val;
    }
    dataMap_[key] = combined;
}

bool DataPersistence::GetEntry(const std::string &key, std::string &value) const
{
    auto it = dataMap_.find(key);
    if(it != dataMap_.end()) {
        value = it->second;
        return true;
    }
    return false;
}    

bool DataPersistence::GetEntry(const std::string &key, int &value) const
{
    std::string strValue;
    if (GetEntry(key, strValue)) {
        try {
            value = std::stoi(strValue);
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}

bool DataPersistence::GetEntry(const std::string &key, float &value) const
{
    std::string strValue;
    if (GetEntry(key, strValue)) {
        try {
            value = std::stof(strValue);
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}

bool DataPersistence::GetEntry(const std::string &key, std::vector<std::string> &value) const
{
    std::string strValue;
    if (GetEntry(key, strValue)) {
        value.clear();
        size_t start = 0;
        size_t end = strValue.find(',');
        while (end != std::string::npos) {
            value.push_back(strValue.substr(start, end - start));
            start = end + 1;
            end = strValue.find(',', start);
        }
        value.push_back(strValue.substr(start));
        return true;
    }
    return false;
}
    
void DataPersistence::DeleteEntry(const std::string &key)
{
    dataMap_.erase(key);
}
    
void DataPersistence::DeleteAllEntries()
{
    dataMap_.clear();   
}
    
void DataPersistence::SaveDocument()
{   
    if(filepath_ == "") {
        return; // Caminho do arquivo não definido
    }    
    File file = fsManager_.openFile(filepath_, "w");   
    if (!file) {
        return;
    }
    for (const auto &entry : dataMap_) {
        file.println(entry.first.c_str());
        file.println(entry.second.c_str());
    }
    file.flush();
    fsManager_.closeFile(file); 
}