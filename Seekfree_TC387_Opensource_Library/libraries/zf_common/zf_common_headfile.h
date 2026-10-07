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
#include "Flash.h"
#include "Fuzzy_PID.h"
#define PIT0                            (CCU60_CH0 )                            // 使用的周期中断编号
#define ENCODER_DIR_R                     (TIM6_ENCODER)                         // 带方向编码器对应使用的编码器接口
#define ENCODER_DIR_PULSE_R               (TIM6_ENCODER_CH1_P20_3)               // PULSE 对应的引脚
#define ENCODER_DIR_DIR_R                (TIM6_ENCODER_CH2_P20_0)               // DIR 对应的引脚
#define ENCODER_DIR_L                     (TIM5_ENCODER)                         // 带方向编码器对应使用的编码器接口
#define ENCODER_DIR_PULSE_L               (TIM5_ENCODER_CH1_P10_3)               // PULSE 对应的引脚
#define ENCODER_DIR_DIR_L                (TIM5_ENCODER_CH2_P10_1)               // DIR 对应的引脚

#define STEER_RIGHT  4155  //舵机右打死,-
#define STEER_MID    4290  //舵机归中
#define STEER_LEFT   4955  //舵机左打死,+
#define Instance_target 45
extern uint16 original_image[MT9V03X_H][MT9V03X_W];
extern int32 error;    // 中线偏差值
extern int32 error_instance;
extern int j;
extern int nega_pressure;    //负压涵道
//编码器输出
extern int16 dat_R;
extern int16 dat_L;
extern int16 dat_A;                        //左右轮均值
//电机参数
extern int16 Speed;
extern int16 motor_L;
extern int16 motor_R;
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

extern float ins_speed;
extern float sp_KP;
extern float sp_KI;
extern float sp_KD;
extern float sp_Bias_error;
extern float sp_Inteal_Bias_error;
extern float sp_Last_bias_error;
extern int base_speed;
extern int SP_err;
extern int Servo_Steer;
//中线最大值
extern uint8 hightest;
extern float deta_Speed_mage;
extern float Rad;
extern float Car_L_; //长(mm)
extern float Car_T_; //宽(mm)
extern float ins_temp; //摄像头与灯板距离
//=====================================================用户层=======================================================

#endif

