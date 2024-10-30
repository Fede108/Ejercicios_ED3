#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include <cr_section_macros.h> /* The cr_section_macros is specific to the MCUXpresso delivered toolchain */
#endif
// TODO: insert other include files here
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

/************************************************************************* */

#define DELAY_1 25
#define DELAY_2 60


void pin_config(void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_2;
    pin_cfg.Pinnum    = PINSEL_PIN_11;
    pin_cfg.FuncNum   = PINSEL_FUNC_1;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLUP;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pin_cfg);
    
    pin_cfg.Pinnum  = PINSEL_PIN_12;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    PINSEL_ConfigPin(&pin_cfg);
}

void configure_external_interruption(void){
    EXTI_InitTypeDef exti_cfg;
    exti_cfg.EXTI_Line = EXTI_EINT1;
    exti_cfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE; 
    exti_cfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
    EXTI_Config(&exti_cfg);

   exti_cfg.EXTI_Line = EXTI_EINT2;
   exti_cfg.EXTI_polarity = EXTI_POLARITY_HIGH_ACTIVE_RISING_EDGE;
   EXTI_Config(&exti_cfg); 
}

void EINT1_IRQHandler(void){
    EXTI_ClearEXTIFlag(EXTI_EINT1);
    Systick_config_1();
}

void EINT2_IRQHandler(void){
    EXTI_ClearEXTIFlag(EXTI_EINT2);
    Systick_config_2();
}

void Systick_config_1(void){
    SYSTICK_InternalInit(DELAY_1);
}

void Systick_config_2(void){
    SYSTICK_InternalInit(DELAY_2);
}

int main(void){
    SystemInit();
    pin_config();
    configure_external_interruption();
    NVIC_SetPriority(EINT1_IRQn , 0);
    NVIC_SetPriority(EINT2_IRQn, 1);
    while(1){

    }
    return 0;
}