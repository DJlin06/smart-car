/*
 * icm42688.c
 *
 *  Created on: 2024年8月2日
 *      Author: ljk
 */
/*MIT License
*
*Copyright (c) 2024 ljk
*
*Permission is hereby granted, free of charge, to any person obtaining a copy
*of this software and associated documentation files (the "Software"), to deal
*in the Software without restriction, including without limitation the rights
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*copies of the Software, and to permit persons to whom the Software is
*furnished to do so, subject to the following conditions:
*
*The above copyright notice and this permission notice shall be included in all
*copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*SOFTWARE.
*/
#include "icm42688.h"
ICM42688_CONFIG_STRUCT ICM42688_CONFIG={GYRO_250DPS,GYRO_ODR_100HZ,ACC_8G,ACC_ODR_500HZ,_2st,Bandwidth_Factor_4,_2st,Bandwidth_Factor_4,Bias_On_Chip_Off};
float Gyro_Sensitivity,Acc_Sensitivity;
ICM42688_RAW_DATA ICM42688_RAW;
ICM42688_real_data ICM42688;
int32 bias_gyro_x,bias_gyro_y,bias_gyro_z;
static void ICM42688_SPI_HardWare_Init(void)
{
    spi_init(ICM42688_SPI,SPI_MODE0,ICM42688_SPEED,ICM42688_SCK_Pin,ICM42688_MOSI_Pin,ICM42688_MISO_Pin,ICM42688_CS_Pin);  //SPI初始化a                                                        //片选引脚初始化
}

static uint16_t SPI_Transfer_16bit(uint16_t dout)
{
    uint16_t return_data;                       //定义返回值
    spi_transfer_16bit(ICM42688_SPI,&dout,&return_data,1);  //三次16位SPI通信
    return return_data;                         //返回通信结果
}

//读取芯片ID
static void Find_ICM42688(void)
{
    SPI_Transfer_16bit(WHO_AM_I);
    uint8 ID = (uint8)SPI_Transfer_16bit(WHO_AM_I);
    if(ID!=ICM42688_ID)
    {
        printf("ICM42688 not recognized\n");
        while(1);
    }
}
//设置陀螺仪量程与采样率
static void ICM42688_SET_GYRO(GYRO_FSR gyro_fsr,GYRO_ODR gyro_odr)
{
    uint8 WRITE_GYRO_CONFIG0 = 0x4F;
    uint8 GYRO_CONFIG0 = 0x00;
    uint8 GYRO_CONFIG_Verify = 0x00;
    uint16 READ_GYRO_CONFIG0 = 0XCF00;
    switch(gyro_fsr)
        {
            case GYRO_15_625DPS:
                GYRO_CONFIG0|=0xE0;
                Gyro_Sensitivity = SENSITIVITY_ICM42688_GYRO_15_625dps;
                break;
            case GYRO_31_25DPS:
                GYRO_CONFIG0|=0xC0;
                Gyro_Sensitivity = SENSITIVITY_ICM42688_GYRO_31_25dps;
                break;
            case GYRO_62_5DPS:
                GYRO_CONFIG0|=0xA0;
                Gyro_Sensitivity = SENSITIVITY_ICM42688_GYRO_62_5dps;
                break;
            case GYRO_125DPS:
                GYRO_CONFIG0|=0x80;
                Gyro_Sensitivity = SENSITIVITY_ICM42688_GYRO_125dps;
                break;
            case GYRO_250DPS:
                GYRO_CONFIG0|=0x60;
                Gyro_Sensitivity = SENSITIVITY_ICM42688_GYRO_250dps;
                break;
            case GYRO_500DPS:
                GYRO_CONFIG0|=0x40;
                Gyro_Sensitivity = SENSITIVITY_ICM42688_GYRO_500dps;
                break;
            case GYRO_1000DPS:
                GYRO_CONFIG0|=0x20;
                Gyro_Sensitivity = SENSITIVITY_ICM42688_GYRO_1000dps;
                break;
            case GYRO_2000DPS:
                GYRO_CONFIG0|=0x00;
                Gyro_Sensitivity = SENSITIVITY_ICM42688_GYRO_2000dps;
                break;
        }
    switch(gyro_odr)
        {
            case GYRO_ODR_12_5HZ:
                GYRO_CONFIG0|=0x0B;
                break;
            case GYRO_ODR_25HZ:
                GYRO_CONFIG0|=0x0A;
                break;
            case GYRO_ODR_50HZ:
                GYRO_CONFIG0|=0x09;
                break;
            case GYRO_ODR_100HZ:
                GYRO_CONFIG0|=0x08;
                break;
            case GYRO_ODR_200HZ:
                GYRO_CONFIG0|=0x07;
                break;
            case GYRO_ODR_500HZ:
                GYRO_CONFIG0|=0x0F;
                break;
            case GYRO_ODR_1000HZ:
                GYRO_CONFIG0|=0x06;
                break;
            case GYRO_ODR_2000HZ:
                GYRO_CONFIG0|=0x05;
                break;
            case GYRO_ODR_4000HZ:
                GYRO_CONFIG0|=0x04;
                break;
            case GYRO_ODR_8000HZ:
                GYRO_CONFIG0|=0x03;
                break;
            case GYRO_ODR_16000HZ:
                GYRO_CONFIG0|=0x02;
                break;
            case GYRO_ODR_32000HZ:
                GYRO_CONFIG0|=0x01;
                break;
            }
    SPI_Transfer_16bit(((uint16)WRITE_GYRO_CONFIG0<<8|GYRO_CONFIG0));
    GYRO_CONFIG_Verify = (uint8)SPI_Transfer_16bit(READ_GYRO_CONFIG0);
    if(GYRO_CONFIG_Verify!=GYRO_CONFIG0)
    {
        printf("Gyroscope set failed!\n");
        while(1);
    }
}
 //设置加速度计量程与采样率
static void ICM42688_SET_ACC(ACC_FSR acc_fsr,ACC_ODR acc_odr)
{
    uint8 WRITE_ACCEL_CONFIG0 = 0x50;
    uint8 ACCEL_CONFIG0 = 0x00;
    uint8 ACCEL_CONFIG_Verify = 0x00;
    uint16 READ_ACCEL_CONFIG0 = 0XD000;
    switch(acc_fsr)
        {
            case ACC_2G:
                ACCEL_CONFIG0|=0x60;
                Acc_Sensitivity = SENSITIVITY_ICM42688_ACC_2G;
                break;
            case ACC_4G:
                ACCEL_CONFIG0|=0x40;
                Acc_Sensitivity = SENSITIVITY_ICM42688_ACC_4G;
                break;
            case ACC_8G:
                ACCEL_CONFIG0|=0x20;
                Acc_Sensitivity = SENSITIVITY_ICM42688_ACC_8G;
                break;
            case ACC_16G:
                ACCEL_CONFIG0|=0x00;
                Acc_Sensitivity = SENSITIVITY_ICM42688_ACC_16G;
                break;
        }
    switch(acc_odr)
        {
            case ACC_ODR_12_5HZ:
                ACCEL_CONFIG0|=0x0B;
                break;
            case ACC_ODR_25HZ:
                ACCEL_CONFIG0|=0x0A;
                break;
            case ACC_ODR_50HZ:
                ACCEL_CONFIG0|=0x09;
                break;
            case ACC_ODR_100HZ:
                ACCEL_CONFIG0|=0x08;
                break;
            case ACC_ODR_200HZ:
                ACCEL_CONFIG0|=0x07;
                break;
            case ACC_ODR_500HZ:
                ACCEL_CONFIG0|=0x0F;
                break;
            case ACC_ODR_1000HZ:
                ACCEL_CONFIG0|=0x06;
                break;
            case ACC_ODR_2000HZ:
                ACCEL_CONFIG0|=0x05;
                break;
            case ACC_ODR_4000HZ:
                ACCEL_CONFIG0|=0x04;
                break;
            case ACC_ODR_8000HZ:
                ACCEL_CONFIG0|=0x03;
                break;
            case ACC_ODR_16000HZ:
                ACCEL_CONFIG0|=0x02;
                break;
            case ACC_ODR_32000HZ:
                ACCEL_CONFIG0|=0x01;
                break;
            }
    SPI_Transfer_16bit(((uint16)WRITE_ACCEL_CONFIG0<<8|ACCEL_CONFIG0));
    ACCEL_CONFIG_Verify = (uint8)SPI_Transfer_16bit(READ_ACCEL_CONFIG0);
    if(ACCEL_CONFIG_Verify!=ACCEL_CONFIG0)
    {
        printf("Accelerometer set failed!\n");
        while(1);
    }
}

//设置内置滤波器
static void ICM42688_SET_FILTER(Bandwidth_Factor Gyro_Bandwidth_Factor,Filter_Order Gyro_Filter_Order,Bandwidth_Factor Acc_Bandwidth_Factor,Filter_Order Acc_Filter_Order)
{
    uint8 WRITE_GYRO_ACCEL_CONFIG0=0X52;
    uint8 WRITE_GYRO_CONFIG1=0X51;
    uint8 WRITE_ACCEL_CONFIG1=0X53;
    uint8 Gyro_Filter_Ord_Config = 0x00;
    uint8 Acc_Filter_Ord_Config = 0x00;
    uint8 Filter_Config=0x00;
    switch(Gyro_Bandwidth_Factor)
    {
        case Bandwidth_Factor_2:
           Filter_Config|=0x00;
            break;
        case Bandwidth_Factor_4:
           Filter_Config|=0x01;
            break;
        case Bandwidth_Factor_5:
           Filter_Config|=0x02;
            break;
        case Bandwidth_Factor_8:
           Filter_Config|=0x03;
            break;
        case Bandwidth_Factor_10:
           Filter_Config|=0x04;
            break;
        case Bandwidth_Factor_16:
           Filter_Config|=0x05;
            break;
        case Bandwidth_Factor_20:
           Filter_Config|=0x06;
            break;
        case Bandwidth_Factor_40:
           Filter_Config|=0x07;
            break;
        case Low_latency_1:
           Filter_Config|=0x0E;
            break;
        case Low_Latency_2:
           Filter_Config|=0x0F;
            break;
    }
    switch(Acc_Bandwidth_Factor)
    {
        case Bandwidth_Factor_2:
           Filter_Config|=0x00;
            break;
        case Bandwidth_Factor_4:
           Filter_Config|=0x10;
            break;
        case Bandwidth_Factor_5:
           Filter_Config|=0x20;
            break;
        case Bandwidth_Factor_8:
           Filter_Config|=0x30;
            break;
        case Bandwidth_Factor_10:
           Filter_Config|=0x40;
            break;
        case Bandwidth_Factor_16:
           Filter_Config|=0x50;
            break;
        case Bandwidth_Factor_20:
           Filter_Config|=0x60;
            break;
        case Bandwidth_Factor_40:
           Filter_Config|=0x70;
            break;
        case Low_latency_1:
           Filter_Config|=0xE0;
            break;
        case Low_Latency_2:
           Filter_Config|=0xF0;
            break;
    }
    SPI_Transfer_16bit(((uint16)WRITE_GYRO_ACCEL_CONFIG0<<8|Filter_Config));
    switch(Gyro_Filter_Order)
    {
        case _1st:
            Gyro_Filter_Ord_Config|=0x02;
            break;
        case _2st:
            Gyro_Filter_Ord_Config|=0x06;
            break;
        case _3st:
            Gyro_Filter_Ord_Config|=0xA0;
            break;
    }
    SPI_Transfer_16bit(((uint16)WRITE_GYRO_CONFIG1<<8|Gyro_Filter_Ord_Config));
    switch(Acc_Filter_Order)
    {
        case _1st:
            Acc_Filter_Ord_Config|=0x02;
            break;
        case _2st:
            Acc_Filter_Ord_Config|=0x06;
            break;
        case _3st:
            Acc_Filter_Ord_Config|=0xA0;
            break;
    }
    SPI_Transfer_16bit(((uint16)WRITE_ACCEL_CONFIG1<<8|Acc_Filter_Ord_Config));
}

//传感器复位
static void Rest_ICM42688(void)
{
    uint8 WRITE_PWR_MGMT0 = 0X4E;
    SPI_Transfer_16bit(((uint16)WRITE_PWR_MGMT0<<8|0X00));
}

//启动陀螺仪与加速度计并进入低噪声模式
static void Set_ICM42688_LN_Mode(void)
{
    uint8 WRITE_PWR_MGMT0 = 0X4E;
    SPI_Transfer_16bit(((uint16)WRITE_PWR_MGMT0<<8|0X0F));
}

//ICM42688回调函数 连续8次访问数据寄存器 ICM42688可自增访问地址
void ICM42688_Get_Data(void)
{
    uint8_t Add[13];
    uint8 LSB_Data[13];
    Add[0] = READ_ACC_X_HIGH;
    spi_transfer_8bit(ICM42688_SPI,Add,LSB_Data,13);  //依次读取数据寄存器
    ICM42688_RAW.acc_x_lsb = (int16)(((uint16)LSB_Data[1]<<8 |LSB_Data[2]));
    ICM42688_RAW.acc_y_lsb = (int16)(((uint16)LSB_Data[3]<<8 |LSB_Data[4]));
    ICM42688_RAW.acc_z_lsb = (int16)(((uint16)LSB_Data[5]<<8 |LSB_Data[6]));
    ICM42688_RAW.gyro_x_lsb = (int16)(((uint16)LSB_Data[7]<<8 |LSB_Data[8]));
    ICM42688_RAW.gyro_y_lsb = (int16)(((uint16)LSB_Data[9]<<8 |LSB_Data[10]));
    ICM42688_RAW.gyro_z_lsb = (int16)(((uint16)LSB_Data[11]<<8 |LSB_Data[12]));
    ICM42688.gyro_x = ICM42688_RAW.gyro_x_lsb/Gyro_Sensitivity;
    ICM42688.gyro_y = ICM42688_RAW.gyro_y_lsb/Gyro_Sensitivity;
    ICM42688.gyro_z = ICM42688_RAW.gyro_z_lsb/Gyro_Sensitivity;
    ICM42688.acc_x = ICM42688_RAW.acc_x_lsb/Acc_Sensitivity;
    ICM42688.acc_y = ICM42688_RAW.acc_y_lsb/Acc_Sensitivity;
    ICM42688.acc_z = ICM42688_RAW.acc_z_lsb/Acc_Sensitivity;
}

////求Bias平均数并且填入相关寄存器 传感器会自动减去Bias 参数为采样次数 暂时弃用
////此处采用较为简单的求平均数方法求解Bias 也可以用一些状态观测等更好的办法替代！
//void static Remove_Bias_On_Chip(uint16 times)
//{
//    uint8 WRITE_OFFSET_USER0 = 0X77;//X轴陀螺仪Bias低八位
//    uint8 WRITE_OFFSET_USER1 = 0X78;//X与Y轴陀螺仪Bias高四位
//    uint8 WRITE_OFFSET_USER2 = 0X79;//Y轴陀螺仪Bias低八位
//    uint8 WRITE_OFFSET_USER3 = 0X7A;//Z轴陀螺仪Bias低八位
//    uint8 WRITE_OFFSET_USER4 = 0X7B;//Z轴陀螺仪Bias高四位
//    for(uint16 i=0;i<times;i++)
//    {
//        system_delay_us(10);
//        ICM42688_Get_Data();
//        bias_gyro_x+=ICM42688_RAW.gyro_x_lsb;
//        bias_gyro_y+=ICM42688_RAW.gyro_y_lsb;
//        bias_gyro_z+=ICM42688_RAW.gyro_z_lsb;
//    }
//    bias_gyro_x=bias_gyro_x/times;
//    bias_gyro_y=bias_gyro_y/times;
//    bias_gyro_z=bias_gyro_z/times;
//    //printf("%d %d %d\n",bias_gyro_x,bias_gyro_y,bias_gyro_z);
//    //while(1);
//    bias_gyro_x = 0xABCD;
//    //SPI_Transfer_16bit(((uint16)WRITE_OFFSET_USER0<<8|((uint8)(bias_gyro_x>>4))));
//    //SPI_Transfer_16bit(((uint16)WRITE_OFFSET_USER0<<8|0X44));
////    SPI_Transfer_16bit(((uint16)WRITE_OFFSET_USER1<<8|(uint8)(((bias_gyro_y&=0XF000)|((bias_gyro_x>>4)&0X0F00))>>8)));
////    SPI_Transfer_16bit(((uint16)WRITE_OFFSET_USER2<<8|((uint8)bias_gyro_y>>4)));
////    SPI_Transfer_16bit(((uint16)WRITE_OFFSET_USER3<<8|((uint8)bias_gyro_z>>4)));
////    SPI_Transfer_16bit(((uint16)WRITE_OFFSET_USER4<<8|(uint8)((0x0000|((bias_gyro_z>>4)&0X0F00))>>8)));
//    uint16 test = SPI_Transfer_16bit(0XF700);
//    uint16 test2 = SPI_Transfer_16bit(0XF800);
//    printf("%X %X\n",test,test2);
//}

void icm42688_init(ICM42688_CONFIG_STRUCT*ICM42688_CONFIG)
{
    ICM42688_SPI_HardWare_Init();//单片机SPI相关外设初始化
    Rest_ICM42688();             //复位ICM42688
    system_delay_ms(10);
    Find_ICM42688();             //读取芯片ID
    ICM42688_SET_GYRO((*ICM42688_CONFIG).GYRO_FSR,(*ICM42688_CONFIG).GYRO_ODR);          //设置陀螺仪量程与采样率
    ICM42688_SET_ACC((*ICM42688_CONFIG).ACC_FSR,(*ICM42688_CONFIG).ACC_ODR);             //设置加速度计量程与采样率
    ICM42688_SET_FILTER((*ICM42688_CONFIG).Gyro_Bandwidth_Factor,(*ICM42688_CONFIG).Gyro_Filter_Order,(*ICM42688_CONFIG).Acc_Bandwidth_Factor,(*ICM42688_CONFIG).Acc_Filter_Order);      //设置滤波器阶数与带宽系数
    system_delay_ms(10);
    Set_ICM42688_LN_Mode();     //启动陀螺仪与加速度计，并进入低噪声模式
    system_delay_ms(10);
}


