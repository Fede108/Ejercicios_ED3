  // cuenta = (cuenta + 1) % 10;  // Incrementar la cuenta de 0 a 9

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
#define OUTPUT 1
#define DISPLAY ((uint32_t)0x3F)
#define TIME_1 1
#define TIME_1000 1000
#define LENGTH 10
uint8_t cont=0;
uint8_t estado=0;
uint8_t velocidad=0;
uint8_t numDisplay[LENGTH]= {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};


void pin_cfg (void){
/*pines de eint*/
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Pinnum  = PINSEL_PORT_2;
    pin_cfg.Portnm  = PINSEL_PIN_10;
    pin_cfg.Funcnum = PINSEL_FUNC_1;
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLDOWN;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin (&pin_cfg);

    pin_cfg.Pinnum  = PINSEL_PORT_11;
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
    PINSEL_ConfigPin (&pin_cfg);

    pin_cfg.Pinnum  = PINSEL_PORT_12;    
    PINSEL_ConfigPin (&pin_cfg);
/*pines de salida*/
    pin_cfg.Pinnum  = PINSEL_PORT_0;
    pin_cfg.Portnm  = PINSEL_PIN_0;
    pin_cfg.Funcnum = PINSEL_FUNC_0;
    PINSEL_ConfigPin (&pin_cfg);
    pin_cfg.Pinnum  = PINSEL_PORT_1;
    PINSEL_ConfigPin (&pin_cfg);
    pin_cfg.Pinnum  = PINSEL_PORT_2;
    PINSEL_ConfigPin (&pin_cfg);
    pin_cfg.Pinnum  = PINSEL_PORT_3;
    PINSEL_ConfigPin (&pin_cfg);
    pin_cfg.Pinnum  = PINSEL_PORT_4;
    PINSEL_ConfigPin (&pin_cfg);
    pin_cfg.Pinnum  = PINSEL_PORT_5;
    PINSEL_ConfigPin (&pin_cfg);
    pin_cfg.Pinnum  = PINSEL_PORT_6;
    PINSEL_ConfigPin (&pin_cfg);
    pin_cfg.Pinnum  = PINSEL_PORT_7;
    PINSEL_ConfigPin (&pin_cfg);

    GPIO_SetDir(PINSEL_PORT_0 , DISPLAY , OUTPUT);

    NVIC_Enable (EINT3_IRQn);
}

void eint_config (void){
    EXTI_InitTypeDef ext_cfg;
    ext_cfg.EXTI_Line  = EXTI_EINT0;
    ext_cfg.EXTI_Mode  = EXTI_MODE_LEVEL_SENSITIVE;
    ext_cfg.EXTI_Polarity = EXTI_HIGH_ACTIVE_OR_RISING_EDGE;
    EXTI_Config(&ext_cfg);
    
    ext_cfg.EXTI_Line  = EXTI_EINT1;
    ext_cfg.EXTI_Mode  = EXTI_MODE_EDGE_SENSITIVE;
    ext_cfg.EXTI_Polarity = EXTI_LOW_ACTIVE_OR_FALLING_EDGE;
    EXTI_Config(&ext_cfg);

    ext_cfg.EXTI_Line  = EXTI_EINT2;
    EXTI_Config(&ext_cfg);
}

void EINT0_IRQHandler (void){
    cont=0;
    EXTI_ClearExtiFlag(EINT0);
}

void EINT1_IRQHandler (void){
    EXTI_ClearExtiFlag(EINT1);
    estado = !estado;
    if(estado==1){
        SYSTICK_IntCmd(ENABLE);
        SYSTICK_Cmd(ENABLE);
    }
    else{
        SYSTICK_IntCmd(DISABLE);
        SYSTICK_Cmd(DISABLE);
    }
}    

void EINT2_IRQHandler (void){
    EXTI_ClearExtiFlag(EINT2);
    velocidad = !velocidad;
    SYSTICK_IntCmd(DISABLE);
    SYSTICK_Cmd(DISABLE);

    if(velocidad==1){
        SYSTICK_InternalInit(TIME_1000);
    }    
    else{
        SYSTICK_InternalInit(TIME_1);
    }

    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);
}    

void Systick_Handler (void){
    GPIO_ClearValue (PINSEL_PORT_0 , DISPLAY);
    cont = (cont + 1) % 10;  // Incrementar la cuenta de 0 a 9
    GPIO_SetValue (PINSEL_PORT_0 , DISPLAY & numDisplay[cont]);
    
}

int main (void){
    SystemInit();
    pin_cfg();
    eint_config();
    SYSTICK_InternalInit(TIME_1);
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    NVIC_Enable (EINT0_IRQn);
    NVIC_Enable (EINT1_IRQn);
    NVIC_Enable (EINT2_IRQn);
    NVIC_SetPriority (EINT0_IRQn , 0);
    NVIC_SetPriority (EINT1_IRQn , 1);
    NVIC_SetPriority (EINT2_IRQn , 2);
    NVIC_SetPriority (Systick_IRQn , 3);

    while (1){

    }
    return 0;
}