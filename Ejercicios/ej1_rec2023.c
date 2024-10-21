/**
 * @file ej1_rec2023.c
 * @author your name (you@domain.com)
 * @brief Por un pin del ADC del microcontrolador LPC1769 ingresa una tensión de rango dinámico 0 a 3,3[v]
    proveniente de un sensor de temperatura. Debido a la baja tasa de variación de la señal, se pide tomar una
    muestra cada 30[s]. Pasados los 2[min] se debe promediar las últimas 4 muestras y en función de este valor,
    tomar una decisión sobre una salida digital de la placa:
        ● Si el valor es <1 [V] colocar la salida en 0 (0[V]).
        ● Si el valor es >= 1[V] y <=2[V] modular una señal PWM con un Ciclo de trabajo que va desde el 50%
        hasta el 90% proporcional al valor de tensión, con un periodo de 20[KHz].
        ● Si el valor es > 2[V] colocar la salida en 1 (3,3[V]).

 * @version 0.1
 * @date 2024-10-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

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


void configure_timer (void){
    

}