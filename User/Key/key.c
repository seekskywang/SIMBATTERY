#include "Key/key.h"
#include "lpc177x_8x_gpio.h"
#include "Globalvalue/GlobalValue.h"
//volatile unsigned long SysTickCnt;
//void SysTick_Handler (void)
//{
//	SysTickCnt++;
//}
void Start_Debugled(char flag)
{
    if(flag )
    {
        GPIO_SetValue(1, (1<<2));
        GPIO_SetValue(5, (1<<4));
    
    
    }
    else
    {
        GPIO_ClearValue(1, (1<<2));
        GPIO_ClearValue(5, (1<<4));
    
    }


}
void Beep_on(void)
{
	GPIO_SetValue(0, (1<<12));

}
void Beep_Off(void)
{
	GPIO_ClearValue(0, (1<<12));

}
void Bais_LedOn(void)
{
	GPIO_SetValue(1, (1<<14));

}

void Bais_LedOff(void)
{
	GPIO_ClearValue(1, (1<<14));
}
void Lock_LedOn(void)
{
	GPIO_SetValue(1, (1<<0));

}
void Lock_LedOff(void)
{
	GPIO_ClearValue(1, (1<<0));
}
void Lock_Control_On(void)
{
	GPIO_ClearValue(1, (1<<9));
	

}
void Lock_Control_Off(void)
{
	GPIO_SetValue(1, (1<<9));

}
void Pass_Led(void)
{
	GPIO_SetValue(1, (1<<2));
	GPIO_ClearValue(5, (1<<4));
    Plc_pass();

}
void All_LedOff1(void)
{
	GPIO_ClearValue(1, (1<<2));
	GPIO_ClearValue(5, (1<<4));
//	GPIO_ClearValue(1, (1<<9));
//	Turnoff_backlight();
	//GPIO_ClearValue(1, (1<<9));


}
void All_LedOff(void)
{
	GPIO_ClearValue(1, (1<<2));
	GPIO_ClearValue(5, (1<<4));
	GPIO_ClearValue(1, (1<<9));
	Turnoff_backlight();
	//GPIO_ClearValue(1, (1<<9));


}
void Fail_led(void)
{
	
	
	GPIO_SetValue(5, (1<<4));
	GPIO_ClearValue(1, (1<<2));
    Plc_Fail();
   

}
void Power_On_led(void)
{
	GPIO_SetValue(0, (1<<10));
	GPIO_ClearValue(2, (1<<26));
    Power_Relay(0);

}
void Power_Off_led(void)
{
	GPIO_SetValue(2, (1<<26));
	GPIO_ClearValue(0, (1<<10));
    Power_Relay(1);

}
void Turnon_backlight(void)
{
	GPIO_SetValue(2, (1<<1));

}
void Turnoff_backlight(void)
{
	
    
    
    GPIO_ClearValue(2, (1<<1));


}
void Power_Relay(unsigned char flag)//��λ����Դ�̵�������  1 ����  0 �ػ�
{
	if(flag)
		GPIO_SetValue(0, (1<<20));
	else
		GPIO_ClearValue(0, (1<<20));
    Delay(20);


}
void Plc_pass(void)//�ϸ�̵���
{
    GPIO_SetValue(0, (1<<22));
    GPIO_ClearValue(2, (1<<11));
}
void Plc_Fail(void)//���ϸ�̵���
{
    GPIO_SetValue(2, (1<<11));
    GPIO_ClearValue(0, (1<<22));
}

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief		Delay function
 * @param[in]	tick - number milisecond of delay time
 * @return 		None
 **********************************************************************/
//void Delay (unsigned long tick)
//{
//	unsigned long systickcnt;

//	systickcnt = SysTickCnt;
//	while ((SysTickCnt - systickcnt) < tick);
//}
void GPIO_Led_Configuration(void)
{
	GPIO_SetDir(0, (1<<10), GPIO_DIRECTION_OUTPUT);//������
	GPIO_SetDir(2, (1<<26), GPIO_DIRECTION_OUTPUT);//�ػ���
	GPIO_SetDir(5, (1<<4), GPIO_DIRECTION_OUTPUT);//�ϸ��
	GPIO_SetDir(1, (1<<2), GPIO_DIRECTION_OUTPUT);//���ϸ��
	GPIO_SetDir(1, (1<<0), GPIO_DIRECTION_OUTPUT);//LOCK��
	GPIO_SetDir(1, (1<<14), GPIO_DIRECTION_OUTPUT);//BAIS��
	GPIO_SetDir(0, (1<<12), GPIO_DIRECTION_OUTPUT);//������
	GPIO_SetDir(2, (1<<1), GPIO_DIRECTION_OUTPUT);//����


}
void GPIO_Plc_Configuration(void)
{
	GPIO_SetDir(0, (1<<20), GPIO_DIRECTION_OUTPUT);//�����̵���
    GPIO_SetDir(0, (1<<22), GPIO_DIRECTION_OUTPUT);//�ϸ�̵���
	GPIO_SetDir(2, (1<<10), GPIO_DIRECTION_OUTPUT);//
	GPIO_SetDir(2, (1<<11), GPIO_DIRECTION_OUTPUT);//���ϸ�̵���
	GPIO_SetDir(0, (1<<11), GPIO_DIRECTION_INPUT);//����

}
void GPIO_Key_Configuration(void)
{
	GPIO_SetDir(1, (1<<9), GPIO_DIRECTION_OUTPUT);//74LS164����
	GPIO_SetDir(1, (1<<10), GPIO_DIRECTION_OUTPUT);//74LS164�����ݶ�
	GPIO_SetDir(1, (1<<1), GPIO_DIRECTION_OUTPUT);//74LS164��cp
	
	GPIO_SetDir(0, (1<<23), GPIO_DIRECTION_INPUT);//4���������˿�
	GPIO_SetDir(0, (1<<24), GPIO_DIRECTION_INPUT);
	GPIO_SetDir(0, (1<<25), GPIO_DIRECTION_INPUT);
	GPIO_SetDir(0, (1<<26), GPIO_DIRECTION_INPUT);
	
	
//	GPIO_SetDir(0, (1<<27), GPIO_DIRECTION_INPUT);//F��ݼ�
//	GPIO_SetDir(0, (1<<14), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(5, (1<<2),  GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(4, (1<<26), GPIO_DIRECTION_INPUT);
	GPIO_SetDir(0, (1<<19), GPIO_DIRECTION_INPUT);//���ؼ�
	GPIO_SetDir(1, (1<<15), GPIO_DIRECTION_INPUT);//LOCK
///*key1(P4_26)  key2(P2_22)  key3(P0_10)  */	
	GPIO_SetValue(1, (1<<9));//HC164���ƽ�
//	GPIO_SetDir(5, (1<<4), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(2, (1<<22), GPIO_DIRECTION_INPUT);
//	GPIO_SetDir(0, (1<<10), GPIO_DIRECTION_INPUT);	
}void _164Delaay(void)
{
	u8 i;
	for(i=0;i<10;i--)
	;

};
void HW_Sendvalueto164(u8 value)
{
	uint8_t i;
	_Hc164CpL;
	for(i=0;i<8;i++)
	{
		if((value&0x80)==0x80)
			_Hc164DH;
		else
			_Hc164DL;
		_Hc164CpH;
		_164Delaay();
		_Hc164CpL;
		value<<=1;
	}

}
void HW_keyInt(void)
{
	GPIO_Key_Configuration();
	HW_Sendvalueto164(0xff);

}
const u8 Scan_Value[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};
u8 HW_KeyScsn(void)
{
	static u8 keynum,keynum1;
	keynum=HW_KeyScsn1();
	if(keynum!=0xff)
	{
		Delay(10);
		keynum1=HW_KeyScsn1();
		
		if(keynum1==keynum)
		{
			//Key_Beep();
			return keynum;
			
		}
		else
			return 0xff;
	
	
	}
	return 0xff;

}
u8 HW_KeyScsn1(void)
{
	
	u8 row,i;
//	u8 scanvalue=0;
	u8 key_value=0xff;
	u8 value=0xff;
	HW_Sendvalueto164(0);
	if(!(GPIO_ReadValue(0) & (1<<23)))
		row=0;
	else if(!(GPIO_ReadValue(0) & (1<<24)))
		row=1;
	else if(!(GPIO_ReadValue(0) & (1<<25)))
		row=2;
	else if(!(GPIO_ReadValue(0) & (1<<26)))
		row=3;
	else row=4;
	if(row<4)
	{
		
		for(i=0;i<8;i++)
		{
			HW_Sendvalueto164(Scan_Value[i]);
			switch(row)
			{
				case 0:
						
						if(!(GPIO_ReadValue(0) & (1<<23)))
						{
							
							value=i;
						}
						
					break;
				case 1:
					if(!(GPIO_ReadValue(0) & (1<<24)))
						value=i;
					break;
				case 2:
					if(!(GPIO_ReadValue(0) & (1<<25)))
						value=i;
					break;
				case 3:
					if(!(GPIO_ReadValue(0) & (1<<26)))
						value=i;
					break;
				default:
					break;
			
			
			}
		if(value!=0xff)
			break;
		}
		key_value=row*8+value;
	
	
	
	}
	else
	{	
		//GPIO_SetValue(0, (1<<19));
		GPIO_SetValue(1, (1<<15));
		//GPIO_SetValue(4, (1<<26));
		if(!(GPIO_ReadValue(1) & (1<<15)))
			key_value=15;
		else 
			if(!(GPIO_ReadValue(0) & (1<<11)))
				key_value=7;
//			else 
//				if(!(GPIO_ReadValue(5) & (1<<2)))
//				key_value=0x35;
//			else
//				if(!(GPIO_ReadValue(4) & (1<<26)))
//				key_value=0x36;
//			else if(!(GPIO_ReadValue(0) & (1<<19)))
//				key_value=0x37;
			else
				key_value=0xff;
	
	
	
	}
	return key_value;

}
void Delay_Key(void)
{
	u8 key;
	do{
		key=HW_KeyScsn();
	}while(key!=0xff);


}
void Key_Beep(void)
{
    if(Saveeeprom.Sys_set.key_board)
	Beep_on();
	Delay(20);
	Beep_Off();

}




