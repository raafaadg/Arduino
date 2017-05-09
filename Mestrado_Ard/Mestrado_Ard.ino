#include <ESP8266WiFi.h>
#include <ArduinoJson.h> 
#include <FS.h>

const char* ssid = "Leutner";
const char* password = "engenharia12";
String buf;

const int AnalogIn  = A0;
int ADread = 0, mod = 0, i = 0;
float EWMA = 0.0;
int Off_set = 350;
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  
  //Abre o sistema de arquivos (mount)
  openFS();
  //Cria o arquivo caso o mesmo não exista
  createFile();
 
  writeFile("Teste Mestrado");
  writeFile("Connecting to " + (String)ssid);
  delay(10);
  
  //WiFi.begin(ssid, password);
  WiFi.softAP("ESP Mestrado");
  
  //while (WiFi.status() != WL_CONNECTED)
 
  // Start the server
  server.begin();
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
//  if (!client) {
//    return;
//  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  if(client.available()){
    delay(1);
  
  
    // Read the first line of the request
    String req = client.readStringUntil('\r');
    Serial.println(req);
    client.flush();
  
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    // Match the request
    int val;
    if (req.indexOf("/gpio/0") != -1){
      
      root["comando"] = "0";
      root["nome"] = "Rafael";
      root["idade"] = "23";
      root["peso"] = "77";
      root["tipo"] = "Humano";
      root["valor"] = "10";
  
      writeResponse(client,root);
      writeFile(root["nome"]);
    }
  
    else if (req.indexOf("/gpio/1") != -1){
      
      root["comando"] = "1";
      root["nome"] = "Aline";
      root["idade"] = "26";
      root["peso"] = "65";
      root["tipo"] = "Humano";
      root["valor"] = "20";
      writeResponse(client,root);
    }
    else if (req.indexOf("/gpio/2") != -1){
        
        root["comando"] = "2";
        root["nome"] = "Theo";
        root["idade"] = "0.5";
        root["peso"] = "15.5";
        root["tipo"] = "Animal";
        root["valor"] = "25"; 
        writeResponse(client,root);
      }
    else if (req.indexOf("/gpio/3") != -1){
   
        root["comando"] = "1";
        root["nome"] = "Fefe";
        root["idade"] = "1";
        root["peso"] = "3.5";
        root["tipo"] = "Animal";
        root["valor"] = "30";
        writeResponse(client,root);
  
      }
      else if (req.indexOf("/gpio/4") != -1){
  
      lerArquivo();   
      client.print(buf);
      client.flush();   
      }
  
    else {
      Serial.println("invalid request");
      client.stop();
      return;
    }
      
    delay(1);
    Serial.println("Client disonnected");
  }
}

void formatFS(void){
  SPIFFS.format();
}
 
void createFile(void){
  File wFile;
 
  //Cria o arquivo se ele não existir
  if(SPIFFS.exists("/log.txt")){
    Serial.println("Arquivo ja existe!");
  } else {
    Serial.println("Criando o arquivo...");
    wFile = SPIFFS.open("/log.txt","w+");
 
    //Verifica a criação do arquivo
    if(!wFile){
      Serial.println("Erro ao criar arquivo!");
    } else {
      Serial.println("Arquivo criado com sucesso!");
    }
  }
  wFile.close();
}
 
void deleteFile(void) {
  //Remove o arquivo
  if(SPIFFS.remove("/log.txt")){
    Serial.println("Erro ao remover arquivo!");
  } else {
    Serial.println("Arquivo removido com sucesso!");
  }
}
 
void writeFile(String msg) {
 
  //Abre o arquivo para adição (append)
  //Inclue sempre a escrita na ultima linha do arquivo
  File rFile = SPIFFS.open("/log.txt","a+");
 
  if(!rFile){
    Serial.println("Erro ao abrir arquivo!");
  } else {
    rFile.println("Log: " + msg);
    Serial.println(msg);
  }
  rFile.close();
}
 
void readFile(void) {
  //Faz a leitura do arquivo
  File rFile = SPIFFS.open("/log.txt","r");
  Serial.println("Reading file...");
  while(rFile.available()) {
    String line = rFile.readStringUntil('\n');
    buf += line;
    buf += "<br />";
  }
  rFile.close();
}
 
void closeFS(void){
  SPIFFS.end();
}
 
void openFS(void){
  //Abre o sistema de arquivos
  if(!SPIFFS.begin()){
    Serial.println("Erro ao abrir o sistema de arquivos");
  } else {
    Serial.println("Sistema de arquivos aberto com sucesso!");
  }
}

void lerArquivo(void){
  buf = "";
 
  buf += "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
  buf += "<h3 style=""text-align: center;"">ESP8266 Web Log</h3>";
  buf += "<p>";
  readFile();
  buf += "</p>";
  buf += "<h4>Criado por Pedro Minatel</h4>";
  buf += "</html>\n";
}

void writeResponse(WiFiClient& client, JsonObject& json) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();

  json.prettyPrintTo(client);
}


