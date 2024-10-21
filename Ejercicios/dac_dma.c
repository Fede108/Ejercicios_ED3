/**
 * @file dac_dma.c
 * @author your name (you@domain.com)
 * @brief 23. 1- A partir del ejemplo adjunto y dada la siguiente variable tipo array que define un cuarto de onda senoidal, como se muestra en la figura adjunta, generar una onda analogica senoidal de 400 Hz de frecuencia utilizando un timer y el DAC. uint32 t seno [17]=(0, 50, 100, 148, 196, 241, 284, 324, 361, 395, 425, 451, 472, 489, 501, 509, 511)
              2- Mediante una interrupción externa EINT1 disminuir a la mitad la amplitud actual de la onda cada vez que se presione un pulsador.
 * @version 0.1
 * @date 2024-10-17
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
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_libcfg_default.h"

#define SIGNAL_FREQ 400 
#define NUM_SAMPLING 17*4
#define PCLK_DAC_IN_TIME  1/(25*1000000)  //CCLK divided by 4 = 25MHz
#define DMA_CHANNEL_ZERO 0


uint32_t seno [17]={0, 50, 100, 148, 196, 241, 284, 324, 361, 395, 425, 451, 472, 489, 501, 509, 511};
uint32_t dato [NUM_SAMPLING] = {};
uint32_t sampling_time = 0;
uint32_t counter_value = 0;

void config_dac (void) {
     PINSEL_CFG_Type PinCfg;
    // Configure pin P0.26 as DAC output
    PinCfg.Funcnum = PINSEL_FUNC_2;           // DAC function
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL; // Disable open drain
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;   // No pull-up or pull-down
    PinCfg.Pinnum = PINSEL_PIN_26;            // Pin number 26
    PinCfg.Portnum = PINSEL_PORT_0;           // Port number 0
    PINSEL_ConfigPin(&PinCfg);

    DAC_Init(LPC_DAC);
    DAC_CONVERTER_CFG_Type dac_cfg;
    dac_cfg.CNT_ENA    = ENABLE;
    dac_cfg.DMA_ENA    = ENABLE;
    dac_cfg.DBLBUF_ENA = ENABLE;    /* Double-buffering is enabled only if both, the CNT_ENA and the DBLBUF_ENA bits are set
                                    in DACCTRL. Any write to the DACR register will only load the pre-buffer,
                                    which shares its register address with the DACR register. The DACR itself will be loaded
                                    from the pre-buffer whenever the counter reaches zero and the DMA request is set. At the
                                    same time the counter is reloaded with the COUNTVAL register value.
                                    Reading the DACR register will only return the contents of the DACR register itself, not
                                    the contents of the pre-buffer register.*/
    DAC_ConfigDAConverterControl (LPC_DAC , &dac_cfg);
    sampling_time = (1/SIGNAL_FREQ*NUM_SAMPLING);  /* time = tr*value_cont -> value_cont = time / tr */
    counter_value = (sampling_time / PCLK_DAC_IN_TIME); 
    DAC_SetDMATimeOut(LPC_DAC, counter_value); 
}

void seno_signal (void)
{
      uint16_t index = 0;  // Índice general para llenar dato[]

    /*signal from 0 to 90*/
    for (uint8_t i = 0 ; i < 17 ; i++){    
         dato[index] = 511 + seno[i];  // max dato 1022, dac max value 1024
         dato[index] <<= 6; // Shift left to align with DAC register format
         index++;
    }
     /*signal from 90 to 180*/
    for (uint8_t i = 17 ; i >= 1 ; i--){
         dato[index] = 511 + seno[i-1];  // max dato 1022, dac max value 1024
         dato[index] <<= 6; // Shift left to align with DAC register format
         index++;
    }
     /*signal from 180 to 270*/
    for (uint8_t i = 0 ; i < 17 ; i++){
         dato[index] = 511 - seno[i];  // max dato 1022, dac max value 1024
         dato[index] <<= 6; // Shift left to align with DAC register format
         index++;
    }
     /*signal from 270 to 360*/
    for (uint8_t i = 17 ; i >= 1 ; i--){ 
         dato[index] = 511 - seno[i-1];  // max dato 1022, dac max value 1024
         dato[index] <<= 6; // Shift left to align with DAC register format
         index++;
    }
}

void config_dma (void) {

    GPDMA_LLI_Type DMA_LLI_Struct;
    DMA_LLI_Struct.DstAddr = 0;
    DMA_LLI_Struct.SrcAddr = (uint32_t) & (LPC_DAC->DACR);
    DMA_LLI_Struct.NextLLI = (uint32_t)&DMA_LLI_Struct; //se enlaza a si misma
    DMA_LLI_Struct.Control =  NUM_SAMPLING                 //TransferSize                 _
                             | (2 << 12)                   // Source Burst size 1byte      | Optimized Peripheral Source and Destination transfer width
                             | (2 << 15)                   // Destination Burst size byte _|
                             | (2 << 18)                   // Source width: 32-bit
                             | (2 << 21)                   // Destination width: 32-bit
                             | (1 << 26);                  // Increment source address

    // Initialize the DMA module
    GPDMA_Init();

    GPDMA_Channel_CFG_Type gpdma_cfg;
    gpdma_cfg.ChannelNum    = 0;
    gpdma_cfg.TransferSize  = NUM_SAMPLING;
    gpdma_cfg.TransferType  = GPDMA_TRANSFERTYPE_M2P;
    gpdma_cfg.TransferWidth = 0;                       // Not used. Used for TransferType is GPDMA_TRANSFERTYPE_M2M only
    gpdma_cfg.SrcMemAddr    = (uint32_t) & dato;
    gpdma_cfg.DstMemAddr    = 0;                       // No memory destination (peripheral)
    gpdma_cfg.SrcConn       = 0;                       // Source is memory  
    gpdma_cfg.DstConn       = GPDMA_CONN_DAC;
    gpdma_cfg.DMALLI        = (uint32_t)&DMA_LLI_Struct;

    // Apply DMA configuration
    GPDMA_Setup(&gpdma_cfg);
}

int main (void) {
    SystemInit();
    config_dac();
    seno_signal();
    config_dma();
    // Enable DMA channel 0 to start the waveform generation
    GPDMA_ChannelCmd(DMA_CHANNEL_ZERO, ENABLE);

    while (1)
    {
        /* code */
    }

}
