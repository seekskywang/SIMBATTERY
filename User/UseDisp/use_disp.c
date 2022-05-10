#include "LCD/lpc177x_8x_lcd.h"
#include"EX_SDRAM/EX_SDRAM.H"
#include "LCD/AsciiLib.h"
#include "use_disp.h"
#include  "stdint.h"
#include  "stdio.h"
#include "Key/key.h"
#include "test/Test.h"
#include  "Globalvalue/globalvalue.h"
#include "lpc177x_8x_eeprom.h"
#include "timer.h"
#include <math.h>
#include "debug_frmwrk.h"
#include "rtc.h"
#include "lpc177x_8x_rtc.h"

extern u8 primcomp,scedcomp;
float PRaw;
uint8_t CorrectionflagC=0,CorrectionflagR=0,Correc_successflag=0;
const uint8_t Num_1[][9]=
{"1","2","3","4","5","6","7","8","9"};

char CmdStr[CmdNumb][CmdLen] =
{
	{"<0/RET_1"},        //1��λ      1���ַ�
	{"<0/MOD_"},         //2 ģʽ      1���ַ�   
	{"<0/LM_"},         //// 3 LM5116 1���ַ� 
	{"<0/HORL_"},           //4 �иߵ͵�λ  1���ַ�   
	{"<0/SWITCH_"},         //5 �ܿ���     1���ַ�  
	{"<0/CSV_?"},          //6 ��ѯ��ѹ
	{"<0/CSI_?"},            //7 ��ѯ����
	{"<0/ADCV_?"},          //8 ��ѯADC��ѹ
	{"<0/ADCI_?"},          //9 ��ѯADC���� 
	{"<0/LVL_"},          // 10 ���õ�ѹ
	{"<0/STATUS_?"},        //11 ״̬     1���ַ� 
	{"<0/CALV_"},        //12 ������ѹУ׼     1���ַ�
	{"<0/CALI_"},        //13 ��������У׼     1���ַ�
	{"<0/CTRLV_"},        //14 ���Ƶ�ѹУ׼     1���ַ�
	{"<0/CLEARI_"},        //15 ��������У׼     1���ַ�
	{"<0/OVER_"}, 			//16 �����ź�   1���ַ�
	{"<0/TEMP_?"}, 			//17 ��ѯ�¶�   1���ַ�
};

char RecStr[CmdNumb][CmdLen] =
{
	{"STATUS_\0"},        //״̬��ȡ
	{"RET_\0"},           //��λ      1���ַ�   
	{"CSV_\0"},           //��ȡCS1237��ѹ������
	{"MOD_\0"},           //ģʽ�л�    1���ַ�  
	{"CSI_\0"},           //��ȡCS1237���������� 
	{"ADCI_\0"},          //ADC����
	{"LM_\0"},            // ����LM5116 1���ַ� 
	{"ADCV_\0"},          //ADC��ѹ
	{"LOWI_\0"},          //������ʱ���� 1���ַ� 
	{"HORL_\0"},          //�иߵ͵�λ  1���ַ� 
	{"LVL_\0"},           //DAC8562��� A· 0~5V
	{"SWITCH_\0"},        //�ܿ���     1���ַ� 
	{"CALV_\0"},        //12��ѹУ׼     1���ַ� 
	{"CALI_\0"},        //13����У׼     1���ַ� 
	{"OVER_\0"},        //14����     1���ַ� 
	{"TEMP_\0"},        //15�¶�
};

const uint8_t Test_Setitem_E[][9+1]=
{
	{"Func    :"},
	{"Freq    :"},
	{"ELevel  :"},
	{"MRange  :"},
	{"Speed:"},
//    {"ƫ��    :"},
	
	{"Vm:"},
	{"Im:"},
	{"Corr:"}
	
};

const uint8_t Test_Setitem[][14+1]=
{
	{"��ѹ�趨  :"},
	{"��籣��  :"},
	{"�ŵ籣��  :"},
	{"��ݵ�ѹ    :"},
	{"��籣��ʱ��:"},
    {"�ŵ籣��ʱ��:"},
	
//	{"Vm:"},
//	{"Im:"},
//	{"У��:"}
	
};

const uint8_t Fac_Calitem[][14+1]=
{
	{"1V    "},
	{"10V   "},
	{"13V   "},
	{"18V   "},	
	{"19V   "},
	{"19.5V "}
};

const uint8_t Fac_ICalitem[][14+1]=
{	
	{"-10mA  "},
	{"-5A    "},
	{"-10A   "},
	{"10mA "},
	{"5A   "},
	{"10A  "},
};

const uint8_t CtrV_Calitem[][14+1]=
{
	{"1V    "},
	{"10V   "},
	{"15V   "},
	{"19V   "},
	{"      "},
	{"      "}
};

const uint8_t Button_TipPage1[][7+1]=  //���Բ���ѡ��ʱ����������ʾ����
{
    {"Cp-.."},
    {"Cs-.."},
    {"Lp-.."},
    {"Ls-.."},
    {" "},


};
const uint8_t Button_TipPage2[][7+1]=  //���Բ���ѡ��ʱ����������ʾ����
{
    {"Z-.."},
    {"Y-.."},
    {"R-.."},
    {"G-B"},
    {" "},


};
const uint8_t Button_TipPage3[][7+1]=  //���Բ���ѡ��ʱ����������ʾ����
{
    {"DCR"},
    {" "},
    {" "},
    {""},
    {" "},


};
const uint8_t Cp_Button_Tip[][7+1]=  //���Բ���ѡ��ʱ����������ʾ����
{
    {"Cp-D"},
    {"Cp-Q"},
    {"Cp-G"},
    {"Cp-Rp"},
    {"RETURN"},


};
const uint8_t Cs_Button_Tip[][7+1]=  //���Բ���ѡ��ʱ����������ʾ����
{
    {"Cs-D"},
    {"Cs-Q"},
    {"Cs-Rs"},
    {"    "},
    {"RETURN"},


};
const uint8_t Lp_Button_Tip1[][7+1]=  //���Բ���ѡ��ʱ����������ʾ����
{
    {"Lp-Q"},
	{"Lp-Rp"},
    {"Lp-Rd"}, 
	{"Lp-D"},
    {"RETURN"},


};
const uint8_t Lp_Button_Tip2[][7+1]=  //���Բ���ѡ��ʱ����������ʾ����
{
    {"Lp-D"},
    {"Lp-G"},
    {"    "},
    {"    "},
    {"RETURN"},


};
const uint8_t Ls_Button_Tip[][7+1]=  //���Բ���ѡ��ʱ����������ʾ����
{
    {"Ls-D"},
    {"Ls-Q"},	
    {"Ls-Rs"},
    {"Ls-Rd"},
    {"RETURN"},


};
const uint8_t Z_Button_Tip[][7+1]=  //���Բ���ѡ��ʱ����������ʾ����
{
    {"Z-d"},
    {"Z-r"},
    {" "},
    {" "},
    {"RETURN"},


};

const uint8_t Y_Button_Tip[][7+1]=  //���Բ���ѡ��ʱ����������ʾ����
{
    {"Y-d"},
    {"Y-r"},
    {" "},
    {" "},
    {"RETURN"},


};
const uint8_t R_Button_Tip[][7+1]=  //���Բ���ѡ��ʱ����������ʾ����
{
    {"R-X"},
    {"Rp-Q"},
    {"Rs-Q"},
    {" "},
    {"RETURN"},


};
const uint8_t User_FUNC[][8+1]=
{
	{"Cp-D"},
	{"Cp-Q"},
    {"Cp-G"},
	{"Cp-Rp"},
	{"Cs-D"},
	{"Cs-Q"},
    {"Cs-Rs"},
    {"Lp-Q"},//7
    {"Lp-Rp"},
    {"Lp-Rd"},
    {"Lp-D"},
//    {"Lp-G"},//10
    {"Ls-D"},
    {"Ls-Q"},
    {"Ls-Rs"},
    {"Ls_Rd"},//14
    {"Z-d"},
    {"Z-r"},
    {"Y-d"},
    {"Y-r"},//18
	{"R-X"},
	{"Rp-Q"},
	{"Rs-Q"},
    {"G-B"},
    {"DCR"},
};
const uint8_t RangeDisp_Main[][3+1]=
{
	{"Cp:"},{"Cp:"},{"Cp:"},{"Cp:"},
	{"Cs:"},{"Cs:"},{"Cs:"},
	{"Lp:"},{"Lp:"},{"Lp:"},{"Lp:"},
	{"Ls:"},{"Ls:"},{"Ls:"},{"Ls:"},
	{"Z :"},{"Z :"},
	{"Y :"},{"Y :"},
	{"R :"},{"Rp:"},{"Rs:"},
	{"G :"}
	
};
const uint8_t RangeDisp_Second[][3+1]=
{
	{"D :"},{"Q :"},{"G :"},{"Rp:"},
	{"D :"},{"Q :"},{"Rs:"},
	{"Q :"},{"Rp:"},{"Rd:"},{"D :"},
	{"D :"},{"Q :"},{"Rs:"},{"Rd:"},
	{"r :"},{"r :"},
	{"r :"},{"r :"},
	{"X :"},{"Q :"},{"Q :"},
	{"B :"}
};
const uint8_t Fun_SelectValue[4][7]=
{
	{0,4,	7,	11,	15,	17,	19},
	{1,5,	8,	12,	16,	18,	20},
	{2,6,	9,	13,	0xff,0xff,21},
	{3,0xff,10,	14,	0xff,0xff,0xff}


};
const uint32_t FreqNum[]=
	{20,25,30,40,50,60,75,100,120,150,200,250,300,400,500,600,750,
	1e3,1200,1500,2e3,2500,3e3,4e3,5e3,6e3,7500,
	1e4,12e3,15e3,2e4,25e3,3e4,4e4,5e4,6e4,75e3,
	1e5};
//const uint32_t FreqNum[]=
//	{20,25,30,40,50,60,75,100,120,150,200,250,300,400,500,600,750,
//	1000,1200,1500,2000,2500,3000,4000,5000,6000,7500,
//	10000,12000,15000,20000,25000,30000,40000,50000,60000,75000,
//	100000,120000};
const uint8_t User_Freq[][10+1]=
{
	{"20.000 Hz"},
	{"25.000 Hz"},
	{"30.000 Hz"},
	{"40.000 Hz"},
    {"50.000 Hz"},
    {"60.000 Hz"},
    {"75.000 Hz"},
    {"100.00 Hz"},
    {"120.00 Hz"},
    {"150.00 Hz"},
    {"200.00 Hz"},
    {"250.00 Hz"},
    {"300.00 Hz"},
    {"400.00 Hz"},
    {"500.00 Hz"},
    {"600.00 Hz"},
    {"750.00 Hz"},
    {"1.0000KHz"},
    {"1.2000KHz"},
    {"1.5000KHz"},
    {"2.0000KHz"},
    {"2.5000KHz"},
    {"3.0000KHz"},
    {"4.0000KHz"},
    {"5.0000KHz"},
    {"6.0000KHz"},
    {"7.5000KHz"},
    {"10.000KHz"},
    {"12.000KHz"},
    {"15.000KHz"},
    {"20.000KHz"},
    {"25.000KHz"},
    {"30.000KHz"},
    {"40.000KHz"},
    {"50.000KHz"},
    {"60.000KHz"},
    {"75.000KHz"},
    {"100.00KHz"},
	{"120.00KHz"},
	{"150.00KHz"},
	{"200.00KHz"},


};
const uint8_t Setup_Beep_E[][6+1]=
{
	"OFF",
	"PASS",
	"FAIL",

};
const uint8_t Setup_Beep[][6+1]=
{
	"�ر�",
	"�ϸ�",
	"���ϸ�",

};
const uint8_t *Setup_Valueall[]=
{
	*User_FUNC,
	*User_Freq,



};
const uint8_t FreqButton_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"DECR --"},
    {"DECR -"},
    {"INCR +"},
    {"INCR ++"},
	{" "}

};
const uint8_t User_Level[][10+1]=
{
    {"0.100 V"},
    {"0.300 V"},
	{"1.000 V"},

};
const uint8_t LevleButton_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"DECR -"},
    {"INCR +"},

};

const uint8_t User_Range[][10+1]=
{
    {"AUTO "},
    {"10 �� "},
    {"100�� "},
    {"1 k�� "},
    {"10k�� "},
    {"100k��"},
	{"HOLD"},

};
const uint8_t RangeButton_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"AUTO"},
    {"HOLD"},
    {"DECR -"},
    {"INCR +"},

};
const uint8_t User_Biad[][10+1]=
{
    {"---- "},
   

};
const uint8_t BiasButton_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"DECR -"},
    {"INCR +"},

};
const uint8_t User_Speed[][10+1]=
{
    {"FAST "},
    {"MED "},
    {"SLOW "},
   
};
const uint8_t SpeedButton_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"FAST "},
    {"MED "},
    {"SLOW "},
};
const uint8_t User_Comp[][10+1]=
{
    
    {"OFF"},
	{"ON "},
   
};
const uint8_t Limit_Scan_Fun[][4+1]=
{
	{"SEQ"},
	{"STEP"},

};
const uint8_t Limit_Scan_Page[][6+1]=
{
	{"Last P"},
	{"Next P"},

};
	
const uint8_t Sys_Bais_Value[][6+1]=
{
	{"INT"},
	{"OUT"},
	{"NONE"}

};
const uint8_t Sys_Addr_value[][6+1]=
{
	{"RS232C"},
	{"GPIB"},
	{"USBTMC"},
	{"USBCDC"},
	{"RS485"},
	{"NONE"}

};
const uint8_t Switch_Value[][4+1]=
{
	{"OFF"},
	{"ON"},
	{"NONE"}

};

const uint8_t IRaly_Value[][4+1]=
{
	{"LOW "},
	{"HIGH"},
	{"AUTO"}

};

const uint8_t CompButton_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"ON"},
	{"OFF "},
};
const uint8_t User_ListMode[][10+1]=
{
    {"SEQ"},
    {"STEP "},
   
};
const uint8_t ListModeButton_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"SEQ "},
    {"SETP "}
};
const uint8_t Setup_Trig[][10+1]=
{
    {"INT "},
    {"MAN "},
    {"EXT "},
    {"BUS "},
   
};
const uint8_t SetupTrig_Button_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"INT "},
    {"MAN "},
    {"EXT "},
    {"BUS "},
};
const uint8_t Setup_Alc[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t SetupAlc_Button_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Setup_Rsou[][10+1]=
{
    {"100��"},
    {"30��"},
   
};
const uint8_t SetupRsou_Button_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"100��"},
    {"30��"},
};
const uint8_t SetupTrigDly_Button_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"DECR -"},
    {"INCR +"},
};
const uint8_t Setup_Rev_A[][10+1]=
{
    {"ABS  "},
    {"%   "},
    {"OFF   "},
   
};
const uint8_t SetupRev_A_Button_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"ABS"},
    {"%  "},
    {"OFF"},
};
const uint8_t Setup_Rev_B[][10+1]=
{
    {"ABS  "},
    {"%   "},
    {"OFF   "},
   
};
const uint8_t SetupRev_B_Button_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"ABS"},
    {"%  "},
    {"OFF"},
};
const uint8_t SetupAvg_Button_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"DECR -"},
    {"INCR +"},
};
const uint8_t Setup_Vm[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t SetupVm_Button_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Setup_Dcr[][10+1]=
{
    {"FIX  "},
    {"ALT "},
   
};
const uint8_t SetupDcr_Button_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"FIX  "},
    {"ALT "},
};
const uint8_t SetupRefa_Button_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"MEASURE"},
};
const uint8_t SetupRefB_Button_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
{
    {"MEASURE"},
};

const uint8_t Correct_Open[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t CorrectOpen_Button_Tip[][7+1]=  //�ȽϽ���ѡ��ʱ����������ʾ����
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Correct_Short[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t CorrectShort_Button_Tip[][7+1]=  //�ȽϽ���ѡ��ʱ����������ʾ����
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Correct_Load[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t CorrectLoad_Button_Tip[][7+1]=  //�ȽϽ���ѡ��ʱ����������ʾ����
{
    {"ON  "},
    {"OFF "},
};
const uint8_t CorrectSpot_Button_Tip[][7+1]=  //�ȽϽ���ѡ��ʱ����������ʾ����
{
    {"DECR --"},
    {"DECR -"},
    {"INCR --"},
    {"INCR ++"}
};
const uint8_t Correct_Freq[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t CorrectFreq_Button_Tip[][7+1]=  //�ȽϽ���ѡ��ʱ����������ʾ����
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Correct_Cable[][10+1]=
{
    {"0m  "},
    {"1m "},
    {"2m  "},
    {"4m "},
   
};
const uint8_t CorrectCable_Button_Tip[][7+1]=  //�ȽϽ���ѡ��ʱ����������ʾ����
{
    {"0m "},
    {"1m "},
    {"2m "},
    {"4m "},
};
//const uint8_t User_ListNo[][10+1]=
//{
//    {"SEQ"},
//    {"STEP "},
//   
//};
//const uint8_t ListNoButton_Tip[][7+1]=  //Ƶ��ѡ��ʱ����������ʾ����
//{
//    {"SEQ "},
//    {"SETP "}
//};
//const uint8_t Test_Setitem_E[][9+1]=
//{
//	{"FUNC    :"},
//	{"FREQ    :"},
//	{"LEVEL   :"},
//	{"RANGE   :"},
//    {"BIAS    :"},
//	{"SPEED   :"},
//	
//	{"Vm:"},
//	{"Im:"},
//	{"CORR:"}
//	
//};
const uint8_t  Range_Item[][7+1]=
{
	{"����  :"},
	{"Ƶ��  :"},
	{"��ƽ  :"},
	{"����  :"},
//	{"ƫ��  :"},
    {"�ٶ�  :"},
    {"�Ƚ�  :"},
};
const uint8_t  Range_Item_E[][7+1]=
{
	{"FUNC  :"},
	{"FREQ  :"},
	{"LEVEL :"},
	{"RANGE :"},
//	{"BIAS  :"},
	{"SPEED :"},
	{"COMP  :"}
};
//const uint8_t	Set_testitem[][9+1]=
//{
//	{"����    :"},
//	{"Ƶ��    :"},
//	{"��ƽ    :"},
//	{"����    :"},
//	{"���ƽ  :"},
//	{"����    :"},
//	{"������ʱ:"},
//	{"������ʱ:"},
//	{"Ѷ�칦��:"},
//	{"����    :"},
//	{"�ٶ�    :"},
//	{"ƫ��    :"},
//	{"ƽ��    :"},
//	{"V/I���� :"},
//	
//	{"DCR���� :"},
//	{"DC����  :"},
//	{"DC��ƽ  :"},
//    {"������  :"},
//};

const uint8_t	Set_testitem[][11+1]=
{
	{"��ݵ�ѹ1 :"},
	{"��ݵ�ѹ2 :"},
	{"��ݵ�ѹ3 :"},
	{"��ݵ�ѹ4 :"},
	{"��ݵ�ѹ5 :"},
	{"��ݵ�ѹ6 :"},
//	{"������ʱ"},
//	{"������ʱ"},
//	{"Ѷ�칦��"},
//	{"����    "},
//	{"�ٶ�    "},
////	{"ƫ��    "},
////	{"ƽ��    "},
//	{"V/I���� "},
//	
////	{"DCR���� "},
////	{"DC����  "},
////	{"DC��ƽ  "},
//    {"������  "},
};


const uint8_t	Set_testitem_E[][9+1]=
{
	{"FASTV1  :"},
	{"FASTV2  :"},
	{"FASTV3  :"},
	{"FASTV4  :"},
	{"FASTV5  :"},
	{"FASTV6  :"},
};
//const uint8_t Sys_Setitem_E[][10+1]=
//{
//	{"�Ǳ��� :"},
//	{"�ϸ�Ѷ�� :"},
//	{"����Ѷ�� :"},
//	{"��ʾ���� :"},
//	{"����     :"},
//	{"�˵����� :"},
//	{"����     :"},
//	{"������   :"},
//	{"ƫ��Դ   :"},
//	{"���߷�ʽ :"},
//	{"GPIB��ַ :"},
//	{"ֻ��     :"},
//	{"ʱ��     :"}
//};

const uint8_t Set_V_Res[][5+1]=
{
	{"0.001"},
	{"0.01 "},
	{"0.1  "},
	{"1    "},	
};

const uint8_t Tune_V[][1+1]=
{
	{"+"},
	{"-"},
};

const uint8_t Sys_Setitem[][10+1]=
{
	{"�Ǳ��� "},
	{"�������� "},
	{"U�̿��� "},
	{"��ʾ���� "},
//	{"����     "},
	{"������λ "},
	{"����     "},
//	{"����     "},
	{"������   "},
	{"ƫ��Դ   "},
	{"���߷�ʽ "},
	{"GPIB��ַ "},
	{"ֻ��     "},
	{"ʱ��     "}
};
const uint8_t Sys_Setitem_E[][10+1]=
{
	{"MAIN FUNC:"},
	{"PASS BEEP:"},
//	{"FAIL BEEP:"},
	{"U  SWITCH:"},
	{"LANGUAGE :"},
	//{"PASS WORD:"},
	{"MENU DISP:"},
	{"DATA     :"},
	{"BAUD RATE:"},
	{"BIAS SRC :"},
	{"BUS MODE :"},
	{"GPIBADDR :"},
	{"TALK ONLY:"},
	{"TIME     :"}
};
const uint8_t All_TopName[][21+1]=
{
	{"< ������ʾ >"},
	{"< ������ʾ >"},
	{"< ��������ʾ >"},
	{"< �б�ɨ����ʾ >"},
	{"< �������� >"},
	{"< �û�У�� >"},
	{"< �����б����� >"},
	{"< �б�ɨ������ >"},
	{"< ϵͳ���� >"},
	{"[ LCR�ļ��б� ]"},
	{"< У������ >"},

};
const uint8_t All_TopName_E[][21+1]=
{
	{"< MEAS DISPLAY >"},
	{"< BIN No. DISP >"},
	{"< BIN COUNT DISP >"},
	{"< LIST SWEEP DISP >"},
	{"< MEASURE SETUP >"},
	{"< CORRECTION >"},
	{"< LIMIT TABLE SETUP >"},
	{"< LIST SWEEP SETUP >"},
	{"< SYSTEM SETUP >"},
	{"[ LCR FILE LIST ]"},
	{"< CORRECTION SET >"},

};
const uint8_t Range_Count_Item[][6+1]=
{
	{"���� :"},
	{"��� :"},
	{"���� :"}

};
const uint8_t Range_Count_Item_E[][6+1]=
{
	{"PARAM :"},
	{"NOM.  :"},
	{"COUNT :"}

};

const uint8_t User_Check_Item[][12+1]=
{
	{""},
	{""},
//	{"��·  :"},
//	{"��·  :"},
//	{"����  :"},
//	{"У����   :"},
//	{"Ƶ��     :"},
//	{" �ο�   A:"},
//	{" ��·   A:"},
//	{" ��·   A:"},
//	{" ����   A:"},
//	{"����  :"},
//	{"��ʽ  :"},
//	{"����  :"},
//	{"       "},
//	{"  B:"},
//	{"  B:"},
//	{"       "},
//	{"  B:"},
//	{"  B:"}

};


const uint8_t User_Check_Opencircuit[][7+1]=	//��·
{
	{"OFF"},
	{"ON"}
};
const uint8_t DEBUG_Check_Opencircuit[][7+1]=	//��·
{
	{"��ʼ"},
	{"     "}
};
const uint8_t DEBUG_Check_Opencircuit_E[][7+1]=	//��·
{
	{"STAR"},
	{"     "}
};
const uint8_t User_Check_Shortcircuit[][7+1]=	//��·
{
	{"OFF"},
	{"ON"}
};

const uint8_t User_Check_Item_E[][12+1]=
{
	{"OPEN  :"},
	{"SHORT :"},
//	{"LOAD  :"},
//	{"SPOT No.  :"},
//	{"FREQ      :"},
//	{" REF     A:"},
//	{" OPEN    A:"},
//	{" SHORT   A:"},
//	{" LOAD:   A"},
//	{"CABLE :"},
//	{"MODE  :"},
//	{"FUNC  :"},
//	{"       "},
//	{"  B:"},
//	{"  B:"},
//	{"       "},
//	{"  B:"},
//	{"  B:"}



};
const uint8_t User_LimitList_Item[][7+1]=
{
 	{"����  :"},
	{"���  :"},
	{"��ʽ  :"},
	{"����  :"},
	{"�Ƚ�  :"},
	{""},


};
const uint8_t User_LimitList_Value[][3+1]=
{
	
	"ABS",
	"%",
};
const uint8_t User_LimitList_Item_E[][7+1]=
{
 	{"PARAM :"},
	{"NOM   :"},
	{"MODE  :"},
	{"AUX   :"},
	{"COMP  :"},
	{""},


};
const uint8_t User_LimitList_Item2[][3+1]=
{
	{"BIN"},
	{" 1"},
	{" 2"},
	{" 3"},
	{" 4"},
	{" 5"},
	{" 6"},
	{" 7"},
	{" 8"},
	{" 9"},
//	{"2nd"}


};

const uint8_t User_LimitScan_Item2[][3+1]=
{
	{"No."},
	{"001"},
	{"002"},
	{"003"},
	{"004"},
	{"005"},
	{"006"},
	{"007"},
	{"008"},
	{"009"},
	{"010"}


};
const uint8_t User_ListScan_Item[][8+1]=
{
	{"ģʽ :"},
	{"Ƶ��[Hz]"},
    {"��ƽ[V]"},
	{"LMT"},
	{"����"},
	{"����"},
    {"��ʱ[s]"},

};
const uint8_t User_ListScan_Item_E[][8+1]=
{
	{"MODE :"},
	{"FREQ[Hz]"},
    {"LEVEL[V]"},
	{"LMT"},
	{"LOW "},
	{"HIGH"},
    {"DELY[s]"},

};
const uint8_t Sys_Language_Value[][7+1]=
{
	{"����"},
	{"ENGLISH"}

};
const uint8_t Sys_Buard_Value[][6+1]=
{
	{"SHUT"},
	{"4800"},
	{"9600"},
	{"19200"},
	{"38400"},
	{"56000"},
	{"115200"},
    {"NONE"},
};
const uint8_t Set_Unit[][5+1]=
{
	{"p"},
	{"n"},
	{"u"},
	{"m"},
	{"next"},
	{"*1"},
	{"k"},
	{"M"},
	{" "},
	{"back"},



};
const uint8_t Disp_Unit[][2+1]=
{"p","n","u","m"," ","k","M","G",""};

const uint8_t Disp_Range_Main_NUm[]={0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,3,3,3,2,2,1,1};
const uint8_t Disp_Range_Main_Disp[][2+1]=
{	"F ",
	"H ",
	"��",
	"S ",
	"r ",
	"��",
	"  "
};//F,H,����S r ��ո�
uint32_t const Main_Func_Comp[][2]=                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
{
	{0,21},
	{0,33},
	{0,1},
	{0,3},
	{0,0},
	{0,0},
	{0,60000},
	{0,60000},
	{0,2},
	{0,2},
	{0,7},//����
	{0,0},
	{0,2},
	{0,16},
	{0,1},
	{0,0},
	{0,0},
	{0,0},
	{0,999999},
	{0,5},
	{0,7},
	{0,999999},
	{0,5},
	{0,7}


};

const uint8_t Correction_Capacitance[][7+1]=			//У������
{
	{"100uF"},
	{"10uF"},
	{"1uF"},
	{"100nF"},
	{"10nF"},
	{"1nF"},
	{"100pF"},
	{"10pF"},
};

const uint8_t Correction_Resist[][7+1]=			//У������
{
	{"10��"},
	{"100��"},
	{"1K��"},
	{"10K��"},
	{"100K��"},
};

void DrawLogo(uint16_t x,uint16_t y)
{
//	page_flag = 0xff;
//	LCD_Clear(LCD_COLOR_BLACK);	
//	LCD_SetColors(LCD_COLOR_GR7,LCD_COLOR_BLACK);
	/*J*/
	LCD_DrawLine(x,y,x+41,y,LCD_COLOR_LOGO);
	LCD_DrawLine(x-1,y-1,x-1+44,y-1,LCD_COLOR_LOGO);
	LCD_DrawLine(x-1,y-2,x-1+46,y-2,LCD_COLOR_LOGO);
	LCD_DrawLine(x,y-3,x+46,y-3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+2,y-4,x+2+46,y-4,LCD_COLOR_LOGO);
	LCD_DrawLine(x+35,y-5,x+35+14,y-5,LCD_COLOR_LOGO);
	LCD_DrawLine(x+35,y-6,x+35+15,y-6,LCD_COLOR_LOGO);
	LCD_DrawLine(x+35,y-7,x+35+15,y-7,LCD_COLOR_LOGO);
	LCD_DrawLine(x+36,y-8,x+36+14,y-8,LCD_COLOR_LOGO);
	LCD_DrawLine(x+36,y-9,x+36+15,y-9,LCD_COLOR_LOGO);
	LCD_DrawLine(x+37,y-10,x+37+14,y-10,LCD_COLOR_LOGO);
	LCD_DrawLine(x+37,y-11,x+37+14,y-11,LCD_COLOR_LOGO);
	LCD_DrawLine(x+37,y-12,x+37+14,y-12,LCD_COLOR_LOGO);
	LCD_DrawLine(x+38,y-13,x+38+14,y-13,LCD_COLOR_LOGO);
	LCD_DrawLine(x+37,y-14,x+37+15,y-14,LCD_COLOR_LOGO);
	LCD_DrawLine(x+38,y-15,x+38+14,y-15,LCD_COLOR_LOGO);
	LCD_DrawLine(x+38,y-16,x+38+14,y-16,LCD_COLOR_LOGO);
	LCD_DrawLine(x+38,y-17,x+38+15,y-17,LCD_COLOR_LOGO);
	LCD_DrawLine(x+39,y-18,x+39+14,y-18,LCD_COLOR_LOGO);
	LCD_DrawLine(x+39,y-19,x+39+14,y-19,LCD_COLOR_LOGO);
	LCD_DrawLine(x+39,y-20,x+39+15,y-20,LCD_COLOR_LOGO);
	LCD_DrawLine(x+40,y-21,x+40+14,y-21,LCD_COLOR_LOGO);
	LCD_DrawLine(x+40,y-22,x+40+14,y-22,LCD_COLOR_LOGO);
	LCD_DrawLine(x+40,y-23,x+40+14,y-23,LCD_COLOR_LOGO);
	LCD_DrawLine(x+40,y-24,x+40+14,y-24,LCD_COLOR_LOGO);
	LCD_DrawLine(x+41,y-25,x+41+13,y-25,LCD_COLOR_LOGO);
	LCD_DrawLine(x+41,y-26,x+41+14,y-26,LCD_COLOR_LOGO);
	LCD_DrawLine(x+41,y-27,x+41+14,y-27,LCD_COLOR_LOGO);
	LCD_DrawLine(x+42,y-28,x+42+14,y-28,LCD_COLOR_LOGO);
	LCD_DrawLine(x+42,y-29,x+42+14,y-29,LCD_COLOR_LOGO);
	LCD_DrawLine(x+42,y-30,x+42+14,y-30,LCD_COLOR_LOGO);
	LCD_DrawLine(x+42,y-31,x+42+15,y-31,LCD_COLOR_LOGO);
	LCD_DrawLine(x+42,y-32,x+42+15,y-32,LCD_COLOR_LOGO);
	LCD_DrawLine(x+43,y-33,x+43+13,y-33,LCD_COLOR_LOGO);
	LCD_DrawLine(x+43,y-34,x+43+14,y-34,LCD_COLOR_LOGO);
	LCD_DrawLine(x+44,y-35,x+44+13,y-35,LCD_COLOR_LOGO);
	LCD_DrawLine(x+44,y-36,x+44+14,y-36,LCD_COLOR_LOGO);
	LCD_DrawLine(x+44,y-37,x+44+14,y-37,LCD_COLOR_LOGO);
	LCD_DrawLine(x+44,y-38,x+44+15,y-38,LCD_COLOR_LOGO);
	LCD_DrawLine(x+44,y-39,x+44+15,y-39,LCD_COLOR_LOGO);
	LCD_DrawLine(x+45,y-40,x+45+14,y-40,LCD_COLOR_LOGO);
	LCD_DrawLine(x+45,y-41,x+45+14,y-41,LCD_COLOR_LOGO);
	LCD_DrawLine(x+45,y-42,x+45+15,y-42,LCD_COLOR_LOGO);
	LCD_DrawLine(x+45,y-43,x+45+15,y-43,LCD_COLOR_LOGO);
	LCD_DrawLine(x+46,y-44,x+46+14,y-44,LCD_COLOR_LOGO);
	LCD_DrawLine(x+46,y-45,x+46+14,y-45,LCD_COLOR_LOGO);
	LCD_DrawLine(x+46,y-46,x+46+15,y-46,LCD_COLOR_LOGO);
	LCD_DrawLine(x+47,y-47,x+47+14,y-47,LCD_COLOR_LOGO);
	LCD_DrawLine(x+47,y-48,x+47+14,y-48,LCD_COLOR_LOGO);
	LCD_DrawLine(x+47,y-49,x+47+15,y-49,LCD_COLOR_LOGO);
	LCD_DrawLine(x+47,y-50,x+47+15,y-50,LCD_COLOR_LOGO);
	LCD_DrawLine(x+47,y-51,x+47+14,y-51,LCD_COLOR_LOGO);
	LCD_DrawLine(x+48,y-52,x+48+14,y-52,LCD_COLOR_LOGO);
	LCD_DrawLine(x+48,y-53,x+48+14,y-53,LCD_COLOR_LOGO);
	LCD_DrawLine(x+49,y-54,x+49+13,y-54,LCD_COLOR_LOGO);
	LCD_DrawLine(x+49,y-55,x+49+14,y-55,LCD_COLOR_LOGO);
	
	/*i*/
	LCD_DrawRect(x+72,y-54,x+72+12,y-54+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+71,y-52,x+71+13-1,y-52,LCD_COLOR_LOGO);
	LCD_DrawRect(x+71,y-51,x+71+12,y-51+4,LCD_COLOR_LOGO);
	LCD_DrawRect(x+70,y-47,x+70+12,y-47+3,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+68,y-44+4,x+68+14-1,y-44+4,LCD_COLOR_LOGO);
	LCD_DrawLine(x+69,y-43+4,x+69+13,y-43+4,LCD_COLOR_LOGO);
	LCD_DrawLine(x+68,y-42+4,x+68+13-1,y-42+4,LCD_COLOR_LOGO);
	LCD_DrawLine(x+68,y-41+4,x+68+12-1,y-41+4,LCD_COLOR_LOGO);
	LCD_DrawRect(x+68-1,y-40+4,x+68-1+13,y-40+4+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+68-1,y-37+4,x+68-1+12-1,y-37+4,LCD_COLOR_LOGO);
	LCD_DrawRect(x+67-1,y-36+4,x+67-1+13,y-36+4+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+67-1,y-33+4,x+67-1+12-1,y-33+4,LCD_COLOR_LOGO);
	LCD_DrawRect(x+66-1,y-32+4,x+66-1+13,y-32+4+2,LCD_COLOR_LOGO);
	LCD_DrawLine(x+66-1,y-30+4,x+66-1+12-1,y-30+4,LCD_COLOR_LOGO);
	LCD_DrawRect(x+65-1,y-29+4,x+65-1+13,y-29+4+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+64-1,y-26+4,x+64-1+14-1,y-26+4,LCD_COLOR_LOGO);
	LCD_DrawRect(x+64-1,y-25+4,x+64-1+13,y-25+4+2,LCD_COLOR_LOGO);
	LCD_DrawRect(x+64-1,y-23+4,x+64-1+12,y-23+4+2,LCD_COLOR_LOGO);
	LCD_DrawLine(x+63-1,y-21+4,x+63-1+13-1,y-21+4,LCD_COLOR_LOGO);
	LCD_DrawRect(x+63-1,y-20+4,x+63-1+12,y-20+4+2,LCD_COLOR_LOGO);
	LCD_DrawRect(x+62-1,y-18+4,x+62-1+13,y-18+4+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+62-1,y-15+4,x+62-1+12-1,y-15+4,LCD_COLOR_LOGO);
	LCD_DrawRect(x+61-1,y-14+4,x+61-1+13,y-14+4+3,LCD_COLOR_LOGO);
	LCD_DrawRect(x+60-1,y-11+4,x+60-1+13,y-11+4+2,LCD_COLOR_LOGO);
	LCD_DrawRect(x+59-1,y-5,x+59-1+13,y-5+2,LCD_COLOR_LOGO);
	LCD_DrawLine(x+59-1,y-3,x+59-1+14-1,y-3,LCD_COLOR_LOGO);
	LCD_DrawRect(x+59-1,y-2,x+59-1+13,y-2+2,LCD_COLOR_LOGO);
	LCD_DrawLine(x+59-1,y,x+59-1+12-1,y,LCD_COLOR_LOGO);
	
	/*n*/
	LCD_DrawLine(x+88,y-40,x+88+23-1,y-40,LCD_COLOR_LOGO);
	LCD_DrawLine(x+85,y-39,x+85+27-1,y-39,LCD_COLOR_LOGO);
	LCD_DrawLine(x+84,y-38,x+84+30-1,y-38,LCD_COLOR_LOGO);
	LCD_DrawLine(x+83,y-37,x+83+32-1,y-37,LCD_COLOR_LOGO);
	LCD_DrawLine(x+83,y-36,x+83+3-1,y-36,LCD_COLOR_LOGO);
	LCD_DrawLine(x+104,y-36,x+104+12-1,y-36,LCD_COLOR_LOGO);
	LCD_DrawLine(x+83,y-35,x+83+1-1,y-35,LCD_COLOR_LOGO);
	LCD_DrawLine(x+104,y-35,x+104+12-1,y-35,LCD_COLOR_LOGO);
	LCD_DrawRect(x+104,y-34,x+104+12,y-34+3,LCD_COLOR_LOGO);
	LCD_DrawRect(x+103,y-31,x+103+13,y-31+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+102,y-28,x+102+13-1,y-28,LCD_COLOR_LOGO);
	LCD_DrawLine(x+103,y-27,x+103+12-1,y-27,LCD_COLOR_LOGO);
	LCD_DrawLine(x+102,y-26,x+102+13-1,y-26,LCD_COLOR_LOGO);
	LCD_DrawRect(x+101,y-25,x+101+13,y-25+2,LCD_COLOR_LOGO);
	LCD_DrawRect(x+101,y-23,x+101+12,y-23+2,LCD_COLOR_LOGO);
	LCD_DrawRect(x+100,y-21,x+100+13,y-21+3,LCD_COLOR_LOGO);
	LCD_DrawRect(x+99,y-18,x+99+13,y-18+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+99,y-15,x+99+12-1,y-15,LCD_COLOR_LOGO);
	LCD_DrawRect(x+98,y-14,x+98+13,y-14+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+98,y-11,x+98+12-1,y-11,LCD_COLOR_LOGO);
	LCD_DrawLine(x+97,y-10,x+97+13-1,y-10,LCD_COLOR_LOGO);
	LCD_DrawRect(x+97,y-9,x+97+12,y-9+4,LCD_COLOR_LOGO);
	LCD_DrawRect(x+96,y-5,x+96+12,y-5+3,LCD_COLOR_LOGO);
	LCD_DrawRect(x+95,y-2,x+95+13,y-2+3,LCD_COLOR_LOGO);
	
	/*k*/
	LCD_DrawLine(x+127,y-56,x+127+12-1,y-56,LCD_COLOR_LOGO);
	LCD_DrawRect(x+126,y-55,x+126+13,y-55+3,LCD_COLOR_LOGO);
	LCD_DrawRect(x+126,y-52,x+126+12,y-52+2,LCD_COLOR_LOGO);
	LCD_DrawRect(x+125,y-50,x+125+12,y-50+2,LCD_COLOR_LOGO);
	LCD_DrawLine(x+124,y-48,x+124+13-1,y-48,LCD_COLOR_LOGO);
	LCD_DrawLine(x+124,y-47,x+124+12-1,y-47,LCD_COLOR_LOGO);
	LCD_DrawLine(x+124,y-46,x+124+13-1,y-46,LCD_COLOR_LOGO);
	LCD_DrawLine(x+124,y-45,x+124+12-1,y-45,LCD_COLOR_LOGO);
	LCD_DrawRect(x+123,y-44,x+123+13,y-44+2,LCD_COLOR_LOGO);
	LCD_DrawRect(x+123,y-42,x+123+12,y-42+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+153,y-41,x+153+12-1,y-41,LCD_COLOR_LOGO);
	LCD_DrawLine(x+151,y-40,x+151+13-1,y-40,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+122,y-39,x+122+13-1,y-39,LCD_COLOR_LOGO);
	LCD_DrawLine(x+150,y-39,x+150+12-1,y-39,LCD_COLOR_LOGO);
	
	LCD_DrawRect(x+122,y-38,x+122+12,y-38+2,LCD_COLOR_LOGO);
	LCD_DrawLine(x+148,y-38,x+148+13-1,y-38,LCD_COLOR_LOGO);
	LCD_DrawLine(x+147,y-37,x+147+12-1,y-37,LCD_COLOR_LOGO);
	
	LCD_DrawRect(x+121,y-36,x+121+13,y-36+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+146,y-36,x+146+12-1,y-36,LCD_COLOR_LOGO);
	LCD_DrawLine(x+145,y-35,x+145+11-1,y-35,LCD_COLOR_LOGO);
	LCD_DrawLine(x+143,y-34,x+143+12-1,y-34,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+120,y-33,x+120+13-1,y-33,LCD_COLOR_LOGO);
	LCD_DrawLine(x+141,y-33,x+141+13-1,y-33,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+121,y-32,x+121+12-1,y-32,LCD_COLOR_LOGO);
	LCD_DrawLine(x+140,y-32,x+140+12-1,y-32,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+120,y-31,x+120+13-1,y-31,LCD_COLOR_LOGO);
	LCD_DrawLine(x+138,y-31,x+138+13-1,y-31,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+120,y-30,x+120+12-1,y-30,LCD_COLOR_LOGO);
	LCD_DrawLine(x+137,y-30,x+137+13-1,y-30,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+119,y-29,x+119+13-1,y-29,LCD_COLOR_LOGO);
	LCD_DrawLine(x+135,y-29,x+135+15-1,y-29,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+119,y-28,x+119+12-1,y-28,LCD_COLOR_LOGO);
	LCD_DrawLine(x+134,y-28,x+134+16-1,y-28,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+119,y-27,x+119+31-1,y-27,LCD_COLOR_LOGO);
	LCD_DrawLine(x+118,y-26,x+118+31-1,y-26,LCD_COLOR_LOGO);
	LCD_DrawLine(x+118,y-25,x+118+32-1,y-25,LCD_COLOR_LOGO);
	LCD_DrawLine(x+118,y-24,x+118+32-1,y-24,LCD_COLOR_LOGO);
	LCD_DrawLine(x+117,y-23,x+117+33-1,y-23,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+118,y-22,x+118+19-1,y-22,LCD_COLOR_LOGO);
	LCD_DrawLine(x+138,y-22,x+138+12-1,y-22,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+117,y-21,x+117+19-1,y-21,LCD_COLOR_LOGO);
	LCD_DrawLine(x+138,y-21,x+138+12-1,y-21,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+117,y-20,x+117+18-1,y-20,LCD_COLOR_LOGO);
	LCD_DrawLine(x+138,y-20,x+138+13-1,y-20,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+116,y-19,x+116+17-1,y-19,LCD_COLOR_LOGO);
	LCD_DrawLine(x+138,y-19,x+138+13-1,y-19,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+116,y-18,x+116+16-1,y-18,LCD_COLOR_LOGO);
	LCD_DrawLine(x+138,y-18,x+138+14-1,y-18,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+116,y-17,x+116+14-1,y-17,LCD_COLOR_LOGO);
	LCD_DrawLine(x+138,y-17,x+138+14-1,y-17,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+116,y-16,x+116+13-1,y-16,LCD_COLOR_LOGO);
	LCD_DrawLine(x+138,y-16,x+138+14-1,y-16,LCD_COLOR_LOGO);
	
	LCD_DrawRect(x+115,y-15,x+115+13,y-15+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+115,y-12,x+115+12-1,y-12,LCD_COLOR_LOGO);
	LCD_DrawRect(x+114,y-11,x+114+13,y-11+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+114,y-8,x+114+12-1,y-8,LCD_COLOR_LOGO);
	LCD_DrawRect(x+113,y-7,x+113+13,y-7+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+112,y-4,x+112+14-1,y-4,LCD_COLOR_LOGO);
	LCD_DrawRect(x+112,y-3,x+112+13,y-3+3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+112,y,x+112+12-1,y,LCD_COLOR_LOGO);
	
	LCD_DrawRect(x+139,y-15,x+139+13,y-15+3,LCD_COLOR_LOGO);	
	LCD_DrawRect(x+139,y-12,x+139+14,y-12+2,LCD_COLOR_LOGO);
	LCD_DrawRect(x+140,y-10,x+140+13,y-10+3,LCD_COLOR_LOGO);
	LCD_DrawRect(x+140,y-7,x+140+14,y-7+3,LCD_COLOR_LOGO);
	LCD_DrawRect(x+140,y-4,x+140+15,y-4+2,LCD_COLOR_LOGO);
	LCD_DrawLine(x+141,y-2,x+141+14-1,y-2,LCD_COLOR_LOGO);
	LCD_DrawLine(x+143,y-1,x+143+12-1,y-1,LCD_COLOR_LOGO);
	LCD_DrawLine(x+145,y,x+145+10-1,y,LCD_COLOR_LOGO);
	
	/*o*/
	LCD_DrawLine(x+173,y-41,x+173+35-1,y-41,LCD_COLOR_LOGO);
	LCD_DrawLine(x+171,y-40,x+171+39-1,y-40,LCD_COLOR_LOGO);
	LCD_DrawLine(x+169,y-39,x+169+41-1,y-39,LCD_COLOR_LOGO);
	LCD_DrawLine(x+168,y-38,x+168+43-1,y-38,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+167,y-37,x+167+13-1,y-37,LCD_COLOR_LOGO);
	LCD_DrawLine(x+198,y-37,x+198+13-1,y-37,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+166,y-36,x+166+13-1,y-36,LCD_COLOR_LOGO);
	LCD_DrawLine(x+199,y-36,x+199+13-1,y-36,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+166,y-35,x+166+12-1,y-35,LCD_COLOR_LOGO);
	LCD_DrawLine(x+198,y-35,x+198+14-1,y-35,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+165,y-34,x+165+13-1,y-34,LCD_COLOR_LOGO);
	LCD_DrawLine(x+198,y-34,x+198+14-1,y-34,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+165,y-33,x+165+13-1,y-33,LCD_COLOR_LOGO);
	LCD_DrawLine(x+198,y-33,x+198+13-1,y-33,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+165,y-32,x+165+13-1,y-32,LCD_COLOR_LOGO);
	LCD_DrawLine(x+198,y-32,x+198+13-1,y-32,LCD_COLOR_LOGO);
	
	LCD_DrawRect(x+164,y-31,x+164+14,y-31+2,LCD_COLOR_LOGO);
	LCD_DrawRect(x+197,y-31,x+197+14,y-31+2,LCD_COLOR_LOGO);
	LCD_DrawLine(x+164,y-29,x+164+13-1,y-29,LCD_COLOR_LOGO);
	LCD_DrawLine(x+197,y-29,x+197+13-1,y-29,LCD_COLOR_LOGO);
	
	LCD_DrawRect(x+163,y-28,x+163+14,y-28+2,LCD_COLOR_LOGO);
	LCD_DrawRect(x+196,y-28,x+196+14,y-28+2,LCD_COLOR_LOGO);
	LCD_DrawLine(x+163,y-26,x+163-1+13,y-26,LCD_COLOR_LOGO);
	LCD_DrawLine(x+196,y-26,x+196-1+13,y-26,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+162,y-25,x+162-1+14,y-25,LCD_COLOR_LOGO);
	LCD_DrawLine(x+196,y-25,x+196-1+13,y-25,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+162,y-25,x+162-1+13,y-25,LCD_COLOR_LOGO);
	LCD_DrawLine(x+195,y-25,x+195-1+14,y-25,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+162,y-24,x+162-1+14,y-24,LCD_COLOR_LOGO);
	LCD_DrawLine(x+195,y-24,x+195-1+13,y-24,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+162,y-23,x+162-1+13,y-23,LCD_COLOR_LOGO);
	LCD_DrawLine(x+195,y-23,x+195-1+13,y-23,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+161,y-22,x+161-1+14,y-22,LCD_COLOR_LOGO);
	LCD_DrawLine(x+194,y-22,x+194-1+14,y-22,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+161,y-21,x+161-1+14,y-21,LCD_COLOR_LOGO);
	LCD_DrawLine(x+194,y-21,x+194-1+14,y-21,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+161,y-20,x+161-1+13,y-20,LCD_COLOR_LOGO);
	LCD_DrawLine(x+194,y-20,x+194-1+13,y-20,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+160,y-19,x+160-1+14,y-19,LCD_COLOR_LOGO);
	LCD_DrawLine(x+194,y-19,x+194-1+13,y-19,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+160,y-18,x+160-1+14,y-18,LCD_COLOR_LOGO);
	LCD_DrawLine(x+193,y-18,x+193-1+14,y-18,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+160,y-17,x+160-1+13,y-17,LCD_COLOR_LOGO);
	LCD_DrawLine(x+193,y-17,x+193-1+14,y-17,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+159,y-16,x+159-1+14,y-16,LCD_COLOR_LOGO);
	LCD_DrawLine(x+193,y-16,x+193-1+14,y-16,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+160,y-15,x+160-1+13,y-15,LCD_COLOR_LOGO);
	LCD_DrawLine(x+193,y-15,x+193-1+13,y-15,LCD_COLOR_LOGO);	
	
	LCD_DrawRect(x+159,y-14,x+159+13,y-14+4,LCD_COLOR_LOGO);
	LCD_DrawLine(x+192,y-14,x+192-1+14,y-14,LCD_COLOR_LOGO);	
	LCD_DrawRect(x+192,y-13,x+192+13,y-13+3,LCD_COLOR_LOGO);
	
	LCD_DrawRect(x+159,y-10,x+159+12,y-10+3,LCD_COLOR_LOGO);
	LCD_DrawRect(x+191,y-10,x+191+13,y-10+2,LCD_COLOR_LOGO);
	LCD_DrawLine(x+190,y-8,x+190-1+13,y-8,LCD_COLOR_LOGO);	
	
	LCD_DrawLine(x+159,y-7,x+159-1+14,y-7,LCD_COLOR_LOGO);
	LCD_DrawLine(x+188,y-7,x+188-1+14,y-7,LCD_COLOR_LOGO);
	
	LCD_DrawLine(x+159,y-6,x+159-1+43,y-6,LCD_COLOR_LOGO);
	LCD_DrawLine(x+160,y-5,x+160-1+41,y-5,LCD_COLOR_LOGO);
	LCD_DrawLine(x+161,y-4,x+161-1+39,y-4,LCD_COLOR_LOGO);
	LCD_DrawLine(x+162,y-3,x+162-1+36,y-3,LCD_COLOR_LOGO);
	LCD_DrawLine(x+163,y-2,x+163-1+34,y-2,LCD_COLOR_LOGO);
	LCD_DrawLine(x+164,y-1,x+164-1+32,y-1,LCD_COLOR_LOGO);
//	
//	/*��*/
//	LCD_DrawLine(x+224,y-36,18,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+223,y-35,22,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+222,y-34,24,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+221,y-33,25,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+221,y-32,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+238,y-32,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+220,y-31,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+238,y-31,9,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+219,y-30,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+238,y-30,9,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+218,y-29,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+239,y-29,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+218,y-28,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+239,y-28,9,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+217,y-27,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+226,y-27,22,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+215,y-26,33,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+214,y-25,35,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+213,y-24,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+227,y-24,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+232,y-24,9,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+213,y-23,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+227,y-23,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+232,y-23,9,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+227,y-22,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+226,y-21,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+226,y-20,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+213,y-19,34,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+212,y-18,35,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+212,y-17,35,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+225,y-16,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+213,y-15,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+225,y-15,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+238,y-15,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+212,y-14,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+225,y-14,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+237,y-14,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+212,y-13,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+224,y-13,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+237,y-13,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+212,y-12,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+224,y-12,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+236,y-12,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+212,y-11,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+223,y-11,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+235,y-11,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+212,y-10,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+223,y-10,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+235,y-10,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+212,y-9,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+223,y-9,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+235,y-9,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+212,y-8,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+222,y-8,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+234,y-8,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+212,y-7,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+222,y-7,19,LCD_DIR_HORIZONTAL);

//	LCD_DrawLine(x+214,y-6,16,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+231,y-6,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+217,y-5,4,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+222,y-5,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+232,y-5,4,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+222,y-4,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawFullRect(x+206,y-3,39,2);
//	
//	/*��*/
//	LCD_DrawLine(x+269,y-38,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+285,y-38,6,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+267,y-37,9,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+285,y-37,6,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+265,y-36,11,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+284,y-36,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+256,y-35,18,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+275,y-35,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+284,y-35,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+256,y-34,17,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+275,y-34,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+283,y-34,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+256,y-33,15,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+274,y-33,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+283,y-33,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+261,y-32,6,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+274,y-32,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+283,y-32,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+261,y-31,6,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+274,y-31,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+282,y-31,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+261,y-30,6,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+274,y-30,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+282,y-30,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+260,y-29,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+274,y-29,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+282,y-29,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+253,y-28,20,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+275,y-28,6,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+282,y-28,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+253,y-27,19,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+274,y-27,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+282,y-27,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+259,y-26,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+274,y-26,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+282,y-26,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+258,y-25,8,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+274,y-25,14,LCD_DIR_HORIZONTAL);

//	LCD_DrawLine(x+259,y-24,6,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+277,y-24,11,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+253,y-23,4,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+258,y-23,10,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+271,y-23,5,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+281,y-23,6,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+253,y-22,16,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+270,y-22,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+280,y-22,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+253,y-21,16,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+270,y-21,6,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+280,y-21,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+252,y-20,24,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+280,y-20,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+252,y-19,4,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+257,y-19,12,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+270,y-19,6,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+280,y-19,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+252,y-18,4,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+257,y-18,12,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+270,y-18,6,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+279,y-18,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+251,y-17,18,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+270,y-17,6,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+279,y-17,7,LCD_DIR_HORIZONTAL);
//		
//	LCD_DrawLine(x+251,y-16,12,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+264,y-16,4,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+269,y-16,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+279,y-16,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+251,y-15,17,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+269,y-15,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+278,y-15,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+251,y-14,11,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+263,y-14,5,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+269,y-14,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+278,y-14,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+250,y-13,17,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+270,y-13,6,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+278,y-13,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+250,y-12,17,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+271,y-12,6,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+278,y-12,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+250,y-11,11,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+262,y-11,5,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+273,y-11,12,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+249,y-10,18,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+277,y-10,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+249,y-9,5,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+255,y-9,12,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+273,y-9,14,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+248,y-8,39,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+248,y-7,12,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+260,y-7,22,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+248,y-6,12,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+260,y-6,5,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+275,y-6,7,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+248,y-5,18,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+274,y-5,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+248,y-4,4,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+253,y-4,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+260,y-4,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+274,y-4,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+247,y-3,4,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+253,y-3,6,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+260,y-3,5,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+274,y-3,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+247,y-2,4,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+253,y-2,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+262,y-2,2,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+273,y-2,9,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+247,y-1,1,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+252,y-1,7,LCD_DIR_HORIZONTAL);
//	LCD_DrawLine(x+273,y-1,8,LCD_DIR_HORIZONTAL);
//	
//	LCD_DrawLine(x+273,y,2,LCD_DIR_HORIZONTAL);
}

//��ʾ ����Ŀ�ݼ�����
void Disp_Fastbutton(void)
{
	uint32_t i;
	for(i=0;i<6;i++)
	{
		LCD_DrawRect( 4*i+i*76, 228, 76+i*80, 271 , LCD_COLOR_TEST_BUTON ) ;
		LCD_DrawLine( 80*i+1, 227, 76+i*80, 227 , LCD_COLOR_TEST_LINE );
		//if(i<5)
		LCD_DrawLine( 76+i*80, 227, 76+i*80, 271 , LCD_COLOR_TEST_LINE );
		LCD_DrawLine( 76+i*80+1, 227, 76+i*80+1, 271 , Black );
		LCD_DrawLine( 76+i*80+2, 227, 76+i*80+2, 271 , Black );
		LCD_DrawLine( 76+i*80+3, 227, 76+i*80+3, 271 , Black );
		
	}

}//118





void Disp_button_Num_time(void)
	
{
	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;
	WriteString_16(84, 271-30, "msec",  0);
	WriteString_16(84+80, 271-30, "sec",  0);


}

void Disp_button_Num_V(void)
	
{
	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;
	WriteString_16(84, 271-30, "V",  0);


}

void Disp_button_Num_A(void)
	
{
	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;
	WriteString_16(84, 271-30, "A",  0);


}

void Disp_button_Num_ms(void)
	
{
	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;
	WriteString_16(84, 271-30, "ms",  0);


}
//������������n,u,p,m,next,
void Disp_button_Num_Input(uint8_t page)    
	
{
	uint8_t i;
	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;//
	if(page==0)
	{
		for(i=0;i<5;i++)
		{ 
			if(i==4)
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+14, BUTTOM_Y_VALUE, Set_Unit[i],  0);
			else 
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+29, BUTTOM_Y_VALUE, Set_Unit[i],  0);
		}
	}
	else
	{
		for(i=0;i<5;i++)
		{
			if(i==4)
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+14, BUTTOM_Y_VALUE, Set_Unit[i+5],  0);
			else if(i==0)
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+24, BUTTOM_Y_VALUE, Set_Unit[i+5],  0);
			else if(i==3)
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+10, BUTTOM_Y_VALUE, Set_Unit[i+5],  0);
			else
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+29, BUTTOM_Y_VALUE, Set_Unit[i+5],  0);
		}
	}
	
//	WriteString_16(84+80, 271-30, "sec",  0);


}
void Disp_button_Num_Freq(void)
	
{
	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;
	WriteString_16(84, 271-30, "Hz",  0);
	WriteString_16(84+80, 271-30, "KHz",  0);


}
void Disp_button_Num_Avg(void)
	
{
	Disp_Fastbutton();
	
	Colour.black=LCD_COLOR_TEST_BUTON;
	Colour.Fword=White;
	WriteString_16(84, 271-30, "*1",  0);
	//WriteString_16(84+80, 271-30, "KHz",  0);


}

void  Disp_Button_Test(void)
{
		Disp_Fastbutton();
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
	if(SaveSIM.lang == 1)
	{
		WriteString_16(83, 271-29, "MEASSET",  0);
		WriteString_16(83+80,271-29, "SYSSET",  0);
//		WriteString_16(92+80+80,271-29, "COUNT ",  0);
	}else if(SaveSIM.lang == 0){
		WriteString_16(83, 271-29, "��������",  0);
		WriteString_16(83+80,271-29, "ϵͳ����",  0);
//		WriteString_16(92+80+80,271-29, "������",  0);
	}
	//	WriteString_16(83, 271-29, "��������",  0);
	  
	//	WriteString_16(83+80,271-29, "������ʾ",  0);
	  
	//	WriteString_16(92+80+80,271-29, "������",  0);
	

}



void Disp_Button_value1(uint32_t value)
{
	Disp_Fastbutton();
	if(value==0)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		if(SaveSIM.lang == 1)
		{
			WriteString_16(83, 271-29, "MEASSET",  0);
			WriteString_16(83+80,271-29, "No.DISP",  0);
			WriteString_16(92+80+80,271-29, "COUNT ",  0);
		}else if(SaveSIM.lang == 0){
			WriteString_16(83, 271-29, "������ʾ",  0);
			WriteString_16(83+80,271-29, "������ʾ",  0);
			WriteString_16(92+80+80,271-29, "������",  0);
		}
//	    WriteString_16(64, 100, "����",  0);
//		WriteString_16(84, 271-40, "����",  0);
//		WriteString_16(84, 271-20, "��ʾ",  0);
//		WriteString_16(84+80, 271-40, "����",  0);
//		WriteString_16(84+80, 271-20, "��ʾ",  0);
//		WriteString_16(84+80+80, 271-40, "��",  0);
//		WriteString_16(84+80+80, 271-20, "����",  0);
//		WriteString_16(83, 271-29, "������ʾ",  0);
//		WriteString_16(83+80,271-29, "������ʾ",  0);
//		WriteString_16(92+80+80,271-29, "������",  0);
//		WriteString_16(83, 271-29, "MEASSET",  0);
//		WriteString_16(83+80,271-29, "No.DISP",  0);
//		WriteString_16(92+80+80,271-29, "COUNT ",  0);
//		WriteString_16(84+80+80+80, 271-40, "�б�",  0);
//		WriteString_16(84+80+80+80, 271-20, "ɨ��",  0);
//		WriteString_16(84+80+80+80+80, 271-40, "����",  0);
//		WriteString_16(84+80+80+80+80, 271-20, " 1/2",  0);
	}
    else if(value==1)
    {
        Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
//		WriteString_16(84, 271-40, "�ļ�",  0);
//		WriteString_16(84, 271-20, "����",  0);
//		WriteString_16(84+80, 271-40, "����",  0);
//		WriteString_16(84+80, 271-20, "����",  0);
//        WriteString_16(84+80+80+80+80, 271-40, "����",  0);
//		WriteString_16(84+80+80+80+80, 271-20, "2/2",  0);
		
//			WriteString_16(83, 271-29, "�ļ�����",  0);
//			WriteString_16(83+80, 271-29, "��������",  0);
//			WriteString_16(84+80+80+80+80-1, 271-29, "����",  0);
		if(SaveSIM.lang == 1)
		{
			WriteString_16(83, 271-29, "FILE SET",  0);
			WriteString_16(83+80, 271-29, "SaveData",  0);
			WriteString_16(84+80+80+80+80-1, 271-29, "MORE",  0);
		}else if(SaveSIM.lang == 0){
			WriteString_16(83, 271-29, "�ļ�����",  0);
			WriteString_16(83+80, 271-29, "��������",  0);
			WriteString_16(84+80+80+80+80-1, 271-29, "����",  0);
		}
			WriteString_16(84+80+80+80+80+41, 271-29, "2/2",  0);
    }
    else
    {
        Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
//        WriteString_16(83, 271-29, "  ȡ�� ",  0);
//		WriteString_16(83+80,271-29, "  ȷ�� ",  0);
		if(SaveSIM.lang == 1)
		{
			 WriteString_16(83, 271-29, "CANCEL ",  0);
			WriteString_16(83+80,271-29, "CONFIRM",  0);
		}else if(SaveSIM.lang == 0){
			WriteString_16(83, 271-29, "  ȡ�� ",  0);
			WriteString_16(83+80,271-29, "  ȷ�� ",  0);
		}
		//WriteString_16(92+80+80,271-29, "������",  0);
    
    
    }

}
void Disp_Button_ItemScan_no(void)
{
	Disp_Fastbutton();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BUTON;

	if(SaveSIM.lang == 1)
	{
		WriteString_16(84, 271-40, "CLEA",  0);
		WriteString_16(84, 271-20, "LIST",  0);
	}else if(SaveSIM.lang == 0){
		WriteString_16(84, 271-40, "���",  0);
		WriteString_16(84, 271-20, "���",  0);
	}
	
//				WriteString_16(84+80, 271-40, "�Զ�",  0);
//				WriteString_16(84+80, 271-20, "����",  0);
//        WriteString_16(84+80+80, 271-30, "����",  0);
//		WriteString_16(84+80+80, 271-20, "����",  0);
//	WriteString_16(84+80+80+80, 271-30, "��һҳ",  0);
//	WriteString_16(84+80+80+80+80, 271-30, "��һҳ",  0);
	if(SaveSIM.lang == 1)
	{
		WriteString_16(84+80+80+80, 271-30, "Last P",  0);
		WriteString_16(84+80+80+80+80, 271-30, "Next P",  0);
	}else if(SaveSIM.lang == 0){
		WriteString_16(84+80+80+80, 271-30, "��һҳ",  0);
		WriteString_16(84+80+80+80+80, 271-30, "��һҳ",  0);
	}
	
}
void Disp_Button_ItemScanSet(uint32_t value)
{
	Disp_Fastbutton();
 	if(value==0||value==1)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		if(SaveSIM.lang == 1)
		{
			WriteString_16(84, 271-30, "DelRow",  0);
			WriteString_16(84+80, 271-40, "LINE",  0);
			WriteString_16(84+80, 271-20, "ATOU",  0);
			WriteString_16(84+80+80, 271-40, "LOG ",  0);
			WriteString_16(84+80+80, 271-20, "ATOU",  0);
			WriteString_16(84+80+80+80, 271-30, "Last P",  0);
			WriteString_16(84+80+80+80+80, 271-30, "Next P",  0);
			
		}else if(SaveSIM.lang == 0){
			WriteString_16(84, 271-30, "ɾ����",  0);
	//		WriteString_16(84, 271-20, "����",  0);
			WriteString_16(84+80, 271-40, "����",  0);
			WriteString_16(84+80, 271-20, "�Զ�",  0);
			WriteString_16(84+80+80, 271-40, "����",  0);
			WriteString_16(84+80+80, 271-20, "�Զ�",  0);
			WriteString_16(84+80+80+80, 271-30, "��һҳ",  0);
	//		WriteString_16(84+80+80+80, 271-20, "����",  0);
			WriteString_16(84+80+80+80+80, 271-30, "��һҳ",  0);
		}
		//WriteString_16(84, 271-30, "ɾ����",  0);
//		WriteString_16(84, 271-20, "����",  0);
		//WriteString_16(84+80, 271-40, "����",  0);		
		//WriteString_16(84+80, 271-20, "�Զ�",  0);
		
		
	//	WriteString_16(84+80+80, 271-20, "�Զ�",  0);
		
//		WriteString_16(84+80+80+80, 271-20, "����",  0);
		
//		WriteString_16(84+80+80+80+80, 271-20, " 1/2",  0);
	}
    else 
//		if(value==2)
//		{
//			Colour.Fword=White;
//				Colour.black=LCD_COLOR_TEST_BUTON;
//				WriteString_16(84, 271-40, "���",  0);
//				WriteString_16(84, 271-20, "���",  0);
////				WriteString_16(84+80, 271-40, "�Զ�",  0);
////				WriteString_16(84+80, 271-20, "����",  0);
//		//        WriteString_16(84+80+80, 271-30, "����",  0);
//		//		WriteString_16(84+80+80, 271-20, "����",  0);
//				WriteString_16(84+80+80+80, 271-30, "��һҳ",  0);
//				WriteString_16(84+80+80+80+80, 271-30, "��һҳ",  0);
//			
//		
//		}else
				
			{
				Colour.Fword=White;
				Colour.black=LCD_COLOR_TEST_BUTON;
				if(SaveSIM.lang == 1)
				{
					WriteString_16(84, 271-30, "DelRow",  0);
					WriteString_16(84+80, 271-40, "ATOU",  0);
					WriteString_16(84+80, 271-20, "COPY",  0);
					WriteString_16(84+80+80+80, 271-30, "Last P",  0);
					WriteString_16(84+80+80+80+80, 271-30, "Next P",  0);
					
				}else if(SaveSIM.lang == 0){
					WriteString_16(84, 271-30, "ɾ����",  0);
			//		WriteString_16(84, 271-20, "����",  0);
					WriteString_16(84+80, 271-40, "�Զ�",  0);
					WriteString_16(84+80, 271-20, "����",  0);
			//        WriteString_16(84+80+80, 271-30, "����",  0);
			//		WriteString_16(84+80+80, 271-20, "����",  0);
					WriteString_16(84+80+80+80, 271-30, "��һҳ",  0);
					WriteString_16(84+80+80+80+80, 271-30, "��һҳ",  0);
				}
			//	WriteString_16(84, 271-30, "ɾ����",  0);
		//		WriteString_16(84, 271-20, "����",  0);
			//	WriteString_16(84+80, 271-40, "�Զ�",  0);
				
			//	WriteString_16(84+80, 271-20, "����",  0);
				
		//        WriteString_16(84+80+80, 271-30, "����",  0);
		//		WriteString_16(84+80+80, 271-20, "����",  0);
				
			
			
			}


}
void Disp_Button_TestSet(uint32_t value)
{
	Disp_Fastbutton();
 	if(value==0)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
//		WriteString_16(84, 271-40, "����",  0);
//		WriteString_16(84, 271-20, "����",  0);
//		WriteString_16(84+80, 271-40, "�û�",  0);
//		WriteString_16(84+80, 271-20, "У��",  0);
//		WriteString_16(84+80+80, 271-40, "����",  0);
//		WriteString_16(84+80+80, 271-20, "����",  0);
		if(SaveSIM.lang == 1)
		{
			WriteString_16(83, 271-29, "MEADISP",  0);
			WriteString_16(83+80, 271-29, "SYSSET",  0);
//			WriteString_16(83+80+80, 271-29, "LMTSET",  0);
//			WriteString_16(83+80+80+80, 271-29, "SYSSET",  0);
			
		}else if(SaveSIM.lang == 0){
			WriteString_16(83, 271-29, "������ʾ",  0);
			WriteString_16(83+80, 271-29, "ϵͳ����",  0);
//			WriteString_16(83+80+80, 271-29, "��������",  0);
//			WriteString_16(83+80+80+80, 271-29, "ϵͳ����",  0);
		}
		
		
		
//		WriteString_16(84+80+80+80, 271-40, "�б�",  0);
//		WriteString_16(84+80+80+80, 271-20, "����",  0);
//		WriteString_16(84+80+80+80+80, 271-40, "����",  0);
//		WriteString_16(84+80+80+80+80, 271-20, " 1/2",  0);
	}
    else if(value==1)
    {
        Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
//		WriteString_16(84, 271-40, "�ļ�",  0);
//		WriteString_16(84, 271-20, "����",  0);
//		WriteString_16(84+80, 271-40, "ϵͳ",  0);
//		WriteString_16(84+80, 271-20, "����",  0);
//        WriteString_16(84+80+80, 271-30, "����",  0);
//		WriteString_16(84+80+80, 271-20, "����",  0);
//        WriteString_16(84+80+80+80+80, 271-40, "����",  0);
		if(SaveSIM.lang == 1)
		{
			WriteString_16(84, 271-40, "FILE",  0);
			WriteString_16(84, 271-20, "MANE",  0);
			WriteString_16(84+80, 271-40, "SYS ",  0);
			WriteString_16(84+80, 271-20, "SET ",  0);
			WriteString_16(84+80+80, 271-30, "TOOL",  0);
			WriteString_16(84+80+80, 271-20, "SET ",  0);
			WriteString_16(84+80+80+80+80, 271-40, "MORE",  0);
			WriteString_16(84+80+80+80+80, 271-20, " 2/2",  0);  
			
		}else if(SaveSIM.lang == 0){
			WriteString_16(84, 271-40, "�ļ�",  0);
			WriteString_16(84, 271-20, "����",  0);
			WriteString_16(84+80, 271-40, "ϵͳ",  0);
			WriteString_16(84+80, 271-20, "����",  0);
			WriteString_16(84+80+80, 271-30, "����",  0);
			WriteString_16(84+80+80, 271-20, "����",  0);
			WriteString_16(84+80+80+80+80, 271-40, "����",  0);
			WriteString_16(84+80+80+80+80, 271-20, " 2/2",  0);  
		}
		  
    
    }
    else
    {
        Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
//        WriteString_16(83, 271-29, "����У��",  0);
//		WriteString_16(83+80, 271-29, "����У��",  0);
//		WriteString_16(83+80+80, 271-29, "�˳�����",  0);
//		WriteString_16(83+80+80+80, 271-29, "���У��",  0);
		if(SaveSIM.lang == 1)
		{
			WriteString_16(83, 271-29, "VOLCal",  0);
			WriteString_16(83+80, 271-29, "CtrlCal",  0);
			WriteString_16(83+80+80, 271-29, "CurCal",  0);
//			WriteString_16(83+80+80+80, 271-29, "Cle Corr",  0); 
			
		}else if(SaveSIM.lang == 0){
			WriteString_16(83, 271-29, "��ѹУ��",  0);
			WriteString_16(83+80, 271-29, "����У��",  0);
			WriteString_16(83+80+80, 271-29, "����У��",  0);
//			WriteString_16(83+80+80+80, 271-29, "���У��",  0); 
		}
     
    }

}

void Disp_Button_SysSet(void)
{

	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BUTON;
		
//		WriteString_16(83, 271-29, "��������",  0);
	//	WriteString_16(83, 271-29, "��������",  0);
	if(SaveSIM.lang == 1)
	{
		WriteString_16(83, 271-29, "MEADISP",  0);
		WriteString_16(83+80, 271-29, "MeasSet",  0);
		
	}else if(SaveSIM.lang == 0){
		WriteString_16(83, 271-29, "������ʾ",  0);
		WriteString_16(83+80, 271-29, "��������",  0); 
	}
	
//		WriteString_16(83, 271-29, "ϵͳ",  0);
//		WriteString_16(83, 271-29, "����",  0);
		
//		WriteString_16(83+80, 271-29, "����",  0);
//		WriteString_16(84+80+80, 271-40, "����",  0);
//		WriteString_16(84+80+80, 271-20, "����",  0);
//		WriteString_16(84+80+80+80, 271-40, "Ĭ��",  0);
//		WriteString_16(84+80+80+80, 271-20, "����",  0);
//		WriteString_16(84+80+80+80+80, 271-40, "ϵͳ",  0);
//		WriteString_16(84+80+80+80+80, 271-20, "��λ",  0);


}

void Disp_Button_Correction(void)
{

		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;	
//		WriteString_16(83, 271-29, "����У��",  0);
//		WriteString_16(83+80, 271-29, "����У��",  0);
//		WriteString_16(83+160+19, 271-29, "�˳�",  0);
//		WriteString_16(83+160+160, 271-29, "���У��",  0);
	if(SaveSIM.lang == 1)
	{
		WriteString_16(83, 271-29, "C Corret",  0);
		WriteString_16(83+80, 271-29, "R Corret",  0);
		WriteString_16(83+160+19, 271-29, "Exit",  0);
		WriteString_16(83+160+160, 271-29, "Cle Corr",  0);
		
	}else if(SaveSIM.lang == 0){
		WriteString_16(83, 271-29, "����У��",  0);
		WriteString_16(83+80, 271-29, "����У��",  0);
		WriteString_16(83+160+19, 271-29, "�˳�",  0);
		WriteString_16(83+160+160, 271-29, "���У��",  0);
	}
	
//		WriteString_16(83+80+19, 271-29, "ȷ��",  0);
}
void Disp_TopBar_Color(void)	//���˵��ĵ�ɫ��ɫ
{
	LCD_DrawRect( 0, 0, 210, 22 , LCD_COLOR_TEST_BAR);

}
void Disp_MidRect(void)
{
	LCD_DrawRect(0,92,479 ,205 , LCD_COLOR_TEST_MID ) ;
}
void Disp_TestScreen(void)
{
//	Disp_TopBar_Color();
	LCD_DrawRect(0, 0, 160,22 , LCD_COLOR_TEST_BAR);
	Disp_Fastbutton();//��ʾ���ٰ���
	Disp_MidRect();//��ʾ�м�ɫ��
}
//
//������ʾ���˵�����Ŀ��ʾ
//
void Disp_Test_Item(void)
{
	uint32_t i;
	Disp_TestScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveSIM.lang == 1)
	{
		WriteString_16(0,4, All_TopName_E[0],  0);
		
	}else if(SaveSIM.lang == 0){
		WriteString_16(0,4, All_TopName[0],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	for(i=0;i<6;i++)
	{
		if(i<3)
		{
			if(SaveSIM.lang == 1)
			{
				WriteString_16(0, 26+i*22, Test_Setitem_E[i],  0);
				
			}else if(SaveSIM.lang == 0){
				WriteString_16(0, 26+i*22, Test_Setitem[i],  0);
			}
		}else{
			if(SaveSIM.lang == 1)
			{
				WriteString_16(250,26+(i-3)*22, Test_Setitem_E[i],  0);
				
			}else if(SaveSIM.lang == 0){
				WriteString_16(250,26+(i-3)*22, Test_Setitem[i],  0);
			}
			
		}

		
	}
	Disp_Button_Test();
	

}
//������ʾ�Ӻ���
void Disp_Range_MidRect(void)
{

	LCD_DrawRect( 0, 160,479 , 190 , LCD_COLOR_TEST_MID ) ;
}
void Disp_RangeScreen(void)
{
//	Disp_TopBar_Color();
	LCD_DrawRect( 0, 0, 160, 22 , LCD_COLOR_TEST_BAR);
	Disp_Fastbutton();//��ʾ���ٰ���
	Disp_Range_MidRect();//��ʾ�м�ɫ��
}
//������ʾ���˵���Ŀ��ʾ
void Disp_Range_Item(void)
{
	uint32_t i;
	Disp_RangeScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveSIM.lang == 1)
	{
		WriteString_16(0, 4, All_TopName_E[1],  0);
		
	}else if(SaveSIM.lang == 0){
		WriteString_16(0, 4, All_TopName[1],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	for(i=0;i<6;i++)
	{
		if(i<3)
		{
			if(SaveSIM.lang == 1)
			{
				WriteString_16(0, 26+i*22, Range_Item_E[i],  0);
				
			}else if(SaveSIM.lang == 0){
				WriteString_16(0, 26+i*22, Range_Item[i],  0);
			}
			
		}else{
			if(SaveSIM.lang == 1)
			{
				WriteString_16(250, 26+(i-3)*22, Range_Item_E[i],  0);
				
			}else if(SaveSIM.lang == 0){
				WriteString_16(250, 26+(i-3)*22, Range_Item[i],  0);
			}
			
		}
//		WriteString_16(0, 210, "R :",  0);
//		WriteString_16(120, 210, Test_Setitem[7],  0);
		
	
	}
	Disp_RangeDispvalue(SaveData.Main_Func.Param.test);
	
	Disp_Button_value1(0);
	

}
//��������ʾ�Ӻ���

void Disp_Rang_Count_MidRect(void)
{

	LCD_DrawRect( 0, 86,479 , 160 , LCD_COLOR_TEST_MID ) ;		//��ɫ
	LCD_DrawRect( 0, 193,479 , 227 , LCD_COLOR_TEST_MID ) ;
}

void Disp_Range_CountScreen(void)
{
//	Disp_TopBar_Color();
	LCD_DrawRect( 0, 0, 178, 22 , LCD_COLOR_TEST_BAR);
	Disp_Fastbutton();//��ʾ���ٰ���
	Disp_Rang_Count_MidRect();//��ʾ�м�ɫ��
}
//��������ʾ����
void Disp_Range_Count_Item(void)
{
	uint32_t i;
	Disp_Range_CountScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveSIM.lang == 1)
	{
		WriteString_16(0, 4, All_TopName_E[2],  0);	
	}else if(SaveSIM.lang == 0){
		WriteString_16(0, 4, All_TopName[2],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	for(i=0;i<3;i++)
	{
		if(i==1)
		{
			if(SaveSIM.lang == 1)
			{
				WriteString_16(i*160+30, FIRSTLINE-2, Range_Count_Item_E[i],  0);	
			}else if(SaveSIM.lang == 0){
				WriteString_16(i*160+30, FIRSTLINE-2, Range_Count_Item[i],  0);
			}		
		}else if(i==2){
			if(SaveSIM.lang == 1)
			{
				WriteString_16(i*160+40, FIRSTLINE-2, Range_Count_Item_E[i],  0);	
			}else if(SaveSIM.lang == 0){
				WriteString_16(i*160+40, FIRSTLINE-2, Range_Count_Item[i],  0);
			}			
		}else{
			if(SaveSIM.lang == 1)
			{
				WriteString_16(i*160, FIRSTLINE-2, Range_Count_Item_E[i],  0);	
			}else if(SaveSIM.lang == 0){
				WriteString_16(i*160, FIRSTLINE-2, Range_Count_Item[i],  0);
			}			
		}
//		WriteString_16(0, 210, "R :",  0);
//		WriteString_16(120, 210, Test_Setitem[7],  0);//WriteString_12
		
	
	}
	WriteString_16(LIST1+60, FIRSTLINE-2, User_Freq[SaveData.Main_Func.Freq],  1);//��ʾ����
	Hex_Format(SaveData.Limit_Tab.Nom.Num , SaveData.Limit_Tab.Nom.Dot , 6 , 0);
	WriteString_16(LIST2, FIRSTLINE-2, DispBuf,  1);//��ʾ���ֵ  ����Ҫ������ʾ��λ
	WriteString_16(LIST2+70, FIRSTLINE-2, Disp_Unit[SaveData.Limit_Tab.Nom.Unit],  1);
	Disp_Set_Unit_16(SaveData.Main_Func.Param.test,SELECT_2END-112,FIRSTLINE-2 );//��ʾ��λ
//	WriteString_16(LIST2-30, FIRSTLINE, DispBuf,  1);
	
	WriteString_12(0,FIRSTLINE+20-2,"BIN",0);//Num_1
	WriteString_12(120,FIRSTLINE+20-2,"LOW [F]",0);
	WriteString_12(250,FIRSTLINE+20-2,"HIGH[F]",0);
	WriteString_12(413,FIRSTLINE+20-2,"COUNT",0);
	
	for(i=6;i<15;i++)
	{
		if((i-6>1&&i-6<7)||((i-6>8)))
			Colour.black=LCD_COLOR_TEST_MID;
		else
			Colour.black=LCD_COLOR_TEST_BACK;

		
		if(SaveData.Limit_Tab.Comp_Value[i-6].low.Num!=0)//SaveData.Limit_Tab.Comp_Value[Button_Page.index-6].low
		{
			Hex_Format(SaveData.Limit_Tab.Comp_Value[i-6].low.Num ,
			SaveData.Limit_Tab.Comp_Value[i-6].low.Dot , 6 , 1);//�ӵ�λ
			//SaveData.Limit_Tab.Comp_Value[Button_Page.index-16].high
			WriteString_12(LIST2-130, 58+(i-6)*15, DispBuf,  1);//��ʾ���ֵ  ����Ҫ������ʾ��λ
		}
		else
		WriteString_12(LIST2-130, 58+(i-6)*15, "------",  1);	
	}
	Colour.black=LCD_COLOR_TEST_MID;
	if(SaveData.Limit_Tab.Comp_Value[9].high.Num!=0)
	{
		Hex_Format(SaveData.Limit_Tab.Comp_Value[9].high.Num, 
			SaveData.Limit_Tab.Comp_Value[9].high.Dot , 6 , 1);//�ӵ�λ//�ӵ�λ
		
		WriteString_12(LIST2, 196, DispBuf,  1);//��ʾ���ֵ  ����Ҫ������ʾ��λ
	}
	else
	WriteString_12(LIST2, 196, "------",  1);
	
	for(i=16;i<25;i++)
	{
		if((i-16>1&&i-16<7)||(i-16>8))
			Colour.black=LCD_COLOR_TEST_MID;
		else
			Colour.black=LCD_COLOR_TEST_BACK;
			
		if(SaveData.Limit_Tab.Comp_Value[i-16].high.Num!=0)
		{
			Hex_Format(SaveData.Limit_Tab.Comp_Value[i-16].high.Num, 
			SaveData.Limit_Tab.Comp_Value[i-16].high.Dot , 6 , 1);//�ӵ�λ
			
			WriteString_12(LIST2, 58+(i-16)*15, DispBuf,  1);//��ʾ���ֵ  ����Ҫ������ʾ��λ
		}
		else
			WriteString_12(LIST2, 58+(i-16)*15, "------",  1);
	}
	Colour.black=LCD_COLOR_TEST_MID;
	if(SaveData.Limit_Tab.Comp_Value[9].high.Num!=0)
		{
			Hex_Format(SaveData.Limit_Tab.Comp_Value[9].low.Num, 
			SaveData.Limit_Tab.Comp_Value[9].low.Dot , 6 , 1);//�ӵ�λ
		
		WriteString_12(LIST2-130, 196, DispBuf,  1);//��ʾ���ֵ  ����Ҫ������ʾ��λ
		}
		else
			WriteString_12(LIST2-130, 196, "------",  1);
	
	
	for(i=1;i<10;i++)
	{
		if(i>2&&i<8)
			Colour.black=LCD_COLOR_TEST_MID;
		else
			Colour.black=LCD_COLOR_TEST_BACK;
			
		WriteString_12(8,FIRSTLINE+17+15*i,Num_1[i-1],0);
		
	}
	Colour.black=LCD_COLOR_TEST_MID;
	WriteString_12(0, 196, RangeDisp_Second[SaveData.Main_Func.Param.test],  0);
//	WriteString_12(0,196,"2nd",0);
	WriteString_12(0,212,"AUX:",0);
	WriteString_12(248,212,"OUT:",0);
	
	
	
	Disp_Button_value1(0);


}
//�б���ʾ�Ӻ���
void Disp_List_Count_MidRect(void)
{

	LCD_DrawRect( 0, 175,479 , 160 , LCD_COLOR_TEST_MID ) ;
//	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
}

void Disp_List_CountScreen(void)
{
	Disp_TopBar_Color();
	Disp_Fastbutton();//��ʾ���ٰ���
	Disp_List_Count_MidRect();//��ʾ�м�ɫ��
}
//�б���ʾ����
void Disp_List_Count_Item(void)
{
	Disp_List_CountScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	WriteString_16(0, 4, All_TopName_E[3],  0);
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;

//	WriteString_16(LIST1, FIRSTLINE, "��ʽ :",  0);
//	WriteString_16(LIST1+40, FIRSTLINE+SPACE1, "Ƶ��[Hz]",  0);	
//	WriteString_16(LIST1+140, FIRSTLINE+SPACE1, "��ƽ[V]",  0);	
	if(SaveSIM.lang == 1)
	{
		WriteString_16(LIST1, FIRSTLINE, "Meth :",  0);
		WriteString_16(LIST1+40, FIRSTLINE+SPACE1, "Freq[Hz]",  0);	
		WriteString_16(LIST1+140, FIRSTLINE+SPACE1, "Levl[V]",  0);		
	}else if(SaveSIM.lang == 0){
		WriteString_16(LIST1, FIRSTLINE, "��ʽ :",  0);
		WriteString_16(LIST1+40, FIRSTLINE+SPACE1, "Ƶ��[Hz]",  0);	
		WriteString_16(LIST1+140, FIRSTLINE+SPACE1, "��ƽ[V]",  0);	
	}	
	
	WriteString_16(LIST1+140+100, FIRSTLINE+SPACE1, "Cp[F]",  0);
	WriteString_16(LIST1+140+100+100, FIRSTLINE+SPACE1, "D[ ]",  0);
	WriteString_16(LIST1+440, FIRSTLINE+SPACE1, "CMP",  0);
	Disp_Button_value1(0);

}

//�б���ʾ�Ӻ���
void Disp_TestSet_MidRect(void)
{

	LCD_DrawRect( 0, 90,479 , 154 , LCD_COLOR_TEST_MID ) ;
//	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
}

void Disp_TestSetScreen(void)
{
	LCD_DrawRect( 0, 0, 168, 22 , LCD_COLOR_TEST_BAR);
//	Disp_TopBar_Color();
	Disp_Fastbutton();//��ʾ���ٰ���
	Disp_TestSet_MidRect();//��ʾ�м�ɫ��
}

static  void  Dis_Dot(void)
{
   uint8_t  i;
	
	for(i=0;i<10;i++)
	{
		if(i<5)
		{
			if(i==3||i==4)		Colour.black=LCD_COLOR_TEST_MID;
			else                    Colour.black=LCD_COLOR_TEST_BACK;
			WriteString_16(LIST1+72, FIRSTLINE+i*SPACE1+1, ":",  0);
		}
		else 
		{
			if(i==8||i==9)	  Colour.black=LCD_COLOR_TEST_MID;
			else 					  Colour.black=LCD_COLOR_TEST_BACK;
			WriteString_16(LIST2+72, FIRSTLINE+(i-5)*SPACE1+1, ":",  0);
		}
	}

}



//�б���ʾ����	   //  �������ý�����ʾ�˵�����
void Disp_Test_Set_Item(void)
{
	uint32_t i;
	Disp_TestSetScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveSIM.lang ==1)
	{
		WriteString_16(0, 4, All_TopName_E[4],  0);
	}else if(SaveSIM.lang ==0){
		WriteString_16(0, 4, All_TopName[4],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	for(i=0;i<(sizeof(Set_testitem_E)/(sizeof(Set_testitem_E[0])));i++)
	if(SaveSIM.lang == 1)
	{
		if(i<sizeof(Set_testitem_E)/(sizeof(Set_testitem_E[0]))/2)
		{
			if(i>2&&i<5)
				Colour.black=LCD_COLOR_TEST_MID;
			
			else
				Colour.black=LCD_COLOR_TEST_BACK;	
			
			WriteString_16(LIST1, FIRSTLINE+SPACE1*i, Set_testitem_E[i],  0);

		}
		else
		{
			if((i-sizeof(Set_testitem_E)/(sizeof(Set_testitem_E[0]))/2)>2&&(i-sizeof(Set_testitem_E)/(sizeof(Set_testitem_E[0]))/2)<5)
				Colour.black=LCD_COLOR_TEST_MID;
			else
				Colour.black=LCD_COLOR_TEST_BACK;

			WriteString_16(LIST2, FIRSTLINE+SPACE1*(i-sizeof(Set_testitem_E)/(sizeof(Set_testitem_E[0]))/2), Set_testitem_E[i],  0);
			
			
		}		
	}else if(SaveSIM.lang == 0){
		if(i<sizeof(Set_testitem)/(sizeof(Set_testitem[0]))/2)
		{
			if(i>2&&i<5)
				Colour.black=LCD_COLOR_TEST_MID;
			else
				Colour.black=LCD_COLOR_TEST_BACK;	
			
			WriteString_16(LIST1, FIRSTLINE+SPACE1*i, Set_testitem[i],  0);

		}
		else
		{
			if((i-sizeof(Set_testitem)/(sizeof(Set_testitem[0]))/2)>2&&(i-sizeof(Set_testitem)/(sizeof(Set_testitem[0]))/2)<5)
				Colour.black=LCD_COLOR_TEST_MID;
			else
				Colour.black=LCD_COLOR_TEST_BACK;

			WriteString_16(LIST2, FIRSTLINE+SPACE1*(i-sizeof(Set_testitem)/(sizeof(Set_testitem[0]))/2), Set_testitem[i],  0);
			
			
		}	
	}	
	
//	Dis_Dot();
	Disp_Button_TestSet(0);

}
//����ʱ������ʾ�������ʾ
void Disp_RangeDispValue(Button_Page_Typedef *Button_Page)
{
//	uint32_t flag;
	uint32_t i;
//	uint32_t xpose;
	uint32_t Black_Select;
//	uint32_t Select_color;

//����	
	
	Colour.black=LCD_COLOR_TEST_BACK;	
	WriteString_16(LIST1+73, FIRSTLINE, User_FUNC[SaveData.Main_Func.Param.test],  1);//�����㷨  ��˳��Ĺ���
	
//Ƶ��	
	WriteString_16(LIST1+73, FIRSTLINE+SPACE1, User_Freq[SaveData.Main_Func.Freq],  1);
	
	
//��ƽ	

	WriteString_16(LIST1+73, FIRSTLINE+SPACE1*2, User_Level[SaveData.Main_Func.Level],  1);
//����

//	if(SaveData.Main_Func.Range.Auto)
//		WriteString_16(LIST2+88, FIRSTLINE, User_Range[0],  0);
//	else
//	{
		WriteString_16(LIST2+73, FIRSTLINE, User_Range[SaveData.Main_Func.Range.Range],  0);
	
//	}
	
//�ٶ�
	
	WriteString_16(LIST2+73, FIRSTLINE+SPACE1, User_Speed[SaveData.Main_Func.Speed],  1);
	
//ƫ��

//	WriteString_16(LIST2+73, FIRSTLINE+SPACE1, User_Comp[SaveData.Main_Func.Bias],  1);
	Black_Select=(Button_Page->index==1)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST2+72, FIRSTLINE+SPACE1*2-2,SELECT_2END-50 , FIRSTLINE+SPACE1*3-4 , Colour.black ) ;//SPACE1//SPACE1
	WriteString_16(LIST2+73, FIRSTLINE+SPACE1*2, User_Comp[SaveData.Limit_Tab.Comp],  1);
	Disp_Fastbutton();
	if(Button_Page->index==1)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		for(i=0;i<2;i++)
		 {
			 if(i==0)
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+20, BUTTOM_Y_VALUE, User_Comp[i],  0);
			 else
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+25, BUTTOM_Y_VALUE, User_Comp[i],  0); 
		 }
	}
	else
	
	Disp_Button_value1(0);
	

}
//����ʱ������ʾ�������ʾ        ��������ʾON �� OFF
void Disp_Range_ComDispValue(Button_Page_Typedef *Button_Page)
{
//	uint32_t flag;
	uint32_t i;
//	uint32_t xpose;
	uint32_t Black_Select;
//	uint32_t Select_color;

	Colour.black=LCD_COLOR_TEST_BACK;	
	
	Black_Select=(Button_Page->index==1)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}	
	LCD_DrawRect( 320+99, 26-2,320+140 , 26+16-2 , Colour.black ) ;//SPACE1//SPACE1
	WriteString_16(320+100, 26-2, User_Comp[SaveData.Limit_Tab.count],  1);
	Disp_Fastbutton();
	if(Button_Page->index==1)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		for(i=0;i<2;i++)
		  {
			  if(i==0)
				  WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+20, BUTTOM_Y_VALUE, User_Comp[i],  0);
			  else
				  WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+25, BUTTOM_Y_VALUE, User_Comp[i],  0); 
		  }
	}
	else
	
	Disp_Button_value1(0);
	

}

//����ʱ����ʾ�б�ɨ��Ĳ���ֵ��Ҳ���ǴӴ��ڽ��չ�����ֵ
void Disp_Scan_Compvalue(uint8_t set)
{
	uint8_t i;
//	uint32_t  Black_Select;
//	static uint32_t pos[2];
	
//	Black_Select=(SaveData.Limit_ScanValue.xpos)?1:0;//count
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
//	pos[0]
//	if(set)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//		LCD_DrawRect( 48, FIRSTLINE,88 , FIRSTLINE+16 , Colour.black ) ;
//	}
//	Colour.black=LCD_COLOR_TEST_BACK;
//	if(pos[0]!=SaveData.Limit_ScanValue.xpos||pos[1]!=SaveData.Limit_ScanValue.ypos)
//	{
//		LCD_DrawRect( 48, FIRSTLINE,88 , FIRSTLINE+16 , Colour.black ) ;
//	
//	}
	for(i=1;i<11;i++)
	{
		
		if(i>2&&i<7)
			Colour.black=LCD_COLOR_TEST_MID;
		else
			Colour.black=LCD_COLOR_TEST_BACK;
		
		if(SaveData.Limit_ScanValue.Range_Set_main[SaveData.Limit_ScanValue.num*10+(i-1)].Num==0xffffffff)
			WriteString_12(8+30+108+100,FIRSTLINE+SPACE1+3+ (i)*16, "----",  0);
		else
		{
			Hex_Format(SaveData.Limit_ScanValue.Range_Set_main[SaveData.Limit_ScanValue.num*10+(i-1)].Num ,
			 SaveData.Limit_ScanValue.Range_Set_main[SaveData.Limit_ScanValue.num*10+(i-1)].Dot , 5 , 1);
				WriteString_12(8+30+108+100,FIRSTLINE+SPACE1+3+ (i)*16, DispBuf,  0);
			
		}
		if(SaveData.Limit_ScanValue.Range_Sed_Second[SaveData.Limit_ScanValue.num*10+(i-1)].Num==0xffffffff)
			WriteString_12(8+30+108+100+100,FIRSTLINE+SPACE1+3+ (i)*16, "----",  0);
		else
		{
			
			Hex_Format(SaveData.Limit_ScanValue.Range_Sed_Second[SaveData.Limit_ScanValue.num*10+(i-1)].Num ,
				SaveData.Limit_ScanValue.Range_Sed_Second[SaveData.Limit_ScanValue.num*10+(i-1)].Dot , 5 , 1);
			WriteString_12(8+30+108+100+100,FIRSTLINE+SPACE1+3+ (i)*16, DispBuf,  0);
		
		}
		if(SaveData.Limit_ScanValue.comp[SaveData.Limit_ScanValue.num*10+(i-1)]==0xff)
			WriteString_12(8+30+108+100+100+100,FIRSTLINE+SPACE1+3+ (i)*16, "-",  0);
		else
		{
			
			if(SaveData.Limit_ScanValue.comp[SaveData.Limit_ScanValue.num*10+(i-1)])
				WriteString_12(8+30+108+100+100+100,FIRSTLINE+SPACE1+3+ (i)*16, "P",  0);
			else
				WriteString_12(8+30+108+100+100+100,FIRSTLINE+SPACE1+3+ (i)*16, "F",  0);
		
		}
		//for(i=1;i<11;i++)
		{
			
			if(i>2&&i<7)
				Colour.black=LCD_COLOR_TEST_MID;
			else
				Colour.black=LCD_COLOR_TEST_BACK;	
			if(SaveData.Limit_ScanValue.freq[SaveData.Limit_ScanValue.num*10+(i-1)]>30)
				WriteString_12(8+30,FIRSTLINE+SPACE1+3+ (i)*16, "----",  0);
			else
				{
					WriteString_12(8+30,FIRSTLINE+SPACE1+3+ (i)*16, User_Freq[SaveData.Limit_ScanValue.freq[SaveData.Limit_ScanValue.num*10+(i-1)]],  0);
				
				}
			if(SaveData.Limit_ScanValue.ac_leave[SaveData.Limit_ScanValue.num*10+(i-1)]>2)
				WriteString_12(8+30+108,FIRSTLINE+SPACE1+3+ (i)*16, "----",  0);
			else
				{
					WriteString_12(8+30+108,FIRSTLINE+SPACE1+3+ (i)*16, User_Level[SaveData.Limit_ScanValue.ac_leave[SaveData.Limit_ScanValue.num*10+(i-1)]],  0);
				
				}
				
			Hex_Format(SaveData.Limit_ScanValue.num*10+i , 0 , 3 , 1);
			
			WriteString_12(8,FIRSTLINE+SPACE1+3+ (i)*16, DispBuf,  0);
		}
				
		
	}

}
//const List_Scan[][6][2]=
//{
//	{{},{},{},{},{}},
//	{{},{},{},{},{}},
//	{{},{},{},{},{}},
//	{{},{},{},{},{}},
//	{{},{},{},{},{}},
//	{{},{},{},{},{}},
//	{},
//	{},
//	{},
//	{}


//};

//void Disp_List_Select(Button_Page_Typedef* Button_Page,uint8_t i)
//{
//	uint8_t Black_Select;
////	uint8_t i;
////	for(i=0;i<10;i++)
//	{
//		Black_Select=(Button_Page->index==i)?1:0;
//		if(Black_Select)
//		{
//			Colour.black=LCD_COLOR_SELECT;
//		
//		}
//		else
//		{
//			Colour.black=LCD_COLOR_TEST_BACK;
//		}
//		//if()	
//		LCD_DrawRect( 32+Button_Page->page*60, FIRSTLINE+SPACE1+3+16*(Button_Page->index-1),88+Button_Page->page*60 ,
//			FIRSTLINE+SPACE1+3+16*Button_Page->index+16 , Colour.black ) ;
//		//LCD_DrawRect( LIST1+88, FIRSTLINE-2,SELECT_1END , FIRSTLINE+SPACE1-4 , Colour.black ) ;//SPACE1
////		WriteString_16(LIST1+88, FIRSTLINE, User_FUNC[SaveData.Main_Func.Param.test],  1);//�����㷨  ��˳��Ĺ���
//	}
//}
//void Disp_List_Select(Button_Page_Typedef* Button_Page)
//{
//	static uint32_t pos[2];
//	if(Button_Page->index==0&&Button_Page->page==0)
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	
//	}
//	else
//		if(Button_Page->index==1&&Button_Page->page==0)//
//		{
//			Colour.black=LCD_COLOR_SELECT;
//			LCD_DrawRect( 48, FIRSTLINE,88 , FIRSTLINE+16 , Colour.black ) ;
//			Colour.black=LCD_COLOR_TEST_BACK;
////			pos[0]=Button_Page->index;
////			pos[1]=Button_Page->page;
//		
//		}
//		else
//		{
//			Colour.black=LCD_COLOR_SELECT;
//			LCD_DrawRect( 32+Button_Page->page*60, FIRSTLINE+SPACE1+3+16*(Button_Page->index-1),88+Button_Page->page*60 ,
//			FIRSTLINE+SPACE1+3+16*Button_Page->index+16 , Colour.black ) ;
//			
//			Colour.black=LCD_COLOR_TEST_BACK;
////			if(pos[0]==1&&pos[1]==0)
////			{
////				LCD_DrawRect( 48, FIRSTLINE,88 , FIRSTLINE+16 , Colour.black ) ;
////			}
////			else
////			{
////			
////				LCD_DrawRect( 32+pos[1]*60, FIRSTLINE+SPACE1+3+16*pos[0],88+pos[1]*60 ,
////			FIRSTLINE+SPACE1+3+16*pos[0]+16 , Colour.black ) ;
////			}
////			pos[0]=Button_Page->index;
////			pos[1]=Button_Page->page;
//		
//		}



//}
//����ʱ����ʾ�б�ɨ��Ĳ���ֵ��Ҳ���ǴӴ��ڽ��չ�����ֵ
void Disp_Scan_SetCompvalue(Button_Page_Typedef* Button_Page)
{
	uint8_t i;
	if(Button_Page->index==2)
		Disp_Button_ItemScan_no();
	else
		if(Button_Page->index>2)
		Disp_Button_ItemScanSet(Button_Page->third);
	if(Button_Page->index==1&&Button_Page->third==0)//��һ��ֵ
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	LCD_DrawRect( 54, FIRSTLINE,54+40 , FIRSTLINE+16 , Colour.black ) ;	
	WriteString_16(54,FIRSTLINE, User_FUNC[SaveData.Sys_Setup.Main_Func],  0);
	
	if(Button_Page->index==2&&Button_Page->third==0)//�ڶ���ֵ
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	LCD_DrawRect( 0, FIRSTLINE+24,30 , FIRSTLINE+24+16 , Colour.black ) ;	
	WriteString_16(0,FIRSTLINE+24, "No.",  0);
	
	for(i=3;i<13;i++)
	{
		
		if(Button_Page->index==i&&Button_Page->third==2)//������
			{
				Colour.black=LCD_COLOR_SELECT;
			
			}
			else
			{
				Colour.black=LCD_COLOR_TEST_BACK;
			}
			LCD_DrawRect( 250, FIRSTLINE+SPACE1+3+ (i-2)*16,250+56 ,
			FIRSTLINE+SPACE1+3+ (i-2)*16+16 , Colour.black ) ;	
		if(SaveData.Limit_ScanValue.Range_Set_main[Button_Page->force*10+(i-3)].Num==0xffffffff)
			WriteString_12(250,FIRSTLINE+SPACE1+3+ (i-2)*16, "----",  0);
		else
		{
			Hex_Format(SaveData.Limit_ScanValue.Range_Set_main[Button_Page->force*10+(i-3)].Num ,
			 SaveData.Limit_ScanValue.Range_Set_main[Button_Page->force*10+(i-3)].Dot , 5 , 1);
				WriteString_12(250,FIRSTLINE+SPACE1+3+ (i-2)*16, DispBuf,  0);
			
		}
		if(Button_Page->index==i&&Button_Page->third==3)//������
			{
				Colour.black=LCD_COLOR_SELECT;
			
			}
			else
			{
				Colour.black=LCD_COLOR_TEST_BACK;
			}
			LCD_DrawRect( 250+80, FIRSTLINE+SPACE1+3+ (i-2)*16,250+80+56 ,
			FIRSTLINE+SPACE1+3+ (i-2)*16+16 , Colour.black ) ;	
		if(SaveData.Limit_ScanValue.Range_Sed_Second[Button_Page->force*10+(i-3)].Num==0xffffffff)
			WriteString_12(250+80,FIRSTLINE+SPACE1+3+ (i-2)*16, "----",  0);
		else
		{
			
			Hex_Format(SaveData.Limit_ScanValue.Range_Sed_Second[Button_Page->force*10+(i-3)].Num ,
				SaveData.Limit_ScanValue.Range_Sed_Second[Button_Page->force*10+(i-3)].Dot , 5 , 1);
			WriteString_12(250+80,FIRSTLINE+SPACE1+3+ (i-2)*16, DispBuf,  0);
		
		}
		if(Button_Page->index==i&&Button_Page->third==4)//������
			{
				Colour.black=LCD_COLOR_SELECT;
			
			}
			else
			{
				Colour.black=LCD_COLOR_TEST_BACK;
			}
			LCD_DrawRect( 420, FIRSTLINE+SPACE1+3+ (i-2)*16,479 ,
			FIRSTLINE+SPACE1+3+ (i-2)*16+16 , Colour.black ) ;	
		if(SaveData.Limit_ScanValue.comp[Button_Page->force*10+(i-3)]==0xff)
			WriteString_12(420,FIRSTLINE+SPACE1+3+ (i-2)*16, "----",  0);
		else
		{
			
			//if(SaveData.Limit_ScanValue.comp[SaveData.Limit_ScanValue.num*10+(i-2)])
				WriteString_12(420,FIRSTLINE+SPACE1+3+ (i-2)*16, "P",  0);
			//else
				//WriteString_12(8+30+108+100+100+100,FIRSTLINE+SPACE1+3+ (i-1)*16, "F",  0);
		
		}
		{
			
			if(Button_Page->index==i&&Button_Page->third==0)//��һ��
			{
				Colour.black=LCD_COLOR_SELECT;
			
			}
			else
			{
				Colour.black=LCD_COLOR_TEST_BACK;
			}
			LCD_DrawRect(40, FIRSTLINE+SPACE1+3+ (i-2)*16,126 , 
			FIRSTLINE+SPACE1+3+ (i-2)*16+16 , Colour.black ) ;
			if(SaveData.Limit_ScanValue.freq[Button_Page->force*10+(i-3)]>0x30)
				WriteString_12(40,FIRSTLINE+SPACE1+3+ (i-2)*16, "----",  0);
			else
				{
					WriteString_12(40,FIRSTLINE+SPACE1+3+ (i-2)*16, 
					User_Freq[SaveData.Limit_ScanValue.freq[Button_Page->force*10+(i-3)]],  0);
				
				}
			if(Button_Page->index==i&&Button_Page->third==1)//�ڶ���
			{
				Colour.black=LCD_COLOR_SELECT;
			
			}
			else
			{
				Colour.black=LCD_COLOR_TEST_BACK;
			}
			LCD_DrawRect( 38+88, FIRSTLINE+SPACE1+3+ (i-2)*16,38+88+70 , 
			FIRSTLINE+SPACE1+3+ (i-2)*16+16 , Colour.black ) ;	
			if(SaveData.Limit_ScanValue.ac_leave[Button_Page->force*10+(i-3)]>2)
				WriteString_12(8+30+88,FIRSTLINE+SPACE1+3+ (i-2)*16, "----",  0);
			else
				{
					WriteString_12(8+30+88,FIRSTLINE+SPACE1+3+ (i-2)*16, 
					User_Level[SaveData.Limit_ScanValue.ac_leave[Button_Page->force*10+(i-3)]],  0);
				
				}
			Colour.black=LCD_COLOR_TEST_BACK;	
			Colour.black=LCD_COLOR_TEST_BACK;	
			Hex_Format(Button_Page->force*10+i-2 , 0 , 3 , 1);
			
			WriteString_12(8,FIRSTLINE+SPACE1+3+ (i-2)*16, DispBuf,  0);
		}
				
		
	}
	



}
//����ʱ���б�ɨ����ʾֵ
void Disp_LIMIT_ComDispValue(Button_Page_Typedef *Button_Page)
{
//	uint32_t flag;
	uint32_t i;
//	uint32_t xpose;
	uint32_t Black_Select;
//	uint32_t Select_color;

	Colour.black=LCD_COLOR_TEST_BACK;	
	
	Black_Select=(Button_Page->index==1)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}	
	LCD_DrawRect( 48, FIRSTLINE,88 , FIRSTLINE+16 , Colour.black ) ;//SPACE1//SPACE1
	WriteString_16(48, FIRSTLINE, Limit_Scan_Fun[SaveData.Limit_ScanValue.fun],  1);
	
	Black_Select=(Button_Page->index==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}	
	LCD_DrawRect( 8, FIRSTLINE+SPACE1,30 , FIRSTLINE+SPACE1+16 , Colour.black ) ;//SPACE1//SPACE1
	WriteString_16(8, FIRSTLINE+SPACE1, "No.",  1);
	Disp_Fastbutton();
	if(Button_Page->index==1)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		for(i=0;i<2;i++)
			WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Limit_Scan_Fun[i],  0);
	}
	else if(Button_Page->index==2)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		for(i=0;i<2;i++)
			WriteString_16(BUTTOM_X_VALUE+(i+3)*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Limit_Scan_Page[i],  0);
	}else
		Disp_Button_value1(Button_Page->page);
	

}
//���Ե�ʱ����ʾ������ֵ     // ������ʾ�е�����ֵ
void Disp_Test_value(Button_Page_Typedef* Button_Page)
{
//	uint32_t flag;
	uint32_t i;
//	uint32_t xpose;
	uint32_t Black_Select;
//	uint32_t Select_color;
	if(SaveData.Main_Func.V_i==1) 
	{
		Colour.black=LCD_COLOR_TEST_BACK;
		WriteString_16(30,210, User_Comp[1],  1);
		WriteString_16(30+120, 210, User_Comp[1],  1);
		
	}
//��ѹ����	
	Black_Select=(Button_Page->index==1)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	LCD_DrawRect( LIST1+87+16,FIRSTLINE-2,SELECT_1END ,FIRSTLINE+SPACE1-4 , Colour.black ) ;//SPACE1
	if(Black_Select && coder_flag == 1)
	{
		Colour.black=LCD_COLOR_YELLOW;
		if(spinbit < 3)
		{
			LCD_DrawRect(LIST1+88+16+50-spinbit*10, FIRSTLINE+16,LIST1+88+16+50-spinbit*10+8, FIRSTLINE+16+2,Colour.black);
		}else{
			LCD_DrawRect(LIST1+88+16+40-spinbit*10, FIRSTLINE+16,LIST1+88+16+40-spinbit*10+8, FIRSTLINE+16+2,Colour.black);
		}
		Colour.black=LCD_COLOR_SELECT;	
	}	
	if(coder_flag == 1)
	{
		Hex_Format(SaveSIM.Voltage.Num,3,5,1);
	}else{
		Hex_Format(SaveSIM.Voltage.Num,3,5,0);
	}
	WriteString_16(LIST1+88+16,FIRSTLINE, DispBuf,  0);//�����㷨  ��˳��Ĺ���
	WriteString_16(LIST1+88+16+70, FIRSTLINE, "V",  1);
//	if(DispBuf[0] == 0x20)
//	{
//		spinbitmax = 3;
//	}else{
		spinbitmax = 4;
//	}
//	WriteString_16(LIST2-88, SCREENHIGH-FIRSTLINE, User_Range[SaveData.Main_Func.Range.Range],  0);
//��籣������	
	Black_Select=(Button_Page->index==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	Hex_Format(SaveSIM.ChargePC.Num,3,5,0);	
	LCD_DrawRect(LIST1+87+16, FIRSTLINE+SPACE1-2,SELECT_1END ,FIRSTLINE+SPACE1*2-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+16,FIRSTLINE+SPACE1, DispBuf,  0);//�����㷨  ��˳��Ĺ���
	WriteString_16(LIST1+88+16+70, FIRSTLINE+SPACE1, "A",  1);
	
	
//�ŵ籣������	
	Black_Select=(Button_Page->index==3)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	Hex_Format(SaveSIM.LoadPC.Num,3,5,0);		
	LCD_DrawRect(LIST1+87+16, FIRSTLINE+SPACE1*2-2,SELECT_1END ,FIRSTLINE+SPACE1*3 -4, Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+16,FIRSTLINE+SPACE1*2, DispBuf,  0);//�����㷨  ��˳��Ĺ���
	WriteString_16(LIST1+88+16+70, FIRSTLINE+SPACE1*2, "A",  1);
//��ݵ�ѹ
	Black_Select=(Button_Page->index==4)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	Hex_Format(SaveSIM.QuickV[SaveSIM.qvflag].Num,3,5,0);
	LCD_DrawRect( LIST2+87+32, FIRSTLINE-2,SELECT_2END+8 , FIRSTLINE+SPACE1*1-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST2+88+32,FIRSTLINE, DispBuf,  0);//�����㷨  ��˳��Ĺ���
	WriteString_16(LIST2+88+32+70, FIRSTLINE, "V",  1);
	
//��籣��ʱ��
	Black_Select=(Button_Page->index==5)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	Hex_Format(SaveSIM.ChargePT.Num,0,3,0);	
	LCD_DrawRect( LIST2+87+32, FIRSTLINE+SPACE1-2,SELECT_2END+9,FIRSTLINE+SPACE1*2-4, Colour.black ) ;//SPACE1
	WriteString_16(LIST2+88+32,FIRSTLINE+SPACE1, DispBuf,  0);//�����㷨  ��˳��Ĺ���
	WriteString_16(LIST2+88+32+60, FIRSTLINE+SPACE1, "ms",  1);
//	WriteString_16(LIST2+88+32,FIRSTLINE+SPACE1, User_Speed[SaveData.Main_Func.Speed],  1);
	
//�ŵ籣��ʱ��
	Black_Select=(Button_Page->index==6)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	Hex_Format(SaveSIM.LoadPT.Num,0,3,0);		
	LCD_DrawRect( LIST2+87+32, FIRSTLINE+SPACE1*2-2,SELECT_2END+9,FIRSTLINE+SPACE1*3-4, Colour.black ) ;//SPACE1
	WriteString_16(LIST2+88+32,FIRSTLINE+SPACE1*2, DispBuf,  0);//�����㷨  ��˳��Ĺ���
	WriteString_16(LIST2+88+32+60, FIRSTLINE+SPACE1*2, "ms",  1);
//	WriteString_16(LIST2+88+32,FIRSTLINE+SPACE1*2, User_Speed[SaveData.Main_Func.Speed],  1);
//ƫ��
//	if(index==12)
//		index=13;
//	Black_Select=(Button_Page->index==6)?1:0;
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
//		
//	LCD_DrawRect( LIST2+87,FIRSTLINE+SPACE1*2-2,SELECT_2END ,FIRSTLINE+SPACE1*3-4 , Colour.black ) ;//SPACE1
//	WriteString_16(LIST2+88,FIRSTLINE+SPACE1*2, User_Comp[1],  1);//SaveData.Main_Func.Bias
	Disp_Fastbutton();
	
	switch(Button_Page->index)
	{
		case 0:
//			Disp_Button_Test();
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			if(Button_Page->page == 0)
			{
				for(i=0;i<3;i++)
				{
					Hex_Format(SaveSIM.QuickV[i+(Button_Page->page)*3].Num,3,5,0);
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,DispBuf,  0);
				}
				WriteString_16(BUTTOM_X_VALUE+3*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,"NEXT",  0);
			}else if(Button_Page->page == 1){
				for(i=0;i<3;i++)
				{
					Hex_Format(SaveSIM.QuickV[i+(Button_Page->page)*3].Num,3,5,0);
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,DispBuf,  0);
				}
				WriteString_16(BUTTOM_X_VALUE+3*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,"PREV",  0);
			}
			//	Disp_Button_value1(Button_Page->page);
			break;
		case 1:				//��ʾ������İ���ֵ
//			Colour.Fword=White;
//			Colour.black=LCD_COLOR_TEST_BUTON;
//			WriteString_16(BUTTOM_X_VALUE+4, BUTTOM_Y_VALUE,Set_V_Res[SaveSIM.resflag],  0);
//			for(i=1;i<3;i++)
//			{
//				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,Tune_V[i-1],  0);
//			}
		break;
		case 2:

		break;
		case 3:
			break;

		case 4:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			if(Button_Page->page == 0)
			{
				for(i=0;i<3;i++)
				{
					Hex_Format(SaveSIM.QuickV[i+(Button_Page->page)*3].Num,3,5,0);
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,DispBuf,  0);
				}
				WriteString_16(BUTTOM_X_VALUE+3*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,"NEXT",  0);
			}else if(Button_Page->page == 1){
				for(i=0;i<3;i++)
				{
					Hex_Format(SaveSIM.QuickV[i+(Button_Page->page)*3].Num,3,5,0);
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,DispBuf,  0);
				}
				WriteString_16(BUTTOM_X_VALUE+3*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,"PREV",  0);
			}
			

			break;
		case 5:

			break;
//		case 6:
//			Colour.Fword=White;
//			Colour.black=LCD_COLOR_TEST_BUTON;
//			for(i=0;i<2;i++)
//			{
//				if(i==0)
//					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+20, BUTTOM_Y_VALUE, User_Comp[i],  0);
//				else 
//					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+25, BUTTOM_Y_VALUE, User_Comp[i],  0);
//			}
//			break;
		default:
			
		break;
			
	}

}
//��ʾ���ò�����ֵSetup_Valueall
void DispSet_value(Button_Page_Typedef* Button_Page)
{
	uint32_t i;
	uint32_t Black_Select;

//��ݵ�ѹ1	
	Black_Select=(Button_Page->index==1)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	Hex_Format(SaveSIM.QuickV[0].Num,3,5,0);
	LCD_DrawRect( LIST1+87+16,FIRSTLINE-2,SELECT_1END ,FIRSTLINE+SPACE1-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+16,FIRSTLINE, DispBuf,  0);//�����㷨  ��˳��Ĺ���
	WriteString_16(LIST1+88+16+70, FIRSTLINE, "V",  1);

//��ݵ�ѹ2	
	Black_Select=(Button_Page->index==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	Hex_Format(SaveSIM.QuickV[1].Num,3,5,0);	
	LCD_DrawRect(LIST1+87+16, FIRSTLINE+SPACE1-2,SELECT_1END ,FIRSTLINE+SPACE1*2-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+16,FIRSTLINE+SPACE1, DispBuf,  0);//�����㷨  ��˳��Ĺ���
	WriteString_16(LIST1+88+16+70, FIRSTLINE+SPACE1, "V",  1);
	
	
//��ݵ�ѹ3	
	Black_Select=(Button_Page->index==3)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	Hex_Format(SaveSIM.QuickV[2].Num,3,5,0);		
	LCD_DrawRect(LIST1+87+16, FIRSTLINE+SPACE1*2-2,SELECT_1END ,FIRSTLINE+SPACE1*3 -4, Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+16,FIRSTLINE+SPACE1*2, DispBuf,  0);//�����㷨  ��˳��Ĺ���
	WriteString_16(LIST1+88+16+70, FIRSTLINE+SPACE1*2, "V",  1);
//��ݵ�ѹ4
	Black_Select=(Button_Page->index==4)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	Hex_Format(SaveSIM.QuickV[3].Num,3,5,0);	
	LCD_DrawRect( LIST2+87+16, FIRSTLINE-2,SELECT_2END+8 ,FIRSTLINE+SPACE1-4, Colour.black ) ;//SPACE1
	WriteString_16(LIST2+88+16,FIRSTLINE, DispBuf,  0);//�����㷨  ��˳��Ĺ���
	WriteString_16(LIST2+88+16+70, FIRSTLINE, "V",  1);
	
//��ݵ�ѹ5
	Black_Select=(Button_Page->index==5)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	Hex_Format(SaveSIM.QuickV[4].Num,3,5,0);	
	LCD_DrawRect( LIST2+87+16, FIRSTLINE+SPACE1-2,SELECT_2END+8 ,FIRSTLINE+SPACE1*2-4, Colour.black ) ;//SPACE1
	WriteString_16(LIST2+88+16,FIRSTLINE+SPACE1, DispBuf,  0);//�����㷨  ��˳��Ĺ���
	WriteString_16(LIST2+88+16+70, FIRSTLINE+SPACE1, "V",  1);
//	WriteString_16(LIST2+88+32,FIRSTLINE+SPACE1, User_Speed[SaveData.Main_Func.Speed],  1);
	
//��ݵ�ѹ6
	Black_Select=(Button_Page->index==6)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	Hex_Format(SaveSIM.QuickV[5].Num,3,5,0);		
	LCD_DrawRect( LIST2+87+16, FIRSTLINE+SPACE1*2-2,SELECT_2END+8 ,FIRSTLINE+SPACE1*3-4, Colour.black ) ;//SPACE1
	WriteString_16(LIST2+88+16,FIRSTLINE+SPACE1*2, DispBuf,  0);//�����㷨  ��˳��Ĺ���
	WriteString_16(LIST2+88+16+70, FIRSTLINE+SPACE1*2, "V",  1);
	

	Disp_Fastbutton();
	switch(Button_Page->index)
	{
		case 0:
				Disp_Button_TestSet(Button_Page->page);
			break;
		
	}
}




//�������ð���ֵ
void Disp_LimitSEt_value(Button_Page_Typedef* Button_Page)
{
//	uint32_t flag;
	uint32_t i;
//	uint32_t xpose;
	uint32_t Black_Select;
//	uint32_t Select_color;
//	if(SaveData.Main_Func.V_i==1)
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//		WriteString_16(30, 210, User_Comp[1],  1);
//		WriteString_16(30+120, 210, User_Comp[1],  1);
//		
//	}
//	
	Colour.black=LCD_COLOR_TEST_BACK;

	if(SaveData.Limit_Tab.Mode==0)//������
	{
		Disp_Set_Unit_12(SaveData.Main_Func.Param.test,330+40,65-2);
		Disp_Set_Unit_12(SaveData.Main_Func.Param.test,160+40,65-2);
		
	}
	else
	{
		WriteString_12(330+40, 65-2, "%",  0);	
		WriteString_12(160+40, 65-2, "%",  0);
		
	}
	Black_Select=(Button_Page->index==1)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+69, FIRSTLINE-2,SELECT_1END-39 , FIRSTLINE+SPACE1-6 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+70, FIRSTLINE-2, User_FUNC[SaveData.Main_Func.Param.test],  1);//�����㷨  ��˳��Ĺ���
	
//���
	Black_Select=(Button_Page->index==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	//WriteString_16(LIST1+60, FIRSTLINE, User_Freq[SaveData.Main_Func.Freq],  1);//��ʾ����
	Hex_Format(SaveData.Limit_Tab.Nom.Num , SaveData.Limit_Tab.Nom.Dot , 5 , 0);//�ӵ�λ
	LCD_DrawRect( LIST2-24, FIRSTLINE-2,SELECT_2END-123+6 , FIRSTLINE+16, Colour.black ) ;
	WriteString_16(LIST2-21, FIRSTLINE-2, DispBuf,  1);//��ʾ���ֵ  ����Ҫ������ʾ��λ
	WriteString_16(LIST2+47, FIRSTLINE-2, Disp_Unit[SaveData.Limit_Tab.Nom.Unit],  1);
	Disp_Set_Unit_16(SaveData.Main_Func.Param.test,SELECT_2END-135,FIRSTLINE-2 );
	
	
//��ʽ
	Black_Select=(Button_Page->index==3)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST2+149, FIRSTLINE-2,SELECT_2END+35 , FIRSTLINE+SPACE1*1 -6, Colour.black ) ;//SPACE1
	WriteString_16(LIST2+150, FIRSTLINE-2, User_LimitList_Value[SaveData.Limit_Tab.Mode],  1);
//����
	Black_Select=(Button_Page->index==4)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+69, FIRSTLINE+SPACE1-4,SELECT_1END-39 , FIRSTLINE+SPACE1*2-8 , Colour.black ) ;//SPACE1
//	if(SaveData.Main_Func.Range.Auto)
		WriteString_16(LIST1+70, FIRSTLINE+SPACE1-4, Switch_Value[SaveData.Limit_Tab.Param],  0);
//	else
//	{
//		WriteString_16(LIST1+88, FIRSTLINE, User_Range[SaveData.Main_Func.Range.Range+1],  0);
//	
//	}
	
//�Ƚ�
	Black_Select=(Button_Page->index==5)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST2-22, FIRSTLINE+SPACE1-4,SELECT_2END-130 , FIRSTLINE+SPACE1*2-8 , Colour.black ) ;//SPACE1
	WriteString_16(LIST2-21, FIRSTLINE+SPACE1-4, Switch_Value[SaveData.Limit_Tab.Comp],  1);
	for(i=6;i<15;i++)
	{
		//SaveData.Limit_Tab.Comp_Value[Button_Page.index-6].low
		if(i==Button_Page->index)
			Colour.black=LCD_COLOR_SELECT;
		else
			if(i-6>2&&i-6<7)
			Colour.black=LCD_COLOR_TEST_MID;
		else
			Colour.black=LCD_COLOR_TEST_BACK;
		LCD_DrawRect( LIST2-90-12, 76+(i-6)*15,LIST2-20 , 77+(i-6)*15+15 , Colour.black ) ;
		if(SaveData.Limit_Tab.Comp_Value[i-6].low.Num!=0)
		{
			Hex_Format(SaveData.Limit_Tab.Comp_Value[i-6].low.Num , 
			SaveData.Limit_Tab.Comp_Value[i-6].low.Dot , 5 , 0);//�ӵ�λ
			
//			WriteString_12(LIST2-90-8, 76+(i-6)*15, "-",  1);
			WriteString_12(LIST2-90, 76+(i-6)*15, DispBuf,  1);//��ʾ���ֵ  ����Ҫ������ʾ��λ
			if(SaveData.Limit_Tab.Mode==0)
			WriteString_12(LIST2-42, 76+(i-6)*15, 
			Disp_Unit[SaveData.Limit_Tab.Comp_Value[i-6].low.Unit],  1);
			//WriteString_16(LIST1+70, FIRSTLINE+SPACE1, Switch_Value[SaveData.Limit_Tab.Param],  0);
			//WriteString_16(LIST2+34, FIRSTLINE, Disp_Unit[SaveData.Limit_Tab.Nom.Unit],  1);
		}
		else
		WriteString_12(LIST2-90, 76+(i-6)*15, "------",  1);	
	}
	for(i=16;i<25;i++)
	{
		
		if(i==Button_Page->index)
			Colour.black=LCD_COLOR_SELECT;
		else
			if(i-16>2&&i-16<7)
			Colour.black=LCD_COLOR_TEST_MID;
		else
			Colour.black=LCD_COLOR_TEST_BACK;
			
		LCD_DrawRect( LIST2+58, 76+(i-16)*15,LIST2+70+70 , 77+(i-16)*15+15, Colour.black ) ;
		if(SaveData.Limit_Tab.Comp_Value[i-16].high.Num !=0)
		{
			Hex_Format(SaveData.Limit_Tab.Comp_Value[i-16].high.Num ,
				SaveData.Limit_Tab.Comp_Value[i-16].high.Dot , 5 , 0);//�ӵ�λ
			
			WriteString_12(LIST2+80, 76+(i-16)*15, DispBuf,  0);//��ʾ���ֵ  ����Ҫ������ʾ��λ
			if(SaveData.Limit_Tab.Mode==0)
			WriteString_12(LIST2+70+58, 76+(i-16)*15, 
			Disp_Unit[SaveData.Limit_Tab.Comp_Value[i-16].high.Unit],  1);
		}
		else
			WriteString_12(LIST2+80, 76+(i-16)*15, "------",  1);
	}
//��������ѡ
	if(Button_Page->index == 15)
			Colour.black=LCD_COLOR_SELECT;
		else
			Colour.black=LCD_COLOR_TEST_BACK;
	LCD_DrawRect( LIST2-90-12, 76+(15-6)*15,LIST2-20 , 77+(15-6)*15+15 , Colour.black ) ;
//	if(SaveData.Limit_Tab.Comp_Value[15-6].low.Num!=0)
//	{
		Hex_Format(SaveData.Limit_Tab.Comp_Value[15-6].low.Num , 
		SaveData.Limit_Tab.Comp_Value[15-6].low.Dot , 5 , 0);//�ӵ�λ
		
//			WriteString_12(LIST2-90-8, 76+(i-6)*15, "-",  1);
		WriteString_12(LIST2-90, 76+(15-6)*15, DispBuf,  1);//��ʾ���ֵ  ����Ҫ������ʾ��λ
		WriteString_12(LIST2-42, 76+(15-6)*15, 
		Disp_Unit[SaveData.Limit_Tab.Comp_Value[15-6].low.Unit],  1);
		//WriteString_16(LIST1+70, FIRSTLINE+SPACE1, Switch_Value[SaveData.Limit_Tab.Param],  0);
		//WriteString_16(LIST2+34, FIRSTLINE, Disp_Unit[SaveData.Limit_Tab.Nom.Unit],  1);
//	}
//	else
//	WriteString_12(LIST2-90, 76+(15-6)*15, "------",  1);
	
	if(Button_Page->index == 25)
			Colour.black=LCD_COLOR_SELECT;
		else
			Colour.black=LCD_COLOR_TEST_BACK;
	LCD_DrawRect( LIST2+58, 76+(25-16)*15,LIST2+70+70 , 77+(25-16)*15+15, Colour.black ) ;
//	if(SaveData.Limit_Tab.Comp_Value[25-16].high.Num !=0)
//	{
		Hex_Format(SaveData.Limit_Tab.Comp_Value[25-16].high.Num ,
			SaveData.Limit_Tab.Comp_Value[25-16].high.Dot , 5 , 0);//�ӵ�λ
		
		WriteString_12(LIST2+80, 76+(25-16)*15, DispBuf,  0);//��ʾ���ֵ  ����Ҫ������ʾ��λ
		WriteString_12(LIST2+70+58, 76+(25-16)*15, 
		Disp_Unit[SaveData.Limit_Tab.Comp_Value[25-16].high.Unit],  1);
//	}
//	else
//		WriteString_12(LIST2+80, 76+(25-16)*15, "------",  1);
	
//	LCD_DrawRect( LIST2+88, FIRSTLINE+SPACE1*2-2,SELECT_2END , FIRSTLINE+SPACE1*3-4 , Colour.black ) ;//SPACE1
//	WriteString_16(LIST2+88, FIRSTLINE+SPACE1*2, User_Comp[SaveData.Main_Func.Bias],  1);
	Disp_Fastbutton();
	
	switch(Button_Page->index)
	{
		case 0:
				Disp_Button_TestSet(Button_Page->page);
			break;
		case 1:

		break;
		case 2:

		break;
		case 3:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				if(i==0)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+20, BUTTOM_Y_VALUE, User_LimitList_Value[i],  0);
				else
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+30, BUTTOM_Y_VALUE, User_LimitList_Value[i],  0);
			}
			break;

		case 4:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				if(i==0)
				    WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+20, BUTTOM_Y_VALUE, Switch_Value[i],  0);
				else
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+25, BUTTOM_Y_VALUE, Switch_Value[i],  0);
			}
			break;
		case 5:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				if(i==0)
				    WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+20, BUTTOM_Y_VALUE, Switch_Value[i],  0);
				else
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+25, BUTTOM_Y_VALUE, Switch_Value[i],  0);
			}
			break;
//		case 6:
//			Colour.Fword=White;
//			Colour.black=LCD_COLOR_TEST_BUTON;
//			for(i=0;i<2;i++)
//			{
//				
//				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, User_Comp[i],  0);
//			}
//			break;
		default:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			if(SaveSIM.lang==1)
			{
				WriteString_16(BUTTOM_X_VALUE+9, BUTTOM_Y_VALUE, "Cel R",  0);
				WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE+1, "CelList",  0);
			}else if(SaveSIM.lang==0){
				WriteString_16(BUTTOM_X_VALUE+9, BUTTOM_Y_VALUE, "�����",  0);
				WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE+1, "������",  0);
			}
			
//			WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE+10, "���",  0);
		break;
		
			

		
			
	
	
	}

}
//��ʾ���ò�����ֵSetup_Valueall   //�û�У���еİ���ֵ����
void Disp_Debug_value(Button_Page_Typedef* Button_Page)
{
//	uint32_t flag;
	uint32_t i;
//	uint32_t xpose;
	uint32_t Black_Select;
//	uint32_t Select_color;

	Black_Select=(Button_Page->index==1)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88, FIRSTLINE-2,SELECT_1END , FIRSTLINE+SPACE1-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88, FIRSTLINE, Switch_Value[SaveData.User_Correction.Open],  1);//��·
	
//��·
	Black_Select=(Button_Page->index==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88, FIRSTLINE+SPACE1-2,SELECT_1END , FIRSTLINE+SPACE1*2-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88, FIRSTLINE+SPACE1, Switch_Value[SaveData.User_Correction.Short],  1);
	
	
////����
//	Black_Select=(Button_Page->index==3)?1:0;
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
//		
//	LCD_DrawRect( LIST1+88, FIRSTLINE+SPACE1*2-2,SELECT_1END , FIRSTLINE+SPACE1*3 -4, Colour.black ) ;//SPACE1
//	WriteString_16(LIST1+88, FIRSTLINE+SPACE1*2, Switch_Value[SaveData.User_Correction.Load],  1);
//	
////У����
//	Black_Select=(Button_Page->index==4)?1:0;
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
//		
//	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*3-2,SELECT_1END , FIRSTLINE+SPACE1*4-4 , Colour.black ) ;//SPACE1
//	sprintf((char *)DispBuf,"%3d",SaveData.User_Correction.Short);
//	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*3, DispBuf,  1);	
//	
////Ƶ��
////	if(index==5)
////		index=6;
//	Black_Select=(Button_Page->index==5)?1:0;
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
//		
//	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*4-2,SELECT_1END , FIRSTLINE+SPACE1*5-4 , Colour.black ) ;//SPACE1
//	if(SaveData.User_Correction.Freq==1)//��
//		WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*4, "1.00000KHz",  1);
//	else
//		WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*4, "----------",  1);
//		
////�ο�A
//	Black_Select=(Button_Page->index==6)?1:0;
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
//		
//	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*5-2,SELECT_1END , FIRSTLINE+SPACE1*6-4 , Colour.black ) ;//SPACE1
//	if(SaveData.User_Correction.Freq==0)
//	{
//		sprintf((char *)DispBuf,"%3d",SaveData.User_Correction.Ref_A.Num);
//		WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*5,DispBuf,  0);
//		//��ʾ��λ
//	}
//	else
//	{
//		WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*5,"----------",  0);
//	}
//	
////��·A
//	Black_Select=(Button_Page->index==7)?1:0;
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
//		SaveData.Main_Func.Trig_time.Num=100000;
//	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*6-2,SELECT_1END , FIRSTLINE+SPACE1*7-4 , Colour.black ) ;//SPACE1
////	if(SaveData.Main_Func.Trig_time<999)
//	{
////		sprintf((char *)DispBuf,"%d",SaveData.Main_Func.Trig_time);
//		WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*6, "0.00000uS",  0);
//		
//	}
////	else
////	{
////		//sprintf((char *)DispBuf,"%6.3d",SaveData.Main_Func.Trig_time);
////		Hex_Format(SaveData.Main_Func.Trig_time , 3 , 6 , 0);
////		WriteString_16(LIST1+88+56+16, FIRSTLINE+SPACE1*6, "s",  0);
////		
////	}
//		
//	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*6, DispBuf,  0);
//	
//	
////��·A
//	Black_Select=(Button_Page->index==8)?1:0;
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
////	if(SaveData.Main_Func.Trig_time<999)
//	{
//		sprintf((char *)DispBuf,"%d",SaveData.User_Correction.Short_A.Num);
//		//WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*7, "ms",  0);
//		
//	}
////	else
////	{
////		//sprintf((char *)DispBuf,"%6.3d",SaveData.Main_Func.Temp_time);
////		Hex_Format(SaveData.Main_Func.Trig_time , 3 , 6 , 0);
////		WriteString_16(LIST1+88+56+16, FIRSTLINE+SPACE1*7, "s",  0);
////		
////	}
//	
//		
//	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*7-2,SELECT_1END , FIRSTLINE+SPACE1*8-4 , Colour.black ) ;//SPACE1
//	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*7, DispBuf,  0);
//	
////����A
//	Black_Select=(Button_Page->index==9)?1:0;
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
//		
//	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*8-2,SELECT_1END , FIRSTLINE+SPACE1*9-4 , Colour.black ) ;//SPACE1
//	sprintf((char *)DispBuf,"%d",SaveData.User_Correction.Load_A.Num);
//	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*8, DispBuf,  1);
//	
////����
//	Black_Select=(Button_Page->index==10)?1:0;
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
//		
//	LCD_DrawRect( LIST2+88, FIRSTLINE-2,SELECT_2END , FIRSTLINE+SPACE1*1-4 , Colour.black ) ;//SPACE1

//	WriteString_16(LIST2+88, FIRSTLINE, "0m",  0);
//	
//	
////��ʽ
////	Black_Select=(Button_Page->index==11)?1:0;
////	if(Black_Select)
////	{
////		Colour.black=LCD_COLOR_SELECT;
////	
////	}
////	else
////	{
//		Colour.black=LCD_COLOR_TEST_BACK;
////	}
//		
////	LCD_DrawRect( LIST2+88, FIRSTLINE+SPACE1-2,SELECT_2END , FIRSTLINE+SPACE1*2-4 , Colour.black ) ;//SPACE1
//	WriteString_16(LIST2+88, FIRSTLINE+SPACE1, "Single",  1);
//	
////����
////	if(index==12)
////		index=13;
//	Black_Select=(Button_Page->index==11)?1:0;
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
//		
//	LCD_DrawRect( LIST2+88, FIRSTLINE+SPACE1*2-2,SELECT_2END , FIRSTLINE+SPACE1*3-4 , Colour.black ) ;//SPACE1
//	WriteString_16(LIST2+88, FIRSTLINE+SPACE1*2, User_Comp[SaveData.Main_Func.Bias],  1);	
	Disp_Fastbutton();
	switch(Button_Page->index)
	{
		case 0:
				Disp_Button_TestSet(Button_Page->page);
			break;
		case 1:
			if(Button_Page->page==0)
			{
				Colour.Fword=White;
				Colour.black=LCD_COLOR_TEST_BUTON;
				for(i=0;i<2;i++)
				{
					if(i==0)
				      WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+19, BUTTOM_Y_VALUE, User_Check_Opencircuit[i],  0);
					else 
					  WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+24, BUTTOM_Y_VALUE, User_Check_Opencircuit[i],  0);				
				}
//				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+13, BUTTOM_Y_VALUE, Button_TipPage1[i],  0);
//				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4-1, BUTTOM_Y_VALUE+1, "MORE",  0);
//				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4+41, BUTTOM_Y_VALUE+1, "1/3",  0);
//				
			}
			else if(Button_Page->page==1)
			{
				Colour.Fword=White;
				Colour.black=LCD_COLOR_TEST_BUTON;
				for(i=0;i<4;i++)
				{
				  if(i==3)
					 WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+19, BUTTOM_Y_VALUE, Button_TipPage2[i],  0); 
				  else
				     WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+14, BUTTOM_Y_VALUE, Button_TipPage2[i],  0);
				}
				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4-1, BUTTOM_Y_VALUE+1, "MORE",  0);
				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4+41, BUTTOM_Y_VALUE+1, "2/3",  0);
				
			
			
			}
			else
			{
				Colour.Fword=White;
				Colour.black=LCD_COLOR_TEST_BUTON;
				WriteString_16(BUTTOM_X_VALUE, BUTTOM_Y_VALUE+19,Button_TipPage3[0],  0);
				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4-1, BUTTOM_Y_VALUE+1, "MORE",  0);
				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4+41, BUTTOM_Y_VALUE+1, "3/3",  0);
			
			
			}
		break;
		case 2:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
					if(i==0)
				      WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+19, BUTTOM_Y_VALUE, User_Check_Shortcircuit[i],  0);
					else 
					  WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+24, BUTTOM_Y_VALUE, User_Check_Shortcircuit[i],  0);	
//				if(i==1||i==2)
//				   WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, FreqButton_Tip[i],  0);
//				else
//				   WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, FreqButton_Tip[i],  0);
			}
		break;
		case 3:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, LevleButton_Tip[i],  0);
			}
			break;
		case 4:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<4;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Setup_Trig[i],  0);
			}
			break;
		case 5:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, CompButton_Tip[i],  0);
			}
			break;
		case 6:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Setup_Rsou[i],  0);
			}
			break;
		case 7:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
			}
			break;
		case 8:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
			}
			break;
		case 9:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<3;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, SetupRev_A_Button_Tip[i],  0);
			}
			break;
		case 10:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<4;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, RangeButton_Tip[i],  0);
			}
			break;
		case 11:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<3;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, SpeedButton_Tip[i],  0);
			}
			break;
		case 12:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, User_Comp[i],  0);
			}
			break;
		case 13:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
			}
			break;
		case 14://
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, CompButton_Tip[i],  0);
			}
			break;
		case 15:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Setup_Dcr[i],  0);
			}
			break;
		case 16:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<4;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, RangeButton_Tip[i],  0);
			}
			break;
		case 17:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<4;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Setup_Dcr[i],  0);
			}
			break;
		case 18:
			break;
		case 19:
			break;
		case 20:
			break;
		default:
			break;
		
			
	
	
	}

}
//��ʾ���ò�����ֵSetup_Valueall		//ϵͳ�����е�������
void Disp_Sys_value(Button_Page_Typedef* Button_Page)
{
//	uint32_t flag;
	uint32_t i;
//	uint32_t xpose;
	uint32_t Black_Select;
//	uint32_t Select_color;

	Black_Select=(Button_Page->index==1)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+94, FIRSTLINE,SELECT_1END , FIRSTLINE+SPACE1-4 , Colour.black ) ;//SPACE1
	if(SaveSIM.JK5506==1)
	{
		WriteString_16(LIST1+95, FIRSTLINE+2, "JK5506B(1.1)",  0);//
	}else if(SaveSIM.JK5506==0){
		WriteString_16(LIST1+95, FIRSTLINE+2, "JK5506(1.1)",  0);//
	}
	
	
//�ϸ�Ѷ��
	Black_Select=(Button_Page->index==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+94, FIRSTLINE+SPACE1,SELECT_1END , FIRSTLINE+SPACE1*2-4 , Colour.black );//SPACE1
	WriteString_16(LIST1+95, FIRSTLINE+SPACE1+2, Switch_Value[SaveSIM.keybeep],  1);
	
	
//���ϸ�Ѷ��
	Black_Select=(Button_Page->index==3)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+94, FIRSTLINE+SPACE1*2,SELECT_1END , FIRSTLINE+SPACE1*3 -4, Colour.black ) ;//SPACE1
	WriteString_16(LIST1+95, FIRSTLINE+SPACE1*2+2, Switch_Value[Saveeeprom.Sys_set.U_flag],  1);
	
//��ʾ����
	Black_Select=(Button_Page->index==4)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+94, FIRSTLINE+SPACE1*3,SELECT_1END , FIRSTLINE+SPACE1*4-4 , Colour.black ) ;//SPACE1
//	sprintf((char *)DispBuf,"%3d",SaveData.User_Correction.Short);
	WriteString_16(LIST1+95, FIRSTLINE+SPACE1*3+2, Sys_Language_Value[SaveSIM.lang],  0);	
	
//����
//	if(index==5)
//		index=6;
//	Black_Select=(Button_Page->index==5)?1:0;
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
//		
//	LCD_DrawRect( LIST1+94, FIRSTLINE+SPACE1*4,SELECT_1END +3, FIRSTLINE+SPACE1*5-4 , Colour.black ) ;//SPACE1
//	if(SaveData.Sys_Setup.Password==1)//��
//		WriteString_16(LIST1+95, FIRSTLINE+SPACE1*4+2, "�ر�",  0);
//	else
//		WriteString_16(LIST1+95, FIRSTLINE+SPACE1*4, "----------",  1);
		
//��λѡ��
	Black_Select=(Button_Page->index==5)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+94, FIRSTLINE+SPACE1*4,SELECT_1END , FIRSTLINE+SPACE1*5-4 , Colour.black ) ;//SPACE1
	//if(SaveData.Sys_Setup.Menu_Disp==0)
	//{
		//sprintf((char *)DispBuf,"%3d",SaveData.User_Correction.Ref_A);
		WriteString_16(LIST1+95, FIRSTLINE+SPACE1*4+2,IRaly_Value[SaveSIM.Iraly],  0);
		//��ʾ��λ
//	}
//	else
//	{
//		WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*5,"----------",  0);
//	}
	
//��
	Black_Select=(Button_Page->index==6)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	//SaveData.Sys_Setup.Data_Value.Year=17;
	LCD_DrawRect( LIST1+94, FIRSTLINE+SPACE1*5,LIST1+90+25 , FIRSTLINE+SPACE1*6-4 , Colour.black ) ;//SPACE1

	Hex_Format(RTC_TIME_DISP.YEAR , 0 , 2 , 1);
//	WriteString_16(LIST1+90, FIRSTLINE+SPACE1*5+2, DispBuf,  0);
	WriteString_16(LIST1+95, FIRSTLINE+SPACE1*5+2, DispBuf,  0);
	Colour.black=LCD_COLOR_TEST_BACK;
	WriteString_16(LIST1+95+20, FIRSTLINE+SPACE1*5+2, "-",  0);
//��
	
	Black_Select=(Button_Page->index==7)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	//SaveData.Sys_Setup.Data_Value.Mon=17;
	LCD_DrawRect( LIST1+94+30, FIRSTLINE+SPACE1*5,LIST1+90+25+30 , FIRSTLINE+SPACE1*6-4 , Colour.black ) ;//SPACE1

	Hex_Format(RTC_TIME_DISP.MONTH , 0 , 2 , 1);
	WriteString_16(LIST1+95+30, FIRSTLINE+SPACE1*5+2, DispBuf,  0);
	Colour.black=LCD_COLOR_TEST_BACK;
	WriteString_16(LIST1+95+20+30, FIRSTLINE+SPACE1*5+2, "-",  0);
	
	
	//��
	
	Black_Select=(Button_Page->index==8)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	//SaveData.Sys_Setup.Data_Value.data=17;
	LCD_DrawRect( LIST1+94+30+32, FIRSTLINE+SPACE1*5,LIST1+90+25+30+32 , FIRSTLINE+SPACE1*6-4 , Colour.black ) ;//SPACE1

	Hex_Format(RTC_TIME_DISP.DOM , 0 , 2 , 1);
    DispBuf[2]=0;
	WriteString_16(LIST1+95+30+32, FIRSTLINE+SPACE1*5+2, DispBuf,  0);
    Colour.black=LCD_COLOR_TEST_BACK;
    
    WriteString_16(LIST1, FIRSTLINE+SPACE1*6+2, "Fact No.:",  0);
    WriteString_16(LIST1+94, FIRSTLINE+SPACE1*6+2, SaveSIM.fac_num,  0);
    //Saveeeprom.fac_num;
	
//����
//	Black_Select=(Button_Page->index==10)?1:0;
//	if(Black_Select)
//	{
//		Colour.black=LCD_COLOR_SELECT;
//	
//	}
//	else
//	{
//		Colour.black=LCD_COLOR_TEST_BACK;
//	}
//		
//	LCD_DrawRect( LIST2+94, FIRSTLINE,SELECT_2END , FIRSTLINE+SPACE1-4 , Colour.black ) ;//SPACE1
//	if(SaveData.Sys_Setup.Cipher==0)
//		WriteString_16(LIST2+95, FIRSTLINE+2, "------",  0);	
//	else
//	{
//		sprintf((char *)DispBuf,"%2d",SaveData.Sys_Setup.Cipher);
//		WriteString_16(LIST2+95, FIRSTLINE+2,DispBuf,0);
//	}

	

	
//������
	Black_Select=(Button_Page->index==9)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST2+94, FIRSTLINE,SELECT_2END , FIRSTLINE+SPACE1*1-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST2+95, FIRSTLINE+2, Sys_Buard_Value[7],  0);
	
//ƫ��Դ
	
	Black_Select=(Button_Page->index==10)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST2+94, FIRSTLINE+SPACE1*1,SELECT_2END , FIRSTLINE+SPACE1*2-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST2+95, FIRSTLINE+SPACE1*1+2, Sys_Bais_Value[2],  1);	
	
	//���߷�ʽ
	
	Black_Select=(Button_Page->index==11)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST2+94, FIRSTLINE+SPACE1*2,SELECT_2END , FIRSTLINE+SPACE1*3-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST2+95, FIRSTLINE+SPACE1*2+2, Sys_Addr_value\
				[5],  1);	
	
//GPIB��ַ
	
	Black_Select=(Button_Page->index==12)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST2+94, FIRSTLINE+SPACE1*3,SELECT_2END , FIRSTLINE+SPACE1*4-4 , Colour.black ) ;//SPACE1
	sprintf((char *)DispBuf,"%2d",SaveData.Sys_Setup.GP_Addr);
	WriteString_16(LIST2+95, FIRSTLINE+SPACE1*3+2,"NONE" ,  1);	
	

//ֻ��
	
	Black_Select=(Button_Page->index==13)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST2+94, FIRSTLINE+SPACE1*4,SELECT_2END , FIRSTLINE+SPACE1*5-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST2+95, FIRSTLINE+SPACE1*4+2, Switch_Value\
				[2],  1);


//ʱ
	Black_Select=(Button_Page->index==14)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	//SaveData.Sys_Setup.Timer_Value.Hour=1;
	LCD_DrawRect( LIST2+94, FIRSTLINE+SPACE1*5,LIST2+90+25, FIRSTLINE+SPACE1*6-4 , Colour.black ) ;//SPACE1

	Hex_Format(RTC_TIME_DISP.HOUR , 0 , 2 , 1);
	WriteString_16(LIST2+95, FIRSTLINE+SPACE1*5+2, DispBuf,  0);
	Colour.black=LCD_COLOR_TEST_BACK;
	WriteString_16(LIST2+95+20, FIRSTLINE+SPACE1*5+2, ":",  0);
//��
	
	Black_Select=(Button_Page->index==15)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	//SaveData.Sys_Setup.Timer_Value.Min=17;
	LCD_DrawRect( LIST2+94+30, FIRSTLINE+SPACE1*5,LIST2+90+25+30 , FIRSTLINE+SPACE1*6-4 , Colour.black ) ;//SPACE1

	Hex_Format(RTC_TIME_DISP.MIN , 0 , 2 , 1);
	WriteString_16(LIST2+95+30, FIRSTLINE+SPACE1*5+2, DispBuf,  0);
	Colour.black=LCD_COLOR_TEST_BACK;
	WriteString_16(LIST2+95+20+30, FIRSTLINE+SPACE1*5+2, ":",  0);
	
	
	//��
	
	Black_Select=(Button_Page->index==16)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	//SaveData.Sys_Setup.Timer_Value.Sec=17;
	LCD_DrawRect( LIST2+94+30+32, FIRSTLINE+SPACE1*5,LIST2+90+25+30+32 , FIRSTLINE+SPACE1*6-4 , Colour.black ) ;//SPACE1

	Hex_Format(RTC_TIME_DISP.SEC , 0 , 2 , 1);
    DispBuf[2]=0;
	WriteString_16(LIST2+95+30+32, FIRSTLINE+SPACE1*5+2, DispBuf,  0);
	
	Disp_Fastbutton();
	switch(Button_Page->index)
	{
		case 0:
				Disp_Button_SysSet();
			break;
		case 1:
//			if(Button_Page->page==0)
//			{
//				Colour.Fword=White;
//				Colour.black=LCD_COLOR_TEST_BUTON;
//				for(i=0;i<4;i++)
//				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Button_TipPage1[i],  0);
//				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4, BUTTOM_Y_VALUE-10, "MORE",  0);
//				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4, BUTTOM_Y_VALUE+10, " 1/3",  0);
////				
//			}
//			else if(Button_Page->page==1)
//			{
//				Colour.Fword=White;
//				Colour.black=LCD_COLOR_TEST_BUTON;
////				for(i=0;i<4;i++)
//			//	WriteString_16(BUTTOM_X_VALUE+18, BUTTOM_Y_VALUE, "����",  0);
//		if(SaveSIM.lang ==1)
//		{
//			WriteString_16(BUTTOM_X_VALUE+18, BUTTOM_Y_VALUE, "Brid",  0);
//		}else if(SaveSIM.lang ==0){
//			WriteString_16(BUTTOM_X_VALUE+18, BUTTOM_Y_VALUE, "����",  0);
//		}
        
//				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4, BUTTOM_Y_VALUE-10, "MORE",  0);
//				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4, BUTTOM_Y_VALUE+10, " 2/3",  0);
				
			
			
//			}
//			else
//			{
//				Colour.Fword=White;
//				Colour.black=LCD_COLOR_TEST_BUTON;
//				WriteString_16(BUTTOM_X_VALUE, BUTTOM_Y_VALUE,Button_TipPage3[0],  0);
//				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4, BUTTOM_Y_VALUE-10, "MORE",  0);
//				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4, BUTTOM_Y_VALUE+10, " 3/3",  0);
//			
//			
//			}
		break;
		case 2:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				if(i==0)
					WriteString_16(BUTTOM_X_VALUE+19, BUTTOM_Y_VALUE, User_Comp[i],  0);
				else
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+24, BUTTOM_Y_VALUE, User_Comp[i],  0);
			}
		break;
		case 3:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				if(i==0)
					WriteString_16(BUTTOM_X_VALUE+19, BUTTOM_Y_VALUE, User_Comp[i],  0);
				else
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+24, BUTTOM_Y_VALUE, User_Comp[i],  0);
			}
			break;
		case 4:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				if(i==0)
				    WriteString_16(BUTTOM_X_VALUE+18, BUTTOM_Y_VALUE, Sys_Language_Value[i],  0);
				else 
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, Sys_Language_Value[i],  0);
			}
			break;
//		case 5:
//			Colour.Fword=White;
//			Colour.black=LCD_COLOR_TEST_BUTON;
//			for(i=0;i<2;i++)
//			{
//				if(i==0)
//				    WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+24, BUTTOM_Y_VALUE, CompButton_Tip[i],  0);
//				else 
//					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+19, BUTTOM_Y_VALUE, CompButton_Tip[i],  0);
//			}
//			break;
		case 5:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<3;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, IRaly_Value[i],  0);
			}
			break;
		case 6:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
			}
			break;
		case 7:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
			}
			break;
		case 8:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
			}
			break;
		case 9:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
			}
			break;
		case 10:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				if(i==0)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+19, BUTTOM_Y_VALUE, Sys_Bais_Value[i],  0);
				else 
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, Sys_Bais_Value[i],  0);
			}
			break;
		case 11:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<5;i++)
			{
				if(i==0||i==2||i==3)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, Sys_Addr_value[i],  0);
				else if(i==1)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+14, BUTTOM_Y_VALUE, Sys_Addr_value[i],  0);
				else 
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+9, BUTTOM_Y_VALUE, Sys_Addr_value[i],  0);
			}
			break;
		case 12:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
			}
			break;
		case 13://
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
					if(i==0)
				    WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+24, BUTTOM_Y_VALUE, CompButton_Tip[i],  0);
				else 
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+19, BUTTOM_Y_VALUE, CompButton_Tip[i],  0);

			}
			break;
		case 14:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
			}
			break;
		case 15:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
			}
			break;
		case 16:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, BiasButton_Tip[i],  0);
			}
			break;
		case 19:
			break;
		case 20:
			break;
		case 21:
			break;
		default:
			break;
		
			
	
	
	}

}
//�û�У�������Ӻ���
void Disp_Usercheck_Screen(void)
{
	
	LCD_DrawRect( 0, 0, 150, 22 , LCD_COLOR_TEST_BAR);
//	Disp_TopBar_Color();
	Disp_Fastbutton();//��ʾ���ٰ���
//	Disp_TestSet_MidRect();//��ʾ�м�ɫ��
}
void Disp_FacrCheck_Item(Button_Page_Typedef* Button_Page)
{
	uint32_t i;
	Disp_Usercheck_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	WriteString_16(0, 4, All_TopName[10],  0);
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	
	if(Button_Page->page == 1)
	{
		for(i=0;i<6;i++)
		{
			WriteString_16(0, 26+i*22, Fac_Calitem[i],  0);		
		}
	}else if(Button_Page->page == 2){
		for(i=0;i<6;i++)
		{
			WriteString_16(0, 26+i*22, CtrV_Calitem[i],  0);		
		}
	}else if(Button_Page->page == 3){
		for(i=0;i<6;i++)
		{
			WriteString_16(0, 26+i*22, Fac_ICalitem[i],  0);		
		}
	}
	
	//for(i=0;i<(sizeof(User_Check_Item)/(sizeof(User_Check_Item[0])));i++)
	//if(i<sizeof(User_Check_Item)/(sizeof(User_Check_Item[0]))/2)

//	else
//	{
//		WriteString_16(LIST2, FIRSTLINE+SPACE1*(i-sizeof(User_Check_Item)/(sizeof(User_Check_Item[0]))/2), User_Check_Item[i],  0);
//	}	
	Disp_Button_TestSet(2);
}

void Disp_FacCal(Button_Page_Typedef* Button_Page)
{
	uint32_t i;
	uint32_t Black_Select;
	
	if(Button_Page->page == 1)
	{
		for(i=0;i<6;i++)
		{
			Colour.black=LCD_COLOR_TEST_BACK;
			LCD_DrawRect(80, 26+i*22,172 ,26+i*22+16 , Colour.black ) ;
		}
		for(i=0;i<6;i++)
		{
			Black_Select=(Button_Page->index==(i+1))?1:0;
			if(Black_Select)
			{
				Colour.black=LCD_COLOR_SELECT;
			}
			else
			{
				Colour.black=LCD_COLOR_TEST_BACK;
			}
			Hex_Format(SaveSIM.CALV[i].Num,3,5,0);
			LCD_DrawRect(80, 26+i*22,156 ,26+i*22+16 , Colour.black ) ;
			WriteString_16(80, 26+i*22, DispBuf,  0);		
			WriteString_16(80+76, 26+i*22, "V",  1);
		}
	}else if(Button_Page->page == 2){
		for(i=0;i<6;i++)
		{
			Colour.black=LCD_COLOR_TEST_BACK;
			LCD_DrawRect(80, 26+i*22,172 ,26+i*22+16 , Colour.black ) ;
		}
		for(i=0;i<4;i++)
		{
			Black_Select=(Button_Page->index==(i+1))?1:0;
			if(Black_Select)
			{
				Colour.black=LCD_COLOR_SELECT;
			}
			else
			{
				Colour.black=LCD_COLOR_TEST_BACK;
			}
			Hex_Format(SaveSIM.CTRLV[i].Num,3,5,0);
			LCD_DrawRect(80, 26+i*22,156 ,26+i*22+16 , Colour.black ) ;
			WriteString_16(80, 26+i*22, DispBuf,  0);		
			WriteString_16(80+76, 26+i*22, "V",  1);
		}
	}else if(Button_Page->page == 3){
		for(i=0;i<6;i++)
		{
			Colour.black=LCD_COLOR_TEST_BACK;
			LCD_DrawRect(80, 26+i*22,172 ,26+i*22+16 , Colour.black ) ;
		}
		for(i=0;i<6;i++)
		{
			Black_Select=(Button_Page->index==(i+1))?1:0;
			if(Black_Select)
			{
				Colour.black=LCD_COLOR_SELECT;
			}
			else
			{
				Colour.black=LCD_COLOR_TEST_BACK;
			}
			Hex_Format(SaveSIM.CALI[i].Num,3,5,0);
			LCD_DrawRect(80, 26+i*22,156 ,26+i*22+16 , Colour.black ) ;
			WriteString_16(80, 26+i*22, DispBuf,  0);		
			WriteString_16(80+76, 26+i*22, "A",  1);
		}
	}
	Disp_Fastbutton();
	switch(Button_Page->index)
	{
		case 0:
		{
			Disp_Button_TestSet(2);
		}break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			WriteString_16(BUTTOM_X_VALUE+3*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,"����",  0);
			WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,"У��",  0);
		break;
	}
}


void Disp_Fac_Debug_R(Button_Page_Typedef* Button_Page)//����У������
{
	uint32_t i;
	uint32_t Black_Select;
	Black_Select=(Button_Page->index==1)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE-2,SELECT_1END , FIRSTLINE+SPACE1-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+4, FIRSTLINE, Correction_Resist[0],  0);//100uF
	

	Black_Select=(Button_Page->index==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1-2,SELECT_1END , FIRSTLINE+SPACE1*2-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1, Correction_Resist[1],  0);//10uF
	
	

	Black_Select=(Button_Page->index==3)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*2-2,SELECT_1END , FIRSTLINE+SPACE1*3 -4, Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*2, Correction_Resist[2],  0);//1uF
//	
////У����
	Black_Select=(Button_Page->index==4)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*3-2,SELECT_1END , FIRSTLINE+SPACE1*4-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*3, Correction_Resist[3],  0);	//100nF
//	
////Ƶ��
////	if(index==5)
////		index=6;
	Black_Select=(Button_Page->index==5)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*4-2,SELECT_1END , FIRSTLINE+SPACE1*5-4 , Colour.black ) ;//SPACE1
	
	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*4, Correction_Resist[4],  0);//10nF


	Disp_Fastbutton();
	switch(Button_Page->index)
	{
		case 0:
				Disp_Button_TestSet(2);
			break;
        
        default :
			if(Button_Page->page==0)
			{
				Colour.Fword=White;
				Colour.black=LCD_COLOR_TEST_BUTON;
				for(i=0;i<2;i++)
				{
					if(i==0)
				      WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+19, BUTTOM_Y_VALUE, DEBUG_Check_Opencircuit_E[i],  0);
					else 
					  WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+24, BUTTOM_Y_VALUE, DEBUG_Check_Opencircuit_E[i],  0);				
				}
//				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+13, BUTTOM_Y_VALUE, Button_TipPage1[i],  0);
//				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4-1, BUTTOM_Y_VALUE+1, "MORE",  0);
//				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4+41, BUTTOM_Y_VALUE+1, "1/3",  0);
//				
			}
			
		break;

	
	}



}
void Disp_Fac_Debug_C(Button_Page_Typedef* Button_Page)//����У������
{
	uint32_t i;
	uint32_t Black_Select;
	Black_Select=(Button_Page->index==1)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE-2,SELECT_1END , FIRSTLINE+SPACE1-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+4, FIRSTLINE, Correction_Capacitance[0],  1);//100uF
	

	Black_Select=(Button_Page->index==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1-2,SELECT_1END , FIRSTLINE+SPACE1*2-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1, Correction_Capacitance[1],  1);//10uF
	
	

	Black_Select=(Button_Page->index==3)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*2-2,SELECT_1END , FIRSTLINE+SPACE1*3 -4, Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*2, Correction_Capacitance[2],  1);//1uF
//	
////У����
	Black_Select=(Button_Page->index==4)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*3-2,SELECT_1END , FIRSTLINE+SPACE1*4-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*3, Correction_Capacitance[3],  1);	//100nF
//	
////Ƶ��
////	if(index==5)
////		index=6;
	Black_Select=(Button_Page->index==5)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*4-2,SELECT_1END , FIRSTLINE+SPACE1*5-4 , Colour.black ) ;//SPACE1
	
	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*4, Correction_Capacitance[4],  1);//10nF
//		
////�ο�A
	Black_Select=(Button_Page->index==6)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*5-2,SELECT_1END , FIRSTLINE+SPACE1*6-4 , Colour.black ) ;//SPACE1

	{
		WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*5,Correction_Capacitance[5],  0);//1nF
	}
//	
////��·A
	Black_Select=(Button_Page->index==7)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*6-2,SELECT_1END , FIRSTLINE+SPACE1*7-4 , Colour.black ) ;//SPACE1

		
	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*6, Correction_Capacitance[6],  0);//100P
	
//	
////��·A
	Black_Select=(Button_Page->index==8)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}

		
	LCD_DrawRect( LIST1+88+4, FIRSTLINE+SPACE1*7-2,SELECT_1END , FIRSTLINE+SPACE1*8-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*7, Correction_Capacitance[7],  0);//10p

	Disp_Fastbutton();
	switch(Button_Page->index)
	{
		case 0:
				Disp_Button_TestSet(2);
			break;
        
        default :
			if(Button_Page->page==0)
			{
				Colour.Fword=White;
				Colour.black=LCD_COLOR_TEST_BUTON;
				for(i=0;i<2;i++)
				{
					if(i==0)
				      WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+19, BUTTOM_Y_VALUE, DEBUG_Check_Opencircuit_E[i],  0);
					else 
					  WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+24, BUTTOM_Y_VALUE, DEBUG_Check_Opencircuit_E[i],  0);				
				}
//				WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+13, BUTTOM_Y_VALUE, Button_TipPage1[i],  0);
//				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4-1, BUTTOM_Y_VALUE+1, "MORE",  0);
//				WriteString_16(BUTTOM_X_VALUE+BUTTOM_MID_VALUE*4+41, BUTTOM_Y_VALUE+1, "1/3",  0);
//				
			}
			
		break;

	
	}



}
void Disp_UserCheck_Item(void)
{
	uint32_t i;
	Disp_Usercheck_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveSIM.lang ==1)
	{
		WriteString_16(0, 4, All_TopName_E[5],  0);
	}else if(SaveSIM.lang ==0){
		WriteString_16(0, 4, All_TopName[5],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	for(i=0;i<(sizeof(User_Check_Item_E)/(sizeof(User_Check_Item_E[0])));i++)
	//if(i<sizeof(User_Check_Item)/(sizeof(User_Check_Item[0]))/2)
	{
		WriteString_16(LIST1, FIRSTLINE+SPACE1*i, User_Check_Item_E[i],  0);

	}
//	else
//	{
//		WriteString_16(LIST2, FIRSTLINE+SPACE1*(i-sizeof(User_Check_Item)/(sizeof(User_Check_Item[0]))/2), User_Check_Item[i],  0);
//	}	
	Disp_Button_TestSet(0);
}
//�����б������Ӻ���
void Disp_LimitList_MidRect(void)
{

	LCD_DrawRect( 0, 185,479 , 150+32 , LCD_COLOR_TEST_MID ) ;
//	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
}
void Disp_LimitList_Screen(void)
{
	Disp_TopBar_Color();
	Disp_Fastbutton();//��ʾ���ٰ���
//	Disp_LimitList_MidRect();//��ʾ�м�ɫ��	 User_LimitList_Item
}

void Disp_LimitList_Item(void)			//�����б�����
{
	uint32_t i;
	Disp_LimitList_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveSIM.lang ==1)
	{
		WriteString_16(0, 4, All_TopName_E[6],  0);
	}else if(SaveSIM.lang ==0){
		WriteString_16(0, 4, All_TopName[6],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	if(SaveSIM.lang ==1)
	{
		for(i=0;i<(sizeof(User_LimitList_Item_E)/(sizeof(User_LimitList_Item_E[0])));i++)
		if(i<sizeof(User_LimitList_Item_E)/(sizeof(User_LimitList_Item_E[0]))/2)
		{ 
			if(i==2)
			   WriteString_16(330, FIRSTLINE-2, User_LimitList_Item_E[i],  0);
			else
				WriteString_16(LIST1+i*160, FIRSTLINE-2, User_LimitList_Item_E[i],  0);
		}
		else
		{
			WriteString_16(LIST1+(i-sizeof(User_LimitList_Item_E)/(sizeof(User_LimitList_Item_E[0]))/2)*160,
			 FIRSTLINE+SPACE1-4, User_LimitList_Item_E[i],  0);
		}
	}else if(SaveSIM.lang ==0){
		for(i=0;i<(sizeof(User_LimitList_Item)/(sizeof(User_LimitList_Item[0])));i++)
		if(i<sizeof(User_LimitList_Item)/(sizeof(User_LimitList_Item[0]))/2)
		{ 
			if(i==2)
			   WriteString_16(330, FIRSTLINE-2, User_LimitList_Item[i],  0);
			else
				WriteString_16(LIST1+i*160, FIRSTLINE-2, User_LimitList_Item[i],  0);
		}
		else
		{
			WriteString_16(LIST1+(i-sizeof(User_LimitList_Item)/(sizeof(User_LimitList_Item[0]))/2)*160,
			 FIRSTLINE+SPACE1-4, User_LimitList_Item[i],  0);
		}
	}
	
	for(i=0;i<10;i++)			//
	{
		if(i>3&&i<8)
		 	Colour.black=LCD_COLOR_TEST_MID;
		else
			Colour.black=LCD_COLOR_TEST_BACK;
		WriteString_12(LIST1, 65+i*15-2, User_LimitList_Item2[i],  0);
	}
	WriteString_12(LIST1, 65+10*15-2, RangeDisp_Second[SaveData.Main_Func.Param.test],  0);

	WriteString_12(160, 65-2,"LOW",  0);
	
		
	WriteString_12(330, 65-2,"HIGH",  0);	
	
	Disp_Set_Unit_16(SaveData.Main_Func.Param.test,SELECT_2END-148,FIRSTLINE-2 );//��ʾ��λ
	
	
	//Disp_Set_Unit_12(SaveData.Main_Func.Param.test,LIST2-154,65);
	Disp_Button_TestSet(0);
}
//�б�ģʽѡ����Ӻ���
void Disp_ListScan_MidRect(void)
{

	LCD_DrawRect( 0, 180,479 , 150+32 , LCD_COLOR_TEST_MID ) ;
//	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
}
void Disp_ListScan_Screen(void)
{
	Disp_TopBar_Color();
	Disp_Fastbutton();//��ʾ���ٰ���	  
//	Disp_ListScan_MidRect();//��ʾ�м�ɫ��	 User_LimitList_Item
}
void Disp_ListScan_Item(void)
{
	uint32_t i;
	Disp_ListScan_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveSIM.lang ==1)
	{
		WriteString_16(0, 4, All_TopName_E[7],  0);
	}else if(SaveSIM.lang ==0){
		WriteString_16(0, 4, All_TopName[7],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	
	if(SaveSIM.lang ==1)
	{
		WriteString_16(LIST1, FIRSTLINE, User_ListScan_Item_E[0],  0);
		for(i=1;i<(sizeof(User_ListScan_Item_E)/(sizeof(User_ListScan_Item_E[0])));i++)
		WriteString_16(LIST1+30+(i-1)*77, FIRSTLINE+SPACE1, User_ListScan_Item_E[i],  0);
	}else if(SaveSIM.lang ==0){
		WriteString_16(LIST1, FIRSTLINE, User_ListScan_Item[0],  0);
		for(i=1;i<(sizeof(User_ListScan_Item)/(sizeof(User_ListScan_Item[0])));i++)
		WriteString_16(LIST1+30+(i-1)*77, FIRSTLINE+SPACE1, User_ListScan_Item[i],  0);
	}
	
//	for(i=0;i<11;i++)
//	WriteString_16(LIST1, FIRSTLINE+SPACE1+i*16, User_LimitScan_Item2[i],0);
	Disp_Button_TestSet(0);
}



//ϵͳ�����Ӻ���
void Disp_SysLine(void)
{
 	uint32_t i;
	if(SaveSIM.lang ==1)
	{
		for(i=0;i<(sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0])));i++)
		{
			if(i<((sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0]))+1)/2))
				LCD_DrawLine( 95, FIRSTLINE+(i+1)*SPACE1-2, 90+100,FIRSTLINE+(i+1)*SPACE1-2, White );
			else
				LCD_DrawLine( 95+250, FIRSTLINE+(i-(sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0]))-1)/2)*SPACE1-2, 90+100+250,FIRSTLINE+(i-(sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0]))-1)/2)*SPACE1-2, White );
		}
	}else if(SaveSIM.lang ==0){
		for(i=0;i<(sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0])));i++)
		{
			if(i<((sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))+1)/2))
				LCD_DrawLine( 95, FIRSTLINE+(i+1)*SPACE1-2, 90+100,FIRSTLINE+(i+1)*SPACE1-2, White );
			else
				LCD_DrawLine( 95+250, FIRSTLINE+(i-(sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))-1)/2)*SPACE1-2, 90+100+250,FIRSTLINE+(i-(sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))-1)/2)*SPACE1-2, White );
		}
	}
	

}
//void Disp_Sys_MidRect(void)
//{
//
//	LCD_DrawRect( 0, 90+32,479 , 150+32 , LCD_COLOR_TEST_MID ) ;
////	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
//}
void Disp_Sys_Screen(void)
{
//	Disp_TopBar_Color();
	LCD_DrawRect(0, 0, 160,22 , LCD_COLOR_TEST_BAR);
	Disp_Fastbutton();//��ʾ���ٰ���	  
//	Disp_ListScan_MidRect();//��ʾ�м�ɫ��	 User_LimitList_Item  
}


// ��ʾϵͳ����
static void  Disp_SysDot(void)
{
	uint8_t i;
	if(SaveSIM.lang ==1)
	{
		for(i=0;i<(sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0])));i++)
		{
			if(i<((sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0]))+1)/2))
				WriteString_16(LIST1+84+4, FIRSTLINE+SPACE1*i, ":",  0);
			else
				WriteString_16(LIST2+84+4, FIRSTLINE+SPACE1*(i-(sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0])))/2), ":",  0);	
		}
	}else if(SaveSIM.lang ==0){
		for(i=0;i<(sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0])));i++)
		{
			if(i<((sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))+1)/2))
				WriteString_16(LIST1+84+4, FIRSTLINE+SPACE1*i, ":",  0);
			else
				WriteString_16(LIST2+84+4, FIRSTLINE+SPACE1*(i-(sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0])))/2), ":",  0);	
		}
	}
	
}


//ϵͳ����
void Disp_Sys_Item(void)
{
	uint32_t i;
	Disp_Sys_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveSIM.lang ==1)
	{
		WriteString_16(0, 4, All_TopName_E[8],  0);
	}else if(SaveSIM.lang ==0){
		WriteString_16(0, 4, All_TopName[8],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
//	WriteString_16(LIST1, FIRSTLINE, User_ListScan_Item[0],  0);
	if(SaveSIM.lang ==1)
	{
		for(i=0;i<(sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0])));i++)
		if(i<sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0]))/2)
		{
			WriteString_16(LIST1, FIRSTLINE+SPACE1*i, Sys_Setitem_E[i],  0);

		}
		else
		{
			WriteString_16(LIST2,FIRSTLINE+SPACE1*(i-(sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0]))+1)/2), Sys_Setitem_E[i],  0);
		}
	}else if(SaveSIM.lang ==0){
		for(i=0;i<(sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0])));i++)
		if(i<sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))/2)
		{
			WriteString_16(LIST1, FIRSTLINE+SPACE1*i, Sys_Setitem[i],  0);

		}
		else
		{
			WriteString_16(LIST2,FIRSTLINE+SPACE1*(i-(sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))+1)/2), Sys_Setitem[i],  0);
		}
	}
	
	
	Disp_SysDot();
	Disp_SysLine();
	Disp_Button_SysSet();
}

void Disp_serve_Screen(void)
{

	LCD_DrawRect(0, 0, 112,22 , LCD_COLOR_TEST_BAR);
	Disp_Fastbutton();//��ʾ���ٰ���	   
}

//У���������ý������
void Disp_Serve_correction (void)
{
	uint8_t i;

	lcd_Clear(LCD_COLOR_TEST_BACK);
	Disp_serve_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveSIM.lang ==1)
	{
		WriteString_16(0, 4, All_TopName_E[10],  0);
	}else if(SaveSIM.lang ==0){
		WriteString_16(0, 4, All_TopName[10],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	for(i=0;i<8;i++)
	{
	   if(i<4)
		   WriteString_16(LIST1+100, FIRSTLINE+SPACE1*(i+2), Correction_Capacitance[i],  0);
	   else 
		   WriteString_16(LIST2+60, FIRSTLINE+SPACE1*(i-2), Correction_Capacitance[i],  0);	 
	  
	}
//	 WriteString_16(LIST1+85, FIRSTLINE+SPACE1,"���ݳ�ʼ��",  0);
//SPACE1
		
	Disp_Button_Correction();
	
}


void Disp_Start_Corr(void)
{
	lcd_Clear(LCD_COLOR_TEST_BACK);
	Disp_serve_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveSIM.lang ==1)
	{
		WriteString_16(0, 4, All_TopName_E[10],  0);
	}else if(SaveSIM.lang ==0){
		WriteString_16(0, 4, All_TopName[10],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	
	WriteString_16(LIST2-75, FIRSTLINE+SPACE1*4+6,"StarCorr",  0);
	Disp_Button_Correction();

}



//У���������ý������
void Disp_Serve_correctionR (void)
{
	uint8_t i;

	lcd_Clear(LCD_COLOR_TEST_BACK);
	Disp_serve_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveSIM.lang ==1)
	{
		WriteString_16(0, 4, All_TopName_E[10],  0);
	}else if(SaveSIM.lang ==0){
		WriteString_16(0, 4, All_TopName[10],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	for(i=0;i<5;i++)
	{
		   WriteString_16(LIST2-60, FIRSTLINE+SPACE1*(i+2), Correction_Resist[i],  0);	
	}
//	WriteString_16(LIST2-75, FIRSTLINE+SPACE1,"�����ʼ��",  0);
//SPACE1
		
	Disp_Button_Correction();
	
}

//У������
void Disp_Correction_Set(Button_Page_Typedef* Button_Page)
{
	uint8_t i;
	
	for(i=1;i<9;i++)
	{	
		
	//else
		 if(i<5)
		{
				if(i==Button_Page->index)
				{
					Colour.black=LCD_COLOR_SELECT;
					LCD_DrawRect( LIST1+80, FIRSTLINE+SPACE1*(i+1),LIST1+190 , FIRSTLINE+SPACE1*(i+2)-4 , Colour.black ) ;
					WriteString_16(LIST1+100, FIRSTLINE+SPACE1*(i+1), Correction_Capacitance[i-1],  0);
				}
				else
				{
					Colour.black=LCD_COLOR_TEST_BACK;
					LCD_DrawRect( LIST1+80, FIRSTLINE+SPACE1*(i+1),LIST1+190 , FIRSTLINE+SPACE1*(i+2)-4 , Colour.black ) ;
					WriteString_16(LIST1+100, FIRSTLINE+SPACE1*(i+1), Correction_Capacitance[i-1],  0);
				}
		}
		
		else
		{
				if(i==Button_Page->index)
				{
					Colour.black=LCD_COLOR_SELECT;
					LCD_DrawRect( LIST2+50, FIRSTLINE+SPACE1*(i-3),LIST2+150 , FIRSTLINE+SPACE1*(i-2)-4 , Colour.black ) ;
					WriteString_16(LIST2+60, FIRSTLINE+SPACE1*(i-3), Correction_Capacitance[i-1],  0);				
				}
				else
				{
					Colour.black=LCD_COLOR_TEST_BACK;
					LCD_DrawRect( LIST2+50, FIRSTLINE+SPACE1*(i-3),LIST2+150 , FIRSTLINE+SPACE1*(i-2)-4 , Colour.black ) ;
					WriteString_16(LIST2+60, FIRSTLINE+SPACE1*(i-3), Correction_Capacitance[i-1],  0);	
				}
		
		}
	
	}
	
	
		Disp_Fastbutton();	
	switch(Button_Page->index)
	{
		case 0:
//				Disp_Button_Correction();
//			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
				Colour.Fword=White;
				Colour.black=LCD_COLOR_TEST_BUTON;
				if(SaveSIM.lang ==1)
				{
					WriteString_16(83+19, 271-29, "CONF",  0);	
					WriteString_16(83+83+19, 271-29, "SAVE",  0);
				}else if(SaveSIM.lang ==0){
					WriteString_16(83+19, 271-29, "ȷ��",  0);	
					WriteString_16(83+83+19, 271-29, "����",  0);
				}
				
			break ;
	}
	
	if(Correc_successflag==1)
	{		
		if(SaveSIM.lang ==1)
		{
			if(Button_Page->index<5)
			  WriteString_16(LIST1+210, FIRSTLINE+SPACE1*(Button_Page->index+1), "DONE",  0);
			else
			  WriteString_16(LIST2+170, FIRSTLINE+SPACE1*(Button_Page->index-3), "DONE",  0);
		}else if(SaveSIM.lang ==0){
			if(Button_Page->index<5)
			  WriteString_16(LIST1+210, FIRSTLINE+SPACE1*(Button_Page->index+1), "���",  0);
			else
			  WriteString_16(LIST2+170, FIRSTLINE+SPACE1*(Button_Page->index-3), "���",  0);
		}
			
		
    }		
	
	
	
	
}



//У������
void Disp_Correction_SetR(Button_Page_Typedef* Button_Page)
{
	uint8_t i;
	
	for(i=1;i<6;i++)
	{	
				if(i==Button_Page->index)
				{
					Colour.black=LCD_COLOR_SELECT;
					LCD_DrawRect( LIST2-80, FIRSTLINE+SPACE1*(i+1),LIST2+30 , FIRSTLINE+SPACE1*(i+2)-4 , Colour.black ) ;
					WriteString_16(LIST2-60, FIRSTLINE+SPACE1*(i+1), Correction_Resist[i-1],  0);
				}
				else
				{
					Colour.black=LCD_COLOR_TEST_BACK;
					LCD_DrawRect( LIST2-80, FIRSTLINE+SPACE1*(i+1),LIST2+30 , FIRSTLINE+SPACE1*(i+2)-4 , Colour.black ) ;
					WriteString_16(LIST2-60, FIRSTLINE+SPACE1*(i+1), Correction_Resist[i-1],  0);
				}					
	
	}
	
	
		Disp_Fastbutton();	
	switch(Button_Page->index)
	{
		case 0:
//				Disp_Button_Correction();
//			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
				Colour.Fword=White;
				Colour.black=LCD_COLOR_TEST_BUTON;
//				WriteString_16(83+19, 271-29, "ȷ��",  0);
//				WriteString_16(83+83+19, 271-29, "����",  0);
		if(SaveSIM.lang ==1)
		{
			WriteString_16(83+19, 271-29, "CONF",  0);
			WriteString_16(83+83+19, 271-29, "SAVE",  0);
		}else if(SaveSIM.lang ==0){
			WriteString_16(83+19, 271-29, "ȷ��",  0);
			WriteString_16(83+83+19, 271-29, "����",  0);
		}
		
			break ;
	}

	if(Correc_successflag==1)
	{
		 if(SaveSIM.lang ==1)
		{
			WriteString_16(LIST2+50, FIRSTLINE+SPACE1*(Button_Page->index+1), "DONE",  0);
		}else if(SaveSIM.lang ==0){
			WriteString_16(LIST2+50, FIRSTLINE+SPACE1*(Button_Page->index+1), "���",  0);
		}
				
    }		
	
}


Sort_TypeDef Time_Set_Cot(Sort_TypeDef *Time)
{
	if(Time->Num > 999000)
		Time->Num=999000;
//	Time->Dot=0;
//	Time->Unit=1;
	
	return *Time;
}


Sort_TypeDef Time_Set_Cov(Sort_TypeDef *Time)
{
	if(Time->Dot==0)//û��С����
	{
		if(Time->Unit==0)//��λ�Ǻ���
		{
			if(Time->Num>20000)//����60.000  ��ʾ60.000
			{
				Time->Num=20000;
				Time->Dot=3;
			}
			else
			if(Time->Num>999)//����999  ��ʾX.XXX
			{
				//Time->Num=60;
				Time->Dot=3;
				Time->Unit=1;
				
			} 
		
		}
		else//��λ����
		{
			if(Time->Num>20)//����60�� ��ʾ60��
				Time->Num=20;
			Time->Num*=1000;//����1000��  С����Ϊ����λ
			Time->Dot=3;	
			Time->Unit=1;			
		}
	
	
	}
	else if(Time->Dot==1)//�Ӹ�λ����һ��С����Ϊ1
	{
		if(Time->Unit==0)//��λΪ����ʱ
		{
			
				
			if(Time->Num>0)//x.xxxxʱ ��ʾ���λ
			{
				Time->Num/=10000;
			
			}
//			else if(Time->Num/1000>0)//x.xxxʱ��ʾ���λ
//			{
//				Time->Num/=1000;
//			
//			
//			}else if(Time->Num/100>0)//x.xxʱ ��ʾ���λ
//			{
//				Time->Num/=100;
//			
//			
//			}else if(Time->Num/10>0)//x.xʱ ��ʾ���λ
//			{
//				Time->Num/=10;
//			
//			
//			}
			Time->Dot=0;//��Ϊ�Ǻ���  ����û��С����
			Time->Unit=1;
	
		
		}
		else//��λ����
		{
			if(Time->Num>=10000)//x.xxxxʱ ��ʾx.xxx
			{
				//Time->Num/=10;
				Time->Unit=1;
				Time->Dot=3;//��Ϊ�Ǻ���  ����û��С����
				
			
			}else if(Time->Num>=1000)//x.xxxʱ��ʾx.xxx
			{
				
				Time->Unit=0;
				Time->Dot=0;//��Ϊ�Ǻ���  ����û��С����
			
			
			}
			Time->Num/=10;
//			else if(Time->Num>=100)//x.xxʱ ��ʾx.xx0
//			{
//				Time->Num*=10;
//			
//			
//			}else if(Time->Num>=10)//x.xʱ ��ʾx.x00
//			{
//				Time->Num*=100;
//			
//			
//			}
//			else
//				Time->Num*=1000;//x.000
				
			
			
	
		
		
		
		}
	
	
	
	}else if(Time->Dot==2)//xx.
	{
		if(Time->Unit==0)//��λΪ����ʱ
		{
			
			if(Time->Num>=1000)//xx.xxxʱ ��ʾ���λ
			{
				Time->Num/=1000;
				//Time->Dot=0;
				
			
			}
//			else if(Time->Num/100>=10)//xx.xxʱ��ʾ���λ
//			{
//				Time->Num/=1000;
//			
//			
//			}
			else// if(Time->Num<1000)//xx.xʱ ��ʾ���λ
			{
				Time->Num=0;
			
			
			}
			Time->Dot=0;//��Ϊ�Ǻ���  ����û��С����
	
		
		}
		else//��λ����
		{
			if(Time->Num>20000)//
			{
				Time->Num=20000;
			
			}
//			else if(Time->Num>=10000)//xx.xxxʱ��ʾxx.xxx
//			{
//				//Time->Num*=10;
//			
//			
//			}
//			else if(Time->Num>=1000)//xx.xxʱ ��ʾxx.xx0
//			{
//				Time->Num*=10;
//			
//			
//			}
			else if(Time->Num<1000)//xx.xʱ ��ʾxx.x00
			{
				//Time->Num*=100;
				Time->Dot=0;//��Ϊ����  ��С����3
				Time->Unit=0;
			
			
			}
//			else if()
//				Time->Num*=1000;//xx.000
			else
			{
				
				Time->Dot=3;//��Ϊ����  ��С����3
				Time->Unit=1;
			}
	
		
		
		
		}
			
	}else if(Time->Dot==3)//xxx.
	{
		if(Time->Unit==0)//��λΪ����ʱ
		{
			
			if(Time->Num>=100)//xxx.xxʱ ��ʾ���λxxx
			{
				Time->Num/=100;
			
			}
			else
			{
				Time->Num=0;
			
			}
//			else if(Time->Num/10>=100)//xxx.xʱ��ʾ���λxxx
//			{
//				Time->Num/=100;
//			
//			
//			}
			Time->Dot=0;//��Ϊ�Ǻ���  ����û��С����
	
		
		}
		else//��λ����
		{
			if(Time->Num>20000)
			{
				Time->Num=20000;//��һλ��ʾ
				Time->Dot=3;
				Time->Unit=1;
				
				
			}
			else if(Time->Num>=100)
			{
				//Time->Num*=10;
				Time->Dot=3;
				Time->Unit=1;
			
			
			}
			else //if(Time->Num>=0)
			{
				//Time->Num*=10;
				Time->Dot=0;
				Time->Unit=0;
			}
			Time->Num*=10;
			
		
		}
	}else if(Time->Dot==4)//xxxx.x
	{
		if(Time->Unit==0)//��λΪ����ʱ
		{
			
			if(Time->Num>=10000)//xxxx.xʱ ��ʾ���λxxx
			{
				
				{
					//Time->Num/=10;	
					Time->Dot=3;//��Ϊ�Ǻ���  ����û��С����
					Time->Unit=1;
				}
			
			} //xxxxʱ��ʾ���λx.xxx
			else if(Time->Num>0)
			{
				//Time->Num/=10;	
				Time->Dot=0;//��Ϊ�Ǻ���  ����û��С����
				Time->Unit=0;
			
			
			}
			Time->Num/=10;
			
			
	
		
		}
		else//��λ����
		{
			if(Time->Num>200)
			{
				Time->Num=200;
				Time->Dot=3;
				Time->Unit=1;
			}
			else
				if(Time->Num>=10)
				{
					Time->Dot=3;
					Time->Unit=1;
				
				
				
				}
				else
				{
					Time->Dot=0;
					Time->Unit=0;
				
				}
			
		Time->Num*=100;
		}
	}
	else if(Time->Dot==5)//xxxxx.
	{
		if(Time->Unit==0)//��λΪ����ʱ
		{
			
			if(Time->Num>20000)//xxxx.xʱ ��ʾ���λxxx
			{
				
				{
					Time->Num=20000;	
					Time->Dot=3;//��Ϊ�Ǻ���  ����û��С����
					Time->Unit=1;
				}
			
			} //xxxxʱ��ʾ���λx.xxx
			else if(Time->Num>=1000)
			{
				
				Time->Dot=3;//��Ϊ�Ǻ���  ����û��С����
				Time->Unit=1;
			
			
			
			}
			else
			{
				Time->Dot=0;//��Ϊ�Ǻ���  ����û��С����
				Time->Unit=0;
			
			}
			
			
	
		
		}
		else//��λ����
		{
			if(Time->Num>20)
			{
				Time->Num=20;
				
			}
			Time->Num*=1000;
			
			Time->Dot=3;
				Time->Unit=1;
			
		
		}
	}
	else
	{
	
		Time->Num=20000;
		Time->Dot=3;
		Time->Unit=1;
	
	}

	return *Time;
}
Sort_TypeDef Freq_Set_Cov(Sort_TypeDef *Freq)
{
//	uint8_t i;
	if(Freq->Unit==0)//��λ��Hz
	{
		switch(Freq->Dot)
		{
			case 0:
				if(Freq->Num>1e5)
				{
					Freq->Num=1e5;
					Freq->Updata_flag=0;
				
				}
				else
					Freq->Updata_flag=1;
					
				break;
			case 1://1��С���� 
				break;
			case 2://����С����
//				for(i=0;i<5;i++)
//				{
//					if(Freq->Num>=10&&Freq->Num<100)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
				Freq->Num/=1000;
				
				break;
			case 3://����С����
//				for(i=0;i<5;i++)
//				{
//					if(Freq->Num>=100&&Freq->Num<1000)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
				Freq->Num/=100;
				break;
			case 4://4��С����
//				for(i=0;i<5;i++)
//				{
//					if(Freq->Num>=1e3&&Freq->Num<1e4)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
				Freq->Num/=10;
				break;
			default:
//				for(i=0;i<5;i++)
//				{
//					if(Freq->Num>=1e4&&Freq->Num<1e5)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
				break;
		
		
		
		}
		
		
		
		
	
	}
	else//��λ��KHZ
	{
		switch(Freq->Dot)
		{
			case 0:
				if(Freq->Num>100000)
				Freq->Num=100000;
				else
					Freq->Num*=1000;
					
				break;
			case 1:
				if(Freq->Num>0)
					Freq->Num/=10;
//				if(Freq->Num>=10000)
				
//				for(i=0;i<5;i++)
//				{
//					if((Freq->Num>=10&&Freq->Num<100)||Freq->Num<10)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
				//Freq->Num*=100;
				break;
			case 2:
				
//				for(i=0;i<5;i++)
//				{
//					if(Freq->Num>=10&&Freq->Num<100)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
//				Freq->Num*=1000;
				break;
			case 3:
				if(Freq->Num>10000)
					Freq->Num=10000;
				Freq->Num*=10;
//				for(i=0;i<5;i++)
//				{
//					if(Freq->Num>=10&&Freq->Num<100)
//						break;
//					else
//						Freq->Num/=10;
//				
//				
//				}
//				Freq->Num*=1e4;
				break;
			case 4:
				if(Freq->Num>1000)
					Freq->Num=1000;
				Freq->Num*=100;
				
				break;
			case 5:
			if(Freq->Num>100)
				Freq->Num=100;
			Freq->Num*=1000;
			
			break;
			default:
				Freq->Num=1e6;
				break;
		
		}
	
	
	}
	
	
	return *Freq;
}
Sort_TypeDef Input_Set_Cov(Sort_TypeDef *Input_Ref)//
{
//	uint8_t i;
//	uint32_t buff[]={10000,1000,100,10,1};
	uint32_t value;
	value=pow(10.0,(float)Input_Ref->Dot);
	value=Input_Ref->Num/value;
	if(value>=1000)
	{
		Input_Ref->Unit++;
		Input_Ref->Dot+=3;
	
	
	}else if(value==0)
	{
		if(Input_Ref->Unit>0)
		{
			Input_Ref->Unit--;
			if(Input_Ref->Dot>=3)
				Input_Ref->Dot-=3;
		
		
		}
	
	
	}
	
	return *Input_Ref;
}
Sort_TypeDef Input_Set_CovPre(Sort_TypeDef *Input_Ref)//
{
//	uint8_t i;
//	uint32_t buff[]={10000,1000,100,10,1};//10.000
	uint32_t value;
	Input_Ref->Unit=0;
	//if(Input_Ref->Dot)
	value=pow(10.0,(float)Input_Ref->Dot);
	value=Input_Ref->Num/value;
	if(value>100)
	{
		
		Input_Ref->Dot=2;
	
	
	}
	
	return *Input_Ref;
}
void Disp_Cp_D(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
//	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+8*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);

}
void Disp_Cp_Q(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+9*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Cp_G(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+6*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+6);

}
void Disp_Cp_Rp(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+6);

}
void Disp_Cs_Rs(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+6);

}
void Disp_Cs_D(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+8*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
//	WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+8);

}
void Disp_Cs_Q(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+9*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Lp_Q(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+9*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Lp_Rp(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Lp_Rd(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+10*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Lp_D(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+8*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Lp_G(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+6*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Ls_D(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+8*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Ls_Q(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+9*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Ls_Rs(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);

}
void Disp_Ls_Rd(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+10*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);

}
void Disp_Z_d(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+3*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55, (uint8_t*)Out_Assic+22*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+10*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+10);

}
void Disp_Z_r(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+3*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55, (uint8_t*)Out_Assic+22*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+11*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+10);

}
void Disp_Y_d(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+4*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55, (uint8_t*)Out_Assic+22*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+10*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+10);

}
void Disp_Y_r(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+4*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55, (uint8_t*)Out_Assic+22*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+11*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+10);

}
void Disp_R_X(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+5*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55, (uint8_t*)Out_Assic+22*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+21*40*55/8);	//X
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);	//�ո�
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+10);

}

void Disp_Rs_Q(void)
{
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+9*40*55/8);//_p
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+6);

}
void Disp_Rp_Q(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+5*40*55/8);//R
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+9*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+6);

}
void Disp_G_B(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+6*40*55/8);//G
	//LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+7*40*55/8);//B
	LCD_ShowFontCN_40_55(60+40,92,40,55, (uint8_t*)Out_Assic+22*40*55/8);	//�ո�
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);	//�ո�
	
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//ð��
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
//	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+6);
}

//const uint8_t func_Item[][]=
//{};
void Disp_Button_Fun_Set(uint16_t xpos,uint16_t ypos,uint8_t * Disp_Item,Button_Page_Typedef* Button_Page)
{
	uint8_t while_flag=1,i;
	uint8_t key,Disp_Flag=1;
	uint32_t keynum;
	Button_Page->third=0;
	do{
		key=HW_KeyScsn();
	
	}
		while(key!=0xff);
	Button_Page->third=SaveData.Main_Func.Param.test;
	while(while_flag)
	{
		if(Disp_Flag)
		{
			Disp_Flag=0;
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			Disp_Fastbutton();
			Colour.black=LCD_COLOR_SELECT;
			LCD_DrawRect( LIST1+88, FIRSTLINE-2,SELECT_1END , FIRSTLINE+SPACE1-4 , Colour.black );//SPACE1
			WriteString_16(LIST1+88, FIRSTLINE, User_FUNC[Button_Page->third],  1);//�����㷨  ��˳��Ĺ���
//			LCD_DrawRect( LIST1+88, ypos-2,SELECT_1END , ypos+SPACE1-4 , Colour.black );//SPACE1
//			WriteString_16(LIST1+88, ypos, User_FUNC[Button_Page->third],  1);//�����㷨  ��˳��Ĺ���
			Colour.black=LCD_COLOR_TEST_BUTON;
			for(i=0;i<5;i++)
			{  
			  if(Button_Page->force==0)
			  {
				if(i==4)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
				else if(i==3)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+9, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
				else
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+14, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
			  }
			  
			  	 if(Button_Page->force==1)
			  {
				if(i==4)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
				else if(i==2)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+9, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
				else
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+14, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
			  }
			
			  if(Button_Page->force==4||Button_Page->force==5) 
			  {
				  if(i==4)
				     WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
				   else
					 WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+19, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
						
			  }

			  	 if(Button_Page->force==2)
			  {
				if(i==4)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
				else if(i==1||i==2)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+9, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
				else
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+14, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
			  }
			  
			  if(Button_Page->force==3)
			  {
				if(i==4)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
				else if(i<2)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+14, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
				else
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+9, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
			  }
			  
				  if(Button_Page->force==6)
			  {
				if(i==4)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
				else if(i==0)
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+19, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
				else
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+14, BUTTOM_Y_VALUE, (Disp_Item+i*8),  0);
			  }

			}
		}
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==KEY_NUM)
		{
			Disp_Flag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1:
					
					if(Fun_SelectValue[0][Button_Page->force]!=0xff)
					{
					Button_Page->third=Fun_SelectValue[0][Button_Page->force];
						while_flag=0;
						
					}
					//SaveData.Main_Func.Param.test
					SaveData.Main_Func.Param.test=Button_Page->third;
					Button_Page->third=0xff;
					break;
				case Key_F2:
					
					if(Fun_SelectValue[1][Button_Page->force]!=0xff)
					{
						//if(Fun_SelectValue[3][Button_Page->force]!=0xff)
						Button_Page->third=Fun_SelectValue[1][Button_Page->force];
						while_flag=0;
					}
					SaveData.Main_Func.Param.test=Button_Page->third;
					if(Fun_SelectValue[3][Button_Page->force]!=0xff)
					Button_Page->third=0xff;
					break;
				case Key_F3:
					
					if(Fun_SelectValue[2][Button_Page->force]!=0xff)
					{
						//if(Fun_SelectValue[3][Button_Page->force]!=0xff)
					Button_Page->third=Fun_SelectValue[2][Button_Page->force];
						while_flag=0;
					}
					SaveData.Main_Func.Param.test=Button_Page->third;
					if(Fun_SelectValue[3][Button_Page->force]!=0xff)
					Button_Page->third=0xff;
					break;
				case Key_F4:
					
					if(Fun_SelectValue[3][Button_Page->force]!=0xff)
					{
						//if(Fun_SelectValue[3][Button_Page->force]!=0xff)
					Button_Page->third=Fun_SelectValue[3][Button_Page->force];
						while_flag=0;
					}
					SaveData.Main_Func.Param.test=Button_Page->third;
					if(Fun_SelectValue[3][Button_Page->force]!=0xff)
					Button_Page->third=0xff;
					break;
				case Key_F5:
					
					while_flag=0;
					break;
				case Key_Disp:
					
                    SetSystemStatus(SYS_STATUS_TEST);
				while_flag=0;
				break;
				case Key_SETUP:
					
                    SetSystemStatus(SYS_STATUS_SETUPTEST);
				while_flag=0;
				break;
				case Key_LEFT:
					
					while_flag=0;
//					if(Button_Page->index==0)
//						Button_Page->index=6;
//					else
//					if(Button_Page->index>3)
//						Button_Page->index-=3;
//					else
//						Button_Page->index+=2;
//					Button_Page->page=0;
				Button_Page->index=0;
				break;
				case Key_RIGHT:
					
					while_flag=0;
//					if(Button_Page->index==0)
//						Button_Page->index=1;
//					else
//					if(Button_Page->index<=3)
//						Button_Page->index+=3;
//					else
//						Button_Page->index-=2;
//					Button_Page->page=0;
				Button_Page->index=10;
						
				break;
				case Key_DOWN:
					
					while_flag=0;
//					if(Button_Page->index>5)
//						Button_Page->index=0;
//					else
//						Button_Page->index++;
//					Button_Page->page=0;
				Button_Page->index=2;
					
				break;
				case Key_UP:
					
					while_flag=0;
//					if(Button_Page->index<1)
//						Button_Page->index=6;
//					else
//						Button_Page->index--;
//					Button_Page->page=0;
				Button_Page->index=0;
				break;
				default:
					
					break;
				
			
			
			
			}
			
		
		
		}
	
	
	}


}

//���ȿ���
void FAN_CTRL(void)
{
	if(mainswitch == 1)
	{
		if((Test_Dispvalue.Imvalue.Num > 2000 && Irange == 1) || temperature >= 45)
		{
			FAN_ON();
		}else if(Test_Dispvalue.Imvalue.Num < 1800 && Irange == 1 && temperature <= 35){
			FAN_OFF();
		}
	}else{
		if(temperature >= 45)
		{
			FAN_ON();
		}else if(temperature <= 35){
			FAN_OFF();
		}
	}
	
}
void Disp_Testvalue(uint8_t siwtch)
{
	
	if(siwtch == 0)
	{
		Test_Dispvalue.Vmvalue.Num = 0;
		Test_Dispvalue.Imvalue.Num = 0;
		Test_Dispvalue.Pvalue.Num = 0;
		Hex_Format(0, 3 , 5 , 0);//��ʾ��ѹ
		WriteString_Big2(130-40-30,95+10,DispBuf);
		
		LCD_ShowFontCN_40_55(300-30-32,95+10,16,32,(uint8_t*)Out_Assic2+8*(32*16/8));
		Hex_Format(0, 3 , 5 , 0);//��ʾ����+
		WriteString_Big2(300-30,95+10,DispBuf);
		
		LCD_ShowFontCN_40_55(300-30-32,150+10,16,32,(uint8_t*)Out_Assic2+7*(32*16/8));
		Hex_Format(0, 3 , 5 , 0);//��ʾ����-
		WriteString_Big2(300-30,150+10,DispBuf);
		
		if(SaveSIM.JK5506 == 0)
		{
			Prange = 1;
		}
		Hex_Format(0, 3 , 6 , 0);//��ʾ����
		WriteString_Big2(130-40-16-30,150+10,DispBuf);
//		LCD_ShowFontCN_40_55(90-40,95+55,40,55,(uint8_t*)Out_Assic+22*(55*40/8));
	}else if(siwtch == 1)
	{
		
		Hex_Format(Test_Dispvalue.Vmvalue.Num, 3 , 5 , 0);//��ʾ��ѹ
		WriteString_Big2(130-40-30,95+10,DispBuf); 	
		
		
		if(Test_Dispvalue.Imvalue.sign == 0)
		{
			Hex_Format(Test_Dispvalue.Imvalue.Num, 3 , 5 , 0);//
			WriteString_Big2(300-30,150+10,DispBuf);//��ʾ����-
			
			Hex_Format(0, 3 , 5 , 0);//��ʾ����+
			WriteString_Big2(300-30,95+10,DispBuf);
//			LCD_ShowFontCN_40_55(90-40-30,95+55,16,32,(uint8_t*)Out_Assic2+7*(32*16/8));
		}else if(Test_Dispvalue.Imvalue.sign == 1){
			Hex_Format(Test_Dispvalue.Imvalue.Num, 3 , 5 , 0);//
			WriteString_Big2(300-30,95+10,DispBuf);//��ʾ����+
			
			Hex_Format(0, 3 , 5 , 0);//��ʾ����-
			WriteString_Big2(300-30,150+10,DispBuf);
//			LCD_ShowFontCN_40_55(90-40-30,95+55,16,32,(uint8_t*)Out_Assic2+8*(32*16/8));
		}
		
		if(Irange == 1)
		{
			PRaw = (float)Test_Dispvalue.Vmvalue.Num/1000.0*((float)Test_Dispvalue.Imvalue.Num/1000.0)*1000;//��λmW
		}else{
			PRaw = (float)Test_Dispvalue.Vmvalue.Num/1000.0*((float)Test_Dispvalue.Imvalue.Num/1000.0);//��λmW
		}
		

		if(SaveSIM.JK5506 == 0)
		{
			Prange = 1;
			Test_Dispvalue.Pvalue.Num = (uint32_t)(PRaw);
		}else if(SaveSIM.JK5506 == 1){
			if(Prange == 1)
			{
				if(PRaw < 10)
				{
					Prange = 0;
					Test_Dispvalue.Pvalue.Num = (uint32_t)(PRaw*1000);
				}else{
					Test_Dispvalue.Pvalue.Num = (uint32_t)(PRaw);
				}
			}else if(Prange == 0){
				if(PRaw > 100)
				{
					Prange = 1;
					Test_Dispvalue.Pvalue.Num = (uint32_t)(PRaw);
				}else{
					Test_Dispvalue.Pvalue.Num = (uint32_t)(PRaw*1000);
				}
			}
		}

		Hex_Format(Test_Dispvalue.Pvalue.Num, 3 , 6 , 0);//��ʾ����
		WriteString_Big2(130-40-16-30,150+10,DispBuf);
		
		
	}
	FAN_CTRL();//���ȿ���

}
void Disp_Big_MainUnit(uint8_t unit,uint8_t unit1)		//��ʾ��������λ
{

	LCD_ShowFontCN_40_55(200-30,95+10,16,32,(uint8_t*)Out_Assic2+0*(32*16/8));//V
	
	if(Prange == 1)
	{
		LCD_ShowFontCN_40_55(158,150+10,16,32,(uint8_t*)Out_Assic2+5*(32*16/8));
	}else{
		LCD_ShowFontCN_40_55(158,150+10,16,32,(uint8_t*)Out_Assic2+3*(32*16/8));
	}
	LCD_ShowFontCN_40_55(158+16,150+10,16,32,(uint8_t*)Out_Assic2+6*(32*16/8));//W
}

void Disp_Big_SecondUnit(uint8_t unit,uint8_t unit1)	//��ʾ��������λ
{
//	const uint8_t nuitnum[]={12,15,16,17,22,14,13};
//	const uint8_t nuit_nuit[]={18,19,20,2,11,23,22};//F,H,����S r ��ո�
//	if(Irange == 1)
//	{
//		LCD_ShowFontCN_40_55(DISP_UNIT_XPOS-40,DISP_UNIT_YPOS+55,40,55,(uint8_t*)Out_Assic+22*(55*40/8));
//	}else{
//		LCD_ShowFontCN_40_55(DISP_UNIT_XPOS-40,DISP_UNIT_YPOS+55,40,55,(uint8_t*)Out_Assic+31*(55*40/8));
//	}
//	LCD_ShowFontCN_40_55(DISP_UNIT_XPOS,DISP_UNIT_YPOS+55,40,55,(uint8_t*)Out_Assic+28*(55*40/8));
	
	if(Irange == 1)
	{
		LCD_ShowFontCN_40_55(430-16-16-30,95+10,16,32,(uint8_t*)Out_Assic2+5*(32*16/8));
		LCD_ShowFontCN_40_55(430-16-16-30,150+10,16,32,(uint8_t*)Out_Assic2+5*(32*16/8));
	}else{
		LCD_ShowFontCN_40_55(430-16-16-30,95+10,16,32,(uint8_t*)Out_Assic2+3*(32*16/8));
		LCD_ShowFontCN_40_55(430-16-16-30,150+10,16,32,(uint8_t*)Out_Assic2+3*(32*16/8));
	}
	LCD_ShowFontCN_40_55(430-16-30,95+10,16,32,(uint8_t*)Out_Assic2+2*(32*16/8));
	LCD_ShowFontCN_40_55(430-16-30,150+10,16,32,(uint8_t*)Out_Assic2+2*(32*16/8));
//	LCD_ShowFontCN_40_55(DISP_UNIT_XPOS+40,DISP_UNIT_YPOS+55,40,55,(uint8_t*)Out_Assic+nuit_nuit[unit1]*(55*40/8));
}




//void DelayMs(uint32_t nMs)
//{
//	uint32_t i;
//	for(; nMs >0; nMs--)
//	{
//		for(i=10301;i>0;i--);
//	}
//}



void  Start_Correction(void)			//��ʼУ��
{
	uint8_t Send_corr_start[8]={0xAB,0x01,0x08,0x06,0xC0,0x01,0x00,0xBF};
	uint8_t i=5;
	
	INSTRUCT_TO_DEBUG;
	UARTPuts( LPC_UART0, Send_corr_start);
	INSTRUCT_TO_DEBUG;
	while(i)	
	{
			i--;
		if(ComBuf.rec.buf[SITE]==INSTR_FIVE)
			i=0;		
//		else 
//			UARTPuts( LPC_UART0, Send_corr_start);
		INSTRUCT_TO_DEBUG;			
	}	

	
}

void Init_C(void)//���ݳ�ʼ��У��
{
  uint8_t Send_correction_C[8]={0xAB,0x01,0x08,0x06,0xE0,0x00,0x00,0xBF};
  uint8_t i=5;

		 UARTPuts( LPC_UART0, Send_correction_C);
		 INSTRUCT_TO_DEBUG;
			while(i)	
		{
				i--;
			if(ComBuf.rec.buf[SITE]==INSTR_FIVE)
				i=0;		
//			else 
//				UARTPuts( LPC_UART0, Send_correction_C);
			INSTRUCT_TO_DEBUG;
		}	
}


void Correction_C(Button_Page_Typedef* Button_Page)		//����У��
{
	uint8_t Send_correction_C[8]={0xAB,0x01,0x08,0x06,0xE0,0x00,0x00,0xBF};
	uint8_t i=105;	

		Send_correction_C[5]=Button_Page->index;//����У��ֵ
		UARTPuts( LPC_UART0, Send_correction_C);
		LONGDELAY;
		while(i)	
		{
				i--;
			if(ComBuf.rec.buf[SITE]==INSTR_FIVE)
			{  
				i=0;		
				Correc_successflag=1;
			}

			LONGDELAY;
		}
		Correc_successflag=1;
}
	
void Save_C(void)	//�������ֵ
{
	uint8_t Send_correction_C[8]={0xAB,0x01,0x08,0x06,0xE0,0x0F,0x00,0xBF};		//����У������
	uint8_t i=5;

		 UARTPuts( LPC_UART0, Send_correction_C);
		 INSTRUCT_TO_DEBUG;
			while(i)	
		{
				i--;
			if(ComBuf.rec.buf[CORRE_SITE]==INSTR_FOUR)
				i=0;		
//			else 
//				UARTPuts( LPC_UART0, Send_correction_C);
			INSTRUCT_TO_DEBUG;
		}		

}

//��ʼ������
void Init_R(void)
{
	uint8_t Send_correction_R[8]={0xAB,0x01,0x08,0x06,0xE1,0x00,0x00,0xBF};
	uint8_t i=5;

	UARTPuts( LPC_UART0, Send_correction_R);
	INSTRUCT_TO_DEBUG;
			while(i)	
		{
				i--;
			if(ComBuf.rec.buf[SITE]==INSTR_FIVE)
				i=0;		
//			else 
//				UARTPuts( LPC_UART0, Send_correction_R);
			INSTRUCT_TO_DEBUG;
		}

}

void Correction_R(Button_Page_Typedef* Button_Page)		//����У��
{
	uint8_t Send_correction_R[8]={0xAB,0x01,0x08,0x06,0xE1,0x00,0x00,0xBF};
	uint8_t i=106;
	    Send_correction_R[5]=Button_Page->index;//����У��ֵ
	    UARTPuts( LPC_UART0, Send_correction_R);						
		LONGDELAY;
		while(i)	
		{
				i--;
			if(ComBuf.rec.buf[SITE]==INSTR_FIVE)
			{ 
				i=0;	
				Correc_successflag=1;	
			}
		 LONGDELAY;
		}
	Correc_successflag=1;
	
}


void Save_R(void)	//�������ֵ
{
   uint8_t Send_correction_R[8]={0xAB,0x01,0x08,0x06,0xE1,0x0F,0x00,0xBF};
   uint8_t i=5;

		 UARTPuts( LPC_UART0, Send_correction_R);
		 INSTRUCT_TO_DEBUG;
			while(i)	
		{
				i--;
			if(ComBuf.rec.buf[CORRE_SITE]==INSTR_FOUR)
				i=0;		
//			else 
//				UARTPuts( LPC_UART0, Send_correction_R);
			INSTRUCT_TO_DEBUG;
		}		

}

void Exit_correction(void)		//�˳�У��
{

	uint8_t Exit_corre[8]={0xAB,0x01,0x08,0x06,0xE2,0x00,0x00,0xBF};
	uint8_t i=5;		
		UARTPuts( LPC_UART0, Exit_corre);		//�˳�У��
		INSTRUCT_TO_DEBUG;
			while(i)	
		{
				i--;
			if(ComBuf.rec.buf[CORRE_SITE]==INSTR_FOUR)
				i=0;		
			else 
				UARTPuts( LPC_UART0, Exit_corre);
			INSTRUCT_TO_DEBUG;
		}


}

void Clear_Instr(void)		//���У��
{
	uint8_t Clear_corre[8]={0xAB,0x01,0x08,0x06,0xD0,0x01,0x00,0xBF};
	uint8_t i=3;		
	UARTPuts( LPC_UART0, Clear_corre);		//�˳�У��
//	LONGDELAY;
//		while(i)	
//	{
//			i--;
//		if(ComBuf.rec.buf[CORRE_SITE]==INSTR_FOUR)
//			i=0;		
////		else 
////			UARTPuts( LPC_UART0, Clear_corre);
//		LONGDELAY;
//	}


}

void Close_Clear(void)		//�ر�����
{
	uint8_t Send_buff[8]={0xAB,0X01,0x08,0x06,0xF0,0x00,0x00,0xBF};
	UARTPuts( LPC_UART0, Send_buff);
}


void Open_Clear(void)		//��·����
{
	uint8_t Send_buff[8]={0xAB,0X01,0x08,0x06,0xF0,0x01,0x00,0xBF};
	UARTPuts( LPC_UART0, Send_buff);
}



void Short_Clear(void)			//��·����
{
	uint8_t Send_buff[8]={0xAB,0X01,0x08,0x06,0xF0,0x02,0x00,0xBF};
	UARTPuts( LPC_UART0, Send_buff);
}

void Send_Freq(Send_Ord_Typedef *ord)
{
	uint8_t Send_buff[10]={0xAB,0X01,0x08,0x06};
	Send_buff[4]=ord->Ordel;
	Send_buff[5]=ord->name;
	Send_buff[6]=0X80;//У��� 
	Send_buff[7]=0XBF;
	Send_buff[8]=0;
	UARTPuts( LPC_UART0, Send_buff);	
	//_printf("s%",(const)Send_buff);

}
void Send_UartStart(void)
{
	uint8_t Send_buff[24]={0xAB,0X01,22,0x06};
	uint8_t i=5;
	Send_buff[4]=0x60;
	Send_buff[5]=Disp_Main_Ord[SaveData.Main_Func.Param.test][0];
	Send_buff[6]=0X70;
	Send_buff[7]=Disp_Main_Ord[SaveData.Main_Func.Param.test][1];
	Send_buff[8]=0x71;
	Send_buff[9]=Disp_Main_Ord[SaveData.Main_Func.Param.test][2];
	Send_buff[10]=0x80;
	Send_buff[11]=SaveData.Main_Func.Freq;
	Send_buff[12]=0x90;
	Send_buff[13]=SaveData.Main_Func.Level;
	Send_buff[14]=0xa0;
	Send_buff[15]=SaveData.Main_Func.Speed;
	Send_buff[16]=0xb0;
	Send_buff[17]=SaveData.Main_Func.Range.Range;
//	Send_buff[18]=0xc0;
//	Send_buff[19]=SaveData.Main_Func.Avg;
	Send_buff[18]=0xe0;
	Send_buff[19]=SaveData.Main_Func.Rsou;

//	Send_buff[11]=SaveData.Main_Func.Rsou;
//	Send_buff[10]=0xe0;
//	Send_buff[11]=SaveData.Main_Func.Rsou;
	Send_buff[20]=0X00;//У��� 0
	Send_buff[21]=0XBF;	
//	Send_buff[21]=0X80;//У��� 0
//	Send_buff[22]=0XBF;
//	Send_buff[23]=0;
	UARTPuts( LPC_UART0, Send_buff);	
//	INSTRUCT_TO_DEBUG;
//	while(i)	
//	{
//			i--;
//		if(ComBuf.rec.buf[5]==0x00)
//			i=0;
//		else 
//			UARTPuts( LPC_UART0, Send_buff);
//	INSTRUCT_TO_DEBUG;
//	}
	//_printf("s%",(const)Send_buff);

}

void softdelay(u32 time)
{
	u32 count = 0;;
	while(count < time)
	{
		count++;
	}
}
void Send_Request(u8 x,u8 req)
{
	
	u8 i,len;
	char buf[50];
	DE485();
	busyflag = 1;
	strcpy(USART_RX_BUF,CmdStr[x-1]);
	len = strlen(CmdStr[x-1]);
	switch(x)
	{
		case 1:
		{
			len -= 1;
		}break;
		case 2:
		{
			sprintf(&USART_RX_BUF[len],"%1d",req);
		}break;
		case 3:
		{
			sprintf(&USART_RX_BUF[len],"%1d",req);
			sendflag = 1;
		}break;
		case 4:
		{
			sprintf(&USART_RX_BUF[len],"%1d",req);
		}break;
		case 5:
		{
			sprintf(&USART_RX_BUF[len],"%1d",req);
		}break;
		case 6:
		{
//			sprintf(&USART_RX_BUF[len],"%1d",req);

		}break;
		case 8:
		{
//			sprintf(&USART_RX_BUF[len],"%1d",req);

		}break;
		case 10:
		{
			sprintf(buf,"+%07.3f,%07.3f,%07.3f,%03d,%03d,%01d",SaveSIM.Voltage.Num/1000.0,SaveSIM.ChargePC.Num/1000.0,SaveSIM.LoadPC.Num/1000.0
			,SaveSIM.ChargePT.Num,SaveSIM.LoadPT.Num,SaveSIM.Iraly);
			strcat(USART_RX_BUF,buf);
			len+=strlen(buf)-1;
			sendflag = 2;
		}break;
		case 11:
		{
			len -= 1;
		}break;
		case 12:
		{
			sprintf(buf,"%d%07.3f",req-1,SaveSIM.CALV[req-1].Num/1000.0);
			strcat(USART_RX_BUF,buf);
			len+=7;
		}break;
		case 13:
		{
			sprintf(buf,"%d%07.3f",req-1,SaveSIM.CALI[req-1].Num/1000.0);
			strcat(USART_RX_BUF,buf);
			len+=7;
		}break;
		case 14:
		{
			sprintf(buf,"%d%07.3f",req-1,SaveSIM.CTRLV[req-1].Num/1000.0);
			strcat(USART_RX_BUF,buf);
			len+=7;
		}break;
		case 15:
		{
			sprintf(&USART_RX_BUF[len],"%1d",req);
		}break;
		case 16:
		{
			if(req == 1)
			{
				sprintf(buf,"%03d",SaveSIM.ChargePT.Num);
			}else{
				sprintf(buf,"%03d",SaveSIM.LoadPT.Num);
			}
			strcat(USART_RX_BUF,buf);
			len+=2;
//			sendflag = 3;
		}break;
		case 17:
		{
			len -= 1;
		}break;
	}
	USART_RX_BUF[len+1] = 0x0d;
	USART_RX_BUF[len+2] = 0x0a;
	
//	delayMs(0,2);
	UARTPuts(LPC_UART0,USART_RX_BUF);
	softdelay(5000);
//	delayMs(0,2);
	RE485();
	busyflag = 0;
}

uint16_t SetErr_ACK(char *buf, uint8_t addr ,uint8_t ucErr)
{
		uint16_t i=0;
		uint8_t len = 0;
		char stradd[3] = {0};
		int addlen=0;
		memset(buf,0,sizeof(buf));
    	buf[i++] = ChrStartS;
		sprintf(stradd,"%d",addr);
		addlen=strlen(stradd);
		strncat(&buf[i],stradd,addlen);
		i+=addlen;
		buf[i++] = '/';
		switch(ucErr)
        {
          case 1:         //
          	   len = 9;
          	   memmove(&buf[i],"PARA ERR!",len);   
          	   break;
	      case 2:         //
          	   len = 8;
          	   memmove(&buf[i],"CMD ERR!",len);   
          	   break;
	      case 3:         //
          	   len = 8;
          	   memmove(&buf[i],"REM ERR!",len);   
          	   break;
		  default:    
               len = 8;
          	   memmove(&buf[i],"CMD ERR!",len);   
               break;
	    }
	    i+=len;
		buf[i++] = ChrEndS;
		return i;
}

uint16_t SerialRemoteHandleL(uint8_t len,char* buf)
{

  
    uint16_t currCharNum;
    uint32_t temp1 = 0;
  uint8_t Gmode_Vale;
	uint8_t i,j;
	
	uint8_t addr = 0;
	char str[30]={0};
	int cmd_flag=255;
	uint8_t templen =0;
	uint8_t tmpFg;
	uint8_t LM_S_Vale,LOW_I_Vale,H_L_Vale,SWITCH_Vale;
	uint16_t I_ADC_Vale,V_ADC_Vale;

	float Lvl_Vale,fadcx,fsingal, fVale;
 
//	uint8 len = 11;
	uint8_t pntlen = 0;
//  char StrON[2] = {"ON"};

	
	currCharNum=0;
	if((buf[currCharNum] != ChrStartR) || (buf[len-2] != ChrEndR)||(buf[len-1] != ChrEndS))
    {
//		return SetErr_ACK(buf,addr ,CMD_ERR); 
		return 0; 
    } 
	str[currCharNum++] = ChrStartR;
	sprintf(&str[currCharNum],"%d",addr);
	strncat(str,"/",1);
	currCharNum =strlen(str);
	cmd_flag=strncmp(buf,str,strlen(str));
	//testflag = cmd_flag;
	if(0!=cmd_flag)
	{
//		return SetErr_ACK(buf, addr ,CMD_ERR);
		return 0;
	}
	for (j=0;j<CmdNumb;j++)
    {
		templen = strlen(RecStr[j]);
		cmd_flag=strncmp(&buf[currCharNum],RecStr[j],templen);
	    if(0==cmd_flag)
	    {

		  buf[0] = ChrStartS;
		  currCharNum+=templen;
		  switch(j)
          {
          	case 0:	
			break;
			case 1:

				break;
			case 2:
				pntlen = 5;    //
				if(buf[currCharNum]=='+')
				{
					currCharNum++;
					for(i=0;i<5;i++)
					{
						temp1 = temp1*10+(buf[currCharNum++]-0x30);
					}
					Test_Dispvalue.Vmvalue.Num = temp1;
//					currCharNum++;
					temp1 = 0;
					
					if(buf[currCharNum]=='+')
					{
						Test_Dispvalue.Imvalue.sign = 0;
						currCharNum++;
						for(i=0;i<5;i++)
						{
							temp1 = temp1*10+(buf[currCharNum++]-0x30);
						}
					}else if(buf[currCharNum]=='-'){
						Test_Dispvalue.Imvalue.sign = 1;
						currCharNum++;
						for(i=0;i<5;i++)
						{
							temp1 = temp1*10+(buf[currCharNum++]-0x30);
						}
					}
					Test_Dispvalue.Imvalue.Num = temp1;
					
					temp1 = 0;
					if(buf[currCharNum++] == ',')
					{
						temp1 = temp1*10+(buf[currCharNum++]-0x30);
						Irange = temp1;
					}
					
					temp1 = 0;
					if(buf[currCharNum++] == ',')
					{
						temp1 = temp1*10+(buf[currCharNum++]-0x30);
						if(mainswitch == 1)
						{
							mainswitch = temp1;
						}
					}
					temp1 = 0;
					if(buf[currCharNum++] == ',')
					{
						for(i=0;i<2;i++)
						{
							temp1 = temp1*10+(buf[currCharNum++]-0x30);
						}
						temperature = temp1;
					}
//					if(mainswitch == 1)
//					{
//						sendflag = 1;
//					}
				}
				else
				{
					
				}
				break;
//			case 3:
//				pntlen = 1;
//				if(buf[currCharNum]=='?')
//				{
//					sprintf(&buf[currCharNum],"%1d",Gmode_Vale);
//				    currCharNum+=pntlen;
//					buf[currCharNum] = ChrEndR;
//					currCharNum++;
////				    buf[currCharNum ++] = ChrEndS;
//					
//				}
//				else
//				{
//					tmpFg = 1;
//		            for(i=0;i<pntlen;i++)
//		            {
//					   if(IsDigitChar(buf[currCharNum+i])==1)
//					   {
//					      tmpFg = (tmpFg&0x01);
//					   }
//					   else
//					   {
//					      tmpFg = 0;
//					   }
//		            }
//					if(tmpFg!=1||buf[currCharNum+pntlen]!=ChrEndR)
//					{
//						return SetErr_ACK(buf, addr ,CMD_ERR);
//						
//					}					
//					for(i=0,temp1=0;i<pntlen;i++)
//		            {
//		                  temp1 = temp1*10+(buf[currCharNum++]-0x30);
//					}		                   
//				    if(temp1 >Mode_H)
//			         {
//			             return SetErr_ACK(buf, addr ,PARA_ERR);
//						 
//			         }
//		        
//							 Mode=temp1;
//							Change_MODE_Val(Mode);
//					 buf[currCharNum++] = ChrEndS;
//		        }
//				break;
//			case 4:
//				pntlen = 10;    //
//				if(buf[currCharNum]=='?')
//				{
//					I_CS_Vale = Read_CS1237();
//					
//					buf[currCharNum++]=(I_CS_Vale>=0?'+':'-');
//					sprintf(&buf[currCharNum],"%010d",I_CS_Vale);
//					currCharNum+=pntlen;
//				  buf[currCharNum++ ] = ChrEndS;
//					
//				}
//				else
//				{
//					return SetErr_ACK(buf, addr ,CMD_ERR);
//		    }
//				break;
//			case 5:
//				pntlen = 7;
//				if(buf[currCharNum]=='?')
//				{
//					I_ADC_Vale = Get_Adc_Average(ADC_Channel_14,3);
//					fadcx=(Vref*(((float)I_ADC_Vale/4096))); 
//					sprintf(&buf[currCharNum],"%01.5f",fadcx);
//				//	sprintf(&buf[currCharNum],"%07d",I_ADC_Vale);
//				    currCharNum+=pntlen;
//				    buf[currCharNum++ ] = ChrEndS;
//					
//				}
//				else
//				{
//					return SetErr_ACK(buf, addr ,CMD_ERR);
//		    }
//		  
//				break;
			case 6:
				pntlen = 1;				
				for(i=0,temp1=0;i<pntlen;i++)
				{
					  temp1 = temp1*10+(buf[currCharNum++]-0x30);
				}		                   
				if(temp1>1)
			    {
				    return SetErr_ACK(buf, addr ,PARA_ERR);
					
			    }
				mainswitch=temp1;
				sendflag = 0;
//				Change_LM_Val(LM_S_Vale);
				buf[currCharNum++] = ChrEndR;
			break;
//			case 7:
//				pntlen = 7;
//				if(buf[currCharNum]=='?')
//				{
//					V_ADC_Vale = Get_Adc_Average(ADC_Channel_15,3);
//					fadcx=(Vref*(((float)V_ADC_Vale/4096))); 
//							sprintf(&buf[currCharNum],"%01.5f",fadcx);
//					//V_ADC_Vale = Get_Adc_Average(15,5);
//					//sprintf(&buf[currCharNum],"%07d",V_ADC_Vale);
//				  currCharNum+=pntlen;
//				  buf[currCharNum++ ] = ChrEndS;
//				}
//				else
//				{
//					return SetErr_ACK(buf, addr ,CMD_ERR);
//		    }
//				break;
//			case 8:
//				pntlen = 1;
//				if(buf[currCharNum]=='?')
//				{
//					LOW_I_Vale = OverI();
//					LOW_I = LOW_I_Vale;
//					sprintf(&buf[currCharNum],"%1d",LOW_I_Vale);
//				    currCharNum+=pntlen;
//				    buf[currCharNum++ ] = ChrEndS;
//					
//				}
//				else
//				{
//					tmpFg = 1;
//		            for(i=0;i<pntlen;i++)
//		            {
//					   if(IsDigitChar(buf[currCharNum+i])==1)
//					   {
//					      tmpFg = (tmpFg&0x01);
//					   }
//					   else
//					   {
//					      tmpFg = 0;
//					   }
//		            }
//					if(tmpFg!=1||buf[currCharNum+pntlen]!=ChrEndR)
//					{
//						return SetErr_ACK(buf, addr ,CMD_ERR);
//						
//					}					
//					for(i=0,temp1=0;i<pntlen;i++)
//		            {
//		                  temp1 = temp1*10+(buf[currCharNum++]-0x30);
//					}		                   
//				    if(temp1>SWITCH_A_H)
//			         {
//			             return SetErr_ACK(buf, addr ,PARA_ERR);
//						 
//			         }
//		        	 LOW_I_Vale=temp1;
//							Change_LOW_I_Val(LOW_I_Vale);
//					 buf[currCharNum++] = ChrEndS;
//		        }
//				
//				break;
//			case 9:
//				pntlen = 1;
//				if(buf[currCharNum]=='?')
//				{
//					sprintf(&buf[currCharNum],"%1d",H_L_Vale);
//				    currCharNum+=pntlen;
//				    buf[currCharNum ++] = ChrEndS;
//					
//				}
//				else
//				{
//					tmpFg = 1;
//		            for(i=0;i<pntlen;i++)
//		            {
//					   if(IsDigitChar(buf[currCharNum+i])==1)
//					   {
//					      tmpFg = (tmpFg&0x01);
//					   }
//					   else
//					   {
//					      tmpFg = 0;
//					   }
//		            }
//					if(tmpFg!=1||buf[currCharNum+pntlen]!=ChrEndR)
//					{
//						return SetErr_ACK(buf, addr ,CMD_ERR);
//						
//					}					
//					for(i=0,temp1=0;i<pntlen;i++)
//		            {
//		                  temp1 = temp1*10+(buf[currCharNum++]-0x30);
//					}		                   
//				    if(temp1 >1)
//			         {
//			             return SetErr_ACK(buf, addr ,PARA_ERR);
//						 
//			         }

//							 H_L_Vale=temp1;
//						
//							Change_H_L_Vale(H_L_Vale);
//					 buf[currCharNum++] = ChrEndS;
//		        }
//				break;
			case 10:
				sendflag = 0;
				break;
			case 15:
				pntlen = 2;				
				for(i=0,temp1=0;i<pntlen;i++)
				{
					  temp1 = temp1*10+(buf[currCharNum++]-0x30);
				}		                   

				temperature=temp1;
//				sendflag = 0;
//				Change_LM_Val(LM_S_Vale);
				buf[currCharNum++] = ChrEndR;
			break;
//			case 11:
//				pntlen = 1;
//				if(buf[currCharNum]=='?')
//				{
//					sprintf(&buf[currCharNum],"%1d",SWITCH_Vale);
//				    currCharNum+=pntlen;
//				    buf[currCharNum++ ] = ChrEndS;
//					
//				}
//				else
//				{
//					tmpFg = 1;
//		            for(i=0;i<pntlen;i++)
//		            {
//					   if(IsDigitChar(buf[currCharNum+i])==1)
//					   {
//					      tmpFg = (tmpFg&0x01);
//					   }
//					   else
//					   {
//					      tmpFg = 0;
//					   }
//		            }
//					if(tmpFg!=1||buf[currCharNum+pntlen]!=ChrEndR)
//					{
//						return SetErr_ACK(buf, addr ,CMD_ERR);
//						
//					}					
//					for(i=0,temp1=0;i<pntlen;i++)
//		            {
//		                  temp1 = temp1*10+(buf[currCharNum++]-0x30);
//					}		                   
//				    if(temp1>SWITCH_A_H)
//			         {
//			             return SetErr_ACK(buf, addr ,PARA_ERR);
//						 
//			         }
//		        	 SWITCH_Vale=temp1;
//							Change_SWITCH_Val(SWITCH_Vale);
//					 buf[currCharNum++] = ChrEndR;
//		        }
//				break;
//				case 12:
//				{
//					pntlen = 8;
//					if(buf[currCharNum]=='?')
//					{
//						buf[currCharNum++]=(Lvl_Vale>=0?'+':'-');
//									sprintf(&buf[currCharNum],"%03.3f",Lvl_Vale);
//						currCharNum+=pntlen;
//						buf[currCharNum++ ] = ChrEndS;
//					}
//					else
//					{
//						tmpFg = 1;
//						for(i=0;i<pntlen;i++)
//						{
//						   if(i==0||i==4)
//							  continue;
//						   if(IsDigitChar(buf[currCharNum+i])==1)
//						   {
//							  tmpFg = (tmpFg&0x01);
//						   }
//						   else
//						   {
//							  tmpFg = 0;
//						   }
//						}
//						if(tmpFg!=1||buf[currCharNum+pntlen]!=ChrEndR)
//						{
//							return SetErr_ACK(buf, addr ,CMD_ERR);
//							
//						}
//						temp1=0;
//						for(i=0;i<pntlen;i++)
//						{
//						   if(i==0)
//						   {
//							   fsingal = buf[currCharNum++]-0x30;
//							   continue; 
//						   } 
//						   else if(i==4)
//						   {
//							   currCharNum++;
//							   continue; 
//						   }
//						   else									 
//						   temp1 = temp1*10+(buf[currCharNum++]-0x30);
//						}	
//						CalHandle(0,fsingal,temp1);
//						
//					}
//				}break;
			default:    //ERR
//           		 return SetErr_ACK(buf, addr ,CMD_ERR);
				 return 0;
				  
		  }
		  break;
		}
		
	}
	if(j>=CmdNumb)
	{
//		return SetErr_ACK(buf, addr ,CMD_ERR);
		return 0;		
	}
	return currCharNum+1;
}




void Send_Main_Ord(void)
{
	uint8_t Send_buff[20]={0xAB,0X01,0x0C,0x06}; 
	uint8_t *pt;
	uint8_t i;
//	uint8_t dat,Sendflag=3;
	pt=(uint8_t *)&Send_Mainord;
	Send_Mainord.first.name=Disp_Main_Ord[SaveData.Main_Func.Param.test][0];
	Send_Mainord.first.Ordel=0X60;
	Send_Mainord.second.name=Disp_Main_Ord[SaveData.Main_Func.Param.test][1];
	Send_Mainord.second.Ordel=0X70;
	Send_Mainord.third.Ordel=0X71;
	Send_Mainord.third.name=Disp_Main_Ord[SaveData.Main_Func.Param.test][2];
	for(i=0;i<sizeof(Send_Mainord);i++)
	{
		Send_buff[4+i]=*(pt+i);
	
	}
//	Send_buff[3]=Send_Mainord.first.name;
//	Send_buff[4]=ord->name;
	Send_buff[10]=0X00;//У��� 
	Send_buff[11]=0XBF;
//	Send_buff[11]=0;
	UARTPuts( LPC_UART0, Send_buff);
//	sprintf((char *)DispBuf,"%2d",Send_Mainord.first.name);
//	WriteString_16(140, 4, DispBuf,  0);
//	
//	sprintf((char *)DispBuf,"%2d",Send_Mainord.second.name);
//	WriteString_16(170, 4, DispBuf,  0);
//	
//	sprintf((char *)DispBuf,"%2d",Send_Mainord.third.name);
//	WriteString_16(210, 4, DispBuf,  0);
	
}
void Disp_RangeDispvalue(uint8_t data)
{
	Colour.black=LCD_COLOR_TEST_BACK;
	WriteString_16(20, 200, RangeDisp_Main[data],  1);	//  Cp��ʾ
	WriteString_16(280, 200, RangeDisp_Second[data],  1);   //Rp��ʾ	
	Colour.black=LCD_COLOR_TEST_MID;
	LCD_ShowFontCN_40_55(80,92+30,40,55, (uint8_t*)Out_Assic+7*40*55/8);
	LCD_ShowFontCN_40_55(80+40,92+30,40,55, (uint8_t*)Out_Assic+24*40*55/8);
	LCD_ShowFontCN_40_55(80+40+20,92+30,40,55, (uint8_t*)Out_Assic+25*40*55/8);
//


}

//������ʾ��Cp�ĵ�λ����ϵ�λ��
void Disp_Range_DispMainUnit(uint8_t num,uint8_t unit)		//
{
    if(num>6)
        num=6;
    if(unit>6)
        unit=6;
	WriteString_16(128, 200, (uint8_t*)Disp_Unit[num],  1);//��λ1
	WriteString_16(138, 200, (uint8_t*)Disp_Range_Main_Disp[unit],  0);//��λ2

}

//������ʾ��Rp�ĵ�λ
void Disp_Range_DispSecondUnit(uint8_t num,uint8_t unit)
{
     if(num>6)
        num=6;
    if(unit>6)
        unit=6;
	WriteString_16(388, 200, (uint8_t*)Disp_Unit[num],  0);//��λ1
	WriteString_16(398, 200, (uint8_t*)Disp_Range_Main_Disp[unit],  0);//��λ2

}


//������ʾ�е�Cp��Rp��ֵ
void Disp_RangeTestvalue(void)
{
	//u8 i;
	const uint8_t DISP_UnitMain[]=
	{ 0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,3,3,2,2,2};
	const uint8_t DISP_UnitSecond[]=
	{ 6,6,3,2,6,6,2,6,2,2,6,6,6,2,2,5,4,5,4,2,6,6};
	Colour.black=LCD_COLOR_TEST_BACK;
	WriteString_16(52,200 ,Test_Dispvalue.Main_valuebuff,1);//
	WriteString_16(312,200 ,Test_Dispvalue.Secondvaluebuff,1);//
	Disp_Range_DispMainUnit(Test_Dispvalue.Unit[0],DISP_UnitMain[SaveData.Main_Func.Param.test]);
	Disp_Range_DispSecondUnit(Test_Dispvalue.Unit[1],DISP_UnitSecond[SaveData.Main_Func.Param.test]);
	//Count_buff
}
const uint8_t NUM_OUT[][4]=
{
	{"1  "},
	{"2  "},
	{"3  "},
	{"4  "},
	{"5  "},
	{"6  "},
	{"7  "},
	{"8  "},
	{"9  "},
	{"OUT"},
	{"0  "},



};
void Disp_Range_Num(uint8_t num)
{
	WriteString_16(300, 160, (uint8_t*)NUM_OUT[num],  0);


}
void Disp_RangeCount(void)
{	
	u8 i;	
	for(i=0;i<11;i++)	
	{
		if((i>1&&i<7)||((i>8)))
			Colour.black=LCD_COLOR_TEST_MID;
		else
			Colour.black=LCD_COLOR_TEST_BACK;
		Hex_Format(Count_buff[i] , 0, 6 , 0);//�ӵ�λ
				
		WriteString_12(LIST2+140, 58+i*15, DispBuf,  1);//��ʾ���ֵ  ����Ҫ������ʾ��λ
	}

}
void Disp_Set_Unit_16(uint32_t unit,uint32_t x,uint32_t y)//Disp_Range_Main_NUm
{
	WriteString_16(x, y, Disp_Range_Main_Disp[Disp_Range_Main_NUm[unit]],  0);


}
void Disp_Set_Unit_12(uint32_t unit,uint32_t x,uint32_t y)
{
	WriteString_12(x, y, Disp_Range_Main_Disp[unit],  1);


}
void Disp_Test_Unit_v(uint32_t x,uint32_t y)
{
	WriteString_16(x, y, "V",  1);

}
void Disp_Test_Unit_mA(uint32_t x,uint32_t y)
{
	WriteString_16(x, y, "mA",  1);

}
void Savetoeeprom(void)
{
	//Saveeeprom
	u8 *pt;
//	u8 i;
	
	
	pt=(u8*)&SaveSIM;
	
	EEPROM_Write(0, 0, pt, MODE_8_BIT, sizeof(SaveSIM));


}
void ReadComp(void)
{
	if(SaveSIM.JK5506 > 1)
	{
		SaveSIM.JK5506 = 0;
	}
}
void ReadSavedata(void)
{
	u8 i;
	SaveSet *pt;
	//Saveeeprom_Typedef 
	pt=&SaveSIM;
	EEPROM_Read(0, 0, (u8 *)pt, MODE_8_BIT, sizeof(SaveSIM));
	
}


void lcd_image(uint8_t *pt)
{
    unsigned long i;
	uint32_t color;
	unsigned long *pDst = (unsigned long *)LCD_VRAM_BASE_ADDR;
  
    for( i = 0; (C_GLCD_H_SIZE * C_GLCD_V_SIZE) > i; i++)
    {
		color=(uint32_t)*pt<<16|(uint32_t)*(pt+1)<<8|*(pt+2);
    	*pDst++ = color; 
		*pt++;
		*pt++;
		*pt++;
    }
}
void Send_Uart3(uint8_t *buff)
{
//	UARTPutChar (LPC_UART3,0xaa);
	UARTPuts(LPC_UART3,buff);
	


}
void Disp_Usbflag(uint8_t flag)
{
	Colour.black=LCD_COLOR_TEST_BUTON;
	if(flag==1)
		Colour.Fword=LCD_COLOR_GREEN;
		
	else if(flag==2)
		Colour.Fword=LCD_COLOR_BLUE;
	else
		Colour.Fword=LCD_COLOR_TEST_BUTON;
//	LCD_ShowFontCN_40_55(uint16_t x,uint16_t y,uint16_t xsize,uint16_t ysize,uint8_t * image);
	LCD_ShowFontCN_40_55(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE,BUTTOM_Y_VALUE,48,24,(uint8_t*)nBitmapDot);
	//WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, " ��ʾ",  0);
	

}

void Disp_switch(void)
{
	Colour.black=LCD_COLOR_TEST_BUTON;
	if(mainswitch == 0)
	{
		Colour.Fword=LCD_COLOR_BLUE;
	}else if(mainswitch == 1){
		Colour.Fword=LCD_COLOR_GREEN;
	}
	WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE+20, BUTTOM_Y_VALUE,User_Comp[mainswitch],  0);
}
