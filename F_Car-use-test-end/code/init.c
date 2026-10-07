/*
 * init.c
 *
 *  Created on: 2025年1月16日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
void All_init()//初始化函数
{
    gpio_init(P10_3, GPO, 0, GPO_PUSH_PULL);//前车灯光控制引脚
    gpio_init(P02_5, GPO, 0, GPO_PUSH_PULL);//蜂鸣器控制引脚
    //按键控制引脚
    gpio_init(IO_MODE,GPI,0,0);
    gpio_init(IO_ADD,GPI,0,0);
    gpio_init(IO_SUB,GPI,0,0);
    gpio_init(IO_End,GPI,0,0);
    mt9v03x_init();//摄像头初始化
    imu660ra_init();//陀螺仪初始化
    wireless_uart_init();//无线转串口初始化
    ips200_init(IPS200_TYPE_SPI);//显示屏初始化
    ips200_show_string(0,0,"Camera init");
    pwm_init(Handao_PWM,100,1000); //涵道PWM初始化
    pwm_init(FS_PWM_1,100,1000);    //风扇PWM初始化
    pwm_init(FS_PWM_2,100,1000);
    //电机控制信号PWM初始化
    pwm_init(PWM_1,17*1000,0);
    pwm_init(PWM_2,17*1000,0);
    pwm_init(PWM_3,17*1000,0);
    pwm_init(PWM_4,17*1000,0);
    system_delay_ms(500);//延时等待驱动上电
    pit_ms_init(PIT2,5);        //5ms采样
    pit_ms_init(PIT1,10);     //10ms采样
    pit_ms_init(PIT0,2);  //2ms中断
    pit_ms_init(PIT3,100);  //100ms中断
    encoder_dir_init(ENCODER_DIR_R, ENCODER_DIR_PULSE_R, ENCODER_DIR_DIR_R);          // 初始化编码器模块与引脚 带方向增量编码器模式
    encoder_dir_init(ENCODER_DIR_L, ENCODER_DIR_PULSE_L, ENCODER_DIR_DIR_L);          // 初始化编码器模块与引脚 带方向增量编码器模式
    Flash_read();//读取FLASH数据
    //Speed = 195;
    Stop_Flag = 0;//停车标志位清零
}
void DIR_PULSE(int16 *DIR_DIR_R,int16 *DIR_DIR_L)  //编码器读取
{
    *DIR_DIR_R=-encoder_get_count(ENCODER_DIR_R)*2.375;
    *DIR_DIR_L=encoder_get_count(ENCODER_DIR_L)*2.375;
    encoder_clear_count(ENCODER_DIR_R);
    encoder_clear_count(ENCODER_DIR_L);
}

void Print(int *m)//输出电机速度
{

    if(*m)
    {
        printf("%d,%d,%d\n",dat_R,dat_L,Speed);
        //printf("%d\n",error);
        *m = 0;
    }
    else *m++;
}
void Isp_init() //显示屏首页显示
{
    ips200_show_string(0,9*16,"B_S_L:");//j*8
    ips200_show_string(100,9*16,"C&M_L:");
    ips200_show_string(0,10*16,"B_S_R:");//j*8
    ips200_show_string(100,10*16,"C&M_R:");
    ips200_show_string(0,11*16,"S_TOP:");
    ips200_show_string(100,11*16,"L&R_LOS:");
    ips200_show_string(160,12*16,"B_LOS:");
    ips200_show_string(0,13*16,"L_G_D_U_YYX:");
    ips200_show_string(0,14*16,"R_G_D_U_YYX:");
    ips200_show_string(0,15*16,"L_D_D_Find:");
    ips200_show_string(0,16*16,"R_U_D_Find:");
    ips200_show_string(0,12*16,"Cro_F:");
    ips200_show_string(80,12*16,"Isl_F:");
    ips200_show_string(0,8*16,"Isp_Mode:");
    ips200_show_string(8*9,8*16,"image_test");
    ips200_show_string(0,18*16,"Real_Speed:");

}
