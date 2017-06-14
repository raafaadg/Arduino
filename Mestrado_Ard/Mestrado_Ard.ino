#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h> 
#include <FS.h>

String buf;
String buf2;

const int AnalogIn  = A0;
int ADread = 0, mod = 0, i = 0;
float EWMA = 0.0;
int Off_set = 350;
int kk=1;
ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  //Abre o sistema de arquivos (mount)
  openFS();
  //Cria o arquivo caso o mesmo não exista
  deleteFile();
  createFile();
 
  writeFile("Teste Mestrado");
  delay(10);
  
  WiFi.mode(WIFI_AP);
  //WiFi.begin(ssid, password);
  WiFi.softAP("ESP Mestrado");
   
  // Start the server
  server.on ( "/mestrado/6", []() {
    server.send ( 200, "application/json", "{\"comando\":\"0\",\"nome\":\"Rafael\",\"idade\":23,\"peso\":77,\"valor\":20}" );
  } ); 
  server.on("/mestrado/7",dados);
  server.on("/mestrado/8",lerArquivo);
  server.on("/mestrado/10",capturar);
  server.begin();

//  delay(50000);
//  Serial.print("Desligar WIFI");
//  WiFi.disconnect();   //desconecta a conexão WiFi
// WiFi.mode(WIFI_OFF);   //desabilita o modem WiFi para reduzir o consumo de energia
// WiFi.forceSleepBegin(); //entra no modo Sleep

  
}

void loop() {
  server.handleClient();
  if(kk==1)
    for (int k=0;k<10;k++)
      EMG();
    ++kk;
    if(kk==1000){
    
//      WiFi.forceSleepEnd();
//      WiFi.mode(WIFI_AP);
//      WiFi.softAP("ESP Mestrado");
    }
  
}

void capturar(){}
void dados(){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["comando"] = "0";
  root["nome"] = "Rafael";
  root["idade"] = "23";
  root["peso"] = "77";
  root["tipo"] = "Humano";
  JsonArray& valor = root.createNestedArray("valor");
  File rFile = SPIFFS.open("/log.txt","r");
  Serial.println("Reading file...");
  while(rFile.available()) {
    String line = rFile.readStringUntil('\n');
    valor.add(line);
  }

  String jsonout;
  root.printTo(jsonout);
  Serial.print(jsonout);
  server.send(200, "application/json", jsonout);
  
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
  if(SPIFFS.remove("/log.txt")==false){
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
    rFile.println(msg);
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
    buf2 += line;
    buf2 += "\n";
  }
  rFile.close();
  Serial.print(buf2);

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
  buf += "<h3 style=""text-align: center;"">ESP8266 Valores Resgistrados pelo analogRead</h3>";
  buf += "<p>";
  readFile();
  buf += "</p>";
  buf += "</html>\n";
  server.send(200, "text/html", buf); 
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
  Serial.println(EWMA);  //imprime o valor da EWMA
  writeFile(String(EWMA));
}

