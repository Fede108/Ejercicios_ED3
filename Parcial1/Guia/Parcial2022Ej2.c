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
#define TIME 10
#define OUTPUT 1
uint8_t val=0;
uint32_t promedio=0;
uint8_t enable=0;
uint8_t values[8];

void pin_cfg (void){
/*pines gpio salida*/
     // Configurar P0.0 a P0.7 como GPIO
    PINSEL_CFG_Type pin_cfg;
    
    for (uint8_t i = 0; i < 8; i++) {
        pin_cfg.Portnum = PINSEL_PORT_0;  // Puerto 0
        pin_cfg.Pinnum = i;               // Pines 0 a 7
        pin_cfg.Funcnum = 0;              // Función GPIO
        pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;  // Resistencia de pull-up
        pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL; // No open-drain
        PINSEL_ConfigPin(&pin_cfg);
    }
    
    // Configurar P0.0 a P0.7 como salidas
    GPIO_SetDir(PINSEL_PORT_0, 0xFF, OUTPUT);  // 0xFF = 1111 1111 (pines 0 a 7), 1 para salida


/*pines de eint*/
    PINSEL_CFG_Type pin_cfg;
    pin_cfg.Pinnum  = PINSEL_PORT_2;
    pin_cfg.Portnm  = PINSEL_PIN_11;
    pin_cfg.Funcnum = PINSEL_FUNC_1;
    pin_cfg.Pinmode = PINSEL_PINMODE_PULLUP;
    pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin (&pin_cfg);

}

void configure_external_interrupt(void)
{
    EXTI_InitTypeDef exti_cfg;
	exti_cfg.EXTI_Line = EXTI_EINT1;
	exti_cfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	exti_cfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
	EXTI_Config(&exti_cfg);
    NVIC_EnableIRQn (EINT1_IRQn);
}

void EINT1_IRQHandler (void){
    EXTI_ClearEXTIFlag(EINT1);
    enable = !enable;
    if(enable==1){
        SYSTICK_IntCmd(ENABLE);
        SYSTICK_Cmd(ENABLE);
    }
    else {
        SYSTICK_IntCmd(DISABLE);
        SYSTICK_Cmd(DISABLE);
    }
}

void Systick_IRQHandler (void){
    calcular_promedio();
    GPIO_ClearValue (PINSEL_PORT_0 , 0xFF)
    GPIO_SetValue (PINSEL_PORT_0 , promedio & 0xFF);   
    SYSTICK_ClearCounterFlag();
}

void calcular_promedio (void){
    val=0;
    for (uint8_t i=0 ; i<8 ; i++){
        val=val+value[i];
    }
    promedio=val/8;
}

int main (void){
    SystemInit();
    SYSTICK_InternalInit(TIME);
    NVIC_SetPriority(Systick_IRQn , 0);
    pin_cfg();
    configure_external_interrupt();
    while (1){

    }
    return 0;
}
