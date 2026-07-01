// Configuração do IP do dispositivo IoT (Altere para o IP que a rede fornecer)
const ESP32_IP = "172.21.44.94"; 

document.getElementById('dispenserForm').addEventListener('submit', function(e) {
    e.preventDefault();

    const volumeInput = document.getElementById('volume');
    const volume = parseInt(volumeInput.value);
    const btnSubmit = document.getElementById('btnSubmit');
    const statusMessage = document.getElementById('statusMessage');
    const statusText = document.getElementById('statusText');
    const progressFill = document.getElementById('progressFill');

    // Validação de segurança no Client-Side para garantir os múltiplos de 50
    if (volume % 50 !== 0 || volume <= 0) {
        alert("Por favor, insira uma quantidade múltipla de 50ml.");
        return;
    }

    // Fator de conversão baseado no ensaio prático: 100ml = 6800ms (68ms por ml)
    const tempoCalculadoMs = volume * 68;
    const tempoSegundos = (tempoCalculadoMs / 1000).toFixed(2);

    // Interface em estado de processamento
    btnSubmit.disabled = true;
    statusMessage.classList.remove('hidden');
    statusText.innerText = `Enviando comando: ${volume}ml (${tempoSegundos}s)...`;
    
    // Inicia animação da barra de progresso casada com o tempo real
    progressFill.style.transition = `width ${tempoSegundos}s linear`;
    progressFill.style.width = '100%';

    // Requisição HTTP HTTP GET para a ESP32 passando o tempo calculado
    fetch(`http://${ESP32_IP}/bomba?tempo=${tempoCalculadoMs}`, { mode: 'cors' })
    .then(response => {
        if(response.ok) {
            statusText.innerText = `Dispensando ${volume}ml de água...`;
            
            // Aguarda o término físico do ciclo para liberar o botão
            setTimeout(() => {
                statusText.innerText = "Processo concluído com sucesso!";
                btnSubmit.disabled = false;
                setTimeout(() => {
                    // Reseta a barra após o sucesso
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