/**
 * @file Ej19.c
 * @author your name (you@domain.com)
 * @brief Utilizando el modo Capture, escribir un código en C para que guarde en una
        variable ,llamada "shooter", el tiempo (en milisegundos) que le lleva a una persona
        presionar dos pulsadores con un único dedo.
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

#define INPUT 0
#define FALLING 1
#define PULSADOR_1 ((uint32_t)(1<<25))

//timer0 
void configure_timer (void){
    TIM_TIMERCFG_Type timer_cfg;
    timer_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    timer_cfg.PrescaleValue  = 1000; //Clock 100Mhz. Time resolution 1ms 
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timer_cfg);
    NVIC_EnableIRQ(TIMER0_IRQn);    
}

//register capture when an event in CAP0.0 (timer0 , capture register0) occurs
void configure_capture (void){
    TIM_CAPTURECFG_Type capture_cfg;
    capture_cfg.CaptureChannel = 0; //Capture reg 0 is loaded with the value of TC when there is an event on CAPn.0
    capture_cfg.FallingEdge    = ENABLE;
    capture_cfg.IntOnCaption   = ENABLE;
    capture_cfg.RisingEdge     = DISABLE;
    TIM_ConfigCapture(LPC_TIM0 , &capture_cfg); 
}

void configure_pin (void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_1;
    pin_cfg.Pinnum    = PINSEL_PIN_26; 
    pin_cfg.Funcnum   = PINSEL_FUNC_3;  //Pin config as CAP0.0
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLUP;
    PINSEL_ConfigPin(&pin_cfg);

    pin_cfg.Portnum   = PINSEL_PORT_1;
    pin_cfg.Pinnum    = PINSEL_PIN_25;
    pin_cfg.Funcnum   = PINSEL_FUNC_0;
    PINSEL_ConfigPin(&pin_cfg);
    
    GPIO_SetDir(PINSEL_PORT_1 , PULSADOR_1 , INPUT);
    GPIO_IntCmd(PINSEL_PORT_1 , PULSADOR_1 , FALLING);
    NVIC_EnableIRQ(EINT3_IRQn);
}

void TIMER0_IRQHandler (void){
    TIM_ClearIntPending(LPC_TIM0 , TIM_CR0_INT);
    uint32_t capturedValue = TIM_GetCaptureValue(LPC_TIM0, TIM_COUNTER_INCAP0); //counter value in ms
    TIM_Cmd(LPC_TIM0, DISABLE);
    TIM_ResetCounter(LPC_TIM0);
}

void EINT3_IRQHandler (void){
    GPIO_ClearInt(PINSEL_PORT_1 , PULSADOR_1);
    TIM_Cmd(LPC_TIM0, ENABLE);
}

int main (void){
    SystemInit();
    configure_timer();
    configure_capture();
    configure_pin();
    while (1){

    }
    return 0;
}


