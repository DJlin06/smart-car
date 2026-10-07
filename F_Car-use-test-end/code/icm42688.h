/*
 * icm42688.h
 *
 *  Created on: 2024年8月2日
 *      Author: ljk
 *      mail:983688746@qq.com
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
#ifndef CODE_ICM42688_H_
#define CODE_ICM42688_H_
#include "zf_common_typedef.h"
#include "zf_driver_delay.h"
#include "zf_driver_gpio.h"
#include "zf_driver_spi.h"
#define   WHO_AM_I  0xF500
#define ICM42688_SPI          (SPI_0)                                                           //SPI模块号定义
#define ICM42688_MOSI_Pin     (SPI0_MOSI_P20_14)                                                //MOSI引脚定义
#define ICM42688_MISO_Pin     (SPI0_MISO_P20_12)                                                //MISO引脚定义
#define ICM42688_CS_Pin       (SPI0_CS2_P20_13)                                                 //CS片选引脚定义
#define ICM42688_SCK_Pin      (SPI0_SCLK_P20_11)                                                //SCLK引脚定义
#define ICM42688_SPEED        (10 * 1000 * 1000)                                                //SPI时钟速率 此处为10M

#define ICM42688_ID           0X47
#define READ_ACC_X_HIGH       0X9F

#define SENSITIVITY_ICM42688_GYRO_15_625dps  2097.2
#define SENSITIVITY_ICM42688_GYRO_31_25dps   1048.6
#define SENSITIVITY_ICM42688_GYRO_62_5dps    524.3
#define SENSITIVITY_ICM42688_GYRO_125dps     262.0
#define SENSITIVITY_ICM42688_GYRO_250dps     131.0
#define SENSITIVITY_ICM42688_GYRO_500dps     65.5
#define SENSITIVITY_ICM42688_GYRO_1000dps    32.8
#define SENSITIVITY_ICM42688_GYRO_2000dps    16.4

#define SENSITIVITY_ICM42688_ACC_16G  2048
#define SENSITIVITY_ICM42688_ACC_8G   4096
#define SENSITIVITY_ICM42688_ACC_4G   8192
#define SENSITIVITY_ICM42688_ACC_2G   16384
typedef struct ICM42688_RAW_DATA{

    int16 acc_x_lsb;
    int16 acc_y_lsb;
    int16 acc_z_lsb;
    int16 gyro_x_lsb;
    int16 gyro_y_lsb;
    int16 gyro_z_lsb;
    int16 temp_lsb;//温度暂不读取
} ICM42688_RAW_DATA;//与传感器通信获取的LSB原始数据 还需要转化为有物理意义的数据

typedef struct ICM42688_real_data {

    float acc_x;
    float acc_y;
    float acc_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float temp;//温度暂不读取

} ICM42688_real_data;//有物理意义的真实数据

typedef enum        // 枚举陀螺仪采样率
{
    GYRO_ODR_12_5HZ,
    GYRO_ODR_25HZ,
    GYRO_ODR_50HZ,
    GYRO_ODR_100HZ,
    GYRO_ODR_200HZ,
    GYRO_ODR_500HZ,
    GYRO_ODR_1000HZ,
    GYRO_ODR_2000HZ,
    GYRO_ODR_4000HZ,
    GYRO_ODR_8000HZ,
    GYRO_ODR_16000HZ,
    GYRO_ODR_32000HZ,
}GYRO_ODR;          //ODR -->Output Data Rate

typedef enum        // 枚举加速度计采样率
{
    ACC_ODR_12_5HZ,
    ACC_ODR_25HZ,
    ACC_ODR_50HZ,
    ACC_ODR_100HZ,
    ACC_ODR_200HZ,
    ACC_ODR_500HZ,
    ACC_ODR_1000HZ,
    ACC_ODR_2000HZ,
    ACC_ODR_4000HZ,
    ACC_ODR_8000HZ,
    ACC_ODR_16000HZ,
    ACC_ODR_32000HZ,
}ACC_ODR;

typedef enum   //陀螺仪量程枚举  DPS --> Degree Per Second
{
    GYRO_2000DPS,
    GYRO_1000DPS,
    GYRO_500DPS,
    GYRO_250DPS,
    GYRO_125DPS,
    GYRO_62_5DPS,
    GYRO_31_25DPS,
    GYRO_15_625DPS,
}GYRO_FSR;      //FSR --> Full Scan Range

typedef enum   //加速度计量程枚举
{
    ACC_16G,
    ACC_8G,
    ACC_4G,
    ACC_2G,
}ACC_FSR;

typedef enum   //内部滤波器阶数枚举 若不设置 默认使用二阶
{
    _1st,      //一阶
    _2st,      //二阶
    _3st,      //三阶
}Filter_Order;

typedef enum   //带宽系数枚举  具体作用请阅读手册31 81页 最终的3dB带宽由 ODR(采样率) 滤波器阶数设置 带宽系数 共同决定 请参考手册31页往后的表格
{
    Bandwidth_Factor_2,
    Bandwidth_Factor_4,
    Bandwidth_Factor_5,
    Bandwidth_Factor_8,
    Bandwidth_Factor_10,
    Bandwidth_Factor_16,
    Bandwidth_Factor_20,
    Bandwidth_Factor_40,
    Low_latency_1,
    Low_Latency_2,
}Bandwidth_Factor;

typedef enum   //是否启用芯片内置的去零偏功能 若启用 初始化过程需保持静止 完成后零偏会装入相关寄存器 ICM42688会自动减去 功能暂时弃用
{
    Bias_On_Chip_On,
    Bias_On_Chip_Off,
}Bias_On_Chip;

typedef struct ICM42688_CONFIG_STRUCT
{
    GYRO_FSR GYRO_FSR;
    GYRO_ODR GYRO_ODR;
    ACC_FSR ACC_FSR;
    ACC_ODR ACC_ODR;
    Filter_Order Gyro_Filter_Order;
    Bandwidth_Factor Gyro_Bandwidth_Factor;
    Filter_Order Acc_Filter_Order;
    Bandwidth_Factor Acc_Bandwidth_Factor;
    Bias_On_Chip Bias_On_Chip;
}ICM42688_CONFIG_STRUCT;//ICM42688初始化结构体
extern ICM42688_CONFIG_STRUCT ICM42688_CONFIG;
void icm42688_init(ICM42688_CONFIG_STRUCT*ICM42688_CONFIG);
void ICM42688_Get_Data(void);
extern ICM42688_real_data ICM42688;
#endif /* CODE_ICM42688_H_ */
