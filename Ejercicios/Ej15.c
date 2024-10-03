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

void config_timer_and_match (void){
    TIM_TIMERCFG_Type timer_config;
    timer_config.PrescaleOption = TIM_PRESCALE_TICKVAL;
    timer_config.PrescaleValue  = 2;
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE , &timer_config);

    TIM_MATCHCFG_Type match_config;
    match_config.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
    match_config.IntOnMatch = ENABLE;
    match_config.MatchChannel = 0;
    match_config.MatchValue = 6;
    match_config.ResetOnMatch = ENABLE;
    match_config.StopOnMatch = DISABLE;
    TIM_ConfigMatch (LPC_TIM0, &match_config);
    

}


