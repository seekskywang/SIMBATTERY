/**********************************************************************
* $Id$		debug_frmwrk.h			2011-06-02
*//**
* @file		debug_frmwrk.h
* @brief	Contains some utilities that used for debugging through UART
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
* 
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/

#ifndef __DEBUG_FRMWRK_H_
#define __DEBUG_FRMWRK_H_
#include "Globalvalue/GlobalValue.h"
#include "lpc177x_8x_uart.h"

#define USED_UART_DEBUG_PORT	0

#define NUM_SKIPPED_ALLOWED			(10)

#define DBG_GETVAL_IN_DEC			(0)
#define DBG_GETVAL_IN_HEX			(1)
//#define DBG_GETVAL_IN_CHARS			(2) 

#define	USED_UART_DEBUG_PORT3   3
#define USED_UART_PORT3			(LPC_UART_TypeDef *)LPC_UART3



#if (USED_UART_DEBUG_PORT == 0)
#define DEBUG_UART_PORT	(LPC_UART_TypeDef *)LPC_UART0
#elif (USED_UART_DEBUG_PORT == 1)
#define DEBUG_UART_PORT	(LPC_UART_TypeDef *)LPC_UART1
#elif (USED_UART_DEBUG_PORT == 2)
#define DEBUG_UART_PORT	(LPC_UART_TypeDef *)LPC_UART2
#elif (USED_UART_DEBUG_PORT == 3)
#define DEBUG_UART_PORT	(LPC_UART_TypeDef *)LPC_UART3
#elif (USED_UART_DEBUG_PORT == 4)
#define DEBUG_UART_PORT	(LPC_UART_TypeDef *)LPC_UART4
#else
 #error "Invalid UART port selection!"
#endif

#define _DBG(x)								_db_msg(DEBUG_UART_PORT, x)
#define _DBG_(x)							_db_msg_(DEBUG_UART_PORT, x)

#define _DBC(x)								_db_char(DEBUG_UART_PORT, x)

#define _DBD(x)								_db_dec(DEBUG_UART_PORT, x)
#define _DBD16(x)							_db_dec_16(DEBUG_UART_PORT, x)
#define _DBD32(x)							_db_dec_32(DEBUG_UART_PORT, x)

#define _DBH(x)								_db_hex(DEBUG_UART_PORT, x)
#define _DBH16(x)							_db_hex_16(DEBUG_UART_PORT, x)
#define _DBH32(x)							_db_hex_32(DEBUG_UART_PORT, x)

#define _DBH_(x)							_db_hex_(DEBUG_UART_PORT, x)
#define _DBH16_(x)							_db_hex_16_(DEBUG_UART_PORT, x)
#define _DBH32_(x)							_db_hex_32_(DEBUG_UART_PORT, x)

#define _DG									_db_get_char(DEBUG_UART_PORT)
#define _DGV(option, numCh, val)			_db_get_val(DEBUG_UART_PORT, option, numCh, val)


//==========================================================
//��������
#define _HW_CONFIG_FALSE	(0)
#define _HW_CONFIG_TRUE		(1)

//==========================================================
//�շ�֡���峤��
#define REC_LEN_MAX  	(60)//����֡����
#define SEND_LEN_MAX 	(18)//����֡����

////���ڷ��ͽṹ
typedef struct
{
	int8_t begin;//���Ϳ�ʼ��־
	int8_t ptr;//��ǰ����ָ��
	int8_t len;//�������ݳ���
	int8_t buf[SEND_LEN_MAX];//���ݷ��ͻ���
}ComSend_TypeDef;
#define TICKS_PER_SEC 100	//ϵͳʱ��ÿ���������100x10mS(T0)=1S
//�������ʱ��,3*1000/50=60mS
#define REC_TIME_OUT	 (3*TICKS_PER_SEC/50)
//���ڽ��սṹ
typedef struct
{
	int8_t end;//���ս�����־
	int8_t ptr;	//��ǰ����ָ��
	int8_t len;//�������ݳ���
	int8_t buf[REC_LEN_MAX];//���ݽ��ջ���
	int8_t TimeOut;//���ճ�ʱ(10ms����)
}ComRec_TypeDef;

////�����շ��ṹ
typedef struct
{
	ComSend_TypeDef send;
	ComRec_TypeDef  rec;
}Com_TypeDef;
extern Com_TypeDef ComBuf;//�����շ�����
//==========================================================
//ͨ��Э�飬8λ��9600������
//==========================================================
//��ʼ��  �ܳ���  Ŀ���ַ  ������  ���ݰ�  �ۼӺ�  ������
//1�ֽ�	  1�ֽ�	  1�ֽ�	    1�ֽ�	N�ֽ�   1�ֽ�   1�ֽ�
//==========================================================
#define UART_SEND_BEGIN 	    (0xAB)//(0xAB)
#define UART_SEND_END 			(0xAF)//(0xAF)
#define FRAME_LEN_MAX 			(REC_LEN_MAX)//һ֡����ֽ�
#define FRAME_LEN_MIN 			(3)	//һ֡��С�ֽ�
#define UART_REC_BEGIN 			(0xAB)//(0xAA)
#define UART_REC_END 			(0xBF)//(0xBF)
#define UART_REC_END1           UART_SEND_END

//֡���Ͷ���
#define FRAME_READ_RESULT		(0x22)	//��ȡ���
#define FRAME_START				(0x01)	//����
#define FRAME_RESET				(0x02)	//��λ
#define FRAME_DATA				(0xF1)	//����֡
#define FRAME_CLR_BOOT_NUM		(0xF2)	//�忪������֡
#define FRAME_WRITE_SN			(0xF3)	//д���к�֡

//֡λ�ö���
#define PFRAMEHEAD				(0)	//֡ͷλ��
#define PFRAMEKIND				(3)	//֡����λ��
#define PDATASTART				(2)	//���ݰ��׵�ַλ��

//ͨ��״̬����
#define COM_SUCCESS				(0x00)	//�ɹ�
#define COM_ERR_BUSY	 	 	(0x01)	//�ŵ�æ
#define COM_ERR_CRC				(0x02)	//У�����
#define COM_ERR_NACK 			(0x03)	//��Ӧ��
#define COM_ERR_UNEXPECT 		(0x04)	//������֡
#define COM_ERR_FORMAT			(0x05)	//���ݸ�ʽ����
#define COM_ERR_SAVE 			(0x06)	//�޷���������
#define COM_ERR_ADDR_FIRE	 	(0x07)	//��ַ��ͻ
#define COM_ERR_UNDEFINE		(0x08)	//δ�������
//void  _printf (const  char *format, ...);
#define SetRecTimeOut(time) (ComBuf.rec.TimeOut=time)
extern Com_TypeDef ComBuf3;

extern const uint8_t READDATA[7];
extern void (*_db_msg)(LPC_UART_TypeDef *UARTx, const void *s);
extern void (*_db_msg_)(LPC_UART_TypeDef *UARTx, const void *s);
extern void (*_db_char)(LPC_UART_TypeDef *UARTx, uint8_t ch);
extern void (*_db_dec)(LPC_UART_TypeDef *UARTx, uint8_t decn);
extern void (*_db_dec_16)(LPC_UART_TypeDef *UARTx, uint16_t decn);
extern void (*_db_dec_32)(LPC_UART_TypeDef *UARTx, uint32_t decn);
extern void (*_db_hex)(LPC_UART_TypeDef *UARTx, uint8_t hexn);
extern void (*_db_hex_16)(LPC_UART_TypeDef *UARTx, uint16_t hexn);
extern void (*_db_hex_32)(LPC_UART_TypeDef *UARTx, uint32_t hexn);
extern void (*_db_hex_)(LPC_UART_TypeDef *UARTx, uint8_t hexn);
extern void (*_db_hex_16_)(LPC_UART_TypeDef *UARTx, uint16_t hexn);
extern void (*_db_hex_32_)(LPC_UART_TypeDef *UARTx, uint32_t hexn);

extern uint8_t (*_db_get_char)(LPC_UART_TypeDef *UARTx);
extern uint8_t (*_db_get_val)(LPC_UART_TypeDef *UARTx, uint8_t option, uint8_t numCh, uint32_t * val);

uint8_t UARTGetValue (LPC_UART_TypeDef *UARTx, uint8_t option,
											uint8_t numCh, uint32_t* val);
void UARTPutChar (LPC_UART_TypeDef *UARTx, uint8_t ch);
extern void UARTPuts(LPC_UART_TypeDef *UARTx, const void *str);
void UARTPuts_(LPC_UART_TypeDef *UARTx, const void *str);
void UARTPuts(LPC_UART_TypeDef *UARTx, const void *str);
void UARTPutDec(LPC_UART_TypeDef *UARTx, uint8_t decnum);
void UARTPutDec16(LPC_UART_TypeDef *UARTx, uint16_t decnum);
void UARTPutDec32(LPC_UART_TypeDef *UARTx, uint32_t decnum);
void UARTPutHex (LPC_UART_TypeDef *UARTx, uint8_t hexnum);
void UARTPutHex16 (LPC_UART_TypeDef *UARTx, uint16_t hexnum);
void UARTPutHex32 (LPC_UART_TypeDef *UARTx, uint32_t hexnum);
uint8_t UARTGetChar (LPC_UART_TypeDef *UARTx);
//void  _printf (const  char *format, ...);
void debug_frmwrk_init(void);

void  debug_uart3_init(uint32_t freq);
//void Send_Uart3(uint8_t *buff);
//void Send_Freq(Send_Ord_Typedef *ord);

#endif /* __DEBUG_FRMWRK_H_ */
