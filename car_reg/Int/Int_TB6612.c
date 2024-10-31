#include "Int_TB6612.h"

/*TB6612初始化*/
void Int_TB6612_Init(void)
{
	//初始化定时器
	Driver_TIM4_Init();
	
	//初始化GPIO，PB12-15，推挽输出 mode=11，cnf=00
	GPIOB->CRH |=(GPIO_CRH_MODE12|GPIO_CRH_MODE13|GPIO_CRH_MODE14|GPIO_CRH_MODE15);
	GPIOB->CRH &=~(GPIO_CRH_CNF12|GPIO_CRH_CNF13|GPIO_CRH_CNF14|GPIO_CRH_CNF15);
	
	
	
}
//控制电机A的方向 uint8_t direct
void Int_TB6612_MotorA(uint8_t direct)
{
	if(direct==GO)
	{
		//正转AIN1=0；AIN2=1
		AIN1_L;
		AIN2_H;
	}
	else if(direct==BACK)
	{
		//反转 AIN1=1，AIN2=0
		AIN1_H;
		AIN2_L;
	}
	else
	{
		//刹车
		AIN1_H;
		AIN2_H;
	}
}

//控制电机B的方向 uint8_t direct
void Int_TB6612_MotorB(uint8_t direct)
{
	if(direct==GO)
	{
		//正转BIN1=0；BIN2=1
		BIN1_L;
		BIN2_H;
	}
	else if(direct==BACK)
	{
		//反转 BIN1=1，BIN2=0
		BIN1_H;
		BIN2_L;
	}
	else
	{
		//刹车
		BIN1_H;
		BIN2_H;
	}
}

//根据带符号的pwm值，设置转速和方向
void Int_TB6612_SetPWM(int pwma,int pwmb)
{
	if(pwma>0)
	{
		Int_TB6612_MotorA(GO);//要正转
	}
	else if(pwma<0)
	{
		Int_TB6612_MotorA(BACK);//要反转
		pwma=-pwma;
	}
	else
	{
		Int_TB6612_MotorA(STOP);//刹车
	}
	
	//设置定时器通道CH4的ccr值
	TIM4->CCR4=pwma;
	
	if(pwmb>0)
	{
		Int_TB6612_MotorB(GO);//要正转
	}
	else if(pwmb<0)
	{
		Int_TB6612_MotorB(BACK);//要反转
		pwmb=-pwmb;
	}
	else
	{
		Int_TB6612_MotorB(STOP);//刹车
	}
	
	//设置定时器通道CH3的ccr值
	TIM4->CCR3=pwmb;
}


