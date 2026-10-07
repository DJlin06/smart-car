/*********************************************************************************************************************
* TC387 Opensourec Library 即（TC387 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC387 开源库的一部分
*
* TC387 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          zf_common_headfile
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.9.20
* 适用平台          TC387QP
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-11-04       pudding            first version
********************************************************************************************************************/

#ifndef _zf_common_headfile_h_
#define _zf_common_headfile_h_

//===================================================C语言 函数库===================================================
#include "math.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
//===================================================C语言 函数库===================================================

//===================================================芯片 SDK 底层===================================================
#include "ifxAsclin_reg.h"
#include "SysSe/Bsp/Bsp.h"
#include "IfxCcu6_Timer.h"
#include "IfxScuEru.h"
//===================================================芯片 SDK 底层===================================================

//====================================================开源库公共层====================================================
#include "zf_common_typedef.h"
#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_fifo.h"
#include "zf_common_font.h"
#include "zf_common_function.h"
#include "zf_common_interrupt.h"
#include "isr_config.h"
//====================================================开源库公共层====================================================

//===================================================芯片外设驱动层===================================================
#include "zf_driver_adc.h"
#include "zf_driver_delay.h"
#include "zf_driver_dma.h"
#include "zf_driver_encoder.h"
#include "zf_driver_exti.h"
#include "zf_driver_flash.h"
#include "zf_driver_gpio.h"
#include "zf_driver_pit.h"
#include "zf_driver_pwm.h"
#include "zf_driver_soft_iic.h"
#include "zf_driver_spi.h"
#include "zf_driver_soft_spi.h"
#include "zf_driver_uart.h"
#include "zf_driver_timer.h"
//===================================================芯片外设驱动层===================================================

//===================================================外接设备驱动层===================================================
#include "zf_device_absolute_encoder.h"
#include "zf_device_ble6a20.h"
#include "zf_device_bluetooth_ch9141.h"
#include "zf_device_gnss.h"
#include "zf_device_camera.h"
#include "zf_device_dl1a.h"
#include "zf_device_dl1b.h"
#include "zf_device_icm20602.h"
#include "zf_device_imu660ra.h"
#include "zf_device_imu963ra.h"
#include "zf_device_ips114.h"
#include "zf_device_ips200.h"
#include "zf_device_key.h"
#include "zf_device_mpu6050.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_oled.h"
#include "zf_device_ov7725.h"
#include "zf_device_scc8660.h"
#include "zf_device_tft180.h"
#include "zf_device_tsl1401.h"
#include "zf_device_type.h"
#include "zf_device_uart_receiver.h"
#include "zf_device_virtual_oscilloscope.h"
#include "zf_device_wifi_uart.h"
#include "zf_device_wifi_spi.h"
#include "zf_device_wireless_uart.h"
//===================================================外接设备驱动层===================================================

//====================================================应用组件层=====================================================
#include "seekfree_assistant.h"
#include "seekfree_assistant_interface.h"
//====================================================应用组件层=====================================================

//=====================================================用户层=======================================================
#include "images.h"
#include "pid.h"
#include "init.h"
#include "icm42688.h"
#include "Fuzzy_PID.h"
#include "island.h"
#include "Flash.h"
#include "UI.h"
//以下部分均在定义处注释过，因此在这里不重复解释
#define PIT0                            (CCU60_CH0 )                            // 使用的周期中断编号
#define PIT1                            (CCU61_CH0 )
#define PIT2                            (CCU60_CH1 )
#define PIT3                            (CCU61_CH1 )
#define ENCODER_DIR_R                     (TIM6_ENCODER)                         // 带方向编码器对应使用的编码器接口
#define ENCODER_DIR_PULSE_R               (TIM6_ENCODER_CH1_P20_3)               // PULSE 对应的引脚
#define ENCODER_DIR_DIR_R                (TIM6_ENCODER_CH2_P20_0)               // DIR 对应的引脚
#define ENCODER_DIR_L                     (TIM4_ENCODER)                         // 带方向编码器对应使用的编码器接口
#define ENCODER_DIR_PULSE_L               (TIM4_ENCODER_CH1_P02_8)               // PULSE 对应的引脚
#define ENCODER_DIR_DIR_L                (TIM4_ENCODER_CH2_P00_9)               // DIR 对应的引脚
#define PWM_1 ATOM0_CH0_P21_2//RB
#define PWM_2 ATOM0_CH1_P21_3//RF
#define PWM_3 ATOM0_CH2_P21_4//LF
#define PWM_4 ATOM0_CH3_P21_5//LB
#define FS_PWM_1 ATOM1_CH4_P02_4//RB
#define FS_PWM_2 ATOM1_CH6_P02_6//RB
#define Handao_PWM ATOM1_CH7_P02_7//RB

#define IO_MODE P20_7
#define IO_ADD P20_6
#define IO_SUB P11_2
#define IO_End P11_3
extern int32 error;
extern int j;
extern int nega_pressure;
extern int16 dat_R;                    //编码器输出
extern int16 dat_L;
extern int16 dat_A;                        //左右轮均值
//电机参数
extern int32 Speed;
extern int32 Real_Speed;
extern int16 motor_L;
extern int16 motor_R;
extern int16 motor;
//电机PID参数
extern float Velocity_KP;
extern float Velocity_KI;
extern float Velocity_KD;

extern float Bias_L;
extern float Pwm_motor_L;
extern float Last_bias_L;
extern float Inteal_Bias_L;

extern float Bias_R;
extern float Pwm_motor_R;
extern float Last_bias_R;
extern float Inteal_Bias_R;

extern float Chaspeed;
extern float Cha_KP;
extern float Cha_KI;
extern float Cha_KD;
extern float Bias_error;
extern float Inteal_Bias_error;
extern float Last_bias_error;
extern int SP_err;

extern uint8 image_two_value[MT9V03X_H][MT9V03X_W];//二值化后的原数组
extern uint8 image_two_original[MT9V03X_H][MT9V03X_W];   //原始灰度图像存放数组
extern uint8 Value_isp_Flag; //图像显示标志位，1为显示，0为不显示
extern uint8 Left_Line[MT9V03X_H]; //左边线数组
extern uint8 Right_Line[MT9V03X_H];//右边线数组
extern uint8 Mid_Line[MT9V03X_H];  //中线数组
extern uint8 Road_Wide[MT9V03X_H]; //赛宽数组
extern uint8 White_Column[MT9V03X_W];    //每列白列长度
extern uint8 Search_Stop_Line;     //搜索截止行,只记录长度，想要坐标需要用视野高度减去该值
extern uint8 Boundry_Start_Left;   //左右边界起始点
extern uint8 Boundry_Start_Right;  //第一个非丢线点,常规边界起始点
extern uint8 Left_Lost_Time;       //边界丢线数
extern uint8 Right_Lost_Time;
extern uint8 Both_Lost_Time;//两边同时丢线数
extern uint8 Longest_White_Column_Left[2]; //最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
extern uint8 Longest_White_Column_Right[2];//最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
extern uint8 Left_Lost_Flag[MT9V03X_H]; //左丢线数组，丢线置1，没丢线置0
extern uint8 Right_Lost_Flag[MT9V03X_H]; //右丢线数组，丢线置1，没丢线置0

//环岛
extern uint8 Island_State;     //环岛状态标志
extern uint8 Left_Island_Flag; //左右环岛标志
extern uint8 Right_Island_Flag;//左右环岛标志
extern int interrupt;
extern int island_state_5_down[2];//状态5时即将离开环岛，左右边界边最低点，[0]存y，第某行，{1}存x，第某列
extern int island_state_3_up[2];//状态3时即将进入环岛用，左右上面角点[0]存y，第某行，{1}存x，第某列
extern int left_down_guai[2];//四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
extern int right_down_guai[2];//四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
extern int monotonicity_change_line[2];//单调性改变点坐标，[0]寸某行，[1]寸某列
extern int monotonicity_change_left_flag;//不转折是0
extern int monotonicity_change_right_flag;//不转折是0
extern int continuity_change_right_flag; //连续是0
extern int continuity_change_left_flag;  //连续是0
extern uint8 Left_Up_Guai[2];
extern uint8 Right_Up_Guai[2];
//十字
extern uint8 Cross_Flag;
extern uint8 X_Cross;
extern uint8 Cross_state; //十字状态位
extern uint8 Left_Down_Find; //十字使用，找到被置行数，没找到就是0
extern uint8 Left_Up_Find;   //四个拐点标志
extern uint8 Right_Down_Find;
extern uint8 Right_Up_Find;
extern uint8 Straight_Flag;
extern uint8 Img_Disappear_Flag;
extern int32 Straight_Speed;
extern int32 Island_Speed_S;
extern int32 Island_Speed_M;
extern int32 Island_Speed_L;
extern int32 Isp_Speed;
extern int32 Straight_Speed;
extern int32 Island_Speed;
extern float FJ_Angle;
extern uint8 Stop_Flag;
extern int32 Time;
extern int32 Time_max;
extern double radiu;
extern int Mode;
extern int Isp_image03x;
extern int Isp_Mode;
extern int Speed_Min;
extern int Speed_Max;
extern int radiu_k;
extern int32 EXP_TIME;
extern uint8 Zebra_Stripes_Flag;
extern uint8 Change_Speed;
extern int32 Ness_pressure;
extern int32 Handao;
extern int32 Cro_Isl[15];
extern int32 CI_Speed[15];
extern int32 CIF;
extern int32 Y_MEET;    //最大前瞻计算点
//=====================================================用户层=======================================================

#endif

