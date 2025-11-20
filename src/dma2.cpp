#include <iikit.h>      // Biblioteca base do framework Arduino
#include <driver/dac.h>
#include "AdcDmaEsp.h"

// 1) Configurações para o DAC (gerano um seno)
const int SAMPLES = 100;
uint8_t sineTable[SAMPLES];
volatile int sineIndex = 0;
    
// === Timer callback ===
void IRAM_ATTR onTimer(void *param) {
    dac_output_voltage(DAC_CHANNEL_1, sineTable[sineIndex++]);// Envia o próximo ponto da senoide para o DAC
    if (sineIndex >= SAMPLES) sineIndex = 0;  // volta pro início da tabela
}

// 2) Configurações para o DMA_ADC
#define BLOCK_SIZE 512  // 512 amostras
AdcDmaEsp adcDma;
uint16_t samples[BLOCK_SIZE];

void setup()
{
    IIKit.setup();
    // Mapeamos a senoide (de -1 a 1) para valores de 0 a 255, que o DAC espera.
    for (int i = 0; i < SAMPLES; i++)
    {
        float angle = (2 * PI * i) / SAMPLES;
        float s = sin(angle);
        int dacValue = (int)((s + 1.0) * 70.0f + 100.0f) ; // mapeia: -1 → 25 e 1 → 225
        sineTable[i] = (uint8_t) dacValue;
    }

    // === Configura o timer de alta precisão ===
    // DAC1	GPIO 25	DAC_CHANNEL_1
    // DAC2	GPIO 26	DAC_CHANNEL_2
    dac_output_enable(DAC_CHANNEL_1);
    esp_timer_create_args_t timerConf = {};
    timerConf.callback = &onTimer;
    timerConf.name = "sineTimer";

    esp_timer_handle_t timer;
    esp_timer_create(&timerConf, &timer);
    esp_timer_start_periodic(timer, 166); // 60 Hz com 100 amostras → 6000 Hz → período 166 microsegundos

    // OBS: begin() agora aceita: (GPIO, sample_rate)
    bool ok = adcDma.beginGPIO(36,4000,10);
    if (!ok) wserial::println("Falha ao iniciar ADC DMA!");
    else wserial::println("ADC DMA iniciado com sucesso.");
}

void loop()
{
    IIKit.loop();
    size_t n = adcDma.read(samples,BLOCK_SIZE); // Lê o bloco completo (bloqueante)
    if (n > 0)  wserial::plotRaw("adcValue", 1, samples, n); // Envia para o LasecPlot
}