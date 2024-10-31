#include "Int_Encoder.h"

void Int_Encoder_Init(void)
{
	Dri_TIM2_Init();
	Dri_TIM3_Init();
}

/**
 * @description: 读取指定编码器的计数值并清零
 * @param {uint8_t} TIMX  定时器编号
 * @return {*}  读取到的计数值
 */
int Int_Encoder_ReadCounter(uint8_t TIMX)
{
    int Encoder_TIM;
    switch (TIMX)
    {
    case 2: /* 定时器2编码器：左轮 */
        Encoder_TIM = (short)TIM2->CNT;
        TIM2->CNT = 0;
        break;
    case 3: /* 定时器3编码器：右轮 */
        Encoder_TIM = (short)TIM3->CNT;
        TIM3->CNT = 0;
        break;
    default:
        Encoder_TIM = 0;
    }
	//由于电机安装是镜像的，差180度，cnt的数据一个正一个负，后续处理要统一方向
    return Encoder_TIM;
}
