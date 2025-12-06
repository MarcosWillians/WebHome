
#pragma once

#include "fs_manager.h"
#include <string>
#include <unordered_map>  
#include <vector>  

class DataPersistence {
public:       
    DataPersistence(FsManager &fsManager);
    bool ReadDocument(const std::string &document);
    void AddOrUpdateEntry(const std::string &key, const std::string &value);
    void AddOrUpdateEntry(const std::string &key, int value);
    void AddOrUpdateEntry(const std::string &key, float value);
    void AddOrUpdateEntry(const std::string &key, std::vector<std::string> &value);
    bool GetEntry(const std::string &key, std::string &value) const;
    bool GetEntry(const std::string &key, int &value) const;
    bool GetEntry(const std::string &key, float &value) const;    
    bool GetEntry(const std::string &key, std::vector<std::string> &value) const;
    
    void DeleteEntry(const std::string &key);
    void DeleteAllEntries();
    void SaveDocument();
    void CreateDocument(const std::string &document);
private:
    FsManager &fsManager_;
    String filepath_; 
    std::string document_;
    std::unordered_map<std::string, std::string> dataMap_;           
};