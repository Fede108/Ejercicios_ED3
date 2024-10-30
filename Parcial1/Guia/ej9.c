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
#define LETRA ((uint32_t)(0xFF<<16))
#define A     ((uint32_t)(0x41<<16))
#define a     ((uint32_t)(0x61<<16))
#define OUTPUT 1
uint8_t mayuscula = 0; 


void pin_config(void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_2;
    pin_cfg.Pinnum    = PINSEL_PIN_10 ; 
    pin_cfg.FuncNum   = PINSEL_FUNC_1;
    pin_cfg.Pinmode   = PINSEL_MODE_PULLDOWN;  
    pin_cfg.OpenDrain = PINSEL_MODE_NORMAL;  

    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_11;
    PINSEL_ConfigPin (&pin_cfg);

    pin_cfg.Portnum = PINSEL_PORT_1;
    pin_cfg.Pinnum  = PINSEL_PIN_16;
    PINSEL_ConfigPin(&pin_cfg);

    pin_cfg.Pinnum  = PINSEL_PIN_17;
    PINSEL_ConfigPin(&pin_cfg);

    pin_cfg.Pinnum  = PINSEL_PIN_18;
    PINSEL_ConfigPin(&pin_cfg); 



    GPIO_SetDir(PINSEL_PORT_1 , LETRA , OUTPUT);
}

void EINT_config(void){

    EXTI_InitTypeDef exti_cfg;
    exti_cfg.EXTI_Line = EXTI_EINT0;
    exti_cfg.EXTI_Mode = EXTI_MODE_LEVEL_SENSITIVE;
    exti_cfg.EXTI_Polarity = EXTI_POLARITY_HIGH_ACTIVE_OR_RISING_EDGE;
    EXTI_Config(&exti_cfg);

    exti_cfg.EXTI_Line = EXTI_EINT1;
    exti_cfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
    EXTI_Config(&exti_cfg);
   

}

void EINT0_IRQHandler(void){
    EXTI_ClearEXTIFlag(EXTI_EINT0);
    mayuscula=1;

}

void EINT1_IRQHandler(void){
    EXTI_ClearEXTIFlag(EXTI_EINT1);
    if(mayuscula==1)
    {
    GPIO_ClearValue(PINSEL_PORT_1 , LETRA);
    GPIO_SetValue(PINSEL_PORT_1 , A );
    mayuscula=0;
    }
    else
    {
    GPIO_ClearValue(PINSEL_PORT_1 , LETRA);
    GPIO_SetValue(PINSEL_PORT_1 , a);
    }
}

int main(void){
    SystemInit();
    pin_config();
    EINT_config();
    NVIC_SetPriority (EINT0_IRQn , 0);
    NVIC_SetPriority (EINT1_IRQn , 1);
    NVIC_Enable(EINT1_IRQn);
    NVIC_Enable(EINT0_IRQn);
    while (1){

    }
    return 0;
}


