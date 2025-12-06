#include "fs_manager.h"

FsManager::FsManager()
{
    LittleFS.begin();
}

FsManager::~FsManager() {
    LittleFS.end();
}

File FsManager::openFile(String path, const char* mode) 
{
    return LittleFS.open(path, mode);  
}

void FsManager::closeFile(File &file) 
{
    if (file) file.close();      
}

bool FsManager::fileExists(String path) 
{
    return LittleFS.exists(path);
}

bool FsManager::createFile(String path) {
    File file = LittleFS.open(path, "w");   
    if (!file) {        
        return false;
    }
    file.close();   
    return true;
}
