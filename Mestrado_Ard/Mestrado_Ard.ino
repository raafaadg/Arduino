#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h> 
#include <FS.h>
#include "user_interface.h"
#include <math.h>

#define ssid      "TELECOM_LAMMI"        // WiFi SSID
#define password  "schottky"    // WiFi password
#define SPIFFS_CFG_PHYS_SZ(ignore)        (1024*1024*2)
#define SPIFFS_CFG_PHYS_ERASE_SZ(ignore)  (65536)
#define SPIFFS_CFG_PHYS_ADDR(ignore)      (0)
#define SPIFFS_CFG_LOG_PAGE_SZ(ignore)    (256)
#define SPIFFS_CFG_LOG_BLOCK_SZ(ignore)   (65536)

static os_timer_t mTimer;
bool cap = false, _timeout = false;
String buf,buf2;
const int AnalogIn  = A0;
const int WakeUp = 5;
int ADread = 0, mod = 0, i = 0, contr = 2, count = 0;
float EWMA = 0.0;
int Off_set = 495;
long startTime, recordTime ;
FSInfo fs_info;
File file;
ESP8266WebServer server(80);
DynamicJsonBuffer jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
//JsonArray& valor = root.createNestedArray("valor");

void tCallback(void *tCall){
    _timeout = true;
}
void usrInit(void){
    os_timer_setfn(&mTimer, tCallback, NULL);
    os_timer_arm(&mTimer, 100, true);
}

void setup() {
  Serial.begin(115200);
  pinMode(WakeUp, INPUT);
  //Abre o sistema de arquivos (mount)
  openFS();
  usrInit();
  delay(10);
  
  WiFi.mode(WIFI_AP);
  //WiFi.begin(ssid, password);
  WiFi.softAP("ESP Brux Mestrado");
   //delay(100);
  // Start the server

  server.on ( "/mestrado/6", []() {
    server.send ( 200, "application/json", "{\n \"time\": \"02:41:48 PM\",\n \"milliseconds_since_epoch\": 1525876908085,\n \"date\": \"05-09-2018\"\n}" );
  } ); 
  server.on("/mestrado/inf",infos);
  server.on("/mestrado/del",deleteFile);
  server.on("/mestrado/data",dados);
  server.on("/mestrado/create",createFile);
  server.on("/mestrado/dir",diretorio);
  server.on("/mestrado/read",lerArquivo);
  server.on("/mestrado/go",capturar);
  server.on("/mestrado/json2",json2);
  server.on("/mestrado/json3",json3);
  server.begin();

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
  /*if(!cap){
    EMG();
    delay(50);
    }*/
  
  if(cap){
      EMG();
      if (_timeout){
        if(EWMA > 0){
          writeFile(String(round(EWMA)));
        }
        else{
          writeFile("0");
          }
        _timeout = false;
      }
      yield();
  }
    if(!digitalRead(WakeUp)&& contr==1){
      //Serial.println("Ligar WIFI");
      WiFi.forceSleepWake();
      WiFi.mode(WIFI_AP);
      delay(100);
      cap = false;
      contr = 2;
      file.close();
    }  
    /*if(!digitalRead(WakeUp)){
      cap = false;
      delay(50);
      }*/
    
}

void diretorio(void){
  server.send(200, "text/html", "Diretórios existentes");
  Dir dirr = SPIFFS.openDir("/");
  while(dirr.next()){
    Serial.printf("%s - %u bytes\n", dirr.fileName().c_str(), dirr.fileSize());
    }
  }

void infos(void){
    File rFile = SPIFFS.open("/log.txt","r");
    buf = "";
    buf += "<h3 style=""text-align: center;"">Enviando informações sobre o ESP e arquivo</h3>";
    buf += "<p>";
    buf += "<h2 style=""text-align: center;"">Tamanho do Arquivo</h3>";
    buf += rFile.size();
    buf += "</p>";
    buf += "</html>\n";
    rFile.close();
  }
void capturar(){
  buf = "";
  buf += "<h3 style=""text-align: center;"">Enviado comando de captura</h3>";
  buf += "<p>";
  buf += "</p>";
  buf += "</html>\n";
  server.send(200, "text/html", buf);
  //Serial.println("Enviado comando de captura");
  cap = true;
  contr=1;
  //Serial.println("Desligar WIFI");
   //WiFi.disconnect();   //desconecta a conexão WiFi
   WiFi.mode(WIFI_OFF);   //desabilita o modem WiFi para reduzir o consumo de energia
   WiFi.forceSleepBegin(); //entra no modo Sleep
   delay(100);
   file = SPIFFS.open("/log.txt","a");
   //startTime = millis();
  }
void dados(){
  server.send(200, "text/html", "Criando buffer json");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  /*root["comando"] = "0";
  root["nome"] = "Rafael";
  root["idade"] = "23";
  root["peso"] = "77";
  root["tipo"] = "Humano";*/
  JsonArray& valor = root.createNestedArray("valor");
  JsonArray& valor2 = root.createNestedArray("valor2");
  
  File rFile = SPIFFS.open("/log.txt","r");
  Serial.println("Reading file...");
  int i = 0;
  while(rFile.available()) {
    String line = rFile.readStringUntil(',');
    //Serial.println(line);
    valor.add(line);
    valor2.add(String(i));
    Serial.println(i);
    i = i + 1;
  }
}

void json2(void){

  String jsonout;
  root.printTo(jsonout);
  server.send(200, "application/json", jsonout);
  //for(int k = 0; k < valor.size(); k++)
  //  valor.remove(k);
    
//  root.remove("valor");
//  JsonArray& valor = root.createNestedArray("valor"); 
}
void json3(void){
  EMG();
  //root["valor"] = random(30,60);
  String jsonout;
  root.printTo(jsonout);
  server.send(200, "application/json", jsonout);
  //valor.remove(0);
}

void formatFS(void){
  SPIFFS.format();
}
 
void createFile(void){
  File wFile;
 
  //Cria o arquivo se ele não existir
  if(SPIFFS.exists("/log.txt")){
    server.send(200, "text/html", "Arquivo ja existe!");
    //Serial.println("Arquivo ja existe!");
  } else {
    server.send(200, "text/html", "Criando o arquivo...");
    //Serial.println("Criando o arquivo...");
    wFile = SPIFFS.open("/log.txt","w+");
 
    //Verifica a criação do arquivo
    if(!wFile){
      server.send(200, "text/html", "Erro ao criar arquivo!");
      //Serial.println("Erro ao criar arquivo!");
    } else {
      server.send(200, "text/html", "Arquivo criado com sucesso!");
      //Serial.println("Arquivo criado com sucesso!");
    }
  }
  wFile.close();
}
 
void deleteFile(void) {
  //Remove o arquivo
  
  if(SPIFFS.remove("/log.txt")==false){
    server.send(200, "text/html", "Erro ao remover arquivo!");
    //Serial.println("Erro ao remover arquivo!");
  } else {
    server.send(200, "text/html", "Arquivo removido com sucesso!");
    //Serial.println("Arquivo removido com sucesso!");
  }
  //Cria o arquivo caso o mesmo não exista
  createFile();
  writeFile("Teste Mestrado");
}
 
void writeFile(String msg) {
 
  //Abre o arquivo para adição (append)
  //Inclue sempre a escrita na ultima linha do arquivo
  //File rFile = SPIFFS.open("/log.txt","a+");
 
  if(!file){
    //Serial.println("Erro ao abrir arquivo!");
  } else {
    file.print(msg);
    file.print(',');
    //Serial.println(msg);
  }
  //rFile.close();
}
 
void readFile(void) {
  //Faz a leitura do arquivo
  File rFile = SPIFFS.open("/log.txt","r");
  //Serial.println("Reading file...");
  count = 0;
  while(rFile.available()) {
    String line = rFile.readStringUntil(',');
    Serial.println(line);
//    buf += line;
//    buf += ",";
//    //buf += "<br />";
//    buf2 += line;
//    buf2 += "\n";
//    count = count + 1;
//    delay(1);
    /*if(count == 500){
      serial
      }*/
  }
    //buf += "<br />";
  //Serial.println("Acabou a leitura");
  rFile.close();
  //Serial.print(buf2);

}
 
void closeFS(void){
  SPIFFS.end();
}
 
void openFS(void){
  //Abre o sistema de arquivos
  if(!SPIFFS.begin()){
    //Serial.println("Erro ao abrir o sistema de arquivos");
  } else {
    //Serial.println("Sistema de arquivos aberto com sucesso!");
  }
}

void lerArquivo(void){
  Serial.println("Leitura Inicializada");
  server.send(200, "text/html", "Leitura Inicializada"); 
//  buf = "";
//  buf += "<h3 style=""text-align: center;"">ESP8266 Valores Resgistrados pelo analogRead</h3>";
//  buf += "<p>";
  readFile();
//  buf += "</p>";
//  buf += "</html>\n";
//  server.send(200, "text/html", buf); 
  Serial.println("Leitura Finalizada");
}

void writeResponse(WiFiClient& client, JsonObject& json) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();
  json.prettyPrintTo(client);
}

void EMG(void){

  for (i = 0; i < 2200; i++){
  ADread = analogRead(AnalogIn)-Off_set;  //efetua a leitura do AD e subtrai do seu nivel de off-set
  mod = abs (ADread);  //calcula o módulo da leitura do AD
  EWMA = mod*0.0001+EWMA*0.9999;  // calcula a média movel exponencial para 10000 amostras
  }
  Serial.println((EWMA));  //imprime o valor da EWMA
  root["valor"] = round(EWMA);
  //valor.add(EWMA);
  //writeFile(String(EWMA));
  /*if(valor.size())
    for(int k = 0; k < valor.size(); k++)
      valor.remove(k);*/
}

