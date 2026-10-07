/*
 * pid.c
 *
 *  Created on: 2025年1月11日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
int image_Position_PID(int Position,int target,float KP,float KI,float KD,float *Bias,float *Inteal_Bias,float *Last_Bias,float *Output,float MAX,float MIN)//位置式PID
{
    *Bias=(float)target-Position;
    *Inteal_Bias+=*Bias;
    *Output=KP*(*Bias)+KI*(*Inteal_Bias)+KD*(*Bias-*Last_Bias);
    *Last_Bias=*Bias;
    if(*Output >= MAX)*Output=MAX;
    else if (*Output <=MIN)*Output=MIN;
    return (int)*Output;
}
int Position_PID(int Position,int target,float KP,float KI,float KD,float *Bias,float *Inteal_Bias,float *Last_Bias,float *Output,float MAX,float MIN)//位置式PID
{
    *Bias=(float)target-Position;
    *Inteal_Bias+=*Bias;
    *Output+=KP*(*Bias)+KI*(*Inteal_Bias)+KD*(*Bias-*Last_Bias);
    *Last_Bias=*Bias;
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
void PID_send()//无线调参
{
            int Usart_RxFlag=0;
            int dot_Flag=0;
            int dot_after_num=0;
            uint8 data_buffer[32];
            uint8 data_len = (uint8)wireless_uart_read_buffer(data_buffer, 32);
            float Data = 0;
            system_delay_ms(50);
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
                    case 1:Velocity_KP=Data;break;
                    case 2:Velocity_KI=Data;break;
                    case 3:Velocity_KD=Data;break;
                    case 4:Speed = (int)Data;break;
                    case 5:Cha_KP = Data;break;
                    case 6:Cha_KI = Data;break;
                    case 7:Cha_KD = Data;break;
                    case 8:pwm_set_duty(ATOM0_CH3_P21_5,(int)Data);break;
                    case 9:/*pwm_set_duty(ATOM0_CH1_P33_9,Data);*/break;

                }
                }
                Flash_write();
                printf("Change!\n");
                //printf("%f,%f,%f,%d\n",Cha_KP,Cha_KI,Cha_KD,Speed);
            }
}
int base_speed = 0;
#define pi 3.141592
int Last_Servo = STEER_MID;
int Last_L_Servo = STEER_MID;
void Servo_ctrl()
{
    //if(Speed !=0)
    //float CAR_L = Car_L_ + 4.8;
    Servo_Steer = image_Position_PID(error,0,Cha_KP,Cha_KI,Cha_KD,&Bias_error,&Inteal_Bias_error,&Last_bias_error,&Chaspeed,400,-400) + STEER_MID;
    float Rad = ((Servo_Steer - STEER_MID)*pi)/3000;
    //error_instance = sqrt(CAR_L*CAR_L + ins_temp*ins_temp - 2*ins_temp*CAR_L*cos(Rad));
    Speed = -image_Position_PID(error_instance,Instance_target,sp_KP,sp_KI,sp_KD,&sp_Bias_error,&sp_Inteal_Bias_error,&sp_Last_bias_error,&Chaspeed,80,-80) + base_speed;
    AKM_SPEED_ERR();
    pwm_set_duty(ATOM0_CH1_P33_9,Servo_Steer);
}

float Car_L_ = 20.0; //长(cm)
float Car_T_ = 15.5; //宽(cm)
float Rad = 0;
void AKM_SPEED_ERR() //阿克曼结构
{
    float k = (Car_T_*tan(Rad))/(2*Car_L_);
    float dif_SP = (Speed*k)/(1 - k);
    SP_err = (int)dif_SP;
}
void motor_ctrl(int image_error,int Speed_Target,int16 Speed_R,int16 Speed_L)//电机控制
{
        motor_R=Position_PID(Speed_R,Speed_Target + image_error,Velocity_KP,Velocity_KI,Velocity_KD,&Bias_R,&Inteal_Bias_R,&Last_bias_R,&Pwm_motor_R,6000,-6000);//右轮电机
        motor_L=Position_PID(Speed_L,Speed_Target - image_error,Velocity_KP,Velocity_KI,Velocity_KD,&Bias_L,&Inteal_Bias_L,&Last_bias_L,&Pwm_motor_L,6000,-6000);//左轮电机
        if(motor_R>=0)
        {
        pwm_set_duty(ATOM0_CH7_P02_7,0);        //-右轮
        pwm_set_duty(ATOM0_CH6_P02_6,motor_R);  //+右轮
        }
        else if(motor_R < 0)
        {
            pwm_set_duty(ATOM0_CH7_P02_7,-motor_R);        //-右轮
            pwm_set_duty(ATOM0_CH6_P02_6,0);  //+右轮
        }
        if(motor_L >= 0)
        {
            pwm_set_duty(ATOM0_CH4_P02_4,motor_L);  //+左轮
            pwm_set_duty(ATOM0_CH5_P02_5,0);        //-左轮
        }
        else if(motor_L < 0)
        {
            pwm_set_duty(ATOM0_CH4_P02_4,0);  //+左轮
            pwm_set_duty(ATOM0_CH5_P02_5,-motor_L);        //-左轮
        }
        //Servo_ctrl();
}
