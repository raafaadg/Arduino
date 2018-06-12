#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h> 
#include <FS.h>
#include "user_interface.h"
#include <math.h>
//#include <Thread.h>
//#include <ThreadController.h>

#define ssid      "TELECOM_LAMMI"        // WiFi SSID
#define password  "schottky"    // WiFi password
#define SPIFFS_CFG_PHYS_SZ(ignore)        (1024*1024*2)
#define SPIFFS_CFG_PHYS_ERASE_SZ(ignore)  (65536)
#define SPIFFS_CFG_PHYS_ADDR(ignore)      (0)
#define SPIFFS_CFG_LOG_PAGE_SZ(ignore)    (256)
#define SPIFFS_CFG_LOG_BLOCK_SZ(ignore)   (65536)

static os_timer_t mTimer;
//static os_timer_t2 mTimer;
bool cap = false, _timeout = false;
String buf,buf2,jsonout;
const int AnalogIn  = A0;
//const int WakeUp = 5;
const int WakeUp = D1;
int ADread = 0, mod = 0, i = 0, contr = 2, count = 0;
float EWMA = 0.0;
int Off_set = 495;
long startTime, recordTime ;
FSInfo fs_info;
File file;
ESP8266WebServer server(80);

DynamicJsonDocument jsonBuffer;
JsonObject& root = jsonBuffer.to<JsonObject>();
JsonArray& valor = root.createNestedArray("valor");

void returnOK() {
  server.send(200, "text/plain", "");
}

void tCallback(void *tCall){
    _timeout = true;
}
void usrInit(void){
    os_timer_setfn(&mTimer, tCallback, NULL);
    os_timer_arm(&mTimer, 100, true);
}

//ThreadController controll = ThreadController();
//Thread myThread = Thread();

// callback for myThread
//void niceCallback(){
//    server.handleClient();
//}

//void timerCallback(){
//  controll.run();
//}

void setup() {
  Serial.begin(115200);
  
  //myThread.onRun(niceCallback);
  
  pinMode(WakeUp, INPUT);
  //Abre o sistema de arquivos (mount)
  openFS();
  usrInit();
  delay(10);
  
  WiFi.mode(WIFI_AP);
  //WiFi.begin(ssid, password);
//  WiFi.softAP("ESP Brux Mestrado");
  WiFi.softAP("ESP Node Mestrado");
   //delay(100);
  // Start the server
  MDNS.begin("bruxismo");
  server.on ( "/mestrado/6", []() {
    server.send ( 200, "application/json", "{\n \"time\": \"02:41:48 PM\",\n \"milliseconds_since_epoch\": 1525876908085,\n \"date\": \"05-09-2018\"\n}" );
  } ); 
  server.on("/mestrado/inf",infos);
  server.on("/mestrado/del",deleteFile);
  server.on("/mestrado/create",createFile);
  server.on("/mestrado/dir",diretorio);
  server.on("/mestrado/read",lerArquivo);
  server.on("/mestrado/go",capturar);
  server.on("/mestrado/json3",json3);
  server.on("/edit", handleFileRead);
  server.onNotFound(handleNotFound);
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

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void loop() {
  server.handleClient();
//  myThread.run();
  ArduinoOTA.handle();
  /*if(!cap){
    EMG();
    delay(50);
    }*/
  
  if(cap){
      EMG(2200);
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
    else{
    if(!digitalRead(WakeUp)){
      EMG(500);
      delay(80);
      }
    }
}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

void handleFileRead(void) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: ");
    File file = SPIFFS.open("/log.txt", "r"); 
    // Open the file
    size_t sent = server.streamFile(file, "text/html");    // Send it to the client
    file.close();                                          // Close the file again

}

void diretorio(void){
  char dir[400];
  Dir dirr = SPIFFS.openDir("/");
  while(dirr.next()){
    Serial.printf("%s - %u bytes\n", dirr.fileName().c_str(), dirr.fileSize());
    }
    /*snprintf ( dir, 400,

"<html>\
  <head>\
    <meta/>\
    <title>ESP8266 Diretorio</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Tamanho do Arquivo de texto</h1>\
    <p>%s: %u %s</p>\
  </body>\
</html>",

    dirr.fileName().c_str(), dirr.fileSize(), "bytes"
  );*/
  
  buf="";
  buf2="";
  buf+= "<html>";
  buf+= "<head>";
  buf+= "<meta/>";
  buf+= "<title>ESP8266 Diretorio</title>";
  buf+= "<style>";
  buf+= "body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }";
  buf+= "</style>";
  buf+= "</head>";
  buf+= "<body>";
  buf+= "<h1>Tamanho do Arquivo de texto</h1>";
  //buf2 = sprintf("<p>%s: %u %s</p>",dirr.fileName().c_str(),dirr.fileSize(),"bytes");
  //buf+= buf2;
  //buf+= "<p>"+dirr.fileName().c_str()+": "+String(dirr.fileSize())+" bytes</p>";
  buf+= "</body>";
  buf+= "</html>";
  
  server.send ( 200, "text/html", buf );
  }

void infos(void){
    File rFile = SPIFFS.open("/log.txt","r");
    buf = "";
    //buf += "<html>";
    buf += "<h3 style=""text-align: center;"">Enviando informações sobre o ESP e arquivo</h3>";
    buf += "<p>";
    buf += "<h2 style=""text-align: center;"">Tamanho do Arquivo</h3>";
    buf += rFile.size();
    buf += "</p>";
    buf += "</html>";
    server.send ( 200, "text/html", buf );
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

void json3(void){
  //EMG();
  //root["valor"] = random(30,60);
  jsonout = "";
  serializeJson(root,jsonout);
  //serializeJson(root,Serial);
  //Serial.println("");
  server.send(200, "application/json", jsonout);
  jsonBuffer.clear();
  JsonObject& root = jsonBuffer.to<JsonObject>();
  JsonArray& valor = root.createNestedArray("valor");
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
  serializeJsonPretty(json,client);
}

void EMG(int control){
  for (i = 0; i < control; i++){
    ADread = analogRead(AnalogIn)-Off_set;  //efetua a leitura do AD e subtrai do seu nivel de off-set
    mod = abs (ADread);  //calcula o módulo da leitura do AD
    EWMA = mod*0.0001+EWMA*0.9999;  // calcula a média movel exponencial para 10000 amostras
    
  }
//  Serial.println((EWMA));  //imprime o valor da EWMA
  valor.add(round(EWMA));
//  Serial.println(analogRead(AnalogIn));  //imprime o valor da EWMA
//  valor.add(analogRead(AnalogIn));
 
  //writeFile(String(round(EWMA)));
  if(valor.size() > 99){
    jsonBuffer.clear();     
    JsonObject& root = jsonBuffer.to<JsonObject>();
    JsonArray& valor = root.createNestedArray("valor");
  }
}

