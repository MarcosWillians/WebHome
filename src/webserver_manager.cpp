#include "webserver_manager.h"


WebserverManager::WebserverManager(int Port, FsManager &fsManager, wifiManager &wifiMgr) : wifiMgr_(wifiMgr), fsManager_(fsManager) , server_(Port) 
{
    server_.onNotFound([this]() {
        if (!this->handleFileRead(server_.uri())) {
            server_.send(404, "text/plain", "Arquivo não encontrado");
        }
    });   
        
    if (!LittleFS.begin()) {
        // Initialization failed
        return;
    }
}       

void WebserverManager::TaskRefresh() 
{
    static wifiManager::WiFiStatus lastWifiSts = wifiManager::WiFiStatus::UNKNOWN;
    static bool handleClientEnabled = false;
    static bool serverInitialized = false;

    if (wifiMgr_.ismodeAp_) {
        if(!serverInitialized) {
            serverInitialized = true;
            handleClientEnabled = true;
            server_.begin();
        }        
    }
    else if(lastWifiSts != wifiMgr_.wifiGetConnectStatus()) {
        lastWifiSts = wifiMgr_.wifiGetConnectStatus();
        if(lastWifiSts == wifiManager::WiFiStatus::CONNECTED) {
            Serial.println("WiFi conectado. IP: " + WiFi.localIP().toString());
            if(!serverInitialized) {
                serverInitialized = true;
                server_.begin();
            }
            handleClientEnabled = true;                        
        } else if(lastWifiSts == wifiManager::WiFiStatus::DISCONNECTED) {
            handleClientEnabled = false;
            Serial.println("WiFi desconectado.");
        } else if(lastWifiSts == wifiManager::WiFiStatus::CONNECTING) {
            Serial.println("Conectando ao WiFi...");
        }
    }

    if (handleClientEnabled)
        server_.handleClient();
}   

bool WebserverManager::handleFileRead(String path)
{
    if (path.endsWith("/")) path += "index.html";
    String contentType = getContentType(path);
    File file = fsManager_.openFile(path, "r");
    if (!file)        
        return false;   
    server_.streamFile(file, contentType);
    file.close();
    return true;
}

String  WebserverManager::getContentType(String filename)
{
    if (filename.endsWith(".htm") || filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".png")) return "image/png";
    else if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) return "image/jpeg";
    else if (filename.endsWith(".gif")) return "image/gif";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    else if (filename.endsWith(".svg")) return "image/svg+xml";
    return "text/plain";
}