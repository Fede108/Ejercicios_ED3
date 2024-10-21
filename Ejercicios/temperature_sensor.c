#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include <cr_section_macros.h> /* MCUXpresso-specific macros */
#endif

#include "lpc17xx_gpio.h"   /* GPIO */
#include "lpc17xx_pinsel.h" /* Pin Configuration */
#include "lpc17xx_timer.h"  /* Timer0 */
#include "lpc17xx_adc.h"

#ifdef __USE_CMSIS
#include <LPC17xx.h>
#endif

#include "lpc17xx_pinsel.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_timer.h"

uint8_t FLAG = 0;
uint8_t PROM_DC;
volatile uint32_t TALTO, T_TOTAL;
uint8_t CONT; 
uint i = 9;
uint32_t DC;
uint32_t valores[10];
uint32_t bits = 0; 

void ConfPines(void);
void ConfCapture(void);
void DutyCycle(void);

int main(void){
    ConfPines();
    ConfCapture();
    while(1){}
}

void ConfPines(void){
    PINSEL_CFG_Type PinCfg;

    PinCfg.Portnum = PINSEL_PORT_0;
    PinCfg.Pinnum = PINSEL_PIN_4;
    PinCfg.Funcnum = PINSEL_FUNC_3; //P0.4 como capture
    PinCfg.Pinmode = PINSEL_PINMODE_PULLDOWN;

    PINSEL_ConfigPin(&PinCfg);
}

void ConfCapture(void){
    TIM_CAPTURECFG_Type CaptCfg;
    CaptCfg.CaptureChannel = 0;
    CaptCfg.RisingEdge = ENABLE;
    CaptCfg.FallingEdge = ENABLE;
    CaptCfg.IntOnCaption = ENABLE;

    TIM_TIMERCFG_Type timCfg;
    timCfg.PrescaleOption = TIM_PRESCAL_USVAL;
    timCfg.PrescaleValue = 1000;

    TIM_MATCHCFG_Type matchCfg;
    matchCfg.MatchChannel = 0;
    matchCfg.IntOnMatch = ENABLE;
    matchCfg.ResetOnMatch = ENABLE;
    matchcfg.ExtMatchOutputType = TIM_EXMATCH_NOTHING;
    matchcfg.MatchValue = 500; //0,5 seg interruption

    TIM_Init(LPC_TIM0 , TIM_TIMER_MODE , &timCfg);
    TIM_ConfigCapture(LPC_TIM0 , &CaptCfg);
    TIM_ConfigMatch(LPC_TIM0 , &matchCfg);
    TIM_Cmd(LPC_TIM0 , ENABLE);
    NVIC_EnableIRQ(TIMER0_IRQn);
}

void TIMER0_IRQHandler(void){
    if(TIM_GetIntStatus(LPC_TIM0 , TIM_MR0_INT)){
        DAC_Init(LPC_DAC);
        DAC_UpdateValue(LPC_DAC , bits);
    }
    else{
        if(FLAG == 0){
            T_TOTAL = TIM_GetCaptureValue(LPC_TIM0 , 0);
            FLAG++;
            DutyCycle();
        }
        else{
            T_ALTO = TIM_GetCaptureValue(LPC_TIM0 , 0);
            FLAG = 0;
        }
    }
}

void DutyCycle(void){
    DC = (T_ALTO * 100)/ T_TOTAL;
    uint32_t sum = 0;
    if(i >= 0){                                                
        valores[i] = DC;
         i--;
    }
    else{
        for(int j = 8; j>0 ; j--){
            valores[j++] = valores[j]; //Muevo los valores una posicion hacia adelante
        }
        valores [0] = DC;//Nuevo valor en la primera posicion
        //suma los ultimos diez valores
        for(int x = 0; x<10; x++){
            sum = valores[x] + sum;
        }
        PROM_DC = (sum / 10);
        bits = (PROM_DC * 620) / 100;
    }
}