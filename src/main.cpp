#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <LittleFS.h>

#include "command.h"
#include "webserver_manager.h"
#include "websocket_manager.h"
#include "wifi_manager.h"
#include "config_form.h"
#include "ui.h"
#include "data_persistence.h"
#include "main_navigator_form.h"
#include "lamps.h"

Command commandProcessor;
wifiManager wifiMgr;
FsManager fsMgr;
WebserverManager webServerMgr(80, fsMgr, wifiMgr);
WebsocketManager webSocketMgr(81,wifiMgr);
UI userInterface(webSocketMgr);
DataPersistence dataPersistence(fsMgr);  
ConfigForm configForm(userInterface, commandProcessor);
MainNavigatorForm mainNavigatorForm(userInterface, dataPersistence);
Lamps lamps(mainNavigatorForm,dataPersistence,commandProcessor);  

const int ledPin = 2; 

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(115200);
  wifiMgr.wifiConnectAP("Marcos81", "marcos1212e#");  
  userInterface.setInstance(&userInterface);
  webSocketMgr.setEventCallback(userInterface.WebSocketEventCb);

}

void loop() {
  webServerMgr.TaskRefresh();
  webSocketMgr.TaskRefresh();
  lamps.TaskRefresh(); 
}

