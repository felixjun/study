#include "Dri_TIM.h"

void Driver_TIM4_Init(void)
{
    /* 1.  开启时钟*/
    /* 1.1 定时器4的时钟 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    /* 1.2 GPIO的时钟 PB */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    /* 2. 设置GPIO的复用推挽输出 PB8 CNF=10 MODE=11*/
	GPIOB->CRH|=GPIO_CRH_MODE8;
    GPIOB->CRH|=GPIO_CRH_CNF8_1;
	GPIOB->CRH&=~GPIO_CRH_CNF8_0;
	
	GPIOB->CRH|=GPIO_CRH_MODE9;
    GPIOB->CRH|=GPIO_CRH_CNF9_1;
	GPIOB->CRH&=~GPIO_CRH_CNF9_0;


    /* 3. 定时器配置 */
    /* 3.1 预分频器的配置 pwm输出频率=72M时钟频率/(分频*重载值) */
    TIM4->PSC = 1 - 1;
    /* 3.2 自动重装载寄存器的配置 */
    TIM4->ARR = 7200 - 1;
    /* 3.3 计数器的计数方向 0=向上 1=向下*/
    TIM4->CR1 &= ~TIM_CR1_DIR;
	
	/*配置pwm*/
    /* 3.4 配置通道3的捕获比较寄存器,pb8在通道3 */
    TIM4->CCR3 = 0;
    /* 3.5 CCMR2才有通道3，4，把通道3配置为输出  CCMR2_CC3S=00 */
    TIM4->CCMR2 &= ~TIM_CCMR2_CC3S;
    /* 3.6 配置通道的输出比较模式，pwm模式1，CCMR2_OC3M=110*/
    TIM4->CCMR2 |= TIM_CCMR2_OC3M_2;
    TIM4->CCMR2 |= TIM_CCMR2_OC3M_1;
    TIM4->CCMR2 &= ~TIM_CCMR2_OC3M_0;
    /* 3.7 使能通道3 CCER_CC3E=1 */
    TIM4->CCER |= TIM_CCER_CC3E;
	/* 3.8 设置通道的极性 0=高电平有效  1=低电平有效 */
    TIM4->CCER &= ~TIM_CCER_CC3P; 
	
	
	/* 3.4 配置通道4的捕获比较寄存器 */
    TIM4->CCR4 = 0;
    /* 3.5 把通道4配置为输出  CCMR2_CC4S=00 */
    TIM4->CCMR2 &= ~TIM_CCMR2_CC4S;
    /* 3.6 配置通道的输出比较模式 CCMR2_OC4M=110*/
    TIM4->CCMR2 |= TIM_CCMR2_OC4M_2;
    TIM4->CCMR2 |= TIM_CCMR2_OC4M_1;
    TIM4->CCMR2 &= ~TIM_CCMR2_OC4M_0;
    /* 3.7 使能通道4 CCER_CC4E=1 */
    TIM4->CCER |= TIM_CCER_CC4E;
	/* 3.8 设置通道的极性 0=高电平有效  1=低电平有效 */
    TIM4->CCER &= ~TIM_CCER_CC4P; 
    
	/* 使能计数器 */
    TIM4->CR1 |= TIM_CR1_CEN;

}
void Dri_TIM2_Init(void)
{
	//开启定时器2时钟
	RCC->APB1ENR |=RCC_APB1ENR_TIM2EN;
	//开启GPIO时钟.AFIO时钟---重映射
	RCC->APB2ENR |=(RCC_APB2ENR_IOPAEN |RCC_APB2ENR_IOPBEN |RCC_APB2ENR_AFIOEN);
	
	//GPIO重映射
	//关闭JTAG，保留SWD开启,SWJ_CFG=010
	AFIO->MAPR &=~ AFIO_MAPR_SWJ_CFG_2;
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_1;
	AFIO->MAPR &=~ AFIO_MAPR_SWJ_CFG_0;
	//pa15--TIM2-CH1.PB3---TIM2-CH2,TIM2_REMAP=11
	AFIO->MAPR |=AFIO_MAPR_TIM2_REMAP_FULLREMAP;
	
	//初始化引脚 浮空输入 mode=00，cnf=01
	
	GPIOA->CRH&=~GPIO_CRH_MODE15;
    GPIOA->CRH&=~GPIO_CRH_CNF15_1;
	GPIOA->CRH|=GPIO_CRH_CNF15_0;
	
	
	GPIOB->CRL&=~GPIO_CRL_MODE3;
    GPIOB->CRL&=~GPIO_CRL_CNF3_1;
	GPIOB->CRL|=GPIO_CRL_CNF3_0;
	
	TIM4->PSC = 1 - 1;//预分频
	TIM4->ARR = 65536 - 1;//自动重装载
	
	
	 /* 关于编码器模式的配置 */
    /* 编码器3模式，SMS=011 */
    TIM2 -> SMCR |= TIM_SMCR_SMS;
    TIM2 -> SMCR &= ~TIM_SMCR_SMS_2;
    /* CC1S\CC2S = 01 */
    TIM2 -> CCMR1 |= (TIM_CCMR1_CC1S_0|TIM_CCMR1_CC2S_0);
    TIM2 -> CCMR1 &= ~(TIM_CCMR1_CC1S_1|TIM_CCMR1_CC2S_1);
    /* CC1P\CC2P = 0 */
    TIM2 -> CCER &= ~(TIM_CCER_CC1P|TIM_CCER_CC2P);

    /* 使能定时器的计数器 */
    TIM2 ->CR1 |= TIM_CR1_CEN;

}
/**
 * @description: TIM3编码器模式：B2A、B2B
 * @return {*}
 */
void Dri_TIM3_Init(void)
{

    /* 1.1 定时器2的时钟 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    /* 1.2 GPIO的时钟 PA\PB */
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN|RCC_APB2ENR_AFIOEN);

    /* 关闭JTAG，但是启用SWD（SWJ_CFG=010） */
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_1;
    AFIO->MAPR &= ~(AFIO_MAPR_SWJ_CFG_2 | AFIO_MAPR_SWJ_CFG_0);
    /* 重映射引脚 TIM3_REMAP=10(PB4=TIM3-CH1,PB5=TIM3-CH2) */
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_1;
    AFIO->MAPR &= ~AFIO_MAPR_TIM3_REMAP_0;

    /* 初始化引脚，浮空输入（mode=00，cnf=01） */
    GPIOB->CRL &= ~GPIO_CRL_MODE4;
    GPIOB->CRL &= ~GPIO_CRL_CNF4_1;
    GPIOB->CRL |= GPIO_CRL_CNF4_0;

    GPIOB->CRL &= ~GPIO_CRL_MODE5;
    GPIOB->CRL &= ~GPIO_CRL_CNF5_1;
    GPIOB->CRL |= GPIO_CRL_CNF5_0;

    /* 定时器配置基本配置 */
    /* 3.1 预分频器的配置 */
    TIM3->PSC = 1 - 1;
    /* 3.2 自动重装载寄存器的配置，设为最大值 */
    TIM3->ARR = 0xFFFF;

    /* 关于编码器模式的配置 */
    /* 编码器3模式，SMS=011 */
    TIM3 -> SMCR |= TIM_SMCR_SMS;
    TIM3 -> SMCR &= ~TIM_SMCR_SMS_2;
    /* CC1S\CC2S = 01 */
    TIM3 -> CCMR1 |= (TIM_CCMR1_CC1S_0|TIM_CCMR1_CC2S_0);
    TIM3 -> CCMR1 &= ~(TIM_CCMR1_CC1S_1|TIM_CCMR1_CC2S_1);
    /* CC1P\CC2P = 0 */
    TIM3 -> CCER &= ~(TIM_CCER_CC1P|TIM_CCER_CC2P);

    /* 使能定时器的计数器 */
    TIM3 ->CR1 |= TIM_CR1_CEN;

}
