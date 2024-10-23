#define SRAM_ADC_BASE 0x2007C000 // 8kB - 12bits

#define SRAM_DAC_BASE 0x2007E000 // 8kB - 10bits


volatile uint16_t *adc_buffer = (uint16_t*)SRAM_ADC_BASE;
volatile uint16_t *dac_buffer = (uint16_t*)SRAM_DAC_BASE;

void configure_dma(uint16_t* adc, uint16_t* dac);

void funcion_incremento (void){
    configure_dma(adc_buffer, dac_buffer);
    adc_buffer++;
    dac_buffer++;
}

void configure_dma(uint32_t* table);
uint32_t dac_waveform[NUM_SAMPLES]; // Buffer to store DAC waveform values
configure_dma(dac_waveform);   