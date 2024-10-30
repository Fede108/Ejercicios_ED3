
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
#define SWITCHES ((uint32_t) 0x0F)
#define INPUT 0
#define FALLING 1
uint8_t Array[10];
uint8_t priority=0;

void pin_cfg (void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Pinnum  = PINSEL_PORT_0;
    pin_cfg.Portnm  = PINSEL_PIN_0;
    pin_cfg.Funcnum = PINSEL_FUNC_0;
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin (&pin_cfg);

    pin_cfg.Pinnum  = PINSEL_PIN_1;
    PINSEL_ConfigPin (&pin_cfg);
    pin_cfg.Pinnum  = PINSEL_PIN_2;
    PINSEL_ConfigPin (&pin_cfg);
    pin_cfg.Pinnum  = PINSEL_PIN_3;
    PINSEL_ConfigPin (&pin_cfg);

    GPIO_SetDir(PINSEL_PORT_0 , SWITCHES , INPUT );
    GPIO_IntCmd(PINSEL_PORT_0 , SWITCHES , FALLING);
    NVIC_Enable(EINT3_IRQn);

}

void EINT3_Handler (void)
{
        for (uint8_t i=9 , i>0, i--){
            Array[i]=Array[i-1];
    
        }
        for (uint8_t i=0 , i<9, i++){
            Array[i+1]=Array[i];
        }
        Array[0] = (GPIO_ReadValue (PINSEL_PORT_0) & SWITCHES);

        if (cont==200){
            priority++;
            cont=0;
            NVIC_SetPriority (EINT3_IRQn , priority);
        }

        if(priority==31){
            NVIC_Disable (EINT3_IRQn);
        }
}
        
int main (void){
    SystemInit();
    pin_cfg();
    NVIC_SetPriority(EINT3_IRQn , 0x00)
    while(1){
        

    }
    return 0;
    
}