#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <LittleFS.h>

#include "webserver_manager.h"
#include "websocket_manager.h"
#include "wifi_manager.h"
#include "wifi_config_form.h"
#include "ui.h"
#include "data_persistence.h"
#include "main_navigator_form.h"
#include "lamps.h"

// Projeto: Sistema de automação residencial baseado em webservers com websokets
// 
// Interface com o usuário via página web hospedada no sistema de arquivos LittleFS
// -O usuario deve apertar um botão na placa para colocar a placa em modo de programação
// -O modo de programação da placa fica ativo por 2 minutos
// -O usuário conecta seu celular ou computador na rede Wi-Fi criada pela placa
// -O usuário acessa a página web hospedada na placa e faz a configuração 
// 
// Descrição da tela de configuração:
// Pagina web operada via WebSockets acessivel por uma rede Wi-Fi criada pela própria placa com um IP determinado
// -O usuario deve definir o nome da rede Wi-Fi (SSID) e a senha
// -O usuario deve definir o nome do ambiente ao qual a placa controla, exemplo: "Sala", "Quarto", etc
// -O usuário deve definir também um IP fixo para a placa. O IP fixo é necessario para que o menu de outras
// placas possam se comunicar com essa placa
// -Deve selecionar os recursos da placa que deseja ativar (exemplo: controle de lâmpadas, sensores, etc)
// -A ativação do recurso se dá pela sua nomeação, o qual ficará disponível na webpage depois
// -Se o recurso for uma lampada, por exemplo, depois aparecerá na tela principal uma lampada que pode ligar ou desligar
// -O usuário pode adicionar os nomes e ips de outros ambientes, e os seus respectivos endereços IP
// -Os endereços IPs de outras placas serão utillizados para acessar o botão "Outros ambientes" em qualquer placa
// -A pagina de configuração deve ter um botão "Salvar" que salva todas as configurações no sistema de arquivos LittleFS
// -Depois de salvar, a placa deve reiniciar e tentar conectar na rede Wi-Fi configurada
//
// Descrição da tela principal:
// -A tela principal deve mostrar o nome do ambiente (definido na configuração)
// -A tela principal deve mostrar o status dos recursos ativados na configuração (exemplo: lâmpadas ligadas ou desligadas)
// -A tela principal deve ter um botão "Outros ambientes" que abre uma nova página
//
// Implementação técnica:
// -Uma classe para gerenciar o sistema de arquivos LittleFS
// -Uma classe para gerenciar a conexão Wi-Fi
// -Uma classe para gerenciar o servidor web
// -Uma classe para gerenciar o servidor WebSocket
// -Uma classe para gerenciar os recursos (lâmpadas, sensores, etc)


// Webpages
// -A página principal (index.html) 
// -A página de configuração (config.html)
//    - Deve ter um campo para o nome da rede Wi-Fi (SSID) e a senha
//    - A página de configuração deve ter um campo para definir o IP fixo da placa  
//    - A pagina de configuração terá um campo para o nome da página corrente (exemplo: "Sala", "Cozinha", etc)
//    - Deve ter um campo para adicionar o nome do recurso (exemplo: "Lâmpada", "Sensor", "Sala" etc) e o tipo do recurso (exemplo: "lampada", "sensor", "link pra outra pagina", etc)
//              - Se o recurso for uma lâmpada, sensor, (mas não for um link para outra página) o campo de link será automaticamante preenchido com o endereço do tipo do recurso + ex: ipdaplacaatual + /lamp.html )
//    - Se o recurso for um link para outra página, deve ter um campo para o endereço IP da outra placa + "/" + o nome da página (exemplo: "192.168.0.120/sala.html")       
//    - Deve ter uma lista dos recursos adicionados
//    - Deve ter um botão "Adicionar recurso" que adiciona o recurso na lista de recursos
//          -O botão "Adicionar recurso" deve enviar um comando config-add_resource via WebSocket para a placa
//          -O comando a placa deve responder com o comando enviando a lista atualizada de recursos via WebSocket
//    - Deve ter um botão "Remover recurso" que apaga o ultimo recurso da lista de recursos
//         -O botão "Remover recurso" deve enviar um comando config-remove_resource via WebSocket para a placa
//         -O comando a placa deve responder com o comando enviando a lista atualizada de recursos via WebSocket
//    - Deve ter um botão "Salvar" que salva todas as configurações no sistema de arquivos LittleFS
//         -O botão "Salvar" deve enviar um comando config-save via WebSocket para a placa
//         -O comando a placa deve salvar as configurações no sistema de arquivos LittleFS
//         -O comando "Salvar" deve enviar todos os dados de configuração via WebSocket para a placa (SSID, senha, IP fixo, nome do ambiente, lista de recursos, etc)
//         -O comando a placa deve responder com o comando config-saved via WebSocket
 

// Especiificação de implemetacao
// -O webserver dará conta de prover a unica pagina index.html e os arquivos estaticos (css, js, imagens)
// -O websocket proverá toda a comunicação dinâmica entre a página web e a placa
// -Vamos ter um modulo chamado UserInterface usará o Websocket para comunicar com o usuario enviando e recebendo jsons
// -O modulo UserInterface deverá possuir métodos para que outros objetos abaixo dele possam se inscrever para estabelecer a sua interface com o usuario
//    -Deve haver metodos para a criação de telas com vários objetos como (menus, botões, listas, etc)
//    -Deve haver metodos para que outros objetos (devices) possam ser registrados no UserInterface para receber ou enviar dados
//    -Esse modulo é responsavel por fazer o parser do json e a construção das mensagens que construirão remotamente as telas dos usuarios
// -Vamos ter um modulo chamado UserInterfaceConfig que criará todas configurações para o modulo UserInterface.
//    -criação de telas para as aplicações (ex: configuração de rede, configuração/controle de dispositivos etc)
//    -Registro de funções das aplicações

wifiManager wifiMgr;
FsManager fsMgr;
WebserverManager webServerMgr(80, fsMgr, wifiMgr);
WebsocketManager webSocketMgr(81,wifiMgr);
UI userInterface(webSocketMgr);
//ConfigForm configForm(userInterface);
DataPersistence dataPersistence(fsMgr);
WifiConfigForm wificonfigForm(userInterface,dataPersistence);
MainNavigatorForm mainNavigatorForm(userInterface, dataPersistence);
Lamps lamps(mainNavigatorForm,dataPersistence);  

const int ledPin = 2; // Use o pino 2 (LED_BUILTIN) como exemplo

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String msg = String((char*)payload);
    if (msg == "ligar") {
      digitalWrite(ledPin, HIGH);
      webSocketMgr.sendTXT(num, "Lâmpada ligada!");
    } else if (msg == "desligar") {
      digitalWrite(ledPin, LOW);
      webSocketMgr.sendTXT(num, "Lâmpada desligada!");
    } else if (msg == "get_buttons") {
      webSocketMgr.sendTXT(num, "[[\"lighton.png\",\"Sala\",\"\"],[\"lightoff.png\",\"Cozinha\",\"\"],[\"link.png\",\"Google\",\"http://www.google.com\"]]");            
    }
    else {
      webSocketMgr.sendTXT(num, "Comando desconhecido: ") ;
    }
  }
}



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

