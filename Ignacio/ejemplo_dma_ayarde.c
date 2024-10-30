//Ejemplo 1 DMA con DAC
/*#include "lpc17xx_dac.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpdma.h"

#define DMA_SIZE 60
#define NUM_SINE_SAMPLE 60
#define SINE_FREQ_IN_HZ 50
#define PCLK_DAC_IN_MHZ 25 //CCLK divided by 4

void confPin(void);
void confDMA(void);
void confDac(void);

GPDMA_Channel_CFG_Type GPDMACfg;
uint32_t dac_sine_lut[NUM_SINE_SAMPLE];  // SrcAdr

int main(void){
    uint32_t i;
    uint32_t sin_0_to_90_16_samples[16] = {\
        0,1045,2079,3090,4067, \
        5000,5877,6691,7431,8090,\
        8660,9135,9510,9781,9945,10000\
    };
    confPin();
    confDac();

    //Prepare DAC sine look up table
    for(i = 0; i<NUM_SINE_SAMPLE, i++){
        if( i<=15 ){
            dac_sine_lut[i] = 512 + 512*sin_0_to_90_16_samples[i] / 10000;
            if(i == 15) dac_sine_lut[i] = 1023;
        }
        else if(i <=30 ){
            dac_sine_lut[i] = 512 + 512*sin_0_to_90_16_samples[30-i] / 10000;       
            }
        else if(i <= 45 ){
            dac_sine_lut[i] = 512 -512*sin_0_to_90_16_samples[i-30]/10000;
        }
        else{
            dac_sine_lut[i] = 512 - 512*sin_0_to_90_16_samples[60-i]/10000;
        }
        dac_sine_lut[i] = (dac_sine_lut[i] << 6);
    }
    //Ya tengo los datos en memoria, ahora configuro el DMA
    confDMA();
    //Enable GPDMA channel 0
    while(1);
    return 0;
}

void confPin(void){
    PINSEL_CFG_Type PinCfg

    //Init DAC pin connect
    //AOUT on P0.26
    
    PinCfg.Portnum = PINSEL_PORT_0;
    PinCfg.Pinnum = PINSEL_PIN_26;
    PinCfg.Funcnum = PINSEL_FUNC_2;
    PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
    PinCfg.Opendrain = PINSEL_PINMODE_NORMAL;

    PINSEL_ConfigPin(&PinCfg); 
}

void confDMA(void){
    GPDMA_LLI_Type DMA_LLI_Struct;
    //Prepare DMA link list item structure
    DMA_LLI_Struct.SrcAddr = (uint32_t)dac_sine_lut;
    DMA_LLI_Struct.DstAddr = (uint32_t) & (LPC_DAC -> DACR);// Destino periferico DAC, asigno la direccion donde se encuetra el registro DACR(Ahi se guarda el dato que se quiere convertir de digital a analogico)
    DMA_LLI_Struct.NextLLI = (uint32_t) & DMA_LLI_Struct; //Se enlaza a si misma
    DMA_LLI_Struct.Control = DMA_SIZE 
                                        | (2<<18) //source width 32 bits
                                        | (2<<21) //dest width 32 bit
                                        | (1<<26) ;//source increment

    //Initialize GPDMA Controler
    GPDMA_Init();//Se hace un reset, limpian banderas y canales
    //Setup GPDMA Channel
    //channel 0
    GPDMACfg.ChannelNum = 0;
    //Source memory
    GPDMACfg.SrcMemAddr = (uint32_t)(dac_sine_lut);
    //Destination memory - unused
    GPDMACfg.DstMemAdrr = 0;
    //Transfer Size
    GPDMACfg.TransferSize = DMA_SIZE;
    //Transfer width - unused
    GPDMACfg.TransferWidth = 0;
    //Transfer type
    GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
    //Source conection - unused
    GPDMACfg.SrcConn = 0;
    //Destination connection
    GPDMACfg.DstConn = GPDMA_CONN_DAC;
    //Linked Item List - unused
    GPDMA.DMALLI = (uint32_t) & DMA_LLI_Struct;
    //Setup channel with given parameter
    GPDMA_Setup(&GPDMACfg);
    GPDMA_ChannelCmd(0 , ENABLE);
}

void confDac(void){
    uint32_t temp;
    DAC_CONVERTER_CFG_Type DAC_ConverterCfgStruct;
    DAC_ConverterCfgStruct.CNT_ENA = SET;
    DAC_ConverterCfgStruct.DMA_ENA = SET;

    DAC_Init(LPC_DAC);
    //Set time out for DAC
    tmp = (PCLK_DAC_IN_MHZ * 1000000) / (SINE_FREQ_IN_HZ * NUM_SINE_SAMPLE);
    DAC_SetDMATimeOut(LPC_DAC, tmp);
    DAC_ConfigDAConverterControl(LPC_DAC , &DAC_ConverterCfgStruct);
}
*/

//Ejemplo 2 transferencia memoria a memoria
/*#include "lpc17xx.h"
#include "lpc17xx_gpdma.h"
#define DMA_SIZE 16

//DMAScr_Buffer will be burn into flash when compile

const uint32_t DMASrc_Buffer[DMA_SIZE] = 
{
    0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
	0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
	0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F20,
	0x31323334, 0x35363738, 0x393A3D3C, 0x3D3E3F30
}

uint32_t DMADest_Buffer[DMA_SIZE];

volatile uint32_t Channel0_TC; //Terminal Counter Flag for channel 0. Cuando termina la transferencia
volatile uint32_t Chanel0_Err;//Error counter flag for channel 0. Indica si hubo error en la transferencia

void DMA_IRQHandler(void);//Declaro handler
void Buffer_Verify(void); //Verifica que los datos del destino sean los mismos que los de la fuente
void confDMA(void);
void confDac(void);

int main(void){
    confDMA();
    Channel0_TC = 0; // Reset terminal counter
    Channel0_Err = 0;//Reset error counter
    GPDMA_ChannelCmd(0,ENABLE); //Enable GPDMA channel 0
    NVIC_EnableIRQ(DMA_IRQ);//Enable GPDMA interrupt


}

void confDMA(void){
    GPDMA_Channel_CFG_Type GPDMACfg;
    NVIC_DisableIRQ(DMA_IRQn);//Disable DMA interrupt
    GPDMA_Init();//Initialize GPDMA controler

    GPDMACfg.ChanelNum = 0;
    GPDMACfg.SrcMemAddr = (uint32_t)DMASrc_Buffer;
    GPDMACfg.DstMemAddr = (uint32_t)DMADest_Buffer;
    GPDMACfg.TransferSize = DMA_SIZE; // Cantidad de elementos
    GPDMACfg.TransferWidth = GPDMA_WIDTH_WORD;
    GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2M;
    GPDMACfg.SrcConn = 0;//Source connection - unused
    GPDMACfg.DstConn = 0;//Destination connection - unused
    GPDMACfg.DMALLI = 0;//Linker List Item - unused
    GPDMA_Setup(&GPDMACfg);//Setup channel with given parameter

void Buffer_Verify(void){
    uint8_t i;
    uint32_t *src_addr = (uint32_t * )DMASrc_Buffer;
    uint32_t *dest_addr = (uint32-t *)DMADest_Buffer;
    for( i = 0; i < DNA_SIZE , i++ ){
        if( *src_addr++ != *dest_addr ){
            while(1){}
        }
    }
}

void DMA_IRQHandler(void){
    if(GPDMA_IntGetStatus(GPDMA_STAT_INT , 0)){ //Check interrupt status on channel 0
        //Check counter terminal status
        if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC , 0)){
            //Clear terminate counter interrupt pending
            GPDMA_ClearIntPending(GPDMA_STATCLR_INTTC, 0);
            Channel0_TC++;
        }
        if(GPDMA_IntGetStatus(GPDMA_STAT_INTERR, 0 )){
            //Clear error counter interrupt pending
            GPDMA_ClearIntPendig(GPDMA_STAT_INTERR , 0);
            Channel0_Err++;
        }
    }
    return;
}
}
*/

//Ejemplo 3 

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_adc.h"

#define DMA_SWAP_SIZE 10
#define TABLE_LENGTH 1034


//Declaro funciones de configuracion
void confDMA(void);
void confADC(void);

GPDMA_Channel_CFG_Type GPDMACfg;

uint32_t table[TABLE_LENGTH];
uint32_t random[2];
uint32_t aux_buffer[DMA_SWAP_SIZE];

GPDMA_LLI_Type DMA_LLI_Struct1;
GPDMA_LLI_Type DMA_LLI_Struct2;
GPDMA_LLI_Type DMA_LLI_Struct3;

uint32_t bitcount;

int main(void){// A partir del ADC genero un valor aleatorio
    uint32_t i;
    random[0] = 545;
    random [1] = 433;
    bitcount = 0;

    //Prepare table
    for(int i = 0; i < TABLE_LENGTH ; i++){
        table[i] = i;
    }
    confADC();
    while(1);
    return 0;
}

void confDMA(void){
    //Declaro las 3 listas
    //Prepare DMA link list itm structure

    DMA_LLI_Struct1.SrcAddr = (uint32_t)table + 4*random[0];
    DMA_LLI_Struct1.DstAddr = (uint32_t)aux_buffer;
    DMA_LLI_Struct1.NextLLI = (uint32_t) & DMA_LLI_Struct2;
    DMA_LLI_Struct1.Control = DMA_SWAP_SIZE
                |(2<<18)//source width 32 bits
                |(2<<36)//dest width 32 bits
                |(1<<26)//source increment
                |(1<<27)//dest increment
    ;

    DMA_LLI_Struct2.SrcAddr = (uint32_t)table + 4*random[1];
    DMA_LLI_Struct2.DstAddr = (uint32_t)table + 4*random[0]; //??
    DMA_LLI_Struct2.NextLLI = (uint32_t) & DMA_LLI_Struct3;
    DMA_LLI_Struct2.Control = DMA_SWAP_SIZE
                |(2<<18)//source width 32 bits
                |(2<<36)//dest width 32 bits
                |(1<<26)//source increment
                |(1<<27)//dest increment
    ;

    DMA_LLI_Struct3.SrcAddr = (uint32_t)aux_buffer;
    DMA_LLI_Struct3.DstAddr = (uint32_t)table + 4*random[1];
    DMA_LLI_Struct3.NextLLI = 0;//End of list
    DMA_LLI_Struct3.Control = DMA_SWAP_SIZE
                |(2<<18) //source width 32 bits
                |(2<<36)//dest width 32 bits
                |(1<<26)//source increment
                |(1<<27)//dest increment
    ;

    GPDMA_Init(); //inicializo dma
    //Configuro el canal gpdma
    GPDMACfg.ChannelNum = 7;
    GPDMACfg.SrcMemAdrr = DMA_LLI_Struct1.SrcAddr;
    GPDMACfg.DstMemAdrr = DMA_LLI_Struct1.DstAddr;
    GPDMACfg.TransferSize = DMA_SWAP_SIZE;
    GPDMACfg.TransferWidth = GPDMA_TRANSFERTYPE_M2M;
    GDMACfg.SrcConn = 0; //Source connection unused
    GPDMACfg.DstConn = 0;//Destination connection unused
    GPDMACfg.DMALLI = (uint32_t) & DMA_LLI_Struct2;
    GPDMA_Setup(&GPDMACfg); //Setup channel with given parameter
}

void ConfADC(void){
    ADC_Init(LPC_ADC, 2000);
    LPC_SC -> PCONP | = (1 << 12); //Peripheral ADC enable
    LPC_SC ->ADCR | = (1<<21);     
}



    






