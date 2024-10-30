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

#define DATA ((uint32_t)0xFF)
#define INPUT 0
#define OUTPUT 1
#define LENGTH 8
#define TRUE   1
#define DELAY 1000

int8_t Array[LENGTH];
uint8_t i=0;
int8_t promedio=0;


void config_gpio(void){

    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   = PINSEL_PORT_1;
    pin_cfg.Pinnum    = PINSEL_PIN_0;
    pin_cfg.FuncNum   = PINSEL_FUNC_0;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLUP;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;

    PINSEL_ConfigPin(&pin_cfg);
    
    pin_cfg.Pinnum = PINSEL_PIN_1;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_2;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_3;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_4;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_5;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_6;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_7;
    PINSEL_ConfigPin(&pin_cfg);

    pin_cfg.Portnum = PINSEL_PORT_0;
    pin_cfg.Pinnum = PINSEL_PIN_0;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_1;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_2;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_3;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_4;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_5;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_6;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_7;
    PINSEL_ConfigPin(&pin_cfg);

    GPIO_SetDir(PINSEL_PORT_0 , DATA , INPUT );
    GPIO_SetDir(PINSEL_PORT_1 , DATA , OUTPUT );
}

void config_systick(void){
    SYSTICK_InternalInit(DELAY);
}


void Systick_Handler (void){

    if(i<LENGTH)
    {
    Array[i]=(GPIO_ReadValue(PINSEL_PORT_0) &DATA);
    promedio_movil();
    i++;
    }
    else{
    i=0;
    Array[i]=(GPIO_ReadValue(PINSEL_PORT_0) &DATA);
    promedio_movil(); 
    }

}

void promedio_movil(void){
    promedio=0;
    for(uint8_t y=0, y<LENGTH, y++ ){
        promedio=Array[y]+promedio;
    }
    promedio=promedio/LENGTH;
}

int main(void){
    SystemInit();
    config_systick();
    config_gpio();
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    while(TRUE){


    }
    return 0;
}