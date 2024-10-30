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
#define INPUT 0
#define RISING 0
#define FALLING 1
#define P_2 ((uint32_t) 0xFF)
uint8_t indice = 0;
uint8_t Dato[16];

void pin_config(void){
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Pinnum  = PINSEL_PORT_2;
    pin_cfg.Portnm  = PINSEL_PIN_0;
    pin_cfg.Funcnum = PINSEL_FUNC_0;
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin (&pin_cfg);

    for (int i = 1; i < 8; i++) {
        pin_cfg.Pinnum  = i;
      
        PINSEL_ConfigPin(&pin_cfg);
    }

    GPIO_SetDir (PINSEL_PORT_2 , P_2 , INPUT);
    GPIO_IntCmd (PINSEL_PORT_2 , P_2 , RISING);
    GPIO_IntCmd (PINSEL_PORT_2 , P_2 , FALLING);
    NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQHandler (void) {
    Dato [indice] = GPIO_ReadValue (PINSEL_PORT_2) & P_2;
    indice = (indice + 1) % 16
    GPIO_ClearInt (PINSEL_PORT_2 , P_2)
}

int main (void){
    SystemInit();
    pin_config();
    while (1){

    }
    return 0;
}

