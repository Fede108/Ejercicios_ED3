/**
 * @file 2023_ej1.c
 * @author your name (you@domain.com)
 * @brief Programar el microcontrolador LPC1769 en un código de lenguaje C para que, utilizando un timer
         y un pin de capture de esta placa sea posible demodular una señal PWM que ingresa por dicho pin 
         (calcular el ciclo de trabajo y el periodo) y sacar una tensión continua proporcional al ciclo de 
         trabajo a través del DAC de rango dinámico 0-2V con un rate de actualización de 0,5s del promedio 
         de las últimos diez valores obtenidos en la captura.
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
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_libcfg_default.h"

#define N 10

uint32_t duty_cycle[N] = {};
volatile uint32_t count_high, count_low;
uint32_t promedio   = 0;
uint32_t dac_signal = 0;

void configure_dac (void){
    PINSEL_CFG_Type PinCfg;
    // Configure pin P0.26 as DAC output
    PinCfg.Funcnum = PINSEL_FUNC_2;           // DAC function
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL; // Disable open drain
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;   // Pull-up 
    PinCfg.Pinnum = PINSEL_PIN_26;            // Pin number 26
    PinCfg.Portnum = PINSEL_PORT_0;           // Port number 0
    PINSEL_ConfigPin(&PinCfg);
    DAC_Init (LPC_DAC);
}

void configure_timer (void){
    TIM_TIMERCFG_Type tim_cfg;
    tim_cfg.PrescaleOption = TIM_PRESCALE_USVAL;                    
    tim_cfg.PrescaleValue  = 100;
    TIM_Init(LPC_TIM0 , TIM_TIMER_MODE , &tim_cfg);
}

void configure_match_capture (void){
    // P1.26 as CAP0.0
    PINSEL_CFG_Type pinCfg;
    pinCfg.Portnum   = PINSEL_PORT_1;
    pinCfg.Pinnum    = PINSEL_PIN_26;
    pinCfg.Funcnum   = PINSEL_FUNC_3;
    pinCfg.Pinmode   = PINSEL_PINMODE_PULLUP;
    pinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pinCfg);

    TIM_MATCHCFG_Type match_cfg;
    match_cfg.MatchChannel       = 0;
    match_cfg.IntOnMatch         = ENABLE;
    match_cfg.ResetOnMatch       = ENABLE;
    match_cfg.StopOnMatch        = DISABLE;
    match_cfg.MatchValue         = 5000;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
    TIM_ConfigMatch(LPC_TIM0, &match_cfg);

    TIM_CAPTURECFG_Type capture_cfg;
    capture_cfg.CaptureChannel = 0;
    capture_cfg.FallingEdge    = ENABLE;
    capture_cfg.RisingEdge     = ENABLE;
    capture_cfg.IntOnCaption   = ENABLE;
    TIM_ConfigCapture(LPC_TIM0 , &capture_cfg);

    NVIC_EnableIRQ(TIMER0_IRQn);
    TIM_Cmd(LPC_TIM0 , ENABLE);        
}

void TIM0_IRQHandler (void){
    static uint8_t edge_flag = 0; /* pin pull-up, first interruption will be a falling edge (high cycle)*/
    static uint8_t sampling_num = 0;

    if (TIM_GetIntStatus(LPC_TIM0 , TIM_CR0_INT))
    {               
        TIM_ClearIntPending(LPC_TIM0 , TIM_CR0_INT);
        if(edge_flag){
            count_low = TIM_GetCaptureValue(LPC_TIM0 , TIM_COUNTER_INCAP0); /* rising edge */
            sampling_num = (sampling_num < 10) ? sampling_num + 1 : 0; 
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
        promedio = 0;
        for (uint8_t i = 0; i < N; i++)
        {       
            promedio = duty_cycle[i] + promedio;
        }
        promedio   = promedio/N; 
        /* vref = 3.3v --> 10 bits = 1023 
         rango dinámico 0-2V => 2*1023/3.3 = 620 */
        dac_signal = (promedio*620)/100;
        DAC_UpdateValue(LPC_DAC , dac_signal);
    }
}

