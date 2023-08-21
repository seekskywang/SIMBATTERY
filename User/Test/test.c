#include "test.h"
#include "LCD/lpc177x_8x_lcd.h"
#include "lpc177x_8x_uart.h"
#include "Globalvalue/GlobalValue.h"
#include "use_disp.h"
#include "EX_SDRAM/EX_SDRAM.H"
#include "stdint.h"
#include "key/key.h"
#include "uart.h"
#include "timer.h"
#include "lpc177x_8x_eeprom.h"
#include "math.h"
#include "LCD/AsciiLib.h"
#include  "stdio.h"
#include  "usbhost_fat.h"
#include 	"use_disp.h"
#include "lpc177x_8x_rtc.h"
#include "open.h"
#include "lpc177x_8x_gpio.h"

const char *PASSWORD="12345789";
uint8_t  Avg_Test_Main[25][6];
uint8_t  Avg_Test_Second[25][6];
uint8_t  Avg_Count=0;
uint8_t USART_RX_BUF[200];
u8 primcomp,scedcomp;
uint32_t Tick_10ms=0;
uint32_t OldTick;
uint32_t sendcount;

const uint8_t Disp_Unit1[]={'p','n','u','m',' ','k','M'};
const uint8_t Uart_Ordel[]={0x60,0x70,0x71,0x80,0x90,0xa0,0xb0,0xc0,0xe0};
const uint8_t READDATA[7]={0xAB,0x01,0x06,0x03,0x08,0xbf,'\0'};
const u32 MaxVoltage[3]={20000,30000,40000};//����ѹ
const u32 MaxCurrent[3]={10000,6000,5000};//������

//������ѹУ׼��
const u32 CalTestV[3][6]={
	{1000,10000,13000,17000,19000,19500},
	{1000,10000,13000,17000,19000,19500},
	{1000,10000,13000,17000,19000,19500}
};

//���Ƶ�ѹУ׼��
const u32 CalCtrlV[3][4]={
	{1000,10000,13000,17000},
	{1000,10000,13000,17000},
	{1000,10000,13000,17000}
};
extern uint16_t USART_RX_STA;
uint32_t encodertest1;
uint32_t encodertest2;
uint32_t encodertest3;
uint32_t encodertest4;
uint8_t coder_flag;
uint8_t spinbit;
uint8_t spinbitmax;
uint8_t spindelay;
uint8_t spinsave;
uint8_t bootdelay;
uint8_t spinlock;
//const uint8_t Disp_Main_Ord[][3]={
//	{1,1,0},
//	{1,1,1},
//	{1,1,2},
//	{1,1,3},//Cp
//	
//	{0,1,0},
//	{0,1,1},
//	{0,1,3},//Cs
//	{1,0,0},
//	{1,0,0},
//	{1,0,0},
//	{1,0,1},//Lp
//	{1,0,3},
//	{1,0,4},
//	{1,0,0},
//	{0,0,0},//Ls
//	{0,0,1},
//	{0,0,5},
//	{0,0,4},
//	{0,3,6},//Z
//	{0,3,7},
//	{0,4,6},//Y
//	{0,4,7},
//	{0,2,8},//R
//	{1,2,1},
//	{0,2,1},
//	{1,1,0},
//	{1,5,9},//GB
//	//{1,1,0},
//	};

//�����ط�
void CMD_Resend(void)
{
	switch(sendflag)
	{
		case 1:
		{
			Send_Request(3,mainswitch);
		}break;
		case 2:
		{
			Send_Request(10,1);
		}break;
		default:break;
	}
}

//��ȡ�¶�
void Read_Temp(void)
{
	Send_Request(17,0);
}
const uint8_t Disp_Main_Ord[][3]={
	{1,1,6},
	{1,1,7},
	{1,1,5},
	{1,1,2},//Cp-D,CP-Q,CP-G,CP-RP,
	
	{0,1,6},
	{0,1,7},
	{0,1,2},//Cs-D,Cs-Q,Cs-Rs
	
	{1,0,7},
	{1,0,2},
	{1,0,2},
	{1,0,6},//Lp-Q,Lp-Rp,Lp-Rd,Lp-D
	
	{0,0,6},
	{0,0,7},
	{0,0,2},
	{0,0,2},//Ls-D,Ls-Q,Ls-Rs,Ls-Rd
	
	{0,3,6},
	{0,3,2},//Z-d,Z-r
	{0,4,6},
	{0,4,2},//Y-d,Y-r
	{0,2,8},//R-X
	{1,2,7},//Rp-Q
	{0,2,7},//Rs-Q
	{0,5,9},//G-B
	};	

void MODS_Poll(void)
{
	uint16_t addr;
	static uint16_t crc1;
    static uint32_t testi;

    testi=g_tModS.RxCount;
	if(testi>7)				
	{
		testi=testi;
	}
	testi=g_tModS.RxCount; 					/* ??? */

	if (g_tModS.RxCount < 4)				/* ??????��?4???????? */
	{
		g_tModS.RxCount = 0;
	}

	if(g_tModS.RxCount > 5 && Tick_10ms > 2)
	{
		RecHandle();
		g_tModS.RxCount = 0;
	}

							
	
err_ret:
#if 0										/* �˲���Ϊ�˴��ڴ�ӡ�ᖨʵ�������пɲ�Ҫ */
	g_tPrint.Rxlen = g_tModS.RxCount;
	memcpy(g_tPrint.RxBuf, g_tModS.RxBuf, g_tModS.RxCount);
#endif
	
 						/* ��������������������´�֡ͬ�{*/
}	

//==========================================================
//�������ƣ�Power_Process
//�������ܣ��ϵ紦��
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.26
//�޸����ڣ�2015.10.26 08:53
//��ע˵������������SET����У׼����ģʽ
//==========================================================

void Power_Process(void)
{
	uint16_t i;
//	u8 buff[4096];
	u8 key;
    u8 keynum;
    Disp_Coordinates_Typedef Debug_Cood;
	HW_keyInt();
	GPIO_Led_Configuration();
	Turnon_backlight();
//	Beep_on();
	HW_Sendvalueto164(0);
	Bais_LedOff();
	Lock_LedOff();
	Pass_Led();
	Power_Off_led();
	ReadSavedata();
	#ifdef DISP_JK
	if(SaveSIM.jkflag)
	{
		DrawLogo(140,160);
	}
	#endif
	InitGlobalValue();//��ʼ��ȫ�ֱ���
	init_timer(0, 1);//��ʱ��0��ʼ��
	enable_timer(0);
//	init_timer(1, 1);//��ʱ��1��ʼ��
//	enable_timer(1);
//	timer3Init();
	
//	Set_Compbcd_float();
	i=0;//��ʾ��ʱ
	bootdelay = 50;
	
//Delay(2000);
	while(GetSystemStatus()==SYS_STATUS_POWER)
	{    
        i++;
		
        Delay(10);
		if(i == 10)
		{
//			Send_Request(1,0);
		}
//		if(i==50)
//		{
//			Send_Request(10,1);
//		}
        if(i>20+80*SaveSIM.jkflag)
            SetSystemStatus(SYS_STATUS_TEST);//����״̬
         key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==KEY_NUM)
		{	
			//Disp_Flag=1;
			Key_Beep();
            switch(key)
			{
				case Key_UP:
                    lcd_Clear(LCD_COLOR_TEST_BACK);
                    Debug_Cood.xpos=180;
                    Debug_Cood.ypos =120;
                    Debug_Cood.lenth=120;
                    input_password(&Debug_Cood);
                    break;//0xD0
                case Key_DOT:
                    Clear_Instr();
                    
                
                    break;
                default:
                    break;
                }
            }
//		i++;
//		if(i>POWERON_DISP_TIME)//��ʱ20*100mS=2s���Զ��˳�
			

//		key=Key_Read_WithTimeOut(TICKS_PER_SEC_SOFTTIMER/10);//�ȴ�����(100*10ms/10=100ms)
//		switch(key)
//		{
////			case KEY_SET:		//���ü�
//			case L_KEY_SET:		//�������ü�
//				//SetSystemStatus(SYS_STATUS_DEBUG);//����״̬
//                
//				Beep_One();//��������һ��
//				break;
//	
//			case KEY_UP:		//�ϼ�
//			case L_KEY_UP:		//�����ϼ�
//				break;
//	
//			case KEY_DOWN:		//�¼�
//			case L_KEY_DOWN:	//�����¼�
//				break;
//	
//			case KEY_LEFT:		//���
//			case L_KEY_LEFT:	//�������
//				break;

//			case KEY_RIGHT:		//�Ҽ�
//			case L_KEY_RIGHT:	//�����Ҽ�
//				break;

//			case KEY_ENTER:		//ȷ�ϼ�
//			case L_KEY_ENTER:	//����ȷ�ϼ�
//				break;
//	
//			case KEY_START:		//������
//			case L_KEY_START:	//����������
//				break;
//	
//			case KEY_RESET:		//��λ��
//			case L_KEY_RESET:	//������λ��
//				i=POWERON_DISP_TIME;//��ʾ��ʱ����
//				break;
//			
//			default:
//				break;
//		}
		
		//Delay_1ms(50);
		//Range_Control(3);
	}
}
void Send_T0_USB(void)
{
    const uint8_t DISP_UnitMain[]=
	{ 0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,3,3,2,2,2};
	const uint8_t DISP_UnitSecond[]=
	{ 6,6,3,2,6,6,2,6,2,2,6,6,6,2,2,5,4,5,4,2,6,6};
    u8 i;
       // if(Button_Page.index==0)
    
//    if(usb_oenflag==1)
//        {
//            //Write_Usbdata ( UserBuffer,27);
//            Disp_Usbflag(1);
//            
//        }
//        else
//            Disp_Usbflag(2);
    UserBuffer[0]=0XAA;
    for(i=0;i<6;i++)
		{
			*(UserBuffer+i)=Test_Dispvalue.Main_valuebuff[i];
			*(UserBuffer+10+i)=Test_Dispvalue.Secondvaluebuff[i];
            ComBuf3.send.buf[i+1]=Test_Dispvalue.Main_valuebuff[i];
            ComBuf3.send.buf[i+9]=Test_Dispvalue.Secondvaluebuff[i];
            
		
		}
		*(UserBuffer+8)=' ';
		*(UserBuffer+9)='	';
		*(UserBuffer+20)=' ';
		*(UserBuffer+21)='	';
		*(UserBuffer+6)=(uint8_t)Disp_Unit1[Test_Dispvalue.Unit[0]];
		*(UserBuffer+7)=Disp_Range_Main_Disp[DISP_UnitMain[SaveData.Main_Func.Param.test]][0];
        ComBuf3.send.buf[7]=(uint8_t)Disp_Unit1[Test_Dispvalue.Unit[0]];
        ComBuf3.send.buf[8]=Disp_Range_Main_Disp[DISP_UnitMain[SaveData.Main_Func.Param.test]][0];;
        
		*(UserBuffer+8)=Disp_Range_Main_Disp[DISP_UnitMain[SaveData.Main_Func.Param.test]][1];
		
		*(UserBuffer+8+9)=(uint8_t)Disp_Unit1[Test_Dispvalue.Unit[1]];
        ComBuf3.send.buf[7+9]=Test_Dispvalue.Unit[1];
		
		*(UserBuffer+8+10)=Disp_Range_Main_Disp[DISP_UnitSecond[SaveData.Main_Func.Param.test]][0];
		*(UserBuffer+8+11)=Disp_Range_Main_Disp[DISP_UnitSecond[SaveData.Main_Func.Param.test]][1];
        ComBuf3.send.buf[7+10]=DISP_UnitSecond[SaveData.Main_Func.Param.test];
//		*(UserBuffer+8+8)=DISP_UnitMain[SaveData.Main_Func.Param.test];
		
        if(SaveData.Limit_Tab.Comp)
        {
            if(primcomp)
            {
					
                    *(UserBuffer+22)='F';
                    ComBuf3.send.buf[7+11]='F';
					*(UserBuffer+23)=' ';
					*(UserBuffer+24)='	';
            }
			else
				
			{
				*(UserBuffer+22)='P';
				ComBuf3.send.buf[7+11]='P';
				*(UserBuffer+23)=' ';
				*(UserBuffer+24)='	';
				
			}
			 if(scedcomp)
            {
					
                    *(UserBuffer+25)='F';
					*(UserBuffer+26)=' ';
					*(UserBuffer+27)='	';
                    ComBuf3.send.buf[7+11]='F';
                
            }
			else
				
			{
				*(UserBuffer+25)='P';
				*(UserBuffer+26)=' ';
				*(UserBuffer+27)='	';
				ComBuf3.send.buf[7+11]='P';
				
			}
        }
        else{
			*(UserBuffer+22)=' ';
			*(UserBuffer+23)=' ';
			*(UserBuffer+24)='	';
			*(UserBuffer+25)=' ';
			*(UserBuffer+26)=' ';
			*(UserBuffer+27)='	';
		}
		
		*(UserBuffer+28)='\r';
		*(UserBuffer+29)='\n';
        ComBuf3.send.buf[7+12]=0XBF;
}	
void EncoderHandler(void)
{
	
	if(encodertest3 != LPC_TIM3->CR1 && encodertest4 != LPC_TIM3->CR0)
	{
		
		encodertest3 = (LPC_TIM3->CR1/*%1000*/);
		encodertest4 = (LPC_TIM3->CR0/*%1000*/);
		if(EnccounterA > EnccounterB)
		{
			spintest = 1;
		}else if(EnccounterB > EnccounterA){
			spintest = 2;
		}
	}
}

void EncodeScan()
{	
	unsigned char BC,ZX=0,FX=0;
	
//	if(!(GPIO_ReadValue(2) & (1<<23)))
//	{
//		encodertest1 = 0;
//	}else{
//		encodertest1 = 1;
//	}

//	if(!(GPIO_ReadValue(2) & (1<<22)))
//	{
//		encodertest2 = 0;
//	}else{
//		encodertest2 = 1;
//	}
	if(EncoderA != encodertest1)
	{
		encodertest1 = EncoderA;
		if(EncoderB != EncoderA)
		{
			if(spinlock == 0)
				spintest = 2;
		}else {
			if(spinlock == 0)
				spintest = 1;
		}
	}
//	if(EncoderA==0 && FX == 0)
//	{
//		ZX = 1;
//		if(EncoderB==0 && BC == 0)
//		{
//			BC = 1;
//			spintest = 1;//������ת
//		}
//	}else
//	
//	if(EncoderB==0 && ZX == 0)
//	{
//		FX = 1;
//		if(EncoderA==0 && BC == 0)
//		{
//			BC = 1;
//			spintest = 2;//������ת
//		}
//	}

	
	
	
}
//���Գ���
void Test_Process(void)
{
	int t=0;
	uint16_t len;
	uint8_t len1 =0;
	Disp_Coordinates_Typedef  Coordinates;
	
	uint32_t  numBlks, blkSize;
	uint8_t  inquiryResult[INQUIRY_LENGTH],rc;
	Button_Page_Typedef Button_Page;
	Main_Second_TypeDed Main_Second;//�������ͷ����������
	uint32_t keynum=0;
	uint8_t key,lock_flag=0;
    uint8_t page=1,i;
	uint8_t Disp_Flag=1;
    uint8_t timebuff[10];
	static uint8_t TrigFlag=0;
	
	Send_Ord_Typedef Uart;
	const uint8_t DISP_UnitMain[]=
	{ 0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,3,3,2,2,2};
	const uint8_t DISP_UnitSecond[]=
	{ 6,6,3,2,6,6,2,6,2,2,6,6,6,2,2,5,4,5,4,2,6,6};
    UART_TxCmd(LPC_UART0, ENABLE);
	Button_Page.page=0;
	Button_Page.index=0;
	Button_Page.third=0xff;
	Disp_Test_Item();
	Main_Second.Main_flag=0;
	Main_Second.Second_falg=0;
	Delay_Key();
	Send_Request(10,1);//��������
    GPIO_ClearInt(0, 1<<19);
    NVIC_EnableIRQ(GPIO_IRQn);
	encodertest1 = EncoderA;
	spintest = 0;
	coder_flag = 0;
	spinbit = 0;
	while(GetSystemStatus()==SYS_STATUS_TEST)
	{
		Colour.Fword=LCD_COLOR_WHITE;
        Colour.black=LCD_COLOR_TEST_BACK;
		if(bootdelay > 0)
		{
			bootdelay --;
		}
//		encodertest1 = (GPIO_ReadValue(2) & (1<<23))>>23;
//		encodertest2 = (GPIO_ReadValue(2) & (1<<22))>>22;
//		if(!(GPIO_ReadValue(2) & (1<<23)))
//		{
//			encodertest1 = 0;
//		}else{
//			encodertest1 = 1;
//		}

//		if(!(GPIO_ReadValue(2) & (1<<22)))
//		{
//			encodertest2 = 0;
//		}else{
//			encodertest2 = 1;
//		}
//		EncoderHandler();
		if(spintest != 0 && spinlock == 0)
		{
			Key_Beep();
			if(coder_flag == 0)
			{
				if(spintest == 1)
				{
					if(Button_Page.index>5)
						Button_Page.index=0;
					else
						Button_Page.index++;
					Button_Page.page=0;
				}else if(spintest == 2){
					if(Button_Page.index<1)
						Button_Page.index=6;
					else
						Button_Page.index--;
					Button_Page.page=0;
				}
				
			}else if(coder_flag == 1){
				if(Button_Page.index == 1)
				{
					if(spintest == 1)
					{
						SaveSIM.Voltage.Num += pow(10,spinbit);
						if(SaveSIM.Voltage.Num > MaxVoltage[SaveSIM.Version])
						{
							SaveSIM.Voltage.Num = MaxVoltage[SaveSIM.Version];
						}
					}else if(spintest == 2){
						if(SaveSIM.Voltage.Num < pow(10,spinbit))
						{
							SaveSIM.Voltage.Num = 0;
						}else{
							SaveSIM.Voltage.Num -= pow(10,spinbit);
//							if(SaveSIM.Voltage.Num < pow(10,spinbit))
//							{
//								if(spinbit > 0)
//								spinbit --;
//							}
						}
					}
					spinsave = 1;
					spindelay = 20;
				}

			}
			spintest = 0;
			Disp_Flag=1;
		}
		if(spindelay == 0 && spinsave == 1)
		{
			Send_Request(10,1);
			Savetoeeprom();
			spinsave = 0;
		}else{
			spindelay--;
		}
//		encodertest1 = GPIO_ReadValue(2) & (1<<23);
//		encodertest2 = GPIO_ReadValue(2) & (1<<25);
        if(Rtc_intflag)
        {
            Rtc_intflag=0;
            Colour.Fword=LCD_COLOR_WHITE;
            Colour.black=LCD_COLOR_TEST_BACK;
            //sprintf((char *)timebuff,"%2d:%2d:%2d",RTC_TIME_DISP.HOUR,RTC_TIME_DISP.MIN,RTC_TIME_DISP.SEC);
            //Hex_Format(RTC_TIME_DISP.HOUR, 0, 2, 1);
            timebuff[0]=RTC_TIME_DISP.HOUR/10+'0';
            timebuff[1]=RTC_TIME_DISP.HOUR%10+'0';
            timebuff[2]=':';
            timebuff[3]=RTC_TIME_DISP.MIN/10+'0';
            timebuff[4]=RTC_TIME_DISP.MIN%10+'0';
            timebuff[5]=':';
            timebuff[6]=RTC_TIME_DISP.SEC/10+'0';
            timebuff[7]=RTC_TIME_DISP.SEC%10+'0';
            timebuff[8]=0;
            
            WriteString_16(LIST1+360, 2, timebuff,  0);
        }
//        sprintf((char *)DispBuf,"%2d-%2d-%2d",RTC_TIME_DISP.HOUR,RTC_TIME_DISP.MIN,RTC_TIME_DISP.SEC);
//        WriteString_16(LIST1+360, 2, DispBuf,  0);
		if(Disp_Flag==1)
		{
			Disp_Test_value(&Button_Page);
			
			Disp_Flag=0;
		
		}//
		//_printf("CoreClock: %s\n",READDATA); 
		if(USART_RX_STA&0x8000)	   //�ж��Ƿ����������
		{					   
		//	len=USART_RX_STA&0x3f;//�õ��˴ν��յ������ݳ���   ǰ��λΪ״̬λ ����Ҫ��ȥ
		//	printf("\r\n�����͵���ϢΪ:\r\n\r\n");
//			DE485;
			len1=USART_RX_STA&0x3f;
			len = SerialRemoteHandleL(len1,USART_RX_BUF);
			len1 =0;
			memset(USART_RX_BUF,0,200);
//			RE485;
			
		//	printf("\r\n\r\n");//���뻻��
			USART_RX_STA=0;			  //���� ����Ĵ���		  Ϊһ�´βɼ���׼��
		}else{
			if(/*mainswitch == 1 && */timer0_counter>50 && Disp_Flag == 0 && busyflag == 0)//��������
			{
				sendcount ++;
				Send_Request(6,0);
				timer0_counter=0;
			}
		}
		if(mainswitch == 0 && timer2_counter > 500)
		{
			Read_Temp();
			timer2_counter = 0;
		}
//		Uart_Process();
//		Send_Uart3((uint8_t *)READDATA);
		Colour.black=LCD_COLOR_TEST_MID;
//	sprintf((char *)DispBuf,"%2d",SaveData.Main_Func.Param.test);
//	WriteString_16(210, 4, DispBuf,  0);
		
		if(timer1_counter > 10)
		{
			CMD_Resend();
			timer1_counter = 0;
		}

//		WriteString_16(462,186,"W",0);
		Disp_Big_MainUnit(Test_Dispvalue.Unit[0],DISP_UnitMain[SaveData.Main_Func.Param.test]);//��ʾ��λ
//			Test_Dispvalue.Secondvalue.Dot=3;
		Disp_Big_SecondUnit(Test_Dispvalue.Unit[1],DISP_UnitSecond[SaveData.Main_Func.Param.test]);//��������λ
		Disp_Testvalue(mainswitch);			//��ʾ����ֵ
		
		Disp_switch();
		All_LedOff1();
//        Uart3_Process();
//		else
//			Disp_Usbflag(0);
		 
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
					
                    switch(Button_Page.index)
					{
						case 0:
							//if(Button_Page.page==0)
//								SetSystemStatus(SYS_STATUS_SETUPTEST);
							SaveSIM.qvflag = 0 + 3*Button_Page.page;
							SaveSIM.Voltage = SaveSIM.QuickV[SaveSIM.qvflag];
							Send_Request(10,1);
							break;
						case 1:
						{
//							SaveSIM.resflag ++;
//							if(SaveSIM.resflag > 3)
//							{
//								SaveSIM.resflag = 0;
//							}
						}break;
						case 4:
							SaveSIM.qvflag = 0 + 3*Button_Page.page;
							SaveSIM.Voltage = SaveSIM.QuickV[SaveSIM.qvflag];
							Send_Request(10,1);
							break;					
						default:
							break;
							
					}
					Savetoeeprom();
				break;
				case Key_F2:
					
					switch(Button_Page.index)
					{
						case 0:
						{
//							SetSystemStatus(SYS_STATUS_SYSSET);
							SaveSIM.qvflag = 1 + 3*Button_Page.page;
							SaveSIM.Voltage = SaveSIM.QuickV[SaveSIM.qvflag];
							Send_Request(10,1);
						}break;
						case 1:
						{
//							SaveSIM.Voltage.Num += 1 * pow(10.0,(float)SaveSIM.resflag);
//							if(SaveSIM.Voltage.Num > 20000)
//							{
//								SaveSIM.Voltage.Num = 20000;
//							}
//							Send_Request(10,1);
						}break;
						case 4:
							SaveSIM.qvflag = 1 + 3*Button_Page.page;
							SaveSIM.Voltage = SaveSIM.QuickV[SaveSIM.qvflag];
							Send_Request(10,1);
							break;
						default:
							break;
					
					
					}
					Savetoeeprom();
				break;
				case Key_F3:
					
					switch(Button_Page.index)
					{
						case 0:
						{
							SaveSIM.qvflag = 2 + 3*Button_Page.page;
							SaveSIM.Voltage = SaveSIM.QuickV[SaveSIM.qvflag];
							Send_Request(10,1);
						}
						break;
						case 1:
						{
//							if(SaveSIM.Voltage.Num < 1 * pow(10.0,(float)SaveSIM.resflag))
//							{
//								SaveSIM.Voltage.Num = 0;
//							}else{
//								SaveSIM.Voltage.Num -= 1 * pow(10.0,(float)SaveSIM.resflag);
//							}
//							Send_Request(10,1);
						}break;
						case 4://MAX_R_RANGE
							SaveSIM.qvflag = 2 + 3*Button_Page.page;
							SaveSIM.Voltage = SaveSIM.QuickV[SaveSIM.qvflag];
							Send_Request(10,1);
							break;
						default:
							break;
					
					
					}
					Savetoeeprom();
				break;
				case Key_F4:
					
					switch(Button_Page.index)
					{
						case 0:
							if(Button_Page.page <1)
							{
								Button_Page.page ++;
							}else{
								Button_Page.page = 0;
							}
							break;
						case 4:
							if(Button_Page.page <1)
							{
								Button_Page.page ++;
							}else{
								Button_Page.page = 0;
							}
							break;
						default:
							break;
					
					
					}	
					Savetoeeprom();
				break;
				case Key_F5:
					if(mainswitch == 0)
					{
						mainswitch = 1;
					}else{
						mainswitch = 0;
					}
					Send_Request(3,mainswitch);
				break;
				case Key_Disp:
					
//                    SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
					if(coder_flag == 1)
					{
						coder_flag = 0;
						spinbit = 0;
					}else{
						SetSystemStatus(SYS_STATUS_SETUPTEST);
					}
				break;
				case Key_FAST:
					
				break;
//				case Key_LEFT:
//					
//					if(Button_Page.index==0)
//						Button_Page.index=6;
//					else
//					if(Button_Page.index>3)
//						Button_Page.index-=3;
//					else
//						Button_Page.index+=2;
//					Button_Page.page=0;
//				break;
//				case Key_RIGHT:
//					
//					if(Button_Page.index==0)
//						Button_Page.index=1;
//					else
//					if(Button_Page.index<=3)
//						Button_Page.index+=3;
//					else
//						Button_Page.index-=2;
//					Button_Page.page=0;
//						
//				break;
//				case Key_DOWN:
//					
//					if(Button_Page.index>5)
//						Button_Page.index=0;
//					else
//						Button_Page.index++;
//					Button_Page.page=0;
//					
//				break;
				case Key_UP:
					if(coder_flag == 1 && Button_Page.index == 1)
					{
						if(spinbit < spinbitmax)
						{
							spinbit ++;
						}else{
							spinbit = 0;
						}
					}
					if(coder_flag == 0  && Button_Page.index == 1)
					{
						coder_flag = 1;
					}
					
				break;
				
				case Key_NUM1:
//					Send_Request(3,1);
//				break;
				case Key_NUM2:
//					SaveData.Set_Bat.setvoltage += 10;
//				break;
				case Key_NUM3:
//					SaveData.Set_Bat.setvoltage -= 10;
//				break;
				case Key_NUM4:					
//				break;
				case Key_NUM5:
//					SaveData.Set_Bat.setvoltage += 100;
//				break;
				case Key_NUM6:
//					SaveData.Set_Bat.setvoltage -= 100;
//				break;
				case Key_NUM7:
//				break;
				case Key_NUM8:
//					Send_Request(12,0);
//				break;
				case Key_NUM9:
//					SaveData.Set_Bat.setvoltage -= 1000;	
//				break;
				case Key_NUM0:
//					Send_Request(3,0);
//				break;
				case Key_DOT:
					switch(Button_Page.index)
					{
						case 1:
							Coordinates.xpos=LIST1+88+16;
							Coordinates.ypos=FIRSTLINE;
							Coordinates.lenth=76;
							SaveSIM.Voltage=Disp_Set_Num(&Coordinates);
							Send_Request(10,1);
							break;
						case 2:
							Coordinates.xpos=LIST1+88+16;
							Coordinates.ypos=FIRSTLINE+SPACE1*1;
							Coordinates.lenth=76;
							SaveSIM.ChargePC=Disp_Set_C(&Coordinates);
							Send_Request(10,1);
							break;
						case 3:
							Coordinates.xpos=LIST1+88+16;
							Coordinates.ypos=FIRSTLINE+SPACE1*2;
							Coordinates.lenth=76;
							SaveSIM.LoadPC=Disp_Set_C(&Coordinates);
							Send_Request(10,1);
							break;
						case 5:
							Coordinates.xpos=LIST2+88+32;
							Coordinates.ypos=FIRSTLINE+SPACE1;
							Coordinates.lenth=76;
							SaveSIM.ChargePT = Disp_Set_T(&Coordinates);
							Send_Request(10,1);
							break;
						case 6:
							Coordinates.xpos=LIST2+88+32;
							Coordinates.ypos=FIRSTLINE+SPACE1*2;
							Coordinates.lenth=76;
							SaveSIM.LoadPT = Disp_Set_T(&Coordinates);
							Send_Request(10,1);
							break;
						default:
							break;					
					
					}
					Savetoeeprom();
				break;
				case Key_BACK:
					Send_Request(11,1);
				break;
				case Key_LOCK:
					if(lock_flag)
						lock_flag=0;
					else
						lock_flag=1;
					if(lock_flag)
					{
						Lock_Control_On();
						Lock_LedOn();
						
					}
					else
					{
						Lock_LedOff();
						Lock_Control_Off();
					
					
					}
					
				break;
				case Key_BIAS:
					spinlock=!spinlock;
					if(spinlock)
						Bais_LedOn();
					else
						Bais_LedOff();
				break;
				case Key_REST:
					
				break;
				case Key_TRIG:
					
				break;
				default:
					
				break;
					
			}
		
		
		}
	
	
	}
//    UART_TxCmd(LPC_UART0, DISABLE);

}
//�ļ��������
void File_Process(void)
{
	 uint32_t keynum=0;
	 uint8_t key;
	Delay_Key();
  	while(GetSystemStatus()==SYS_STATUS_FILE)
	{
	 key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==2)
		{
			switch(key)
			{
				case Key_F1:
				break;
				case Key_F2:
				break;
				case Key_F3:
				break;
				case Key_F4:
				break;
				case Key_F5:
				break;
				case Key_Disp:
				break;
				case Key_SETUP:
				break;
				case Key_FAST:
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
				break;
				case Key_NUM2:
				break;
				case Key_NUM3:
				break;
				case Key_NUM4:
				break;
				case Key_NUM5:
				break;
				case Key_NUM6:
				break;
				case Key_NUM7:
				break;
				case Key_NUM8:
				break;
				case Key_NUM9:
				break;
				case Key_NUM0:
				break;
				case Key_DOT:
				break;
				case Key_BACK:
				break;
				case Key_LOCK:
				break;
				case Key_BIAS:
				break;
				case Key_REST:
				break;
				case Key_TRIG:
				break;
				default:
				break;
					
			}
		
		
		}
	
	
	
	}

}
//�������ó���  ��������
void Setup_Process(void)
{

	Button_Page_Typedef Button_Page;
	Disp_Coordinates_Typedef  Coordinates;
	Send_Ord_Typedef Uart;
	
	uint32_t keynum=0;
	uint8_t key;
//    uint8_t page=1;
	uint8_t Disp_Flag=1;
//	uint8_t index=0;
//	uint32_t *pt;
//	pt=(uint32_t *)&SaveData.Main_Func;
	Button_Page.index=0;
	Button_Page.page=0;
    Disp_Test_Set_Item();
	Delay_Key();
 	while(GetSystemStatus()==SYS_STATUS_SETUPTEST)
	{
	    if(spintest != 0)
		{
			Key_Beep();
			if(spintest == 1)
			{
				if(Button_Page.index>5)
					Button_Page.index=0;
				else
					Button_Page.index++;
				Button_Page.page=0;
			}else if(spintest == 2){
				if(Button_Page.index<1)
					Button_Page.index=6;
				else
					Button_Page.index--;
				Button_Page.page=0;
			}
			
			spintest = 0;
			Disp_Flag=1;
		}
		
		if(Disp_Flag==1)
		{
			DispSet_value(&Button_Page);
			Disp_Flag=0;
		
		}
		
//		if(timer0_counter>0)//��������
//		{
//			switch(Uart_Send_Flag)
//			{
//				case 0:
////					Send_Request();
//					break;
//				case 1:
//					Send_Main_Ord();
//					break;
//				case 2:
//					Send_Freq(&Uart);
//					break;
//				default:
////					Send_Request();
//					break;
//			
//			}
//			Uart_Send_Flag=0;
//			
//			timer0_counter=0;
//		
//		
//		}
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==7)
		{	Disp_Flag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1:
					
					switch(Button_Page.index)
					{
						case 0:

								SetSystemStatus(SYS_STATUS_TEST);//

								
							break;
						default:
							break;
					
					
					}

				break;
				case Key_F2:
					

					switch(Button_Page.index)
					{
						case 0:
							SetSystemStatus(SYS_STATUS_SYSSET);
								
							break;
						default:
							break;
					
					
					}				
				

				break;
				case Key_F3:
					
					switch(Button_Page.index)
					{
						case 0:

							break;
						default:
							break;
					
					
					}	
					
				break;
				case Key_F4:
					
					switch(Button_Page.index)
					{
						case 0:
							if(Button_Page.page==0)				//SYS_STATUS_SYSSET
							    
//								if(Button_Page.page==0)
//								SetSystemStatus(SYS_STATUS_ITEMSET);
							break;
						default:
							break;
					
					
					}	
				
				break;
				case Key_F5:
					
					switch(Button_Page.index)
					{
						case 0:
//							if(Button_Page.page==0)
//								Button_Page.page=1;
//							else
//								Button_Page.page=0;
////							page=2;
//							Disp_Button_TestSet(page);
						break;
						case 1:
							if(Button_Page.page<1)
								Button_Page.page++;
							else
								Button_Page.page=0;

						
						break;
					
					
					}
                    
					
				break;
				case Key_Disp:
						
                        SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
						
                        SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
					
				break;
//				case Key_LEFT:
//					
//					if(Button_Page.index==0)
//						Button_Page.index=6;
//					else
//					if(Button_Page.index>3)
//						Button_Page.index-=3;
//					else
//						Button_Page.index+=2;
//					Button_Page.page=0;
//						
//				break;
//				case Key_RIGHT:
//					
//					if(Button_Page.index==0)
//						Button_Page.index=1;
//					else
//					if(Button_Page.index<=3)
//						Button_Page.index+=3;
//					else
//						Button_Page.index-=2;
//					Button_Page.page=0;
//						
//				break;
//				case Key_DOWN:
//					
//					if(Button_Page.index>5)
//						Button_Page.index=0;
//					else
//						Button_Page.index++;
//					Button_Page.page=0;
//					
//				break;
//				case Key_UP:
//					
//					if(Button_Page.index<1)
//						Button_Page.index=6;
//					else
//						Button_Page.index--;
//					Button_Page.page=0;
//				break;
				case Key_NUM1:											
				//break;
				case Key_NUM2:
				//break;
				case Key_NUM3:
				//break;
				case Key_NUM4:
				//break;
				case Key_NUM5:
				//break;
				case Key_NUM6:
				//break;
				case Key_NUM7:
				//break;
				case Key_NUM8:
				//break;
				case Key_NUM9:
				//break;
				case Key_NUM0:
				//break;
				case Key_DOT:					
					switch(Button_Page.index)
					{
						case 1:
							Coordinates.xpos=LIST1+88+16;
							Coordinates.ypos=FIRSTLINE;
							Coordinates.lenth=76;
							SaveSIM.QuickV[0]=Disp_Set_Num(&Coordinates);
							break;
						case 2:
							Coordinates.xpos=LIST1+88+16;
							Coordinates.ypos=FIRSTLINE+SPACE1*1;
							Coordinates.lenth=76;
							SaveSIM.QuickV[1]=Disp_Set_Num(&Coordinates);
							break;
						case 3:
							Coordinates.xpos=LIST1+88+16;
							Coordinates.ypos=FIRSTLINE+SPACE1*2;
							Coordinates.lenth=76;
							SaveSIM.QuickV[2]=Disp_Set_Num(&Coordinates);
							break;
						case 4:
							Coordinates.xpos=LIST2+88+16;
							Coordinates.ypos=FIRSTLINE;
							Coordinates.lenth=76;
							SaveSIM.QuickV[3]=Disp_Set_Num(&Coordinates);
							break;
						case 5:
							Coordinates.xpos=LIST2+88+16;
							Coordinates.ypos=FIRSTLINE+SPACE1;
							Coordinates.lenth=76;
							SaveSIM.QuickV[4]=Disp_Set_Num(&Coordinates);
							break;
						case 6:
							Coordinates.xpos=LIST2+88+16;
							Coordinates.ypos=FIRSTLINE+SPACE1*2;
							Coordinates.lenth=76;
							SaveSIM.QuickV[5]=Disp_Set_Num(&Coordinates);
							break;
						default:
							break;					
					
					}
					Savetoeeprom();
				case Key_BACK:
					
				break;
				case Key_LOCK:
					
				break;
				case Key_BIAS:
					
				break;
				case Key_REST:
					
				break;
				case Key_TRIG:
					
				break;
				default:
					
				break;
					
			}
		
		
		}
	 	
	
	
	
	}
	Savetoeeprom();
}
//���ݱ������
void Data_StoreProcess(void)
{
	uint32_t keynum=0;
	uint8_t key;
//	Button_Page_Typedef Button_Page;
//	Button_Page.index=0;
//	Button_Page.page=0;
	Delay_Key();
 	while(GetSystemStatus()==SYS_STATUS_DATASTORE)
	{
	 key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==2)
		{
			switch(key)
			{
				case Key_F1:
				break;
				case Key_F2:
				break;
				case Key_F3:
				break;
				case Key_F4:
				break;
				case Key_F5:
				break;
				case Key_Disp:
				break;
				case Key_SETUP:
				break;
				case Key_FAST:
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
				break;
				case Key_NUM2:
				break;
				case Key_NUM3:
				break;
				case Key_NUM4:
				break;
				case Key_NUM5:
				break;
				case Key_NUM6:
				break;
				case Key_NUM7:
				break;
				case Key_NUM8:
				break;
				case Key_NUM9:
				break;
				case Key_NUM0:
				break;
				case Key_DOT:
				break;
				case Key_BACK:
				break;
				case Key_LOCK:
				break;
				case Key_BIAS:
				break;
				case Key_REST:
				break;
				case Key_TRIG:
				break;
				default:
				break;
					
			}
		
		
		}
	 	
	
	}
}
//������ʾ
void Range_Process(void)
{
	
	uint32_t keynum=0;
	uint8_t key,num,lock_flag=0;
	Send_Ord_Typedef Uart;
//    uint8_t page=1;
	uint8_t Disp_flag=1;
//	uint8_t index=0;
	Button_Page_Typedef Button_Page;
	Button_Page.index=0;
	Button_Page.page=0;
	Disp_Range_Item();
	Delay_Key();
//    Delay(300);
    UART_TxCmd(LPC_UART0, ENABLE);
    Uart_Send_Flag=0;
 	while(GetSystemStatus()==SYS_STATUS_RANGE)
	{
		if(Disp_flag==1)
		{
			Disp_RangeDispValue(&Button_Page);
			Disp_flag=0;	
		}
		
//		Uart_Process();
		
//		if(timer0_counter>0)//��������
//		{
//			switch(Uart_Send_Flag)
//			{
//				case 0:
////					Send_Request();
//					break;
//				case 1:
//					Send_Main_Ord();
//					break;
//				case 2:
//					Send_Freq(&Uart);
//					break;
//				default:
////					Send_Request();
//					break;
//			
//			}
//			Uart_Send_Flag=0;
//			
//			timer0_counter=0;
//		
//		
//		}
		if(SaveData.Limit_Tab.Comp)
		{
			num=Test_Comp(&Comp_Change);
			Colour.black=LCD_COLOR_TEST_MID;
			Disp_Range_Num( num);
			Test_Comp_Fmq();
			
		}
        Send_T0_USB();
		Disp_RangeTestvalue();
	 	key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==KEY_NUM)
		{
			Disp_flag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_TEST);
					}
					else
						SaveData.Limit_Tab.Comp=0;
				break;
				case Key_F2:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_RANGE);
					}
					else
						SaveData.Limit_Tab.Comp=1;
				break;
				case Key_F3:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_RANGECOUNT);
					}
				break;
				case Key_F4:
					
//					if(Button_Page.index==0)
//					{
//						if(Button_Page.page==0)
//							SetSystemStatus(SYS_STATUS_ITEM);
//					}
				break;
				case Key_F5:
					
//					if(Button_Page.index==0)
//					{
//						if(Button_Page.page==1)
//							Button_Page.page=0;
//						else
//							Button_Page.page=1;
//						Disp_Button_value1(Button_Page.page);
//					}
				break;
				case Key_Disp:
					
                    SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
					
                    SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
					
//				break;
				case Key_LEFT:
//				break;
				case Key_RIGHT:
//				break;
				case Key_UP:
//				break;
				case Key_DOWN:
					
					if(Button_Page.index)
						Button_Page.index=0;
					else
						Button_Page.index=1;
				break;
				case Key_NUM1:
					
				break;
				case Key_NUM2:
					
				break;
				case Key_NUM3:
					
				break;
				case Key_NUM4:
					
				break;
				case Key_NUM5:
					
				break;
				case Key_NUM6:
					
				break;
				case Key_NUM7:
					
				break;
				case Key_NUM8:
					
				break;
				case Key_NUM9:
					
				break;
				case Key_NUM0:
					
				break;
				case Key_DOT:
					
				break;
				case Key_BACK:
					
				break;
				case Key_LOCK:
					if(lock_flag)
						lock_flag=0;
					else
						lock_flag=1;
					if(lock_flag)
					{
						Lock_Control_On();
						Lock_LedOn();
						
					}
					else
					{
						Lock_LedOff();
						Lock_Control_Off();
					
					
					}
					
				break;
				case Key_BIAS:
					
				break;
				case Key_REST:
					
				break;
				case Key_TRIG:
					
				break;
				default:
					
				break;
					
			}
		
		
		}
	
	
	}
//    UART_TxCmd(LPC_UART0, DISABLE);
}
//��������ʾ
void Range_CountProcess(void)
{
   	 uint32_t keynum=0;
	uint32_t uart_count=0;
	 uint8_t key,lock_flag=0;
	u8 i;
	uint8_t Disp_flag=1;
//	 uint8_t page=1;
	Button_Page_Typedef Button_Page;
	Button_Page.index=0;
	Button_Page.page=0;
    UART_TxCmd(LPC_UART0, ENABLE);
	Disp_Range_Count_Item();
	Delay_Key();
	Set_Compbcd_float();
	for(i=0;i<=10;i++)
		Count_buff[i]=0;
 	while(GetSystemStatus()==SYS_STATUS_RANGECOUNT)
	{
		key=HW_KeyScsn();
		if(Disp_flag==1)
		{
			Disp_Range_ComDispValue(&Button_Page);
			Disp_flag=0;
			
		}
//		uart_count=Uart_Process();
//		if(timer0_counter>0)//��������
//		{
//			switch(Uart_Send_Flag)
//			{
//				case 0:
////					Send_Request();
//					break;
//				case 1:
//					Send_Main_Ord();
//					break;
//				case 2:
//					//Send_Freq(&Uart);
//					break;
//				default:
////					Send_Request();
//					break;
//			
//			}
//			Uart_Send_Flag=0;
//			
//			timer0_counter=0;
//				
//		}
		if(SaveData.Limit_Tab.Comp&&uart_count==1)
		{
			Test_Comp(&Comp_Change);
			Test_Comp_Fmq();
			
		}
        Send_T0_USB();
		//Disp_RangeTestvalue();
		if(SaveData.Limit_Tab.Param)//�����Ƚ�
			;
//		Disp_RangeCount();//���ӷ�ѡ�������
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==KEY_NUM)
		{	Disp_flag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1:
					
                    if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_TEST);
					}
					else
						SaveData.Limit_Tab.count=0;
				break;
				case Key_F2:
					
					if(Button_Page.index==0)
					{
                    if(Button_Page.page==0)
                        SetSystemStatus(SYS_STATUS_RANGE);
					}
					else
						SaveData.Limit_Tab.count=1;
				break;
				case Key_F3:
					
					if(Button_Page.index==0)
                    if(Button_Page.page==0)
                        SetSystemStatus(SYS_STATUS_RANGECOUNT);
				break;
				case Key_F4:
					
//					if(Button_Page.index==0)
//                    if(Button_Page.page==0)
//                        SetSystemStatus(SYS_STATUS_ITEM);
				break;
				case Key_F5:
					
//                    if(Button_Page.page==1)
//						Button_Page.page=0;
//					else
//						Button_Page.page=1;
//                    Disp_Button_value1(Button_Page.page);
				break;
				case Key_Disp:
					
                    SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
					
                    SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
					
				break;
				case Key_LEFT:
				//break;
				case Key_RIGHT:
				//break;
				case Key_UP:
				//break;
				case Key_DOWN:
					
					if(Button_Page.index==0)
						Button_Page.index=1;
					else
						Button_Page.index=0;
				break;
				case Key_NUM1:
				case Key_NUM2:
				case Key_NUM3:
				case Key_NUM4:
				case Key_NUM5:
				case Key_NUM6:
				case Key_NUM7:
				case Key_NUM8:
				case Key_NUM9:
				case Key_NUM0:
				case Key_DOT:
				case Key_BACK:
					break ;
				case Key_LOCK:
					if(lock_flag)
						lock_flag=0;
					else
						lock_flag=1;
					if(lock_flag)
					{
						Lock_Control_On();
						Lock_LedOn();
						
					}
					else
					{
						Lock_LedOff();
						Lock_Control_Off();
					
					
					}
					break;
				case Key_BIAS:
				case Key_REST:
				case Key_TRIG:
					
				break;
				default:
					
				break;
					
			}
		
		
		}
	
	
	}
//    UART_TxCmd(LPC_UART0, DISABLE);
}
// �б���ʾ
void ItemProcess(void)
{
	
	uint32_t keynum;
	uint8_t key;
	uint8_t Disp_flag=1;
//    uint8_t page=1;
	
	Button_Page_Typedef Button_Page;
	SaveData.Limit_ScanValue.num=0;
	Button_Page.index=0;
	Button_Page.page=0;
	Disp_List_Count_Item();
	Delay_Key();
 	while(GetSystemStatus()==SYS_STATUS_ITEM)
	{
		if(Disp_flag)
		{
			Disp_flag=0;
			Disp_LIMIT_ComDispValue(&Button_Page);
		}
		
		Disp_Scan_Compvalue(0);
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==KEY_NUM)
		{
			Disp_flag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1:
					
                    if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_TEST);
					}
					else if(Button_Page.index==1)
						SaveData.Limit_ScanValue.fun=0;
					
				break;
				case Key_F2:
					 
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_RANGE);
					
					}else if(Button_Page.index==1)
						SaveData.Limit_ScanValue.fun=1;
					
				break;
				case Key_F3:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_RANGECOUNT);
					
					}
				break;
				case Key_F4:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_ITEM);
					}
					else if(Button_Page.index==2)
					{
						if(SaveData.Limit_ScanValue.num<1)
							SaveData.Limit_ScanValue.num=20;
						else
							SaveData.Limit_ScanValue.num--;
					
					}
				break;
				case Key_F5:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							Button_Page.page=1;
						else
							Button_Page.page=0;
						Disp_Button_value1(Button_Page.page);
					}
					else if(Button_Page.index==2)
					{
						if(SaveData.Limit_ScanValue.num>=20)
							SaveData.Limit_ScanValue.num=0;
						else
							SaveData.Limit_ScanValue.num++;
					
					}
				break;
				case Key_Disp:
					
                    SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
					
                    SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
					
				break;
				case Key_LEFT:
					
				break;
				case Key_RIGHT:
					
				break;
				case Key_UP:
					
					if(Button_Page.index>0)
						Button_Page.index--;
				break;
				case Key_DOWN:
					
					if(Button_Page.index<2)
						Button_Page.index++;
				break;
				case Key_NUM1:
				case Key_NUM2:
				case Key_NUM3:
				case Key_NUM4:
				case Key_NUM5:
				case Key_NUM6:
				case Key_NUM7:
				case Key_NUM8:
				case Key_NUM9:
				case Key_NUM0:
				case Key_DOT:
				case Key_BACK:
				case Key_LOCK:
				case Key_BIAS:
				case Key_REST:
				case Key_TRIG:
					
				break;
				default:
					
				break;
					
			}		
		}
	
	}
}
//�б�ɨ������
void Use_ITEMSetProcess(void)
{
	Disp_Coordinates_Typedef  Coordinates;
	uint32_t keynum=0;
	uint8_t key;
//    uint8_t page=0;
	uint8_t Disp_flag=1;
	Button_Page_Typedef Button_Page;
	
	Button_Page.index=0;
	Button_Page.page=0;
	Button_Page.third=0;
	Button_Page.force=0;
	SaveData.Limit_ScanValue.num=0;
	Disp_ListScan_Item();
	Delay_Key();
 	while(GetSystemStatus()==SYS_STATUS_ITEMSET)
	{
	  	
		if(Disp_flag)
		{
			Disp_flag=0;
		
			Disp_Scan_SetCompvalue(&Button_Page);
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
			Disp_flag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_SETUPTEST);
						else
							SetSystemStatus(SYS_STATUS_FILE);
					}
						
				break;
				case Key_F2:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_USERDEBUG);
						else
							SetSystemStatus(SYS_STATUS_SYSSET);
					}
				break;
				case Key_F3://
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_LIMITSET);
						else
							SetSystemStatus(SYS_STATUS_TOOL);
					}
				break;
				case Key_F4:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_ITEMSET);
					}else 
					if(Button_Page.index>1)
					{
						if(Button_Page.force>0)
							Button_Page.force--;
					
					}
					
				break;
				case Key_F5:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page)
							Button_Page.page=0;
						else
							Button_Page.page=1;
						Disp_Button_TestSet(Button_Page.page);
					}else
					if(Button_Page.force<20)
						Button_Page.force++;
				break;
				case Key_Disp:
					
                    SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
					
                    SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
					
				break;
				case Key_LEFT:
					
					if(Button_Page.third>0)
						Button_Page.third--;
						
					
				break;
				case Key_RIGHT:
					
					if(Button_Page.third<5)
						Button_Page.third++;
					else
						Button_Page.third=5;
				break;
				case Key_UP:
					
					if(Button_Page.index>0)
						Button_Page.index--;
					else
						Button_Page.page=0;
						
				break;
				case Key_DOWN:
					
					if(Button_Page.index<12)
						Button_Page.index++;
					else
						Button_Page.index=12;
				break;
				case Key_NUM1:
				//break;
				case Key_NUM2:
				//break;
				case Key_NUM3:
				//break;
				case Key_NUM4:
				//break;
				case Key_NUM5:
				//break;
				case Key_NUM6:
				//break;
				case Key_NUM7:
				//break;
				case Key_NUM8:
				//break;
				case Key_NUM9:
				//break;
				case Key_NUM0:
				//break;
				case Key_DOT:
				//break;
					
					if(Button_Page.index>2)
					{
						if(Button_Page.third==0)
						{
							Coordinates.xpos=40;//FIRSTLINE+SPACE1+3+ (i-2)*16
							Coordinates.ypos=FIRSTLINE+SPACE1+3+(Button_Page.index-2)*16;
							
							SaveData.Limit_ScanValue.freq[Button_Page.force*10+
							(Button_Page.index-3)]=Freq_Set_Num(&Coordinates);
						}else 
						if(Button_Page.third==1)
						{
						
						}else
						if(Button_Page.third==2)
						{
						
						}else
						if(Button_Page.third==3)
						{
						
						}else
						if(Button_Page.third==4)
						{
							Coordinates.xpos=420;
							Coordinates.ypos=FIRSTLINE+SPACE1+3+(Button_Page.index-2)*16;
							Coordinates.lenth=60;
							SaveData.Limit_ScanValue.time[Button_Page.force*10+
							(Button_Page.index-3)]=Disp_Set_Num(&Coordinates);
						
						
						}
					
					}
				case Key_BACK:
					
				break;
				case Key_LOCK:
					
				break;
				case Key_BIAS:
					
				break;
				case Key_REST:
					
				break;
				case Key_TRIG:
					
				break;
				default:
					
				break;
					
			}
		
		
		}
	
	
	}
}
//�������� 
void Use_LimitSetProcess(void)
{
	
	uint32_t keynum=0;
	uint8_t key,i;
	Disp_Coordinates_Typedef Coordinates;
//    uint8_t page=1;
	uint8_t Disp_flag=1;
	Button_Page_Typedef Button_Page;
	Button_Page.index=0;
	Button_Page.page=0;
	Disp_LimitList_Item();
	Delay_Key();
 	while(GetSystemStatus()==SYS_STATUS_LIMITSET)
	{
	  	if(Disp_flag)
		{
			Disp_flag=0;
			Disp_LimitSEt_value(& Button_Page);
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
			Disp_flag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_SETUPTEST);
						else
							SetSystemStatus(SYS_STATUS_FILE);
					
					}else if(Button_Page.index==1)
					;
					else if(Button_Page.index==2)
						;
					else if(Button_Page.index==3)
					{
						SaveData.Limit_Tab.Mode=0;
					
					}
					else if(Button_Page.index==4)
					{
						SaveData.Limit_Tab.Param=0;
					}
					else if(Button_Page.index==5)
					{
						SaveData.Limit_Tab.Comp=0;
					}else
					{
						if(Button_Page.index>15)
						{
						SaveData.Limit_Tab.Comp_Value[Button_Page.index-16].high.Num=0;
						SaveData.Limit_Tab.Comp_Value[Button_Page.index-16].low.Num=0;	
						}else if(Button_Page.index>5)
						{
						SaveData.Limit_Tab.Comp_Value[Button_Page.index-6].high.Num=0;
						SaveData.Limit_Tab.Comp_Value[Button_Page.index-6].low.Num=0;	
						
						}					
					}
						
				break;
				case Key_F2:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_USERDEBUG);
						else
							SetSystemStatus(SYS_STATUS_SYSSET);
					}
					else if(Button_Page.index==1)
					;
					else if(Button_Page.index==2)
						;
					else if(Button_Page.index==3)
					{
						SaveData.Limit_Tab.Mode=1;
					
					}
					else if(Button_Page.index==4)
					{
						SaveData.Limit_Tab.Param=1;
					}
					else if(Button_Page.index==5)
					{
						SaveData.Limit_Tab.Comp=1;
					}
				break;
				case Key_F3:
					
					if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_LIMITSET);
						else
							SetSystemStatus(SYS_STATUS_TOOL);
					}
				break;
				case Key_F4:
					
				    if(Button_Page.index==0)
					{
						if(Button_Page.page==0)
							SetSystemStatus(SYS_STATUS_SYSSET);
					}
	
//					if(Button_Page.index==0)
//					{
//						if(Button_Page.page==0)
//							SetSystemStatus(SYS_STATUS_ITEMSET);
//					}
					
				break;
				case Key_F5:
					
					if(Button_Page.index==0)
					{
//						if(Button_Page.page)
//							Button_Page.page=0;
//						else
//						Button_Page.page=1;
//						Disp_Button_TestSet(Button_Page.page);
					}else if(Button_Page.index>5)
					{
						for(i=0;i<10;i++)
						{
							SaveData.Limit_Tab.Comp_Value[i].low.Num=0;
							SaveData.Limit_Tab.Comp_Value[i].high.Num=0;
							
						}
						Button_Page.index=0;
					
					
					}
				break;
				case Key_Disp:
					
                    SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
					
                    SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
					
				break;
				case Key_LEFT:
					
					if(Button_Page.index!=0)
					{
						if(Button_Page.index<=5)
						{
							Button_Page.index--;
						
						}
						else
						{
							if(Button_Page.index>15)
								Button_Page.index-=10;
							else
								Button_Page.index+=9;
						}
					
					
					}
					
				break;
				case Key_RIGHT:
					
					if(Button_Page.index!=0)
					{
//						if(Button_Page.index<=5)
//						{
//							Button_Page.index++;
//						
//						}
//						else
						//if(Button_Page.index)
						{
							if(Button_Page.index>5&Button_Page.index<16)
								Button_Page.index+=10;
							else if(Button_Page.index > 15){
								Button_Page.index-=9;
							}else{
								//if(Button_Page.index<)
								Button_Page.index+=1;
							}
						}
					
					
					}
				break;
				case Key_UP:
					
					if(Button_Page.index>16)
						Button_Page.index--;
					else if(Button_Page.index==16)
						Button_Page.index=5;
					else
						
					if(Button_Page.index>0)
						Button_Page.index--;
					else if(Button_Page.index==0)
						Button_Page.index=15;
				break;
				case Key_DOWN:
					
					if(Button_Page.index<15)
						Button_Page.index++;
					else if(Button_Page.index==15)
						Button_Page.index=0;
					else
						
					if(Button_Page.index<25)
						Button_Page.index++;
					else
						Button_Page.index=0;
						
				break;
				case Key_NUM1:
//				break;
				case Key_NUM2:
//				break;
				case Key_NUM3:
//				break;
				case Key_NUM4:
//				break;
				case Key_NUM5:
//				break;
				case Key_NUM6:
//				break;
				case Key_NUM7:
//				break;
				case Key_NUM8:
//				break;
				case Key_NUM9:
//				break;
				case Key_NUM0:
//				break;
				case Key_DOT:
					
					if(Button_Page.index==2)//LIST2-24, FIRSTLINE,
					{
							Coordinates.xpos=LIST2-24;//FIRSTLINE+SPACE1+3+ (i-2)*16
							Coordinates.ypos=FIRSTLINE-2;
							Coordinates.lenth=66;
							SaveData.Limit_Tab.Nom=Disp_Set_InputNum(&Coordinates);
					}else if(Button_Page.index>5&&Button_Page.index<=14)
						{
							//LIST2-90, 76+(i-6)*15
							Coordinates.xpos=LIST2-90;//FIRSTLINE+SPACE1+3+ (i-2)*16
							Coordinates.ypos=76+(Button_Page.index-6)*15;
							Coordinates.lenth=60;
							SaveData.Limit_Tab.Comp_Value[Button_Page.index-6].low=Disp_Set_InputpreNum(&Coordinates);
							SaveData.Limit_Tab.Comp_Value[Button_Page.index-6].high=SaveData.Limit_Tab.Comp_Value[Button_Page.index-6].low;
							//Sort_TypeDef Disp_Set_InputpreNum(Disp_Coordinates_Typedef *Coordinates)
						}else if(Button_Page.index>15 && Button_Page.index<25)
						{
							Coordinates.xpos=LIST2+70;//FIRSTLINE+SPACE1+3+ (i-2)*16
							Coordinates.ypos=76+(Button_Page.index-16)*15;
							Coordinates.lenth=60;
							SaveData.Limit_Tab.Comp_Value[Button_Page.index-16].high=Disp_Set_InputpreNum(&Coordinates);
						}else if(Button_Page.index==15)
						{
							Coordinates.xpos=LIST2-90;//FIRSTLINE+SPACE1+3+ (i-2)*16
							Coordinates.ypos=76+(Button_Page.index-6)*15;
							Coordinates.lenth=60;
							SaveData.Limit_Tab.Comp_Value[Button_Page.index-6].low=Disp_Set_InputpreSecNum(&Coordinates);							
							SaveData.Limit_Tab.Comp_Value[Button_Page.index-6].high=SaveData.Limit_Tab.Comp_Value[Button_Page.index-6].low;
						}else if(Button_Page.index==25)
						{
							Coordinates.xpos=LIST2+70;//FIRSTLINE+SPACE1+3+ (i-2)*16
							Coordinates.ypos=76+(Button_Page.index-16)*15;
							Coordinates.lenth=60;
							SaveData.Limit_Tab.Comp_Value[Button_Page.index-16].high=Disp_Set_InputpreSecNum(&Coordinates);
						}
						do{
							key=HW_KeyScsn();
						}
						while(key!=0xff);
						
				break;
				case Key_BACK:
					
				break;
				case Key_LOCK:
					
				break;
				case Key_BIAS:
					
				break;
				case Key_REST:
					
				break;
				case Key_TRIG:
					
				break;
				default:
					
				break;
					
			}
		
		
		}
	
	
	}
	Set_Compbcd_float();
	Savetoeeprom();
}



//ϵͳ����
void Use_SysSetProcess(void)
{
	uint32_t  passwordnum=123769;
	uint32_t keynum=0;
	uint8_t key;	//,i=5
	uint8_t Disp_flag=1;
    RTC_TIME_Type Set_Time;
	Button_Page_Typedef Button_Page;
	SaveData.Sys_Setup.Cipher=0;
	Button_Page.index=0;
	Button_Page.page=0;
    lcd_Clear(LCD_COLOR_TEST_BACK);
	Disp_Sys_Item();
	Delay_Key();
 	while(GetSystemStatus()==SYS_STATUS_SYSSET)
	{
	  	if(spintest != 0)
		{
			Key_Beep();
			if(spintest == 1)
			{
				if(Button_Page.page==0)
				{
					if(Button_Page.index>15)
						Button_Page.index=0;
					else if(Button_Page.index<8)
						Button_Page.index++;
					else if(Button_Page.index==14)
						Button_Page.index++;
					else if(Button_Page.index==15)
						Button_Page.index++;
					else
						Button_Page.index = 14;
				}
				else if(Button_Page.page==1)
				{
					if(Button_Page.index>7)
						Button_Page.index=0;
					else
						Button_Page.index++;					
				}
				else
				{
					if(Button_Page.index>4)
						Button_Page.index=0;
					else
						Button_Page.index++;	
				
				}
			}else if(spintest == 2){
				if(Button_Page.page==0)
				{
					if(Button_Page.index<1)
						Button_Page.index=16;
					else if(Button_Page.index<9)
						Button_Page.index--;
					else if(Button_Page.index==16)
						Button_Page.index--;
					else if(Button_Page.index==15)
						Button_Page.index--;
					else if(Button_Page.index==14)
						Button_Page.index=8;
				}
				else if(Button_Page.page==1)
				{
					if(Button_Page.index<1)
						Button_Page.index=8;
					else
						Button_Page.index--;
				
				}
				else
				{
				
					if(Button_Page.index<1)
						Button_Page.index=5;
					else
						Button_Page.index--;
				
				}
			}
			
			spintest = 0;
			Disp_flag=1;
		}
		
		if(Disp_flag==1||Rtc_intflag)
		{
			Rtc_intflag=0;
			Disp_Sys_value(&Button_Page);
			Disp_flag=0;
		}
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==5)
		{
			Disp_flag=1;
			Key_Beep();
			Correc_successflag=0;
			switch(key)
			{
				case Key_F1:
					
					if(Button_Page.page==0)
					switch(Button_Page.index)
					{ 
						case 0:
//							if(Button_Page.page==0)
								SetSystemStatus(SYS_STATUS_TEST);
//							else
//							{
//								SetSystemStatus(SYS_STATUS_SYSSET);
//								Button_Page.index=0;
//								Button_Page.page=0;								
//							}
							break;
						case 1:
							break;
						case 2:
							SaveSIM.keybeep=0;
							break;
						case 3:
							Saveeeprom.Sys_set.U_flag=0;
							break;
						case 4:
							SaveSIM.lang=0;
							break;
//						case 5:
//							SaveData.Sys_Setup.Password=1;
//							break;
						case 5:
							SaveSIM.Iraly = 0;
							break;
						case 6:
							if(RTC_TIME_DISP.YEAR<1)
								RTC_TIME_DISP.YEAR=RTC_YEAR_MAX;
							else
								RTC_TIME_DISP.YEAR--;
                            RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, RTC_TIME_DISP.YEAR);
                            
                            RTC_CalibConfig(LPC_RTC, 0, RTC_CALIB_DIR_FORWARD);
                            RTC_CalibCounterCmd(LPC_RTC, ENABLE);
							break;
						case 7:
							if(RTC_TIME_DISP.MONTH<=1)
								RTC_TIME_DISP.MONTH=RTC_MONTH_MAX;
							else
								RTC_TIME_DISP.MONTH--;
                            RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, RTC_TIME_DISP.MONTH);
                            
                            RTC_CalibConfig(LPC_RTC, 0, RTC_CALIB_DIR_FORWARD);
                            RTC_CalibCounterCmd(LPC_RTC, ENABLE);
							break;
						case 8:
							if(RTC_TIME_DISP.DOM<=1)
								RTC_TIME_DISP.DOM=RTC_DAYOFMONTH_MAX;
							else
								RTC_TIME_DISP.DOM--;
                            RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, RTC_TIME_DISP.DOM);
                            
                            RTC_CalibConfig(LPC_RTC, 0, RTC_CALIB_DIR_FORWARD);
                            RTC_CalibCounterCmd(LPC_RTC, ENABLE);
							break;
//						case 10:
//							break ;
						case 9:
							if(Saveeeprom.Sys_set.buard<1)
								Saveeeprom.Sys_set.buard=5;
							else
								Saveeeprom.Sys_set.buard--;
							break;
						case 10:
							SaveData.Sys_Setup.Bias=0;
							break;
						case 11:
							SaveData.Sys_Setup.Bus_Mode=0;
							break;
						case 12:
							if(SaveData.Sys_Setup.GP_Addr<1)
								SaveData.Sys_Setup.GP_Addr=99;
							else
								SaveData.Sys_Setup.GP_Addr--;
							break;
						case 13:
							SaveData.Sys_Setup.Talk_Only=1;
							break;
						case 14:
							if(RTC_TIME_DISP.HOUR<1)
								RTC_TIME_DISP.HOUR=RTC_HOUR_MAX;
							else
								RTC_TIME_DISP.HOUR--;
                             RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR, RTC_TIME_DISP.HOUR);
                            
                            RTC_CalibConfig(LPC_RTC, 0, RTC_CALIB_DIR_FORWARD);
                            RTC_CalibCounterCmd(LPC_RTC, ENABLE);
							break;
						case 15:
							if(RTC_TIME_DISP.MIN<1)
								RTC_TIME_DISP.MIN=RTC_MINUTE_MAX;
							else
								RTC_TIME_DISP.MIN--;
                            RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, RTC_TIME_DISP.MIN);
                            
                            RTC_CalibConfig(LPC_RTC, 0, RTC_CALIB_DIR_FORWARD);
                            RTC_CalibCounterCmd(LPC_RTC, ENABLE);
							break;
						case 16:
							if(RTC_TIME_DISP.SEC<1)
								RTC_TIME_DISP.SEC=RTC_SECOND_MAX;
							else
								RTC_TIME_DISP.SEC--;
                            RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, RTC_TIME_DISP.SEC);
                            
                            RTC_CalibConfig(LPC_RTC, 2, RTC_CALIB_DIR_FORWARD);
                            RTC_CalibCounterCmd(LPC_RTC, ENABLE);
							break;
						default:
							break;
					
					}
					
					else  if(Button_Page.page==1)
						switch(Button_Page.index)
					{ 
						case 0:
							break ;
						case 1:
						case 2:										  
						case 3:
						case 4:
						case 5:							
						case 6:					
						case 7:
						case 8:
//							Uart_Send_Flag=2;
							Correction_C(&Button_Page);		
							Correc_successflag=1;
							break;
						default:
							break;						
					}
					else  if(Button_Page.page==2)
						switch(Button_Page.index)
						{
							case 0:
								 break ;
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
	//							Uart_Send_Flag=4;
								Correction_R(&Button_Page);	
								Correc_successflag=1;
								break ;
								default :
									break ;
						}
					else
					{
						Button_Page.page=1;
						Button_Page.index=0;
						Disp_Serve_correction();
						Init_C();							
					}
						
                    
				break;
				case Key_F2:
					
					if(Button_Page.page==0)
					switch(Button_Page.index)
					{
						case 0:
							SetSystemStatus(SYS_STATUS_SETUPTEST);
						//	SetSystemStatus(SYS_STATUS_SETUPTEST);
							break;
						case 1:
							break;
						case 2:
							SaveSIM.keybeep=1;
							break;
						case 3:
							Saveeeprom.Sys_set.U_flag=1;
							break;
						case 4:
							SaveSIM.lang=1;
							break;
//						case 5:
//							SaveData.Sys_Setup.Password=0;
//							break;
						case 5:
							SaveSIM.Iraly = 1;
							break;
						case 6:
							if(RTC_TIME_DISP.YEAR>RTC_YEAR_MAX)
								RTC_TIME_DISP.YEAR=0;
							else
								RTC_TIME_DISP.YEAR++;
                            RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, RTC_TIME_DISP.YEAR);
                            
                            RTC_CalibConfig(LPC_RTC, 0, RTC_CALIB_DIR_FORWARD);
                            RTC_CalibCounterCmd(LPC_RTC, ENABLE);
							break;
						case 7:
							if(RTC_TIME_DISP.MONTH>=RTC_MONTH_MAX)
								RTC_TIME_DISP.MONTH=1;
							else
								RTC_TIME_DISP.MONTH++;
                            RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, RTC_TIME_DISP.MONTH);
                            
                            RTC_CalibConfig(LPC_RTC, 0, RTC_CALIB_DIR_FORWARD);
                            RTC_CalibCounterCmd(LPC_RTC, ENABLE);
							break;
						case 8:
							if(RTC_TIME_DISP.DOM>=RTC_DAYOFMONTH_MAX)
								RTC_TIME_DISP.DOM=1;
							else
								RTC_TIME_DISP.DOM++;
                            RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, RTC_TIME_DISP.DOM);
                            
                            RTC_CalibConfig(LPC_RTC, 0, RTC_CALIB_DIR_FORWARD);
                            RTC_CalibCounterCmd(LPC_RTC, ENABLE);
							break;
//						case 9:
//							break ;							
						case 9:
							if(Saveeeprom.Sys_set.buard>4)
								Saveeeprom.Sys_set.buard=0;
							else
								Saveeeprom.Sys_set.buard++;
							break;
						case 10:
							SaveData.Sys_Setup.Bias=1;
							break;
						case 11:
							SaveData.Sys_Setup.Bus_Mode=1;
							break;
						case 12:
							if(SaveData.Sys_Setup.GP_Addr>99)
								SaveData.Sys_Setup.GP_Addr=0;
							else
								SaveData.Sys_Setup.GP_Addr++;
							break;
						case 13:
							SaveData.Sys_Setup.Talk_Only=0;
							break;
						case 14:
							if(RTC_TIME_DISP.HOUR>=RTC_HOUR_MAX)
								RTC_TIME_DISP.HOUR=0;
							else
								RTC_TIME_DISP.HOUR++;
                            RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR, RTC_TIME_DISP.HOUR);
                            RTC_CalibConfig(LPC_RTC, 0, RTC_CALIB_DIR_FORWARD);
                            RTC_CalibCounterCmd(LPC_RTC, ENABLE);
							break;
						case 15:
							if(RTC_TIME_DISP.MIN>=RTC_MINUTE_MAX)
								RTC_TIME_DISP.MIN=0;
							else
								RTC_TIME_DISP.MIN++;
                            RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, RTC_TIME_DISP.MIN);
                            
                            RTC_CalibConfig(LPC_RTC, 0, RTC_CALIB_DIR_FORWARD);
                            RTC_CalibCounterCmd(LPC_RTC, ENABLE);
							break;
						case 16:
							if(RTC_TIME_DISP.SEC>=RTC_SECOND_MAX)
								RTC_TIME_DISP.SEC=0;
							else
								RTC_TIME_DISP.SEC++;
                            RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, RTC_TIME_DISP.SEC);
                            
                            RTC_CalibConfig(LPC_RTC, 2, RTC_CALIB_DIR_FORWARD);
                            RTC_CalibCounterCmd(LPC_RTC, ENABLE);
							break;
						default:
							break;
					
					}
					else if(Button_Page.page==3)
					{
							Button_Page.page=2;
							Button_Page.index=0;
							Disp_Serve_correctionR();
							Init_R();	
					}
					else if(Button_Page.page==1)
						switch(Button_Page.index)
						{
							case 0:
								break ;
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
							case 6:					
						    case 7:
						    case 8:
								Save_C();
								Button_Page.page=3;
								Button_Page.index=0;
								Disp_Start_Corr();
								break ;
								default :
									break ;
						}
//						if(Button_Page.index!=0)
//						{
//							Save_C();
//							Button_Page.page=3;
//							Button_Page.index=0;
//							Disp_Start_Corr();
//						}
					else  if(Button_Page.page==2)
					switch(Button_Page.index)
					{ 
						case 0:
							break ;
						case 1:
						case 2:										  
						case 3:
						case 4:
						case 5:							
							Save_R();
							Button_Page.page=3;
							Button_Page.index=0;
							Disp_Start_Corr();
							break;
						default:
							break;						
					}
//						if(Button_Page.index!=0)
//						{
//							Save_R();
//							Button_Page.page=3;
//							Button_Page.index=0;
//							Disp_Start_Corr();
//						}					                   
				break;
				case Key_F3:
					
					if(Button_Page.page==0)
					switch(Button_Page.index)
					{
						case 5:
							SaveSIM.Iraly = 2;
							break;
						case 11:
							SaveData.Sys_Setup.Bus_Mode=2;
							
						break;
						default:
						break;
					
					}
					else  if(Button_Page.page==3)
						switch(Button_Page.index)
					{
						case 0:
//						   Uart_Send_Flag=5;
//						Save_R();
//					INSTRUCT_TO_DEBUG;
						Exit_correction();
						INSTRUCT_TO_DEBUG
						Exit_correction();
						SetSystemStatus(SYS_STATUS_TEST);
						break;
						default:
						break;
					 
					}
						
						
				break;
				case Key_F4:
					
					if(Button_Page.page==0)
					switch(Button_Page.index)
					{
						case 0:
							break;//�ָ�Ĭ������
						case 11:
							SaveData.Sys_Setup.Bus_Mode=3;
							
						break;
						default:
						break;
					
					}
				break;
				case Key_F5:
					
					if(Button_Page.page==0)
					switch(Button_Page.index)
					{
						case 0:
							break;//�ָ�ϵͳ��λ
						case 11:
							SaveData.Sys_Setup.Bus_Mode=4;
							
						break;
						default:
						break;
					
					}
					else if(Button_Page.page==3)
					{	
						Exit_correction();
						LONGDELAY;
						Clear_Instr();
					}
				break;
				case Key_Disp:
					
					if(Button_Page.page==3)	
//						Uart_Send_Flag=6;
					{	
						Exit_correction();
						INSTRUCT_TO_DEBUG
						Exit_correction();
						SetSystemStatus(SYS_STATUS_TEST);
					
					}
					else if(Button_Page.page==0)
						SetSystemStatus(SYS_STATUS_TEST);

				break;
				case Key_SETUP:					
					
					 if(Button_Page.page==3)
//						Uart_Send_Flag=5; 
					 {
						Exit_correction();
						INSTRUCT_TO_DEBUG 
						Exit_correction();
						SetSystemStatus(SYS_STATUS_SETUPTEST);
						 
					 }
					 else  if(Button_Page.page==0)
						SetSystemStatus(SYS_STATUS_SETUPTEST); 

				break;
				case Key_FAST:
					
				break;
				case Key_UP:
				{
					switch(Button_Page.index)
					{ 
						case 2:
						{
							if(SaveSIM.keybeep == 1)
							{
								SaveSIM.keybeep = 0;
							}else{
								SaveSIM.keybeep = 1;
							}
						}break;
						case 4:
						{
							if(SaveSIM.lang == 1)
							{
								SaveSIM.lang = 0;
							}else{
								SaveSIM.lang = 1;
							}
						}break;
					}
				}break;
//				case Key_LEFT:
//					
//					if(Button_Page.page==0)
//					{
//						if(Button_Page.index>8&&Button_Page.index<14)
//							Button_Page.index-=8;
//						else if(Button_Page.index==15||Button_Page.index==16)
//							Button_Page.index-=1;
//						else if(Button_Page.index==7||Button_Page.index==8)
//							Button_Page.index-=1;
//						else if(Button_Page.index==14)
//							Button_Page.index-=6;
//                        else if(Button_Page.index==1)
//                            Button_Page.index=16;
////                            
////                        else
////                            Button_Page.index+=7;
//                            
//				    }
//				break;
//				case Key_RIGHT:
//					
//					if(Button_Page.page==0)
//					{
//						if(Button_Page.index<6)
//							Button_Page.index = 14;
//						else if(Button_Page.index<8)	//Button_Page.index==7||Button_Page.index==8
//							Button_Page.index+=1;
//						else if(Button_Page.index==14||Button_Page.index==15)
//							Button_Page.index+=1;
//						else if(Button_Page.index==8)
//							Button_Page.index+=6;
//                        else if(Button_Page.index==16)
//                            Button_Page.index=1;
//                        else
//                            Button_Page.index=1;
//					}
//				break;
//				case Key_DOWN:
//					
//					if(Button_Page.page==0)
//					{
//						if(Button_Page.index>15)
//							Button_Page.index=0;
//						else if(Button_Page.index<8)
//							Button_Page.index++;
//						else if(Button_Page.index==14)
//							Button_Page.index++;
//						else if(Button_Page.index==15)
//							Button_Page.index++;
//						else
//							Button_Page.index = 14;
//					}
//					else if(Button_Page.page==1)
//					{
//						if(Button_Page.index>7)
//							Button_Page.index=0;
//						else
//							Button_Page.index++;					
//					}
//					else
//					{
//						if(Button_Page.index>4)
//							Button_Page.index=0;
//						else
//							Button_Page.index++;	
//					
//					}
//				break;
//				case Key_UP:
//					
//					if(Button_Page.page==0)
//					{
//						if(Button_Page.index<1)
//							Button_Page.index=16;
//						else if(Button_Page.index<9)
//							Button_Page.index--;
//						else if(Button_Page.index==16)
//							Button_Page.index--;
//						else if(Button_Page.index==15)
//							Button_Page.index--;
//						else if(Button_Page.index==14)
//							Button_Page.index=8;
//				    }
//					else if(Button_Page.page==1)
//					{
//						if(Button_Page.index<1)
//							Button_Page.index=8;
//						else
//							Button_Page.index--;
//					
//					}
//					else
//					{
//					
//						if(Button_Page.index<1)
//							Button_Page.index=5;
//						else
//							Button_Page.index--;
//					
//					}
//				break;
				case Key_NUM1:		//passwordnum
					
					if(Button_Page.index==10)
					if(SaveData.Sys_Setup.Cipher<100000)
					    SaveData.Sys_Setup.Cipher=SaveData.Sys_Setup.Cipher*10+1;
				break;
				case Key_NUM2:
					
					if(Button_Page.index==10)
					if(SaveData.Sys_Setup.Cipher<100000)
						SaveData.Sys_Setup.Cipher=SaveData.Sys_Setup.Cipher*10+2;
				break;
				case Key_NUM3:
					
					if(Button_Page.index==10)
					if(SaveData.Sys_Setup.Cipher<100000)
					    SaveData.Sys_Setup.Cipher=SaveData.Sys_Setup.Cipher*10+3;
				break;
				case Key_NUM4:
					
					if(Button_Page.index==10)
					if(SaveData.Sys_Setup.Cipher<100000)
						SaveData.Sys_Setup.Cipher=SaveData.Sys_Setup.Cipher*10+4;
				break;
				case Key_NUM5:
					
					if(Button_Page.index==10)
					if(SaveData.Sys_Setup.Cipher<100000)
						SaveData.Sys_Setup.Cipher=SaveData.Sys_Setup.Cipher*10+5;
				break;
				case Key_NUM6:
					
					if(Button_Page.index==10)
					if(SaveData.Sys_Setup.Cipher<100000)
						SaveData.Sys_Setup.Cipher=SaveData.Sys_Setup.Cipher*10+6;
				break;
				case Key_NUM7:
					
					if(Button_Page.index==10)
					if(SaveData.Sys_Setup.Cipher<100000)
						SaveData.Sys_Setup.Cipher=SaveData.Sys_Setup.Cipher*10+7;
				break;
				case Key_NUM8:
					
					if(Button_Page.index==10)
					if(SaveData.Sys_Setup.Cipher<100000)
						SaveData.Sys_Setup.Cipher=SaveData.Sys_Setup.Cipher*10+8;
				break;
				case Key_NUM9:
					
					if(Button_Page.index==10)
					if(SaveData.Sys_Setup.Cipher<100000)
						SaveData.Sys_Setup.Cipher=SaveData.Sys_Setup.Cipher*10+9;
					if(passwordnum==SaveData.Sys_Setup.Cipher)
					{
						
						Button_Page.index=0;
						Button_Page.page=3;
						Disp_Start_Corr();
//						Disp_Serve_correction();
//						Uart_Send_Flag=1;
						Start_Correction();
					}
				break;
				case Key_NUM0:
					
					if(Button_Page.index==10)
					if(SaveData.Sys_Setup.Cipher<100000)
						SaveData.Sys_Setup.Cipher=SaveData.Sys_Setup.Cipher*10+0;
				break;
				case Key_DOT:
					
				break;
				case Key_BACK:
					
				break;
				case Key_LOCK:
					
				break;
				case Key_BIAS:
					
				break;
				case Key_REST:
					
				break;
				case Key_TRIG:
					
				break;
				default:
					
				break;
					
			}
		
		
		}
	
	
	}
    Savetoeeprom();
}

//�û�У��
void Use_DebugProcess(void)
{
	
	uint32_t keynum=0;
	uint8_t key;
//    uint8_t page=1;
	uint8_t Disp_flag=1;
	Button_Page_Typedef Button_Page;
	Button_Page.index=0;
	Button_Page.page=0;
	Disp_UserCheck_Item();
	Delay_Key();
 	while(GetSystemStatus()==SYS_STATUS_USERDEBUG)
	{
		if(Disp_flag==1)
		{
			Disp_Debug_value(&Button_Page);
			Disp_flag=0;	
		}
	  	
//			if(timer0_counter>0)//��������
//		{
//			switch(Uart_Send_Flag)
//			{
//				case 0:
//					break;
//				case 1:	
//					 Open_Clear();	
//					break;
//				case 2:	
//                    Open_Clear();
//					//Short_Clear();
//					break;
//				case 3:	
//					Close_Clear();
//					break;
//				default:
//					break;
//			
//			}
//			
//			Uart_Send_Flag=0;
//			timer0_counter=0;		
//		
//		}
		
		
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==KEY_NUM)
		{
			Disp_flag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1:
					
                    if(Button_Page.page==0)
					 switch(Button_Page.index)
						{
							case 0:
								SetSystemStatus(SYS_STATUS_SETUPTEST);
								break;
							case 1:	
								SaveData.User_Correction.Open=0;
								Uart_Send_Flag=3;
								break;
							case 2:	
								SaveData.User_Correction.Short=0;
								Uart_Send_Flag=3;
								break;
							default:
								break;
						
						}
//                        SetSystemStatus(SYS_STATUS_SETUPTEST);
//					else
//						SetSystemStatus(SYS_STATUS_FILE);
						
				break;
				case Key_F2:
					
                    if(Button_Page.page==0)
						switch(Button_Page.index)
						{
							case 0:
								SetSystemStatus(SYS_STATUS_USERDEBUG);
								break;
							case 1:	
								SaveData.User_Correction.Open=1;
								Uart_Send_Flag=1;
								break;
							case 2:	
								SaveData.User_Correction.Short=1;
								Uart_Send_Flag=2;
								break;
							default:
								break;
						
						}
//                        SetSystemStatus(SYS_STATUS_USERDEBUG);
//					else
//						SetSystemStatus(SYS_STATUS_SYSSET);
				break;
				case Key_F3:
					
                    if(Button_Page.page==0)
                        SetSystemStatus(SYS_STATUS_LIMITSET);
					else
						SetSystemStatus(SYS_STATUS_TOOL);
				break;
				case Key_F4:
					
                    if(Button_Page.page==0)
                        SetSystemStatus(SYS_STATUS_SYSSET);					
//                    if(Button_Page.page==0)
//                        SetSystemStatus(SYS_STATUS_ITEMSET);
				break;
				case Key_F5:
					
//					if(Button_Page.page)
//						Button_Page.page=0;
//					else
//						Button_Page.page=1;
//                    Disp_Button_TestSet(Button_Page.page);
				break;
				case Key_Disp:
					
                    SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
					
                    SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
					
				break;
				case Key_LEFT:
					
				break;
				case Key_RIGHT:
					
				break;
				case Key_UP:
					    
						if(Button_Page.index>0)
							Button_Page.index--;
						else
							Button_Page.index=1;
							
				break;
				case Key_DOWN:
					
					if(Button_Page.index>1)
						Button_Page.index=0;
					else
						Button_Page.index++;
				break;
				case Key_NUM1:
				//break;
				case Key_NUM2:
				//break;
				case Key_NUM3:
				//break;
				case Key_NUM4:
				//break;
				case Key_NUM5:
				//break;
				case Key_NUM6:
				//break;
				case Key_NUM7:
				//break;
				case Key_NUM8:
				//break;
				case Key_NUM9:
				//break;
				case Key_NUM0:
				//break;
				case Key_DOT:
					
//					if(Button_Page.index==5)
//					{ 	Disp_Coordinates_Typedef Coordinates;
//						Coordinates.xpos=LIST1+92;
//						Coordinates.ypos=FIRSTLINE+SPACE1*4;
//						Coordinates.lenth=86;
//						
//						SaveData.Main_Func.Freq=Freq_Set_Num(&Coordinates);
//					
//					}
				break;
				case Key_BACK:
					
				break;
				case Key_LOCK:
					
				break;
				case Key_BIAS:
					
				break;
				case Key_REST:
					
				break;
				case Key_TRIG:
					
				break;
				default:
					
				break;
					
			}
		
		
		}
	
	
	}
}

//��˾У��
void Fac_DebugProcess(void)
{
	
	uint32_t keynum=0;
	uint8_t key;
    uint8_t set_num;
    uint8_t num_flag=0;
    uint8_t debugbuff[20];
    uint8_t setupflag=0,sendnum;
    Disp_Coordinates_Typedef Coordinates;
	Disp_Coordinates_Typedef Debug_Cood;
//    uint8_t page=1;
	uint8_t Disp_flag=1;
	Button_Page_Typedef Button_Page;
	Button_Page.index=0;
	Button_Page.page=1;

    lcd_Clear(LCD_COLOR_TEST_BACK);
	Disp_FacrCheck_Item(&Button_Page);
	Delay_Key();
	Send_Request(2,1);
 	while(GetSystemStatus()==SYS_STATUS_FACRDEBUG)
	{
		if(spintest != 0)
		{
			Key_Beep();
			if(spintest == 1)
			{
				if(Button_Page.page == 1)
				{//��ѹУ��3��
					if(Button_Page.index>7)
						Button_Page.index=0;
					else
						Button_Page.index++;
					
					if(Button_Page.index == 1)
					{
						SaveSIM.Voltage.Num = 1000;
					}else if(Button_Page.index == 2)
					{
						SaveSIM.Voltage.Num = 10000;
					}else if(Button_Page.index == 3)
					{
						SaveSIM.Voltage.Num = 13000;
					}else if(Button_Page.index == 4)
					{
						SaveSIM.Voltage.Num = 18000;
					}else if(Button_Page.index == 5)
					{
						SaveSIM.Voltage.Num = 19000;
					}else if(Button_Page.index == 6)
					{
						SaveSIM.Voltage.Num = 29000;
					}else if(Button_Page.index == 7)
					{
						SaveSIM.Voltage.Num = 30000;
					}else if(Button_Page.index == 8)
					{
						SaveSIM.Voltage.Num = 39000;
					}
					Send_Request(10,1);
				}else if(Button_Page.page == 2){//��ѹ����4��
					if(Button_Page.index>7)
						Button_Page.index=0;
					else
						Button_Page.index++;
					
					if(Button_Page.index == 1)
					{
						SaveSIM.Voltage.Num = 1000;
					}else if(Button_Page.index == 2)
					{
						SaveSIM.Voltage.Num = 10000;
					}else if(Button_Page.index == 3)
					{
						SaveSIM.Voltage.Num = 15000;
					}else if(Button_Page.index == 4)
					{
						SaveSIM.Voltage.Num = 19000;
					}
					Send_Request(10,1);
				}else if(Button_Page.page == 3){//����У׼
					if(Button_Page.index>6)
						Button_Page.index=0;
					else
						Button_Page.index++;
					if(Button_Page.index == 1 ||Button_Page.index == 4)
					{
						Send_Request(4,0);//�����͵�λ
					}else{
						Send_Request(4,1);//�����ߵ�λ
					}
				}
			}else if(spintest == 2){
				if(Button_Page.page == 1)
				{//��ѹУ��3��
					if(Button_Page.index>0)
						Button_Page.index--;
					else
						Button_Page.index=8;
					
					if(Button_Page.index == 1)
					{
						SaveSIM.Voltage.Num = 1000;
					}else if(Button_Page.index == 2)
					{
						SaveSIM.Voltage.Num = 10000;
					}else if(Button_Page.index == 3)
					{
						SaveSIM.Voltage.Num = 13000;
					}else if(Button_Page.index == 4)
					{
						SaveSIM.Voltage.Num = 17000;
					}else if(Button_Page.index == 5)
					{
						SaveSIM.Voltage.Num = 19000;
					}else if(Button_Page.index == 6)
					{
						SaveSIM.Voltage.Num = 19500;
					}
					Send_Request(10,1);
				}else if(Button_Page.page == 2){//��ѹ����4��
					if(Button_Page.index>0)
						Button_Page.index--;
					else
						Button_Page.index=8;
					
					if(Button_Page.index == 1)
					{
						SaveSIM.Voltage.Num = 1000;
					}else if(Button_Page.index == 2)
					{
						SaveSIM.Voltage.Num = 10000;
					}else if(Button_Page.index == 3)
					{
						SaveSIM.Voltage.Num = 15000;
					}else if(Button_Page.index == 4)
					{
						SaveSIM.Voltage.Num = 19000;
					}
					Send_Request(10,1);
				}else if(Button_Page.page == 3){//��������
					if(Button_Page.index>0)
						Button_Page.index--;
					else
						Button_Page.index=6;
					
					if(Button_Page.index == 1 ||Button_Page.index == 4)
					{
						Send_Request(4,0);//�����͵�λ
					}else{
						Send_Request(4,1);//�����ߵ�λ
					}
				}
			}
			
			spintest = 0;
			Disp_flag=1;
		}
		
		if(Disp_flag==1)
		{
      Disp_FacCal(&Button_Page);
			Disp_flag=0;	
		}	
		
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
        if(debug_over==0)	
		if(keynum==KEY_NUM)
		{
			Disp_flag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1://AB 01 08 06 E0 00 00 BF
					Button_Page.page = 1;
					Disp_FacrCheck_Item(&Button_Page);	
					Send_Request(2,1);
				break;
				case Key_F2:
					Button_Page.page = 2;
					Disp_FacrCheck_Item(&Button_Page);
					Send_Request(2,2);
				break;
				case Key_F3:
//					SaveSIM.Voltage.Num = 5000;
//					Send_Request(10,1);
					Button_Page.page = 3;
					
					Disp_FacrCheck_Item(&Button_Page);
					Send_Request(2,3);
				break;
				case Key_F4://У����������
					if(Button_Page.page == 3)
					{
						if(Button_Page.index == 1 || Button_Page.index == 4)
						{
							Send_Request(15,0);
						}else{
							Send_Request(15,1);
						}
					}
				break;
				case Key_F5:
					if(Button_Page.page == 1)
					{
						Send_Request(12,Button_Page.index);
					}else if(Button_Page.page == 2){
						Send_Request(14,Button_Page.index);
					}else if(Button_Page.page == 3){
						Send_Request(13,Button_Page.index);
					}
					
				break;
				case Key_Disp:
					Send_Request(2,0);
					SetSystemStatus(SYS_STATUS_TEST);
				break;
				case Key_SETUP:
					if(Button_Page.index==0)
                    {
                        Debug_Cood.xpos=70;
                        Debug_Cood.ypos =272-70;
                        Debug_Cood.lenth=120;
                        input_num(&Debug_Cood);
                    
                    }
				break;
				case Key_UP:
					SaveSIM.Version++;
					if(SaveSIM.Version>2)
						SaveSIM.Version=0;
					Savetoeeprom();
				break;
//				case Key_LEFT: 
//					
//				break;
//				case Key_RIGHT:
//					
//				break;
//				case Key_UP:
//					
//					if(Button_Page.page == 1)
//					{//��ѹУ��3��
//						if(Button_Page.index>0)
//							Button_Page.index--;
//						else
//							Button_Page.index=6;
//						
//						if(Button_Page.index == 1)
//						{
//							SaveSIM.Voltage.Num = 1000;
//						}else if(Button_Page.index == 2)
//						{
//							SaveSIM.Voltage.Num = 10000;
//						}else if(Button_Page.index == 3)
//						{
//							SaveSIM.Voltage.Num = 13000;
//						}else if(Button_Page.index == 4)
//						{
//							SaveSIM.Voltage.Num = 17000;
//						}else if(Button_Page.index == 5)
//						{
//							SaveSIM.Voltage.Num = 19000;
//						}else if(Button_Page.index == 6)
//						{
//							SaveSIM.Voltage.Num = 19500;
//						}
//						Send_Request(10,1);
//					}else if(Button_Page.page == 2){//��ѹ����4��
//						if(Button_Page.index>0)
//							Button_Page.index--;
//						else
//							Button_Page.index=4;
//						
//						if(Button_Page.index == 1)
//						{
//							SaveSIM.Voltage.Num = 1000;
//						}else if(Button_Page.index == 2)
//						{
//							SaveSIM.Voltage.Num = 10000;
//						}else if(Button_Page.index == 3)
//						{
//							SaveSIM.Voltage.Num = 15000;
//						}else if(Button_Page.index == 4)
//						{
//							SaveSIM.Voltage.Num = 19000;
//						}
//						Send_Request(10,1);
//					}else if(Button_Page.page == 3){//��������
//						if(Button_Page.index>0)
//							Button_Page.index--;
//						else
//							Button_Page.index=6;
//						
//						if(Button_Page.index == 1 ||Button_Page.index == 4)
//						{
//							Send_Request(4,0);//�����͵�λ
//						}else{
//							Send_Request(4,1);//�����ߵ�λ
//						}
//					}
//				break;
//				case Key_DOWN:
//					
//					if(Button_Page.page == 1)
//					{//��ѹУ��3��
//						if(Button_Page.index>6)
//							Button_Page.index=0;
//						else
//							Button_Page.index++;
//						
//						if(Button_Page.index == 1)
//						{
//							SaveSIM.Voltage.Num = 1000;
//						}else if(Button_Page.index == 2)
//						{
//							SaveSIM.Voltage.Num = 10000;
//						}else if(Button_Page.index == 3)
//						{
//							SaveSIM.Voltage.Num = 13000;
//						}else if(Button_Page.index == 4)
//						{
//							SaveSIM.Voltage.Num = 17000;
//						}else if(Button_Page.index == 5)
//						{
//							SaveSIM.Voltage.Num = 19000;
//						}else if(Button_Page.index == 6)
//						{
//							SaveSIM.Voltage.Num = 19500;
//						}
//						Send_Request(10,1);
//					}else if(Button_Page.page == 2){//��ѹ����4��
//						if(Button_Page.index>4)
//							Button_Page.index=0;
//						else
//							Button_Page.index++;
//						
//						if(Button_Page.index == 1)
//						{
//							SaveSIM.Voltage.Num = 1000;
//						}else if(Button_Page.index == 2)
//						{
//							SaveSIM.Voltage.Num = 10000;
//						}else if(Button_Page.index == 3)
//						{
//							SaveSIM.Voltage.Num = 15000;
//						}else if(Button_Page.index == 4)
//						{
//							SaveSIM.Voltage.Num = 19000;
//						}
//						Send_Request(10,1);
//					}else if(Button_Page.page == 3){//����У׼
//						if(Button_Page.index>6)
//							Button_Page.index=0;
//						else
//							Button_Page.index++;
//						if(Button_Page.index == 1 ||Button_Page.index == 4)
//						{
//							Send_Request(4,0);//�����͵�λ
//						}else{
//							Send_Request(4,1);//�����ߵ�λ
//						}
//					}
//				break;
				case Key_NUM1:
				//break;
				case Key_NUM2:
				//break;
				case Key_NUM3:
				//break;
				case Key_NUM4:
				//break;
				case Key_NUM5:
				//break;
				case Key_NUM6:
				//break;
				case Key_NUM7:
				//break;
				case Key_NUM8:
				//break;
				case Key_NUM9:
				//break;
				case Key_NUM0:
				//break;
				case Key_DOT:
					Coordinates.xpos=80;
					Coordinates.ypos=26+(Button_Page.index-1)*22;
					Coordinates.lenth=80;
					if(Button_Page.page == 1)
					{
						SaveSIM.CALV[Button_Page.index-1]=Disp_Set_Num(&Coordinates);
					}else if(Button_Page.page == 2){
						SaveSIM.CTRLV[Button_Page.index-1]=Disp_Set_Num(&Coordinates);
					}else if(Button_Page.page == 3){
						SaveSIM.CALI[Button_Page.index-1]=Disp_Set_C(&Coordinates);
					}
					Savetoeeprom();
					
				break;
				case Key_BACK:
					
				break;
				case Key_LOCK:
					
				break;
				case Key_BIAS:
					if(SaveSIM.JK5506 == 0)
					{
						SaveSIM.JK5506 = 1;
					}else if(SaveSIM.JK5506 == 1){
						SaveSIM.JK5506 = 0;
					}
				break;
				case Key_REST:
					if(SaveSIM.jkflag == 1)
					{
						SaveSIM.jkflag = 0;
					}else{
						SaveSIM.jkflag = 1;
					}
					Savetoeeprom();
				break;
				case Key_TRIG:
					if(mainswitch == 0)
					{
						mainswitch = 1;						
					}else{
						mainswitch = 0;
					}
					Send_Request(3,mainswitch);
				break;
				default:
					
				break;
					
			}
		
		
		}
	
	
	}
}	
//==========================================================
//�������ƣ�Uart_Process
//�������ܣ����ڴ���
//��ڲ�������
//���ڲ�������
//�������ڣ�2015.10.26 
//�޸����ڣ�2015.10.26 10:02
//��ע˵������
//==========================================================
uint16_t Uart_Process(uint8_t len,char* buf)
{
	uint16_t currCharNum;
    uint32_t temp1;
    uint8_t Gmode_Vale;
	uint8_t i,j;
	
	uint8_t addr;
	char str[30]={0};
	int cmd_flag=255;
	uint8_t templen =0;
	uint8_t tmpFg;
	uint8_t LM_S_Vale,LOW_I_Vale,H_L_Vale,SWITCH_Vale;
	uint16_t I_ADC_Vale,V_ADC_Vale;
	int32_t V_CS_Vale,I_CS_Vale;
	float Lvl_Vale,fadcx,fsingal, fVale;
 
//	uint8 len = 11;
	uint8_t pntlen = 0;
//  char StrON[2] = {"ON"};
	V_CS_Vale = V_CS;
	I_CS_Vale = I_CS;
	I_ADC_Vale = I_ADC;
	V_ADC_Vale = V_ADC;
    LM_S_Vale = LM_S;
    LOW_I_Vale   = LOW_I; 
    Gmode_Vale=Mode;
    H_L_Vale =  H_L;
	Lvl_Vale = LVL_DA;
    addr = 0;
    SWITCH_Vale=SWITCH_A;
	Lvl_Vale = 
	
	currCharNum=0;
	
	if((buf[currCharNum] != ChrStartR) || (buf[len-2] != ChrEndR)||(buf[len-1] != ChrEndS))
    {
		return SetErr_ACK(buf,addr ,CMD_ERR); 
    }
	if (ComBuf.rec.end)//�������ݽ���
	{
		
	}
}


void  Avg_Test(uint8_t *buff,uint8_t dot,uint8_t *buff1,uint8_t dot1)		//ƽ������ֵ
{
	uint8_t i;
//uint8_t  Avg_Test_Main[25][6];
//uint8_t  Avg_Test_Second[25][6];
//uint8_t  Avg_Count=0;
//	if(SaveData.Main_Func.Avg>1)
       for(i=0;i<5;i++)
		{
			Avg_Test_Main[Avg_Count-1][i]=buff[i];	
			Avg_Test_Second[Avg_Count-1][i]=buff1[i];
		}
			Avg_Test_Main[Avg_Count-1][5]=dot;
			Avg_Test_Second[Avg_Count-1][5]=dot;
}


//void  Avg_Test_All(void)
//{
//	uint32_t  Total_data_Main,Total_data_Second;
//	uint8_t i,j,dot_num_main,dot_num_second;
//	uint32_t Multiple_Main,Multiple_Second;	//����
//	
//	
//	for(i=0;i<SaveData.Main_Func.Avg-1;i++)
//	{
//		if(Avg_Test_Main[i][5]>Avg_Test_Main[i+1][5])
//			dot_num_main=Avg_Test_Main[i][5];
//		else
//			dot_num_main=Avg_Test_Main[i+1][5];
//		
//		if(Avg_Test_Second[i][5]>Avg_Test_Second[i+1][5])
//			dot_num_second=Avg_Test_Second[i][5];
//		else
//			dot_num_second=Avg_Test_Second[i+1][5];	
//	}
//	
////	if(dot_num_main==5)
////		 Multiple_Main=100000;
////	else if(dot_num_main==4)
////		 Multiple_Main=10000;
////	else if(dot_num_main==3)
////		 Multiple_Main=1000;
////	else if(dot_num_main==2)
////		 Multiple_Main=100;
////	else if(dot_num_main==1)
////		 Multiple_Main=10;
////	else if(dot_num_main==0)
////		 Multiple_Main=1;
////	
////	if(dot_num_second==5)
////		 Multiple_Second=100000;
////	else if(dot_num_second==4)
////		 Multiple_Second=10000;
////	else if(dot_num_second==3)
////		 Multiple_Second=1000;
////	else if(dot_num_second==2)
////		 Multiple_Second=100;
////	else if(dot_num_second==1)
////		 Multiple_Second=10;
////	else if(dot_num_second==0)
////		 Multiple_Second=1;
//	
//	
//	for(i=0;i<SaveData.Main_Func.Avg;i++)
//	{
//		for(j=0;j<5;j++)
//		{
//			Multiple_Main=Avg_Test_Main[i][j]-'0';
////			Avg_Test_Main[i][j]=-'0';
//			
//		
//		}	
//	}


//}


//void Set_daot(uint8_t *buff,uint8_t dot)
//{
//	uint8_t i;
//	for(i=0;i<dot;i++)
//	{
//		buff[5-i]=buff[5-i-1];		
//	}
//	if(dot==0)
//		buff[5]=' ';
//	else
//	buff[5-dot]='.';
//	for(i=0;i<6;i++)
//	{
//		if(buff[i]>0x80)
//			buff[i]='0';
//	}
//	buff[6]=0;


//}


////ȫ�ֱ���
//u8 WaitRecTimeOver;

//==========================================================
//�������ƣ�PackStandFrame
//�������ܣ������ݴ����֡
//��ڲ�����*framebuf:֡���ݻ���
//			*datbuf:���ݻ���
//			len:���ݳ���
//���ڲ�����һ֡���ݳ���
//�������ڣ�2014.04.11
//�޸����ڣ�2014.04.11 10:28
//��ע˵�����ɵķ���Э��
//��ʼ(0xAB)����ѹ(5) ����(6) ʱ��(4)����ѡ(1)������(0xAF)=18�ֽ�
//==========================================================
int8_t PackStandFrame(int8_t * framebuf, int8_t * datbuf, int8_t len)
{
	if(len>(SEND_LEN_MAX-2))//���ݵ���󳤶�
		len=(SEND_LEN_MAX-2);
	framebuf[0]=UART_SEND_BEGIN;//֡ͷ
	memcpy(&framebuf[1], datbuf, len);//����
	framebuf[len+1]=UART_SEND_END;//֡β
	return (len+1);//����һ֡���ݳ���
}

const u32 UNIT_VALUE[15]=
{
	1,1E3,1E6

};
Sort_TypeDef Input_compvalue(Disp_Coordinates_Typedef *Coordinates)
{
	u8 key;		//,i
	u8 page=0;
	u8 disp_flag=1;
//	u8 index=0;
	u8 disp_cound=0;
	u8 disp_pow=0;
//	u8 input_flag=0;
	u8 dot=5;//С����
	u8 dot_num=0;
	uint8_t While_flag=1;
	uint32_t keynum=0;
	uint8_t Disp_buff[10];
	uint8_t key_count;
	uint8_t dot_num1;
	uint8_t del_flag=0;
	Sort_TypeDef   Sort_set;
	
//	u8 unit=0;//��λ
	u32 Word;
	
	float conp_value=0;
//	float old_value;
	key_count=0;
//	old_value=SaveData.SetParameter.Nominal_value.comp_value;
//	if(SaveData.SysParameter.language==0)
//		Disp_Inputback("������ֵ");//����ɫ
//	else
//		Disp_Inputback("INPUTNUM");//����ɫ
	LCD_DrawRect( Coordinates->xpos, Coordinates->ypos,Coordinates->xpos+Coordinates->lenth , 
	Coordinates->ypos+16 , Red );
	while(While_flag)
	{
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==KEY_NUM)
		{
			disp_flag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1:
					//unit_c=UNIT_VALUE[DispBuf[disp_cound]-'0'];
						conp_value*=pow(10,5-(disp_cound-dot_num));
						//Sort_set.Num=	conp_value *unit_c/pow(10,dot);
						Sort_set.Num=conp_value;
						Sort_set.Dot=dot;
						if(page==0)
							Sort_set.Unit=0;
						else
							Sort_set.Unit=4;
							
						While_flag=0;
					break;
				case Key_F2:
						conp_value*=pow(10,5-(disp_cound-dot_num));
						//Sort_set.Num=	conp_value *unit_c/pow(10,dot);
						Sort_set.Num=conp_value;
						Sort_set.Dot=dot;
						if(page==0)
							Sort_set.Unit=1;
						else
							Sort_set.Unit=5;
						While_flag=0;
					break;
				case Key_F3:
					conp_value*=pow(10,5-(disp_cound-dot_num));
						//Sort_set.Num=	conp_value *unit_c/pow(10,dot);
						Sort_set.Num=conp_value;
						Sort_set.Dot=dot;
						if(page==0)
							Sort_set.Unit=2;
						else
							Sort_set.Unit=6;
						While_flag=0;
					break;
				case Key_F4:
						if(page==0)
						{
						conp_value*=pow(10,5-(disp_cound-dot_num));
						//Sort_set.Num=	conp_value *unit_c/pow(10,dot);
						Sort_set.Num=conp_value;
						Sort_set.Dot=dot;
						if(page==0)
							Sort_set.Unit=3;
//						else
//							Sort_set.Unit=9;	
						While_flag=0;
						}
					break;
				case Key_F5:
					if(page)
						page=0;
					else
						page=1;
				
					break;
				case Key_Disp:
					SetSystemStatus(SYS_STATUS_TEST);
					While_flag=0;
					Sort_set.Updata_flag=0;
				break;
				case Key_SETUP:
					While_flag=0;
					Sort_set.Updata_flag=0;
				SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='1';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
							
					}
				break;
				case Key_NUM2:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='2';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM3:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='3';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM4:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='4';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
					
				break;
				case Key_NUM5:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='5';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM6:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='6';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM7:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='7';
						
						Word=Disp_buff[disp_cound]-'0';
						key_count++;
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM8:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='8';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM9:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='9';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_NUM0:
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='0';
						key_count++;
						Word=Disp_buff[disp_cound]-'0';
						conp_value=conp_value*10+Word;
						disp_pow++;
						disp_cound++;
						if(dot_num==0)
							dot--;
					}
				break;
				case Key_DOT:
					if(dot_num==0)
					{
						dot_num1=key_count;
						Disp_buff[key_count]='.';
						dot_num=1;
						key_count++;
						disp_cound++;
					}
					break;
				case Key_BACK:
					if(key_count>0)
					{	key_count--;
						Disp_buff[key_count]=' ';
						if(dot_num1==key_count)
						{
							dot_num=0;
							dot_num1=0;
							
						}
						del_flag=1;
					
					}
//					else
//					{
//						if(Disp_buff[key_count]==0)
//								Disp_buff[key_count]='-';
//							else if(Disp_buff[key_count]=='-')
//								Disp_buff[key_count]='+';
//							else
//								Disp_buff[key_count]='-';
//							key_count++;
//					}
				break;
				case Key_LOCK:
				break;
				case Key_BIAS:
				break;
				case Key_REST:
				break;
				case Key_TRIG:
				break;
				default:
				break;
					
			}
			if(disp_flag)
			{
				disp_flag=0;
				Disp_button_Num_Input(page);
				Colour.Fword=White;
				Colour.black=Red;
				if(del_flag)
				{
					PutChar( Coordinates->xpos+(key_count+1)*10, Coordinates->ypos, 
					Disp_buff[key_count], Colour.Fword, Colour.black ); 
					del_flag=0;
					
				}
				else if(key_count>0)				
					PutChar( Coordinates->xpos+key_count*10, Coordinates->ypos, 
					Disp_buff[key_count-1], Colour.Fword, Colour.black ); 
			
			}
				
				
			
			
		}
			
	}
		
		
	return Sort_set;
}
	
//���ּ�������ʾ
Sort_TypeDef Disp_NumKeyboard_time(Disp_Coordinates_Typedef *Coordinates )
{
	uint8_t While_flag=1;
	uint8_t Disp_buff[10]={"         ",};
	uint8_t key,i;
	uint8_t dispflag=1;
	uint8_t dot_num=0,dot_num1=0;
	uint8_t page=0;
	uint32_t keynum=0;
	uint8_t key_count=0;
//	uint32_t Num[6]={1,10,100,1e3,1e4,1e5};
	Sort_TypeDef   Sort_set;
	Sort_set.Dot=0;
	Sort_set.Num=0;
	Sort_set.Unit=0;
	Sort_set.Num=0;
//	for(i=0;i<6;i++)
//	Disp_buff[i]=' ';
	Disp_buff[7]=0;
	
	while(While_flag)
	{
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==5)
		{
			dispflag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1:
					
//					if(page==0)
//						Sort_set.Unit=0;
//					else
//						Sort_set.Unit=5;
//					While_flag=0;
//					if(key_count<NUM_LENTH)
//					{
//						if(dot_num==0)
//						{
//							if(key_count>0)
//							{
//								Disp_buff[key_count]='.';
//								dot_num1=key_count;
//								key_count++;
//							
//							
//							}
//							dot_num++;
//						}
//					
//					}
//					
//					Sort_set.Updata_flag=1;
					if(page==0)
						Sort_set.Unit=1;
					else
						Sort_set.Unit=6;
					While_flag=0;
					if(key_count<7)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					
					}
					Sort_set.Updata_flag=1;
					
				break;
				case Key_F2:
					
					
				break;
				case Key_F3:
					
					if(page==0)
						Sort_set.Unit=2;
					else
						Sort_set.Unit=8;
					While_flag=0;
					if(key_count<NUM_LENTH)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							}
							dot_num++;
						}
					
					}
					Sort_set.Updata_flag=1;
				break;
				case Key_F4:
					
					if(page==0)
						Sort_set.Unit=3;
					else
						Sort_set.Unit=7;
					While_flag=0;
					if(key_count<NUM_LENTH)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					
					}
					Sort_set.Updata_flag=1;
				break;
				case Key_F5:
					
					if(page==0)
						page=1;
					else
					page=0;
				break;
				case Key_Disp:
					
					SetSystemStatus(SYS_STATUS_TEST);
					While_flag=0;
					Sort_set.Updata_flag=0;
				break;
				case Key_SETUP:
					
					While_flag=0;
					Sort_set.Updata_flag=0;
				SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
					
				break;
				case Key_LEFT:
					
				break;
				case Key_RIGHT:
					
				break;
				case Key_UP:
					
				break;
				case Key_DOWN:
					
				break;
				case Key_NUM1:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='1';
						
						
							
						
						key_count++;
							
					}
				break;
				case Key_NUM2:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='2';
						key_count++;
					}
				break;
				case Key_NUM3:
					 
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='3';
						key_count++;
					}
				break;
				case Key_NUM4:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='4';
						key_count++;
					}
					
				break;
				case Key_NUM5:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='5';
						key_count++;
					}
				break;
				case Key_NUM6:
					 
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='6';
						key_count++;
					}
				break;
				case Key_NUM7:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='7';
						key_count++;
					}
				break;
				case Key_NUM8:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='8';
						key_count++;
					}
				break;
				case Key_NUM9:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='9';
						key_count++;
					}
				break;
				case Key_NUM0:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='0';
						key_count++;
					}
				break;
				case Key_DOT:
					
					if(key_count<NUM_LENTH&&key_count>0)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					}
//					else 
//					{
//						if(Disp_buff[key_count]==0)
//							Disp_buff[key_count]='-';
//						else if(Disp_buff[key_count]=='-')
//							Disp_buff[key_count]='+';
//						else
//							Disp_buff[key_count]='-';
//						key_count++;
//							
//					
//					
//					
//					
//					}
				break;
				case Key_BACK:
					
					if(key_count>0)
					{	key_count--;
						Disp_buff[key_count]=' ';
						if(dot_num1==key_count)
						{
							dot_num=0;
							dot_num1=0;							
						}
					
					}
					else
					{
						if(Disp_buff[key_count]==0)
								Disp_buff[key_count]='-';
							else if(Disp_buff[key_count]=='-')
								Disp_buff[key_count]='+';
							else
								Disp_buff[key_count]='-';
							key_count++;
					}
				break;
				case Key_LOCK:
					
				break;
				case Key_BIAS:
					
				break;
				case Key_REST:
					
				break;
				case Key_TRIG:
					
				break;
				default:
					
				break;
					
			}
		
		
		}
		if(dispflag)
		{
			dispflag=0;
			LCD_DrawRect( Coordinates->xpos, Coordinates->ypos,Coordinates->xpos+Coordinates->lenth+5 , Coordinates->ypos+16 , Red );
			Colour.Fword=White;
			Colour.black=Red;
			WriteString_16(Coordinates->xpos, Coordinates->ypos, Disp_buff,  0);
			//dispflag=0;
		}
	
	}
	for(i=key_count;i<7;i++)
		Disp_buff[i]='0';
	for(i=0;i<key_count+3;i++)
	{
		if(Disp_buff[0]>='0'&&(Disp_buff[0]<='9'))
		{
			if(Disp_buff[i]>='0'&&(Disp_buff[i]<='9'))
			{
			
				if(dot_num1>i)
				{
					Sort_set.Num*=10;
					Sort_set.Num+=Disp_buff[i]-'0';
				
				}
				else
				{
					Sort_set.Num*=10;
					Sort_set.Num+=Disp_buff[i]-'0';				
				
				}
			}
			
			
			//Sort_set.Num+=(Disp_buff[key_count-1]-'0');
		
		
		}
	
	
	
	}
	Sort_set.Dot=dot_num1;
//	if(Disp_buff[0]>='0'&&(Disp_buff[0]<'9'))
//		{
//			if(Disp_buff[key_count-1]!='.')		
//			{
//				Sort_set.Num*=Num[key_count-dot_num-1];
//				Sort_set.Num+=(Disp_buff[key_count-1]-'0');//*Num[key_count-dot_num-1];
//				
//			}				
//			//*(Disp_buff[key_count-1]-'0'))+=Num[key_count-dot_num-1];
//			else
//			{
//			
//			}
//		
//		
//		}
//			
//		else 
//			;//(Disp_buff[key_count-1]-'0')*Sort_set.Num+=Num[key_count-dot_num-2];
	return Sort_set;

}

//���ּ�������ʾ
Sort_TypeDef Disp_NumKeyboard_Set(Disp_Coordinates_Typedef *Coordinates )
{
	uint8_t While_flag=1;
	uint8_t Disp_buff[10]={"         ",};
	uint8_t key,i;
	uint8_t dispflag=1;
	uint8_t dot_num=0,dot_num1=0;
	uint8_t page=0;
	uint32_t keynum=0;
	uint8_t key_count=0;
//	uint32_t Num[6]={1,10,100,1e3,1e4,1e5};
	Sort_TypeDef   Sort_set;
	Sort_set.Dot=0;
	Sort_set.Num=0;
	Sort_set.Unit=0;
	Sort_set.Num=0;
//	for(i=0;i<6;i++)
//	Disp_buff[i]=' ';
	Disp_buff[7]=0;
	
	while(While_flag)
	{
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==5)
		{
			dispflag=1;
			Key_Beep();
			switch(key)
			{
				case Key_F1:
					
//					if(page==0)
//						Sort_set.Unit=0;
//					else
//						Sort_set.Unit=5;
//					While_flag=0;
//					if(key_count<NUM_LENTH)
//					{
//						if(dot_num==0)
//						{
//							if(key_count>0)
//							{
//								Disp_buff[key_count]='.';
//								dot_num1=key_count;
//								key_count++;
//							
//							
//							}
//							dot_num++;
//						}
//					
//					}
//					
//					Sort_set.Updata_flag=1;
					if(page==0)
						Sort_set.Unit=1;
					else
						Sort_set.Unit=6;
					While_flag=0;
					if(key_count<NUM_LENTH)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					
					}
					Sort_set.Updata_flag=1;
					
				break;
				case Key_F2:
					
					
				break;
				case Key_F3:
					
					if(page==0)
						Sort_set.Unit=2;
					else
						Sort_set.Unit=8;
					While_flag=0;
					if(key_count<NUM_LENTH)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							}
							dot_num++;
						}
					
					}
					Sort_set.Updata_flag=1;
				break;
				case Key_F4:
					
					if(page==0)
						Sort_set.Unit=3;
					else
						Sort_set.Unit=7;
					While_flag=0;
					if(key_count<NUM_LENTH)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					
					}
					Sort_set.Updata_flag=1;
				break;
				case Key_F5:
					
					if(page==0)
						page=1;
					else
					page=0;
				break;
				case Key_Disp:
					
					SetSystemStatus(SYS_STATUS_TEST);
					While_flag=0;
					Sort_set.Updata_flag=0;
				break;
				case Key_SETUP:
					
					While_flag=0;
					Sort_set.Updata_flag=0;
				SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
					
				break;
				case Key_LEFT:
					
				break;
				case Key_RIGHT:
					
				break;
				case Key_UP:
					
				break;
				case Key_DOWN:
					
				break;
				case Key_NUM1:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='1';
						
						
							
						
						key_count++;
							
					}
				break;
				case Key_NUM2:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='2';
						key_count++;
					}
				break;
				case Key_NUM3:
					 
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='3';
						key_count++;
					}
				break;
				case Key_NUM4:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='4';
						key_count++;
					}
					
				break;
				case Key_NUM5:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='5';
						key_count++;
					}
				break;
				case Key_NUM6:
					 
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='6';
						key_count++;
					}
				break;
				case Key_NUM7:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='7';
						key_count++;
					}
				break;
				case Key_NUM8:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='8';
						key_count++;
					}
				break;
				case Key_NUM9:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='9';
						key_count++;
					}
				break;
				case Key_NUM0:
					
					if(key_count<NUM_LENTH)
					{
						Disp_buff[key_count]='0';
						key_count++;
					}
				break;
				case Key_DOT:
					
					if(key_count<NUM_LENTH&&key_count>0)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					}
//					else 
//					{
//						if(Disp_buff[key_count]==0)
//							Disp_buff[key_count]='-';
//						else if(Disp_buff[key_count]=='-')
//							Disp_buff[key_count]='+';
//						else
//							Disp_buff[key_count]='-';
//						key_count++;
//							
//					
//					
//					
//					
//					}
				break;
				case Key_BACK:
					
					if(key_count>0)
					{	key_count--;
						Disp_buff[key_count]=' ';
						if(dot_num1==key_count)
						{
							dot_num=0;
							dot_num1=0;							
						}
					
					}
					else
					{
						if(Disp_buff[key_count]==0)
								Disp_buff[key_count]='-';
							else if(Disp_buff[key_count]=='-')
								Disp_buff[key_count]='+';
							else
								Disp_buff[key_count]='-';
							key_count++;
					}
				break;
				case Key_LOCK:
					
				break;
				case Key_BIAS:
					
				break;
				case Key_REST:
					
				break;
				case Key_TRIG:
					
				break;
				default:
					
				break;
					
			}
		
		
		}
		if(dispflag)
		{
			dispflag=0;
			LCD_DrawRect( Coordinates->xpos, Coordinates->ypos,Coordinates->xpos+Coordinates->lenth+5 , Coordinates->ypos+16 , Red );
			Colour.Fword=White;
			Colour.black=Red;
			WriteString_16(Coordinates->xpos, Coordinates->ypos, Disp_buff,  0);
			//dispflag=0;
		}
	
	}
	for(i=key_count;i<NUM_LENTH;i++)
		Disp_buff[i]='0';
	for(i=0;i<NUM_LENTH;i++)
	{
		if(Disp_buff[0]>='0'&&(Disp_buff[0]<='9'))
		{
			if(Disp_buff[i]>='0'&&(Disp_buff[i]<='9'))
			{
			
				if(dot_num1>i)
				{
					Sort_set.Num*=10;
					Sort_set.Num+=Disp_buff[i]-'0';
				
				}
				else
				{
					Sort_set.Num*=10;
					Sort_set.Num+=Disp_buff[i]-'0';				
				
				}
			}
			
			
			//Sort_set.Num+=(Disp_buff[key_count-1]-'0');
		
		
		}
	
	
	
	}
	Sort_set.Dot=dot_num1;
//	if(Disp_buff[0]>='0'&&(Disp_buff[0]<'9'))
//		{
//			if(Disp_buff[key_count-1]!='.')		
//			{
//				Sort_set.Num*=Num[key_count-dot_num-1];
//				Sort_set.Num+=(Disp_buff[key_count-1]-'0');//*Num[key_count-dot_num-1];
//				
//			}				
//			//*(Disp_buff[key_count-1]-'0'))+=Num[key_count-dot_num-1];
//			else
//			{
//			
//			}
//		
//		
//		}
//			
//		else 
//			;//(Disp_buff[key_count-1]-'0')*Sort_set.Num+=Num[key_count-dot_num-2];
	return Sort_set;

}

Sort_TypeDef Disp_Set_Num(Disp_Coordinates_Typedef *Coordinates)
{
	Sort_TypeDef Sort_num,Sort_num1;
	Disp_button_Num_V();
	Sort_num=Disp_NumKeyboard_Set(Coordinates);
	Sort_num1=Time_Set_Cov(&Sort_num);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
		
	return Sort_num1;

}

Sort_TypeDef Disp_Set_C(Disp_Coordinates_Typedef *Coordinates)
{
	Sort_TypeDef Sort_num,Sort_num1;
	Disp_button_Num_A();
	Sort_num=Disp_NumKeyboard_Set(Coordinates);
	Sort_num1=Time_Set_Cov(&Sort_num);
	if(Sort_num1.Num>MaxCurrent[SaveSIM.Version])
	{
		Sort_num1.Num = MaxCurrent[SaveSIM.Version];
	}
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
		
	return Sort_num1;

}

Sort_TypeDef Disp_Set_T(Disp_Coordinates_Typedef *Coordinates)
{
	Sort_TypeDef Sort_num,Sort_num1;
	Disp_button_Num_ms();
	Sort_num=Disp_NumKeyboard_time(Coordinates);
	Sort_num1=Time_Set_Cot(&Sort_num);
	Sort_num1.Num /= 1000;
//	if(Sort_num1.Num>200)
//	{
//		Sort_num1.Num = 200;
//	}
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
		
	return Sort_num1;

}

Sort_TypeDef Disp_Set_CompNum(Disp_Coordinates_Typedef *Coordinates)
{
	Sort_TypeDef Sort_num,Sort_num1;
	Disp_button_Num_time();
	Sort_num=Disp_NumKeyboard_Set(Coordinates);
	Sort_num1=Time_Set_Cov(&Sort_num);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
		
	return Sort_num1;

}
Sort_TypeDef Disp_Set_InputNum(Disp_Coordinates_Typedef *Coordinates)
{
	Sort_TypeDef Sort_num1;		//Sort_num,
	Disp_button_Num_Input(0);
	Sort_num1=Input_compvalue(Coordinates);
	//if(SaveData.Limit_Tab.Mode==0)
	Sort_num1=Input_Set_Cov(&Sort_num1);
	//else
	//Sort_num1=Input_Set_CovPre(&Sort_num1);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
		
	return Sort_num1;
	
	

}
Sort_TypeDef Disp_Set_InputpreNum(Disp_Coordinates_Typedef *Coordinates)
{
	Sort_TypeDef Sort_num1;
	Disp_button_Num_Input(0);
	Sort_num1=Input_compvalue(Coordinates);//Input_Set_Cov
	if(SaveData.Limit_Tab.Mode==0)
		Sort_num1=Input_Set_Cov(&Sort_num1);
	else
		Sort_num1=Input_Set_CovPre(&Sort_num1);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
		
	return Sort_num1;	

}

Sort_TypeDef Disp_Set_InputpreSecNum(Disp_Coordinates_Typedef *Coordinates)
{
	Sort_TypeDef Sort_num1;
	Disp_button_Num_Input(0);
	Sort_num1=Input_compvalue(Coordinates);//Input_Set_Cov
//	if(SaveData.Limit_Tab.Mode==0)
		Sort_num1=Input_Set_Cov(&Sort_num1);
//	else
//	Sort_num1=Input_Set_CovPre(&Sort_num1);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
		
	return Sort_num1;	

}

uint8_t Freq_Set_Num(Disp_Coordinates_Typedef *Coordinates)//Ƶ������
{
	Sort_TypeDef Sort_num,Sort_num1;
	uint8_t i;
	uint8_t num;
	Disp_button_Num_Freq();
	Sort_num=Disp_NumKeyboard_Set(Coordinates);
	Sort_num1=Freq_Set_Cov(&Sort_num);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
	for(i=4;i<NUM_FREQ+1;i++)
	{
		if(Sort_num1.Num>=FreqNum[i]&&Sort_num1.Num<FreqNum[i+1])
		{
			num=i;
			break;
			
		}
		else
			num=4;
					
	}
		
	return num;
	
}
uint8_t Avg_Set_Num(Disp_Coordinates_Typedef *Coordinates)//ƽ��������
{
	Sort_TypeDef Sort_num,Sort_num1;
	uint8_t i;
	uint8_t num;
	Disp_button_Num_Avg();
	Sort_num=Disp_NumKeyboard_Set(Coordinates);
	Sort_num1=Freq_Set_Cov(&Sort_num);
	if(Sort_num1.Updata_flag==0)
	{
		Sort_num1.Dot=0;
		Sort_num1.Num=0;
		Sort_num1.Unit=0;
	
	}
	if(Sort_num1.Dot==0)
	{
		if(Sort_num1.Num>32)
			Sort_num1.Num=32;
	
	} else
	if(Sort_num1.Dot==1)
	{
		for(i=0;i<5;i++)
		{
			if(Sort_num1.Num>0&&Sort_num1.Num<=9)
				break;
			else
				Sort_num1.Num/=10;
		
		
		}
	
	}else if(Sort_num1.Dot==2)
	{
		for(i=0;i<5;i++)
		{
			if(Sort_num1.Num>10&&Sort_num1.Num<=99)
				break;
			else
				Sort_num1.Num/=10;
		
		
		}
		if(Sort_num1.Num>32)
			Sort_num1.Num=32;
	
	
	
	}else
	{
		Sort_num1.Num=32;
	
	
	}
	num=Sort_num1.Num;
	if(num==0)
		num=1;
		
	return num;
	
}
void Set_daot(uint8_t *buff,uint8_t dot)
{
	uint8_t i;
    if(dot>5)
        dot=0;
	for(i=0;i<dot;i++)
	{
		buff[5-i]=buff[5-i-1];		
	}
	if(dot==0)
		buff[5]=' ';
	else
	buff[5-dot]='.';
	for(i=0;i<6;i++)
	{
		if(buff[i]>0x80)
			buff[i]='0';
	}
	buff[6]=0;


}





uint8_t Test_Comp(All_Compvalue_Typedef *pt)
{
	static float value,secvalue,sechigh;
	static float data,secdata,seclow;
	uint8_t i,j,k=0;
	uint8_t num=9;
	for(i=0;i<5;i++)
	{
		if(pt->all[0].buff[i]==' ')
			data=0;
		else
			data=pt->all[0].buff[i];
		value+=(pow(10,4-i)*data);//����λ��������������
	}
	value=value*pow(1000,pt->all[0].Unit);
	value/=(pow(10,pt->all[0].Dot));
	
	for(i=0;i<5;i++)
	{
		if(pt->all[1].buff[i]==' ')
			secdata=0;
		else
			secdata=pt->all[1].buff[i];
		secvalue+=(pow(10,4-i)*secdata);//����λ��������������
	}
//	if(SaveData.Limit_Tab.Comp_Value[9].high.Unit == 9 && SaveData.Limit_Tab.Comp_Value[9].low.Unit == 9)
//	{
//		secvalue/=(pow(10,pt->all[1].Dot));
//		sechigh = SaveData.Limit_Tab.Comp_Value[9].high.Num/(pow(10,SaveData.Limit_Tab.Comp_Value[9].high.Dot));
//		seclow = SaveData.Limit_Tab.Comp_Value[9].low.Num/(pow(10,SaveData.Limit_Tab.Comp_Value[9].low.Dot));
//	}else{
		secvalue=secvalue*pow(1000,pt->all[1].Unit);
		secvalue/=(pow(10,pt->all[1].Dot));
		sechigh = SaveData.Limit_Tab.Comp_Value[9].high.Num/(pow(10,SaveData.Limit_Tab.Comp_Value[9].high.Dot));
		seclow = SaveData.Limit_Tab.Comp_Value[9].low.Num/(pow(10,SaveData.Limit_Tab.Comp_Value[9].low.Dot));
		sechigh=sechigh*pow(1000,SaveData.Limit_Tab.Comp_Value[9].high.Unit);
		seclow=seclow*pow(1000,SaveData.Limit_Tab.Comp_Value[9].low.Unit);
//	}
	
//	if(SaveData.Limit_Tab.Mode)//�ٷֱȱȽ�
	{
		for(i=0;i<9;i++)
		{
			if(SaveData.Limit_Tab.Comp_Value[i].low.Num!=0)
			{
				if(value>Comp_Testvalue.comp_highvalue[i])//��������
				{
					//Count_buff[10]++;
					j=1;
					primcomp = 1;
					//break;
				}
				else if(value<Comp_Testvalue.comp_lowvalue[i])//С������ �Ӻϸ��־λ
									//����
				{
					//Count_buff[10]++;
					//break;
					j=1;
					primcomp = 1;
				}
				else 
				{
					Count_buff[i]++;
					num=i;
					i=10;
					
					j=0;
					primcomp = 0;
				}
			
			
			}
		
		
		}
//		if(SaveData.Limit_Tab.Comp_Value[9].low.Num!=0)
//		{
			if(secvalue>sechigh)//��������
			{
				//Count_buff[10]++;
				j=1;
				scedcomp = 1;
				num = 9;
				//break;
			}
			else if(secvalue<seclow)//С������ �Ӻϸ��־λ
								//����
			{
				//Count_buff[10]++;
				//break;
				j=1;
				scedcomp = 1;
				num = 9;
			}
			else 
			{
//				Count_buff[9]++;
//				num=i;
//				i=10;
				scedcomp = 0;
				j=0;
			}
		
		
//		}
		if(primcomp==1 || scedcomp == 1)
		{
			Count_buff[10]++;
			Comp_flag=1;
			
		}
		else
			Comp_flag=0;
	
	
	
	}
	value = 0;
	secvalue = 0;
//	else
//	{
//		
//	
//	
//	}//ABS�Ƚ�
return num;
}//Comp_Testvalue  Comp_Testvalue
float Inttofloat(Sort_TypeDef *pt)//intת��Ϊfloat  INT����С����͵�λ
{
	float value;
//	uint8_t i;
	value=pt->Num;
	value=value*pow(1000,pt->Unit);
	value/=pow(10,pt->Dot);
	return value;
}
void Set_Compbcd_float(void)//�����ñȽ�����ת��Ϊfloat����  �������������ֵ�������㣬�õ�
	//����ֱ�ӱȽϵ�ֵ  �����Ϊ2�֣�һ����ABSһ����%
{
	uint8_t i;
	float value;
	for(i=0;i<9;i++)
	{
		Comp_Testvalue.comp_highvalue[i]=Inttofloat( &SaveData.Limit_Tab.Comp_Value[i].high);
		Comp_Testvalue.comp_lowvalue[i]=Inttofloat( &SaveData.Limit_Tab.Comp_Value[i].low);
	
	
	}
	Comp_Testvalue.comp_highvalue[9]=Inttofloat( &SaveData.Limit_Tab.Comp_Value_2nd.high);
	Comp_Testvalue.comp_lowvalue[9]=Inttofloat( &SaveData.Limit_Tab.Comp_Value_2nd.low);
	value=Inttofloat(&SaveData.Limit_Tab.Nom);
	if(SaveData.Limit_Tab.Mode)//�ٷֱȱȽ�
	{
		for(i=0;i<9;i++)
		{
			Comp_Testvalue.comp_highvalue[i]=(100000+Comp_Testvalue.comp_highvalue[i] * 1000)*value;
			Comp_Testvalue.comp_highvalue[i]/=100000;
			Comp_Testvalue.comp_lowvalue[i]=(100000-Comp_Testvalue.comp_lowvalue[i] * 1000)*value;
			Comp_Testvalue.comp_lowvalue[i]/=100000;
			
		}
	
	
	
	}
	else
	{
		
		for(i=0;i<9;i++)
		{
			Comp_Testvalue.comp_highvalue[i]+=value;
			Comp_Testvalue.comp_lowvalue[i]=fabs(value-Comp_Testvalue.comp_lowvalue[i]);
			
		}
		//Comp_Testvalue.comp_highvalue[9]=
		
	
	
	}//ABS�Ƚ�
	



}
void  Write_Usbdata (volatile uint8_t  *buffer,uint32_t num)
{
    int32_t  fdw;
//    int32_t  fdr;
//    uint32_t  tot_bytes_written;
    uint32_t  bytes_written;



        fdw = FILE_Open(FILENAME_W, RDWR);
        if (fdw > 0) 
		{
			usb_oenflag=1;

			bytes_written = FILE_Write(fdw, buffer, num);//MAX_BUFFER_SIZE);

			FILE_Close(fdw);
            
        } 
		else
			usb_oenflag=0;

    } 

//����ػ����򣬷��ڹ���״̬����
void Soft_Turnon(void)
{
	uint32_t i;
	Set_Compbcd_float();
//	ReadSavedata();
//	SetData_High_Low_Comp();
//	Savetoeeprom();
	
	Power_On_led();

	All_LedOff();
	Beep_Off();
    for(i=0xffffff;i>0;i--)
		;
	GPIO_ClearInt(0, 1<<19);	
	while(GetSystemStatus()==SYS_STATUS_TOOL)
	{
		NVIC_EnableIRQ(GPIO_IRQn);
		
//		if(Save_Res.softswitch)
//		{
//			SetSystemStatus(SYS_STATUS_POWER);
//		
//		
//		}
	
	
	}
    NVIC_DisableIRQ(GPIO_IRQn);
	Power_Off_led();
    GPIO_ClearInt(0, 1<<19);
		

}
void Test_Comp_Fmq(void)//Comp_flag==1���ϸ�
{
	if(Comp_flag==0)
		Pass_Led();
	else
		Fail_led();
	if(SaveData.Main_Func.beep==1)//�ϸ�Ѷ��
	{
		if(Comp_flag==0)//�ϸ�
			Beep_on();
		else
			Beep_Off();
	
	}
	else if(SaveData.Main_Func.beep==2)
	{
		if(Comp_flag==1)//���ϸ�
			Beep_on();
		else
			Beep_Off();
	
	}
	else
		Beep_Off();
		
	


}

uint8_t Uart3_Process(void)
{
//	u8 i;

	u8 kind=0xff;
	u8 data=0;
	u8 str[(FRAME_LEN_MAX-FRAME_LEN_MIN)+1];//�շ����ݻ���
//	if(Save_Res.Sys_Setvalue.uart==1)//������Ч
	{
		if (ComBuf3.rec.end)//�������ݽ���
		{
            data=1;
			memset(str,'\0',(FRAME_LEN_MAX-FRAME_LEN_MIN+1));//��ջ���
			{
				//memcpy(str,&ComBuf3.rec.buf[PDATASTART],ComBuf3.send.len-FRAME_LEN_MIN);//���ݰ�
				kind=ComBuf3.rec.buf[PFRAMEKIND];//������

			}
			//׼��������һ֡����sprintf
			ComBuf3.rec.end=0;//���ջ���ɶ���־��λ
			ComBuf3.rec.ptr=0;//����ָ������
		}
	}
//	WriteString_Big(0, 150, (uint8_t *)&ComBuf.rec.buf[1]);
	ComBuf3.rec.end=0;

	switch(kind)
	{
		case FRAME_READ_RESULT://��ȡ���
			//���ڷ��Ͳ�������:��ѹ(5)+����(6)+ʱ��(4)+��ѡ(1)=16�ֽ�
	
			ComBuf3.send.buf[1+5+6+4]=kind;
			ComBuf3.send.begin=0;
			ComBuf3.send.len=PackStandFrame(ComBuf3.send.buf , &ComBuf3.send.buf[1] , 16  );
//			if(SendDataToCom()==0)//���ͳɹ��б�
//			{
//			//	Delay_1ms(100);//��ʱ
//			//	SendDataToCom();//����
//			}
			break;
		
		case FRAME_START://����
			SetSystemStatus(SYS_STATUS_TEST);//ϵͳ״̬-��������
			break;

		case FRAME_RESET://��λ
			//SetSystemStatus(SYS_STATUS_IDLE);//ϵͳ״̬-����
			break;
        case 0x10://���ò���
            
            break;
        case 0x11://����Ƶ��
            break;
        case 0x12://���õ�ƽ
            break;
        case 0x13://������ʾ  ��
            break;
        case 0x14://��������
            break;
        case 0x15://��������
            break;
        case 0x16://����Ѷ��
            break;
        case 0x17://��������
            break;
        case 0x18://���÷�ѡ
            break;
        case 0x19://�����ٶ�
            break;
        case 0x1a://���ô�����ʽ
            break;
        case 0x1b://����DQ������
            break;
        case 0x2c://���õ�1
            break;
        case 0x2d://���õ�2
            break;
        case 0x2e://���õ�3
            break;
        case 0x2f://���õ�4
            break;
        case 0x30://���õ�5
            break;
        case 0x31://���õ�6
            break;
        case 0x32://���õ�7
            break;
        case 0x33://���õ�8
            break;
        case 0x34://���õ�9
            break;
        case 0x20://���ñ��ֵ
            break;
        
        
        //��ȡ����ֵ
        case 0x51://��ȡ����
            break;
        case 0x52://��ȡƵ��
            break;
        case 0x53://��ȡ��ƽ
            break;
        case 0x54://��ȡ��ʾ
            break;
        case 0x55://��ȡ����
            break;
        case 0x56://��ȡ����
            break;
        case 0x57://��ȡѶ��
            break;
        case 0x58://��ȡ����
            break;
        case 0x59://��ȡ��ѡ
            break;
        case 0x5a://��ȡ�ٶ�
            break;
        case 0x5b://��ȡ������ʽ
            break;
        case 0x5c://��ȡD/Qֵ
            break;
        case 0x5d://��ȡ��1ֵ
            break;
        case 0x5e://��ȡ��2ֵ
            break;
        case 0x5f://��ȡ��3ֵ
            break;
        case 0x61://��ȡ���ֵ
            break;
        case 0x62://��ȡ��4ֵ
            break;
        case 0x63://��ȡ��5ֵ
            break;
        case 0x64://��ȡ��6ֵ
            break;
        case 0x65://��ȡ��7ֵ
            break;
        case 0x66://��ȡ��8ֵ
            break;
        case 0x67://��ȡ��9ֵ
            break;
        
        
//        case 0x2d://���õ�2
//            break;
        
		case FRAME_WRITE_SN://д���к�
			break;
		
		case FRAME_CLR_BOOT_NUM://�忪������
			break;
		
		case FRAME_DATA://����֡
			break;

		default:
			break;
	}
	return data;

}


void input_password(Disp_Coordinates_Typedef *Coordinates )
{
  	char While_flag=1;
	char Disp_buff[10];
	u8 key,i;
	char dispflag=1;
	char dot_num=0,dot_num1=0;
    char password_flag=0;
//	vu8 page=0;
	uint32_t keynum=0;
	char  key_count=0;
//	vu32 Num[6]={1,10,100,1e3,1e4,1e5};
//	Sort_TypeDef   Sort_set;
//	Sort_set.Dot=0;
//	Sort_set.Num=0;
//	Sort_set.Unit=0;
//	Sort_set.Num=0;
	for(i=0;i<9;i++)
	Disp_buff[i]=0;
	//Disp_buff[8]=0;
	Disp_Button_value1(2);
	while(While_flag)
	{
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==KEY_NUM)
		{
            password_flag=0;
            Key_Beep();
			dispflag=1;
			switch(key)
			{
				case Key_F1://�˳�
					 While_flag=0;
                       SetSystemStatus(SYS_STATUS_TEST);

					
				break;
				case Key_F3://ȡ��
                    
					
				break;
				case Key_F2://ȷ��
                   if(strcmp(PASSWORD,Disp_buff))//�ȽϺ���  �����ʱ  ����0
                   {//�������
                       key_count=0;
                       for(i=0;i<8;i++)
                        Disp_buff[i]=' ';
                       password_flag=1;
                       
                   
                   
                   }
                   else //������ȷ
                   {
                        While_flag=0;
                       SetSystemStatus(SYS_STATUS_FACRDEBUG);
                   
                   }
                       
					
				break;
				case Key_F4:
					
				break;
				case Key_F5:
					
				break;
				case Key_Disp:
//					SetSystemStatus(SYS_STATUS_TEST);
//					While_flag=0;
//					Sort_set.Updata_flag=0;
				break;
				case Key_SETUP:
//					While_flag=0;
//					Sort_set.Updata_flag=0;
//				SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_FAST:
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
				case Key_UP:
				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='1';
						
						
							
						
						key_count++;
							
					}
				break;
				case Key_NUM2:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='2';
						key_count++;
					}
				break;
				case Key_NUM3:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='3';
						key_count++;
					}
				break;
				case Key_NUM4:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='4';
						key_count++;
					}
					
				break;
				case Key_NUM5:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='5';
						key_count++;
					}
				break;
				case Key_NUM6:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='6';
						key_count++;
					}
				break;
				case Key_NUM7:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='7';
						key_count++;
					}
				break;
				case Key_NUM8:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='8';
						key_count++;
					}
				break;
				case Key_NUM9:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='9';
						key_count++;
					}
				break;
				case Key_NUM0:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='0';
						key_count++;
					}
				break;
				case Key_DOT:
					
					if(key_count<PASSWORD_LENTH&&key_count>0)
					{
						if(dot_num==0)
						{
							if(key_count>0)
							{
								Disp_buff[key_count]='.';
								dot_num1=key_count;
								key_count++;
							
							
							}
							dot_num++;
						}
					}
//					else 
//					{
//						if(Disp_buff[key_count]==0)
//							Disp_buff[key_count]='-';
//						else if(Disp_buff[key_count]=='-')
//							Disp_buff[key_count]='+';
//						else
//							Disp_buff[key_count]='-';
//						key_count++;
//							
//					
//					
//					
//					
//					}
				break;
				case Key_BACK:
					if(key_count>0)
					{	key_count--;
						Disp_buff[key_count]=' ';
						if(dot_num1==key_count)
						{
							dot_num=0;
							dot_num1=0;
							
						}
					
					}
					else
					{
						if(Disp_buff[key_count]==0)
								Disp_buff[key_count]='-';
							else if(Disp_buff[key_count]=='-')
								Disp_buff[key_count]='+';
							else
								Disp_buff[key_count]='-';
							key_count++;
					}
				break;
				case Key_LOCK:
				break;
				case Key_BIAS:
				break;
				case Key_REST:
				break;
				case Key_TRIG:
				break;
				default:
				break;
					
			}
		
		
		}
		if(dispflag)
		{
			dispflag=0;
            LCD_DrawLine( Coordinates->xpos+13, Coordinates->ypos-2, Coordinates->xpos+13+80, Coordinates->ypos-2 , Red );
            LCD_DrawLine( Coordinates->xpos+13, Coordinates->ypos+18, Coordinates->xpos+13+80, Coordinates->ypos+18 , Red );
            for(i=0;i<9;i++)
            LCD_DrawLine( Coordinates->xpos+13+i*10, Coordinates->ypos-2, Coordinates->xpos+13+i*10, Coordinates->ypos+18 , Red );
            
			//LCD_DrawRect( Coordinates->xpos, Coordinates->ypos,Coordinates->xpos+Coordinates->lenth , Coordinates->ypos+16 , Red );
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BACK;
          //  WriteString_16(Coordinates->xpos-38, Coordinates->ypos, "����:",  0);
			WriteString_16(Coordinates->xpos-38, Coordinates->ypos, " PWD:",  0);
			WriteString_16(Coordinates->xpos+15, Coordinates->ypos, ( uint8_t *)Disp_buff,  0);
            if(password_flag)
                WriteString_16(Coordinates->xpos, Coordinates->ypos+20, "PwdERR",  0);
            else
               LCD_DrawRect( Coordinates->xpos, Coordinates->ypos+20,Coordinates->xpos+100 , Coordinates->ypos+40, LCD_COLOR_TEST_BACK );
               // WriteString_16(Coordinates->xpos, Coordinates->ypos, "         ",  0);
           // WriteString_16(Coordinates->xpos, Coordinates->ypos-20, "������8λ����",  0);
						WriteString_16(Coordinates->xpos, Coordinates->ypos-20, "8bit Password",  0);
			//dispflag=0;
		}
	
	}

}
void input_num(Disp_Coordinates_Typedef *Coordinates )
{
  	char While_flag=1;
	uint8_t Disp_buff[12];
	uint8_t  key,i;
	char dispflag=1;
//	char dot_num=0,dot_num1=0;
    char password_flag=0;
	char keynum=0;
	char key_count=0;
//	Sort_TypeDef   Sort_set;
	
	for(i=0;i<8;i++)
	Disp_buff[i]=' ';
	Disp_buff[8]=0;
	
	while(While_flag)
	{
		key=HW_KeyScsn();
		if(key==0xff)
		{
			keynum=0;
		}
		else
			keynum++;
		if(keynum==KEY_NUM)
		{
            password_flag=0;
            Key_Beep();
			dispflag=1;
			switch(key)
			{
				case Key_F1://�˳�
					 
                       //SetSystemStatus(SYS_STATUS_USERDEBUG);

					
				break;
				case Key_F2://ȡ��
                    
					
				break;
				case Key_F3://ȷ�ϱ�����ź�����
//                   if(strcmp(PASSWORD,Disp_buff))//�ȽϺ���  �����ʱ  ����0
//                   {//�������
//                       key_count=0;
//                       for(i=0;i<8;i++)
//                        Disp_buff[i]=' ';
//                       password_flag=1;
//                       
//                   
//                   
//                   }
//                   else //������ȷ
//                   {
//                        While_flag=0;
//                       SetSystemStatus(SYS_STATUS_USERDEBUG);
//                   
//                   }//���Ƶ��ڴ�
                       
					
				break;
				case Key_F4:
					
				break;
				case Key_F5:
                   //return  &Disp_buff[0];
					
				break;
				case Key_Disp:
//					SetSystemStatus(SYS_STATUS_TEST);
//					While_flag=0;
//					Sort_set.Updata_flag=0;
				break;
				case Key_SETUP:
//					While_flag=0;
//					Sort_set.Updata_flag=0;
//				SetSystemStatus(SYS_STATUS_SETUPTEST);
				break;
				case Key_UP:
                    While_flag=0;//����
                    dispflag=0;
                    for(i=0;i<8;i++)
                    {
                        SaveSIM.fac_num[i]=Disp_buff[i];
                    
                    }
                    Savetoeeprom();
                     LCD_DrawRect( 0, Coordinates->ypos-20,Coordinates->xpos+200 , Coordinates->ypos+20, LCD_COLOR_TEST_BACK );
				break;
				case Key_LEFT:
				break;
				case Key_RIGHT:
				break;
//				case Key_UP:
//				break;
				case Key_DOWN:
				break;
				case Key_NUM1:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='1';
						key_count++;
							
					}
				break;
				case Key_NUM2:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='2';
						key_count++;
					}
				break;
				case Key_NUM3:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='3';
						key_count++;
					}
				break;
				case Key_NUM4:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='4';
						key_count++;
					}
					
				break;
				case Key_NUM5:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='5';
						key_count++;
					}
				break;
				case Key_NUM6:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='6';
						key_count++;
					}
				break;
				case Key_NUM7:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='7';
						key_count++;
					}
				break;
				case Key_NUM8:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='8';
						key_count++;
					}
				break;
				case Key_NUM9:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='9';
						key_count++;
					}
				break;
				case Key_NUM0:
					if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='0';
						key_count++;
					}
				break;
				case Key_DOT:
                    if(key_count<PASSWORD_LENTH)
					{
						Disp_buff[key_count]='/';
						key_count++;
					}
					
//					if(key_count<NUM_LENTH&&key_count>0)
//					{
//						if(dot_num==0)
//						{
//							if(key_count>0)
//							{
//								Disp_buff[key_count]='.';
//								dot_num1=key_count;
//								key_count++;
//							
//							
//							}
//							dot_num++;
//						}
//					}
//					else 
//					{
//						if(Disp_buff[key_count]==0)
//							Disp_buff[key_count]='-';
//						else if(Disp_buff[key_count]=='-')
//							Disp_buff[key_count]='+';
//						else
//							Disp_buff[key_count]='-';
//						key_count++;
//							
//					
//					
//					
//					
//					}
				break;
				case Key_BACK:
					if(key_count)
					{	key_count--;
						Disp_buff[key_count]=' ';
//						if(dot_num1==key_count)
//						{
//							dot_num=0;
//							dot_num1=0;
//							
//						}
					
					}
//					else
//					{
//						if(Disp_buff[key_count]==0)
//								Disp_buff[key_count]='-';
//							else if(Disp_buff[key_count]=='-')
//								Disp_buff[key_count]='+';
//							else
//								Disp_buff[key_count]='-';
//							key_count++;
//					}
				break;
				case Key_LOCK:
				break;
				case Key_BIAS:
				break;
				case Key_REST:
				break;
				case Key_TRIG:
				break;
				default:
				break;
					
			}
		
		
		}
		if(dispflag)
		{
			dispflag=0;
            LCD_DrawLine( Coordinates->xpos+13, Coordinates->ypos-2, Coordinates->xpos+13+80, Coordinates->ypos-2 , Red );
            LCD_DrawLine( Coordinates->xpos+13, Coordinates->ypos+18, Coordinates->xpos+13+80, Coordinates->ypos+18 , Red );
            for(i=0;i<9;i++)
            LCD_DrawLine( Coordinates->xpos+13+i*10, Coordinates->ypos-2, Coordinates->xpos+13+i*10, Coordinates->ypos+18 , Red );
            
			//LCD_DrawRect( Coordinates->xpos, Coordinates->ypos,Coordinates->xpos+Coordinates->lenth , Coordinates->ypos+16 , Red );
			Colour.Fword=White;
			Colour.black=LCD_COLOR_TEST_BACK;
            WriteString_16(Coordinates->xpos-70, Coordinates->ypos, "Fact No.:",  0);
			WriteString_16(Coordinates->xpos+15, Coordinates->ypos, ( uint8_t *)Disp_buff,  0);
//            if(password_flag)
//                WriteString_16(Coordinates->xpos, Coordinates->ypos+20, "�������",  0);
//            else
//               LCD_DrawRect( Coordinates->xpos, Coordinates->ypos+20,Coordinates->xpos+100 , Coordinates->ypos+40, LCD_COLOR_TEST_BACK );
               // WriteString_16(Coordinates->xpos, Coordinates->ypos, "         ",  0);
          //  WriteString_16(Coordinates->xpos, Coordinates->ypos-20, "������8λ�������",  0);
			WriteString_16(Coordinates->xpos, Coordinates->ypos-20, "Enter 8 bits No. ",  0);
		}
	
	}

}



