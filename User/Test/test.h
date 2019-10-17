#ifndef __test_h
#define __test_h
#include "stdint.h"
#include  "Globalvalue/globalvalue.h"
extern const uint8_t Disp_Main_Ord[][3];
extern uint8_t CorrectionflagC,CorrectionflagR,Correc_successflag;

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
unsigned char Uart_Process(void);
void Fac_DebugProcess(void);
void Use_SysSetProcess(void);//ϵͳ����
void Use_LimitSetProcess(void);//�����б�����
void Use_ITEMSetProcess(void);//�б�ɨ������
int8_t PackStandFrame(int8_t * framebuf, int8_t * datbuf, int8_t len);
Sort_TypeDef Disp_NumKeyboard_Set(Disp_Coordinates_Typedef *Disp_Coordinates);
Sort_TypeDef Disp_Set_Num(Disp_Coordinates_Typedef *Coordinates);
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


void  Avg_Test(uint8_t *buff,uint8_t dot,uint8_t *buff1,uint8_t dot1);//ƽ������ֵ
void  Write_Usbdata (volatile uint8_t  *buffer,uint32_t num);

void Send_T0_USB(void);
void input_password(Disp_Coordinates_Typedef *Coordinates );//

void input_num(Disp_Coordinates_Typedef *Coordinates );


//void Send_Main_Ord(void);
#endif
