//Ejercicio 14
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
    PinCfg.Opendrain = PINSEL_PINMODE_NORMAL;

    PINSEL_ConfigPin(&PinCfg);
    GPIO_SetDir(PORT0,PIN0,INPUT);

    //Configuro pin 0.6 como MATCH 2.0
    PinCfg.Pinnum = PINSEL_PIN_6;
    PinCfg.Funcnum = PINSEL_FUNC_3;
    PINSEL_ConfigPin(&PinCfg);

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
    
}
*/

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

/*
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
int ValuePrescaler = 100;

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
timcfg.PrescaleValue = ValuePrescaler;


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
EXTICfg.EXTI_Line = EXTI_EINT1;
EXTICfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
EXTICfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;

EXTI_Config(&EXTICfg);
EXTI_ClearEXTIFlag(EXTI_EINT1);//LIMPIO BANDERA

//NVIC_SetPriority(EINT1_IRQn,1);
NVIC_EnableIRQ(EINT1_IRQn);//HABILITO INTERRUPCION
}

//Handler de la interrupcion externa
void EINT1_IRQHandler(void){

TIM_TIMERCFG_Type timcfg;
timcfg.PrescaleOption = TIM_PRESCALE_USVAL;

if(ValuePrescaler<0xFFFFFFFF){
ValuePrescaler = ValuePrescaler*2;
timcfg.PrescaleValue = (ValuePrescaler);
}
else{
timcfg.PrescaleValue = 1;
ValuePrescaler = 1;
}
TIM_Init(LPC_TIM2, TIM_TIMER_MODE , (void*) & timcfg );

EXTI_ClearEXTIFlag(EXTI_EINT1);
}
*/

//Ejercicio 17
/*Consigna
Escribir un programa para que por cada presión de un pulsador, la frecuencia de
parpadeo disminuya a la mitad debido a la modificación del registro del Match 0. El
pulsador debe producir una interrupción por EINT2 con flanco descenden
*/

/*
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef __USE_MCUEXPRESSO
#include <cr.section_macros.h>
#endif

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_exti.h"

void ConfPuertos(void);
void ConfTimer(void);
void ConfInt(void);

uint32_t valor_matcheo = 100;

int main(void){
    ConfPuertos();
    ConfTimer();
    ConfInt();

while(1){
}
return 0;
}


void ConfPuertos(void){
    PINSEL_CFG_Type PinCfg;
    
    PinCfg.Portnum = PINSEL_PORT_2;
    PinCfg.Funcnum = PINSEL_PIN_12;
    PinCfg.Pinmode = PINSEL_FUNC_1;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;

    PINSEL_ConfigPin(&PinCfg);
}

void ConfTimer(void){
    TIM_MATCHCFG_Type   TIM_MatchCfg;
    
    TIM_MatchCfg.MatchChannel = 0;
    TIM_MatchCfg.IntOnMatch = ENABLE;
    TIM_MatchCfg.ResetOnMatch = ENABLE;
    TIM_MatchCfg.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
    TIM_MatchCfg.MatchValue = valor_matcheo;
    
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, (void*)& TIM_MatchCfg);
}

void ConfInt(void){
    EXTI_InitTypeDef EXTICfg;
    
    EXTICfg.EXTI_Line = EXTI_EINT2;
    EXTICfg.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
    EXTICfg.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
    
    EXTI_Config(&EXTICfg);
    EXTI_ClearEXTIFlag(EXTI_EINT2);

    NVIC_EnableIRQ(EINT2_IRQn);
}

void EINT2_IRQHandler(void){
    //Duplicar el valor de matcheo
    valor_matcheo = valor_matcheo*2;

    //Actualizar el valor de matcheo
    TIM_UpdateMatchValue(LPC_TIM0 , 0 , valor_matcheo);

    //Limpiar la bandera de interrupcion
    EXTI_ClearEXTIFLAG(EXTI_EINT2);
}
*/

// EJERCICIO 20

/*
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef _USE_MCU_EXPRESSO
#include<cr.section_macros.h>
#endif
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"

void ConfPines(void);
void ConfADC(void);

uint8_t canal = 0;
uint8_t cont = 0; 

int main(void){
    ConfPines();
    ConfADC();

return;
}

void ConfPines(void){
    //Configuro pines 0.23 a 0.26 como canales de ADC AD0 a AD3
    PINSEL_CFG_Type  PinCfg;
    PinCfg.Portnum = PINSEL_PORT_0;
    PinCfg.Pinnum = PINSEL_PIN_23;
    PinCfg.Funcnum = PINSEL_FUNC_1;
    PINSEL_ConfigPin(&PinCfg);

    PinCfg.Pinnum = PINSEL_PIN_24;
    PINSEL_ConfigPin(&PinCfg);

    PinCfg.Pinnum = PINSEL_PIN_25;
    PINSEL_ConfigPin(&PinCfg);

    PinCfg.Pinnum = PINSEL_PIN_26;
    PINSEL_ConfigPin(&PinCfg);
}

void ConfADC(void){
    //Inicializo adc con frecucencia 200k
    ADC_Init(LPC_ADC,20000);//200kHz
    ADC_BurstCmd(LPC_ADC,ENABLE); //Habilito modo Burst

    //Habilito los 4 canales AD0 a AD3
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0 , ENABLE);
    ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_1, ENABLE);
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_2 , ENABLE);
    ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_3, ENABLE);
    
    //Configuro inicio
    ADC_StartCmd(LPC_ADC , ADC_START_NOW);

    //Configuro disparo en flanco ascendente
    ADC_EdgeStartConfig(LPC_ADC , ADC_START_ON_RISING);
}
*/
/*Consigna:
Diseñar un sistema de control de velocidad para un motor DC utilizando la LPC1769. El sistema utilizará un potenciómetro para ajustar la velocidad deseada, y empleará PWM para controlar la velocidad real del motor. Además, implementará un sistema tal que si detecta una sobrecarga, reducirá la velocidad del motor a un 80% de la velocidad deseada.
Requisitos:
Utilizar el ADC para leer la posición del potenciómetro, que representará la velocidad deseada.
Implementar un timer para generar una señal PWM de 10kHz que controle la velocidad del motor.
Utilizar otro timer para accionar el ADC.
Estados de Operación:
Reposo: Motor apagado (0% duty cycle)
Normal: Motor funcionando a la velocidad deseada
Sobrecarga: Motor funcionando a velocidad reducida (reducir duty cycle en un 20%)
Sistema de Seguridad:
Detectar condiciones de sobrecarga mediante un sensor conectado a un pin GPIO.
Indicadores:
Utilizar LEDs para indicar el estado actual del sistema
*/

/*
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#ifdef _USE_MCU_EXPRESSO
#include<cr.section_macros.h>
#endif

#include "lpc17xx_timer.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"

void ConfADC(void);
void ConfPuertos(void);
void ConfTimers(void);
void ConfIntGPIO(void);

uint8_t MARCA_SOBRECARGA_NORMAL;
#define SOBRECARGA 1
#define NORMAL 0
#define LED_NORMAL (1<<16)
#define LED_SOBRECARGA (1<<17)
#define LED_REPOSO (1<<18)

int main (void){
    uint16_t ticksalto;
    uint16_t ticksbajo;
    uint8_t flag = 0;
    
    ConfADC();
    ConfIntGPIO();
    ConfPuertos();
    ConfTimers();    

    while(1){
    }


}

void ConfADC(void){
    ADC_Init(LPC_ADC , 100000);
    ADC_ChannelCmd(LPC_ADC , 7 , ENABLE);
    ADC_IntConfig(LPC_ADC , ADC_ADINTEN7 , ENABLE);
    ADC_StartCmd(LPC_ADC , ADC_START_ON_MAT01);
    NVIC_SetPriority(ADC_IRQn , 1);
    NVIC_EnableIRQ(ADC_IRQn);
}

void ConfPuertos(void){
    PINSEL_CFG_Type PinCfg;

    PinCfg.Portnum = PINSEL_PORT_0;
    PinCfg.Pinnum = PINSEL_PIN_0;
    PinCfg.Funcnum = PINSEL_FUNC_0;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;

    PINSEL_ConfigPin(&PinCfg);

    PinCfg.Pinnum = PINSEL_PIN_16;
    PINSEL_ConfigPin(&PinCfg);

    PinCfg.Pinnum = PINSEL_PIN_17;
    PINSEL_ConfigPin(&PinCfg);

    PinCfg.Pinnum = PINSEL_PIN_18;
    PINSEL_ConfigPin(&PinCfg);

    GPIO_SetDir(0, (1<<0) , INPUT); //Sobrecarga
    GPIO_SetDir(0, (1<<16) , OUTPUT);//Led VERDE
    GPIO_SetDir(0, (1<<17) , OUTPUT);//Led ROJO
    GPIO_SetDir(0, (1<<18) , OUTPUT);//Led AMARILLO

}


void ConfTimers(void){
    
    TIM_TIMERCFG_Type timcfg;

    timcfg.PrescaleOption = TIM_PRESCALE_USVAL;
    timcfg.PrescaleValue = 1000;
    TIM_Init(LPC_TIM0 , TIM_TIMER_MODE , &timcfg);
    
    TIM_MATCHCFG_Type matchcfg;
    
    matchcfg.MatchChannel = 0;
    matchcfg.IntOnMatch = DISABLE;
    matchcfg.ExtMatchOutputType = TIM_EXMATCH_TOGGLE;
    matchcfg.ResetOnMatch = ENABLE;
    matchcfg.MatchValue = 1000;

    TIM_ConfigMatch(LPC_TIM0 , &matchcfg);
    TIM_Cmd(LPC_TIM0 , ENABLE);

    timcfg.PrescaleOption = TIM_PRESCALE_USVAL;
    timcfg.PrescaleValue = 1;// Resolucion mas chica que la señal


    TIM_Init(LPC_TIM1 , TIM_TIMER_MODE , &timcfg);
    matchcfg.IntOnMatch = ENABLE;
    matchcfg.ExtMatchOutputType = TIM_EXMATCH_TOGGLE;
    matchcfg.MatchValue = 0;

    TIM_ConfigMatch(LPC_TIM1 , &matchcfg);
    TIM_Cmd(LPC_TIM1 , ENABLE);
    NVIC_SetPriority(TIMER1_IRQn , 2);
    NVIC_EnableIRQ(TIMER1_IRQn);
}

void TIMER1_IRQHandler(void){

    if(flag){
        TIM_UpdateMatchValue(LPC_TIM1 , 0 , ticksbajo);
        flag = !flag;
    }
    else{
        TIM_UpdateMatchValue(LPC_TIM1 , 0 , ticksalto);
        flag = !flag;
    }

    TIM_ConfigMatch(LPC_TIM1 , &matchcfg);
    TIM_ClearIntPending(LPC_TIM1 , TIM_MR0_INT);
}

void ADC_IRQHandler(void){
    valor_adc = ADC_ChannelGetData(LPC_ADC , 7);
    float duty_cycle = (valor_adc) / 4095;
    uint16_t total_ticks = 100;

    ticksalto = duty_cycle * total_ticks;

    ticksbajo = total_ticks - ticksalto;

    if(valor_adc == 0){
        GPIO_SetValue(0 , LED_REPOSO);
        GPIO_ClearValue(0, LED_NORMAL);
        GPIO_ClearValue(0, LED_SOBRECARGA);
    }
    else{
        if(MARCA_SOBRECARGA_NORMAL == NORMAL){
            GPIO_SetValue(0, LED_NORMAL);
            GPIO_ClearValue(0, LED_REPOSO);
            GPIO_ClearValue(0, LED_SOBRECARGA);
        }
        if(MARCA_SOBRECARGA_NORMAL == SOBRECARGA){
            GPIO_SetValue(0, LED_SOBRECARGA);
            GPIO_ClearValue(0, LED_NORMAL);
            GPIO_ClearValue(0, LED_REPOSO);
        }
    }
}

void ConfIntGPIO(void){
    GPIO_IntCmd(0,0,1); //Falling Edge
    GPIO_IntCmd(0,0,0); //Rising Edge
    NVIC_EnableIRQ(EINT3_IRQn);
}


void EINT3_IRQHandler(void){
    
    if(GPIO_GetIntStatus( 0 , 0, RisingEdge )){//Pregunto por flag rising, equivale a que hay sobrecarga
    
    MARCA_SOBRECARGA_NORMAL = SOBRECARGA;

    ticksalto = (ticksalto * 80)/ 100 ;//Reducir en un 20% el duty cycle
    ticksbajo = (0,1 * 10 ^-3) - ticksalto;
    }
    else{
        if(GPIO_GetIntStatus(0 , 0 , FallingEdge)){
        MARCA_SOBRECARGA_NORMAL = NORMAL;    
        }
    }
}
*/

//EJERCICIO 21
/*Consigna:
    Realizar un código en C que permita filtrar las muestras provenientes del ADC con
un filtro promediador móvil de N muestras, donde N inicialmente es igual a 2 y
puede ser incrementado en dos unidades cada vez que se presiona un pulsador
conectado en P0.6 hasta llegar a un valor de 600. A su vez, utilizando el bit de
overrun y un timer, complete el código realizado para que en caso de producirse el
evento de pérdida de datos por parte del ADC, se deje de muestrear y se saque por
el pin MATCH2.1 una señal cuadrada de 440 Hz. Considerar una frecuencia de cclk
de 60 Mhz y configurar el ADC para obtener una frecuencia de muestreo de 5
Kmuestras/seg.


#ifdef __USE_CMSIS
#include <LPC17xx.h>
#endif
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_exti.h"

void ConfgPines(void);
void ConfgGPIOInt(void);
void ConfgTimer0(void);
void ConfgADC(void);
void PromediadorMovil(void);
void ConfSenalCuadrada(void);

uint8_t N;
uint8_t tamaño = 0;
uint8_t prom_movil = 0;

int main(void){
    N = 2;
    ConfgPines();
    ConfgGPIOInt();
    ConfgTimer0();
    ConfgADC();
    while(1){
    }

}

void ConfgPines(void){
    PINSEL_CFG_Type PinCfg;
    PinCfg.Portnum = PINSEL_PORT_0;
    PinCfg.Pinnum = PINSEL_PIN_6;
    PinCfg.Funcnum = PINSEL_FUNC_0;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;

    Pinsel_ConfigPin(&PinCfg);
    
    PinCfg.Portnum = PINSEL_PORT_1;
    PinCfg.Pinnum = PINSEL_PIN_30;
    PinCfg.Funcnum = PINSEL_FUNC_3;

    Pinsel_ConfigPin(&PinCfg);
}

void ConfgGPIOInt(void){
    GPIO_SetDir(0 , 1<<6 , GPIO_DIR_INPUT);
    GPIO_IntCmd(0 , (1<<6) , FallingEdge);
    NVIC_SetPriority(EINT3_IRQn , 3);
    NVIC_Enable(EINT3_IRQn);
}

void ConfgTimer0(void){
    TIM_MATCHCFG_Type matchcfg;
    matchcfg.IntOnMatch = DISABLE;
    matchcfg.MatchChannel = 1;
    matchcfg.ResetOnMatch = ENABLE;
    matchcfg.ExtMatchOutput = TIM_EXMATCH_NOTHING;
    matchcfg.MatchValue = 1;

    TIM_TIMERCFG_Type timcfg;
    timcfg.PrescaleOption = TIM_PRESCALE_USVAL;
    timcfg.PrescaleValue = 1000; //Match cada 1ms

    TIM_Init(LPC_TIM0 , TIM_TIMER_MODE , &timcfg);
    TIM_ConfigMatch(LPC_TIM0 , &matchcfg);
    TIM_Cmd(LPC_TIM0 , ENABLE);

}

void ConfgADC(void){
    ADC_Init(LPC_ADC,12000); // Configuro adc para tener una frecuencia de muestreo de 12k, ya que 60M/5K = 12K
    ADC_BurstCmd(LPC_ADC , DISABLE);
    ADC_ChannelCmd(LPC_ADC , 4 , ENABLE);
    ADC_IntConfig(LPC_ADC , START_ON_MAT01);

    NVIC_SetPriority(ADC_IRQn , 2);
    NVIC_Enable(ADC_IRQn);
}

void EINT3_IRQHandler(void){
    if(N<600){     
        N++;//Incrementa en uno en cada interrupcion
    }
    else{
        N = 2;
    }
    GPIO_ClearInt(0, 1<<6);
}

void ADC_Handler(void){
    if(ADC_ChannelGetStatus(LPC_ADC , 4 , BurstStatus)){
        ADC_DeInit(LPC_ADC); //Desabilito el ADC en caso de overrun
        ConfgSenalCuadrada(); // Configuro la señal cuadrada de 440hz
    }
    else{
        PromediadorMovil();
    }
}

void ConfgSenalCuadrada(void){
    TIM_MATCHCFG_Type matchcfg;

    matchcfg.IntOnMatch = DISABLE;
    matchcfg.MatchChannel = 1;
    matchcfg.ResetOnMatch = ENABLE;
    matchcfg.ExtMatchOutputType = TIM_EXMATCH_TOGGLE;
    matchcfg.MatchValue = 68160;//Cargo el valor en ticks = f * t = 60Mhz * 1,136mS

    TIM_TIMERCFG_Type timcfg;

    timcfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
    timcfg.PrescaleValue = 1; //Uso el prescarler en un 1 para usar la frecuencia completa (60Mhz)

    TIM_Init(LPC_TIM0 , TIM_TIMER_MODE , &timcfg);
    TIM_ConfigMatch(LPC_TIM0 , &matchcfg);
    TIM_Cmd(LPC_TIM0 , ENABLE);
}

void PromediadorMovil(void){
    uint32_t acumulador = 0;
    static uint8_t index = 0;
    static uint16_t array[600];

    array[index] = ADC_ChannelGetData(LPC_ADC , 4);

    index++; //Incrementar indice y reiniciar si alcanza a N
    
    if(index >= N){
        index = 0;
    }

    //Calcular el promedio movil usando las ultimas N muestras

    for(int i = 0 ; i<N ; i++){
        acumulador = acumulador + array[i]; //Suma cada valor del arreglo
    }
    prom_movil = (acumulador / tamaño);
}
*/


//EJERCIO 1 PARCIAL
/*
/*Consigna: Usando un timer y un pin de capture demodular una señal PWM que ingresa por dicho pin.
Calcular el ciclo de trabajo y periodo, y sacar una tension continua proporcional al ciclo de trabajo a traves
del DAC de rango dinamico 0 - 2V con un rate de actualizacion de 0,5s del promedio de los ultimos diez valores
obtenindos en la captura

//Corregir

#ifdef __USE_CMSIS
#include <LPC17xx.h>
#endif

#include "lpc17xx_pinsel.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_timer.h"

uint8_t FLAG = 0;
uint8_t PROM_DC;
uint32_t TALTO; uint32_t T_TOTAL;
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
            T_BAJO = TIM_GetCaptureValue(LPC_TIM0 , 0);
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
    T_TOTAL = T_BAJO + T_ALTO;
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
*/


EJERCICIO 2 PARCIAL

#ifdef __USE_CMSIS
#include<LPC17xx.h>
#endif

#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_dac.h"


void ConfPines(void);
void ConfADC(void);
void ConfEINT(void);
void ConfDAC(void);
void ConfDMA_ADC(void);
void ConfDMA_DAC(void);
void generaronda(void);

/*Globales y defines*/
#define PCLK_DAC 20
#define FREC_SENOIDAL 32000
#define DMA_SIZE 10
#define FLAG = 1
#define BANK0_MITAD1 0x2007C000
#define BANK0_MITAD2 0x2007E000
uint16_t*samples = (uint16_t*)BANK0_MITAD2;
uint16_t muestras = 0;



int main(void){
    generaronda();
    ConfPines();
    ConfADC();
    ConfEINT();
    ConfDAC();
    ConfDMA_ADC();
    confDMA_DAC();
    DMA_DAC1();

    while(1){}
}

void generaronda(void){
    //la debo reproducir por el dac
    //Hago linked list de la segunda mitad del banco 0 al final
    //segunda mitad del banco 0 = 0x2007E000 - 0x2007FFFF
    
    /*Genero primera mitad de onda*/
    for(int i = 0; i<2048; i++){
        *samples = muestras;//Lo referencio, le cargo el valor
        muestras = muestras + 4;
        for(int j=0;j<4;j++){
            samples = samples + 1;//Lo desreferencio, estoy apuntando a la posicion y no al valor
        }
    }

    /*Genero segunda mitad de onda*/
    for(int i = 2048; i<4096; i++){
        *samples = muestras;//Lo referencio, le cargo el valor
        muestras = muestras + 4;
        for(int j=0;j<4;j++){
            samples = samples + 1;//Lo desreferencio, estoy apuntando a la posicion y no al valor
        }    
    }  
}

void ConfPines(void){
    PINSEL_CFG_Type PinCfg;
    PinCfg.Portnum = PINSEL_PORT_0;
    PinCfg.Pinnum = PINSEL_PIN_2;
    PinCfg.Funcnum = PINSEL_FUNC_1;

    PINSEL_ConfigPin(&PinCfg);

    PinCfg.Portnum = PINSEL_PORT_2;
    PinCfg.Pinnum = PINSEL_PIN_10;
    PinCfg.Funcnum = PINSEL_FUNC_1;
    PinCfg.Pinmode = PINSE_PINMODE_PULLUP;

    PINSEL_ConfigPin(&PinCfg);
}

void ConfADC(void){
    ADC_Init(LPC_ADC,32000);
    ADC_StartCmd(LPC_ADC , ADC_START_ON_EINT0);
    ADC_EdgeStartConfig(LPC_ADC , ADC_START_CONTINUOS);
    ADC_ChannelCmd(LPC_ADC , 0 ,ENABLE);
    ADC_IntConfig(LPC_ADC, ADC_ADINTEN0 , DISABLE);
}

void ConfDAC(void){
    DAC_CONVERTER_CFG_Type DAC_ConverterConfg;
    DAC_ConverterConfg.CNT_ENA = SET;
    DAC_ConverterConfg.DMA_ENA = SET;

    DAC_Init(LPC_DAC);
    //set time out for dac
    tmp = ((PCLK_DAC * 1000000) / FREC_SENOIDAL ) / muestras;

    DAC_SetDMATimeOut(LPC_DAC , tmp);
    DAC_ConfigDAConverterControl(LPC_DAC , &DAC_ConverterConfg);
}

void ConfDMA_ADC(void){
    
    //Preparo estructura
    GPDMA_LLI_Type GPDMA_LLI;
   
    GPDMA_LLI.SrcAddr = (uint32_t) &(LPC_ADC -> ADRR0); //apunto al adc
    GPDMA_LLI.DstAddr = 0x2007C000;
    GPDMA_LLI.NextLLI = &GPDMA_LLI;
    GPDMA_LLI.Control = DMA_SIZE
                        | (1<<27) | |(1<<21)|(1<<18);//dest increment, 16b src, 16b dst
    //destino no se incrementa, por lo tanto destino increment= 0
    
    //Inicializo
    GPDMA_Init();

    //Guardo datos del adc en la primera mitad del bank0
    GPDMA_Channel_CFG_Type GPDMACfg;
    
    GPDMACfg.ChannelNum = 0;//Canal 0
    GPDMACfg.SrcConn = GPDMA_CONN_ADC;
    GPDMACfg.TransferSize = 4096;
    GPDMACfg.DstMemAddr = 0x2007C000;
    GPDMACfg.TransferWidth = 0;
    GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_P2M;
    GPDMACfg.DstConn = 0;//unused
    GPDMACfg.DMALLI =(uint32_t) & GPDMA_LLI;

    
    GPDMA_Setup(&GPDMACfg);
    GPDMA_ChannelCmd(0,ENABLE);
}

void confDMA_DAC(void){
    //Saco onda por DAC
    //Preparo estructura
    GPDMA_ChannelCmd(1 , DISABLE);
    GPDMA_LLI_Type GPDMALLI;

    GPDMALLI.SrcAddr = BANK0_MITAD2;
    GPDMALLI.DstAddr = (uint32_t) & (LPC_DAC -> DACR);//DAC
    GPDMALLI.NextLLI = &GPDMA_LLI;
    GPDMALLI.Control = DMA_SIZE /*largo de los datos*/
                       |(1<<26)|(1<<21)|(1<<18);//src incr,src 16b,dst 16b,  

    GPDMA_Init();// Inicializo


    //del dma al dac
    GPDMA_Channel_CFG_Type GDMAChanCfg;
    GDMAChanCfg.ChannelNum = 1; //canal 1
    GDMAChanCfg.TransferSize = 4096;
    GDMAChanCfg.TransferWidth = 0;
    GDMAChanCfg.SrcMemAddr = BANK0_MITAD2;
    GDMAChanCfg.DstConn = GPDMA_CONN_DAC; 
    GDMAChanCfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
    GDMACfg.DMALLI = (uint32_t) & GPDMA_LLI;

    GPDMA_Setup(&GPDMACfg);    
    GPDMA_ChannelCmd(1,ENABLE);
}

void ConfEINT(void){
    EXTI_InitTypeDef EXTICfg;

    EXTICfg.EXTI_LINE_ENUM = EXTI_EINT0;
    EXTICfg.EXTI_MODE_ENUM = EXTI_MODE_EDGE_SENSITIVE;
    EXTICfg.EXTI_POLARITY_ENUM = EXTI_POLATITY_HIGH_ACTIVE_OR_RISING_EDGE;
    EXTI_Config(&EXTICfg);

    NVIC_SetPriority(EINT0_IRQn, 1);
    NVIC_Enable(EINT0_IRQn);
}

void DMA_DAC1(void){//Lo de la primera mitad del banco lo saco por el dac
    //Preparo estructura
    GPDMA_ChannelCmd(1 , DISABLE);
    GPDMA_LLI_Type GPDMA_LLI;
    
    GPDMA_LLI.SrcAddr = BANK0_MITAD1;
    GPDMA_LLI.DstAddr = (uint32_t)(LPC_DAC -> DACR);
    GPDMA_LLI.NextLLI = &GPDMAChanCfg;
    GPMA_LLI.Control  = DMA_SIZE|(1<<26)| (1<<21) | (1<<18); //incr src, src16b, dst16b

    GPDMA_Init();
    // saco los valores por dac
    GPDMA_Channel_CFG_Type GPDMA_ChannelCfg;

    GPDMA_ChannelCfg.ChannelNum = 1;
    GPDMA_ChannelCfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
    GPDMA_ChannelCfg.SrcMemAdrr = BANK0_MITAD1;
    GPDMA_ChannelCfg.DstConn = GPDMA_CONN_DAC;
    GPDMA_ChannelCfg.TransferSize = 4096;
    GPDMA_ChannelCfg.TransferWidth = 0;//unused
    GPDMA_ChannelCfg.DMALLI = &GPDMALLI;

    GPDMA_Setup(&GPDMA_ChannelCfg);
    GPDMA_ChannelCmd(1,ENABLE);
}

void EINT0_IRQHandler(void){
    
    if(FLAGG){
        ConfADC();
        confDMA_DAC();
        ConfDMA_ADC();
    }  
    else{//entra primero, estado uno
        while(!ADC_GlobalGetStatus(LPC_ADC, 1)){ //done status
            //espero que termine conversion
        }
        //DONE = 1, termino conversion
        ADC_DeInit(LPC_ADC);
        //tengo que sacer las muestras del adc por dac
        DMA_DAC1();
    }
    FLAGG = !FLAG;
}


