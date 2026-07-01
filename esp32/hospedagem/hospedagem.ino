#include <WiFi.h>
#include <WebServer.h>

// ==========================================
// CONFIGURAÇÕES DA SUA REDE WI-FI
// ==========================================
const char* ssid = "IOT_CT";
const char* password = "1@T_ct2023";

WebServer server(80);
const int RELAY_PIN = 4; // Pino D4 ligado ao IN do Relé

// ==========================================
// A INTERFACE (HTML + CSS + JS)
// ==========================================
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dispensador de Água IoT</title>
    <style>
        /* Configurações Globais e Paleta Refrescante */
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
        body { background: linear-gradient(135deg, #e0f7fa 0%, #e8f5e9 100%); min-height: 100vh; display: flex; justify-content: center; align-items: center; color: #263238; }
        .container { width: 100%; max-width: 420px; padding: 20px; }
        .water-card { background: rgba(255, 255, 255, 0.85); backdrop-filter: blur(10px); border-radius: 24px; padding: 35px 30px; box-shadow: 0 10px 30px rgba(0, 172, 193, 0.1); border: 1px solid rgba(255, 255, 255, 0.6); text-align: center; }
        .icon-header { font-size: 3rem; margin-bottom: 10px; animation: float 3s ease-in-out infinite; }
        h1 { font-size: 1.6rem; color: #006064; font-weight: 600; }
        .subtitle { font-size: 0.9rem; color: #00838f; margin-bottom: 30px; }
        .input-group { text-align: left; margin-bottom: 25px; }
        label { display: block; font-weight: 600; margin-bottom: 8px; color: #006064; font-size: 0.95rem; }
        input[type="number"] { width: 100%; padding: 14px 20px; font-size: 1.2rem; border: 2px solid #b2ebf2; border-radius: 14px; background: #fff; color: #006064; font-weight: bold; outline: none; transition: all 0.3s ease; text-align: center; }
        input[type="number"]:focus { border-color: #00bcd4; box-shadow: 0 0 10px rgba(0, 188, 212, 0.2); }
        .hint { display: block; font-size: 0.8rem; color: #78909c; margin-top: 6px; font-style: italic; }
        button { width: 100%; padding: 16px; font-size: 1.1rem; font-weight: 600; color: white; background: linear-gradient(90deg, #00bcd4, #4caf50); border: none; border-radius: 14px; cursor: pointer; transition: all 0.3s ease; box-shadow: 0 4px 15px rgba(0, 188, 212, 0.3); }
        button:hover { transform: translateY(-2px); box-shadow: 0 6px 20px rgba(0, 188, 212, 0.4); }
        button:active { transform: translateY(0); }
        .status-box { margin-top: 25px; padding: 15px; background: #e0f2f1; border-radius: 12px; border: 1px solid #b2dfdb; transition: all 0.3s ease; }
        .hidden { display: none; }
        #statusText { font-size: 0.95rem; color: #004d40; margin-bottom: 10px; font-weight: 500; }
        .progress-bar { width: 100%; height: 8px; background: #b2dfdb; border-radius: 4px; overflow: hidden; }
        #progressFill { width: 0%; height: 100%; background: #009688; transition: width linear; }
        @keyframes float { 0%, 100% { transform: translateY(0); } 50% { transform: translateY(-8px); } }
    </style>
</head>
<body>
    <div class="container">
        <div class="water-card">
            <div class="icon-header">💧</div>
            <h1>Dispensador de Água</h1>
            <p class="subtitle">Controle de Vazão de Precisão</p>
            
            <form id="dispenserForm">
                <div class="input-group">
                    <label for="volume">Quantidade (ml):</label>
                    <input type="number" id="volume" name="volume" value="100" min="10" max="1000" step="10" required>
                    <span class="hint">Apenas múltiplos de 10ml</span>
                </div>
                <button type="submit" id="btnSubmit">Iniciar Injeção</button>
            </form>

            <div id="statusMessage" class="status-box hidden">
                <p id="statusText">Aguardando comando...</p>
                <div class="progress-bar"><div id="progressFill"></div></div>
            </div>
        </div>
    </div>
    
    <script>
        document.getElementById('dispenserForm').addEventListener('submit', function(e) {
            e.preventDefault();

            const volumeInput = document.getElementById('volume');
            const volume = parseInt(volumeInput.value);
            const btnSubmit = document.getElementById('btnSubmit');
            const statusMessage = document.getElementById('statusMessage');
            const statusText = document.getElementById('statusText');
            const progressFill = document.getElementById('progressFill');

            if (volume % 10 !== 0 || volume <= 0) {
                alert("Por favor, insira uma quantidade múltipla de 10ml.");
                return;
            }

            // A SUA CALIBRAÇÃO (60ms por ml)
            const tempoCalculadoMs = volume * 68;
            const tempoSegundos = (tempoCalculadoMs / 1000).toFixed(2);

            btnSubmit.disabled = true;
            statusMessage.classList.remove('hidden');
            statusText.innerText = `Enviando comando: ${volume}ml (${tempoSegundos}s)...`;
            
            progressFill.style.transition = `width ${tempoSegundos}s linear`;
            progressFill.style.width = '100%';

            // REQUISIÇÃO COM CAMINHO RELATIVO (Para não depender do IP fixo)
            fetch(`/bomba?tempo=${tempoCalculadoMs}`)
            .then(response => {
                if(response.ok) {
                    statusText.innerText = `Dispensando ${volume}ml de água...`;
                    
                    setTimeout(() => {
                        statusText.innerText = "Processo concluído com sucesso!";
                        btnSubmit.disabled = false;
                        setTimeout(() => {
                            progressFill.style.transition = 'none';
                            progressFill.style.width = '0%';
                        }, 1000);
                    }, tempoCalculadoMs);
                } else {
                    throw new Error("Falha no servidor IoT");
                }
            })
            .catch(error => {
                console.error("Erro na comunicação:", error);
                statusText.innerText = "Erro: Dispositivo inacessível.";
                btnSubmit.disabled = false;
                progressFill.style.transition = 'none';
                progressFill.style.width = '0%';
            });
        });
    </script>
</body>
</html>
)=====";

// ==========================================
// FUNÇÕES DO SERVIDOR
// ==========================================

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handleBomba() {
  // Puxa o argumento "tempo" que o fetch do JavaScript enviou
  if (server.hasArg("tempo")) {
    int tempo_ms = server.arg("tempo").toInt();
    
    Serial.print("Comando via Web: Ligando bomba por ");
    Serial.print(tempo_ms);
    Serial.println(" ms.");

    // LIGA O RELÉ E A BOMBA
    digitalWrite(RELAY_PIN, HIGH); 
    
    // Aguarda o tempo exato calculado pelo JavaScript
    delay(tempo_ms); 
    
    // DESLIGA O RELÉ
    digitalWrite(RELAY_PIN, LOW); 
    
    Serial.println("Ciclo finalizado.");
    
    // Libera a resposta para o navegador saber que a água caiu
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Faltou o parametro tempo");
  }
}

// ==========================================
// SETUP INICIAL
// ==========================================
void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Relé inicia desligado

  WiFi.begin(ssid, password);
  Serial.print("Iniciando conexão Wi-Fi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nRede estabelecida!");
  Serial.print("Acesse a interface em: http://");
  Serial.println(WiFi.localIP());

  // Rotas
  server.on("/", handleRoot);
  server.on("/bomba", handleBomba);

  server.begin();
  Serial.println("Servidor web pronto para operacao.");
}

void loop() {
  server.handleClient();
}
