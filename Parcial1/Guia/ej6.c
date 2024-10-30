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

#define P2_0    ((uint32_t)(1<<0))        
#define P2_1    ((uint32_t)(1<<1)) 
#define P0_0    ((uint32_t)(1<<0))
#define RISING 0
#define FALLING 1
#define INPUT 0
#define OUTPUT 1
#define LENGTH 9
#define DELAY 100
uint8_t i = 0;
uint8_t Datos[LENGTH];

void config_gpio(void){
    PINSEL_CFG_Type pin_cfg;

    pin_cfg.Portnum   = PINSEL_PORT_2;
    pin_cfg.Pinnum    = PINSEL_PIN_0;
    pin_cfg.FuncNum   = PINSEL_FUNC_0;
    pin_cfg.Pinmode   = PINSEL_PINMODE_PULLDOWN;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pin_cfg);

    pin_cfg.Pinnum    = PINSEL_PIN_1;
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP
    PINSEL_ConfigPin(&pin_cfg);

    pin_cfg.Portnum   = PINSEL_PORT_0;
    pin_cfg.Pinnum    = PINSEL_PIN_0;
    PINSEL_ConfigPin(&pin_cfg);

    GPIO_SetDir(PINSEL_PORT_2, P2_0 | P2_1, INPUT);
    GPIO_SetDir(PINSEL_PORT_0, P0_0, OUTPUT);

    GPIO_IntCmd(PINSEL_PORT_2, P2_0 , RISING);
    GPIO_IntCmd(PINSEL_PORT_2, P2_1 , FALLING);
}

void config_systick(void){
    SYSTICK_InternalInit(DELAY);
}

void EINT3_IRQHandler(void){
    if(GPIO_GetIntStatus(PINSEL_PORT_2,P2_0,RISING)==ENABLE)
    {   
        primer_secuencia();
        GPIO_ClearInt (PINSEL_PORT_2, P2_0 ); /* Clear interrupt flag */
      
    }
    if(GPIO_GetIntStatus(PINSEL_PORT_2,P2_1,FALLING)==ENABLE)
    {
        segunda_secuencia();
        GPIO_ClearInt(PINSEL_PORT_2, P2_1);
    }
}



void Systick_Handler(void){

    SYSTICK_ClearCounterFlag();
    
    if(i<LENGTH)
    {
        if(Datos[i] & P0_0){
        GPIO_SetValue(PINSEL_PORT_0, P0_0 );
        }
        else {
        GPIO_ClearValue(PINSEL_PORT_0, P0_0);    
        }
    i++;
    }
    else
    {
        i=0;
        if(Datos[i] & P0_0){
        GPIO_SetValue(PINSEL_PORT_0, P0_0 );
        }
        else {
        GPIO_ClearValue(PINSEL_PORT_0, P0_0);    
        }
    }
}


void primer_secuencia(void){
    uint8_t temp[LENGTH] = {0,1,0,0,1,1,0,1,0} 
    for (uint8_t j = 0 , j < LENGTH , j++ ){
        Datos[j]=temp[j];
    }
}

void segunda_secuencia(void){
    uint8_t temp[LENGTH] = {0,1,1,1,0,0,1,1,0};
    for(uint_t j = 0 , j< LENGTH , j++){
        Datos[j]=temp[j];
    } 
    
}


int main (void) {
    SystemInit();
    config_gpio();
    config_systick();
    SYSTICK_IntCmd(ENABLE);
    SYSTICK_Cmd(ENABLE);

    NVIC_SetPriority(EINT3_IRQn, 0); /* Set the priority of the GPIO interrupt */
    NVIC_SetPriority(SYSTICK_IRQn, 1);
    while (TRUE){

    }
    return 0;
}