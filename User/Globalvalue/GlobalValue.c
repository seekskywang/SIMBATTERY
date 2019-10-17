//==========================================================
//�ļ����ƣ�GlobalValue.c
//�ļ�������ȫ�ֱ�������
//�ļ��汾��Ver1.0
//�������ڣ�2015.10.28
//�޸����ڣ�2015.10.28 15:38
//�ļ����ߣ��ƺ���
//��ע˵������
//ע�������
//==========================================================
#include "TypeDefine.h"	//��ֵ����
//#include "Beep.h"
//#include "core_cm3.h"
#include "Globalvalue/GlobalValue.h"

//==========================================================
//ȫ�ֱ���

SaveData_Typedef SaveData;
Saveeeprom_Typedef	Saveeeprom;
Test_Dispvalue_TypeDef Test_Dispvalue; 
Send_Mainord_Typedef Send_Mainord;
Comp_Testvalue_Typedef  Comp_Testvalue;
All_Compvalue_Typedef Comp_Change;
uint8_t usb_oenflag;
uint8_t Debug_over;
uint8_t Comp_flag;
uint8_t debug_over;
uint8_t Rtc_intflag;
//uint32_t MenuIndex;//�����˵���
//uint32_t MenuSelect;//�˵�ѡ����
//
//uint32_t SystemStatus;//ϵͳ״̬
//uint32_t SystemMessage;//ϵͳ��Ϣ
// Save_TypeDef SaveData;//����ֵ
// Cal_TypeDef Cal[6];//У׼ֵ
//
// NumBox_TypeDef NumBox;//��ֵ��
// Res_TypeDef	Res;
// bool F_Fail;//����ʧ�ܱ�־
// bool F_100ms;//100ms��ʱ��־
// bool F_Password;//������Ч��־
//
// uint8_t Range;//��ǰ����
//
uint8_t softswitch;
uint8_t Uart_Send_Flag;
unsigned long Count_buff[12];
unsigned long MenuIndex;//�����˵���
unsigned long MenuSelect;//�˵�ѡ����
//
 unsigned long SystemStatus;//ϵͳ״̬
unsigned long SystemMessage;//ϵͳ��Ϣ
Disp_Coordinates_Typedef Disp_Coordinates;
uint8_t DispBuf[12];
uint8_t Usb_Open_flag;

//
// uint16_t Voltage;//��ѹ
// uint16_t Current;//����
// uint16_t Resistance,xxxx;//����
//uint16_t i_buff[FIT_INUM];		   //�˲�ֵ
//uint8_t	count_ffit;
//u16 	zreo_num[6];
//uint8_t ffit_data1;
//u8 fuhao;
//bool clear_flag;
//bool vol_flag;
//==========================================================
//�������ƣ�InitGlobalValue
//�������ܣ���ʼ��ȫ�ֱ���
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.28
//�޸����ڣ�2015.10.28 15:33
//��ע˵������
//==========================================================
void InitGlobalValue(void)
{
	uint16_t len;
	uint8_t * buf;
						 
	buf=(uint8_t *)&SaveData;//�����׵�ַ
	len=sizeof(SaveData_Typedef);
	while(len--)
	{
		*buf=0;//���
		buf++;
	}
	buf=(uint8_t *)&SaveData.Limit_ScanValue;//�б�ɨ���ʼ��
	len=sizeof(SaveData.Limit_ScanValue);
	while(len--)
	{
		*buf=0xff;//���
		buf++;
	}
	SaveData.Limit_ScanValue.fun=0;
	//F_Password=FALSE;//������Ч��־	
}

////==========================================================
////�������ƣ�LoadDefaultSet
////�������ܣ�����Ĭ������ֵ
////��ڲ�������
////���ڲ�������
////�������ڣ�2015.10.28
////�޸����ڣ�2015.10.28 15:33
////��ע˵������
////==========================================================
//void LoadDefaultSet(void)
//{
//	LoadDefaultParameter();//����Ĭ�ϲ���
//	SaveParameter();//������������
//	
//	LoadDefaultSystem();//����Ĭ��ϵͳֵ
//	SaveSystem();//����ϵͳ����
//	
//	LoadDefaultConfig();//����Ĭ������ֵ
//	SaveConfig();//��������ֵ
//}

////==========================================================
////Ĭ�ϲ�����������
//const uint8_t DefaultParameter[]=
//{
//	0,	//����-�Զ�
//	0,	//Ѷ��-�ر�
//	0,	//����-�ر�
//	0,	//�ⴥ��-�ر�
//	0,	//����-�ر�
//};

////���Ե���
//const s16 BASE_NUM[6]=
//{	
//	100,//1300,
//	0,//170,
//	0,//20,
//	0,//10,
//	0
//};
////���Ե���С����λ��
//const u8 DOT_POS[6]=
//{	
//	2,
//	1,
//	3,
//	2,
//	0
//};
////==========================================================
////����Ĭ�ϲ���
////==========================================================
//void LoadDefaultParameter(void)
//{
//	u8 i;
//	u8 *pt,*pt2;

//	pt =(u8*)&(SaveData.Parameter);
//	pt2=(u8*)&(DefaultParameter);
//	for(i=0;i<sizeof(DefaultParameter);i++)
//	{
//		*pt++=*pt2++;
//	}
//}
////==========================================================
////Ĭ��ϵͳ��������
//const u8 DefaultSystem[]=
//{
//	0,200,1,0,	//��������-020.0m
//	0,100,2,0,	//��������-01.00m
//};
////==========================================================
////����Ĭ��ϵͳֵ
////==========================================================
//void LoadDefaultSystem(void)
//{
//	u8 i;
//	uint8_t *pt;

//	pt =(uint8_t*)&(SaveData.System);
//	for(i=0;i<sizeof(DefaultSystem);i++)
//	{
//		*pt++=DefaultSystem[i];
//	}
//}

////==========================================================
////����Ĭ������ֵ
////==========================================================
//void LoadDefaultConfig(void)
//{
//	SaveData.Config.Password=DEFAULT_PASSWORD;//��������
//	SaveData.Config.BootNum=SET_BOOT_DEFAULT;//��������
//	SaveData.Config.ProductId=DEFAULT_MACHINE_ID;//�������
//}

////==========================================================
////Ĭ��У׼����
//const uint16_t DefaultCalibrate[][2]=
//{
//	 1000, 1000, 
//	 1000, 1000, 
//	 5000, 3200, 
//	 5000, 3200, 
//	 5000, 3200, 
//	 5000, 3200,
//};
////==========================================================
////У׼ֵ�ϡ�����
//const u16 CalibrateLimit[][2]=
//{
//	 1300, 800,
//	 1300, 800, 
//	 1300, 800, 
//	 1300, 800, 
//	 3200,2800, 
//	 3200,2800, 
//};

////==========================================================
////����Ĭ��У׼ֵ
////==========================================================
//void LoadDefaultCalibrate(void)
//{
//	u8 i;
//	u8 *pt,*pt2;

//	pt =(u8*)&(SaveData.Calibrate);
//	pt2=(u8*)&(DefaultCalibrate);
//	for(i=0;i<sizeof(DefaultCalibrate);i++)
//	{
//		*pt++=*pt2++;
//	}
//	SaveCalibrate();//����У׼ֵ
//}

////==========================================================
////�������ƣ�Check_Calibrate_Limit
////�������ܣ�У׼ֵ���
////��ڲ�������
////���ڲ�������
////�������ڣ�2015.10.28
////�޸����ڣ�2015.10.28 15:38
////��ע˵�����ϡ����޼��,	L<=x<=H
////==========================================================
//void Check_Calibrate_Limit(void)
//{
////	u8 i;
//	u8 j;
//	uint16_t *pt;

//	pt=(uint16_t *)&SaveData.Calibrate;
//	j=sizeof(CalibrateLimit)/sizeof(CalibrateLimit[0]);
////	for(i=0;i<j;i++)//��j��
////	{
////		NumBox.Max=CalibrateLimit[i][0];//Max
////		NumBox.Min=CalibrateLimit[i][1];//Min
////		if(*pt > NumBox.Max)
////			*pt=NumBox.Max;
////		if(*pt < NumBox.Min)
////			*pt=NumBox.Min;
////		pt++;
////	}
//}

////==========================================================
////�������ƣ�ReadSaveData
////�������ܣ���ȡ�������
////��ڲ�������
////���ڲ�������
////�������ڣ�2015.10.28
////�޸����ڣ�2015.10.28 15:38
////��ע˵������
////==========================================================
//void ReadSaveData(void)
//{
//#if DEBUG_SUPPORT
//	LoadDefaultSet();//����Ĭ������ֵ
//	LoadDefaultCalibrate();//����Ĭ��У׼ֵ
//#else
//	ReadParameter();//��ȡ���ò���
//	ReadSystem();//��ȡϵͳ����
//	ReadConfig();//��ȡ������Ϣ
//	ReadCalibrate();//��ȡУ׼ֵ
//#endif
//	
//	//���µ�ǰ����
//	if(SaveData.Parameter.Range==0)//�����Զ�
//		Range=0;
//	else
//		Range=SaveData.Parameter.Range-1;//ƫ����1

//}

//==========================================================
//END
//==========================================================

