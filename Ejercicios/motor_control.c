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

#define FREQ_ADC 100000 /* 100 kHz */
#define MAX_ADC_VALUE ((uint16_t)(0xFFF))
#define PIN_0 ((uint32_t)(1<<0))

uint8_t match_value = 0;

void config_pin (void){
   

}

void config_adc (void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_0;
    pin_cfg.Pinnum    = PINSEL_PIN_23;
    pin_cfg.Funcnum   = PINSEL_FUNC_1; /* Adc channel 0  fuction*/
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    PINSEL_ConfigPin(&pin_cfg);

    ADC_Init      (LPC_ADC , FREQ_ADC);
    ADC_ChannelCmd(LPC_ADC , ADC_CHANNEL_0 , ENABLE); /* Enable channel 0 */
    ADC_IntConfig (LPC_ADC , ADC_ADINTEN0 , ENABLE);
    ADC_StartCmd  (LPC_ADC , ADC_START_ON_MAT10);
    NVIC_Enable(ADC_IRQn);
}

void config_timer (void){
    TIM_TIMERCFG_Type tim_cfg;
    tim_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    tim_cfg.PrescaleValue  = 1;
    TIM_Init (LPC_TIM0 , TIM_TIMER_MODE , &tim_cfg);
    /* Timer init for ADC*/
    TIM_Init (LPC_TIM1 , TIM_TIMER_MODE , &tim_cfg);
}

void config_match (void){
    TIM_MATCHCFG_Type match_cfg;
    match_cfg.StopOnMatch        = DISABLE;
    match_cfg.ResetOnMatch       = ENABLE;
    match_cfg.IntOnMatch         = DISABLE;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING; /*??? nothing o toggle*/
    match_cfg.MatchChannel       = 1;
    match_cfg.MatchValue         = 200000; /* Time= 1us*200000 */
    TIM_ConfigMatch (LPC_TIM1 , &match_cfg);

    
    match_cfg.StopOnMatch        = DISABLE;
    match_cfg.ResetOnMatch       = ENABLE;
    match_cfg.IntOnMatch         = ENABLE;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
    match_cfg.MatchChannel       = 0;
    match_cfg.MatchValue         = 100; /* Time = 1us*100 = 100us --> 100kHz */
    TIM_ConfigMatch (LPC_TIM0 , &match_cfg);    

    match_cfg.ResetOnMatch       = DISABLE;
    match_cfg.MatchChannel       = 1;
    match_cfg.MatchValue         = match_value; /* Time= 10us*match_value */
    TIM_ConfigMatch (LPC_TIM0 , &match_cfg);    

    NVIC_EnableIRQ(TIMER0_IRQn); // Habilitar interrupción de TIMER0     
 
    TIM_Cmd(LPC_TIM0, ENABLE);
    TIM_Cmd(LPC_TIM1, ENABLE);

}

void ADC_IRQHandler (void) {
    uint16_t data = ADC_ChannelGetData (LPC_ADC , ADC_CHANNEL_0);
    if(GPIO_ReadValue (PINSEL_PORT_0) & PIN_0){
        match_value = data*20/MAX_ADC_VALUE ;
        TIM_UpdateMatchValue (LPC_TIM0 , 0 , match_value);
    }
    else {
        match_value = data*100/MAX_ADC_VALUE ;
        TIM_UpdateMatchValue (LPC_TIM0 , 0 , match_value);
    }
}

void TIMER0_IRQHandler (void){
    if(TIM_GetIntStatus (LPC_TIM0 , TIM_MR0_INT))
    {   
       TIM_ClearIntPending (LPC_TIM0 , TIM_MR0_INT);
       GPIO_SetValue     ;
    }
    if (TIM_GetIntStatus (LPC_TIM0 , TIM_MR1_INT))
    {   
        TIM_ClearIntPending (LPC_TIM0 , TIM_MR1_INT);
        GPIO_ClearValue  ;
    }
}