/*#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif


#ifdef __USE_MCUEXPRESSO
#include <cr_section_macros.h>
#endif

#include "lpc17xx_timer.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_clkpwr.h"

#define PORT0 ((uint8_t)(0))
#define PIN0 ((uint8_t)(0))
#define INPUT ((uint8_t)(1))

uint32_t ValueDiv = 0;


void confgTimer(void);
void confgPuertos(void);

int main(void){
    confgTimer();
    confgPuertos();
    
    while (1)
    {
    }
    
}

void confgTimer(void){
//Configuracion TIMER1
    TIM_TIMERCFG_Type timCfg;
    timCfg.PrescaleOption = TIM_PRESCALE_USVAL;
    timCfg.PrescaleValue = 50;


    //Configuro el match M2.0
    TIM_MATCHCFG_Type matchCfg;
    matchCfg.MatchChannel = 2;
    matchCfg.IntOnMatch = DISABLE;
    matchCfg.ResetOnMatch = ENABLE;
    matchCfg.StopOnMatch = DISABLE;
    matchCfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
    matchCfg.MatchValue = 50;
    
    //Configuro la estructura pasandosela a la funcion
    TIM_Init(LPC_TIM1, TIM_TIMER_MODE,&timCfg);
    TIM_ConfigMatch(LPC_TIM1, &matchCfg);
    TIM_Cmd(LPC_TIM1,ENABLE);

}

void confgPuertos(void){
    PINSEL_CFG_Type PinCfg;
    PinCfg.Portnum = PINSEL_PORT_0;
    PinCfg.Pinnum = PINSEL_PIN_0;
    PinCfg.Funcnum = PINSEL_FUNC_0;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;

    PINSEL_ConfigPin(&PinCfg);
    GPIO_SetDir(PORT0,PIN0,INPUT);

    NVIC_SetPriority(EINT3_IRQn,0);//Interrupcion por GPIO
    NVIC_EnableIRQ(EINT3_IRQn);
}



void EINT3_IRQHandler(void){
    if(ValueDiv<3){
        CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER1,ValueDiv);
    ValueDiv = ValueDiv + 1;
    }
    else{
        ValueDiv = 0;
        CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER1,ValueDiv);
    }
    //Limpio interrupcion
    GPIO_ClearInt(PORT0,PIN0);
    
}*/

//EJERCICIO 15

/*
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include<cr_section_macros.h>
#endif

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"



void Confg_Timer(void){
    TIM_TIMERCFG_Type timercfg; //ESTRUCTURAS
    TIM_MATCHCFG_Type matchcfg;

    timercfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
    timercfg.PrescalValue = 2;

    matchcfg.IntOnMatch = ENABLE;
    matchcfg.StopOnMatch = DISABLE;
    matchcfg.ResetOnMatch = ENABLE;
    matchcfg.MatchChannel = 0;
    matchcfg.ExtMatchOutputType = TIM_EXTMACH_NOTHING;
    matchcfg.MatchValue = 6;

    TIM_Init(LPC_TIM0,TIM_TIMER_MODE, &timercfg);
    TIM_ConfigMatch(LPC_TIM_0, &matchcfg);
    TIM_Cmd(LPC_TIM0,ENABLE);


}*/

//EJERCICIO 16


#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include <cr_section_macros.h>
#endif

#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_exti.h"

int ValueDiv = 0;

void ConfgTimer(void);
void ConfgEINT(void);
int main(void){
    ConfgTimer();
    ConfgEINT();
    while (1){
    }
}

void ConfgTimer(void){

TIM_TIMERCFG_Type timcfg;
timcfg.PrescaleOption = TIM_PRESCALE_USVAL;
timcfg.PrescaleValue = 100;

//Configuro MATCH
TIM_MATCHCFG_Type matchcfg;
matchcfg.MatchChannel = 0;
matchcfg.IntOnMatch = DISABLE;
matchcfg.ResetOnMatch = ENABLE;
matchcfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
matchcfg.MatchValue = 100;

//Configuracion del temporizador
TIM_ConfigMatch(LPC_TIM2, &matchcfg);
TIM_Init(LPC_TIM2, TIM_TIMER_MODE , (void*) & timcfg );
}

void ConfgEINT(void){

//Configuro PIN2.11 Como EINT
PINSEL_CFG_Type Pincfg;
Pincfg.Portnum = PINSEL_PORT_2;
Pincfg.Pinnum = PINSEL_PIN_11;
Pincfg.Funcnum = PINSEL_FUNC_1;//Configuro como EINT
Pincfg.Pinmode = PINSEL_PINMODE_PULLUP;

PINSEL_ConfigPin(&Pincfg);

//Configuro EINT por flanco descendente
EXTI_InitTypeDef EXTICfg;
EXTICfg.EXTI_LINE_ENUM = EXTI_EINT1;
EXTICfg.EXTI_MODE_ENUM = EXTI_MODE_EDGE_SENSITIVE;
EXTICfg.EXTI_POLARITY_ENUM = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;

EXTI_Config(&EXTICfg);
EXTI_ClearEXTIFlag(EXTI_EINT1);//LIMPIO BANDERA

//NVIC_SetPriority(EINT1_IRQn,1);
NVIC_EnableIRQ(EINT1_IRQn);//HABILITO INTERRUPCION
}

//Handler de la interrupcion externa
void EINT1_IRQHandler(void){
    if(ValueDiv<3){
        ClKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER2,ValueDiv);
        ValueDiv = ValueDiv + 1;
    }
    else{
        ValueDiv = 0;
        CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER2,ValueDiv);
        ValueDiv = ValueDiv + 1; 
    }
//Limpio bandera
EXTI_ClearEXTIFlag(EXTI_EINT1);
}






