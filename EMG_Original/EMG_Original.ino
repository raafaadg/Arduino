#include "ESP8266WiFi.h"

const int AnalogIn  = A0;
int ADread = 0, mod = 0, i = 0;
float EWMA = 0.0;
int Off_set = 350;

void setup() {
 Serial.begin(115200);  //define a velocidade de comunicação serial em 115200 bps
 pinMode(2, OUTPUT);  //define o pino 2 (LED azul) como saída
 WiFi.disconnect();   //desconecta a conexão WiFi
 WiFi.mode(WIFI_OFF);   //desabilita o modem WiFi para reduzir o consumo de energia
 WiFi.forceSleepBegin(); //entra no modo Sleep
 delay(1);
}

void loop() {
  for (i = 0; i < 2200; i++){
  //ADread = analogRead(AnalogIn)-Off_set;  //efetua a leitura do AD e subtrai do seu nivel de off-set
  ADread = analogRead(AnalogIn);  //efetua a leitura do AD e subtrai do seu nivel de off-set
  mod = abs (ADread);  //calcula o módulo da leitura do AD
  EWMA = mod*0.0001+EWMA*0.9999;  // calcula a média movel exponencial para 10000 amostras
  }
  //digitalWrite(2, LOW); //liga o LED
  //Serial.print(millis()/1000);  //imprime o tempo em segundos
  //erial.print(", ");
  Serial.println(ADread);  //imprime o valor da EWMA
  //digitalWrite(2, HIGH); //desliga o LED
}

