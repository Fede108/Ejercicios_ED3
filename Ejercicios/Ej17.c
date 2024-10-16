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


void configure_timer (void){
    TIM_TIMERCFG_Type timer_cfg;
    timer_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    timer_cfg.PrescaleValue  = 100; 
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timer_cfg);
}

void configure_match (void){
    TIM_MATCHCFG_Type match_cfg;
    match_cfg.IntOnMatch         = DISABLE;
    match_cfg.MatchChannel       = 0;
    match_cfg.MatchValue         = MATCH_VALUE;
    match_cfg.ResetOnMatch       = ENABLE;
    match_cfg.StopOnMatch        = DISABLE;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
    TIM_ConfigMatch(LPC_TIM0, &match_cfg);
    TIM_Cmd(LPC_TIM0, ENABLE);
}

void configure_pin (void){
//as mat0.0
    PINSEL_CFG_Type pin_cfg;
    pin_cfg_struct.Portnum   = PINSEL_PORT_0;
    pin_cfg_struct.Pinnum    = PINSEL_PIN_0;
    pin_cfg_struct.Funcnum   = PINSEL_FUNC_0;
    pin_cfg_struct.OpenDrain = PINSEL_PINMODE_NORMAL;
    pin_cfg_struct.Pinmode   = PINSEL_PINMODE_PULLUP; 
    PINSEL_ConfigPin(&pin_cfg_struct);
}

void configure_eint(void){
    EXTI_InitTypeDef exti_cfg;
    exti_cfg.EXTI_Line     = EXTI_EINT2;
    exti_cfg.EXTI_Mode     = EXTI_MODE_EDGE_SENSITIVE;
    exti_cfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
    EXTI_Config(&exti_cfg);
    NVIC_Enable(EINT2_IRQn);
}

void EINT2_IRQHandler(void){
    EXTI_ClearEXTIFlag(EXTI_EINT2);
    match_value = (match_value == MAX_MATCH_VALUE) ? 1 : match_value * 2;
    TIM_UpdateMatchValue(LPC_TIM0, 0, match_value);
}

int main (void){
    System_init()
    configure_timer();
    configure_pin();
    configure_match();
    configure_eint();

    while (1){
        
    }

    return 0;
}
