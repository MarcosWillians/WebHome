#pragma once

#include "fs_manager.h"
#include "wifi_manager.h"

#include <WebServer.h>

class WebserverManager {
public:    
    WebserverManager(int Port, FsManager &fsManager, wifiManager &wifiMgr);    
    void TaskRefresh();
    WebServer server_;
private:
    wifiManager &wifiMgr_;
    FsManager &fsManager_;    
    bool handleFileRead(String path);
    String getContentType(String filename);
};