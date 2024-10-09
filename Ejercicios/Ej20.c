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

#define ADC_FREQ_FOR_EACH_CHANNEL 50000 /* 50 kHz */
#define NUMBER_OF_CHANNELS 4

void configure_adc (void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_0;
    pin_cfg.Pinnum    = PINSEL_PIN_23;
    pin_cfg.Funcnum   = PINSEL_FUNC_1; /* Adc input fuction*/
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum    = PINSEL_PIN_24;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum    = PINSEL_PIN_25;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum    = PINSEL_PIN_26;
    PINSEL_ConfigPin(&pin_cfg);

    ADC_Init(LPC_ADC, ADC_FREQ*NUMBER_OF_CHANNELS); /* Initialize the ADC peripheral with a 50 kHz sampling frequency */
    ADC_BurstCmd(LPC_ADC, ENABLE);/* Enable burst mode*/
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE); /* Enable ADC channel 0 */
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_1, ENABLE); /* Enable ADC channel 1 */
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_2, ENABLE); /* Enable ADC channel 2 */
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_3, ENABLE); /* Enable ADC channel 3 */
    ADC_StartCmd(LPC_ADC, ADC_START_NOW); /*Starts convertion*/
            
}