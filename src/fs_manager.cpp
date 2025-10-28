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