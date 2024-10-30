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
#define RISING 0
#define OUTPUT 1
#define INPUT 1
#define PIN_04 ((uint32_t)(1<<4))
#define PIN_05 ((uint32_t)(1<<5))
#define P02    ((uint32_t) 0b110000)
#define MASK 1
#define TIME 10

uint32_t dato = 0b101010100001010101;
uint8_t i=0;

void pin_config (void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_2;
    pin_cfg.Pinnum    = PINSEL_PIN_4;
    pin_cfg.FuncNum   = PINSEL_FUNC_0;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin (&pin_cfg);
    pin_cfg.Pinnum   = PINSEL_PIN_5;
    PINSEL_ConfigPin (&pin_cfg);

/*MASCARA*/
    #define P02    ((uint32_t) 0b110000)
    LPC_GPIO->FIOMASK2 |= ~P02;

    GPIO_SetDir(PINSEL_PORT_2 , PIN_04 , INPUT);
    GPIO_SetDir(PINSEL_PORT_2 , PIN_05 , OUTPUT);

    GPIO_IntCmd(PINSEL_PORT_2 , PIN_04 , RISING);
    NVIC_EnableIRQn (EINT3_IRQn);

}

void EINT3_IRQHandler (void){
    GPIO_ClearInt (PINSEL_PORT_2 , P02);
    SYSTICK_IntCmd(DISABLE);
    SYSTICK_Cmd(DISABLE);
    i=0;
    SYSTICK->VAL = 0;
    SYSTICK_IntCmd (ENABLE);
    SYSTICK_Cmd (ENABLE);
    

}
uint32_t dato = 0b101010100001010101;
i = (i+1) % 18;
if ((dato >> i) & 1){

}


void SysTick_Handler (void){
    SYSTICK_ClearCounterFlag();
    i = (i+1) % 18;
        if((dato >> i) & 1){
            GPIO_SetValue (PINSEL_PORT_2 , PIN_05)
        }
        else {
            GPIO_ClearValue (PINSEL_PORT_2 , PIN_05)
        }       
}
    
int main (void){
    SystemInit();
    pin_config();
    SYSTICK_InternalInit (TIME);
    SYSTICK_IntCmd (ENABLE);
    SYSTICK_Cmd (ENABLE);
    NVIC_SetPriority (EINT3_IRQn , 0);
    while (1){

    }
    return 0;
}