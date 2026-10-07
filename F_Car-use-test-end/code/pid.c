/*
 * pid.c
 *
 *  Created on: 2025年1月11日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
#include "init.h"
int j;
int nega_pressure = 0;
int16 dat_R = 0;                    //编码器输出
int16 dat_L = 0;
int16 dat_A = 0;                        //左右轮均值
//电机参数
int32 Speed=0;  //基础速度
int32 Real_Speed = 0;   //实际速度
//电机PWM输出
int16 motor_L = 0;
int16 motor_R = 0;
int16 motor = 0;
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
//转向环PID参数
float Chaspeed = 0;
float Cha_KP = 0.35;
float Cha_KI = 0;
float Cha_KD = 2.5;
float Bias_error = 0;
float Inteal_Bias_error = 0;
float Last_bias_error = 0;
int SP_err = 0;     //差速速度
uint8 Stop_Flag=0;
int32 Isp_Speed = 195; //速度显示值
//三种环岛速度，科可手动调整
int32 Island_Speed_S = 210;
int32 Island_Speed_M = 210;
int32 Island_Speed_L = 210;
int32 Straight_Speed = 260; //直道速度，可手动调整
int32 Cro_Isl[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int32 CI_Speed[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int32 CIF = 0;
//位置式PID
int image_Position_PID(int Position,int target,float KP,float KI,float KD,float *Bias,float *Inteal_Bias,float *Last_Bias,int *Output,int MAX,int MIN)//位置式PID
{
    *Bias=(float)target-Position;
    *Inteal_Bias+=*Bias;
    *Output=(int)(KP*(*Bias)+KI*(*Inteal_Bias)+KD*(*Bias-*Last_Bias));
    *Last_Bias=*Bias;
    if(*Output >= MAX)*Output=MAX;
    else if (*Output <=MIN)*Output=MIN;
    return *Output;
}
//增量式PID变式，其中的kp相当于位置式PID的ki，kd相当于位置式PID的kp
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
void PID_send()//无线调参，此函数需要搭配上位机使用
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
                if(data_buffer[0]=='S'&&data_buffer[1]=="T"&&data_buffer[2]=='O'&&data_buffer[3]=='P')
                {
                    Stop_Flag=1;
                }
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
                    case 4:
                        Speed = (int)Data;
                        if(Speed==0)
                            {
                                Stop_Flag=1;
                            }
                        break;
                    case 5:Cha_KP = Data;break;
                    case 6:Cha_KI = Data;break;
                    case 7:Cha_KD = Data;break;
                    case 8:nega_pressure = (int)Data;
                    pwm_set_duty(Handao_PWM,nega_pressure);
                    break;

                }
                Flash_write();
                }

                //printf("Change!\n");
                //printf("%f,%f,%f,%d\n",Cha_KP,Cha_KI,Cha_KD,Speed);
            }
}

void motor_ctrl(int image_error,int Speed_Target,int16 Speed_R,int16 Speed_L)//电机控制PID
{
    if(Stop_Flag!=1&&Speed!=0)
    {
        motor_R=Position_PID(Speed_R,Speed_Target + image_error,Velocity_KP,Velocity_KI,Velocity_KD,&Bias_R,&Inteal_Bias_R,&Last_bias_R,&Pwm_motor_R,9999,-9999);//右轮电机
        motor_L=Position_PID(Speed_L,Speed_Target - image_error,Velocity_KP,Velocity_KI,Velocity_KD,&Bias_L,&Inteal_Bias_L,&Last_bias_L,&Pwm_motor_L,9999,-9999);//左轮电机
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
    else if(Stop_Flag==1)
    {
        //停车控制
        pwm_set_duty(Handao_PWM,0);
        pwm_set_duty(FS_PWM_1,0);
        pwm_set_duty(FS_PWM_2,0);
        gpio_set_level(P10_3,0);
        pwm_set_duty(PWM_1,0);        //-右轮
        pwm_set_duty(PWM_2,0);  //+右轮
        pwm_set_duty(PWM_3,0);  //+左轮
        pwm_set_duty(PWM_4,0);        //-左轮
        /*motor_R=Position_PID(Speed_R,0,Velocity_KP,Velocity_KI,Velocity_KD,&Bias_R,&Inteal_Bias_R,&Last_bias_R,&Pwm_motor_R,9999,-9999);//右轮电机
        motor_L=Position_PID(Speed_L,0,Velocity_KP,Velocity_KI,Velocity_KD,&Bias_L,&Inteal_Bias_L,&Last_bias_L,&Pwm_motor_L,9999,-9999);//左轮电机
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
                            }*/
    }

}
int Speed_Min = 150;
int Speed_Max = 230;
int radiu_k = 1100;
//速度决策部分，这一部分通过曲率计算速度，需要根据实际情况作出相应的修正，比较繁琐，也可以不使用，使用手动修改速度
int curvature_to_speed(double kappa)
{
    //const double a_max = Speed;   // 实测最大向心加速度（m/s²）
    int min_speed = Speed_Min; // 最小防倾倒速度
    int max_speed = Speed_Max; // 直道最大速度
    if(Cross_state&&Cro_Isl[CIF]==4)
        max_speed = Speed_Max-10;
    int k = radiu_k;
    if(Speed==0)
        return 0;
    static int zero_time = 0;
    if (fabs(kappa) < (1e-2)/2.0&&Search_Stop_Line>=75&&continuity_change_left_flag<=55&&continuity_change_right_flag<=55)
        zero_time++;
    else
        zero_time=0;
    if(zero_time>=10)
        return max_speed; // 直道情况
    //int safe_speed = Speed - kappa*k;
    int safe_speed = (int)sqrt((float)k/kappa);
    //double safe_speed = sqrt(a_max / kappa);
    // 限幅处理
    if(safe_speed > max_speed)
        safe_speed = max_speed;
    if(safe_speed < min_speed)
        safe_speed = min_speed;
    return safe_speed;
}
uint8 Change_Speed = 0;//是否选择使用速度决策的标志位，1为使用，0为不使用
void Speed_Choice()
{

    if(Img_Disappear_Flag == 1) //丢图停车
    {
        Stop_Flag = 1;
    }
    else if(Zebra_Stripes_Flag == 1&&interrupt>=50) //斑马线后过0.5s终点停车
    {
        Stop_Flag = 1;
        Zebra_Stripes_Flag = 0;
    }
    else
    {
        //速度决策
        if(Change_Speed==1)
        {
            //根据元素给速度
            if(Island_State<=8&&Island_State>=1&&Speed!=0)
            {
                if(Cro_Isl[CIF]==1)
                Real_Speed = Island_Speed_S;
                else if(Cro_Isl[CIF]==2)
                Real_Speed = Island_Speed_M;
                else if(Cro_Isl[CIF]==3)
                Real_Speed = Island_Speed_L;
            }
            /*else if(Cross_state&&Speed!=0)
            {
                Real_Speed = 170;
            }*/
            else if(Straight_Flag == 1&&Img_Disappear_Flag==0&&Stop_Flag==0&&Speed!=0)
            {
                Real_Speed = Straight_Speed;
            }
            else if(Speed!=0)
            {
                Real_Speed = CI_Speed[CIF];//手动输入
                //Real_Speed = curvature_to_speed(radiu);//曲率计算
            }
            else
                Real_Speed = 0;
        }
        else
        {
            if(Island_State<=8&&Island_State>=1&&Speed!=0)
            {
                if(Cro_Isl[CIF]==1)
                Real_Speed = Island_Speed_S;
                else if(Cro_Isl[CIF]==2)
                Real_Speed = Island_Speed_M;
                else if(Cro_Isl[CIF]==3)
                Real_Speed = Island_Speed_L;
            }
            /*else if(Cross_state&&Speed!=0)
            {
                Real_Speed = 170;
            }*/
            else if(Straight_Flag == 1&&Img_Disappear_Flag==0&&Stop_Flag==0&&Speed!=0)
            {
                Real_Speed = Straight_Speed;
            }
            else if(Speed!=0)
            {
                Real_Speed = Speed;
            }
            else
                Real_Speed = 0;
        }
    }

}
