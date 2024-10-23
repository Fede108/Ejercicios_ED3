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
#define FLAG = 0
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
    
    /Genero primera mitad de onda/
    for(int i = 0; i<2048; i++){
        *samples = muestras;//Lo referencio, le cargo el valor
        muestras = muestras + 4;
        samples = samples + 1;//Lo desreferencio, estoy apuntando a la posicion y no al valor
    }

    /Genero segunda mitad de onda/
    for(int i = 2048; i<4096; i++){
        *samples = muestras;//Lo referencio, le cargo el valor
        muestras = muestras + 4;    // +4 o +1/4?? 
        samples = samples + 1;//Lo desreferencio, estoy apuntando a la posicion y no al valor
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
    ADC_StartCmd(LPC_ADC , ADC_START_ON_EINT0);   // porque en eint0??
    ADC_EdgeStartConfig(LPC_ADC , ADC_START_FALLING); 
    ADC_ChannelCmd(LPC_ADC , 0 ,ENABLE);
    ADC_IntConfig(LPC_ADC, ADC_ADINTEN0 , DISABLE);
}

void ConfDAC(void){
    DAC_CONVERTER_CFG_Type DAC_ConverterConfg;
    DAC_ConverterConfg.CNT_ENA = SET;
    DAC_ConverterConfg.DMA_ENA = SET;

    DAC_Init(LPC_DAC);
    //set time out for dac
    tmp = (PCLK_DAC * 1000000) / FREC_SENOIDAL;  // (PCLK_DAC * 1000000) / FREC_SENOIDAL*nro Muestras;
                                                                        //tiempo/nro muestras == (1/frec*nro muestras)
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
    GPDMA_LLI_Type GPDMALLI;

    GPDMALLI.SrcAddr = BANK0_MITAD2;
    GPDMALLI.DstAddr = (uint32_t) & (LPC_DAC -> DACR);//DAC
    GPDMALLI.NextLLI = &GPDMA_LLI;
    GPDMALLI.Control = DMA_SIZE /largo de los datos/
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
    //Testeo si fue por subida o bajada,si es por bajda, estado uno
    //si es por subida estado dos
    if(FLAGG){
        //flag =1, fue por subida, estadi dos
        FLAG = 0;
        //Por el dac saco la onda previamente almacenada
        //hago una nueva conversion de datos con el adc
        confDMA_DAC();
        ConfDMA_ADC();
    }  
    else{//entra primero, por bajada, estado uno
        while(!ADC_GlobalGetStatus(LPC_ADC, 1)){ //done status
            //espero que termine conversion
        }
        //DONE = 1, termino conversion
        ADC_DeInit(LPC_ADC);
        //tengo que sacer las muestras del adc por dac
        DMA_DAC1();
        FLAG = 1;
    }
}



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
        for(int j = 9; j>0 ; j--){
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



int16_t muestras[3];
uint16_t prom;
uint16_t sum = 0;
float t_alto = 0;
float t_total = 0;
float t_bajo = 0;

void confTMR2(void);
void CalcTiempos(void);

void ADC_Handler(void){
    static uint8_t ptr = 0;
    if(ptr<4){
        muestras[ptr] = ADC_ChannelGetData;
        ptr++;
    }
    else{
        ptr = 0;
    }
}

void TIMER0_IRQHandler{
    for(int i=0;i<4;i++){
        sum = muestras[i] + sum;
    }
    prom = (sum/4);
    if(prom<1365){
        GPIO_ClearValue();
    }
    else if(prom>2482){
        GPIO_SetValue();
    }

    else if(1365<= prom <=2482){
        CalcTiempos();
        confTMR2();
    }
}

void CalcTiempos(){
    DC = (prom)/2482;
    t_alto = DC*0.05;
    t_total = 0.05;
    t_bajo = t_total - t_alto 
}

void confTMR2(){
    TIM_TIMERCFG_Type timcfg;
    timcfg.PrescaleOption = TIM_PRESCALE_USVAL;
    timcfg.PrescaleValue = 5;

    TIM_MATCHCFG_Type matchcfg;
    matchcfg.MatchChannel = 0;
    matchcfg.IntOnMatch = ENABLE;
    matchcfg.MatchValue = t_alto;

    TIM_Init(LPC_TIM2 , TIM_TIMER_MODE , &timcfg);
    TIM_ConfigMatch(LPC_TIM0 , &matchcfg);

    matchcfg.MatchChannel = 1;
    matchcfg.IntOnMatch = ENABLE;
    matchcfg.MatchValue = t_bajo;
    TIM_ConfigMatch(LPC_TIM2 , &matchcfg);
    TIM_Cmd(LPC_TIM2, ENABLE);

    NVIC_Enable(TIMER2_IRQn);
}

void TIMER2_IRQHandler(){
    if(TIM_GetIntStatus(LPC_TIM2 , TIM_MR0_INT)){
        GPIO_ClearValue();
    }
    else if(TIM_GetIntStatus(LPC_TIM2 , TIM_MR1_INT)){
        GPIO_SetValue();
    }
}