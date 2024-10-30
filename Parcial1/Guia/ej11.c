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

#define TIME 1
#define OUTPUT 1
#define PWM    ((uint32_t)(1<<20))

uint8_t cont=0;
uint8_t duty_cycle=1;

void pin_config (void){
    PINSEL_CFG_Type pin_cfg;

    pin_cfg.Portnum   = PINSEL_PORT_2;
    pin_cfg.Pinnum    = PINSEL_PIN_10;
    pin_cfg.FuncNum   = PINSEL_FUNC_1;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pin_cfg);

    pin_cfg.Portnum   = PINSEL_PORT_0;
    pin_cfg.Pinnum    = PINSEL_PIN_22;
    pin_cfg.FuncNum   = PINSEL_FUNC_0;
    PINSEL_ConfigPin(&pin_cfg);

    GPIO_SetDir(PINSEL_PORT_0 , PWM , OUTPUT);

}

void eint_config (void){
    EXTI_InitTypeDef ext_cfg;
    ext_cfg.EXTI_Line  = EXTI_EINT0;
    ext_cfg.EXTI_Mode  = EXTI_MODE_EDGE_SENSITIVE;
    ext_cfg.EXTI_Polarity = EXTI_HIGH_ACTIVE_OR_RISING_EDGE;
    EXTI_Config(&ext_cfg);
}

void systick_config (void){
    SYSTICK_InternalInit(TIME);
}

void Systick_Handler (void){
    SYSTICK_ClearCounterFlag();
    
    if(cont<=duty_cycle)
    {
        cont++;
        GPIO_SetValue (PINSEL_PORT_0 , PWM);
    }
    if(cont<10)
    {
        GPIO_ClearValue (PINSEL_PORT_0 , PWM);
        cont++;  
    }
    else
    {
            cont=0;
    }
    
}

void EINT0_IRQHandler (void){
    EXTI_ClearEXTIFlag(EXTI_EINT0);

    if(duty_cycle<10){
        duty_cycle++;
    }
    else{
      duty_cycle=1;
    }

   
}

int main(void){
    SystemInit();
    pin_config();
    eint_config();
    systick_config();
    SYSTICK_IntCmd()
    SYSTICK_Cmd();
    NVIC_SetPriority(EINT0_IRQn , 0);
    NVIC_SetPriority(SysTick_IRQn , 1);
    NVIC_Enable(EINT0_IRQn);

    while(1){

    }
    return 0;
}

