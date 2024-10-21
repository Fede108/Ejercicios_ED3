/**
 * @file Ej18.c
 * @author your name (you@domain.com)
 * @brief Utilizando los 4 registros match del Timer 0 y las salidas P0.0, P0.1, P0.2 y P0.3,
        realizar un programa en C que permita obtener las formas de ondas adjuntas,
        donde los pulsos en alto tienen una duración de 5 mseg. Un pulsador conectado a
        la entrada EINT3, permitirá elegir entre las dos secuencias mediante una rutina de
        servicio a la interrupción. La prioridad de la interrupción del Timer tiene que ser
        mayor que la del pulsador. Estas formas de ondas son muy útiles para controlar un
        motor paso a paso.
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
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_nvic.h"

#define OUTPUT 1
#define A_0 ((uint32_t)0x00)
#define B_0 ((uint32_t)0x01)
#define A_1 ((uint32_t)0x02)
#define B_1 ((uint32_t)0x03)
uint8_t pulsador = 0;

void configure_timer (void){
    TIM_TIMERCFG_Type timer_cfg;
    timer_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    timer_cfg.PrescaleValue  = 1000; //Clock 100Mhz. Time resolution 1us*1000=1ms 
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timer_cfg);
}

void configure_match (void){
    TIM_MATCHCFG_Type match_cfg;
    match_cfg.StopOnMatch        = DISABLE;
    match_cfg.ResetOnMatch       = DISABLE;
    match_cfg.IntOnMatch         = ENABLE;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
    match_cfg.MatchChannel       = 0;
    match_cfg.MatchValue         = 5;
    TIM_ConfigMatch(LPC_TIM0, &match_cfg);

    match_cfg.MatchChannel = 1;
    match_cfg.MatchValue   = 10;
    TIM_ConfigMatch(LPC_TIM0, &match_cfg);

    match_cfg.MatchChannel = 2;
    match_cfg.MatchValue   = 15;
    TIM_ConfigMatch(LPC_TIM0, &match_cfg);

    match_cfg.MatchChannel = 3;
    match_cfg.MatchValue   = 20;
    match_cfg.ResetOnMatch = ENABLE;
    TIM_ConfigMatch(LPC_TIM0, &match_cfg);

    TIM_Cmd(LPC_TIM0, ENABLE);

    NVIC_SetPriority(TIMER0_IRQn , 0);
    NVIC_Enable(TIMER0_IRQn);
}

void configure_gpio(void){
    PINSEL_CFG_Type pin_cfg_struct;
    pin_cfg.Portnum   = PINSEL_PORT_1;
    pin_cfg.Pinnum    = PINSEL_PIN_20; 
    pin_cfg.Funcnum   = PINSEL_FUNC_2;  //Pin config as eint3
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    PINSEL_ConfigPin(&pin_cfg_struct);

    pin_cfg_struct.Portnum   = PINSEL_PORT_0;
    pin_cfg_struct.Pinnum    = PINSEL_PIN_0;
    pin_cfg_struct.Funcnum   = PINSEL_FUNC_0;
    pin_cfg_struct.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg_struct.Pinmode   = PINSEL_PINMODE_PULLUP; 
    PINSEL_ConfigPin(&pin_cfg_struct);

    pin_cfg_struct.Pinnum    = PINSEL_PIN_1;
    PINSEL_ConfigPin(&pin_cfg_struct);

    pin_cfg_struct.Pinnum    = PINSEL_PIN_2;
    PINSEL_ConfigPin(&pin_cfg_struct);

    pin_cfg_struct.Pinnum    = PINSEL_PIN_3;
    PINSEL_ConfigPin(&pin_cfg_struct);

    
    GPIO_SetDir (PINSEL_PORT_0, A_0|A_1|B_0|B_1 , OUTPUT);
    

}

void configure_eint(void){
    EXTI_InitTypeDef exti_cfg;
    exti_cfg.EXTI_Line     = EXTI_EINT3;
    exti_cfg.EXTI_Mode     = EXTI_MODE_EDGE_SENSITIVE;
    exti_cfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
    EXTI_Config(&exti_cfg);
    EXTI_ClearEXTIFlag(EXTI_EINT3); 
    NVIC_SetPriority(EINT3_IRQn , 1);
    NVIC_Enable(EINT3_IRQn);
}

void EINT3_Handler (void){
    pulsador = !pulsador;
    GPIO_SetValue(PINSEL_PORT_0 , A_0); //Initial condition
    GPIO_ClearValue(PINSEL_PORT_0 , A_1|B_0|B_1 );
    EXTI_ClearEXTIFlag(EXTI_EINT3);
}

void TIMER0_IRQHandler(void){
    if(pulsador){
        first_sequence();
    }
    else{
        second_sequence();
    }
}

void first_sequence (void) {
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)) {
        TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
            GPIO_ClearValue(PINSEL_PORT_0, A_0);
            GPIO_SetValue  (PINSEL_PORT_0, B_0);
    }
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR1_INT)) {
        TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);
            GPIO_ClearValue(PINSEL_PORT_0, B_0);
            GPIO_SetValue  (PINSEL_PORT_0, A_1);
 
    }
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR2_INT)) {
        TIM_ClearIntPending(LPC_TIM0, TIM_MR2_INT);
            GPIO_ClearValue(PINSEL_PORT_0, A_1);
            GPIO_SetValue  (PINSEL_PORT_0, B_1);       
    }
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR3_INT)) {
        TIM_ClearIntPending(LPC_TIM0, TIM_MR3_INT);
            GPIO_ClearValue(PINSEL_PORT_0, B_1);
            GPIO_SetValue  (PINSEL_PORT_0, A_0);
    }
}

void second_sequence (void) {
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)) {
        TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
            GPIO_ClearValue(PINSEL_PORT_0, B_1);
            GPIO_SetValue  (PINSEL_PORT_0, B_0);
    }
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR1_INT)) {
        TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);
            GPIO_ClearValue(PINSEL_PORT_0, A_0);
            GPIO_SetValue  (PINSEL_PORT_0, A_1);
 
    }
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR2_INT)) {
        TIM_ClearIntPending(LPC_TIM0, TIM_MR2_INT);
            GPIO_ClearValue(PINSEL_PORT_0, B_0);
            GPIO_SetValue  (PINSEL_PORT_0, B_1);
        
    }
    if (TIM_GetIntStatus(LPC_TIM0, TIM_MR3_INT)) {
        TIM_ClearIntPending(LPC_TIM0, TIM_MR3_INT);
            GPIO_ClearValue(PINSEL_PORT_0, A_1);
            GPIO_SetValue  (PINSEL_PORT_0, A_0);
    }
}    
    
int main (void){
    SystemInit();
    configure_timer();
    configure_eint();
    configure_gpio();
    configure_match();
    while (1){

    }
    return 0;
}  




