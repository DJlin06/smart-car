/*
 * init.c
 *
 *  Created on: 2025年1月16日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
void All_init()//初始化函数
{
        mt9v03x_init();
        wireless_uart_init();
        ips200_init(IPS200_TYPE_SPI);
        ips200_show_string(0,0,"Camera init");
        pwm_init(ATOM1_CH3_P10_3,300,STEER_MID); //舵机PWM
        //这两个都是风扇的pwm
        pwm_init(ATOM1_CH6_P02_6,100,1000);
        pwm_init(ATOM1_CH7_P02_7,100,1000);
        pwm_init(PWM_1,17*1000,0);
        pwm_init(PWM_2,17*1000,0);
        pwm_init(PWM_3,17*1000,0);
        pwm_init(PWM_4,17*1000,0);
        system_delay_ms(500); //等待驱动上电
        pit_ms_init(CCU61_CH0, 10);     //100hz采样
        pit_ms_init(PIT0,2);  //2ms中断
        encoder_dir_init(ENCODER_DIR_R, ENCODER_DIR_PULSE_R, ENCODER_DIR_DIR_R);          // 初始化编码器模块与引脚 带方向增量编码器模式
        encoder_dir_init(ENCODER_DIR_L, ENCODER_DIR_PULSE_L, ENCODER_DIR_DIR_L);          // 初始化编码器模块与引脚 带方向增量编码器模式
        ness_pressure = Fuya;
}
void DIR_PULSE(int16 *DIR_DIR_R,int16 *DIR_DIR_L)  //编码器
{
    *DIR_DIR_R=-encoder_get_count(ENCODER_DIR_R)*2.375;
    *DIR_DIR_L=encoder_get_count(ENCODER_DIR_L)*2.375;
    encoder_clear_count(ENCODER_DIR_R);
    encoder_clear_count(ENCODER_DIR_L);
}

void Print()//输出电机速度
{
    static int m=0;
    if(m)
    {
        printf("%d,%d,%d\n",dat_R,dat_L,Speed);
        //printf("%d\n",error);
        m = 0;
    }
    else m++;
}

