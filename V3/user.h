/******************************************************************************
;  *       @型号                   : MC32P7351
;  *       @创建日期               : 2022.12.16
;  *       @公司/作者              : sinomcu.com-FAE
;  *       @晟矽微技术支持         : 2048615934
;  *       @晟矽微官网             : http://www.sinomcu.com/
;  *       @版权                   : 2022 sinomcu.com公司版权所有.
;  *---------------------- 建议 ---------------------------------
;  *                   变量定义时使用全局变量                       
******************************************************************************/
#ifndef USER
#define USER
#include "mc32-common.h"
#include "MC32P7351.h"

/*****************************************************************
;       Function : Define variables
;*****************************************************************/

#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long int
#define uint8_t  unsigned char
#define uint16_t unsigned int
#define uint32_t unsigned long int

#define DEF_SET_BIT0                0x01
#define DEF_SET_BIT1                0x02
#define DEF_SET_BIT2                0x04
#define DEF_SET_BIT3                0x08
#define DEF_SET_BIT4                0x10
#define DEF_SET_BIT5                0x20
#define DEF_SET_BIT6                0x40
#define DEF_SET_BIT7                0x80

#define DEF_CLR_BIT0                0xFE
#define DEF_CLR_BIT1                0xFD
#define DEF_CLR_BIT2                0xFB
#define DEF_CLR_BIT3                0xF7
#define DEF_CLR_BIT4                0xEF
#define DEF_CLR_BIT5                0xDF
#define DEF_CLR_BIT6                0xBF
#define DEF_CLR_BIT7                0x7F


//============Define  IO=================
#define 		KEY				P13D
#define  	 	IN_5V			P11D
#define  	 	IN_CHARGE		P12D
#define  	 	IN_LOAD			P04D

#define  	 	LED_SMG5		P00D
#define  	 	LED_SMG4		P01D
#define  	 	LED_SMG3		P02D
#define  	 	LED_SMG2		P03D
#define  	 	LED_SMG1		P15D

#define  	 	MOTO			P17D
#define  	 	SY_IO			P16D


//============Define  Flag=================
typedef union {
    unsigned char byte;
    struct
    {
        u8 bit0 : 1;
        u8 bit1 : 1;
        u8 bit2 : 1;
        u8 bit3 : 1;
        u8 bit4 : 1;
        u8 bit5 : 1;
        u8 bit6 : 1;
        u8 bit7 : 1;
    } bits;
}bit_flag;

volatile bit_flag flag1;
#define    	KEY_PRESS			flag1.bits.bit0
#define    	KEY_PRESS_L			flag1.bits.bit1
#define    	CHARGE_EN			flag1.bits.bit2
#define    	B_UP_POWER_CK		flag1.bits.bit3
#define    	TIME_500US_F		flag1.bits.bit4
#define    	B_MOTO_LEVE_ZS		flag1.bits.bit5
#define    	CHARGE_FULL			flag1.bits.bit6
#define    	UP_LOAD_F			flag1.bits.bit7


volatile bit_flag flag2;
#define    	B_LOW_POWER_F		flag2.bits.bit0
#define    	B_LOW_P_DIS_F		flag2.bits.bit1
#define    	BAT_CHECK_F			flag2.bits.bit2
#define    	B_HAVE_LED				flag2.bits.bit3
#define    	B_2E_F				flag2.bits.bit4
#define    	B_2F_F				flag2.bits.bit5
#define    	PWM_INONE			flag2.bits.bit6

volatile bit_flag KEY_FLAG;
#define    	TIME_1MS_F	KEY_FLAG.bits.bit0
#define    	KEY_PRESS_F	KEY_FLAG.bits.bit1
#define    	WORK_ON	KEY_FLAG.bits.bit2
#define    	GET_ADC_ERROR	KEY_FLAG.bits.bit3
#define    	GET_ADC_OK	KEY_FLAG.bits.bit4
#define    	UP_POWER_F	KEY_FLAG.bits.bit5
#define    	BUZ_EN_F	KEY_FLAG.bits.bit6


volatile bit_flag B_SMG_S;
#define    	B_S_A			B_SMG_S.bits.bit0   	   	 //  
#define    	B_S_B		   	B_SMG_S.bits.bit1   	 
#define    	B_S_C			B_SMG_S.bits.bit2  	
#define    	B_S_D			B_SMG_S.bits.bit3  
#define    	B_S_E			B_SMG_S.bits.bit4   	
#define    	B_S_F			B_SMG_S.bits.bit5
#define    	B_S_G			B_SMG_S.bits.bit6

volatile bit_flag B_SMG_B;
#define    	B_G_A			B_SMG_B.bits.bit0   	   	 //  
#define    	B_G_B		   	B_SMG_B.bits.bit1   	 
#define    	B_G_C			B_SMG_B.bits.bit2  	
#define    	B_G_D			B_SMG_B.bits.bit3  
#define    	B_G_E			B_SMG_B.bits.bit4   	
#define    	B_G_F			B_SMG_B.bits.bit5
#define    	B_G_G			B_SMG_B.bits.bit6

volatile bit_flag B_SMG_C;
#define    	B_B_1			B_SMG_C.bits.bit0   	   	 //  
#define    	B_C_1		   	B_SMG_C.bits.bit1   	 
#define    	B_K_FAN		    B_SMG_C.bits.bit2  	
#define    	B_K_CHARGE		B_SMG_C.bits.bit3  
#define    	B_K_DL			B_SMG_C.bits.bit4 


#define		FAIL	1
#define		PASS	0


//===============Field Protection Variables===============
u8 abuf;
u8 statusbuf;

u16	KEY_CNT1;
u8	KEY_CNT2;
u8	MOTO_MODE;

u8	BUFF_MOTO;
u8	BUFF_SMG_DAT;
u8	SMG_MODE;
u8	B_BUFF_0;
u8	B_BUFF_1;
u8	TIME_500US_CNT;
u8	DL_DIS_CNT;
u8	TIME_1MS_CNT;
u8	R_AD_CNT1;
u8	G_VAL;
u8	CHARGE_CNT1;
u8	CHARGE_CNT2;
u8	CHARGE_CNT3;
u8	SL_CNT;
u8	LED_MODE;
u8	BAT_CNT5;
u8	AD_MODE;
u8	MOTO_CNT1;
u8	MOTO_CNT2;
u8	R_AD_CNT2;
u8	BUFF_M_CNT;





u16	CHARG_DIS_CNT;
u16	MOTO_DIS_CNT;
u16	CHARGE_CNT;
u16	DL_DIS_CNT1;
u16	DL_DIS_CNT2;
u16	BUFF_BAT_VALUE;
u16	SET_BAT_VALUE;
u16	IN_LOAD_CNT1;
u16	IN_LOAD_CNT2;
u16	W_OVER_ALL1;
u16	W_OVER_ALL2;
u16 BUFF_OVER_ALL2;

u16	WAIT_ADC_CHANGE;
u16	WAIT_ADC_CHANGE1;
u16	g_temp_value;
u16	g_temp_value1;
u16	PL_DIS_CNT1;




//===============Global Function===============
void 	Sys_Init(void);
void 	CLR_RAM(void);
void 	IO_Init(void);
void 	TIMER0_INT_Init(void);
void 	TIMER1_PWM_Init(void);
void 	TIMER2_PWM_Init(void);


void	SCAN_KEY_CTR(void);
void	SCAN_CHARGE_CTR(void);
void	SCAN_SMG_CTR(void);
void 	SCAN_SMG_DIS(void);
void	GET_SMG_VALUE(uchar	x);
void	SET_DL_DIS(uchar	DL);
void	SCAN_FUNTION_CTR(void);
void	SCAN_SLEEP_CTR(void);

void	SCAN_HOT_BAT_AD(void);
void 	ADC_Power_On_Stability(void);
uint8_t ADC_Zero_ADJ(void);


#endif


/**************************** end of file *********************************************/
