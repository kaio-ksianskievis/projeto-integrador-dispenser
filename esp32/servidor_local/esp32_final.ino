#include <WiFi.h>
#include <WebServer.h>
#include "soc/soc.h"           
#include "soc/rtc_cntl_reg.h"  

// Coloque o nome da sua rede e senha aqui
const char* ssid = "IOT_CT"; 
const char* password = "1@T_ct2023";      

WebServer server(80);
const int RELAY_PIN = 4;

void setup() {
  // 1. Desativa o alarme de queda de tensão
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
  
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); 

  Serial.println("\nIniciando conexão Wi-Fi...");

  // Isso impede o pico de corrente que está reiniciando o seu notebook.
  WiFi.setTxPower(WIFI_POWER_8_5dBm);

  WiFi.begin(ssid, password);

  // Fica aguardando conectar
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n=================================");
  Serial.println("Conexão Wi-Fi estabelecida!");
  Serial.print("COPIE ESTE IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("=================================");

  // 3. Rota do servidor para ligar a bomba
  server.on("/bomba", HTTP_GET, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (server.hasArg("tempo")) {
      String tempoParam = server.arg("tempo");
      unsigned long tempoEnchimento = tempoParam.toInt();

      Serial.print("Comando recebido! Acionando por: ");
      Serial.print(tempoEnchimento);
      Serial.println(" ms");

      digitalWrite(RELAY_PIN, HIGH);
      delay(tempoEnchimento);
      digitalWrite(RELAY_PIN, LOW);

      server.send(200, "text/plain", "Bomba acionada com sucesso");
    } else {
      server.send(400, "text/plain", "Faltou informar o tempo");
    }
  });

  server.begin();
  Serial.println("Servidor Web rodando e aguardando comandos.");
}

void loop() {
  server.handleClient();
}
