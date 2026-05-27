// Definição do Pino
const int RELAY_PIN = 4;   // Pino de controle (IN) do Módulo Relé

// Tempo exato para 100ml baseado no ensaio (5,60 segundos)
const unsigned long TEMPO_ENCHIMENTO = 5600; 

void setup() {
  Serial.begin(115200);

  // Configuração do Relé
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Garante que a bomba inicie desligada

  delay(2000); // Pausa de 2s para você abrir o Serial Monitor

  Serial.println("==================================");
  Serial.println("Iniciando acionamento por tempo...");
  Serial.println("Volume alvo: 100 ml");
  Serial.print("Tempo calculado: ");
  Serial.print(TEMPO_ENCHIMENTO / 1000.0);
  Serial.println(" segundos");
  Serial.println("==================================");

  // 1. LIGA A BOMBA
  digitalWrite(RELAY_PIN, HIGH); 
  Serial.println("Bomba: LIGADA");

  // 2. AGUARDA O TEMPO EXATO DA TABELA
  delay(TEMPO_ENCHIMENTO);

  // 3. DESLIGA A BOMBA
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("Bomba: DESLIGADA");
  Serial.println("Processo concluído com sucesso!");
}

void loop() {
  // O loop fica vazio. 
  // O código roda apenas uma vez quando a placa é ligada ou resetada.
}
