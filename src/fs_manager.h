# pragma once

#include <LittleFS.h>

class FsManager {
public:   
    FsManager();
    ~FsManager();
    File openFile(String path, const char* mode);
    void closeFile(File &file);
    bool fileExists(String path);    
};