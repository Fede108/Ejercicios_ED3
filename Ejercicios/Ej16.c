/**
 * @file Ej16.c
 * @author your name (you@domain.com)
 * @brief Escribir un programa para que por cada presión de un pulsador, la frecuencia de
        parpadeo de un led disminuya a la mitad debido a la modificación del pre-escaler
        del Timer 2. El pulsador debe producir una interrupción por EINT1 con flanco
        descendente
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

#define BLINK_TIME 100
#define BUTTON ((uint32_t)(1<<0))
#define OUTPUT 1
#define INPUT 0
#define FALLING 1
#define MAX_PR_VALUE ((uint32_t)0xFFFFFFFF)
uint32_t PR_VALUE=1;

/* Prototype Functions */
void configure_timer_and_match(void);
void configure_port(void);

void configure_timer(void){

    // Configure Timer2 in microsecond mode with a prescaler
    TIM_TIMERCFG_Type timer_struct;
    timer_struct.PrescaleOption = TIM_PRESCALE_USVAL;
    timer_struct.PrescaleValue  = PR_VALUE; //Every PR value the timer will increment by 1
    TIM_Init(LPC_TIM2, TIM_TIMER_MODE , &timer_struct); //Resetea el TC y PC. Y habilita el temporizador/contador.
}

void configure_match (void){
    //Configure match channel 
    TIM_MATCHCFG_Type match_struct;
    match_struct.MatchChannel       = 0;
    match_struct.MatchValue         = BLINK_TIME;
    match_struct.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
    match_struct.ResetOnMatch       = ENABLE;
    match_struct.IntOnMatch         = DISABLE;
    match_struct.StopOnMatch        = DISABLE;
    TIM_ConfigMatch(LPC_TIM2, &match_struct);
    TIM_Cmd(LPC_TIM2, ENABLE);
}

void configure_port(void){
    //configure pin as gpio 
    PINSEL_CFG_Type pin_cfg_struct;
    pin_cfg_struct.Portnum   = PINSEL_PORT_0;
    pin_cfg_struct.Pinnum    = PINSEL_PIN_0;
    pin_cfg_struct.Funcnum   = PINSEL_FUNC_0;
    pin_cfg_struct.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg_struct.Pinmode   = PINSEL_PINMODE_PULLUP; 
    PINSEL_ConfigPin(&pin_cfg_struct);

    //configure pin as MAT2.0 output
    pin_cfg_struct.Portnum   = PINSEL_PORT_4;
    pin_cfg_struct.Pinnum    = PINSEL_PIN_28;
    pin_cfg_struct.Funcnum   = PINSEL_FUNC_2;
    PINSEL_ConfigPin(&pin_cfg_struct);

    GPIO_SetDir(PINSEL_PORT_0 , BUTTON , INPUT);
    GPIO_IntCmd(PINSEL_PORT_0 , BUTTON , FALLING);
    NVIC_EnableIRQ(EINT3_IRQn);
}


void EINT3_Handler(void){
    GPIO_ClearInt(PINSEL_PORT_0, BUTTON);
    if(PR_VALUE < MAX_PR_VALUE){
        PR_VALUE=PR_VALUE*2;
    }
    else{
        PR_VALUE=1;
    }
    configure_timer();
}

int main(void)
{
    SystemInit(); // Initialize system clock
    configure_port();            // Configure GPIO
    configure_timer();
    configure_match(); // Configure timer and match channels

    while (TRUE)
    {
        // Infinite loop
    }

    return 0; // This never should be reached
}


