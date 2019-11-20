//==========================================================
//�ļ����ƣ�GlobalValue.h
//�ļ�������ȫ�ֱ���ͷ�ļ�
//�ļ��汾��Ver1.0
//�������ڣ�2015.10.26 
//�޸����ڣ�2015.10.28 15:38
//�ļ����ߣ��ƺ���
//��ע˵������
//ע�������
//==========================================================
#ifndef __GlobalValue_h__
#define __GlobalValue_h__
#include "stdint.h"
//#include "TypeDefine.h"
#define FIT_INUM    12
//==========================================================
//����汾��10
#define SOFTWARE_VERSION		(0x10)

//==========================================================
//Ĭ�ϳ������(0)
#define DEFAULT_MACHINE_ID 		(0)

//Ĭ�ϳ�������(0)
#define DEFAULT_PASSWORD 		(0)

//Ĭ�Ͽ�������(0)
#define SET_BOOT_DEFAULT		(0)

//==========================================================
//��������
#define _DEBUG_CONFIG_FALSE		(0)
#define _DEBUG_CONFIG_TRUE		(1)

//==========================================================
//�������
//#define DEBUG_SUPPORT			(_DEBUG_CONFIG_TRUE)
#define DEBUG_SUPPORT			(_DEBUG_CONFIG_FALSE)

//==========================================================
//ISP�������ã����������
#if DEBUG_SUPPORT
	#define HW_ISP_SUPPORT		(_DEBUG_CONFIG_FALSE)
#else
	#define HW_ISP_SUPPORT		(_DEBUG_CONFIG_TRUE)
#endif

//==========================================================
//Uart��������
#define HW_UART_SUPPORT			(_DEBUG_CONFIG_TRUE)

//==========================================================
//��������������
#define HW_BEEP_SUPPORT			(_DEBUG_CONFIG_TRUE)
//==========================================================
//��������
//#define DISP_JK                 (0)
//==========================================================
//ADֵ�˲����ã����������ȶ���
#define AD_FILTER_SUPPORT		(_DEBUG_CONFIG_TRUE)

//==========================================================
#define SETUP_MENU_MAX  	(2)//������ò˵�

#define DEBUG_MENU_MAX  	(6)//���У׼�˵�

#define PARAMETER_MENU_MAX	(3)//�������˵�

#define SYSTEM_MENU_MAX		(4)//���ϵͳ�˵�

#define CONFIG_MENU_MAX		(3)//���������(0-2)

#define PW_LENGTH 			(4)//�������λ��

#define RANGE_MAX 			(4-1)//�������

#define AD_BUF_LENGTH		(8)//A/Dֵ����������

#define MAX_R_RANGE			(5)//�����������

#define BUTTOM_X_VALUE		(84)//��ʾ������ʾ��ʾ�ĵ�һ��Xλ��

#define BUTTOM_MID_VALUE	(80)//��ʾ������ʾ��ʾ���м���λ��

#define BUTTOM_Y_VALUE		(271-30)//��ʾ������ʾ��ʾ�ĵ�һ��Yλ��

#define KEY_NUM				(2)	//����������

#define NUM_LENTH 			(6)//��ʾ����
#define NUM_FREQ 			(33+4)//��ʾ����		

#define PASSWORD_LENTH      (8)//���볤��

//==========================================================
#define OHM 				(0xF4)	//CGROM���Ц��ַ�������Ϊ0xF4


//==========================================================

#define S_RX_BUF_SIZE		30
#define S_TX_BUF_SIZE		128

#define RSP_OK				0		/* ?? */
#define RSP_ERR_CMD			0x01	/* ??????? */
#define RSP_ERR_REG_ADDR	0x02	/* ??????? */
#define RSP_ERR_VALUE		0x03	/* ?????? */
#define RSP_ERR_WRITE		0x04	/* ��??? */

#define SLAVE_REG_P00		0x1000       //R_VOLU
#define SLAVE_REG_P01		0x1001      //Load_Voltage
#define SLAVE_REG_P02		0x1002      //Load_Current
#define SLAVE_REG_P03		0x1003		//Change_Voltage
#define SLAVE_REG_P04		0x1004		//Change_Current
#define SLAVE_REG_P05		0x1005		//Load_OCP
#define SLAVE_REG_P06		0x1006		//Change_OCP
#define SLAVE_REG_P07		0x1007		//Short_Time
#define SLAVE_REG_P08		0x1008		//Leak_Current
#define SLAVE_REG_P09		0x1009		//R1_Volu
#define SLAVE_REG_P10		0x100A		//R2_Volu
#define SLAVE_REG_P11		0x100B		//Temper
#define SLAVE_REG_P12		0x100C		//DHQ_Voltage
#define SLAVE_REG_P13		0x100D		//MODE
#define SLAVE_REG_P14		0x100E		//Load_Mode
#define SLAVE_REG_P15		0x100F		//Load_SET_Voltage
#define SLAVE_REG_P16		0x1010		//Load_SET_Current
#define SLAVE_REG_P17		0x1011		//Change_SET_Voltage
#define SLAVE_REG_P18		0x1012		//Change_SET_Current

#define WRITE_REG_P00		0x2000       //R_VOLU
#define WRITE_REG_P01		0x2001      //Load_Voltage
#define WRITE_REG_P02		0x2002      //Load_Current
#define WRITE_REG_P03		0x2003		//Change_Voltage
#define WRITE_REG_P04		0x2004		//Change_Current
#define WRITE_REG_P05		0x2005		//Load_OCP
#define WRITE_REG_P06		0x3000		//Change_OCP


#define WRITE_REG_P07		0x1007		//Short_Time
#define WRITE_REG_P08		0x1008		//Leak_Current
#define WRITE_REG_P09		0x1009		//R1_Volu
#define WRITE_REG_P10		0x100A		//R2_Volu
#define WRITE_REG_P11		0x100B		//Temper
#define WRITE_REG_P12		0x100C		//DHQ_Voltage
#define WRITE_REG_P13		0x100D		//MODE
#define WRITE_REG_P14		0x100E		//Load_Mode
#define WRITE_REG_P15		0x100F		//Load_SET_Voltage
#define WRITE_REG_P16		0x1010		//Load_SET_Current
#define WRITE_REG_P17		0x1011		//Change_SET_Voltage
#define WRITE_REG_P18		0x1012		//Change_SET_Current

//��ʾ������X����
#define MSG_ADDR_X 			(1)
#define AUTO_ADDR_X 		(12)
#define RANGE_ADDR_X 		(17)

#define RES_ADDR_X 			(1)
#define VOL_ADDR_X 			(12)

#define DISP_UNIT_XPOS	370-8-30
#define DISP_UNIT_YPOS	92
//==========================================================
//���ⳤ��
#define TITLE_LEN_MAX		(8)
#define  FILENAME_R  "MSREAD.TXT"
#ifdef DISP_JK
    #define  FILENAME_W  "JK2817N.TXT"
#else
    #define FILENAME_W  "2817N.TXT"
#endif
extern uint8_t Comp_flag;
extern uint8_t Debug_over;
extern uint8_t mainswitch;
extern uint8_t Irange;
extern uint8_t overflag;
extern uint8_t busyflag;
extern uint8_t ptflag;
extern struct MODS_T g_tModS;
extern uint8_t g_mods_timeout;
//��ֵ�����Զ���
//typedef struct
//{
//	uint8_t *Title;//����
//	uint16_t Num;//��ֵ
//	uint8_t  Dot;//С����(0-3: xxxx xxx.x xx.xx x.xxx)
//	uint8_t  Unit;//��λ(0-1: mOhm��Ohm)
//	uint16_t Max;//���ֵ
//	uint16_t Min;//��Сֵ
//}NumBox_TypeDef;

struct MODS_T
{
	uint8_t RxBuf[S_RX_BUF_SIZE];
	uint8_t TxBuf[S_TX_BUF_SIZE];
	uint8_t RxCount;
	uint8_t RxStatus;
	uint8_t RxNewFlag;
	uint8_t RspCode;
	
	uint8_t TxCount;
};

typedef struct
{
	float first_value;
	float second_value;
	float third_value;
	float fourth_value;
	float comp_highvalue[10];
	float comp_lowvalue[10];
	


}Comp_Testvalue_Typedef;
extern Comp_Testvalue_Typedef  Comp_Testvalue;
//typedef struct
//{
//	uint8_t  buff[5];
//	uint8_t dot;
//	uint8_t unit;


//}
typedef struct
{
	uint8_t Main_Funbuff[8];
	uint8_t Main_Secondbuff[8];
	uint8_t Main_Vmbuff[8];
	uint8_t Main_Imbuff[8];


}Test_DispBCD_Typedef;
typedef struct
{
	uint8_t Main_flag;
	uint8_t Second_falg;

}Main_Second_TypeDed;
//��С����͵�λ����
typedef struct 
{
	uint8_t sign;
	uint32_t Num;
	uint32_t Dot;
	uint32_t Unit;
	uint32_t Updata_flag;
	uint32_t Order;
}Sort_TypeDef;
typedef struct
{
	uint8_t buff[5];
	uint32_t Num;
	uint32_t Dot;
	uint32_t Unit;


}Range_Set_Typedef;
typedef struct
{
	Range_Set_Typedef all[4];

}All_Compvalue_Typedef;
extern All_Compvalue_Typedef Comp_Change;
typedef struct
{
	uint32_t test;
	uint32_t page;

}Param_Typedef;
//==========================================================
typedef struct
{
	uint32_t Auto;
	uint32_t Range;

}AotoRange_Typedef;
//������(20��)��������
typedef struct 
{
	Param_Typedef Param;//����(���б�)
	uint32_t Freq;//Ƶ��(���б�)
	uint32_t Level;//��ƽ(1V  0.3V)
	uint32_t Trig;//�ⲿ��������(0-3���ڲ� �ֶ� �ⲿ ����)
//	uint32_t Alc;//���ƽ(0-1��Ĭ�Ϲ�0)
	uint32_t Rsou;//����(0-1��Ĭ�Ϲ�0)
    Sort_TypeDef Trig_time;
//    Sort_TypeDef Temp_time;
    uint32_t Dev_a;
    uint32_t Dev_b;
    AotoRange_Typedef Range;
//    uint32_t Bias;
    uint32_t Speed;
    uint32_t Avg;
    uint32_t V_i;
//    uint32_t Dcr;
//    AotoRange_Typedef DC_Range;
//    uint32_t DC_Level;
    Sort_TypeDef Ref_A; 
    Sort_TypeDef Ref_B;
	uint32_t beep;
	uint32_t buad;
//	uint8_t  Count;
}Main_Func_TypeDef;
typedef struct
{
	uint8_t param;
	uint8_t freq;
	uint8_t level;
	uint8_t trig;
	uint8_t rsou;
	
	uint8_t range;
	uint8_t bisa;
	uint8_t speed;
	uint8_t avg;
	uint8_t v_i;
	uint8_t beep;
	uint8_t buad;
	Sort_TypeDef Trig_time;
    Sort_TypeDef Temp_time;
	uint8_t language;	
	

}Save_Main_Funce_Typedef;
typedef struct
{
	Sort_TypeDef Mainvalue;
	Sort_TypeDef Secondvalue;
	Sort_TypeDef Vmvalue;
	Sort_TypeDef Imvalue;
	Sort_TypeDef Pvalue;
	uint8_t Rangedisp;
	uint8_t Main_valuebuff[10];
	uint8_t Secondvaluebuff[10];
	uint8_t Vmvaluebuff[10];
	uint8_t Imvaluebuff[10];
	uint8_t Dot[4];
	uint8_t Unit[4];
	


}Test_Dispvalue_TypeDef;
extern Test_Dispvalue_TypeDef Test_Dispvalue; 
typedef struct
{
    uint32_t Open;//��·
    uint32_t Short;//��·
    uint32_t Load;//����
    uint32_t Spot;//У����
    uint32_t Freq;//Ƶ��
    Sort_TypeDef Ref_A;
    Sort_TypeDef Open_A;
    Sort_TypeDef Short_A;
    Sort_TypeDef Load_A;
    uint32_t Cable;//���³���
    uint32_t Mode;//��ʽ
    uint32_t Param;//����
    Sort_TypeDef Ref_B;
    Sort_TypeDef Open_B;
    Sort_TypeDef Short_B;
    Sort_TypeDef Load_B;

}User_Correction_Typedef;
typedef struct
{
    Sort_TypeDef low;
    Sort_TypeDef high;

}Comp_Value_Typedef;
//�����б�����
typedef struct
{
    uint32_t Param;//����
    Sort_TypeDef Nom;//���
    uint32_t Mode;//��ʽ
    uint32_t Aux;//����
    uint32_t Comp;//�ȽϿ���
    Comp_Value_Typedef Comp_Value[10];
    Comp_Value_Typedef Comp_Value_2nd;
	uint32_t count;
}Limit_Tab_Typedef;

typedef struct
{
    uint32_t Value;
    uint32_t Unit; 
} Sweep_Value_Typedef;
typedef struct
{
    uint32_t Freq;
    uint32_t Level;
    uint32_t Lmt;
    Sweep_Value_Typedef Low;
    Sweep_Value_Typedef High;
    Sweep_Value_Typedef Delay_Time;

}List_Value_Typedef;
//�б�ɨ������
typedef struct
{
    uint32_t mode;
    List_Value_Typedef  List_Value[200];


} List_Sweep_Typedef;
typedef struct
{
	uint32_t Year;
	uint32_t Mon;
	uint32_t data;

}Data_Value_Typedef;
typedef struct
{
	uint32_t Hour;
	uint32_t Min;
	uint32_t Sec;

}Timer_Value_Typedef;
//ϵͳ����
typedef struct
{
    uint32_t Main_Func;
    uint32_t Pass_Beep;
    uint32_t Fail_Beep;
    uint32_t Language;
    uint32_t Password;
    uint32_t Menu_Disp;
	Data_Value_Typedef Data_Value;
	Timer_Value_Typedef Timer_Value;
	uint32_t Cipher;	//����
    uint32_t Baue_Rate;
	
    uint32_t Bias;
    uint32_t Bus_Mode;
    uint32_t GP_Addr;
    uint32_t Talk_Only;
}Sys_Setup_Typedef;

typedef struct
{
	uint8_t fun;
	uint8_t num;
	uint8_t freq[210];
	uint8_t ac_leave[210];
	Range_Set_Typedef Range_Set_main[210];
	Range_Set_Typedef Range_Sed_Second[210];
	uint8_t comp[210];
	Sort_TypeDef time[210];
	uint8_t xpos;
	uint8_t ypos;

}Limit_ScanValue_Typedef;

typedef struct
{
	uint16_t setvoltage;
	uint16_t setcurrent;

}Set_Bat_Value_Typedef;

typedef struct
{
	
	User_Correction_Typedef		User_Correction;
	Limit_Tab_Typedef	Limit_Tab;
	List_Value_Typedef	List_Value;
	Sys_Setup_Typedef	Sys_Setup;
	Main_Func_TypeDef	Main_Func;
	Limit_ScanValue_Typedef Limit_ScanValue;
	Set_Bat_Value_Typedef Set_Bat;
    
}SaveData_Typedef;

typedef struct 
{
	uint32_t Num;
	uint32_t Dot;
	uint32_t Unit;
}SetNum_TypeDef;

typedef struct
{	
	Sort_TypeDef Voltage;//��ѹ
	Sort_TypeDef ChargePC;//��籣������
	Sort_TypeDef LoadPC;//�ŵ籣������
    Sort_TypeDef QuickV[6];//��ݵ�ѹֵ
	Sort_TypeDef ChargePT;//��籣��ʱ��
	Sort_TypeDef LoadPT;//�ŵ籣��ʱ��
	uint8_t qvflag;//��ݵ�ѹѡ��
	Sort_TypeDef CALV[4];//������ѹУ׼
	Sort_TypeDef CTRLV[4];//���Ƶ�ѹУ׼
	Sort_TypeDef CALI[6];//��������У׼
}SaveSet;
extern SaveSet SaveSIM;
extern SaveData_Typedef SaveData;
typedef struct
{
    uint8_t buard;
    uint8_t key_board;
    uint8_t U_flag;
    
}Sys_Typedef;
typedef struct
{
	Save_Main_Funce_Typedef	Save_Main_Funce;
	Limit_Tab_Typedef	Limit_Tab;
    Sys_Typedef Sys_set;
    uint8_t fac_num[10];

}Saveeeprom_Typedef;
extern Saveeeprom_Typedef	Saveeeprom;

typedef struct
{
	uint8_t index;
	uint8_t page;
	uint8_t third;
	uint8_t  force;

}Button_Page_Typedef;
typedef struct
{
	uint16_t xpos;
	uint16_t ypos;
	uint16_t lenth;


}Disp_Coordinates_Typedef;
typedef struct
{
	uint8_t Ordel;
	uint8_t name;

} Send_Ord_Typedef;
typedef struct
{
	Send_Ord_Typedef first;
	Send_Ord_Typedef second;
	Send_Ord_Typedef third;

}Send_Mainord_Typedef;
extern Send_Mainord_Typedef Send_Mainord;
extern uint8_t usb_oenflag;
extern uint8_t softswitch;
//extern Disp_Coordinates_Typedef Disp_Coordinates;
//==========================================================
//ϵͳ��(2��)
//typedef struct 
//{
//	Sort_TypeDef Res_Hi;//����(xxxx m��)
//	Sort_TypeDef Res_Lo;//����(xxxx m��)
//	Sort_TypeDef Vol_Hi;//����(xxxx V)
//	Sort_TypeDef Vol_Lo;//����(xxxx V)
//}System_TypeDef;

//==========================================================
//������(3��)
//typedef struct 
//{
//	uint16_t Password;//��������(0-9999��Ĭ��0)
//	uint16_t BootNum;//��������(0-9999��Ĭ��0)
//	uint16_t ProductId;//�������(0-9999��Ĭ��0)
//}Config_TypeDef;

//==========================================================
//У��ֵ
//typedef struct 
//{
//	uint16_t Num;  //��ʾֵ
//	uint16_t NumAd;//A/Dֵ
//}Cal_TypeDef;

////У����
//typedef struct 
//{
//	Cal_TypeDef IrCur[4];//��Ե�������ֵ-4��
//	Cal_TypeDef IrVol[2];//��Ե�����ѹֵ-2��
//	
//}Calibrate_TypeDef;
//typedef struct 
//{
//	uint16_t Real;  //ʵ��
//	uint16_t Imaginary;//����
//	uint16_t DC_voltage;
//	uint8_t	 Dot;
//	uint8_t	 Negative;
//}Res_TypeDef;
//==========================================================
//������Ϣ
//typedef struct 
//{
//	Parameter_TypeDef Parameter;//������
//	System_TypeDef System;//ϵͳ��
//	Config_TypeDef Config;//������
//	Calibrate_TypeDef Calibrate;//У׼��
//	uint16_t Crc16;//У����
//}Save_TypeDef;
//
////==========================================================
////Ĭ��У׼ֵ������
//extern const u16 CalibrateLimit[][2];
//extern const s16 BASE_NUM[6];
//extern const u8 DOT_POS[6];

//==========================================================
//ȫ�ֱ���
//extern  Save_TypeDef SaveData;//����ֵ
//extern  Cal_TypeDef Cal[6];//У׼ֵ
//extern  Res_TypeDef	Res;
//extern  uint8_t Range;//��ǰ����
//extern  bool F_Password;//������Ч��־
//extern  bool F_100ms;//100ms��ʱ��־
//extern  bool F_Fail;//����ʧ�ܱ�־
//extern  uint8_t MenuIndex;//�����˵���
//extern  uint8_t MenuSelect;//�˵�ѡ����
extern  unsigned long SystemStatus;//ϵͳ״̬
extern unsigned long SystemMessage;//ϵͳ��Ϣ
extern uint8_t DispBuf[12];
extern uint8_t Rtc_intflag;
extern unsigned long Count_buff[12];
extern uint8_t Uart_Send_Flag;
extern uint8_t Usb_Open_flag;
extern uint8_t debug_over;
extern uint8_t temperature;
extern uint8_t sendflag;
//����
extern int32_t V_CS,I_CS;
extern uint8_t Mode, LM_S;
extern uint16_t I_ADC,V_ADC;
extern uint8_t LOW_I,H_L,SWITCH_A; 
extern float LVL_DA;
//extern  uint16_t Voltage;//���Ե�ѹ
//extern  uint16_t Current;//���Ե���
//extern  uint16_t Resistance,xxxx;//���Ե���
//extern  NumBox_TypeDef NumBox;//��ֵ��
//extern uint16_t i_buff[FIT_INUM];		   //�˲�ֵ
//extern uint8_t ffit_data1;
//extern	uint8_t	count_ffit;
//extern 	u16 	zreo_num[6];
//extern u8 fuhao;
//extern bool clear_flag;
//extern bool vol_flag;
//extern u32 MenuIndex;//�����˵���
//extern u32 MenuSelect;//�˵�ѡ����
//
//extern u32 SystemStatus;//ϵͳ״̬
//extern u32 SystemMessage;//ϵͳ��Ϣ

//==========================================================
//ϵͳ״̬
enum SysStatusEnum
{	
	SYS_STATUS_ERROR ,			//�쳣
	SYS_STATUS_RESET ,			//��λ
	SYS_STATUS_POWER ,			//����
//	SYS_STATUS_POWER ,
	SYS_STATUS_DATASTORE ,		//���ݱ���
	SYS_STATUS_FILE ,			//�ļ�����
	SYS_STATUS_SETUPTEST ,			//��������
	SYS_STATUS_TEST ,			//����
	SYS_STATUS_RANGE ,		//������ʾ
	SYS_STATUS_RANGECOUNT,	//��������ʾ
	SYS_STATUS_ITEM ,			//�б�ɨ����ʾ
	SYS_STATUS_USERDEBUG,//�û�У�� 
	SYS_STATUS_FACRDEBUG, //����У��
	SYS_STATUS_SYSSET,     //ϵͳ����
    SYS_STATUS_LIMITSET,     //��������
    SYS_STATUS_ITEMSET,//�б�ɨ������
	SYS_STATUS_TOOL,//����
};
//==========================================================
//��ѡ״̬
enum SlectStatusEnum
{
	RH_FAIL,
	RL_FAIL,
	VH_FAIL,
	VL_FAIL,
	ALL_FAIL,
	ALL_PASS,
	


};
//==========================================================
//ϵͳ��Ϣ
enum SysMessageEnum
{
	MSG_CLEAR , 
	MSG_IDLE , 
	MSG_TEST , 
	MSG_ABORT , 
	MSG_PASS ,
//	MSG_FAIL ,
	MSG_HIGH , 
	MSG_LOW , 
};

//==========================================================
//���ADֵ
#define AD_MAX (3000) //12bit=4096-1
////����ѹADֵ
//#define VOL_AD_MAX (AD_MAX-100)
////������ADֵ
//#define CUR_AD_MAX (AD_MAX-100)

//����������
#define RANGR_LIMIT_HIGH (3000)
#define RANGR_LIMIT_LOW  (190)

//==========================================================
//�����б�
void InitGlobalValue(void);//ȫ�ֱ�����ʼ��
void LoadDefaultSet(void);//����Ĭ������ֵ
void LoadDefaultParameter(void);//����Ĭ�ϲ���ֵ
void LoadDefaultSystem(void);//����Ĭ��ϵͳֵ
void LoadDefaultConfig(void);//����Ĭ������ֵ
void LoadDefaultCalibrate(void);//����Ĭ��У׼ֵ
void ReadSaveData(void);//��ȡ�������
//void WriteSaveData(void);//д�뱣�����

#define SetSystemStatus(status) (SystemStatus=status)
#define GetSystemStatus() (SystemStatus)
#define GetSystemMessage() (SystemMessage)
#define SetSystemMessage(msg) (SystemMessage=msg)

void Check_Calibrate_Limit(void);//У׼ֵ���
void Hex_Format(uint32_t dat , uint8_t Dot , uint8_t len , uint8_t dispzero);
extern uint8_t USART_RX_BUF[200];

#endif
