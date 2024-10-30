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

#define OUTPUT 1
#define TRUE   1
#define DELAY  10
#define LED_0 ((uint32_t)(1<<0))
#define LED_1 ((uint32_t)(1<<1))
#define LED_2 ((uint32_t)(1<<2))
#define LED_3 ((uint32_t)(1<<3))
#define LED_4 ((uint32_t)(1<<4))
#define LED_5 ((uint32_t)(1<<5))
#define LED_6 ((uint32_t)(1<<6))
#define LED_7 ((uint32_t)(1<<7))
#define LED_8 ((uint32_t)(1<<8))
#define LED_9 ((uint32_t)(1<<9))

uint8_t var=0;

/// @brief 

void config_gpio(void){
        PINSEL_CFG_Type pin_cfg;

        pin_cfg.Portnum  = PINSEL_PORT_0;
        pin_cfg.Pinnum = PINSEL_PIN_0;
        pin_cfg.FuncNum = PINSEL_FUNC_0;
        pin_cfg.Pinmode = PINSEL_PINMODE_TRISTATE;
        pin_cfg.OpenDrain = PINSEL_PINMODE_NORMAL;

        PINSEL_ConfigPin(&pin_cfg);

        pin_cfg.Pinnum=PINSEL_PIN_1;
        PINSEL_ConfigPin(&pin_cfg);

        pin_cfg.Pinnum=PINSEL_PIN_2;
        PINSEL_ConfigPin(&pin_cfg);

        pin_cfg.Pinnum=PINSEL_PIN_3;
        PINSEL_ConfigPin(&pin_cfg);

        pin_cfg.Pinnum=PINSEL_PIN_4;
        PINSEL_ConfigPin(&pin_cfg);

        pin_cfg.Pinnum=PINSEL_PIN_5;
        PINSEL_ConfigPin(&pin_cfg);

        pin_cfg.Pinnum=PINSEL_PIN_6;
        PINSEL_ConfigPin(&pin_cfg);

        pin_cfg.Pinnum=PINSEL_PIN_7;
        PINSEL_ConfigPin(&pin_cfg);

        pin_cfg.Pinnum=PINSEL_PIN_8;
        PINSEL_ConfigPin(&pin_cfg);

        pin_cfg.Pinnum=PINSEL_PIN_9;
        PINSEL_ConfigPin(&pin_cfg)
        
        GPIO_SetDir(PINSEL_PORT_0, LED_0 | LED_1 | LED_2 | LED_3 | LED_4 | LED_5 | LED_6 | LED_7 | LED_8 | LED_9 , OUTPUT );
        
}

config_systick{
        SYSTICK_InternalInit(DELAY);
        SYSTICK_Cmd(ENABLE);
        SYSTICK_IntCmd(ENABLE);

     }

void first_sequence(void){
        GPIO_SetValue(PINSEL_PORT_0 , LED_2 | LED_6 | LED_7 | LED_1 | LED_9 );
        GPIO_ClearValue(PINSEL_PORT_0 , LED_3 | LED_5 | LED_8 | LED_0 | LED_4 );
}

void second_sequence(void){
        GPIO_ClearValue(PINSEL_PORT_0 , LED_2 | LED_6 | LED_7 | LED_1 | LED_9 );
        GPIO_SetValue(PINSEL_PORT_0 , LED_3 | LED_5 | LED_8 | LED_0 | LED_4 );
}


void Systick_Handler(void)
{
    SYSTICK_ClearCounterFlag();

    if(var==0){
        first_sequence();
        var=1;
    }
    else{
        second_sequence();
        var=0;
    }

    }

void main(void){

    
    config_gpio();
    config_systick();   

    while(true){

    }
}