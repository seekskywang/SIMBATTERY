#ifndef __use_disp_H_
#define __use_disp_H_
#include  "stdint.h"
#include  "Globalvalue/globalvalue.h"
#define FIRSTLINE	26
#define	SPACE1		22
#define LIST1		0
#define LIST2		250
#define SELECT_1END 192
#define SELECT_2END (410+32)
//#define FIRSTLINE	246
//#define	SPACE1		22
//#define LIST1		480
//#define LIST2		230
//#define SELECT_1END   (480-192)
//#define SELECT_2END    38

#define  SCREENWIDE   480
#define  SCREENHIGH   272
#define  INSTRUCT_TO_DEBUG	Delay(0x3f);
#define  SHORT_DELAY  Delay(0x1f);
#define  INSTR_FIVE   0x03
#define  SITE         5
#define  CORRE_SITE   5
#define  INSTR_FOUR   0x00
#define  LONGDELAY	  Delay(0x2ff);
extern const uint8_t Disp_Range_Main_Disp[][2+1];
extern const uint8_t Disp_Unit[][2+1];
extern const uint32_t FreqNum[];
extern const uint8_t Cp_Button_Tip[][7+1];
extern const uint8_t Cs_Button_Tip[][7+1];
extern const uint8_t Lp_Button_Tip1[][7+1];
extern const uint8_t Ls_Button_Tip[][7+1];
extern const uint8_t Z_Button_Tip[][7+1];
extern const uint8_t Y_Button_Tip[][7+1];
extern const uint8_t R_Button_Tip[][7+1];
void Disp_Fastbutton(void);
void Disp_MidRect(void);
void Disp_TestScreen(void);

void Disp_Test_Item(void);//������ʾ���˵�����Ŀ��ʾ
void Disp_Range_Item(void);//������ʾ���˵���Ŀ��ʾ
void Disp_Range_Count_Item(void);//��������ʾ����
void Disp_List_Count_Item(void);//�б���ʾ�Ӻ���
void Disp_Test_Set_Item(void);  //�������ý�����ʾ�˵�����
void Disp_UserCheck_Item(void);	//�û�У��
void Disp_FacrCheck_Item(void);
void Disp_LimitList_Item(void);//�����б�����
void Disp_ListScan_Item(void);
void Disp_Sys_Item(void);//ϵͳ����
void Disp_Serve_correction (void);//У���������ý���
void Disp_Serve_correctionR (void);//У��������ʾ
void Disp_Fac_Debug_C(Button_Page_Typedef* Button_Page);//����У������
void Disp_Fac_Debug_R(Button_Page_Typedef* Button_Page);//����У������
void Disp_Correction_SetR(Button_Page_Typedef* Button_Page);//У����������
void Disp_Fastbutton(void);
void Disp_Button_value1(uint32_t value);
void Disp_Button_TestSet(uint32_t value);
void DispSet_value(Button_Page_Typedef* Button_Page);	//�������õ����ò���ֵ
void Disp_Test_value(Button_Page_Typedef* Button_Page); // ������ʾ�е�����ֵ
void Disp_RangeDispValue(Button_Page_Typedef *Button_Page);//��������ʾON �� OFF����ʱ������ʾ�������ʾ
void Disp_Debug_value(Button_Page_Typedef* Button_Page); //�û�У���еİ���ֵ����
void Disp_Sys_value(Button_Page_Typedef* Button_Page);//ϵͳ�����е�������
void Disp_button_Num_time(void);
void Disp_button_Num_Freq(void);
Sort_TypeDef Time_Set_Cov(Sort_TypeDef *Time);
Sort_TypeDef Freq_Set_Cov(Sort_TypeDef *Freq);
void Disp_button_Num_Avg(void);
void Disp_button_Num_Input(uint8_t page);//������������n,u,p,m,next,
Sort_TypeDef Input_Set_Cov(Sort_TypeDef *Input_Ref);
void Disp_Cp(void);
void Disp_Button_Fun_Set(uint16_t xpos,uint16_t ypos,uint8_t * Disp_Item,Button_Page_Typedef* Button_Page);
void Disp_LimitSEt_value(Button_Page_Typedef* Button_Page);//�������ð���ֵ

void Disp_Correction_Set(Button_Page_Typedef* Button_Page);//У������

void Disp_Cp_D(void);
void Disp_Cp_Q(void);
void Disp_Cp_G(void);
void Disp_Cp_Rp(void);
void Disp_Cs_Rs(void);
void Disp_Cs_D(void);
void Disp_Cs_Q(void);
void Disp_Lp_Q(void);
void Disp_Lp_Rp(void);
void Disp_Lp_Rd(void);
void Disp_Lp_D(void);
void Disp_Lp_G(void);
void Disp_Ls_D(void);
void Disp_Ls_Q(void);
void Disp_Ls_Rs(void);
void Disp_Ls_Rd(void);
void Disp_Z_d(void);
void Disp_Z_r(void);
void Disp_Y_d(void);
void Disp_Y_r(void);
void Disp_R_X(void);
void Disp_Rs_Q(void);
void Disp_Rp_Q(void);
void Disp_Testvalue(void);//��ʾ��������
void Disp_Big_MainUnit(uint8_t unit,uint8_t unit1);
void Disp_Big_SecondUnit(uint8_t unit,uint8_t unit1);
void Send_Freq(Send_Ord_Typedef *ord);
void Send_Main_Ord(void);
//����ʱ������ʾ�������ʾ
void Disp_Range_ComDispValue(Button_Page_Typedef *Button_Page);
void Disp_RangeDispvalue(uint8_t data);
void Disp_Range_DispMainUnit(uint8_t num,uint8_t unit);//������ʾ��Cp�ĵ�λ
void Disp_Range_DispSecondUnit(uint8_t num,uint8_t unit);//������ʾ��Rp�ĵ�λ
void Disp_RangeTestvalue(void);    //������ʾ�е�Cp��Rp��ֵ
void Disp_RangeCount(void);
void Disp_LIMIT_ComDispValue(Button_Page_Typedef *Button_Page);
void Disp_Scan_Compvalue(uint8_t set);
void Disp_Scan_SetCompvalue(Button_Page_Typedef* Button_Page);
void Disp_Button_ItemScanSet(uint32_t value);
void Disp_Button_ItemScan_no(void);
void Send_Request(void);
void Disp_Set_Unit_12(uint32_t unit,uint32_t x,uint32_t y);
void Disp_Set_Unit_16(uint32_t unit,uint32_t x,uint32_t y);
Sort_TypeDef Input_Set_CovPre(Sort_TypeDef *Input_Ref);
void Disp_Test_Unit_v(uint32_t x,uint32_t y);
void Disp_Test_Unit_mA(uint32_t x,uint32_t y);
void Send_UartStart(void);
void Savetoeeprom(void);
void ReadSavedata(void);
void Disp_G_B(void);
//void DelayMs(uint32_t nMs);
void Communiction_Protocol(Button_Page_Typedef* Button_Page );
void lcd_image(uint8_t *pt);

void Disp_Start_Corr(void);    //��ʾ��ʼУ��
void Start_Correction(void);	//��ʼУ��
void Init_C(void);//���ݳ�ʼ��У��
void Correction_C(Button_Page_Typedef* Button_Page)	;	//����У��
void Save_C(void);	//�������ֵ

void Init_R(void);//��ʼ������
void Correction_R(Button_Page_Typedef* Button_Page);		//����У��
void Save_R(void);	//�������ֵ
void Exit_correction(void);	//�˳�У��
void Clear_Instr(void);  //���У��
void Close_Clear(void);		//�ر�����
void Open_Clear(void);		//��·����
void Short_Clear(void);			//��·����

void Send_Uart3(uint8_t *buff);
void Disp_Usbflag(uint8_t flag);
void Disp_Range_Num(uint8_t num);
#endif
