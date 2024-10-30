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
#define P0_1 ((uint32_t)(1<<1))
#define PORT_2 ((uint32_t)0xFF)
#define RISING 0
#define INPUT 0
#define MASK 1
#define TIME 20

uint8_t cont = 0;
uint8_t estado = 0;

void pin_cfg (void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Pinnum  = PINSEL_PORT_0;
    pin_cfg.Portnm  = PINSEL_PIN_1;
    pin_cfg.Funcnum = PINSEL_FUNC_0;
    pin_cfg.Pinmode = PINSEL_MODE_PULLDOWN;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;

    PINSEL_ConfigPin (&pin_cfg);

    // Configuración de P2.0 - P2.7 como salida
    for (int i = 0; i < 8; i++) {
        pin_cfg.Pinnum  = i;
        pin_cfg.Portnm  = PINSEL_PORT_2;
        pin_cfg.Funcnum = PINSEL_FUNC_0;
        PINSEL_ConfigPin(&pin_cfg);
    }


    FIO_SetMask (PINSEL_PORT_2 , PORT_2, MASK );

    GPIO_SetDir(PINSEL_PORT_0 , P0_1 , INPUT);
    GPIO_SetDir(PINSEL_PORT_0 , PORT_2 , OUTPUT);
    GPIO_IntCmd (PINSEL_PORT_0 , P0_1 , RISING);
    NVIC_Enable (EINT3_IRQn);
}

void EINT3_IRQHandler (void){
    cont++;
    NVIC_Disable (EINT3_IRQn);
    SYSTICK_InternalInit(TIME)
    SYSTYCK_IntCmd(ENABLE);
    SYSTYCK_Cmd(ENABLE);
}

void Systick_Handler(void){
    if(GPIO_ReadValue(PINSEL_PORT_0) & P0_1){
        if(estado<3){
            estado++;
        }
        else{
            estado = 0;
            SYSTYCK_IntCmd(DISABLE);
            SYSTYCK_Cmd(DISABLE);
            SysTick->VAL=0; // Reiniciar el valor del contador
            NVIC_Enable (EINT3_IRQn);
            GPIO_SetValue(PINSEL_PORT_2 , PORT_2 & Ascii());
        }
    }
}

uint8_t Ascii (void){

    return cont + 0x30;
}

int main (void){
    SystemInit();
    pin_cfg();
    while(1){

    }
    return 0;
}