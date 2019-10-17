#ifndef __uart_h
#define __uart_h
#include "stdint.h"
#include "string.h"
//==========================================================
//��������
#define _HW_CONFIG_FALSE	(0)
#define _HW_CONFIG_TRUE		(1)

//==========================================================
//�շ�֡���峤��
#define REC_LEN_MAX  	(40)//����֡����
#define SEND_LEN_MAX 	(18)//����֡����

//���ڷ��ͽṹ
typedef struct
{
	int8_t begin;//���Ϳ�ʼ��־
	int8_t ptr;//��ǰ����ָ��
	int8_t len;//�������ݳ���
	int8_t buf[SEND_LEN_MAX];//���ݷ��ͻ���
}ComSend_TypeDef;

//���ڽ��սṹ
typedef struct
{
	int8_t end;//���ս�����־
	int8_t ptr;	//��ǰ����ָ��
	int8_t len;//�������ݳ���
	int8_t buf[REC_LEN_MAX];//���ݽ��ջ���
	int8_t TimeOut;//���ճ�ʱ(10ms����)
}ComRec_TypeDef;

//�����շ��ṹ
typedef struct
{
	ComSend_TypeDef send;
	ComRec_TypeDef  rec;
}Com_TypeDef;
extern Com_TypeDef ComBuf;//�����շ�����
extern Com_TypeDef ComBuf3;
//==========================================================
//ͨ��Э�飬8λ��9600������
//==========================================================
//��ʼ��  �ܳ���  Ŀ���ַ  ������  ���ݰ�  �ۼӺ�  ������
//1�ֽ�	  1�ֽ�	  1�ֽ�	    1�ֽ�	N�ֽ�   1�ֽ�   1�ֽ�
//==========================================================
#define UART_SEND_BEGIN 		(0xAB)//(0xAB)
#define UART_SEND_END 			(0xAF)//(0xAF)
#define FRAME_LEN_MAX 			(REC_LEN_MAX)//һ֡����ֽ�
#define FRAME_LEN_MIN 			(3)	//һ֡��С�ֽ�
#define UART_REC_BEGIN 			(0xAA)//(0xAA)
#define UART_REC_END 			(0xBF)//(0xBF)

//֡���Ͷ���
#define FRAME_READ_RESULT		(0x00)	//��ȡ���
#define FRAME_START				(0x01)	//����
#define FRAME_RESET				(0x02)	//��λ
#define FRAME_DATA				(0xF1)	//����֡
#define FRAME_CLR_BOOT_NUM		(0xF2)	//�忪������֡
#define FRAME_WRITE_SN			(0xF3)	//д���к�֡

//֡λ�ö���
#define PFRAMEHEAD				(0)	//֡ͷλ��
#define PFRAMEKIND				(1)	//֡����λ��
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

#endif

