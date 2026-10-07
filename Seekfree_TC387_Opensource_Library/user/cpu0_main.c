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
* 文件名称          cpu0_main
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
#include "zf_common_headfile.h"


#pragma section all "cpu0_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中
int j;
int nega_pressure;
int16 dat_R = 0;                    //编码器输出
int16 dat_L = 0;
int16 dat_A = 0;                        //左右轮均值
//电机参数
int16 Speed=0;
int16 motor_L;
int16 motor_R;
//电机PID参数
float Velocity_KP=1.5;
float Velocity_KI=0;
float Velocity_KD=20;

float Bias_L=0;
float Pwm_motor_L=0;
float Last_bias_L=0;
float Inteal_Bias_L=0;

float Bias_R=0;
float Pwm_motor_R=0;
float Last_bias_R=0;
float Inteal_Bias_R=0;

float Chaspeed = 0;
float Cha_KP = 3.5;
float Cha_KI = 0;
float Cha_KD = 4.5;
float Bias_error = 0;
float Inteal_Bias_error = 0;
float Last_bias_error = 0;

float ins_speed = 0;
float sp_KP = 1.5;
float sp_KI = 0;
float sp_KD = 1.75;
float sp_Bias_error = 0;
float sp_Inteal_Bias_error = 0;
float sp_Last_bias_error = 0;

int SP_err = 0;
int Servo_Steer =0;
// **************************** 代码区域 ****************************
int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等
    Flash_read();
    All_init();
    // 此处编写用户代码 例如外设初始化代码等
    enableInterrupts();
    // 此处编写用户代码 例如外设初始化代码
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    while (TRUE)
    {
        // 此处编写需要循环执行的代码
                        PID_send(&sp_KP,&sp_KI,&sp_KD,&Speed,&Cha_KP,&Cha_KI,&Cha_KD,&nega_pressure,&Servo_Steer);
                        // 此处编写需要循环执行的代码
                        /*if(j)
                        {
                            printf("%d,%d,%d\n",dat_R,dat_L,Speed);
                            j=0;
                        }
                        */
                        //printf("%d,%d,%d\n",dat_R,dat_L,Speed);
                        //printf("%d,%d\n",hightest,0);
                        //printf("%d\n",nega_pressure);
                        //Print();
                        printf("%d,%d,%d\n",error,error_instance,SP_err);

    }
}
#pragma section all restore
// **************************** 代码区域 ****************************
