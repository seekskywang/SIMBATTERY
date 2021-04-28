#ifndef __test_h
#define __test_h
#include "stdint.h"
#include  "Globalvalue/globalvalue.h"

#define  CmdNumb         17
#define  CmdLen          12 

#define ChrStartR      0x3E
#define ChrStartS      0x3C

#define ChrEndR    	   0x0D
#define ChrEndS    	   0x0A

#define PARA_ERR       1
#define CMD_ERR        2
#define REM_ERR        3

#define EncoderA  ((GPIO_ReadValue(2) & (1<<23))>>23)
#define EncoderB  ((GPIO_ReadValue(2) & (1<<22))>>22)

#define EnccounterA  LPC_TIM3->CR1
#define EnccounterB  LPC_TIM3->CR0

extern const uint8_t Disp_Main_Ord[][3];
extern uint8_t CorrectionflagC,CorrectionflagR,Correc_successflag;
extern uint8_t coder_flag;
extern uint8_t spinbit;
extern uint8_t spinbitmax;
extern uint8_t bootdelay;
void Power_Process(void);


//���Գ���
void Test_Process(void);

//�ļ��������
void File_Process(void);

//�������ó���   ��������
void Setup_Process(void);
//���ݱ������
void Data_StoreProcess(void) ;
//������ʾ
void Range_Process(void);
//��������ʾ
void Range_CountProcess(void);
 // �б���ʾ
void ItemProcess(void);
//�û�У��
void Use_DebugProcess(void) ;
uint16_t Uart_Process(uint8_t len,char* buf);
void Fac_DebugProcess(void);
void Use_SysSetProcess(void);//ϵͳ����
void Use_LimitSetProcess(void);//�����б�����
void Use_ITEMSetProcess(void);//�б�ɨ������
int8_t PackStandFrame(int8_t * framebuf, int8_t * datbuf, int8_t len);
Sort_TypeDef Disp_NumKeyboard_Set(Disp_Coordinates_Typedef *Disp_Coordinates);
Sort_TypeDef Disp_NumKeyboard_time(Disp_Coordinates_Typedef *Disp_Coordinates);
Sort_TypeDef Disp_Set_Num(Disp_Coordinates_Typedef *Coordinates);
Sort_TypeDef Disp_Set_C(Disp_Coordinates_Typedef *Coordinates);
Sort_TypeDef Disp_Set_T(Disp_Coordinates_Typedef *Coordinates);
uint8_t Freq_Set_Num(Disp_Coordinates_Typedef *Coordinates);
uint8_t Avg_Set_Num(Disp_Coordinates_Typedef *Coordinates);//ƽ��������
Sort_TypeDef Disp_Set_InputNum(Disp_Coordinates_Typedef *Coordinates);
void Set_daot(uint8_t *buff,uint8_t dot);
Sort_TypeDef Disp_Set_InputpreNum(Disp_Coordinates_Typedef *Coordinates);
Sort_TypeDef Disp_Set_InputpreSecNum(Disp_Coordinates_Typedef *Coordinates);
void Set_Compbcd_float(void);
uint8_t Test_Comp(All_Compvalue_Typedef *pt);
void Soft_Turnon(void);
void Test_Comp_Fmq(void);
uint8_t Uart3_Process(void);
void EncodeScan();

void  Avg_Test(uint8_t *buff,uint8_t dot,uint8_t *buff1,uint8_t dot1);//ƽ������ֵ
void  Write_Usbdata (volatile uint8_t  *buffer,uint32_t num);

void Send_T0_USB(void);
void input_password(Disp_Coordinates_Typedef *Coordinates );//

void input_num(Disp_Coordinates_Typedef *Coordinates );


//void Send_Main_Ord(void);
#endif
