#include "lpc177x_8x_gpdma.h"
#include "debug_frmwrk.h"
#include "lpc177x_8x_pinsel.h"
#include "stdarg.h"
#include "Globalvalue/GlobalValue.h"

#define DMA_SIZE        8
#define DMASEND_SIZE    21
uint8_t DMADest_Buffer[DMA_SIZE];
uint8_t DMAsend_Buffer[DMASEND_SIZE];
GPDMA_Channel_CFG_Type GPDMACfg;
GPDMA_Channel_CFG_Type GPDMACfg1;

void DMA_IRQHandler (void)
{
		
        uint8_t i,ch;
		for(ch = 0;ch < 2;ch++)
		{
			if (GPDMA_IntGetStatus(GPDMA_STAT_INT, ch))/* ?��2��DMA�����̨�0?D??���䨬? */
			{
	//                GPDMA_ChannelCmd(0, DISABLE);
					if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, ch))
					{                       
							GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, ch);      
	//                        for(i=0;i<DMA_SIZE;i++){printf("%d",DMADest_Buffer[i]);}
	//                        printf("\r\n");
							switch(ch)
							{
								case 0:
								{
									for(i = 0;i < 8;i ++)
									{
										g_tModS.RxBuf[i] = DMADest_Buffer[i];
									}
									g_tModS.RxCount = 8;
									RecHandle();
									g_tModS.RxCount = 0;
									LPC_GPDMACH0->CDestAddr = GPDMACfg.DstMemAddr;// Assign memory destination address
									LPC_GPDMACH0->CControl= GPDMA_DMACCxControl_TransferSize((uint32_t)GPDMACfg.TransferSize) \
									| GPDMA_DMACCxControl_SBSize((uint32_t)GPDMA_LUTPerBurst[GPDMACfg.SrcConn]) \
									| GPDMA_DMACCxControl_DBSize((uint32_t)GPDMA_LUTPerBurst[GPDMACfg.SrcConn]) \
									| GPDMA_DMACCxControl_SWidth((uint32_t)GPDMA_LUTPerWid[GPDMACfg.SrcConn]) \
									| GPDMA_DMACCxControl_DWidth((uint32_t)GPDMA_LUTPerWid[GPDMACfg.SrcConn]) \
									| GPDMA_DMACCxControl_DI \
									| GPDMA_DMACCxControl_I;
									GPDMA_ChannelCmd(0, ENABLE);
								}break;
								case 1:
								{
									GPDMA_ChannelCmd(1, DISABLE);
								}break;
							}
							     
					}
					if (GPDMA_IntGetStatus(GPDMA_STAT_INTERR, ch))                /* ?��2��DMA�����̨�0?D??�䨪?�����䨬? */
					{
							GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, ch);//Channel0_Err++;        /* ??3yDMA�����̨�0?D??�䨪?��???�� */
					}
					
			}
		}
}

void lpc1788_DMA_Init(void)
{
//        GPDMA_Channel_CFG_Type GPDMACfg;

		GPDMA_Init();  
        NVIC_DisableIRQ(DMA_IRQn);               
        NVIC_SetPriority(DMA_IRQn, ((0x01<<3)|0x01));
            
	//DMA USART RX CONFIG	
        GPDMACfg.ChannelNum = 0;
        GPDMACfg.SrcMemAddr =0;       
        GPDMACfg.DstMemAddr = (uint32_t)&DMADest_Buffer;       
        GPDMACfg.TransferSize = sizeof(DMADest_Buffer);
        GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_P2M;       
        GPDMACfg.SrcConn = GPDMA_CONN_UART3_Rx;       
        GPDMACfg.DstConn = 0;       
        GPDMACfg.DMALLI = 0;       

        GPDMA_Setup(&GPDMACfg);
       
	//DMA USART TX CONFIG	
        GPDMACfg1.ChannelNum = 1;
        GPDMACfg1.SrcMemAddr =(uint32_t)&g_tModS.TxBuf;       
        GPDMACfg1.DstMemAddr = 0;       
        GPDMACfg1.TransferSize = 23;
		GPDMACfg1.TransferWidth = 0;
        GPDMACfg1.TransferType = GPDMA_TRANSFERTYPE_M2P;       
        GPDMACfg1.SrcConn = 0;       
        GPDMACfg1.DstConn = GPDMA_CONN_UART3_Tx;       
        GPDMACfg1.DMALLI = 0;       
        GPDMA_Setup(&GPDMACfg1);
//        LPC_SSP0->DMACR |=0x11;//SSP_DMACmd (0, SSP_DMA_RXDMA_EN, ENABLE);
       
        NVIC_EnableIRQ(DMA_IRQn);
        GPDMA_ChannelCmd(0, ENABLE);
		GPDMA_ChannelCmd(1, DISABLE);
}

void DMASendInit(void)
{
//	LPC_GPDMACH1->CDestAddr = GPDMACfg1.DstMemAddr;// Assign memory destination address
//	LPC_GPDMACH1->CControl= GPDMA_DMACCxControl_TransferSize((uint32_t)GPDMACfg1.TransferSize) \
//	| GPDMA_DMACCxControl_SBSize((uint32_t)GPDMA_LUTPerBurst[GPDMACfg1.SrcConn]) \
//	| GPDMA_DMACCxControl_DBSize((uint32_t)GPDMA_LUTPerBurst[GPDMACfg1.SrcConn]) \
//	| GPDMA_DMACCxControl_SWidth((uint32_t)GPDMA_LUTPerWid[GPDMACfg1.SrcConn]) \
//	| GPDMA_DMACCxControl_DWidth((uint32_t)GPDMA_LUTPerWid[GPDMACfg1.SrcConn]) \
//	| GPDMA_DMACCxControl_DI \
//	| GPDMA_DMACCxControl_I;
//	GPDMACfg1.TransferSize = g_tModS.TxCount;
//	LPC_GPDMACH1->CDestAddr = GPDMACfg1.DstMemAddr;// Assign memory destination address
//									LPC_GPDMACH1->CControl= GPDMA_DMACCxControl_TransferSize((uint32_t)GPDMACfg1.TransferSize) \
//									| GPDMA_DMACCxControl_SBSize((uint32_t)GPDMA_LUTPerBurst[GPDMACfg1.SrcConn]) \
//									| GPDMA_DMACCxControl_DBSize((uint32_t)GPDMA_LUTPerBurst[GPDMACfg1.SrcConn]) \
//									| GPDMA_DMACCxControl_SWidth((uint32_t)GPDMA_LUTPerWid[GPDMACfg1.SrcConn]) \
//									| GPDMA_DMACCxControl_DWidth((uint32_t)GPDMA_LUTPerWid[GPDMACfg1.SrcConn]) \
//									| GPDMA_DMACCxControl_DI \
//									| GPDMA_DMACCxControl_I;
	 GPDMACfg1.ChannelNum = 1;
        GPDMACfg1.SrcMemAddr =(uint32_t)&g_tModS.TxBuf;       
        GPDMACfg1.DstMemAddr = 0;       
        GPDMACfg1.TransferSize = g_tModS.TxCount;
		GPDMACfg1.TransferWidth = 0;
        GPDMACfg1.TransferType = GPDMA_TRANSFERTYPE_M2P;       
        GPDMACfg1.SrcConn = 0;       
        GPDMACfg1.DstConn = GPDMA_CONN_UART3_Tx;       
        GPDMACfg1.DMALLI = 0;       
        GPDMA_Setup(&GPDMACfg1);
		
	GPDMA_ChannelCmd(1, ENABLE);
}

