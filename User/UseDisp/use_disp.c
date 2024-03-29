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
uint8_t CorrectionflagC=0,CorrectionflagR=0,Correc_successflag=0;
const uint8_t Num_1[][9]=
{"1","2","3","4","5","6","7","8","9"};

char CmdStr[CmdNumb][CmdLen] =
{
	{"<0/RET_1"},        //1复位      1个字符
	{"<0/MOD_"},         //2 模式      1个字符   
	{"<0/LM_"},         //// 3 LM5116 1个字符 
	{"<0/HORL_"},           //4 切高低档位  1个字符   
	{"<0/SWITCH_"},         //5 总开关     1个字符  
	{"<0/CSV_?"},          //6 查询电压
	{"<0/CSI_?"},            //7 查询电流
	{"<0/ADCV_?"},          //8 查询ADC电压
	{"<0/ADCI_?"},          //9 查询ADC电流 
	{"<0/LVL_"},          // 10 设置电压
	{"<0/STATUS_?"},        //11 状态     1个字符 
	{"<0/CALV_"},        //12 测量电压校准     1个字符
	{"<0/CALI_"},        //13 测量电流校准     1个字符
	{"<0/CTRLV_"},        //14 控制电压校准     1个字符
	{"<0/CLEARI_"},        //15 控制电压校准     1个字符
	{"<0/OVER_"}, 			//16 保护信号   1个字符
};

char RecStr[CmdNumb][CmdLen] =
{
	{"STATUS_\0"},        //状态读取
	{"RET_\0"},           //复位      1个字符   
	{"CSV_\0"},           //读取CS1237电压的数据
	{"MOD_\0"},           //模式切换    1个字符  
	{"CSI_\0"},           //读取CS1237电流的数据 
	{"ADCI_\0"},          //ADC电流
	{"LM_\0"},            // 启动LM5116 1个字符 
	{"ADCV_\0"},          //ADC电压
	{"LOWI_\0"},          //被拉低时过流 1个字符 
	{"HORL_\0"},          //切高低档位  1个字符 
	{"LVL_\0"},           //DAC8562输出 A路 0~5V
	{"SWITCH_\0"},        //总开关     1个字符 
	{"CALV_\0"},        //13电压校准     1个字符 
	{"CALI_\0"},        //13电流校准     1个字符 
	{"OVER_\0"},        //13电流校准     1个字符 
};

const uint8_t Test_Setitem_E[][9+1]=
{
	{"Func    :"},
	{"Freq    :"},
	{"ELevel  :"},
	{"MRange  :"},
	{"Speed:"},
//    {"偏置    :"},
	
	{"Vm:"},
	{"Im:"},
	{"Corr:"}
	
};

const uint8_t Test_Setitem[][14+1]=
{
	{"电压设定  :"},
	{"充电保护  :"},
	{"放电保护  :"},
	{"快捷电压    :"},
	{"充电保护时间:"},
    {"放电保护时间:"},
	
//	{"Vm:"},
//	{"Im:"},
//	{"校正:"}
	
};

const uint8_t Fac_Calitem[][14+1]=
{
	{"0~5V  "},
	{"5~10V "},
	{"10~15V"},
	{"15~20V"},	
	{"      "},
	{"      "}
};

const uint8_t Fac_ICalitem[][14+1]=
{	
	{"10mA  "},
	{"5A    "},
	{"10A   "},
	{"-10mA "},
	{"-5A   "},
	{"-10A  "},
};

const uint8_t CtrV_Calitem[][14+1]=
{
	{"10V   "},
	{"20V   "},
	{"      "},
	{"      "},
	{"      "},
	{"      "}
};

const uint8_t Button_TipPage1[][7+1]=  //测试参数选择时候的下面的提示符号
{
    {"Cp-.."},
    {"Cs-.."},
    {"Lp-.."},
    {"Ls-.."},
    {" "},


};
const uint8_t Button_TipPage2[][7+1]=  //测试参数选择时候的下面的提示符号
{
    {"Z-.."},
    {"Y-.."},
    {"R-.."},
    {"G-B"},
    {" "},


};
const uint8_t Button_TipPage3[][7+1]=  //测试参数选择时候的下面的提示符号
{
    {"DCR"},
    {" "},
    {" "},
    {""},
    {" "},


};
const uint8_t Cp_Button_Tip[][7+1]=  //测试参数选择时候的下面的提示符号
{
    {"Cp-D"},
    {"Cp-Q"},
    {"Cp-G"},
    {"Cp-Rp"},
    {"RETURN"},


};
const uint8_t Cs_Button_Tip[][7+1]=  //测试参数选择时候的下面的提示符号
{
    {"Cs-D"},
    {"Cs-Q"},
    {"Cs-Rs"},
    {"    "},
    {"RETURN"},


};
const uint8_t Lp_Button_Tip1[][7+1]=  //测试参数选择时候的下面的提示符号
{
    {"Lp-Q"},
	{"Lp-Rp"},
    {"Lp-Rd"}, 
	{"Lp-D"},
    {"RETURN"},


};
const uint8_t Lp_Button_Tip2[][7+1]=  //测试参数选择时候的下面的提示符号
{
    {"Lp-D"},
    {"Lp-G"},
    {"    "},
    {"    "},
    {"RETURN"},


};
const uint8_t Ls_Button_Tip[][7+1]=  //测试参数选择时候的下面的提示符号
{
    {"Ls-D"},
    {"Ls-Q"},	
    {"Ls-Rs"},
    {"Ls-Rd"},
    {"RETURN"},


};
const uint8_t Z_Button_Tip[][7+1]=  //测试参数选择时候的下面的提示符号
{
    {"Z-d"},
    {"Z-r"},
    {" "},
    {" "},
    {"RETURN"},


};

const uint8_t Y_Button_Tip[][7+1]=  //测试参数选择时候的下面的提示符号
{
    {"Y-d"},
    {"Y-r"},
    {" "},
    {" "},
    {"RETURN"},


};
const uint8_t R_Button_Tip[][7+1]=  //测试参数选择时候的下面的提示符号
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
	"关闭",
	"合格",
	"不合格",

};
const uint8_t *Setup_Valueall[]=
{
	*User_FUNC,
	*User_Freq,



};
const uint8_t FreqButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
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
const uint8_t LevleButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"DECR -"},
    {"INCR +"},

};

const uint8_t User_Range[][10+1]=
{
    {"AUTO "},
    {"10 Ω "},
    {"100Ω "},
    {"1 kΩ "},
    {"10kΩ "},
    {"100kΩ"},
	{"HOLD"},

};
const uint8_t RangeButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
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
const uint8_t BiasButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
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
const uint8_t SpeedButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"FAST "},
    {"MED "},
    {"SLOW "},
};
const uint8_t User_Comp[][10+1]=
{
    
    {"OFF"},
	{"ON"},
   
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
const uint8_t CompButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"ON"},
	{"OFF "},
};
const uint8_t User_ListMode[][10+1]=
{
    {"SEQ"},
    {"STEP "},
   
};
const uint8_t ListModeButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
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
const uint8_t SetupTrig_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
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
const uint8_t SetupAlc_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Setup_Rsou[][10+1]=
{
    {"100Ω"},
    {"30Ω"},
   
};
const uint8_t SetupRsou_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"100Ω"},
    {"30Ω"},
};
const uint8_t SetupTrigDly_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
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
const uint8_t SetupRev_A_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
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
const uint8_t SetupRev_B_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"ABS"},
    {"%  "},
    {"OFF"},
};
const uint8_t SetupAvg_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"DECR -"},
    {"INCR +"},
};
const uint8_t Setup_Vm[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t SetupVm_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Setup_Dcr[][10+1]=
{
    {"FIX  "},
    {"ALT "},
   
};
const uint8_t SetupDcr_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"FIX  "},
    {"ALT "},
};
const uint8_t SetupRefa_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"MEASURE"},
};
const uint8_t SetupRefB_Button_Tip[][7+1]=  //频率选择时候的下面的提示符号
{
    {"MEASURE"},
};

const uint8_t Correct_Open[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t CorrectOpen_Button_Tip[][7+1]=  //比较界面选择时候的下面的提示符号
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Correct_Short[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t CorrectShort_Button_Tip[][7+1]=  //比较界面选择时候的下面的提示符号
{
    {"ON  "},
    {"OFF "},
};
const uint8_t Correct_Load[][10+1]=
{
    {"ON  "},
    {"OFF "},
   
};
const uint8_t CorrectLoad_Button_Tip[][7+1]=  //比较界面选择时候的下面的提示符号
{
    {"ON  "},
    {"OFF "},
};
const uint8_t CorrectSpot_Button_Tip[][7+1]=  //比较界面选择时候的下面的提示符号
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
const uint8_t CorrectFreq_Button_Tip[][7+1]=  //比较界面选择时候的下面的提示符号
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
const uint8_t CorrectCable_Button_Tip[][7+1]=  //比较界面选择时候的下面的提示符号
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
//const uint8_t ListNoButton_Tip[][7+1]=  //频率选择时候的下面的提示符号
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
	{"功能  :"},
	{"频率  :"},
	{"电平  :"},
	{"量程  :"},
//	{"偏置  :"},
    {"速度  :"},
    {"比较  :"},
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
//	{"功能    :"},
//	{"频率    :"},
//	{"电平    :"},
//	{"触发    :"},
//	{"恒电平  :"},
//	{"内阻    :"},
//	{"触发延时:"},
//	{"步进延时:"},
//	{"讯响功能:"},
//	{"量程    :"},
//	{"速度    :"},
//	{"偏置    :"},
//	{"平均    :"},
//	{"V/I监视 :"},
//	
//	{"DCR极性 :"},
//	{"DC量程  :"},
//	{"DC电平  :"},
//    {"波特率  :"},
//};

const uint8_t	Set_testitem[][11+1]=
{
	{"快捷电压1 :"},
	{"快捷电压2 :"},
	{"快捷电压3 :"},
	{"快捷电压4 :"},
	{"快捷电压5 :"},
	{"快捷电压6 :"},
//	{"触发延时"},
//	{"步进延时"},
//	{"讯响功能"},
//	{"量程    "},
//	{"速度    "},
////	{"偏置    "},
////	{"平均    "},
//	{"V/I监视 "},
//	
////	{"DCR极性 "},
////	{"DC量程  "},
////	{"DC电平  "},
//    {"波特率  "},
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
//	{"仪表功能 :"},
//	{"合格讯响 :"},
//	{"不良讯响 :"},
//	{"显示语言 :"},
//	{"口令     :"},
//	{"菜单保持 :"},
//	{"日期     :"},
//	{"波特率   :"},
//	{"偏置源   :"},
//	{"总线方式 :"},
//	{"GPIB地址 :"},
//	{"只讲     :"},
//	{"时间     :"}
//};


const uint8_t Sys_Setitem[][10+1]=
{
	{"仪表功能 "},
	{"按键声音 "},
	{"U盘开关 "},
	{"显示语言 "},
//	{"口令     "},
	{"菜单保持 "},
	{"日期     "},
//	{"密码     "},
	{"波特率   "},
	{"偏置源   "},
	{"总线方式 "},
	{"GPIB地址 "},
	{"只讲     "},
	{"时间     "}
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
	{"< 测量显示 >"},
	{"< 档号显示 >"},
	{"< 档计数显示 >"},
	{"< 列表扫描显示 >"},
	{"< 测量设置 >"},
	{"< 用户校正 >"},
	{"< 极限列表设置 >"},
	{"< 列表扫描设置 >"},
	{"< 系统设置 >"},
	{"[ LCR文件列表 ]"},
	{"< 校正设置 >"},

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
	{"参数 :"},
	{"标称 :"},
	{"计数 :"}

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
//	{"开路  :"},
//	{"短路  :"},
//	{"负载  :"},
//	{"校正点   :"},
//	{"频率     :"},
//	{" 参考   A:"},
//	{" 开路   A:"},
//	{" 短路   A:"},
//	{" 负载   A:"},
//	{"电缆  :"},
//	{"方式  :"},
//	{"功能  :"},
//	{"       "},
//	{"  B:"},
//	{"  B:"},
//	{"       "},
//	{"  B:"},
//	{"  B:"}

};


const uint8_t User_Check_Opencircuit[][7+1]=	//开路
{
	{"OFF"},
	{"ON"}
};
const uint8_t DEBUG_Check_Opencircuit[][7+1]=	//开路
{
	{"开始"},
	{"     "}
};
const uint8_t DEBUG_Check_Opencircuit_E[][7+1]=	//开路
{
	{"STAR"},
	{"     "}
};
const uint8_t User_Check_Shortcircuit[][7+1]=	//短路
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
 	{"参数  :"},
	{"标称  :"},
	{"方式  :"},
	{"附属  :"},
	{"比较  :"},
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
	{"模式 :"},
	{"频率[Hz]"},
    {"电平[V]"},
	{"LMT"},
	{"下限"},
	{"上限"},
    {"延时[s]"},

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
	{"中文"},
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
	"Ω",
	"S ",
	"r ",
	"°",
	"  "
};//F,H,Ω，S r °空格
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
	{0,7},//量程
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

const uint8_t Correction_Capacitance[][7+1]=			//校正电容
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

const uint8_t Correction_Resist[][7+1]=			//校正电阻
{
	{"10Ω"},
	{"100Ω"},
	{"1KΩ"},
	{"10KΩ"},
	{"100KΩ"},
};

//显示 下面的快捷键按键
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
//按键数字输入n,u,p,m,next,
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
	if(SaveData.Sys_Setup.Language == 1)
	{
		WriteString_16(83, 271-29, "MEASSET",  0);
		WriteString_16(83+80,271-29, "SYSSET",  0);
//		WriteString_16(92+80+80,271-29, "COUNT ",  0);
	}else if(SaveData.Sys_Setup.Language == 0){
		WriteString_16(83, 271-29, "测量设置",  0);
		WriteString_16(83+80,271-29, "系统设置",  0);
//		WriteString_16(92+80+80,271-29, "档计数",  0);
	}
	//	WriteString_16(83, 271-29, "测量设置",  0);
	  
	//	WriteString_16(83+80,271-29, "档号显示",  0);
	  
	//	WriteString_16(92+80+80,271-29, "档计数",  0);
	

}



void Disp_Button_value1(uint32_t value)
{
	Disp_Fastbutton();
	if(value==0)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		if(SaveData.Sys_Setup.Language == 1)
		{
			WriteString_16(83, 271-29, "MEASSET",  0);
			WriteString_16(83+80,271-29, "No.DISP",  0);
			WriteString_16(92+80+80,271-29, "COUNT ",  0);
		}else if(SaveData.Sys_Setup.Language == 0){
			WriteString_16(83, 271-29, "测量显示",  0);
			WriteString_16(83+80,271-29, "档号显示",  0);
			WriteString_16(92+80+80,271-29, "档计数",  0);
		}
//	    WriteString_16(64, 100, "测量",  0);
//		WriteString_16(84, 271-40, "测量",  0);
//		WriteString_16(84, 271-20, "显示",  0);
//		WriteString_16(84+80, 271-40, "档号",  0);
//		WriteString_16(84+80, 271-20, "显示",  0);
//		WriteString_16(84+80+80, 271-40, "档",  0);
//		WriteString_16(84+80+80, 271-20, "计数",  0);
//		WriteString_16(83, 271-29, "测量显示",  0);
//		WriteString_16(83+80,271-29, "档号显示",  0);
//		WriteString_16(92+80+80,271-29, "档计数",  0);
//		WriteString_16(83, 271-29, "MEASSET",  0);
//		WriteString_16(83+80,271-29, "No.DISP",  0);
//		WriteString_16(92+80+80,271-29, "COUNT ",  0);
//		WriteString_16(84+80+80+80, 271-40, "列表",  0);
//		WriteString_16(84+80+80+80, 271-20, "扫描",  0);
//		WriteString_16(84+80+80+80+80, 271-40, "更多",  0);
//		WriteString_16(84+80+80+80+80, 271-20, " 1/2",  0);
	}
    else if(value==1)
    {
        Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
//		WriteString_16(84, 271-40, "文件",  0);
//		WriteString_16(84, 271-20, "管理",  0);
//		WriteString_16(84+80, 271-40, "保存",  0);
//		WriteString_16(84+80, 271-20, "数据",  0);
//        WriteString_16(84+80+80+80+80, 271-40, "更多",  0);
//		WriteString_16(84+80+80+80+80, 271-20, "2/2",  0);
		
//			WriteString_16(83, 271-29, "文件管理",  0);
//			WriteString_16(83+80, 271-29, "保存数据",  0);
//			WriteString_16(84+80+80+80+80-1, 271-29, "更多",  0);
		if(SaveData.Sys_Setup.Language == 1)
		{
			WriteString_16(83, 271-29, "FILE SET",  0);
			WriteString_16(83+80, 271-29, "SaveData",  0);
			WriteString_16(84+80+80+80+80-1, 271-29, "MORE",  0);
		}else if(SaveData.Sys_Setup.Language == 0){
			WriteString_16(83, 271-29, "文件管理",  0);
			WriteString_16(83+80, 271-29, "保存数据",  0);
			WriteString_16(84+80+80+80+80-1, 271-29, "更多",  0);
		}
			WriteString_16(84+80+80+80+80+41, 271-29, "2/2",  0);
    }
    else
    {
        Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
//        WriteString_16(83, 271-29, "  取消 ",  0);
//		WriteString_16(83+80,271-29, "  确认 ",  0);
		if(SaveData.Sys_Setup.Language == 1)
		{
			 WriteString_16(83, 271-29, "CANCEL ",  0);
			WriteString_16(83+80,271-29, "CONFIRM",  0);
		}else if(SaveData.Sys_Setup.Language == 0){
			WriteString_16(83, 271-29, "  取消 ",  0);
			WriteString_16(83+80,271-29, "  确认 ",  0);
		}
		//WriteString_16(92+80+80,271-29, "档计数",  0);
    
    
    }

}
void Disp_Button_ItemScan_no(void)
{
	Disp_Fastbutton();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BUTON;

	if(SaveData.Sys_Setup.Language == 1)
	{
		WriteString_16(84, 271-40, "CLEA",  0);
		WriteString_16(84, 271-20, "LIST",  0);
	}else if(SaveData.Sys_Setup.Language == 0){
		WriteString_16(84, 271-40, "清除",  0);
		WriteString_16(84, 271-20, "表格",  0);
	}
	
//				WriteString_16(84+80, 271-40, "自动",  0);
//				WriteString_16(84+80, 271-20, "复制",  0);
//        WriteString_16(84+80+80, 271-30, "工具",  0);
//		WriteString_16(84+80+80, 271-20, "设置",  0);
//	WriteString_16(84+80+80+80, 271-30, "上一页",  0);
//	WriteString_16(84+80+80+80+80, 271-30, "下一页",  0);
	if(SaveData.Sys_Setup.Language == 1)
	{
		WriteString_16(84+80+80+80, 271-30, "Last P",  0);
		WriteString_16(84+80+80+80+80, 271-30, "Next P",  0);
	}else if(SaveData.Sys_Setup.Language == 0){
		WriteString_16(84+80+80+80, 271-30, "上一页",  0);
		WriteString_16(84+80+80+80+80, 271-30, "下一页",  0);
	}
	
}
void Disp_Button_ItemScanSet(uint32_t value)
{
	Disp_Fastbutton();
 	if(value==0||value==1)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
		if(SaveData.Sys_Setup.Language == 1)
		{
			WriteString_16(84, 271-30, "DelRow",  0);
			WriteString_16(84+80, 271-40, "LINE",  0);
			WriteString_16(84+80, 271-20, "ATOU",  0);
			WriteString_16(84+80+80, 271-40, "LOG ",  0);
			WriteString_16(84+80+80, 271-20, "ATOU",  0);
			WriteString_16(84+80+80+80, 271-30, "Last P",  0);
			WriteString_16(84+80+80+80+80, 271-30, "Next P",  0);
			
		}else if(SaveData.Sys_Setup.Language == 0){
			WriteString_16(84, 271-30, "删除行",  0);
	//		WriteString_16(84, 271-20, "设置",  0);
			WriteString_16(84+80, 271-40, "线性",  0);
			WriteString_16(84+80, 271-20, "自动",  0);
			WriteString_16(84+80+80, 271-40, "对数",  0);
			WriteString_16(84+80+80, 271-20, "自动",  0);
			WriteString_16(84+80+80+80, 271-30, "上一页",  0);
	//		WriteString_16(84+80+80+80, 271-20, "设置",  0);
			WriteString_16(84+80+80+80+80, 271-30, "下一页",  0);
		}
		//WriteString_16(84, 271-30, "删除行",  0);
//		WriteString_16(84, 271-20, "设置",  0);
		//WriteString_16(84+80, 271-40, "线性",  0);		
		//WriteString_16(84+80, 271-20, "自动",  0);
		
		
	//	WriteString_16(84+80+80, 271-20, "自动",  0);
		
//		WriteString_16(84+80+80+80, 271-20, "设置",  0);
		
//		WriteString_16(84+80+80+80+80, 271-20, " 1/2",  0);
	}
    else 
//		if(value==2)
//		{
//			Colour.Fword=White;
//				Colour.black=LCD_COLOR_TEST_BUTON;
//				WriteString_16(84, 271-40, "清除",  0);
//				WriteString_16(84, 271-20, "表格",  0);
////				WriteString_16(84+80, 271-40, "自动",  0);
////				WriteString_16(84+80, 271-20, "复制",  0);
//		//        WriteString_16(84+80+80, 271-30, "工具",  0);
//		//		WriteString_16(84+80+80, 271-20, "设置",  0);
//				WriteString_16(84+80+80+80, 271-30, "上一页",  0);
//				WriteString_16(84+80+80+80+80, 271-30, "下一页",  0);
//			
//		
//		}else
				
			{
				Colour.Fword=White;
				Colour.black=LCD_COLOR_TEST_BUTON;
				if(SaveData.Sys_Setup.Language == 1)
				{
					WriteString_16(84, 271-30, "DelRow",  0);
					WriteString_16(84+80, 271-40, "ATOU",  0);
					WriteString_16(84+80, 271-20, "COPY",  0);
					WriteString_16(84+80+80+80, 271-30, "Last P",  0);
					WriteString_16(84+80+80+80+80, 271-30, "Next P",  0);
					
				}else if(SaveData.Sys_Setup.Language == 0){
					WriteString_16(84, 271-30, "删除行",  0);
			//		WriteString_16(84, 271-20, "管理",  0);
					WriteString_16(84+80, 271-40, "自动",  0);
					WriteString_16(84+80, 271-20, "复制",  0);
			//        WriteString_16(84+80+80, 271-30, "工具",  0);
			//		WriteString_16(84+80+80, 271-20, "设置",  0);
					WriteString_16(84+80+80+80, 271-30, "上一页",  0);
					WriteString_16(84+80+80+80+80, 271-30, "下一页",  0);
				}
			//	WriteString_16(84, 271-30, "删除行",  0);
		//		WriteString_16(84, 271-20, "管理",  0);
			//	WriteString_16(84+80, 271-40, "自动",  0);
				
			//	WriteString_16(84+80, 271-20, "复制",  0);
				
		//        WriteString_16(84+80+80, 271-30, "工具",  0);
		//		WriteString_16(84+80+80, 271-20, "设置",  0);
				
			
			
			}


}
void Disp_Button_TestSet(uint32_t value)
{
	Disp_Fastbutton();
 	if(value==0)
	{
		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
//		WriteString_16(84, 271-40, "测量",  0);
//		WriteString_16(84, 271-20, "设置",  0);
//		WriteString_16(84+80, 271-40, "用户",  0);
//		WriteString_16(84+80, 271-20, "校正",  0);
//		WriteString_16(84+80+80, 271-40, "极限",  0);
//		WriteString_16(84+80+80, 271-20, "设置",  0);
		if(SaveData.Sys_Setup.Language == 1)
		{
			WriteString_16(83, 271-29, "MEADISP",  0);
			WriteString_16(83+80, 271-29, "CORRECT",  0);
			WriteString_16(83+80+80, 271-29, "LMTSET",  0);
			WriteString_16(83+80+80+80, 271-29, "SYSSET",  0);
			
		}else if(SaveData.Sys_Setup.Language == 0){
			WriteString_16(83, 271-29, "测量显示",  0);
			WriteString_16(83+80, 271-29, "用户校正",  0);
			WriteString_16(83+80+80, 271-29, "极限设置",  0);
			WriteString_16(83+80+80+80, 271-29, "系统设置",  0);
		}
		
		
		
//		WriteString_16(84+80+80+80, 271-40, "列表",  0);
//		WriteString_16(84+80+80+80, 271-20, "设置",  0);
//		WriteString_16(84+80+80+80+80, 271-40, "更多",  0);
//		WriteString_16(84+80+80+80+80, 271-20, " 1/2",  0);
	}
    else if(value==1)
    {
        Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
//		WriteString_16(84, 271-40, "文件",  0);
//		WriteString_16(84, 271-20, "管理",  0);
//		WriteString_16(84+80, 271-40, "系统",  0);
//		WriteString_16(84+80, 271-20, "设置",  0);
//        WriteString_16(84+80+80, 271-30, "工具",  0);
//		WriteString_16(84+80+80, 271-20, "设置",  0);
//        WriteString_16(84+80+80+80+80, 271-40, "更多",  0);
		if(SaveData.Sys_Setup.Language == 1)
		{
			WriteString_16(84, 271-40, "FILE",  0);
			WriteString_16(84, 271-20, "MANE",  0);
			WriteString_16(84+80, 271-40, "SYS ",  0);
			WriteString_16(84+80, 271-20, "SET ",  0);
			WriteString_16(84+80+80, 271-30, "TOOL",  0);
			WriteString_16(84+80+80, 271-20, "SET ",  0);
			WriteString_16(84+80+80+80+80, 271-40, "MORE",  0);
			WriteString_16(84+80+80+80+80, 271-20, " 2/2",  0);  
			
		}else if(SaveData.Sys_Setup.Language == 0){
			WriteString_16(84, 271-40, "文件",  0);
			WriteString_16(84, 271-20, "管理",  0);
			WriteString_16(84+80, 271-40, "系统",  0);
			WriteString_16(84+80, 271-20, "设置",  0);
			WriteString_16(84+80+80, 271-30, "工具",  0);
			WriteString_16(84+80+80, 271-20, "设置",  0);
			WriteString_16(84+80+80+80+80, 271-40, "更多",  0);
			WriteString_16(84+80+80+80+80, 271-20, " 2/2",  0);  
		}
		  
    
    }
    else
    {
        Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;
//        WriteString_16(83, 271-29, "电容校正",  0);
//		WriteString_16(83+80, 271-29, "电阻校正",  0);
//		WriteString_16(83+80+80, 271-29, "退出保存",  0);
//		WriteString_16(83+80+80+80, 271-29, "清除校正",  0);
		if(SaveData.Sys_Setup.Language == 1)
		{
			WriteString_16(83, 271-29, "VOLCal",  0);
			WriteString_16(83+80, 271-29, "CtrlCal",  0);
			WriteString_16(83+80+80, 271-29, "CurCal",  0);
//			WriteString_16(83+80+80+80, 271-29, "Cle Corr",  0); 
			
		}else if(SaveData.Sys_Setup.Language == 0){
			WriteString_16(83, 271-29, "电压校正",  0);
			WriteString_16(83+80, 271-29, "控制校正",  0);
			WriteString_16(83+80+80, 271-29, "电流校正",  0);
//			WriteString_16(83+80+80+80, 271-29, "清除校正",  0); 
		}
     
    }

}

void Disp_Button_SysSet(void)
{

	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BUTON;
		
//		WriteString_16(83, 271-29, "密码设置",  0);
	//	WriteString_16(83, 271-29, "测量设置",  0);
	if(SaveData.Sys_Setup.Language == 1)
	{
		WriteString_16(83, 271-29, "MeasSet",  0);
		
	}else if(SaveData.Sys_Setup.Language == 0){
		WriteString_16(83, 271-29, "测量设置",  0); 
	}
	
//		WriteString_16(83, 271-29, "系统",  0);
//		WriteString_16(83, 271-29, "设置",  0);
		
//		WriteString_16(83+80, 271-29, "设置",  0);
//		WriteString_16(84+80+80, 271-40, "极限",  0);
//		WriteString_16(84+80+80, 271-20, "设置",  0);
//		WriteString_16(84+80+80+80, 271-40, "默认",  0);
//		WriteString_16(84+80+80+80, 271-20, "设置",  0);
//		WriteString_16(84+80+80+80+80, 271-40, "系统",  0);
//		WriteString_16(84+80+80+80+80, 271-20, "复位",  0);


}

void Disp_Button_Correction(void)
{

		Colour.Fword=White;
		Colour.black=LCD_COLOR_TEST_BUTON;	
//		WriteString_16(83, 271-29, "电容校正",  0);
//		WriteString_16(83+80, 271-29, "电阻校正",  0);
//		WriteString_16(83+160+19, 271-29, "退出",  0);
//		WriteString_16(83+160+160, 271-29, "清除校正",  0);
	if(SaveData.Sys_Setup.Language == 1)
	{
		WriteString_16(83, 271-29, "C Corret",  0);
		WriteString_16(83+80, 271-29, "R Corret",  0);
		WriteString_16(83+160+19, 271-29, "Exit",  0);
		WriteString_16(83+160+160, 271-29, "Cle Corr",  0);
		
	}else if(SaveData.Sys_Setup.Language == 0){
		WriteString_16(83, 271-29, "电容校正",  0);
		WriteString_16(83+80, 271-29, "电阻校正",  0);
		WriteString_16(83+160+19, 271-29, "退出",  0);
		WriteString_16(83+160+160, 271-29, "清除校正",  0);
	}
	
//		WriteString_16(83+80+19, 271-29, "确定",  0);
}
void Disp_TopBar_Color(void)	//主菜单的底色橘色
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
	Disp_Fastbutton();//显示快速按键
	Disp_MidRect();//显示中间色块
}
//
//测量显示主菜单的项目显示
//
void Disp_Test_Item(void)
{
	uint32_t i;
	Disp_TestScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveData.Sys_Setup.Language == 1)
	{
		WriteString_16(0,4, All_TopName_E[0],  0);
		
	}else if(SaveData.Sys_Setup.Language == 0){
		WriteString_16(0,4, All_TopName[0],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	for(i=0;i<6;i++)
	{
		if(i<3)
		{
			if(SaveData.Sys_Setup.Language == 1)
			{
				WriteString_16(0, 26+i*22, Test_Setitem_E[i],  0);
				
			}else if(SaveData.Sys_Setup.Language == 0){
				WriteString_16(0, 26+i*22, Test_Setitem[i],  0);
			}
		}else{
			if(SaveData.Sys_Setup.Language == 1)
			{
				WriteString_16(250,26+(i-3)*22, Test_Setitem_E[i],  0);
				
			}else if(SaveData.Sys_Setup.Language == 0){
				WriteString_16(250,26+(i-3)*22, Test_Setitem[i],  0);
			}
			
		}

		
	}
	Disp_Button_Test();
	

}
//档号显示子函数
void Disp_Range_MidRect(void)
{

	LCD_DrawRect( 0, 160,479 , 190 , LCD_COLOR_TEST_MID ) ;
}
void Disp_RangeScreen(void)
{
//	Disp_TopBar_Color();
	LCD_DrawRect( 0, 0, 160, 22 , LCD_COLOR_TEST_BAR);
	Disp_Fastbutton();//显示快速按键
	Disp_Range_MidRect();//显示中间色块
}
//档号显示主菜单项目显示
void Disp_Range_Item(void)
{
	uint32_t i;
	Disp_RangeScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveData.Sys_Setup.Language == 1)
	{
		WriteString_16(0, 4, All_TopName_E[1],  0);
		
	}else if(SaveData.Sys_Setup.Language == 0){
		WriteString_16(0, 4, All_TopName[1],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	for(i=0;i<6;i++)
	{
		if(i<3)
		{
			if(SaveData.Sys_Setup.Language == 1)
			{
				WriteString_16(0, 26+i*22, Range_Item_E[i],  0);
				
			}else if(SaveData.Sys_Setup.Language == 0){
				WriteString_16(0, 26+i*22, Range_Item[i],  0);
			}
			
		}else{
			if(SaveData.Sys_Setup.Language == 1)
			{
				WriteString_16(250, 26+(i-3)*22, Range_Item_E[i],  0);
				
			}else if(SaveData.Sys_Setup.Language == 0){
				WriteString_16(250, 26+(i-3)*22, Range_Item[i],  0);
			}
			
		}
//		WriteString_16(0, 210, "R :",  0);
//		WriteString_16(120, 210, Test_Setitem[7],  0);
		
	
	}
	Disp_RangeDispvalue(SaveData.Main_Func.Param.test);
	
	Disp_Button_value1(0);
	

}
//档计数显示子函数

void Disp_Rang_Count_MidRect(void)
{

	LCD_DrawRect( 0, 86,479 , 160 , LCD_COLOR_TEST_MID ) ;		//蓝色
	LCD_DrawRect( 0, 193,479 , 227 , LCD_COLOR_TEST_MID ) ;
}

void Disp_Range_CountScreen(void)
{
//	Disp_TopBar_Color();
	LCD_DrawRect( 0, 0, 178, 22 , LCD_COLOR_TEST_BAR);
	Disp_Fastbutton();//显示快速按键
	Disp_Rang_Count_MidRect();//显示中间色块
}
//档计数显示界面
void Disp_Range_Count_Item(void)
{
	uint32_t i;
	Disp_Range_CountScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveData.Sys_Setup.Language == 1)
	{
		WriteString_16(0, 4, All_TopName_E[2],  0);	
	}else if(SaveData.Sys_Setup.Language == 0){
		WriteString_16(0, 4, All_TopName[2],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	for(i=0;i<3;i++)
	{
		if(i==1)
		{
			if(SaveData.Sys_Setup.Language == 1)
			{
				WriteString_16(i*160+30, FIRSTLINE-2, Range_Count_Item_E[i],  0);	
			}else if(SaveData.Sys_Setup.Language == 0){
				WriteString_16(i*160+30, FIRSTLINE-2, Range_Count_Item[i],  0);
			}		
		}else if(i==2){
			if(SaveData.Sys_Setup.Language == 1)
			{
				WriteString_16(i*160+40, FIRSTLINE-2, Range_Count_Item_E[i],  0);	
			}else if(SaveData.Sys_Setup.Language == 0){
				WriteString_16(i*160+40, FIRSTLINE-2, Range_Count_Item[i],  0);
			}			
		}else{
			if(SaveData.Sys_Setup.Language == 1)
			{
				WriteString_16(i*160, FIRSTLINE-2, Range_Count_Item_E[i],  0);	
			}else if(SaveData.Sys_Setup.Language == 0){
				WriteString_16(i*160, FIRSTLINE-2, Range_Count_Item[i],  0);
			}			
		}
//		WriteString_16(0, 210, "R :",  0);
//		WriteString_16(120, 210, Test_Setitem[7],  0);//WriteString_12
		
	
	}
	WriteString_16(LIST1+60, FIRSTLINE-2, User_Freq[SaveData.Main_Func.Freq],  1);//显示参数
	Hex_Format(SaveData.Limit_Tab.Nom.Num , SaveData.Limit_Tab.Nom.Dot , 6 , 0);
	WriteString_16(LIST2, FIRSTLINE-2, DispBuf,  1);//显示标称值  后面要接着显示单位
	WriteString_16(LIST2+70, FIRSTLINE-2, Disp_Unit[SaveData.Limit_Tab.Nom.Unit],  1);
	Disp_Set_Unit_16(SaveData.Main_Func.Param.test,SELECT_2END-112,FIRSTLINE-2 );//显示单位
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
			SaveData.Limit_Tab.Comp_Value[i-6].low.Dot , 6 , 1);//加单位
			//SaveData.Limit_Tab.Comp_Value[Button_Page.index-16].high
			WriteString_12(LIST2-130, 58+(i-6)*15, DispBuf,  1);//显示标称值  后面要接着显示单位
		}
		else
		WriteString_12(LIST2-130, 58+(i-6)*15, "------",  1);	
	}
	Colour.black=LCD_COLOR_TEST_MID;
	if(SaveData.Limit_Tab.Comp_Value[9].high.Num!=0)
	{
		Hex_Format(SaveData.Limit_Tab.Comp_Value[9].high.Num, 
			SaveData.Limit_Tab.Comp_Value[9].high.Dot , 6 , 1);//加单位//加单位
		
		WriteString_12(LIST2, 196, DispBuf,  1);//显示标称值  后面要接着显示单位
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
			SaveData.Limit_Tab.Comp_Value[i-16].high.Dot , 6 , 1);//加单位
			
			WriteString_12(LIST2, 58+(i-16)*15, DispBuf,  1);//显示标称值  后面要接着显示单位
		}
		else
			WriteString_12(LIST2, 58+(i-16)*15, "------",  1);
	}
	Colour.black=LCD_COLOR_TEST_MID;
	if(SaveData.Limit_Tab.Comp_Value[9].high.Num!=0)
		{
			Hex_Format(SaveData.Limit_Tab.Comp_Value[9].low.Num, 
			SaveData.Limit_Tab.Comp_Value[9].low.Dot , 6 , 1);//加单位
		
		WriteString_12(LIST2-130, 196, DispBuf,  1);//显示标称值  后面要接着显示单位
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
//列表显示子函数
void Disp_List_Count_MidRect(void)
{

	LCD_DrawRect( 0, 175,479 , 160 , LCD_COLOR_TEST_MID ) ;
//	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
}

void Disp_List_CountScreen(void)
{
	Disp_TopBar_Color();
	Disp_Fastbutton();//显示快速按键
	Disp_List_Count_MidRect();//显示中间色块
}
//列表显示界面
void Disp_List_Count_Item(void)
{
	Disp_List_CountScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	WriteString_16(0, 4, All_TopName_E[3],  0);
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;

//	WriteString_16(LIST1, FIRSTLINE, "方式 :",  0);
//	WriteString_16(LIST1+40, FIRSTLINE+SPACE1, "频率[Hz]",  0);	
//	WriteString_16(LIST1+140, FIRSTLINE+SPACE1, "电平[V]",  0);	
	if(SaveData.Sys_Setup.Language == 1)
	{
		WriteString_16(LIST1, FIRSTLINE, "Meth :",  0);
		WriteString_16(LIST1+40, FIRSTLINE+SPACE1, "Freq[Hz]",  0);	
		WriteString_16(LIST1+140, FIRSTLINE+SPACE1, "Levl[V]",  0);		
	}else if(SaveData.Sys_Setup.Language == 0){
		WriteString_16(LIST1, FIRSTLINE, "方式 :",  0);
		WriteString_16(LIST1+40, FIRSTLINE+SPACE1, "频率[Hz]",  0);	
		WriteString_16(LIST1+140, FIRSTLINE+SPACE1, "电平[V]",  0);	
	}	
	
	WriteString_16(LIST1+140+100, FIRSTLINE+SPACE1, "Cp[F]",  0);
	WriteString_16(LIST1+140+100+100, FIRSTLINE+SPACE1, "D[ ]",  0);
	WriteString_16(LIST1+440, FIRSTLINE+SPACE1, "CMP",  0);
	Disp_Button_value1(0);

}

//列表显示子函数
void Disp_TestSet_MidRect(void)
{

	LCD_DrawRect( 0, 90,479 , 154 , LCD_COLOR_TEST_MID ) ;
//	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
}

void Disp_TestSetScreen(void)
{
	LCD_DrawRect( 0, 0, 168, 22 , LCD_COLOR_TEST_BAR);
//	Disp_TopBar_Color();
	Disp_Fastbutton();//显示快速按键
	Disp_TestSet_MidRect();//显示中间色块
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



//列表显示界面	   //  测量设置界面显示菜单的项
void Disp_Test_Set_Item(void)
{
	uint32_t i;
	Disp_TestSetScreen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveData.Sys_Setup.Language ==1)
	{
		WriteString_16(0, 4, All_TopName_E[4],  0);
	}else if(SaveData.Sys_Setup.Language ==0){
		WriteString_16(0, 4, All_TopName[4],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	for(i=0;i<(sizeof(Set_testitem_E)/(sizeof(Set_testitem_E[0])));i++)
	if(SaveData.Sys_Setup.Language == 1)
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
	}else if(SaveData.Sys_Setup.Language == 0){
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
//测试时档号显示界面的显示
void Disp_RangeDispValue(Button_Page_Typedef *Button_Page)
{
//	uint32_t flag;
	uint32_t i;
//	uint32_t xpose;
	uint32_t Black_Select;
//	uint32_t Select_color;

//功能	
	
	Colour.black=LCD_COLOR_TEST_BACK;	
	WriteString_16(LIST1+73, FIRSTLINE, User_FUNC[SaveData.Main_Func.Param.test],  1);//增加算法  把顺序改过来
	
//频率	
	WriteString_16(LIST1+73, FIRSTLINE+SPACE1, User_Freq[SaveData.Main_Func.Freq],  1);
	
	
//电平	

	WriteString_16(LIST1+73, FIRSTLINE+SPACE1*2, User_Level[SaveData.Main_Func.Level],  1);
//量程

//	if(SaveData.Main_Func.Range.Auto)
//		WriteString_16(LIST2+88, FIRSTLINE, User_Range[0],  0);
//	else
//	{
		WriteString_16(LIST2+73, FIRSTLINE, User_Range[SaveData.Main_Func.Range.Range],  0);
	
//	}
	
//速度
	
	WriteString_16(LIST2+73, FIRSTLINE+SPACE1, User_Speed[SaveData.Main_Func.Speed],  1);
	
//偏置

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
//测试时档号显示界面的显示        档计数显示ON 或 OFF
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

//测试时，显示列表扫描的测试值，也就是从串口接收过来的值
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
////		WriteString_16(LIST1+88, FIRSTLINE, User_FUNC[SaveData.Main_Func.Param.test],  1);//增加算法  把顺序改过来
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
//测试时，显示列表扫描的测试值，也就是从串口接收过来的值
void Disp_Scan_SetCompvalue(Button_Page_Typedef* Button_Page)
{
	uint8_t i;
	if(Button_Page->index==2)
		Disp_Button_ItemScan_no();
	else
		if(Button_Page->index>2)
		Disp_Button_ItemScanSet(Button_Page->third);
	if(Button_Page->index==1&&Button_Page->third==0)//第一个值
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	LCD_DrawRect( 54, FIRSTLINE,54+40 , FIRSTLINE+16 , Colour.black ) ;	
	WriteString_16(54,FIRSTLINE, User_FUNC[SaveData.Sys_Setup.Main_Func],  0);
	
	if(Button_Page->index==2&&Button_Page->third==0)//第二个值
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
		
		if(Button_Page->index==i&&Button_Page->third==2)//第三列
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
		if(Button_Page->index==i&&Button_Page->third==3)//第四列
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
		if(Button_Page->index==i&&Button_Page->third==4)//第五列
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
			
			if(Button_Page->index==i&&Button_Page->third==0)//第一列
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
			if(Button_Page->index==i&&Button_Page->third==1)//第二列
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
//测试时，列表扫描显示值
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
//测试的时候显示的设置值     // 测量显示中的设置值
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
//电压设置	
	Black_Select=(Button_Page->index==1)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
	//if()
	Hex_Format(SaveSIM.Voltage.Num,3,5,0);
	LCD_DrawRect( LIST1+87+16,FIRSTLINE-2,SELECT_1END ,FIRSTLINE+SPACE1-4 , Colour.black ) ;//SPACE1
	WriteString_16(LIST1+88+16,FIRSTLINE, DispBuf,  0);//增加算法  把顺序改过来
	WriteString_16(LIST1+88+16+70, FIRSTLINE, "V",  1);
//	WriteString_16(LIST2-88, SCREENHIGH-FIRSTLINE, User_Range[SaveData.Main_Func.Range.Range],  0);
//充电保护电流	
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
	WriteString_16(LIST1+88+16,FIRSTLINE+SPACE1, DispBuf,  0);//增加算法  把顺序改过来
	WriteString_16(LIST1+88+16+70, FIRSTLINE+SPACE1, "A",  1);
	
	
//放电保护电流	
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
	WriteString_16(LIST1+88+16,FIRSTLINE+SPACE1*2, DispBuf,  0);//增加算法  把顺序改过来
	WriteString_16(LIST1+88+16+70, FIRSTLINE+SPACE1*2, "A",  1);
//快捷电压
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
	WriteString_16(LIST2+88+32,FIRSTLINE, DispBuf,  0);//增加算法  把顺序改过来
	WriteString_16(LIST2+88+32+70, FIRSTLINE, "V",  1);
	
//充电保护时间
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
	WriteString_16(LIST2+88+32,FIRSTLINE+SPACE1, DispBuf,  0);//增加算法  把顺序改过来
	WriteString_16(LIST2+88+32+60, FIRSTLINE+SPACE1, "ms",  1);
//	WriteString_16(LIST2+88+32,FIRSTLINE+SPACE1, User_Speed[SaveData.Main_Func.Speed],  1);
	
//放电保护时间
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
	WriteString_16(LIST2+88+32,FIRSTLINE+SPACE1*2, DispBuf,  0);//增加算法  把顺序改过来
	WriteString_16(LIST2+88+32+60, FIRSTLINE+SPACE1*2, "ms",  1);
//	WriteString_16(LIST2+88+32,FIRSTLINE+SPACE1*2, User_Speed[SaveData.Main_Func.Speed],  1);
//偏置
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
			Disp_Button_Test();
			//	Disp_Button_value1(Button_Page->page);
			break;
		case 1:				//显示功能项的按键值
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
			}else if(Button_Page->page == 1){
				for(i=0;i<3;i++)
				{
					Hex_Format(SaveSIM.QuickV[i+(Button_Page->page)*3].Num,3,5,0);
					WriteString_16(BUTTOM_X_VALUE+i*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,DispBuf,  0);
				}
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
//显示设置参数的值Setup_Valueall
void DispSet_value(Button_Page_Typedef* Button_Page)
{
	uint32_t i;
	uint32_t Black_Select;

//快捷电压1	
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
	WriteString_16(LIST1+88+16,FIRSTLINE, DispBuf,  0);//增加算法  把顺序改过来
	WriteString_16(LIST1+88+16+70, FIRSTLINE, "V",  1);

//快捷电压2	
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
	WriteString_16(LIST1+88+16,FIRSTLINE+SPACE1, DispBuf,  0);//增加算法  把顺序改过来
	WriteString_16(LIST1+88+16+70, FIRSTLINE+SPACE1, "V",  1);
	
	
//快捷电压3	
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
	WriteString_16(LIST1+88+16,FIRSTLINE+SPACE1*2, DispBuf,  0);//增加算法  把顺序改过来
	WriteString_16(LIST1+88+16+70, FIRSTLINE+SPACE1*2, "V",  1);
//快捷电压4
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
	WriteString_16(LIST2+88+16,FIRSTLINE, DispBuf,  0);//增加算法  把顺序改过来
	WriteString_16(LIST2+88+16+70, FIRSTLINE, "V",  1);
	
//快捷电压5
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
	WriteString_16(LIST2+88+16,FIRSTLINE+SPACE1, DispBuf,  0);//增加算法  把顺序改过来
	WriteString_16(LIST2+88+16+70, FIRSTLINE+SPACE1, "V",  1);
//	WriteString_16(LIST2+88+32,FIRSTLINE+SPACE1, User_Speed[SaveData.Main_Func.Speed],  1);
	
//快捷电压6
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
	WriteString_16(LIST2+88+16,FIRSTLINE+SPACE1*2, DispBuf,  0);//增加算法  把顺序改过来
	WriteString_16(LIST2+88+16+70, FIRSTLINE+SPACE1*2, "V",  1);
	

	Disp_Fastbutton();
	switch(Button_Page->index)
	{
		case 0:
				Disp_Button_TestSet(Button_Page->page);
			break;
		
	}
}




//极限设置按键值
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

	if(SaveData.Limit_Tab.Mode==0)//上下限
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
	WriteString_16(LIST1+70, FIRSTLINE-2, User_FUNC[SaveData.Main_Func.Param.test],  1);//增加算法  把顺序改过来
	
//标称
	Black_Select=(Button_Page->index==2)?1:0;
	if(Black_Select)
	{
		Colour.black=LCD_COLOR_SELECT;
	
	}
	else
	{
		Colour.black=LCD_COLOR_TEST_BACK;
	}
		
	//WriteString_16(LIST1+60, FIRSTLINE, User_Freq[SaveData.Main_Func.Freq],  1);//显示参数
	Hex_Format(SaveData.Limit_Tab.Nom.Num , SaveData.Limit_Tab.Nom.Dot , 5 , 0);//加单位
	LCD_DrawRect( LIST2-24, FIRSTLINE-2,SELECT_2END-123+6 , FIRSTLINE+16, Colour.black ) ;
	WriteString_16(LIST2-21, FIRSTLINE-2, DispBuf,  1);//显示标称值  后面要接着显示单位
	WriteString_16(LIST2+47, FIRSTLINE-2, Disp_Unit[SaveData.Limit_Tab.Nom.Unit],  1);
	Disp_Set_Unit_16(SaveData.Main_Func.Param.test,SELECT_2END-135,FIRSTLINE-2 );
	
	
//方式
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
//附属
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
	
//比较
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
			SaveData.Limit_Tab.Comp_Value[i-6].low.Dot , 5 , 0);//加单位
			
//			WriteString_12(LIST2-90-8, 76+(i-6)*15, "-",  1);
			WriteString_12(LIST2-90, 76+(i-6)*15, DispBuf,  1);//显示标称值  后面要接着显示单位
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
				SaveData.Limit_Tab.Comp_Value[i-16].high.Dot , 5 , 0);//加单位
			
			WriteString_12(LIST2+80, 76+(i-16)*15, DispBuf,  0);//显示标称值  后面要接着显示单位
			if(SaveData.Limit_Tab.Mode==0)
			WriteString_12(LIST2+70+58, 76+(i-16)*15, 
			Disp_Unit[SaveData.Limit_Tab.Comp_Value[i-16].high.Unit],  1);
		}
		else
			WriteString_12(LIST2+80, 76+(i-16)*15, "------",  1);
	}
//副参数分选
	if(Button_Page->index == 15)
			Colour.black=LCD_COLOR_SELECT;
		else
			Colour.black=LCD_COLOR_TEST_BACK;
	LCD_DrawRect( LIST2-90-12, 76+(15-6)*15,LIST2-20 , 77+(15-6)*15+15 , Colour.black ) ;
//	if(SaveData.Limit_Tab.Comp_Value[15-6].low.Num!=0)
//	{
		Hex_Format(SaveData.Limit_Tab.Comp_Value[15-6].low.Num , 
		SaveData.Limit_Tab.Comp_Value[15-6].low.Dot , 5 , 0);//加单位
		
//			WriteString_12(LIST2-90-8, 76+(i-6)*15, "-",  1);
		WriteString_12(LIST2-90, 76+(15-6)*15, DispBuf,  1);//显示标称值  后面要接着显示单位
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
			SaveData.Limit_Tab.Comp_Value[25-16].high.Dot , 5 , 0);//加单位
		
		WriteString_12(LIST2+80, 76+(25-16)*15, DispBuf,  0);//显示标称值  后面要接着显示单位
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
			if(SaveData.Sys_Setup.Language==1)
			{
				WriteString_16(BUTTOM_X_VALUE+9, BUTTOM_Y_VALUE, "Cel R",  0);
				WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE+1, "CelList",  0);
			}else if(SaveData.Sys_Setup.Language==0){
				WriteString_16(BUTTOM_X_VALUE+9, BUTTOM_Y_VALUE, "清除行",  0);
				WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE+1, "清除表格",  0);
			}
			
//			WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE+10, "表格",  0);
		break;
		
			

		
			
	
	
	}

}
//显示设置参数的值Setup_Valueall   //用户校正中的按键值设置
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
	WriteString_16(LIST1+88, FIRSTLINE, Switch_Value[SaveData.User_Correction.Open],  1);//开路
	
//短路
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
	
	
////负载
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
////校正点
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
////频率
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
//	if(SaveData.User_Correction.Freq==1)//关
//		WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*4, "1.00000KHz",  1);
//	else
//		WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*4, "----------",  1);
//		
////参考A
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
//		//显示单位
//	}
//	else
//	{
//		WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*5,"----------",  0);
//	}
//	
////开路A
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
////短路A
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
////负载A
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
////电缆
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
////方式
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
////功能
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
//显示设置参数的值Setup_Valueall		//系统设置中的设置项
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
	if(SaveData.Sys_Setup.Language==1)
	{
		WriteString_16(LIST1+95, FIRSTLINE+2, "Brid",  0);//
	}else if(SaveData.Sys_Setup.Language==0){
		WriteString_16(LIST1+95, FIRSTLINE+2, "电桥",  0);//
	}
	
	
//合格讯响
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
	WriteString_16(LIST1+95, FIRSTLINE+SPACE1+2, Switch_Value[Saveeeprom.Sys_set.key_board],  1);
	
	
//不合格讯响
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
	
//显示语言
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
	WriteString_16(LIST1+95, FIRSTLINE+SPACE1*3+2, Sys_Language_Value[SaveData.Sys_Setup.Language],  0);	
	
//口令
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
//	if(SaveData.Sys_Setup.Password==1)//关
//		WriteString_16(LIST1+95, FIRSTLINE+SPACE1*4+2, "关闭",  0);
//	else
//		WriteString_16(LIST1+95, FIRSTLINE+SPACE1*4, "----------",  1);
		
//菜单保持
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
		WriteString_16(LIST1+95, FIRSTLINE+SPACE1*4+2,"HOLD",  0);
		//显示单位
//	}
//	else
//	{
//		WriteString_16(LIST1+88+4, FIRSTLINE+SPACE1*5,"----------",  0);
//	}
	
//年
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
//月
	
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
	
	
	//日
	
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
    WriteString_16(LIST1+94, FIRSTLINE+SPACE1*6+2, Saveeeprom.fac_num,  0);
    //Saveeeprom.fac_num;
	
//密码
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

	

	
//波特率
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
	
//偏置源
	
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
	
	//总线方式
	
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
	
//GPIB地址
	
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
	

//只讲
	
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


//时
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
//分
	
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
	
	
	//秒
	
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
				Colour.Fword=White;
				Colour.black=LCD_COLOR_TEST_BUTON;
//				for(i=0;i<4;i++)
			//	WriteString_16(BUTTOM_X_VALUE+18, BUTTOM_Y_VALUE, "电桥",  0);
		if(SaveData.Sys_Setup.Language ==1)
		{
			WriteString_16(BUTTOM_X_VALUE+18, BUTTOM_Y_VALUE, "Brid",  0);
		}else if(SaveData.Sys_Setup.Language ==0){
			WriteString_16(BUTTOM_X_VALUE+18, BUTTOM_Y_VALUE, "电桥",  0);
		}
        
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
			//for(i=0;i<2;i++)
			{
				
				WriteString_16(BUTTOM_X_VALUE+14, BUTTOM_Y_VALUE, "HOLD",  0);
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
//用户校正界面子函数
void Disp_Usercheck_Screen(void)
{
	
	LCD_DrawRect( 0, 0, 150, 22 , LCD_COLOR_TEST_BAR);
//	Disp_TopBar_Color();
	Disp_Fastbutton();//显示快速按键
//	Disp_TestSet_MidRect();//显示中间色块
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
		for(i=0;i<2;i++)
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
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BUTON;
			WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE+4, BUTTOM_Y_VALUE,"校正",  0);
		break;
	}
}


void Disp_Fac_Debug_R(Button_Page_Typedef* Button_Page)//电阻校正界面
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
////校正点
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
////频率
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
void Disp_Fac_Debug_C(Button_Page_Typedef* Button_Page)//电容校正界面
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
////校正点
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
////频率
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
////参考A
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
////开路A
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
////短路A
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
	if(SaveData.Sys_Setup.Language ==1)
	{
		WriteString_16(0, 4, All_TopName_E[5],  0);
	}else if(SaveData.Sys_Setup.Language ==0){
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
//极限列表设置子函数
void Disp_LimitList_MidRect(void)
{

	LCD_DrawRect( 0, 185,479 , 150+32 , LCD_COLOR_TEST_MID ) ;
//	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
}
void Disp_LimitList_Screen(void)
{
	Disp_TopBar_Color();
	Disp_Fastbutton();//显示快速按键
//	Disp_LimitList_MidRect();//显示中间色块	 User_LimitList_Item
}

void Disp_LimitList_Item(void)			//极限列表设置
{
	uint32_t i;
	Disp_LimitList_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveData.Sys_Setup.Language ==1)
	{
		WriteString_16(0, 4, All_TopName_E[6],  0);
	}else if(SaveData.Sys_Setup.Language ==0){
		WriteString_16(0, 4, All_TopName[6],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	if(SaveData.Sys_Setup.Language ==1)
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
	}else if(SaveData.Sys_Setup.Language ==0){
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
	
	Disp_Set_Unit_16(SaveData.Main_Func.Param.test,SELECT_2END-148,FIRSTLINE-2 );//显示单位
	
	
	//Disp_Set_Unit_12(SaveData.Main_Func.Param.test,LIST2-154,65);
	Disp_Button_TestSet(0);
}
//列表模式选择的子函数
void Disp_ListScan_MidRect(void)
{

	LCD_DrawRect( 0, 180,479 , 150+32 , LCD_COLOR_TEST_MID ) ;
//	LCD_DrawRect( 0, 195,479 , 227 , LCD_COLOR_TEST_MID ) ;
}
void Disp_ListScan_Screen(void)
{
	Disp_TopBar_Color();
	Disp_Fastbutton();//显示快速按键	  
//	Disp_ListScan_MidRect();//显示中间色块	 User_LimitList_Item
}
void Disp_ListScan_Item(void)
{
	uint32_t i;
	Disp_ListScan_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveData.Sys_Setup.Language ==1)
	{
		WriteString_16(0, 4, All_TopName_E[7],  0);
	}else if(SaveData.Sys_Setup.Language ==0){
		WriteString_16(0, 4, All_TopName[7],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	
	if(SaveData.Sys_Setup.Language ==1)
	{
		WriteString_16(LIST1, FIRSTLINE, User_ListScan_Item_E[0],  0);
		for(i=1;i<(sizeof(User_ListScan_Item_E)/(sizeof(User_ListScan_Item_E[0])));i++)
		WriteString_16(LIST1+30+(i-1)*77, FIRSTLINE+SPACE1, User_ListScan_Item_E[i],  0);
	}else if(SaveData.Sys_Setup.Language ==0){
		WriteString_16(LIST1, FIRSTLINE, User_ListScan_Item[0],  0);
		for(i=1;i<(sizeof(User_ListScan_Item)/(sizeof(User_ListScan_Item[0])));i++)
		WriteString_16(LIST1+30+(i-1)*77, FIRSTLINE+SPACE1, User_ListScan_Item[i],  0);
	}
	
//	for(i=0;i<11;i++)
//	WriteString_16(LIST1, FIRSTLINE+SPACE1+i*16, User_LimitScan_Item2[i],0);
	Disp_Button_TestSet(0);
}



//系统设置子函数
void Disp_SysLine(void)
{
 	uint32_t i;
	if(SaveData.Sys_Setup.Language ==1)
	{
		for(i=0;i<(sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0])));i++)
		{
			if(i<((sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0]))+1)/2))
				LCD_DrawLine( 95, FIRSTLINE+(i+1)*SPACE1-2, 90+100,FIRSTLINE+(i+1)*SPACE1-2, White );
			else
				LCD_DrawLine( 95+250, FIRSTLINE+(i-(sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0]))-1)/2)*SPACE1-2, 90+100+250,FIRSTLINE+(i-(sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0]))-1)/2)*SPACE1-2, White );
		}
	}else if(SaveData.Sys_Setup.Language ==0){
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
	Disp_Fastbutton();//显示快速按键	  
//	Disp_ListScan_MidRect();//显示中间色块	 User_LimitList_Item  
}


// 显示系统逗号
static void  Disp_SysDot(void)
{
	uint8_t i;
	if(SaveData.Sys_Setup.Language ==1)
	{
		for(i=0;i<(sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0])));i++)
		{
			if(i<((sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0]))+1)/2))
				WriteString_16(LIST1+84+4, FIRSTLINE+SPACE1*i, ":",  0);
			else
				WriteString_16(LIST2+84+4, FIRSTLINE+SPACE1*(i-(sizeof(Sys_Setitem_E)/(sizeof(Sys_Setitem_E[0])))/2), ":",  0);	
		}
	}else if(SaveData.Sys_Setup.Language ==0){
		for(i=0;i<(sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0])));i++)
		{
			if(i<((sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0]))+1)/2))
				WriteString_16(LIST1+84+4, FIRSTLINE+SPACE1*i, ":",  0);
			else
				WriteString_16(LIST2+84+4, FIRSTLINE+SPACE1*(i-(sizeof(Sys_Setitem)/(sizeof(Sys_Setitem[0])))/2), ":",  0);	
		}
	}
	
}


//系统设置
void Disp_Sys_Item(void)
{
	uint32_t i;
	Disp_Sys_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveData.Sys_Setup.Language ==1)
	{
		WriteString_16(0, 4, All_TopName_E[8],  0);
	}else if(SaveData.Sys_Setup.Language ==0){
		WriteString_16(0, 4, All_TopName[8],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
//	WriteString_16(LIST1, FIRSTLINE, User_ListScan_Item[0],  0);
	if(SaveData.Sys_Setup.Language ==1)
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
	}else if(SaveData.Sys_Setup.Language ==0){
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
	Disp_Fastbutton();//显示快速按键	   
}

//校正服务设置界面电容
void Disp_Serve_correction (void)
{
	uint8_t i;

	lcd_Clear(LCD_COLOR_TEST_BACK);
	Disp_serve_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveData.Sys_Setup.Language ==1)
	{
		WriteString_16(0, 4, All_TopName_E[10],  0);
	}else if(SaveData.Sys_Setup.Language ==0){
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
//	 WriteString_16(LIST1+85, FIRSTLINE+SPACE1,"电容初始化",  0);
//SPACE1
		
	Disp_Button_Correction();
	
}


void Disp_Start_Corr(void)
{
	lcd_Clear(LCD_COLOR_TEST_BACK);
	Disp_serve_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveData.Sys_Setup.Language ==1)
	{
		WriteString_16(0, 4, All_TopName_E[10],  0);
	}else if(SaveData.Sys_Setup.Language ==0){
		WriteString_16(0, 4, All_TopName[10],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	
	WriteString_16(LIST2-75, FIRSTLINE+SPACE1*4+6,"StarCorr",  0);
	Disp_Button_Correction();

}



//校正服务设置界面电阻
void Disp_Serve_correctionR (void)
{
	uint8_t i;

	lcd_Clear(LCD_COLOR_TEST_BACK);
	Disp_serve_Screen();
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BAR;
	if(SaveData.Sys_Setup.Language ==1)
	{
		WriteString_16(0, 4, All_TopName_E[10],  0);
	}else if(SaveData.Sys_Setup.Language ==0){
		WriteString_16(0, 4, All_TopName[10],  0);
	}
	
	Colour.Fword=White;
	Colour.black=LCD_COLOR_TEST_BACK;
	for(i=0;i<5;i++)
	{
		   WriteString_16(LIST2-60, FIRSTLINE+SPACE1*(i+2), Correction_Resist[i],  0);	
	}
//	WriteString_16(LIST2-75, FIRSTLINE+SPACE1,"电阻初始化",  0);
//SPACE1
		
	Disp_Button_Correction();
	
}

//校正设置
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
				if(SaveData.Sys_Setup.Language ==1)
				{
					WriteString_16(83+19, 271-29, "CONF",  0);	
					WriteString_16(83+83+19, 271-29, "SAVE",  0);
				}else if(SaveData.Sys_Setup.Language ==0){
					WriteString_16(83+19, 271-29, "确定",  0);	
					WriteString_16(83+83+19, 271-29, "保存",  0);
				}
				
			break ;
	}
	
	if(Correc_successflag==1)
	{		
		if(SaveData.Sys_Setup.Language ==1)
		{
			if(Button_Page->index<5)
			  WriteString_16(LIST1+210, FIRSTLINE+SPACE1*(Button_Page->index+1), "DONE",  0);
			else
			  WriteString_16(LIST2+170, FIRSTLINE+SPACE1*(Button_Page->index-3), "DONE",  0);
		}else if(SaveData.Sys_Setup.Language ==0){
			if(Button_Page->index<5)
			  WriteString_16(LIST1+210, FIRSTLINE+SPACE1*(Button_Page->index+1), "完成",  0);
			else
			  WriteString_16(LIST2+170, FIRSTLINE+SPACE1*(Button_Page->index-3), "完成",  0);
		}
			
		
    }		
	
	
	
	
}



//校正设置
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
//				WriteString_16(83+19, 271-29, "确定",  0);
//				WriteString_16(83+83+19, 271-29, "保存",  0);
		if(SaveData.Sys_Setup.Language ==1)
		{
			WriteString_16(83+19, 271-29, "CONF",  0);
			WriteString_16(83+83+19, 271-29, "SAVE",  0);
		}else if(SaveData.Sys_Setup.Language ==0){
			WriteString_16(83+19, 271-29, "确定",  0);
			WriteString_16(83+83+19, 271-29, "保存",  0);
		}
		
			break ;
	}

	if(Correc_successflag==1)
	{
		 if(SaveData.Sys_Setup.Language ==1)
		{
			WriteString_16(LIST2+50, FIRSTLINE+SPACE1*(Button_Page->index+1), "DONE",  0);
		}else if(SaveData.Sys_Setup.Language ==0){
			WriteString_16(LIST2+50, FIRSTLINE+SPACE1*(Button_Page->index+1), "完成",  0);
		}
				
    }		
	
}


Sort_TypeDef Time_Set_Cot(Sort_TypeDef *Time)
{
	if(Time->Num > 200000)
		Time->Num=200000;
//	Time->Dot=0;
//	Time->Unit=1;
	
	return *Time;
}


Sort_TypeDef Time_Set_Cov(Sort_TypeDef *Time)
{
	if(Time->Dot==0)//没有小数点
	{
		if(Time->Unit==0)//单位是毫秒
		{
			if(Time->Num>20000)//大于60.000  显示60.000
			{
				Time->Num=20000;
				Time->Dot=3;
			}
			else
			if(Time->Num>999)//大于999  显示X.XXX
			{
				//Time->Num=60;
				Time->Dot=3;
				Time->Unit=1;
				
			} 
		
		}
		else//单位是秒
		{
			if(Time->Num>60)//大于60秒 显示60秒
				Time->Num=60;
			Time->Num*=1000;//增加1000倍  小数点为第三位
			Time->Dot=3;	
			Time->Unit=1;			
		}
	
	
	}
	else if(Time->Dot==1)//从高位数第一个小数点为1
	{
		if(Time->Unit==0)//单位为毫秒时
		{
			
				
			if(Time->Num>0)//x.xxxx时 显示最高位
			{
				Time->Num/=10000;
			
			}
//			else if(Time->Num/1000>0)//x.xxx时显示最高位
//			{
//				Time->Num/=1000;
//			
//			
//			}else if(Time->Num/100>0)//x.xx时 显示最高位
//			{
//				Time->Num/=100;
//			
//			
//			}else if(Time->Num/10>0)//x.x时 显示最高位
//			{
//				Time->Num/=10;
//			
//			
//			}
			Time->Dot=0;//因为是毫秒  所以没有小数点
			Time->Unit=1;
	
		
		}
		else//单位是秒
		{
			if(Time->Num>=10000)//x.xxxx时 显示x.xxx
			{
				//Time->Num/=10;
				Time->Unit=1;
				Time->Dot=3;//因为是毫秒  所以没有小数点
				
			
			}else if(Time->Num>=1000)//x.xxx时显示x.xxx
			{
				
				Time->Unit=0;
				Time->Dot=0;//因为是毫秒  所以没有小数点
			
			
			}
			Time->Num/=10;
//			else if(Time->Num>=100)//x.xx时 显示x.xx0
//			{
//				Time->Num*=10;
//			
//			
//			}else if(Time->Num>=10)//x.x时 显示x.x00
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
		if(Time->Unit==0)//单位为毫秒时
		{
			
			if(Time->Num>=1000)//xx.xxx时 显示最高位
			{
				Time->Num/=1000;
				//Time->Dot=0;
				
			
			}
//			else if(Time->Num/100>=10)//xx.xx时显示最高位
//			{
//				Time->Num/=1000;
//			
//			
//			}
			else// if(Time->Num<1000)//xx.x时 显示最高位
			{
				Time->Num=0;
			
			
			}
			Time->Dot=0;//因为是毫秒  所以没有小数点
	
		
		}
		else//单位是秒
		{
			if(Time->Num>20000)//
			{
				Time->Num=20000;
			
			}
//			else if(Time->Num>=10000)//xx.xxx时显示xx.xxx
//			{
//				//Time->Num*=10;
//			
//			
//			}
//			else if(Time->Num>=1000)//xx.xx时 显示xx.xx0
//			{
//				Time->Num*=10;
//			
//			
//			}
			else if(Time->Num<1000)//xx.x时 显示xx.x00
			{
				//Time->Num*=100;
				Time->Dot=0;//因为是秒  有小数点3
				Time->Unit=0;
			
			
			}
//			else if()
//				Time->Num*=1000;//xx.000
			else
			{
				
				Time->Dot=3;//因为是秒  有小数点3
				Time->Unit=1;
			}
	
		
		
		
		}
			
	}else if(Time->Dot==3)//xxx.
	{
		if(Time->Unit==0)//单位为毫秒时
		{
			
			if(Time->Num>=100)//xxx.xx时 显示最高位xxx
			{
				Time->Num/=100;
			
			}
			else
			{
				Time->Num=0;
			
			}
//			else if(Time->Num/10>=100)//xxx.x时显示最高位xxx
//			{
//				Time->Num/=100;
//			
//			
//			}
			Time->Dot=0;//因为是毫秒  所以没有小数点
	
		
		}
		else//单位是秒
		{
			if(Time->Num>20000)
			{
				Time->Num=20000;//加一位显示
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
		if(Time->Unit==0)//单位为毫秒时
		{
			
			if(Time->Num>=10000)//xxxx.x时 显示最高位xxx
			{
				
				{
					//Time->Num/=10;	
					Time->Dot=3;//因为是毫秒  所以没有小数点
					Time->Unit=1;
				}
			
			} //xxxx时显示最高位x.xxx
			else if(Time->Num>0)
			{
				//Time->Num/=10;	
				Time->Dot=0;//因为是毫秒  所以没有小数点
				Time->Unit=0;
			
			
			}
			Time->Num/=10;
			
			
	
		
		}
		else//单位是秒
		{
			if(Time->Num>600)
			{
				Time->Num=600;
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
		if(Time->Unit==0)//单位为毫秒时
		{
			
			if(Time->Num>60000)//xxxx.x时 显示最高位xxx
			{
				
				{
					Time->Num=60000;	
					Time->Dot=3;//因为是毫秒  所以没有小数点
					Time->Unit=1;
				}
			
			} //xxxx时显示最高位x.xxx
			else if(Time->Num>=1000)
			{
				
				Time->Dot=3;//因为是毫秒  所以没有小数点
				Time->Unit=1;
			
			
			
			}
			else
			{
				Time->Dot=0;//因为是毫秒  所以没有小数点
				Time->Unit=0;
			
			}
			
			
	
		
		}
		else//单位是秒
		{
			if(Time->Num>60)
			{
				Time->Num=60;
				
			}
			Time->Num*=1000;
			
			Time->Dot=3;
				Time->Unit=1;
			
		
		}
	}
	else
	{
	
		Time->Num=60000;
		Time->Dot=3;
		Time->Unit=1;
	
	}

	return *Time;
}
Sort_TypeDef Freq_Set_Cov(Sort_TypeDef *Freq)
{
//	uint8_t i;
	if(Freq->Unit==0)//单位是Hz
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
			case 1://1个小数点 
				break;
			case 2://两个小数点
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
			case 3://三个小数点
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
			case 4://4个小数点
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
	else//单位是KHZ
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
	
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);

}
void Disp_Cp_Q(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+9*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Cp_G(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+6*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+6);

}
void Disp_Cp_Rp(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+6);

}
void Disp_Cs_Rs(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+6);

}
void Disp_Cs_D(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+8*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
//	WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+8);

}
void Disp_Cs_Q(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)_C);//_p
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+9*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Lp_Q(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+9*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Lp_Rp(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Lp_Rd(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+10*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Lp_D(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+8*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Lp_G(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+6*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Ls_D(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+8*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Ls_Q(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+9*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+9);

}
void Disp_Ls_Rs(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);

}
void Disp_Ls_Rd(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+1*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+10*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);

}
void Disp_Z_d(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+3*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55, (uint8_t*)Out_Assic+22*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+10*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
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
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
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
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
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
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+10);

}
void Disp_R_X(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+5*40*55/8);//
	LCD_ShowFontCN_40_55(60+40,92,40,55, (uint8_t*)Out_Assic+22*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+21*40*55/8);	//X
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);	//空格
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
//	LCD_ShowFontCN_40_55(60+40,92+55,40,55,(uint8_t*)Out_Assic+10);

}

void Disp_Rs_Q(void)
{
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+9*40*55/8);//_p
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+5*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)Out_Assic+2*40*55/8);
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
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
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
	LCD_ShowFontCN_40_55(60+40+40,92+55,40,55, (uint8_t*)Out_Assic+26*40*55/8);
	//WriteString_Big(60,92+55 ,(uint8_t*)Out_Assic+6);

}
void Disp_G_B(void)
{
	LCD_ShowFontCN_40_55(60,92,40,55,(uint8_t*)Out_Assic+6*40*55/8);//G
	//LCD_ShowFontCN_40_55(60+40,92,40,55,(uint8_t*)_p);
	LCD_ShowFontCN_40_55(60,92+55,40,55,(uint8_t*)Out_Assic+7*40*55/8);//B
	LCD_ShowFontCN_40_55(60+40,92,40,55, (uint8_t*)Out_Assic+22*40*55/8);	//空格
	LCD_ShowFontCN_40_55(60+40,92+55,40,55, (uint8_t*)Out_Assic+22*40*55/8);	//空格
	
	LCD_ShowFontCN_40_55(60+40+40,92,40,55,(uint8_t*)Out_Assic+26*40*55/8);		//冒号
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
			WriteString_16(LIST1+88, FIRSTLINE, User_FUNC[Button_Page->third],  1);//增加算法  把顺序改过来
//			LCD_DrawRect( LIST1+88, ypos-2,SELECT_1END , ypos+SPACE1-4 , Colour.black );//SPACE1
//			WriteString_16(LIST1+88, ypos, User_FUNC[Button_Page->third],  1);//增加算法  把顺序改过来
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
void Disp_Testvalue(uint8_t siwtch)
{
	if(siwtch == 0)
	{
		Test_Dispvalue.Vmvalue.Num = 0;
		Test_Dispvalue.Imvalue.Num = 0;
		Test_Dispvalue.Pvalue.Num = 0;
		Hex_Format(0, 3 , 5 , 0);//显示电压
		WriteString_Big(130-40,95 ,DispBuf);
		
		Hex_Format(0, 3 , 5 , 0);//显示电流
		WriteString_Big(130-40,95+55 ,DispBuf);
		
		Hex_Format(0, 3 , 6 , 0);//显示功率
		WriteString_16(390,185 ,DispBuf,0);
		LCD_ShowFontCN_40_55(90-40,95+55,40,55,(uint8_t*)Out_Assic+22*(55*40/8));
	}else if(siwtch == 1)
	{
		
		Hex_Format(Test_Dispvalue.Vmvalue.Num, 3 , 5 , 0);//显示电压
		WriteString_Big(130-40,95 ,DispBuf);
		
		Hex_Format(Test_Dispvalue.Imvalue.Num, 3 , 5 , 0);//显示电流
		WriteString_Big(130-40,95+55 ,DispBuf);
		if(Irange == 1)
		{
			Test_Dispvalue.Pvalue.Num = (uint32_t)(((float)Test_Dispvalue.Vmvalue.Num/1000.0*((float)Test_Dispvalue.Imvalue.Num/1000.0))*1000);
		}else{
			Test_Dispvalue.Pvalue.Num = (uint32_t)(((float)Test_Dispvalue.Vmvalue.Num/1000.0*((float)Test_Dispvalue.Imvalue.Num/1000.0)));
		}
		Hex_Format(Test_Dispvalue.Pvalue.Num, 3 , 6 , 0);//显示功率
		WriteString_16(390,185,DispBuf,0);
		if(Test_Dispvalue.Imvalue.sign == 0)
		{
			LCD_ShowFontCN_40_55(90-40,95+55,40,55,(uint8_t*)Out_Assic+29*(55*40/8));
		}else if(Test_Dispvalue.Imvalue.sign == 1){
			LCD_ShowFontCN_40_55(90-40,95+55,40,55,(uint8_t*)Out_Assic+30*(55*40/8));
		}
	}


}
void Disp_Big_MainUnit(uint8_t unit,uint8_t unit1)		//显示主参数单位
{
//	const uint8_t nuitnum[]={12,15,16,17,22,14,13};
//	const uint8_t nuit_nuit[]={18,19,20,2,11,23,22};//F,H,Ω，S r °空格
	LCD_ShowFontCN_40_55(DISP_UNIT_XPOS,DISP_UNIT_YPOS,40,55,(uint8_t*)Out_Assic+27*(55*40/8));
//	LCD_ShowFontCN_40_55(DISP_UNIT_XPOS+40,DISP_UNIT_YPOS,40,55,(uint8_t*)Out_Assic+28*(55*40/8));
}

void Disp_Big_SecondUnit(uint8_t unit,uint8_t unit1)	//显示副参数单位
{
//	const uint8_t nuitnum[]={12,15,16,17,22,14,13};
//	const uint8_t nuit_nuit[]={18,19,20,2,11,23,22};//F,H,Ω，S r °空格
	if(Irange == 1)
	{
		LCD_ShowFontCN_40_55(DISP_UNIT_XPOS-40,DISP_UNIT_YPOS+55,40,55,(uint8_t*)Out_Assic+22*(55*40/8));
	}else{
		LCD_ShowFontCN_40_55(DISP_UNIT_XPOS-40,DISP_UNIT_YPOS+55,40,55,(uint8_t*)Out_Assic+31*(55*40/8));
	}
	LCD_ShowFontCN_40_55(DISP_UNIT_XPOS,DISP_UNIT_YPOS+55,40,55,(uint8_t*)Out_Assic+28*(55*40/8));
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



void  Start_Correction(void)			//开始校正
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

void Init_C(void)//电容初始化校正
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


void Correction_C(Button_Page_Typedef* Button_Page)		//电容校正
{
	uint8_t Send_correction_C[8]={0xAB,0x01,0x08,0x06,0xE0,0x00,0x00,0xBF};
	uint8_t i=105;	

		Send_correction_C[5]=Button_Page->index;//发送校正值
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
	
void Save_C(void)	//保存电容值
{
	uint8_t Send_correction_C[8]={0xAB,0x01,0x08,0x06,0xE0,0x0F,0x00,0xBF};		//电容校正保存
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

//初始化电阻
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

void Correction_R(Button_Page_Typedef* Button_Page)		//电阻校正
{
	uint8_t Send_correction_R[8]={0xAB,0x01,0x08,0x06,0xE1,0x00,0x00,0xBF};
	uint8_t i=106;
	    Send_correction_R[5]=Button_Page->index;//发送校正值
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


void Save_R(void)	//保存电阻值
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

void Exit_correction(void)		//退出校正
{

	uint8_t Exit_corre[8]={0xAB,0x01,0x08,0x06,0xE2,0x00,0x00,0xBF};
	uint8_t i=5;		
		UARTPuts( LPC_UART0, Exit_corre);		//退出校正
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

void Clear_Instr(void)		//清除校正
{
	uint8_t Clear_corre[8]={0xAB,0x01,0x08,0x06,0xD0,0x01,0x00,0xBF};
	uint8_t i=3;		
	UARTPuts( LPC_UART0, Clear_corre);		//退出校正
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

void Close_Clear(void)		//关闭清零
{
	uint8_t Send_buff[8]={0xAB,0X01,0x08,0x06,0xF0,0x00,0x00,0xBF};
	UARTPuts( LPC_UART0, Send_buff);
}


void Open_Clear(void)		//开路清零
{
	uint8_t Send_buff[8]={0xAB,0X01,0x08,0x06,0xF0,0x01,0x00,0xBF};
	UARTPuts( LPC_UART0, Send_buff);
}



void Short_Clear(void)			//短路清零
{
	uint8_t Send_buff[8]={0xAB,0X01,0x08,0x06,0xF0,0x02,0x00,0xBF};
	UARTPuts( LPC_UART0, Send_buff);
}

void Send_Freq(Send_Ord_Typedef *ord)
{
	uint8_t Send_buff[10]={0xAB,0X01,0x08,0x06};
	Send_buff[4]=ord->Ordel;
	Send_buff[5]=ord->name;
	Send_buff[6]=0X80;//校验和 
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
	Send_buff[20]=0X00;//校验和 0
	Send_buff[21]=0XBF;	
//	Send_buff[21]=0X80;//校验和 0
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
void Send_Request(u8 x,u8 req)
{
	
	u8 i,len;
	char buf[50];
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
			
		}break;
		case 3:
		{
			sprintf(&USART_RX_BUF[len],"%1d",req);
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
			sprintf(buf,"+%07.3f,%07.3f,%07.3f,%03d,%03d",SaveSIM.Voltage.Num/1000.0,SaveSIM.ChargePC.Num/1000.0,SaveSIM.LoadPC.Num/1000.0
			,SaveSIM.ChargePT.Num,SaveSIM.LoadPT.Num);
			strcat(USART_RX_BUF,buf);
			len+=strlen(buf)-1;
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
		}break;
	}
	USART_RX_BUF[len+1] = 0x0d;
	USART_RX_BUF[len+2] = 0x0a;
	UARTPuts(LPC_UART0,USART_RX_BUF);
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
		return SetErr_ACK(buf,addr ,CMD_ERR); 
    } 
	str[currCharNum++] = ChrStartR;
	sprintf(&str[currCharNum],"%d",addr);
	strncat(str,"/",1);
	currCharNum =strlen(str);
	cmd_flag=strncmp(buf,str,strlen(str));
	//testflag = cmd_flag;
	if(0!=cmd_flag)
	{
		return SetErr_ACK(buf, addr ,CMD_ERR);
		
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
//			case 6:
//				pntlen = 1;
//				if(buf[currCharNum]=='?')
//				{
//					sprintf(&buf[currCharNum],"%1d",LM_S_Vale);
//				    currCharNum+=pntlen;
//				    buf[currCharNum++ ] = ChrEndR;
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
//				    if(temp1>LM_S_H)
//			         {
//			             return SetErr_ACK(buf, addr ,PARA_ERR);
//						 
//			         }
//		        	 LM_S_Vale=temp1;
//							Change_LM_Val(LM_S_Vale);
//					 buf[currCharNum++] = ChrEndR;
//		        }
//				break;
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
//			case 10:
//				pntlen = 8;
//				if(buf[currCharNum]=='?')
//				{
//					buf[currCharNum++]=(Lvl_Vale>=0?'+':'-');
//								sprintf(&buf[currCharNum],"%03.3f",Lvl_Vale);
//				    currCharNum+=pntlen;
//				    buf[currCharNum++ ] = ChrEndS;
//				}
//				else
//				{
//					tmpFg = 1;
//		            for(i=0;i<pntlen;i++)
//		            {
//									if(i==0||i==4)
//										continue;
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
//					temp1=0;
//					for(i=0;i<pntlen;i++)
//		            {
//		               if(i==0)
//					   {
//						   fsingal = (buf[currCharNum++]=='+'?1:(-1));
//						   continue; 
//					   } 
//					   else if(i==4)
//					   {
//						   currCharNum++;
//						   continue; 
//					   }
//					   else									 
//					   temp1 = temp1*10+(buf[currCharNum++]-0x30);
//					}		                   
////				    if(temp1 >1)
////			         {
////			             return SetErr_ACK(buf, addr ,PARA_ERR);
////						 
////			         }
//               
//							  Lvl_Vale=temp1*0.0001*fsingal;
//						    LVL_DA =Lvl_Vale;
////							Voltage_Convert(LVL_DA);
//					 buf[currCharNum++] = ChrEndR;
//		        }
//				break;
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
           		 return SetErr_ACK(buf, addr ,CMD_ERR);
				  
		  }
		  break;
		}
		
	}
	if(j>=CmdNumb)
	{
		return SetErr_ACK(buf, addr ,CMD_ERR);			  
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
	Send_buff[10]=0X00;//校验和 
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
	WriteString_16(20, 200, RangeDisp_Main[data],  1);	//  Cp显示
	WriteString_16(280, 200, RangeDisp_Second[data],  1);   //Rp显示	
	Colour.black=LCD_COLOR_TEST_MID;
	LCD_ShowFontCN_40_55(80,92+30,40,55, (uint8_t*)Out_Assic+7*40*55/8);
	LCD_ShowFontCN_40_55(80+40,92+30,40,55, (uint8_t*)Out_Assic+24*40*55/8);
	LCD_ShowFontCN_40_55(80+40+20,92+30,40,55, (uint8_t*)Out_Assic+25*40*55/8);
//


}

//档号显示中Cp的单位（组合单位）
void Disp_Range_DispMainUnit(uint8_t num,uint8_t unit)		//
{
    if(num>6)
        num=6;
    if(unit>6)
        unit=6;
	WriteString_16(128, 200, (uint8_t*)Disp_Unit[num],  1);//单位1
	WriteString_16(138, 200, (uint8_t*)Disp_Range_Main_Disp[unit],  0);//单位2

}

//档号显示中Rp的单位
void Disp_Range_DispSecondUnit(uint8_t num,uint8_t unit)
{
     if(num>6)
        num=6;
    if(unit>6)
        unit=6;
	WriteString_16(388, 200, (uint8_t*)Disp_Unit[num],  0);//单位1
	WriteString_16(398, 200, (uint8_t*)Disp_Range_Main_Disp[unit],  0);//单位2

}


//档号显示中的Cp和Rp的值
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
		Hex_Format(Count_buff[i] , 0, 6 , 0);//加单位
				
		WriteString_12(LIST2+140, 58+i*15, DispBuf,  1);//显示标称值  后面要接着显示单位
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
	//WriteString_16(BUTTOM_X_VALUE+4*BUTTOM_MID_VALUE, BUTTOM_Y_VALUE, " 显示",  0);
	

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
