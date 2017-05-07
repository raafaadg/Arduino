/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the GPIO2 low,
 *    http://server_ip/gpio/1 will set the GPIO2 high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

const char* ssid = "Leutner";
const char* password = "engenharia12";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void people(){

  
}
void setup() {
  Serial.begin(115200);
  delay(10);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  if (req.indexOf("/gpio/0") != -1){
    client.flush();
    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["comando"] = "0";
    root["nome"] = "Rafael";
    root["idade"] = "23";
    root["peso"] = "77";
    root["tipo"] = "Humano";
    root["valor"] = "10";

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json;charset=utf-8");
    client.println("Server: 192.168.100.8");
    client.println("Connection: close");
    client.println();
    root.printTo(client);
    client.println();
  }

  else if (req.indexOf("/gpio/1") != -1){
    
    client.flush();
    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["comando"] = "1";
    root["nome"] = "Aline";
    root["idade"] = "26";
    root["peso"] = "65";
    root["tipo"] = "Humano";
    root["valor"] = "20";
    
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json;charset=utf-8");
    client.println("Server: 192.168.100.8");
    client.println("Connection: close");
    client.println();
    root.printTo(client);
    client.println();
  }
  else if (req.indexOf("/gpio/2") != -1){
      
      client.flush();
      
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();
      root["comando"] = "2";
      root["nome"] = "Theo";
      root["idade"] = "0.5";
      root["peso"] = "15.5";
      root["tipo"] = "Animal";
      root["valor"] = "25"; 
       
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json;charset=utf-8");
      client.println("Server: 192.168.100.8");
      client.println("Connection: close");
      client.println();
      root.printTo(client);
      client.println();
    }
  else if (req.indexOf("/gpio/3") != -1){
      
      client.flush();
      
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();
      root["comando"] = "1";
      root["nome"] = "Fefe";
      root["idade"] = "1";
      root["peso"] = "3.5";
      root["tipo"] = "Animal";
      root["valor"] = "30";
  
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json;charset=utf-8");
      client.println("Server: 192.168.100.8");
      client.println("Connection: close");
      client.println();
      root.printTo(client);
      client.println();
    }

  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }
    
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

