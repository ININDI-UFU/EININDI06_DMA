#include <iikit.h>      // Biblioteca base do framework Arduino
#include "util/AdcDmaEsp.h" // Classe para configuração do ADC e DMA

void osciloscope(const int16_t *y, size_t ylen)
{
   IIKit.WSerial.plot("adcValue", (uint32_t)1000, y, ylen);
   IIKit.disp.setText(2, ("P1:" + String(y[ylen-1])).c_str());
} 

void setup()
{
    adcDmaSetup(ADC1_CHANNEL_0, 1000UL, osciloscope, 100000UL, ADC_WIDTH_BIT_12);
}

void loop()
{
    IIKit.loop();
    adcDmaLoop();
}
