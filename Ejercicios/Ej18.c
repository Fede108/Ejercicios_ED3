#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include <cr_section_macros.h> /* MCUXpresso-specific macros */
#endif

#include "lpc17xx_gpio.h"   /* GPIO */
#include "lpc17xx_pinsel.h" /* Pin Configuration */
#include "lpc17xx_timer.h"  /* Timer0 */
#include "lpc17xx_clkpwr.h"

void configure_timer (void){
    TIM_TIMERCFG_Type timer_cfg;
    timer_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    timer_cfg.PrescaleValue  = 100; //Clock 100Mhz. Time resolution 1ms 
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timer_cfg);
}

void configure_match (void){
    TIM_MATCHCFG_Type match_cfg;
    match_cfg.StopOnMatch = DISABLE;
    match_cfg.ResetOnMatch = ENABLE;
    match_cfg.IntOnMatch = ENABLE;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
    match_cfg.MatchChannel = 0;
    match_cfg.MatchValue = 5

    


}





