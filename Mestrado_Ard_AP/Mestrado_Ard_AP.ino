/*
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

/* Set these to your desired credentials. */
const char *ssid = "ESPap";
const char *password = "thereisnospoon";

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  server.send(200, "text/html", "<h1>Bem Vindo!</h1>");
}

void handleGPIO0(){
   /* WiFiClient client;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["comando"] = "0";
    root["nome"] = "Rafael";
    root["idade"] = "23";
    root["peso"] = "77";
    root["tipo"] = "Humano";
    root["valor"] = "10";*/

    server.send(200, "application/json;charset=utf-8", "{\"comando\":\"0\",\"nome\":\"Rafael\",\"idade\":\"23\",\"peso\":\"77\",\"tipo\":\"Humano\",\"valor\":\"10\"}");

    /*client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json;charset=utf-8");
    client.println("Server: 192.168.4.1");
    client.println("Connection: close");
    client.println();
    root.printTo(client);
    client.println();*/
  }

void handleGPIO1(){
   /* WiFiClient client;    
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
    client.println("Server: 192.168.4.1");
    client.println("Connection: close");
    client.println();
    root.printTo(client);
    client.println();*/

    server.send(200, "application/json;charset=utf-8", "{\"comando\":\"1\",\"nome\":\"Aline\",\"idade\":\"26\",\"peso\":\"65\",\"tipo\":\"Humano\",\"valor\":\"20\"}");
  }
void handleGPIO2(){
      /*WiFiClient client;      
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
      client.println("Server: 192.168.4.1");
      client.println("Connection: close");
      client.println();
      root.printTo(client);
      client.println();*/

      server.send(200, "application/json;charset=utf-8", "{\"comando\":\"2\",\"nome\":\"Theo\",\"idade\":\"0.5\",\"peso\":\"13.5\",\"tipo\":\"Animal\",\"valor\":\"25\"}");
    }
void handleGPIO3(){
      /*WiFiClient client;      
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.createObject();
      root["comando"] = "3";
      root["nome"] = "Fefe";
      root["idade"] = "1";
      root["peso"] = "3.5";
      root["tipo"] = "Animal";
      root["valor"] = "30";
  
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json;charset=utf-8");
     client.println("Server: 192.168.4.1");
      client.println("Connection: close");
      client.println();
      root.printTo(client);
      client.println();*/

      server.send(200, "application/json;charset=utf-8", "{\"comando\":\"3\",\"nome\":\"Fefe\",\"idade\":\"1\",\"peso\":\"3.5\",\"tipo\":\"Animal\",\"valor\":\"10\"}");
    }

void setup() {
	delay(1000);
	Serial.begin(115200);
	Serial.println();
	Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid);

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
  server.on("/", handleRoot);  
  server.on("/gpio/0", handleGPIO0);
  server.on("/gpio/1", handleGPIO1);
  server.on("/gpio/2", handleGPIO2);
  server.on("/gpio/3", handleGPIO3);

	server.begin();
	Serial.println("HTTP server started");
}

void loop() {
	server.handleClient();


}
