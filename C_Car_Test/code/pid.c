/*
 * pid.c
 *
 *  Created on: 2025年1月11日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
extern float SP_ERR[3];
float SP_ERR[3] = {45,25,15};//差速系数
int j = 0;
int nega_pressure = 0;
int16 dat_R = 0;                    //编码器输出
int16 dat_L = 0;
int16 dat_A = 0;                        //左右轮均值
//电机参数
int16 Speed=1;
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
//后车转向环参数
float Chaspeed = 0;
float Cha_KP = 3.5;
float Cha_KI = 0;
float Cha_KD = 4.5;
float Bias_error = 0;
float Inteal_Bias_error = 0;
float Last_bias_error = 0;

float ins_speed = 0;
//后车距离环参数
float sp_KP = 1.5;
float sp_KI = 0;
float sp_KD = 1.75;
float sp_Bias_error = 0;
float sp_Inteal_Bias_error = 0;
float sp_Last_bias_error = 0;

int SP_err = 0;
int Servo_Steer =0;
//以下两个公式和前车一致
int image_Position_PID(int Position,int target,float KP,float KI,float KD,float *Bias,float *Inteal_Bias,float *Last_Bias,float *Output,float MAX,float MIN)//位置式PID
{
    *Last_Bias=*Bias;
    *Bias=(float)target-Position;
    *Inteal_Bias+=*Bias;//误差累加
    *Output=KP*(*Bias)+KI*(*Inteal_Bias)+KD*(*Bias-*Last_Bias);//pid输出
    if(*Output >= MAX)*Output=MAX;
    else if (*Output <=MIN)*Output=MIN;//阈值
    return (int)*Output;
}
int Position_PID(int Position,int target,float KP,float KI,float KD,float *Bias,float *Inteal_Bias,float *Last_Bias,float *Output,float MAX,float MIN)//位置式PID
{
    *Last_Bias=*Bias;
    *Bias=(float)target-Position;
    *Inteal_Bias+=*Bias;
    *Output+=KP*(*Bias)+KI*(*Inteal_Bias)+KD*(*Bias-*Last_Bias);
    if(*Output >= MAX)*Output=MAX;
    else if (*Output <=MIN)*Output=MIN;
    return (int)*Output;
}
float Pow_invert(uint8_t X,uint8_t n)//x除以n次10
{
  float result=X;
    while(n--)
    {
        result/=10;
    }
    return result;
}
void PID_send()//无线调参，与前车代码类似，可以根据需要自行修改
{
            int Usart_RxFlag=0;
            int dot_Flag=0;
            int dot_after_num=0;
            uint8 data_buffer[32];
            uint8 data_len = (uint8)wireless_uart_read_buffer(data_buffer, 32);
            float Data = 0;
            system_delay_ms(25);
            if(data_len != 0)                                                       // 收到了消息 读取函数会返回实际读取到的数据个数
            {
                if(data_buffer[0]==0x50&&Usart_RxFlag==0)//
                {Usart_RxFlag=1;}
                if(Usart_RxFlag)
                {
                    Usart_RxFlag = data_buffer[1]-48;
                    for(int i=3;data_buffer[i]!=0x21;i++)
                {
                    if(dot_Flag==0)
                {
                  if(data_buffer[i]==0x2E)//如果识别到小数点，则将dot_Flag置1
                  {
                    dot_Flag=1;
                    dot_after_num++;
                  }
                  else//还没遇到小数点前的运算
                  {
                    Data = Data*10 + data_buffer[i]-48;
                  }
                    }
                   else//遇到小数点后的运算
                      {
                        Data = Data + Pow_invert(data_buffer[i]-48,dot_after_num);
                        dot_after_num++;
                      }
                }
                switch(Usart_RxFlag)
                {
                    case 1:sp_KP=Data;break;
                    case 2:sp_KI=Data;break;
                    case 3:sp_KD=Data;break;
                    case 4:base_speed = (int)Data;
                    if(base_speed==0)
                    {
                        Start_Flag = 0;
                        pwm_set_duty(ATOM1_CH6_P02_6,1000);
                        pwm_set_duty(ATOM1_CH7_P02_7,1000);
                        Servo_Steer = STEER_MID;
                        //调整舵机中值这部分取消注释
                        //pwm_set_duty(ATOM0_CH3_P21_5,(int)Data);
                    }
                    else
                            {

                            //Servo_Steer  = STEER_MID -400;
                            }break;
                    case 5:Cha_KP = Data;break;
                    case 6:Cha_KI = Data;break;
                    case 7:Cha_KD = Data;break;
                    case 8:SP_ERR[2] = Data;
                    break;
                    case 9:
                      ness_pressure = (int)Data;
                       // pwm_set_duty(ATOM1_CH3_P10_3, Data);
                    break;

                }
                system_delay_ms(25);
                Flash_write();
                }
                printf("Change!\n");
                //printf("%f,%f,%f,%d\n",Cha_KP,Cha_KI,Cha_KD,Speed);
            }
}
//float Rad = 0;
int base_speed = 0;
#define pi 3.141592
int Last_Servo = STEER_MID;
int Last_L_Servo = STEER_MID;
void Servo_ctrl()//舵机控制
{
    Servo_Steer =STEER_MID+ image_Position_PID(error,0,Cha_KP,0,Cha_KD,&Bias_error,&Inteal_Bias_error,&Last_bias_error,&Chaspeed,420,-420); //舵机计算
    if(Start_Flag==1) //后车速度计算
        Speed =  base_speed -  image_Position_PID(error_instance,Instance_target,sp_KP,sp_KI,sp_KD,&sp_Bias_error,&sp_Inteal_Bias_error,&sp_Last_bias_error,&Chaspeed,250,-250) ;
    AKM_SPEED_ERR();
    pwm_set_duty(ATOM1_CH3_P10_3, Servo_Steer); //调舵机中值时，这行代码需要注释掉
}
float Car_L_ = 20.0; //长(cm)
float Car_T_ = 15.5; //宽(cm)
void AKM_SPEED_ERR() //阿克曼结构
{
    if(abs(Servo_Steer - STEER_MID)<140)
        SP_err  = SP_ERR[0]*(Servo_Steer - STEER_MID)/400;
    else if(abs(Servo_Steer - STEER_MID)<280)
        SP_err  = SP_ERR[1]*(Servo_Steer - STEER_MID)/400;
    else
        SP_err  = SP_ERR[2]*(Servo_Steer - STEER_MID)/400;
}
int ness_pressure = 1000;
void motor_ctrl(int image_error,int Speed_Target,int16 Speed_R,int16 Speed_L)//电机控制
{
    if(Start_Flag==1)
    {
        motor_R=Position_PID(Speed_R,Speed_Target + image_error,Velocity_KP,Velocity_KI,Velocity_KD,&Bias_R,&Inteal_Bias_R,&Last_bias_R,&Pwm_motor_R,9999,-9999);//右轮电机
        motor_L=Position_PID(Speed_L,Speed_Target - image_error,Velocity_KP,Velocity_KI,Velocity_KD,&Bias_L,&Inteal_Bias_L,&Last_bias_L,&Pwm_motor_L,9999,-9999);//左轮电机
    }
    else
    {
        motor_R=Position_PID(Speed_R,0,Velocity_KP,Velocity_KI,Velocity_KD,&Bias_R,&Inteal_Bias_R,&Last_bias_R,&Pwm_motor_R,9999,-9999);//右轮电机
        motor_L=Position_PID(Speed_L,0,Velocity_KP,Velocity_KI,Velocity_KD,&Bias_L,&Inteal_Bias_L,&Last_bias_L,&Pwm_motor_L,9999,-9999);//左轮电机
    }
                        if(motor_R>=0)
                        {
                                pwm_set_duty(PWM_1,0);        //-右轮
                                pwm_set_duty(PWM_2,motor_R);  //+右轮
                        }
                        else if(motor_R < 0)
                        {
                            pwm_set_duty(PWM_1,-motor_R);        //-右轮
                            pwm_set_duty(PWM_2,0);  //+右轮
                        }
                        if(motor_L >= 0)
                        {
                            pwm_set_duty(PWM_3,motor_L);  //+左轮
                            pwm_set_duty(PWM_4,0);        //-左轮
                        }
                        else if(motor_L < 0)
                        {
                            pwm_set_duty(PWM_3,0);  //+左轮
                            pwm_set_duty(PWM_4,-motor_L);        //-左轮
                        }
}
