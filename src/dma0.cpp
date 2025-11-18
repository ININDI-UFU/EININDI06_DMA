#include <iikit.h>
#include <util/AdcDmaEsp.h>

// Instância global
AdcDmaEsp adcDma;

// Buffer para leitura no loop
static int16_t readBuffer[256];

void setup() {
    IIKit.setup();
    delay(1000);
    wserial::println();
    wserial::println("=== Teste ADC DMA ESP32 ===");

    // OBS: begin() agora aceita: (GPIO, sample_rate)
    bool ok = adcDma.beginGPIO(36,4000,10);
    if (!ok) wserial::println("Falha ao iniciar ADC DMA!");
    else wserial::println("ADC DMA iniciado com sucesso.");
}

void loop() {
    IIKit.loop();
    size_t n = adcDma.read(samples,BLOCK_SIZE); // Lê o bloco completo (bloqueante)
    if (n > 0)  wserial::plot("adcValue", 1, samples, n); // Envia para o LasecPlot
}
