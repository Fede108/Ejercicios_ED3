/*
Considerando que se tiene un bloque de datos comprendidos entre las posiciones de memorias, dirección inicial=0x10000800 
a la dirección final= 0x10001000 ambas inclusive y se desea trasladar este bloque de datos una nueva zona de memoria 
comprendida entre la dirección inicial= 0x10002800 y la dirección Final=0x10003000 (en el mismo orden). Teniendo en cuenta 
además que los datos contenidos dentro de la zona de memoria son de 16 bits (AHB Master endianness configuration - por defecto) 
y que estos deben moverse de a uno (1) en cada evento de DMA, se sincronizará la transmisión con evento de match0 del timer0.
*/

#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpdma.h"

#define MILISEGUNDO 10

#define ORIGEN_BASE  0x10000800[1bytes == 8bits]  
                     0x10000801[1bytes == 8bits] 

                     0x10000810[1bytes == 8bits] 
                     0x10000810[1bytes == 8bits]        


                     0x10001000


#define DESTINO_BASE 0x10002800

uint32_t cantidad_muestras_trasnferir = ((0x10001000 - 0x10000800)/2);  // Se divido por dos porque los datos son de 16 bits es decir 2 bytes
uint32_t byte_actual = 0;   

uint32_t src_addr = 0;
uint32_t dst_addr = 0;


void configurar_timer(void){
    TIM_TIMER_CFG_Type timer_cfg;
    timer_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    timer_cfg.PrescaleValue = 100;
    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timer_cfg);

    TIM_MATCH_CFG_Type match_cfg;
    match_cfg.MatchChannel = 0;
    match_cfg.ItsOnMatch = ENABLE;
    match_cfg.StopOnMatch = DISABLE;
    match_cfg.ResetOnMatch = ENABLE;
    match_cfg.MatchValue = MILISEGUNDO;
    match_cfg.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
    TIM_ConfigMatch(LPC_TIM0, &match_cfg);

    TIM_Cmd(LPC_TIM0, ENABLE);
    NVIC_EnableIRQ(TIMER0_IRQn);
}

void configurar_dma(uint32_t src_addr, uint32_t dst_addr){
    GPDMA_Channel_CFG_Type channel_cfg;

    GPDMA_Init();

    channel_cfg.ChannelNum = 0;
    channel_cfg.TransferSize = 1;
    channel_cfg.TransferWidth = GPDMA_WIDTH_HALFWORD;
    channel_cfg.SrcMemAddr = src_addr;
    channel_cfg.DstMemAddr = dst_addr;
    channel_cfg.TransferType = GPDMA_TRANSFERTYPE_M2M;
    channel_cfg.SrcConn = 0;
    channel_cfg.DstConn = 0;
    channel_cfg.DMALLI = 0;

    GPDMA_Setup(&channel_cfg);
    GPDMA_ChannelCmd(0, ENABLE);
}

/**
 * @brief Manejador de interrupciones del Timer0
 *
 * Se encarga de limpiar la bandera de interrupcion del Timer0 y
 * habilitar el canal 0 del GPDMA para que comience a transferir
 * los datos de la memoria flash a la memoria ram.
 */
void Timer0_IRQHandler(void) {
    // Limpiar la interrupción del Timer0
    TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
    
    // Verificar si aun hay datos por tarnsferir
    if(byte_actual < tamaño_completo_a_transferir){
        // Verificar si la transferencia del DMA ya ha finalizado
        if (GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 0)) {
            GPDMA_ChannelCmd(0, DISABLE);

            // Actualizar el siguiente bloque de memoria
            src_addr = ORIGEN_BASE + byte_actual * 2;  // Multiplicamos por 2 ya que son datos de 16 bits
            dst_addr = DESTINO_BASE + byte_actual * 2;

            // Configurar la siguiente transferencia DMA
            configurar_dma(src_addr, dst_addr);
        
            byte_actual++;
        }
    }
    else{
        NVIC_DisableIRQ(TIMER0_IRQn);
        TIM_Cmd(LPC_TIM0, DISABLE);  // Detenemos el timer
    }
}

int main(void){
    configurar_timer();
    while(1){
        // No hacer nada
    }
    return 0;

}


#include "LPC17xx.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_timer.h"

// Memory addresses
#define SRC_ADDRESS 0x10000800
#define DST_ADDRESS 0x10002800

// DMA definitions
#define DMA_CHANNEL_0 0
#define REQUIRED_TRANSFERS 256; // 0x10003000 - 0x10002800 = 0x200 bytes = 512 bytes -> 512 / 2 bytes (transferencias de 16 bits) = 256 transferencias

// Global variables
volatile uint16_t remaining_transfers = REQUIRED_TRANSFERS; // De 16 bits porque son 256 transferencias (8 bits llega a 255)

// Required structures for DMA
GPDMA_Channel_CFG_Type dma;
GPDMA_LLI_Type lli;

void configure_dma(void) {
    lli.SrcAddr = (uint32_t)SRC_ADDRESS;
    lli.DstAddr = (uint32_t)DST_ADDRESS;
    lli.NextLLI = (uint32_t)&lli;
    lli.Control = 1        // 1 transferencia a la vez
                | (1<<17)  // Transferencia a la fuente de 16 bits
                | (1<<21)  // Transferencia de destino de 16 bits
                | (1<<25)  // Incrementa direccion de origen despues de cada transferencia
                | (1<<26)  // Incrementa direccion de destino despues de cada transferencia
                | (1<<30)  // Generar interrupcion al finalizar transferencia y detener transferencia (se reinicia manualmente)

    // Configuración del canal DMA
    dma.ChannelNum = DMA_CHANNEL_0;                  // Canal DMA 0
    dma.TransferSize = 1;                            // 1 transferencia a la vez
    dma.TransferWidth = GPDMA_WIDTH_HALFWORD;        // Transferencias de 16 bits
    dma.SrcMemAddr = (uint32_t)SRC_ADDRESS;          // Dirección de origen
    dma.DstMemAddr = (uint32_t)DST_ADDRESS;          // Dirección de destino
    dma.TransferType = GPDMA_TRANSFERTYPE_M2M;       // Transferencia memoria a memoria
    dma.DMALLI = (uint32_t)&lli;                     // Linked List Item
    dma.SrcConn = 0;                                 // No hay periférico de origen
    dma.DstConn = 0;                                 // No hay periférico de destino
    
    // Configurar el DMA
    GPDMA_Setup(&dma_cfg);
    
    // Iniciar modulo DMA
    GPDMA_Init();

    // Habilita interrupcion del DMA
    NVIC_EnableIRQ(DMA_IRQn);
    // Transferencia > Orden de transferir otro dato
    NVIC_SetPriority(DMA_IRQn, 0);
}

// Configurar Timer1 y Match0 para generar un evento periódico
void configure_timer1(void) {
    TIM_TIMERCFG_Type timer_cfg;
    TIM_MATCHCFG_Type match_cfg;

    // Configuración básica del timer
    timer_cfg.PrescaleOption = TIM_PRESCALE_USVAL;
    timer_cfg.PrescaleValue = 1;  // El contador aumenta cada 1 us
    TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &timer_cfg);

    // Configurar Match0 para generar eventos
    match_cfg.MatchChannel = 0;
    match_cfg.IntOnMatch = ENABLE;
    match_cfg.ResetOnMatch = ENABLE;
    match_cfg.StopOnMatch = ENABLE; // Cuando se llega al Match, se reinicia y detiene la cuenta hasta finalizar la transferencia
    match_cfg.MatchValue = 1000; // Generar evento cada 1 ms (1 us * 1000)
    TIM_ConfigMatch(LPC_TIM1, &match_cfg);

    // Habilita interrupcion del Timer1
    NVIC_EnableIRQ(Timer1_IRQn);
    // Transferencia > Orden de transferir otro dato
    NVIC_SetPriority(Timer1_IRQn, 3);
}

void TIMER1_IRQHandler(void) {
    TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT); 
    GPDMA_ClearIntPending(GPDMA_STATCLR_INTTC, DMA_CHANNEL_0);
}

void DMA_IRQHandler() {
    while (!(GPDMA_IntGetStatus(GPDMA_STAT_INT, 0)))
    {
        /* code */
    }
}

int main(void) {
    // Iniciar system clk
    SystemInit();

    // Configurar el Timer1 y el DMA
    configure_timer1();
    configure_dma();
   
    // Iniciar Timer1
    TIM_Cmd(LPC_TIM1, ENABLE);

    while (1) {
        // El DMA se encargará de la transferencia
    }

    return 0;
}