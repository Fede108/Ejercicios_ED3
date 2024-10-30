#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include <cr_section_macros.h> /* The cr_section_macros is specific to the MCUXpresso delivered toolchain */
#endif
// TODO: insert other include files here
#include "lpc17xx_gpio.h";
#include "lpc17xx_pinsel.h";

/************************************************************************* */


#define A ((uint32_t)(1<<0))
#define B ((uint32_t)(1<<1))
#define C ((uint32_t)(1<<2))
#define D ((uint32_t)(1<<3))
#define E ((uint32_t)(1<<4))
#define F ((uint32_t)(1<<5))
#define G ((uint32_t)(1<<6))
#define H ((uint32_t)(1<<7))
#define LENGTH 10
#define DELAY 1000
#define OUTPUT 1
uint8_t i=0;
uint32_t clean=255;

uint32_t numDisplay[LENGTH]= {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};

void config_gpio(void){

    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   =  PINSEL_PORT_2;
    pin_cfg.Pinnum    =  PINSEL_PIN_0;
    pin_cfg.FuncNum   =  PINSEL_FUNC_0;
    pin_cfg.Pinmode   =  PINSEL_PINMODE_PULLUP;
    pin_cfg.OpenDrain =  PINSEL_PINMODE_NORMAL; 
    
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

    GPIO_SetDir(PINSEL_PORT_2 , A | B | C | D | E | F | G | H , OUTPUT );

}

void config_systick (void) {
        SYSTICK_InternalInit (DELAY);

}

void Sytick_Handler (void) {
    SYSTICK_ClearCounterFlag();
    if(i<LENGTH)
    {   
        GPIO_ClearValue(PINSEL_PORT_2, clean); 
        GPIO_SetValue (PINSEL_PORT_2 , numDisplay[i]);
        i++;
    }

   else
    {   
        i=0;
        GPIO_ClearValue(PINSEL_PORT_2, clean);
        GPIO_SetValue (PINSEL_PORT_2 , numDisplay[i]);
    }   
}
 
int main (void) {
    SystemInit();
    config_gpio();
    config_systick();
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    while(TRUE){

    }
    return 0;
}