/**
 * @file ej2_2023.c
 * @author your name (you@domain.com)
 * @brief Programar el microcontrolador LPC1769 en un código de lenguaje C para que mediante su ADC digitalice una señal analógica cuyo ancho de banda es de 16 khz. La señal analógica tiene una amplitud de pico máxima positiva de 3.3 voltios. Los datos deben ser guardados utilizando el Hardware GDMA en la primera mitad de la memoria SRAM ubicada en el bloque AHB SRAM - bank 0, de manera tal que permita almacenar todos los datos posibles que esta memoria nos permita. Los datos deben ser almacenados como un buffer circular conservando siempre las últimas muestras.
    Por otro lado se tiene una forma de onda como se muestra en la imagen a continuación. Esta señal debe ser generada por una función y debe ser reproducida por el DAC desde la segunda mitad de AHB SRAM - bank 0 memoria utilizando DMA de tal forma que se logre un periodo de 614us logrando la máxima resolución y máximo rango de tensión.
    Durante operación normal se debe generar por el DAC la forma de onda mencionada como wave_form. Se debe indicar cuál es el mínimo incremento de tensión de salida de esa forma de onda.
    Cuando interrumpe una extint conectada a un pin, el ADC configurado debe completar el ciclo de conversión que estaba realizando, y ser detenido, a continuación se comienzan a sacar las muestras del ADC por el DAC utilizando DMA y desde las posiciones de memoria originales.
    Cuando interrumpe nuevamente en el mismo pin, se vuelve a repetir la señal del DAC generada por la forma de onda de wave_form previamente almacenada y se arranca de nuevo la conversión de datos del adc. Se alterna así entre los dos estados del sistema con cada interrupción externa.
    Suponer una frecuencia de core cclk de 80 Mhz. El código debe estar debidamente comentado.
 * @version 0.1
 * @date 2024-10-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include <cr_section_macros.h> /* MCUXpresso-specific macros */
#endif

#include "lpc17xx_gpio.h"   /* GPIO */
#include "lpc17xx_pinsel.h" /* Pin Configuration */
#include "lpc17xx_timer.h"  /* Timer0 */
#include "lpc17xx_exti.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_libcfg_default.h"

#define FREQ_ADC 32000 // 16khZ*2 Max data almacenada en la memoria cumpliendo teorma de Nyquist 
#define WAVE_freq 1560
#define PCLK_DAC 20000000 // DAC clock: 20 MHz (CCLK divided by 4)
#define MAX_RESOLUTION 1023
#define TRANSFER_SIZE 8192/2 // 


#define SRAM_ADC_BASE 0x2007C000 // 8kB first half block 0 -> each data (uint16_t) is keept in 2 bytes
#define SRAM_DAC_BASE 0x2007E000 // 8kB second half block 0 

volatile uint16_t *adc_buffer = (uint16_t*)SRAM_ADC_BASE;
volatile uint16_t *dac_buffer = (uint16_t*)SRAM_DAC_BASE;

uint32_t count_sampling = 0;
uint16_t wave[MAX_RESOLUTION] = {};

GPDMA_LLI_Type LLI_Struct1, LLI_Struct2;

void configure_eint (void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_2;
    pin_cfg.Pinnum    = PINSEL_PIN_10;
    pin_cfg.Funcnum   = PINSEL_FUNC_1; /* EINT 0  fuction*/
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    PINSEL_ConfigPin(&pin_cfg);

    EXTI_InitTypeDef cfg_exti;
    cfg_exti.EXTI_Line     = EXTI_EINT0;
    cfg_exti.EXTI_Mode     = EXTI_MODE_EDGE_SENSITIVE;
    cfg_exti.EXTI_polarity = EXTI_POLARITY_HIGH_ACTIVE_OR_RISING_EDGE;
    EXTI_Config(&cfg_exti);
    NVIC_EnableIRQ (EINT0_IRQn);
}


void configure_adc (void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_0;
    pin_cfg.Pinnum    = PINSEL_PIN_23;
    pin_cfg.Funcnum   = PINSEL_FUNC_1; /* Adc channel 0  fuction*/
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    PINSEL_ConfigPin(&pin_cfg);
 
    ADC_Init      (LPC_ADC , FREQ_ADC);
    ADC_ChannelCmd(LPC_ADC , ADC_CHANNEL_0 , ENABLE);
    ADC_BurstCmd  (LPC_ADC , ENABLE);
    ADC_StartCmd  (LPC_ADC , ADC_START_CONTINUOUS); 
    
}

void configure_dac (void){
    DAC_Init (LPC_DAC);
    DAC_CONVERTER_CFG_Type dac_cfg;
    dac_cfg.CNT_ENA    = ENABLE;           
    dac_cfg.DBLBUF_ENA = ENABLE;
    dac_cfg.DMA_ENA    = ENABLE;
    DAC_ConfigDAConverterControl (LPC_DAC , &dac_cfg);

    DAC_SetBias(LPC_DAC , 0); /* settling time 1us == time resolution para maxima resolucion */
    /*

      WAVE_TIME  = TIME_SAMPL*NUM_SAMPLING
      TIME_SAMPL = WAVE_TIME/NUM_SAMPLING   --> NUM_SAMPLING == MAX_RESOLUTION

      TIME_SAMPL = 1/WAVE_freq*MAX_RESOLUTION 
      TIME_SAMPL = (1/PCLK_DAC)*count_sampling  */

    count_sampling  = 20; /* Time resolution = 1 us -> count_sampling = 1 us*20Mhz = 20 */
    DAC_SetDMATimeOut(LPC_DAC , count_sampling);
}


void configure_dma (void)
{
    GPDMA_LLI_Type LLI_Struct1, LLI_Struct2;

    /* Adc continuos conversion transfer */
    LLI_Struct1.DstAddr = (uint32_t) &adc_buffer;
    LLI_Struct1.SrcAddr = (uint32_t) & (LPC_ADC->ADDR0);
    LLI_Struct1.Control = TRANSFER_SIZE | (1<<27);  
                        /*TransferSize     Increment destination address*/               
    LLI_Struct1.NextLLI = (uint32_t) &LLI_Struct1;

    /* DAC wave output */
    LLI_Struct2.DstAddr = (uint32_t) & (LPC_DAC->DACR); 
    LLI_Struct2.SrcAddr = (uint32_t) &dac_buffer;
    LLI_Struct2.Control = TRANSFER_SIZE  | (1<<26);
                        /*TransferSize    Increment source address*/
    LLI_Struct2.NextLLI = (uint32_t) &LLI_Struct2;

    GPDMA_Init(); /* Initialize the dma module */

    /* Chanel 0 for ADC convertion */
    GPDMA_Channel_CFG_Type GPDMA_Struct;
    GPDMA_Struct.ChannelNum    = 0; /* transfer every 1/32Khz = 31,25us, higher priority  */
    GPDMA_Struct.TransferType  = GPDMA_TRANSFERTYPE_P2M;
    GPDMA_Struct.TransferSize  = TRANSFER_SIZE;
    GPDMA_Struct.TransferWidth = 0; // Not used. Used for TransferType is GPDMA_TRANSFERTYPE_M2M only 
    GPDMA_Struct.SrcConn       = GPDMA_CONN_ADC;
    GPDMA_Struct.SrcMemAddr    = 0;
    GPDMA_Struct.DstConn       = 0;
    GPDMA_Struct.DstMemAddr    = (uint32_t) &adc_buffer;
    GPDMA_Struct.DMALLI        = (uint32_t) &LLI_Struct1;       
    GPDMA_Setup(&GPDMA_Struct);

    /* Chanel 1 for DAC playback */
    GPDMA_Channel_CFG_Type GPDMA_Struct;
    GPDMA_Struct.ChannelNum    = 1; /* transfer every 1us, lower level priority*/
    GPDMA_Struct.TransferType  = GPDMA_TRANSFERTYPE_M2P;
    GPDMA_Struct.TransferSize  = TRANSFER_SIZE;
    GPDMA_Struct.TransferWidth = 0; // Not used. Used for TransferType is GPDMA_TRANSFERTYPE_M2M only 
    GPDMA_Struct.SrcConn       = 0;
    GPDMA_Struct.SrcMemAddr    = (uint32_t) &dac_buffer ;
    GPDMA_Struct.DstConn       = (uint32_t) & (LPC_DAC->DACR);
    GPDMA_Struct.DstMemAddr    = 0;
    GPDMA_Struct.DMALLI        = (uint32_t) &LLI_Struct2;       
    GPDMA_Setup(&GPDMA_Struct);

    GPDMA_ChannelCmd (0 , ENABLE); /*  normal operation starts */
    GPDMA_ChannelCmd (1 , ENABLE);
}

void wave_fuction (void){
    for (uint16_t i = 0; i < MAX_RESOLUTION/2; i++)
    {
        wave[i] = 512 + i;
    }
    for (uint16_t i = 0; i < MAX_RESOLUTION/2; i++)
    {
        wave[i + 512] =  i;
    }

    for (uint16_t index = 0; index<TRANSFER_SIZE ; index++){
        dac_buffer[index] = wave[index];
        dac_buffer[index] <<= 6; // Shift left to align with DAC register format
    }
   
}



void EINT1_IRQn (void){
    static uint8_t flag = 0;
    EXTI_ClearEXTIFlag (EXTI_EINT0);
    if(flag){
        ADC_ChannelCmd(LPC_ADC , ADC_CHANNEL_0 , ENABLE);
        GPDMA_ChannelCmd (0 , ENABLE);
    }
    else {
        ADC_ChannelCmd(LPC_ADC , ADC_CHANNEL_0 , DISABLE);
        GPDMA_ChannelCmd (0 , DISABLE);
        for (uint16_t i = 0; i<TRANSFER_SIZE ; i++){
            dac_buffer[i] = (adc_buffer[i] >> 4) & 0xFFF; // Shift rigth keep adc result only
            dac_buffer[i] <<= 6; // Shift left to align with DAC register format
        }  
    }
    flag = ! flag;
}

int main (void){
    SystemInit();
    wave_fuction();
    configure_adc();
    configure_dac();
    configure_dma();
    configure_eint();
    while (1)
    {
        /* code */
    }
}
