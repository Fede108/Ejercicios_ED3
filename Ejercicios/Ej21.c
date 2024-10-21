/**
 * @file Ej21.c
 * @author your name (you@domain.com)
 * @brief Realizar un código en C que permita filtrar las muestras provenientes del ADC con
        un filtro promediador móvil de N muestras, donde N inicialmente es igual a 2 y
        puede ser incrementado en dos unidades cada vez que se presiona un pulsador
        conectado en P0.6 hasta llegar a un valor de 600. A su vez, utilizando el bit de
        overrun y un timer, complete el código realizado para que en caso de producirse el
        evento de pérdida de datos por parte del ADC, se deje de muestrear y se saque por
        el pin MATCH2.1 una señal cuadrada de 440 Hz. Considerar una frecuencia de cclk
        de 60 Mhz y configurar el ADC para obtener una frecuencia de muestreo de 5
        Kmuestras/seg.
 * @version 0.1
 * @date 2024-10-18
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

#define FREQ 5000 /*5khz sampling rate*/
#define MATCH_FREQ 880 /* Square wave of 440 Hz, match frequency must be twice as high */
#define PIN_6 ((uint32_t)(1<<6))
#define INPUT 0
#define RISING 0

uint32_t data=0;
uint32_t promedio=0;
uint16_t N=2;

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
    tim_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    tim_cfg.PrescaleValue  = 10; /* 1us*10=10us time resolution */
    TIM_Init (LPC_TIM0, TIM_TIMER_MODE, &tim_cfg);

    //configure pin as MAT2.0 output
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_4;
    pin_cfg.Pinnum    = PINSEL_PIN_28;
    pin_cfg.Funcnum   = PINSEL_FUNC_2;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    PINSEL_ConfigPin(&pin_cfg);

    TIM_MATCHCFG_Type match_cfg;
    match_cfg.StopOnMatch        = DISABLE;
    match_cfg.ResetOnMatch       = DISABLE;
    match_cfg.IntOnMatch         = DISABLE;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
    match_cfg.MatchChannel       = 2;
    match_cfg.MatchValue         = (1*100000/(MATCH_FREQ)-1);  /* Time = T_resolution*Match_value */
    /* (1/880Hz)=1.136ms  --> MatchValue=(1.136ms/10us)=113       Time = 10us*113 = 1.13ms */
    TIM_ConfigMatch(LPC_TIM0, &match_cfg);
    TIM_Cmd(LPC_TIM0 , ENABLE);
}

void configure_gpio (void){

    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_0;
    pin_cfg.Pinnum    = PINSEL_PIN_6; 
    pin_cfg.Funcnum   = PINSEL_FUNC_0;  //Pin config as gpio
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    PINSEL_ConfigPin(&pin_cfg);

    GPIO_SetDir(PINSEL_PORT_0 , PIN_6 , INPUT);
    GPIO_IntCmd(PINSEL_PORT_0 , PIN_6 , RISING);
    NVIC_EnableIRQ(EINT3_IRQn);
    
}

void Eint3_IRQHandler(void){
    GPIO_ClearInt(PINSEL_PORT_0 , PIN_6);
    N = (N == 600) ? 2 : N+1;
}

void ADC_IRQHandler (void){

    data = 0; /* Last input analog data is delete */
    NVIC_Disable(ADC_IRQn); /*Nvic disable allows to get data of N sampling in 1 interruption*/
    for(uint16_t i=0; i<N ; i++){
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

void ADC_IRQHandler (void){
        static uint32_t samples_num = 0;
        if (ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_BURST)) /* Check overrun bit*/
        {
            ADC_DeInit(LPC_ADC); /* Close ADC */
            configure_timer_and_match();
            NVIC_Disable(ADC_IRQn);
            
        }
        else if (samples_num < N)
            {
            data += ADC_ChannelGetData(LPC_ADC , ADC_CHANNEL_0); /* DONE is negated when ADDR is read, flag is cleared */
            samples_num++;
            }
            else{
                promedio=(data/N);
                samples_num = 0;
                data=0;
            }
}

void TIM0_IRQHandler (void){
    static uint8_t edge_flag = 0; /*pin pull-up, first interruption will be a falling edge (high cycle)*/
    static uint8_t sampling_num = 0;
    if (TIM_GetIntStatus(LPC_TIM0 , TIM_CR0_INT))
    {               
        TIM_ClearIntPending(LPC_TIM0 , TIM_CR0_INT);
        if(edge_flag){
            count_low = TIM_GetCaptureValue(LPC_TIM0 , TIM_COUNTER_INCAP0); /* rising edge */
            sampling_num = (sampling_num < 10) ? sampling_num++ : 0; 
            duty_cycle[sampling_num] = count_high*100/(count_high + count_low); /* completed period */
        }       
        else{
            count_high = TIM_GetCaptureValue(LPC_TIM0 , TIM_COUNTER_INCAP0); /* falling edge */ 
        }
        edge_flag = ! edge_flag;        
    }
    if (TIM_GetIntStatus(LPC_TIM0 , TIM_MR0_INT))
    {
        TIM_ClearIntPending(LPC_TIM0 , TIM_MR0_INT);
        for (uint8_t i = 0; i < N; i++)
        {       
            promedio = duty_cycle[i] + promedio;
        }
        promedio = promedio/N;                 
    }

}


