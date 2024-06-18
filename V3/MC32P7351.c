/******************************************************************************
;  *       @型号                 : MC32P7351
;  *       @创建日期             : 2022.12.16
;  *       @公司/作者            : sinomcu.com-FAE
;  *       @晟矽微技术支持       : 2048615934
;  *       @晟矽微官网           : http://www.sinomcu.com/
;  *       @版权                 : 2022 sinomcu.com公司版权所有.
;  *----------------------摘要描述---------------------------------
;  *                定时器
;  *                T0   P10 2mS翻转
;  *                T1   P11 1mS翻转
;  *                T2   P12 100uS翻转
;  *                T3   P14 50uS翻转    
		  内部4V
******************************************************************************/

#include "user.h"

/************************************************
;  *    @函数名          : CLR_RAM
;  *    @说明            : 清RAM
;  *    @输入参数        :
;  *    @返回参数        :
;  ***********************************************/
void CLR_RAM(void)
{
    for (FSR0 = 0; FSR0 < 0xff; FSR0++)
    {
        INDF0 = 0x00;
    }
    FSR0 = 0xFF;
    INDF0 = 0x00;
}
/************************************************
;  *    @函数名            : IO_Init
;  *    @说明              : IO初始化
;  *    @输入参数          :
;  *    @返回参数          :
;  ***********************************************/
void IO_Init(void)
{
    IOP0   = 0B00000000;                 //io口数据位
    OEP0   = 0B11100000;                 //io口方向 1:out  0:in
    PUP0   = 0x00;                       //io口上拉电阻   1:enable  0:disable
    PDP0   = 0x00;                       //io口下拉电阻   1:enable  0:disable
    ADIOS0 = 0B00010000;                 //io类型选择  1:模拟输入  0:通用io

    IOP1   = 0B00000000;                 //io口数据位
    OEP1   = 0B11010001;                 //io口方向 1:out  0:in
    PUP1   = 0B00001100;                 //io口上拉电阻   1:enable  0:disable
    PDP1   = 0B00000000;                 //io口下拉电阻   1:enable  0:disable
    ADIOS1 = 0B00000000;                 //io类型选择  1:模拟输入  0:通用io

    PMOD   = 0B00000011;                 //bit7-bit5 P17、P13、P01 io类型选择 1:模拟输入  0:通用io 
                                         //bit2-bit0 P13、P01、P00 io端口输出模式  1:推挽输出 0:开漏输出
    DRVCR  = 0x80;                       //普通驱动
}


/************************************************
;  *    @函数名            : TIMER0_INT_Init
;  *    @说明              : 定时器0初始化
;  *    @输入参数          :
;  *    @返回参数          :
;  ***********************************************/
void TIMER0_INT_Init(void)
{
    T0CR   = 0B00001000;			//使能 PWM0		FTMR
    T0CNT  = 199;
    T0LOAD = 199;				
    
	BUFF_MOTO= 0;
	SY_IO=0;
    T0EN   = 0;
	PWM0EC = 0;
	T0DATA = 0;
}


void TIMER1_PWM_Init(void)
{
    T1CR   = 0b00000000;     	//使能PWM,CPU,2分频
    T1CNT  = 0;
    T1LOAD = 178;          		//62.5us
    //T1DATA = 0;
    T1EN   = 1;
    T1IE   = 1;
}

/*
void TIMER2_PWM_Init(void)
{
    T2CR   = 0b10000001;     //使能PWM,CPU,2分频
    T2CNT  = 125-1;
    T2LOAD = 125-1;          //62.5us
    T2DATA = 0;
    T2EN   = 0;
}
*/
/************************************************
;  *    @函数名            : Sys_Init
;  *    @说明              : 系统初始化
;  *    @输入参数          :
;  *    @返回参数          :
;  ***********************************************/
void Sys_Init(void)
{
    GIE = 0;
    CLR_RAM();
    IO_Init();
    ADC_Power_On_Stability();
    while (ADC_Zero_ADJ());                      //demo演示,校准失败一直校准，用户按照需求选择是否校准
    TIMER0_INT_Init();
	TIMER1_PWM_Init();
	//TIMER2_PWM_Init();
	
    ADCR0 = 0B10101011;                      //使能ADC、12位数据H8L4、VDD/4通道
    ADCR1 = 0x80;                      //32分频、内部VDD
    ADCR2 = 0xff;                      //固定为15ADCLK
    GIE = 1;
}
/************************************************
;  *    @函数名            : main
;  *    @说明              : 主程序
;  *    @输入参数          :
;  *    @返回参数          :
;  ***********************************************/
void main(void)
{
    Sys_Init();
    while (1)
    {
		if(TIME_500US_F)
   	   	{
   	   	   	TIME_500US_F=0;
   	   	   	ClrWdt();
   	   	   	SCAN_HOT_BAT_AD();
   	   	} 	
		
   	   	if(TIME_1MS_F)
   	   	{
   	   	   	TIME_1MS_F=0;
   	   	   	ClrWdt();
			SCAN_KEY_CTR();
			SCAN_CHARGE_CTR();
   	   	   	SCAN_FUNTION_CTR();
			SCAN_SMG_DIS();
			SCAN_SLEEP_CTR();
   	   	}
    }
}


/*********************** SCAN_KEY_CTR ************************/
void SCAN_KEY_CTR(void)
{
	if((!KEY)&&UP_POWER_F&&B_UP_POWER_CK&&(!B_LOW_POWER_F))
	{
		KEY_CNT2=0;
		if(!KEY_PRESS_L)
		{
			KEY_CNT1++;
			if(KEY_CNT1>19)
			{
				KEY_PRESS=1;
			}

			if(KEY_CNT1>999)
			{
				KEY_CNT1=0;
				KEY_PRESS_L=1;
				WORK_ON=0;

				B_MOTO_LEVE_ZS=0;
				//if(!CHARGE_EN)
				//{
					//BAT_CHECK_F=1;
				//}

			}
		}
	}
	else
	{
		KEY_CNT1=0;
		KEY_CNT2++;
		if(KEY_CNT2>19)
		{
			KEY_CNT2=0;
			if(KEY_PRESS_L)
			{
				KEY_PRESS_L=0;
				KEY_PRESS=0;
			}
			else if(KEY_PRESS)
			{
				KEY_PRESS=0;
				WORK_ON=1;
				//if(!CHARGE_EN)
				//{
				//	BAT_CHECK_F=1;
				//}
				B_MOTO_LEVE_ZS=1;
				MOTO_DIS_CNT=0;
				PWM_INONE=0;

				MOTO_CNT2=0;
				
				MOTO_MODE++;
				if(MOTO_MODE>5)
				{
					MOTO_MODE=0;
					WORK_ON=0;
					B_MOTO_LEVE_ZS=0;
				}
				
				//if(MOTO_MODE==1)
				//{
				//	BUFF_MOTO= 31;
				//}
			}
		}
	}
	
}

void SCAN_CHARGE_CTR(void)
{
	if(IN_5V&&UP_POWER_F&&B_UP_POWER_CK)
	{
		if(!CHARGE_EN)
		{
			CHARGE_CNT1++;
			if(CHARGE_CNT1>199)
			{
				CHARGE_CNT1=0;
				CHARGE_EN=1;
				CHARGE_FULL=0;
			}
		}
		else
		{
			if(IN_CHARGE==1)
			{
				CHARGE_CNT2=0;
				CHARGE_CNT1++;
				if(CHARGE_CNT1>199)
				{
					CHARGE_CNT1=0;
					CHARGE_EN=1;
					CHARGE_FULL=1;
					B_LOW_POWER_F=0;
					B_LOW_P_DIS_F=0;
				}
			}
			else
			{
				CHARGE_CNT1=0;
				CHARGE_CNT2++;
				if(CHARGE_CNT2>199)
				{
					CHARGE_CNT2=0;
					CHARGE_EN=1;
					CHARGE_FULL=0;
					B_LOW_POWER_F=0;
					B_LOW_P_DIS_F=0;
				}
			}
		}
	}
	else
	{
		CHARGE_CNT1=0;
		CHARGE_CNT2=0;
		CHARGE_CNT3++;
		if(CHARGE_CNT3>199)
		{
			CHARGE_CNT3=0;
			if(CHARGE_EN)
			{
				CHARGE_EN=0;
				CHARGE_FULL=0;
				B_LOW_POWER_F=0;
			}
		}
	}
}


void SCAN_FUNTION_CTR(void)
{		
		if(WORK_ON)
		{
			if(MOTO_MODE==0)
			{
				TIMER0_INT_Init();
				MOTO=0;
			}
			else 
			{
				MOTO=1;
				if(!PWM_INONE)
				{
					PWM_INONE=1;
					T0EN   = 1;
					PWM0EC = 1;
				}
					
				if(MOTO_MODE==1)			//0.45V
				{
					BUFF_OVER_ALL2=922;
					BUFF_M_CNT=2;
				}
				else if(MOTO_MODE==2)		//0.54V
				{
					BUFF_OVER_ALL2=1106;
					BUFF_M_CNT=3;
				}
				else if(MOTO_MODE==3)		//0.63V
				{
					BUFF_OVER_ALL2=1290;
					BUFF_M_CNT=3;
				}
				else if(MOTO_MODE==4)		//0.72V
				{
					BUFF_OVER_ALL2=1475;
					BUFF_M_CNT=4;
				}
				else if(MOTO_MODE==5)		//0.81V
				{
					BUFF_OVER_ALL2=1679;
					BUFF_M_CNT=5;
				}	
				T0DATA = BUFF_MOTO;
			}
		}
		else
		{
			TIMER0_INT_Init();
			MOTO=0;
			MOTO_MODE=0;
			MOTO_DIS_CNT=0;
		}
}

/********************* SCAN_SMG_CTR *************************/
void SCAN_SMG_CTR(void)
{
   	P00OE=0;
   	P01OE=0;
   	P02OE=0;
   	P03OE=0;
   	P15OE=0;

	B_HAVE_LED=0;
	
   	LED_MODE++;
   	if(LED_MODE>4)
   	{
   	   	LED_MODE=0;
   	}

   	if(LED_MODE==0)
   	{	
		if(B_G_A)
		{
   	   	   	P03OE=1;
   	   	   	LED_SMG2=0;
			B_HAVE_LED=1;
		}
		if(B_G_C)
		{
   	   	   	P02OE=1;
   	   	   	LED_SMG3=0;
			B_HAVE_LED=1;
		}
		if(B_G_E)
		{
   	   	   	P01OE=1;
   	   	   	LED_SMG4=0;
			B_HAVE_LED=1;
		}
		

		if(B_HAVE_LED)
		{
			P15OE=1;
			LED_SMG1=1;
		}
   	}

   	else if(LED_MODE==1)
   	{
		if(B_C_1)
		{
   	   	   	P01OE=1;
   	   	   	LED_SMG4=0;
			B_HAVE_LED=1;
			
		}
		
		if(B_S_A)
		{
   	   	   	P02OE=1;
   	   	   	LED_SMG3=0;
			B_HAVE_LED=1;
			
		}
		
		if(B_G_B)
		{
   	   	   	P15OE=1;
   	   	   	LED_SMG1=0;
			B_HAVE_LED=1;
		
		}
		
		if(B_K_DL)
		{
   	   	   	P00OE=1;
   	   	   	LED_SMG5=0;
			B_HAVE_LED=1;
		}

		if(B_HAVE_LED)
		{
			P03OE=1;
			LED_SMG2=1;
		}
   	}

   	else if(LED_MODE==2)
   	{
		if(B_B_1)
		{
   	   	   	P01OE=1;
   	   	   	LED_SMG4=0;
			B_HAVE_LED=1;
			
		}
		
		if(B_S_B)
		{
   	   	   	P03OE=1;
   	   	   	LED_SMG2=0;
			B_HAVE_LED=1;
			
		}
		
		if(B_G_D)
		{
   	   	   	P15OE=1;
   	   	   	LED_SMG1=0;
			B_HAVE_LED=1;
		
		}
		

		if(B_K_CHARGE)
		{
   	   	   	P00OE=1;
   	   	   	LED_SMG5=0;
			B_HAVE_LED=1;
		
		}

		if(B_HAVE_LED)
		{
			P02OE=1;
			LED_SMG3=1;
		}
   	}
   	
   	else if(LED_MODE==3)
   	{
		if(B_S_C)
		{
   	   	   	P02OE=1;
   	   	   	LED_SMG3=0;
			B_HAVE_LED=1;
			
		}
		
		if(B_S_D)
		{
   	   	   	P03OE=1;
   	   	   	LED_SMG2=0;
			B_HAVE_LED=1;
			
		}
		
		if(B_G_F)
		{
   	   	   	P15OE=1;
   	   	   	LED_SMG1=0;
			B_HAVE_LED=1;
		
		}
		

		if(B_HAVE_LED)
		{
			P01OE=1;
			LED_SMG4=1;
		}
   	}
   	
   	else if(LED_MODE==4)
   	{
		if(B_S_E)
		{
   	   	   	P03OE=1;
   	   	   	LED_SMG2=0;
			B_HAVE_LED=1;
			
		}
		
		if(B_S_F)
		{
   	   	   	P02OE=1;
   	   	   	LED_SMG3=0;
			B_HAVE_LED=1;
			
		}
		
		if(B_S_G)
		{
   	   	   	P01OE=1;
   	   	   	LED_SMG4=0;
			B_HAVE_LED=1;
		
		}
		
		if(B_G_G)
		{
   	   	   	P15OE=1;
   	   	   	LED_SMG1=0;
			B_HAVE_LED=1;
		
		}
		
		if(B_HAVE_LED)
		{
			P00OE=1;
			LED_SMG5=1;
		}
   	}
   	
}




//*********************** SCAN_SMG_DIS ***********************/
void	SCAN_SMG_DIS(void)
{
	if(!UP_POWER_F)
	{
		B_SMG_S.byte=0XFF;
		B_SMG_B.byte=0XFF;
		B_SMG_C.byte=0XFF;
		CHARGE_CNT++;
		if(CHARGE_CNT>1499)
		{
			CHARGE_CNT=0;
			UP_POWER_F=1;
		}

	}
	else if((WORK_ON||CHARGE_EN)&&(B_UP_POWER_CK))	
	{
		if(B_MOTO_LEVE_ZS)
		{
			if(MOTO_MODE==1)
			{
				B_SMG_S.byte=0X00;
				B_SMG_B.byte=0X06;
				B_SMG_C.byte=0X00;
				B_K_CHARGE=0;
			}
			else if(MOTO_MODE==2)
			{
				B_SMG_S.byte=0X00;
				B_SMG_B.byte=0X5B;
				B_SMG_C.byte=0X00;
				B_K_CHARGE=0;
			}
			else if(MOTO_MODE==3)
			{
				B_SMG_S.byte=0X00;
				B_SMG_B.byte=0X4F;
				B_SMG_C.byte=0X00;
				B_K_CHARGE=0;

			}
			else if(MOTO_MODE==4)
			{
				B_SMG_S.byte=0X00;
				B_SMG_B.byte=0X66;
				B_SMG_C.byte=0X00;
				B_K_CHARGE=0;

			}
			else if(MOTO_MODE==5)
			{
				B_SMG_S.byte=0X00;
				B_SMG_B.byte=0X6D;
				B_SMG_C.byte=0X00;
				B_K_CHARGE=0;
			}
			
			MOTO_DIS_CNT++;
			if(MOTO_DIS_CNT>1499)
			{
				MOTO_DIS_CNT=0;
				B_MOTO_LEVE_ZS=0;
				//SET_DL_DIS(SET_BAT_VALUE);
				DL_DIS_CNT=0;
				PL_DIS_CNT1=0;
			}
			
		}
		else 
		{
			if(CHARGE_EN)
			{
				DL_DIS_CNT2=0;
				DL_DIS_CNT1++;
				if(DL_DIS_CNT1>24999) //29999
				{
					DL_DIS_CNT1=0;
					if(BUFF_BAT_VALUE>SET_BAT_VALUE)
					{
						if(SET_BAT_VALUE<100)
						{
							SET_BAT_VALUE++;
						}
					}
				}
			}
			
			else if(WORK_ON)	
			{
				DL_DIS_CNT1=0;
				DL_DIS_CNT2++;
				if(DL_DIS_CNT2>24999)  //29999
				{
					DL_DIS_CNT2=0;

					if(SET_BAT_VALUE==0)
					{
						B_LOW_POWER_F=1;
						B_LOW_P_DIS_F=0;
						WORK_ON=0;
						MOTO_DIS_CNT=0;
						B_MOTO_LEVE_ZS=0;
						MOTO_MODE=0;

					}
					
					if(BUFF_BAT_VALUE<SET_BAT_VALUE)
					{
						if(SET_BAT_VALUE>0)
						{
							SET_BAT_VALUE--;
						}
					}
					
				}

				if(SET_BAT_VALUE<3)
				{
					B_LOW_P_DIS_F=1;
				}
			}
			
			if(CHARGE_EN)
			{
				CHARG_DIS_CNT++;
				if(CHARG_DIS_CNT>999)
				{
					CHARG_DIS_CNT=0;
				}
				if(CHARG_DIS_CNT<499)
				{
					B_K_CHARGE=1;

				}
				else
				{
					if(!CHARGE_FULL)
					{
						B_K_CHARGE=0;
					}
					else
					{
						B_K_CHARGE=1;
					}
				}
				
			}
			else
			{
				B_K_CHARGE=0;
				CHARG_DIS_CNT=0;
			}

			if(B_LOW_POWER_F)
			{
				WORK_ON=0;
				MOTO_DIS_CNT=0;
				B_MOTO_LEVE_ZS=0;
				MOTO_MODE=0;
			}

			if(B_LOW_P_DIS_F)
			{
				PL_DIS_CNT1++;
				if(PL_DIS_CNT1>999)
				{
					PL_DIS_CNT1=0;
				}
				
				if(PL_DIS_CNT1<499)
				{
					DL_DIS_CNT=0;
					B_K_CHARGE=0;
					B_SMG_S.byte=0X00;
					B_SMG_B.byte=0X00;
					B_SMG_C.byte=0X00;

				}
				else
				{
					B_K_CHARGE=1;
					DL_DIS_CNT++;
					if(DL_DIS_CNT>49)
					{
						DL_DIS_CNT=0;
						SET_DL_DIS(SET_BAT_VALUE);

					}
				}
			}
			else
			{
				PL_DIS_CNT1=0;
				B_K_DL=1;
				DL_DIS_CNT++;
				if(DL_DIS_CNT>49)
				{
					DL_DIS_CNT=0;
					SET_DL_DIS(SET_BAT_VALUE);
				}
			}
		}
	}
	else
	{
		DL_DIS_CNT1=0;
		DL_DIS_CNT2=0;
		DL_DIS_CNT=0;
		PL_DIS_CNT1=0;
		B_SMG_S.byte=0;
		B_SMG_B.byte=0;
		B_SMG_C.byte=0;
	}
}


//************************* GET_SMG_VALUE *******************/
void	GET_SMG_VALUE(uchar	x)
{

	if(x==0)
	{
		G_VAL=0X3F;

	}
	else if(x==1)
	{
		G_VAL=0X06;

	}
	else if(x==2)
	{
		G_VAL=0X5B;

	}
	else if(x==3)
	{
		G_VAL=0X4F;

	}
	else if(x==4)
	{
		G_VAL=0X66;

	}
	else if(x==5)
	{
		G_VAL=0X6D;

	}
	else if(x==6)
	{
		G_VAL=0X7D;

	}
	else if(x==7)
	{
		G_VAL=0X07;

	}
	else if(x==8)
	{
		G_VAL=0X7F;

	}
	else if(x==9)
	{
		G_VAL=0X6F;

	}
	else if(x==10)
	{
		G_VAL=0X71;

	}

}


//******************* SET_DL_DIS ************************/
void	SET_DL_DIS(uchar	DL)
{
	
	if(DL>99)
	{
		B_SMG_S.byte=0X3F;
		B_SMG_B.byte=0X3F;
		B_SMG_C.byte=0X03;

	}
	else if(DL<10)
	{
		B_SMG_S.byte=0;
		B_SMG_C.byte=0X00;
		BUFF_SMG_DAT=DL%10;
		GET_SMG_VALUE(BUFF_SMG_DAT);
		B_SMG_B.byte=G_VAL;
	}
	else
	{
		B_SMG_C.byte=0X00;
		BUFF_SMG_DAT=DL%10;
		GET_SMG_VALUE(BUFF_SMG_DAT);
		B_SMG_B.byte=G_VAL;
		BUFF_SMG_DAT=DL/10;
		GET_SMG_VALUE(BUFF_SMG_DAT);
		B_SMG_S.byte=G_VAL;
	}

}

/********************** SCAN_HOT_BAT_AD *************/
void SCAN_HOT_BAT_AD(void)
{
	if(AD_MODE==0)
	{
		ADCR0 = 0B10101011; 			   //使能ADC、12位数据H8L4、VDD/4通道
		ADCR1 = 0x80;					   //32分频、内部2V
		ADCR2 = 0xff;					   //固定为15ADCLK
	}
	else if(AD_MODE==1)
	{
		if((WORK_ON)||(CHARGE_EN)||(!B_UP_POWER_CK))
		{
				ADEOC = 0;
				WAIT_ADC_CHANGE=0;
				GET_ADC_ERROR=0;
				GET_ADC_OK=1;
				g_temp_value=0;
				
				while(!ADEOC)
				{
					WAIT_ADC_CHANGE++;
					if(WAIT_ADC_CHANGE>1500)
					{
						WAIT_ADC_CHANGE=0;
						GET_ADC_ERROR=1;
						GET_ADC_OK=0;
						return;
					}
				}
				
				if(GET_ADC_OK)
				{
					GET_ADC_OK=0;
					g_temp_value = ADRH;
					g_temp_value = g_temp_value << 4 | (ADRL & 0x0f);
					
					W_OVER_ALL1+=g_temp_value;
					R_AD_CNT1++;
					if(R_AD_CNT1>7)
					{
						R_AD_CNT1=0;
						W_OVER_ALL1 = W_OVER_ALL1>>3;
						/*
							... 	
						*/
				
		
						if((!B_UP_POWER_CK))
						{
							BAT_CNT5++;
							if(BAT_CNT5>19)
							{
								BAT_CNT5=0;
								if(W_OVER_ALL1<1587)		//3.1v
								{
									BUFF_BAT_VALUE=0;
									B_LOW_POWER_F=1;
								}
								else if(W_OVER_ALL1>2048)	//4v
								{
									BUFF_BAT_VALUE=100;
								}
								else
								{
						
									W_OVER_ALL1=W_OVER_ALL1-1587;
									BUFF_BAT_VALUE=W_OVER_ALL1/5;
								}
								
								SET_BAT_VALUE=BUFF_BAT_VALUE;
								B_UP_POWER_CK=1;
								//BAT_CHECK_F=0;
							}
						}
		
						else if(CHARGE_EN)
						{
							if(CHARGE_FULL)
							{
								BUFF_BAT_VALUE=100;
							}
							else
							{
								W_OVER_ALL1=W_OVER_ALL1-1587;
								BUFF_BAT_VALUE=W_OVER_ALL1/5;
								if(BUFF_BAT_VALUE>99)
								{
									BUFF_BAT_VALUE=99;	
								}
							}
						}
						
						else
						{
							if(W_OVER_ALL1<1587)
							{
								BUFF_BAT_VALUE=0;
								B_LOW_POWER_F=1;
							}
							else if(W_OVER_ALL1>2048)
							{
								BUFF_BAT_VALUE=100;
							}
							else
							{
								W_OVER_ALL1=W_OVER_ALL1-1587;
								BUFF_BAT_VALUE=W_OVER_ALL1/5;
							}
							//SET_BAT_VALUE=BUFF_BAT_VALUE;
						}
						
						W_OVER_ALL1=0;
					}
				}
				else
				{
					W_OVER_ALL1=0;
					R_AD_CNT1=0;
					g_temp_value=0;
				}
		}		
		else
		{
			W_OVER_ALL1=0;
			R_AD_CNT1=0;
			g_temp_value=0;
		}
	}	
	
	else if(AD_MODE==2)
	{
		ADCR0 = 0B00111011; 			   //使能ADC、12位数据H8L4、AN3通道
		ADCR1 = 0x80;					   //32分频、内部2V
		ADCR2 = 0xff;					   //固定为15ADCLK
	}
	else if(AD_MODE==3)
	{
		if((WORK_ON)||(CHARGE_EN))
		{
				ADEOC = 0;
				WAIT_ADC_CHANGE1=0;
				GET_ADC_ERROR=0;
				GET_ADC_OK=1;
				g_temp_value1=0;
				
				while(!ADEOC)
				{
					WAIT_ADC_CHANGE1++;
					if(WAIT_ADC_CHANGE1>1500)
					{
						WAIT_ADC_CHANGE1=0;
						GET_ADC_ERROR=1;
						GET_ADC_OK=0;
						return;
					}
				}
				
				if(GET_ADC_OK)
				{
					GET_ADC_OK=0;
					g_temp_value1 = ADRH;
					g_temp_value1 = g_temp_value1 << 4 | (ADRL & 0x0f);
					
					W_OVER_ALL2+=g_temp_value1;
					R_AD_CNT2++;
					if(R_AD_CNT2>7)
					{
						R_AD_CNT2=0;
						W_OVER_ALL2 = W_OVER_ALL2>>3;
						/*
							... 	
						*/
				
						if(W_OVER_ALL2<2048)			//1V  
						{
							IN_LOAD_CNT2=0;
							IN_LOAD_CNT1++;
							if(IN_LOAD_CNT1>5)
							{
								IN_LOAD_CNT1=0;
							}
						}
						else
						{
							IN_LOAD_CNT1=0;
							IN_LOAD_CNT2++;
							if(IN_LOAD_CNT2>5)
							{
								IN_LOAD_CNT2=0;
								WORK_ON=0;
							}
						}
						
						if(WORK_ON)
						{
							if(W_OVER_ALL2<BUFF_OVER_ALL2) 		//
							{
								MOTO_CNT2=0;
								MOTO_CNT1++;
								if(MOTO_CNT1>BUFF_M_CNT)
								{
									MOTO_CNT1=0;
									if(BUFF_MOTO<105)
									{
										BUFF_MOTO++;
									}
								}
							}
							else if(W_OVER_ALL2>BUFF_OVER_ALL2)
							{
								MOTO_CNT1=0;
								MOTO_CNT2++;
								if(MOTO_CNT2>BUFF_M_CNT)
								{
									MOTO_CNT2=0;
									if(BUFF_MOTO>0)
									{
										BUFF_MOTO--;
									}
								}
							}
						}
						
						W_OVER_ALL2=0;
					}
				}
				else
				{
					W_OVER_ALL2=0;
					R_AD_CNT2=0;
					g_temp_value1=0;
				}
		}		
		else
		{
			W_OVER_ALL2=0;
			R_AD_CNT2=0;
			g_temp_value1=0;
		}
	}

	AD_MODE++;
	if(AD_MODE>3)
	{
		AD_MODE=0;
	}
}

/************************************************
;  *    @函数名            : ADC_Power_On_Stability
;  *    @说明              : ADC上电稳定
;  *    @输入参数          :
;  *    @返回参数          :
;  ***********************************************/
void ADC_Power_On_Stability(void)
{
    uint8_t cnt;
    ADCR0 = 0B10101011;                      //使能ADC、12位数据H8L4、VDD/4通道
    ADCR1 = 0x80;                      //32分频、内部VDD
    ADCR2 = 0xff;                      //固定为15ADCLK
    ADEOC = 0;
    while (!ADEOC);
    cnt = 0;
    while (!(((ADRH >> 2) == 0x10) || ((ADRH >> 2) == 0x0F)))
    {
        ADEOC = 0;
        while (!ADEOC);
        cnt ++;
        if (cnt >= 200)
            return;                    //用户可以添加返回值,提示错误
    }
}
/************************************************
;  *    @函数名            : ADC_Zero_ADJ
;  *    @说明              : ADC零点校准
;  *    @输入参数          :
;  *    @返回参数          :0 校准成功  1校准失败
;  *    用户按照需求选择是否校准
;  ***********************************************/
uint8_t ADC_Zero_ADJ(void)
{
    ADCR0 = 0xeb;                          //使能ADC、12位数据H8L4、GND通道
    ADCR1 = 0x80;                                //32分频、内部2V
    ADCR2 = 0xff;                                //固定为15ADCLK
    OSADJCR |= DEF_SET_BIT7;                     //使能零点校准、负向修调
    ADEOC = 0;
    while (!ADEOC);
    if ((ADRH == 0) && ((ADRL & 0x0F) == 0))     //结果是否为0
    {
        OSADJTD = 1;                             //正向修调
        OSADJCR |= 0x3F;
        ADEOC = 0;
        while (!ADEOC);
        if ((ADRH == 0) && ((ADRL & 0x0F) == 0)) //结果是否为0
        {
            ADEN = 0;
            return PASS;
        }
        else
        {
            while (1)
            {
                if (OSADJCR & 0x3f)
                {
                    OSADJCR --;
                    ADEOC = 0;
                    while (!ADEOC);
                    if ((ADRH == 0) && ((ADRL & 0x0F) == 0))//结果是否为0
                    {
                        ADEN = 0;
                        return PASS;
                    }
                }
                else
                {
                    ADEN = 0;
                    return FAIL;
                }
            }
        }
    }
    else
    {
        while (1)
        {
            if ((OSADJCR & 0x3f) == 0x3f)
            {
                ADEN = 0;
                return FAIL;
            }
            else
            {
                OSADJCR ++;
                ADEOC = 0;
                while (!ADEOC);
                if ((ADRH == 0) && ((ADRL & 0x0F) == 0))//结果是否为0
                {
                    ADEN = 0;
                    return PASS;
                }
            }
        }
    }
}

void SCAN_SLEEP_CTR(void)
{
	if((!KEY)||(KEY_PRESS)||(WORK_ON)||(CHARGE_EN)||(KEY_PRESS_L)||(IN_5V)||(!B_UP_POWER_CK)||(!UP_POWER_F))
	{
		SL_CNT=0;
		return;
	}
		SL_CNT++;				//每一毫秒加一次
		if(SL_CNT<249)
		{	
			return;
		}
		SL_CNT=0;	
		ADCR0 = 0B00000000; 						 //使能ADC、12位数据H8L4
		ADCR1 = 0x80;								 //32分频、内部2V 
		ADCR2 = 0xff;								 //固定为15ADCLK 
		IO_Init();
		P00OE=1;
		P01OE=1;
		P02OE=1;
		P03OE=1;
		P15OE=1;

		ClrWdt();		
		GIE = 0;	//关闭中断总开关
		T0EN = 0;
		T0IE = 0;
		P1KBCR = 0b00001010;	//使能P14键盘中断
		KBIE = 1;				//使能键盘中断
		Nop();
		Nop();
		Nop();
		Stop(); 				//进入低功耗模式
		Nop();
		Nop();
		ClrWdt();		
		KBIE = 0;				//屏蔽键盘中断
		P1KBCR = 0b00000000;	//屏蔽键盘中断
		IO_Init();
		TIMER0_INT_Init();		//初始化T0定时器
		TIMER1_PWM_Init();
		GIE = 1;				//打开中断总开关
		Nop();
		Nop();


}


/************************************************
;  *    @函数名            : interrupt
;  *    @说明              : 中断函数
;  *    @输入参数          :
;  *    @返回参数          :
;  ***********************************************/
void int_isr(void) __interrupt
{
__asm 
    movra _abuf
    swapar _PFLAG
    movra _statusbuf
__endasm;
//============================T0==========================
    if (T1IF & T1IE)
    {
        T1IF = 0;
		T1CNT=178;
		
   	   	TIME_500US_CNT++;
   	   	TIME_1MS_CNT++;
   	   	if(TIME_1MS_CNT>19)
   	   	{
   	   	   	TIME_1MS_CNT=0;
   	   	   	TIME_1MS_F=1;
   	   	}

   	   	if(TIME_500US_CNT>9)
   	   	{
   	   	   	TIME_500US_CNT=0;
   	   	   	TIME_500US_F=1;
   	   	}
		SCAN_SMG_CTR();
    }

   	__asm 
    swapar _statusbuf
    movra _PFLAG
    swapr _abuf
    swapar _abuf
__endasm;
}

/**************************** end of file *********************************************/
