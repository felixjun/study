#include "App_Car.h"


short gx=0,gy=0,gz=0,ax=0,ay=0,az=0;
float accel_angle;//通过加速度计算出来的角度
float gyro_y;//采样值转化成角度

extern float angle; // 卡尔曼滤波后的角度
int ea, eb;

char bat_str[5];   // 2个整数位+1个小数点+1个小数位+1个字符串结尾\0
char ea_str[7];    // 1个符号位+5个整数位+1个字符串结尾\0
char eb_str[7];    // 1个符号位+5个整数位+1个字符串结尾\0
char angle_str[7]; // 1个符号位+3个整数位+1个小数点+1个小数位+1个字符串结尾\0

/* =======直立环PID参数========= */
float balance_kp = -1500.0;  // 刚出现低频震荡的值=-5500，乘0.6=-540
float balance_kd = 2;     // 刚出现高频震荡的值+5，乘0.6=+0.9
float balance_angle = -1.0; // -1.0
/* =======速度环PID参数========= */
float velocity_kp = 500.0; // 
float velocity_ki = 2.5;  // 1/200



/* =======转向环PID参数========= */
float turn_kp = 0.5; // 0.5




/* 遥控运动的标志位 */
uint8_t flag_up = 0, flag_down = 0, flag_left = 0, flag_right = 0;
int remote_move = 0; // 遥控前后移动的控制量
int remote_turn = 0; // 遥控左右移动的控制量







//计算小车倾角
void App_Car_GetAngle(void)
{
	//读取MPU6050的数据
	Int_MPU6050_GetAccl(&ax,&ay,&az);
	Int_MPU6050_GetGyro(&gx,&gy,&gz);
	
	//通过加速度计算角度
	accel_angle=atan2(ax,az)*180/PI;//求的是弧度
	
	 
	//角速度  65536表示正负2000°，1°有16.4个采样值  
	//角速度符号要和加速度符号一致
	gyro_y=-gy/16.4;
	//算出的倾角和角速度进行卡尔曼滤波
	Com_Filter_Kalman(accel_angle,gyro_y);
	
//	printf("accel_angle=%.1f\r\n",accel_angle);
//	printf("gyro_y=%.1f\r\n",gyro_y);
	ea = Int_Encoder_ReadCounter(2);
    eb = -Int_Encoder_ReadCounter(3);

}

void App_Car_Display(void)
{
    /* 1.填充电压值 */
    double bat_vol = Driver_ADC1_ReadV();
    sprintf(bat_str,"%3.1f",bat_vol);
    OLED_ShowString(32,0,bat_str,16,1);

    /* 2.填充编码器值 */
    sprintf(ea_str, "%6d", ea);
    sprintf(eb_str, "%6d", eb);
    OLED_ShowString(24,16,ea_str,16,1); // 前面有EA:三个字符，x=3*8=24开始;第二行，y=16
    OLED_ShowString(24, 32, eb_str, 16, 1); // 前面有EB:三个字符，x=3*8=24开始;第三行，y=32

    /* 3.填充角度值 */
    sprintf(angle_str, "%5.1f", angle);
    OLED_ShowString(48, 48, angle_str, 16, 1); // 前面有Angle:六个字符，x=6*8=48开始;第四行，y=48

    /* 4.刷写到显存中 */
    OLED_Refresh();
}

void App_Car_PID(void)
{
	int balance_out=0;
	int velocity_out=0;
	int turn_out=0;
	int pwma=0,pwmb=0;
	
	//直立环控制
	balance_out=Com_PID_Balance(balance_kp,balance_kd,angle,balance_angle,gy);
	
	//速度环控制
	/* 添加遥控前后移动的逻辑 */
    if (flag_up)
    {
        remote_move = 50;
    }
    else if (flag_down)
    {
        remote_move = -50;
    }
    else
    {
        /* 如果没有前后控制，需要主动清零控制量，不要影响正常的平衡控制 */
        remote_move = 0;
    }

	velocity_out=Com_PID_Velocity(velocity_kp,velocity_ki,ea,eb,remote_move);
	
	//转向环控制
	 if (flag_left)
    {
        remote_turn+=-20;
    }
    else if (flag_right)
    {
        remote_turn+=20;
    }
    else
    {
        /* 如果不左右转，要清零控制量 */
        remote_turn=0;    
        /* 不左右转，才进行转向环的控制 */
        turn_out = Com_PID_Turn(turn_kp, gz);
    }
    /* 一直按住会不断累加，所以加个限幅 */
    if(remote_turn >500)
    {
        remote_turn = 500;
    }
    else if(remote_turn<-500)
    {
        remote_turn=-500;
    }

	turn_out=Com_PID_Turn(turn_kp,gz);
	
	
	//叠加PID结果，作用到电机上
	pwma=balance_out+velocity_out+turn_out+ remote_turn;
	pwmb=balance_out+velocity_out-turn_out-remote_turn;
	
	Int_TB6612_SetPWM(pwma,pwmb);
}

//USART2中断处理函数
void USART2_IRQHandler(void)
{
    uint8_t receive_data = 0;
    /* 数据接收寄存器非空 */
    if (USART2->SR & USART_SR_RXNE)
    {
        receive_data = USART2->DR;
        switch (receive_data)
        {
        case 'U':
            flag_up = 1, flag_down = 0, flag_left = 0, flag_right = 0;
            break;
        case 'D':
            flag_up = 0, flag_down = 1, flag_left = 0, flag_right = 0;
            break;
        case 'L':
            flag_up = 0, flag_down = 0, flag_left = 1, flag_right = 0;
            break;
        case 'R':
            flag_up = 0, flag_down = 0, flag_left = 0, flag_right = 1;
            break;
        case 'S':
            flag_up = 0, flag_down = 0, flag_left = 0, flag_right = 0;
            break;
        default:
            flag_up = 0, flag_down = 0, flag_left = 0, flag_right = 0;
            break;
        }
    }

}

