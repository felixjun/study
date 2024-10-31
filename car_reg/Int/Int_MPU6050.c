#include "Int_MPU6050.h"

//读取寄存器一个字节
uint8_t Int_MPU6050_ReadByte(uint8_t reg_addr,uint8_t *receive_byte)
{
	/* 1、 发送起始信号 */
    Driver_I2C2_Start();
    /* 2、 发送设备写地址(bit0=0) */
    Driver_I2C_SendAddr(MPU_IIC_ADDR << 1);
    /* 3、 发送内部寄存器地址 */
    Driver_I2C_SendByte(reg_addr);
    /* 4、 重新发送起始信号 */
    Driver_I2C2_Start();
    /* 5、 发送设备读地址（bit0=1） */
    Driver_I2C_SendAddr(MPU_IIC_ADDR << 1 | 0x01);
    /* 6、发送非应答、停止信号（最好先做） */
    Driver_I2C2_NAck();
    Driver_I2C2_Stop();

    /* 7、 读取1字节数据 */
    *receive_byte = Driver_I2C_ReadByte();

    return 0;

}

/**
 * @description: 向MPU6050读多个字节
 * @param {uint8_t} reg_addr  内部寄存器的地址
 * @param {uint8_t} data      接收读取的多字节数据
 * @param {uint8_t} size      要接收的字节数
 * @return {*}
 */
uint8_t Int_MPU6050_ReadBytes(uint8_t reg_addr, uint8_t *data, uint8_t size)
{
    /* 1、 发送起始信号 */
    Driver_I2C2_Start();

    /* 2、 发送设备写地址 */
    Driver_I2C_SendAddr(MPU_IIC_ADDR << 1);
    /* 3、 发送内部寄存器地址 */
    Driver_I2C_SendByte(reg_addr);

    /* 4、 重新发送起始信号 */
    Driver_I2C2_Start();

    /* 5、 发送设备读地址 */
    Driver_I2C_SendAddr(MPU_IIC_ADDR << 1 | 0x01);
    /* 6、 循环接收多字节 */
    for (uint8_t i = 0; i < size; i++)
    {
        if (i == size - 1)
        {
            /* 7、 发送非应答、停止信号（建议最后一个字节前做） */
            Driver_I2C2_NAck();
            Driver_I2C2_Stop();
        }
        else
        {
            Driver_I2C2_Ack();
        }
        data[i] = Driver_I2C_ReadByte();
    }
	return 0;
}

uint8_t Int_MPU6050_WriteByte(uint8_t reg_addr, uint8_t byte)
{
    /* 1. 起始信号 */
    Driver_I2C2_Start();
    /* 2. 发送设备的写地址(bit0=0) */
    Driver_I2C_SendAddr(MPU_IIC_ADDR << 1);
    /* 3. 发送内部寄存器地址 */
    Driver_I2C_SendByte(reg_addr);
    /* 4. 发送1字节数据 */
    Driver_I2C_SendByte(byte);
    /* 5. 发送停止信号 */
    Driver_I2C2_Stop();

    return 0;
}
/**
 * @description: 向MPU6050写多个字节
 * @param {uint8_t} dev_addr  设备地址
 * @param {uint8_t} reg_addr  内部寄存器的地址
 * @param {uint8_t} data      要发送的多字节数据
 * @param {uint8_t} size      要发送的字节数
 * @return {*}
 */
uint8_t Int_MPU6050_WriteBytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t size)
{
    /* 1、 发送起始信号 */
    Driver_I2C2_Start();
    /* 2、 发送设备写地址 */
    Driver_I2C_SendAddr(MPU_IIC_ADDR << 1);
    /* 3、 发送内部寄存器地址 */
    Driver_I2C_SendByte(reg_addr);
    /* 4、 循环发送多字节数据 */
    for (uint8_t i = 0; i < size; i++)
    {
        Driver_I2C_SendByte(data[i]);
    }

    /* 5、 发送停止信号*/
    Driver_I2C2_Stop();
	return 0;
	
}
//根据采样率，设置低通滤波器
void Int_MPU6050_Set_DLPF_CFG(uint16_t rate)
{
	uint8_t cfg=0;
	if(rate/2 >188)
	{
		cfg=1;
	}
	else if(rate/2 >98)
	{
		cfg=2;
	}
	else if(rate/2 >42)
	{
		cfg=3;
	}
	else if(rate/2 >20)
	{
		cfg=4;
	}
	else if(rate/2 >10)
	{
		cfg=5;
	}
	else 
	{
		cfg=6;
	}
	Int_MPU6050_WriteByte(0x1A,cfg<<0);
}
void Int_MPU6050_SetGyroRate(uint16_t rate)
{
	//采样率=输出速率fs（固定值1k）/（1+分频值）
	uint8_t sample_div=0;
	if(rate<4)
	{
		rate=4;
	}
	else if(rate>1000)
	{
		rate=1000;
	}
	sample_div=1000/rate-1;
	Int_MPU6050_WriteByte(0x19,sample_div);
	//采样率>=2*带宽,才不会失真
	//设置低通滤波器
	Int_MPU6050_Set_DLPF_CFG(rate);
}


void Int_MPU6050_Init(void)
{
	uint8_t dev_id;
	//初始化I2C
	Driver_I2C2_Init();
	for_delay_ms(100);
	//复位(默认睡眠)-》延迟一会->唤醒
	Int_MPU6050_WriteByte(0x6B,0x00);
	//陀螺仪量程
	Int_MPU6050_WriteByte(MPU_GYRO_CFG_REG,3<<3);
	//加速度量程
	Int_MPU6050_WriteByte(MPU_ACCEL_CFG_REG,0<<3);
	//其他功能设置（可选），FIFO，第二IIC，中断
	//Int_MPU6050_WriteByte(MPU_INT_EN_REG,0x00);//关闭所有中断
	//Int_MPU6050_WriteByte(0x6A,0x00);//关闭第二IIC
	//Int_MPU6050_WriteByte(0x23,0x00);//关闭FIFO 
	
	//系统时钟源，陀螺仪采样率，低通滤波设置
	Int_MPU6050_ReadByte(MPU_DEVICE_ID_REG,&dev_id);//配置时钟源之前确认正常工作
	if(dev_id==MPU_IIC_ADDR)
	{
		//设置时钟源，陀螺仪x轴时钟
		Int_MPU6050_WriteByte(0x6B,0x01);
		//设置陀螺仪采样率和低通滤波
		Int_MPU6050_SetGyroRate(100);
		//让MPU6050里面两个传感器退出待机模式，进入正常工作状态
		Int_MPU6050_WriteByte(0x6C,0x00);
	}
}

void Int_MPU6050_GetAccl(short *ax, short *ay, short *az)
{
    uint8_t acc_buff[6];
    /* 每个轴对应2个寄存器：高8位、低8位， 第一个是0x3B（X轴的高位） */
    Int_MPU6050_ReadBytes(MPU_ACCEL_XOUTH_REG, acc_buff, 6);
    /*
        acc_buff[0]:X轴加速度的高8位，
        acc_buff[1]:X轴加速度的低8位，
        acc_buff[2]:Y轴加速度的高8位，
        acc_buff[3]:Y轴加速度的低8位，
        acc_buff[4]:Z轴加速度的高8位，
        acc_buff[5]:Z轴加速度的低8位，
     */
    *ax = ((short)acc_buff[0] << 8) | acc_buff[1];
    *ay = ((short)acc_buff[2] << 8) | acc_buff[3];
    *az = ((short)acc_buff[4] << 8) | acc_buff[5];

}

/**
 * @description: 获取陀螺仪的角速度原始数据
 * @param {short} *gx
 * @param {short} *gy
 * @param {short} *gz
 * @return {*}
 */
void Int_MPU6050_GetGyro(short *gx, short *gy, short *gz)
{
    uint8_t gyro_buff[6];
    /* 每个轴对应2个寄存器：高8位、低8位， 第一个是0x43（X轴的高位） */
    Int_MPU6050_ReadBytes(MPU_GYRO_XOUTH_REG, gyro_buff, 6);
    /*
        gyro_buff[0]:X轴角速度的高8位，
        gyro_buff[1]:X轴角速度的低8位，
        gyro_buff[2]:Y轴角速度的高8位，
        gyro_buff[3]:Y轴角速度的低8位，
        gyro_buff[4]:Z轴角速度的高8位，
        gyro_buff[5]:Z轴角速度的低8位，
     */
    *gx = ((short)gyro_buff[0] << 8) | gyro_buff[1];
    *gy = ((short)gyro_buff[2] << 8) | gyro_buff[3];
    *gz = ((short)gyro_buff[4] << 8) | gyro_buff[5];

}



