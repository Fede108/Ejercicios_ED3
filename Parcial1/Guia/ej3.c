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




#define BUTTON ((uint32_t)(1<<4))
#define P_0    ((uint32_t)(1<<0))
#define P_1    ((uint32_t)(1<<1))
#define P_2    ((uint32_t)(1<<2))
#define P_3    ((uint32_t)(1<<3))
#define DELAY 1000
int16_t acumulador=0;

void gpio_config(void){

    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Portnum   =  PINSEL_PORT_0;
    pin_cfg.Pinnum    =  PINSEL_PIN_0;
    pin_cfg.FuncNum   =  PINSEL_FUNC_0;
    pin_cfg.Pinmode   =  PINSEL_PINMODE_PULLUP;
    pin_cfg.OpenDrain =  PINSEL_PINMODE_NORMAL; 
    
    PINSEL_ConfigPin (&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_1;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_2;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_3;
    PINSEL_ConfigPin(&pin_cfg);
    pin_cfg.Pinnum = PINSEL_PIN_4;
    PINSEL_ConfigPin(&pin_cfg);
    
    GPIO_SetDir (PINSEL_PORT_0 , BUTTON , INPUT);
    GPIO_SetDirt (PINSEL_PORT_0, P_0 | P_1 | P_2 | P_3 , INPUT); 
}

void config_systick(void){
    SYSTICK_InternalInit(DELAY);
}

void Systick_Handler (void)
{
    Sytick_ClearCounterFlag();
    if(GPIO_ReadValue(PINSEL_PORT_0)& BUTTON)
    {
        acumulador = acumulador + (GPIO_ReadValue(PINSEL_PORT_0) & (P_0 | P_1 | P_2 | P_3 )); 
    }
    else {
        acumulador = acumulador - (GPIO_ReadValue(PINSEL_PORT_0) & (P_0 | P_1 | P_2 | P_3 ));
    }

}

int main(void){
    SystemInit();
    config_systick();
    gpio_config();
    SYSTICK_IntCmd();
    SYSTICK_Cmd();

    while(true){


    }
    return 0;
}