/**
 * @file ej2_rec2023.c
 * @author your name (you@domain.com)
 * @brief Se tienen tres bloques de datos de 4KBytes de longitud cada uno en el cual se han guardado tres formas
    de onda. Cada muestra de la onda es un valor de 32 bits que se ha capturado desde el ADC. Las direcciones de
    inicio de cada bloque son representadas por macros del estilo DIRECCION_BLOQUE_N, con N=0,1,2.
    Se pide que, usando DMA y DAC se genere una forma de onda por la salida analógica de la LPC1769.
    La forma de onda cambiará en función de una interrupción externa conectada a la placa de la siguiente
    manera:
        ● 1er interrupción: Forma de onda almacenada en bloque 0, con frecuencia de señal de 60[KHz].
        ● 2da interrupción: Forma de onda almacenada en bloque 1 con frecuencia de señal de 120[KHz].
        ● 3ra interrupción: Forma de onda almacenada en bloque 0 y bloque 2 (una a continuación de la otra)
        con frecuencia de señal de 450[KHz].
        ● 4ta interrupción: Vuelve a comenzar con la forma de onda del bloque 0.
    En cada caso se debe utilizar el menor consumo de energía posible del DAC
 * @version 0.1
 * @date 2024-10-20
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

#define TRANSFER_SIZE      4096/4 /* 32 bits= 4bytes data in 4kBytes of memory */
#define DAC_CLOCK          2500000 
#define FREQ_1             600000
#define FREQ_2             1200000
#define FREQ_3             450000
#define DIRECCION_BLOQUE_0 0x2007C000
#define DIRECCION_BLOQUE_1 0x2007D000
#define DIRECCION_BLOQUE_2 0x2007E000

uint32_t *bloque_0 = (uint32_t*)( DIRECCION_BLOQUE_0);
uint32_t *bloque_1 = (uint32_t*)( DIRECCION_BLOQUE_1);
uint32_t *bloque_2 = (uint32_t*)( DIRECCION_BLOQUE_2);

GPDMA_Channel_CFG_Type channel_cfg;
GPDMA_LLI_Type lli_cfg, lli_cfg2;

void configure_dac(void){
    DAC_Init(LPC_DAC);
    DAC_CONVERTER_CFG_Type dac_cfg;
    dac_cfg.CNT_ENA    = ENABLE;
    dac_cfg.DBLBUF_ENA = ENABLE;
    dac_cfg.DMA_ENA    = ENABLE;
    DAC_ConfigDAConverterControl(LPC_DAC , &dac_cfg);
    DAC_SetBias(LPC_DAC, 1); /* less energy consume */
}

void update_TimeOutvalue(uint32_t freq){
   // tiempo = 16,67ms   tiempo por muestra= tiempo/muestras   coun*1/pclkDac = 1/ freq por muestra*
    DAC_SetDMATimeOut(LPC_DAC , DAC_CLOCK/freq*TRANSFER_SIZE );
}

void configure1(void){
    GPDMA_ChannelCmd(0, DISABLE);
    /* continuos signal 1 transfer */
    lli_cfg.DstAddr = (uint32_t) & (LPC_DAC->DACR);
    lli_cfg.SrcAddr = DIRECCION_BLOQUE_0;
    lli_cfg.Control = TRANSFER_SIZE | (2<<18) | (2<<21) | (1<<27); 
    lli_cfg.NextLLI = (uint32_t) &lli_cfg;

    GPDMA_Init();
    /* configure dma signal 1 transfer */
    channel_cfg.ChannelNum    = 0;         
    channel_cfg.DstConn       = GPDMA_CONN_DAC;
    channel_cfg.DstMemAddr    = 0;
    channel_cfg.SrcConn       = 0;
    channel_cfg.SrcMemAddr    = DIRECCION_BLOQUE_0;
    channel_cfg.TransferSize  = TRANSFER_SIZE;
    channel_cfg.TransferType  = GPDMA_TRANSFERTYPE_M2P;
    channel_cfg.TransferWidth = 0;
    channel_cfg.DMALLI        = (uint32_t) &lli_cfg;

    GPDMA_Setup(&channel_cfg);
    GPDMA_ChannelCmd(0, ENABLE);
}

void configure2 (void){ 
    GPDMA_ChannelCmd(0, DISABLE); 
    lli_cfg.SrcAddr = DIRECCION_BLOQUE_1;  /* continuos signal 2 transfer */
    channel_cfg.SrcMemAddr = DIRECCION_BLOQUE_1; /* configure dma signal 2 transfer */
    GPDMA_Setup(&channel_cfg);
    GPDMA_ChannelCmd(0, ENABLE);
}

void configure3 (void){
    GPDMA_ChannelCmd(0, DISABLE);
    /* continuos signal 1-3 transfer */
    lli_cfg.SrcAddr = DIRECCION_BLOQUE_0;
    lli_cfg.NextLLI = (uint32_t) &lli_cfg2;
    /* set up signal 3 transfer */
    lli_cfg2.DstAddr = (uint32_t) & (LPC_DAC->DACR);
    lli_cfg2.SrcAddr = DIRECCION_BLOQUE_2;
    lli_cfg2.Control = TRANSFER_SIZE | (2<<18) | (2<<21) | (1<<27); 
    lli_cfg2.NextLLI = (uint32_t) &lli_cfg;

    
    channel_cfg.SrcMemAddr   = DIRECCION_BLOQUE_0;
    GPDMA_Setup(&channel_cfg);
    GPDMA_ChannelCmd(0, ENABLE);
}

void EINT_IRQHandler (void) {
    EXTI_ClearEXTIFlag (EXTI_EINT0);
    static uint8_t cout=0;
    switch (cout)
    {
    case 0:
        configure1();
        update_TimeOutvalue(FREQ_1);
        cout++;
        break;
    case 1:
        configure2();
        update_TimeOutvalue(FREQ_2);
        cout++;
        break;
     case 2:
        configure3();
        update_TimeOutvalue(FREQ_3);
        cout=0;
        break;
    }
}




