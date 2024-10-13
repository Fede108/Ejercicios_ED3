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

#define FREQ 5000 /*5khz sampling rate*/
#define MATCH_FREQ 880 /* Square wave of 440 Hz, match frequency must be twice as high */
uint32_t data=0;
uint16_t N=0;

void configure_adc (void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_0;
    pin_cfg.Pinnum    = PINSEL_PIN_23;
    pin_cfg.Funcnum   = PINSEL_FUNC_1; /* Adc channel 0  fuction*/
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    PINSEL_ConfigPin(&pin_cfg);
 
    ADC_Init(LPC_ADC, FREQ); /* Initialize the ADC peripheral with a 5 kHz sampling frequency */
    ADC_BurstCmd  (LPC_ADC , ENABLE); /* Enable burst mode */
    ADC_ChannelCmd(LPC_ADC , ADC_CHANNEL_0 , ENABLE); /* Enable channel 0 */
    ADC_IntConfig (LPC_ADC , ADC_ADINTEN0 , ENABLE); /* Enable interruption */
    ADC_StartCmd  (LPC_ADC , ADC_START_NOW); /*Starts convertion*/
    NVIC_Enable(ADC_IRQn);
}

void configure_timer_and_match (void){
    TIM_TIMERCFG_Type tim_cfg;
    tim_cfg.PrescaleOption = TIM_PRESCALE_USVAL 
    tim_cfg.PrescaleValue  = 1000 /*1ms time resolution*/
    TIM_Init (LPC_TIM0, TIM_TIMER_MODE, &tim_cfg);

    TIM_MATCHCFG_Type match_cfg;
    match_cfg.StopOnMatch        = DISABLE;
    match_cfg.ResetOnMatch       = DISABLE;
    match_cfg.IntOnMatch         = DISABLE;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
    match_cfg.MatchChannel       = 0;
    match_cfg.MatchValue         = (1/(MATCH_FREQ*1000)-1);  /* Time=T_resolution*Match_value */
    TIM_ConfigMatch(LPC_TIM0, &match_cfg);
    TIM_Cmd(LPC_TIM0 , ENABLE);
}



void ADC_IRQHandler (void){

    NVIC_Disable(ADC_IRQn); /*Nvic disable allows to get data of N sampling in 1 interruption*/
    for(uint16_t i=0, i<N , i++){
        while (!(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_DONE))); /* Wait for the ADC conversion to finish */
        if (ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_BURST)) /* Check overrun bit*/
        {
            ADC_DeInit(LPC_ADC); /* Close ADC */
            configure_timer_and_match();
            break;
        }
        else{
            data += ADC_ChannelGetData(LPC_ADC , ADC_CHANNEL_0); /* DONE is negated when ADDR is read, flag is cleared */
        }
        
    }
    promedio=(data/N);
    NVIC_Enable(ADC_IRQn);

}
