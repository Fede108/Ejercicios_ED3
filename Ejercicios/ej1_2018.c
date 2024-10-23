#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include <cr_section_macros.h> /* MCUXpresso-specific macros */
#endif

#include "lpc17xx_gpio.h"   /* GPIO */
#include "lpc17xx_pinsel.h" /* Pin Configuration */
#include "lpc17xx_timer.h"  /* Timer0 */
#include "lpc17xx_exti.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_libcfg_default.h"

#define memory 0x10004000

uint32_t *dac_values = (uint32_t*)memory;
uint32_t output=0;
volatile uint32_t match_value = 0;

void configure_dac (void){
    PINSEL_CFG_Type PinCfg;
    // Configure pin P0.26 as DAC output
    PinCfg.Funcnum = PINSEL_FUNC_2;           // DAC function
    PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL; // Disable open drain
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;   // Pull-up 
    PinCfg.Pinnum = PINSEL_PIN_26;            // Pin number 26
    PinCfg.Portnum = PINSEL_PORT_0;           // Port number 0
    PINSEL_ConfigPin(&PinCfg);
    DAC_Init (LPC_DAC);
}

void configure_timer (uint32_t match_value){
    TIM_TIMERCFG_Type tim_cfg;
    tim_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    tim_cfg.PrescaleValue  = 1000;
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &tim_cfg);

    TIM_MATCHCFG_Type match_cfg;
    match_cfg.IntOnMatch         = ENABLE; 
    match_cfg.MatchChannel       = 1;
    match_cfg.MatchValue         = match_value;
    match_cfg.ResetOnMatch       = ENABLE;
    match_cfg.StopOnMatch        = DISABLE;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
    TIM_ConfigMatch(LPC_TIM0, &match_cfg);
}

void TIMER_IRQHandler (void) {


    TIM_ClearIntPending(LPC_TIM0 , TIM_MR0_INT);

    if (match_value == ((*dac_values >> 16) & 0xFF))
    {
        if (output<*dac_values & 0XFF)
        {   
            output++;
            DAC_UpdateValue(LPC_DAC, output);
        }
        else
        {
            match_value = (*dac_values >> 8) & 0xFF;
            TIM_UpdateMatchValue(LPC_TIM0, 0, match_value);
        }    
    }

    if (match_value == ((*dac_values >> 8) & 0xFF))
    {
        match_value = (*dac_values >> 8) & 0xFF;
        TIM_UpdateMatchValue(LPC_TIM0, 0, match_value);
    }

    if (match_value == ((*dac_values >> 8) & 0xFF))
    {
        if (output>0)
        {   
            output--;
            DAC_UpdateValue(LPC_DAC, output);
        }
        else
        {
            match_value = (*dac_values >> 16) & 0xFF;
            TIM_UpdateMatchValue(LPC_TIM0, 0, match_value);
        }     
    }
}

int main(void) 
{   
    
    match_value = (*dac_values >> 16) & 0xFF;
    configure_timer(match_value);
    return 0;
}
