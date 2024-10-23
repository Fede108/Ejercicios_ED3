/**
 * @file ej1_rec2023.c
 * @author your name (you@domain.com)
 * @brief Por un pin del ADC del microcontrolador LPC1769 ingresa una tensión de rango dinámico 0 a 3,3[v]
    proveniente de un sensor de temperatura. Debido a la baja tasa de variación de la señal, se pide tomar una
    muestra cada 30[s]. Pasados los 2[min] se debe promediar las últimas 4 muestras y en función de este valor,
    tomar una decisión sobre una salida digital de la placa:
        ● Si el valor es <1 [V] colocar la salida en 0 (0[V]).
        ● Si el valor es >= 1[V] y <=2[V] modular una señal PWM con un Ciclo de trabajo que va desde el 50%
        hasta el 90% proporcional al valor de tensión, con un periodo de 20[KHz].
        ● Si el valor es > 2[V] colocar la salida en 1 (3,3[V]).

 * @version 0.1
 * @date 2024-10-21
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

#define FREQ 500000
uint32_t duty_cycle;
volatile uint32_t promedio, muestras;


void configure_timer (void){
    TIM_TIMERCFG_Type tim_cfg;
    tim_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    tim_cfg.PrescaleValue  = 1000;
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tim_cfg);

    TIM_MATCHCFG_Type match_cfg;
    match_cfg.IntOnMatch         = DISABLE; 
    match_cfg.MatchChannel       = 1;
    match_cfg.MatchValue         = 15000;
    match_cfg.ResetOnMatch       = ENABLE;
    match_cfg.StopOnMatch        = DISABLE;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
    TIM_ConfigMatch(LPC_TIM0, &match_cfg);
}

void GPIO_PIN (void){
    //as gpio0.0
    PINSEL_CFG_Type pin_cfg_struct;
    pin_cfg_struct.Portnum   = PINSEL_PORT_0;
    pin_cfg_struct.Pinnum    = PINSEL_PIN_0;
    pin_cfg_struct.Funcnum   = PINSEL_FUNC_0;
    pin_cfg_struct.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg_struct.Pinmode   = PINSEL_PINMODE_PULLUP; 
    PINSEL_ConfigPin(&pin_cfg_struct);
    GPIO_SetDir (PINSEL_PORT_0, PINSEL_PIN_0 , 0);

}

void MATCH(uint32_t match_value){
    TIM_TIMERCFG_Type tim_cfg;
    tim_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    tim_cfg.PrescaleValue  = 5;
    TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &tim_cfg);

    TIM_MATCHCFG_Type match_cfg;
    match_cfg.IntOnMatch         = ENABLE; 
    match_cfg.MatchChannel       = 0;
    match_cfg.MatchValue         = match_value;
    match_cfg.ResetOnMatch       = ENABLE;
    match_cfg.StopOnMatch        = DISABLE;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
    TIM_ConfigMatch(LPC_TIM1, &match_cfg);
}

void configure_adc(void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_0;
    pin_cfg.Pinnum    = PINSEL_PIN_23;
    pin_cfg.Funcnum   = PINSEL_FUNC_1; /* Adc channel 0  fuction*/
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    PINSEL_ConfigPin(&pin_cfg);

    ADC_Init      (LPC_ADC, FREQ);
    ADC_StartCmd  (LPC_ADC, ADC_START_ON_MAT01);
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE); 
    ADC_IntConfig (LPC_ADC, ADC_ADINTEN0, ENABLE);
    NVIC_EnableIRQ(ADC_IRQn);
}

void ADC_IRQHandler(void){
    static uint8_t count = 0;
    count = (count < 4) ? count = count + 1 : 0;
    if(!count) {
        promedio = muestras/4;
        promedio = (promedio*3300)/4095;  /* promedio 0 a 3300mV*/
        muestras=0;
        pin_output();
    }

    muestras = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0) + muestras;

}

void pin_output(void){
    if (promedio<1000)
    {   
        GPIO_ClearValue(PINSEL_PORT_0, PINSEL_PIN_0);
        TIM_Cmd(LPC_TIM1, DISABLE);
    }
    if(promedio>2000)
    {
        GPIO_SetValue(PINSEL_PORT_0, PINSEL_PIN_0);  
        TIM_Cmd(LPC_TIM1, DISABLE);
    }
    if(promedio >= 1000 & promedio <= 2000)
    {
        duty_cycle = (promedio*90/2000);
        TIM_UpdateMatchValue(LPC_TIM1, 0, duty_cycle);
        TIM_UpdateMatchValue(LPC_TIM1, 1, (100-duty_cycle));
        TIM_Cmd(LPC_TIM1, ENABLE);
    }
}

void TIMER1_IRQHandler (void){
    if(TIM_GetIntStatus(LPC_TIM1, TIM_MR0_INT))
    {
        TIM_ClearIntPending(LPC_TIM1, 0);
        GPIO_SetValue(PINSEL_PORT_0, PINSEL_PIN_0);  
    }
    
    if(TIM_GetIntStatus(LPC_TIM1, TIM_MR1_INT))
    {
        TIM_ClearIntPending(LPC_TIM1, 1);
        GPIO_ClearValue(PINSEL_PORT_0, PINSEL_PIN_0);  
    }
}