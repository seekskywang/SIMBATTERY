#include "lpc177x_8x_gpdma.h"
#include "debug_frmwrk.h"
#include "lpc177x_8x_pinsel.h"
#include "stdarg.h"
#include "Globalvalue/GlobalValue.h"

#define DMA_SIZE        64
uint8_t DMADest_Buffer[DMA_SIZE]; 

void DMA_IRQHandler (void)
{
        uint8_t i;
        if (GPDMA_IntGetStatus(GPDMA_STAT_INT, 0))/* ?¨¬2¨¦DMA¨ª¡§¦Ì¨¤0?D??¡Á¡ä¨¬? */
        {
                GPDMA_ChannelCmd(0, DISABLE);
                if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 0))/* ?¨¬2¨¦DMA¨ª¡§¦Ì¨¤0??????¨ºy???¨®¡Á¡ä¨¬?¡ê??¨¢¨¨?DMACIntTCStatus??¡ä??¡Â¨¤¡ä?D???D??¨º?¡¤?¨°¨°?a¡ä?¨º?¦Ì??¨¢¨º???2¨²¨¦¨²¡ê¡§??????¨ºy¡ê? */
                {                       
                        GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, 0);/* ??3yDMA¨ª¡§¦Ì¨¤0??????¨ºy?D?????¨® */       
                        for(i=0;i<DMA_SIZE;i++){printf("%d",DMADest_Buffer[i]);}
                        printf("\r\n");
                        LPC_GPDMACH0->CDestAddr = GPDMACfg.DstMemAddr;// Assign memory destination address
                        LPC_GPDMACH0->CControl= GPDMA_DMACCxControl_TransferSize((uint32_t)GPDMACfg.TransferSize)\
                                                                                                                | GPDMA_DMACCxControl_SBSize((uint32_t)GPDMA_LUTPerBurst[GPDMACfg.SrcConn]) \
                                                                                                                | GPDMA_DMACCxControl_DBSize((uint32_t)GPDMA_LUTPerBurst[GPDMACfg.SrcConn]) \
                                                                                                                | GPDMA_DMACCxControl_SWidth((uint32_t)GPDMA_LUTPerWid[GPDMACfg.SrcConn]) \
                                                                                                                | GPDMA_DMACCxControl_DWidth((uint32_t)GPDMA_LUTPerWid[GPDMACfg.SrcConn]) \
                                                                                                                | GPDMA_DMACCxControl_DI \
                                                                                                                | GPDMA_DMACCxControl_I;       
                }
                if (GPDMA_IntGetStatus(GPDMA_STAT_INTERR, 0))                /* ?¨¬2¨¦DMA¨ª¡§¦Ì¨¤0?D??¡ä¨ª?¨®¡Á¡ä¨¬? */
                {
                        GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, 0);//Channel0_Err++;        /* ??3yDMA¨ª¡§¦Ì¨¤0?D??¡ä¨ª?¨®???¨® */
                }
                GPDMA_ChannelCmd(0, ENABLE);
        }
}

void lpc1788_DMA_Init(void)
{
        GPDMA_Channel_CFG_Type GPDMACfg;

        NVIC_DisableIRQ(DMA_IRQn);               
        NVIC_SetPriority(DMA_IRQn, ((0x01<<3)|0x01));
        GPDMA_Init();       
        GPDMACfg.ChannelNum = 0;
        GPDMACfg.SrcMemAddr =0;       
        GPDMACfg.DstMemAddr = (uint32_t)DMADest_Buffer;       
        GPDMACfg.TransferSize = sizeof(DMADest_Buffer);
        GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_P2M;       
        GPDMACfg.SrcConn = GPDMA_CONN_SSP0_Rx;       
        GPDMACfg.DstConn = 0;       
        GPDMACfg.DMALLI = 0;       

        GPDMA_Setup(&GPDMACfg);
       
        LPC_SSP0->DMACR |=0x11;//SSP_DMACmd (0, SSP_DMA_RXDMA_EN, ENABLE);
       
        NVIC_EnableIRQ(DMA_IRQn);
        GPDMA_ChannelCmd(0, ENABLE);
}
