#include "freertos.h"
#include "task.h"
#include "main.h"
#include "Int_TB6612.h"
#include "int_Encoder.h"
#include "Dri_USART1.h"
#include "Dri_USART2.h"
#include "Int_MPU6050.h"
#include "App_Car.h"
#include "Dri_ADC.h"
#include "oled.h"
#include "App_Task.h"


void for_delay_ms(uint32_t ms)
{
    uint32_t Delay = ms * 72000 / 9; /* 72M时钟频率 */
    do
    {
        __NOP(); /* 空指令（NOP）来占用 CPU 时间 */
    } while (Delay--);
}


int main(){
	Driver_USART1_Init();
	Driver_USART2_Init();
	Int_TB6612_Init();
	Int_Encoder_Init();
	Int_MPU6050_Init();
	Driver_ADC1_Init();
	
	
	OLED_Init();
	
	OLED_Clear();
	/*
	第一个参数 x坐标--水平
	第二个参数 y坐标--垂直
	第三个坐标 要显示的字符串
	第四个坐标 字体高度
	第五个坐标 显示模式 0反  1正
	*/
	OLED_ShowString(0,0,"BAT:     V",16,1);
	OLED_ShowString(0,16,"EA:",16,1);
	OLED_ShowString(0,32,"EB:",16,1);
	OLED_ShowString(0,48,"Angel:",16,1);
	
	//测试OLED
	//OLED_ShowString(0,10,"chuck",12,1);
	
	
	OLED_Refresh();
	
	
	/*进入FresRTOS调度*/
	App_Task_Init();
	
	//short ax=0,ay=0,az=0,gx=0,gy=0,gz=0;
	
	//Int_TB6612_SetPWM(3600,-3600);//A3600,B-3600----A左轮 B右轮
	while(1)
	{
		/*测试编码器读取值*/
		//printf("tim2--cnt=%d\r\n",Int_Encoder_ReadCounter(2));
		//printf("tim3--cnt=%d\r\n",Int_Encoder_ReadCounter(3));
		//for_delay_ms(1000);
		
		
		/*测试MPU6050*/
//		Int_MPU6050_GetAccl(&ax,&ay,&az);
//		Int_MPU6050_GetGyro(&gx,&gy,&gz);
//		printf("gx=%d\r\n",gx);
//		printf("gy=%d\r\n",gy);
//		printf("gz=%d\r\n",gz);
//		printf("ax=%d\r\n",ax);
//		printf("ay=%d\r\n",ay);
//		printf("az=%d\r\n",az);
//		for_delay_ms(500);
		
		//测试计算角度
//		App_Car_GetAngle();
//		for_delay_ms(10);
		
		//测试电压角度
//		printf("电池电压=%.1f\r\n",Driver_ADC1_ReadV());
//		for_delay_ms(1000);


		//测试OLED显示
		//App_Car_GetAngle();
		//App_Car_Display();
		//for_delay_ms(1000);
	}
}
	
extern void xPortSysTickHandler(void);
void SysTick_Handler(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        xPortSysTickHandler();
    }
}
// 当调用printf的时候,会自动调用这个方法来执行,只需要调用一个通过串口发送字符的函数
int fputc(int c, FILE *file)
{
    Driver_USART1_SendChar(c);
    return c;
}

