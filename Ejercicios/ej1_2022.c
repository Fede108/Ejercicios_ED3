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

#define memory_block 0x00007

uint16_t *data1 = (uint16_t*) memory_block;

#define FREQ 200000 // 100KHz por muestra
void configure_adc (void){
    //configure pin 0.23-0.24 adc chanel 0, chanel 1
    ADC_Init      (LPC_ADC, FREQ);
    ADC_BurstCmd  (LPC_ADC, ENABLE);
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE); 
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_1, ENABLE);
    ADC_IntConfig (LPC_ADC, ADC_ADINTEN0, ENABLE);
    ADC_IntConfig (LPC_ADC, ADC_ADINTEN1, ENABLE);
    ADC_StartCmd  (LPC_ADC, ADC_START_CONTINUOUS);
    NVIC_EnableIRQ(ADC_IRQn);
        
}

void adc_handler (void){
    if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, 1))
    {
        *data1=(LPC_ADC, ADC_CHANNEL_0);
         data1 = data1 + 1
    }
    if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_1, 1))
    {
        *data1=(LPC_ADC, ADC_CHANNEL_1);
         data1 = data1 + 1
    }
     




}

