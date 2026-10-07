/*
 * UI.c
 *
 *  Created on: 2025年7月31日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
int Mode = 0; //模式选择
int Isp_image03x = 0;//图像显示标志位
int Isp_Mode = 0;
int Mode_change = 0;
int Mode_Stay = 0;
int Isp_Mode_Stay = 0;
int32 EXP_TIME = 250;//曝光时间设置
int32 Ness_pressure = 0;
int32 Handao = 0;
int IO_Mode_Key_Push_Short()    //模式切换短按判断
{
    if(!gpio_get_level(IO_MODE))
    {
    system_delay_ms(20);
    if(!gpio_get_level(IO_MODE))
    {
        do
        {
            while(!gpio_get_level(IO_MODE));
            system_delay_ms(20);
        }
        while(!gpio_get_level(IO_MODE));
        return 1;
    }
    else
        return 0;
    }
    else
        return 0;
}
int IO_Mode_Key_Push_Long()  //模式切换长按判断
{
    if(!gpio_get_level(IO_MODE))
    {
    system_delay_ms(1000);
    if(!gpio_get_level(IO_MODE))
    {
        do
        {
            while(!gpio_get_level(IO_MODE));
            system_delay_ms(20);
        }
        while(!gpio_get_level(IO_MODE));
        return 1;
    }
    else
        return 0;
    }
    else
        return 0;
}
int IO_ADD_Key_Push()  //按键+短按判断
{
    if(!gpio_get_level(IO_ADD))
    {
    system_delay_ms(20);
    if(!gpio_get_level(IO_ADD))
    {
        do
        {
            while(!gpio_get_level(IO_ADD));
            system_delay_ms(20);
        }
        while(!gpio_get_level(IO_ADD));
        return 1;
    }
    else
        return 0;
    }
    else
        return 0;
}
int IO_SUB_Key_Push()  //按键-短按判断
{
    if(!gpio_get_level(IO_SUB))
    {
    system_delay_ms(20);
    if(!gpio_get_level(IO_SUB))
    {
        do
        {
            while(!gpio_get_level(IO_SUB));
            system_delay_ms(20);
        }
        while(!gpio_get_level(IO_SUB));
        return 1;
    }
    else
        return 0;
    }
    else
        return 0;
}
int IO_END_Key_Push()  //按键确认短按判断
{
    if(!gpio_get_level(IO_End))
    {
    system_delay_ms(20);
    if(!gpio_get_level(IO_End))
    {
        do
        {
            while(!gpio_get_level(IO_End));
            system_delay_ms(20);
        }
        while(!gpio_get_level(IO_End));
        return 1;
    }
    else
        return 0;
    }
    else
        return 0;
}
void Begin() //发车准备
{
    Mode_Stay = 1;
    Isp_Mode_Stay = 1;
    Flash_write();
    system_delay_ms(1000);
    nega_pressure = Ness_pressure;
    system_delay_ms(2000);
    pwm_set_duty(Handao_PWM,Handao);    //涵道开启
    gpio_set_level(P10_3,1);    //尾灯使能
    system_delay_ms(3000);
    Speed = Isp_Speed;
}
void Key_Control() //按键控制主体部分
{
    if(Mode_change==0&&Isp_Mode_Stay==0)
    {
        if(IO_Mode_Key_Push_Long())
        {
            Mode_change = 1;
            ips200_show_string(8*9,8*16,"                 ");
            ips200_show_string(8*9,8*16,"Mode_Select");
        }
    }

    if(Mode_change==1)
    {
        if(IO_Mode_Key_Push_Short())
        {
            Isp_Mode+=1;
            if(Isp_Mode==6)
                Isp_Mode = 0;
            ips200_show_string(8*9,8*16,"                    ");//清除
            if(Isp_Mode==0) //模式0，图像测试
            {
                ips200_clear();
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
                ips200_show_string(8*9,8*16,"                 ");
                ips200_show_string(8*9,8*16,"image_test");
                ips200_show_string(0,18*16,"Real_Speed:");
            }
            else if(Isp_Mode==1)    //模式1，速度参数设置
            {
                ips200_clear();
                ips200_show_string(8*9,8*16,"                 ");
                ips200_show_string(8*9,8*16,"Speed_Select");
                ips200_show_string(0,13*16,"Time:");
                ips200_show_string(0,10*16,"Speed:");
                ips200_show_string(0,11*16,"Island_Speed:");
                ips200_show_string(0,12*16,"Straight_Speed:");
                ips200_show_string(0,8*16,"Isp_Mode:");
                ips200_show_string(8*9,8*16,"Speed_Select");
                ips200_show_string(0,9*16,"Mode:");
                ips200_show_string(40,9*16,"Speed");
                ips200_show_string(0,14*16,"Speed_Max:");
                ips200_show_string(0,15*16,"Speed_Min:");
                ips200_show_string(0,16*16,"radiu_k:");
                ips200_show_string(0,17*16,"Speed_Change:");
                ips200_show_string(0,18*16,"FS_pressure:");
                ips200_show_string(0,19*16,"Handao:");
            }
			else if (Isp_Mode == 2)	//模式2，PID参数设置
            {
                ips200_clear();
                ips200_show_string(0,8*16,"Isp_Mode:");
                ips200_show_string(8*9,8*16,"                 ");
                ips200_show_string(8*9,8*16,"Pid_Select");
                ips200_show_string(0,9*16,"Mode:");
                ips200_show_string(40,9*16,"M_KP");
                ips200_show_string(0,13*16,"SP_KP:");
                ips200_show_string(0,14*16,"SP_KI:");
                ips200_show_string(0,15*16,"SP_KD:");
                ips200_show_string(0,10*16,"M_KP:");
                ips200_show_string(0,11*16,"M_KI:");
                ips200_show_string(0,12*16,"M_KD:");
            }
            else if(Isp_Mode==3)    //模式3，图像参数设置
                {
                ips200_clear();
                ips200_show_string(0,8*16,"Isp_Mode:");
                ips200_show_string(8*9,8*16,"                 ");
                ips200_show_string(8*9,8*16,"image_test_Select");
                ips200_show_string(0,9*16,"Mode:");
                ips200_show_string(40,9*16,"EXP_TIME");
                ips200_show_string(0,10*16,"EXP_TIME:");
                ips200_show_string(0,11*16,"Y_MEET:");
                }
            else if(Isp_Mode==4)    //模式4，路径选择设置
                {
                ips200_clear();
                ips200_show_string(0,8*16,"Isp_Mode:");
                ips200_show_string(8*9,8*16,"                 ");
                ips200_show_string(8*9,8*16,"Cro_Isl_Select");
                ips200_show_string(0,9*16,"Mode:");
                ips200_show_string(40,9*16,"0");
                ips200_show_string(0,10*16,"0.");
                ips200_show_string(0,11*16,"1.");
                ips200_show_string(0,12*16,"2.");
                ips200_show_string(0,13*16,"3.");
                ips200_show_string(0,14*16,"4.");
                ips200_show_string(0,15*16,"5.");
                ips200_show_string(0,16*16,"6.");
                ips200_show_string(0,17*16,"7.");
                ips200_show_string(0,18*16,"8.");
                ips200_show_string(0,19*16,"9.");
                ips200_show_string(100,10*16,"10.");
                ips200_show_string(100,11*16,"11.");
                ips200_show_string(100,12*16,"12.");
                ips200_show_string(100,13*16,"13.");
                ips200_show_string(100,14*16,"14.");
                }
			else if (Isp_Mode == 5)	//模式5，巡线速度选择设置
                {
                ips200_clear();
                ips200_show_string(0,8*16,"Isp_Mode:");
                ips200_show_string(8*9,8*16,"                 ");
                ips200_show_string(8*9,8*16,"CI_Speed_Select");
                ips200_show_string(0,9*16,"Mode:");
                ips200_show_string(40,9*16,"0");
                ips200_show_string(0,10*16,"0.");
                ips200_show_string(0,11*16,"1.");
                ips200_show_string(0,12*16,"2.");
                ips200_show_string(0,13*16,"3.");
                ips200_show_string(0,14*16,"4.");
                ips200_show_string(0,15*16,"5.");
                ips200_show_string(0,16*16,"6.");
                ips200_show_string(0,17*16,"7.");
                ips200_show_string(0,18*16,"8.");
                ips200_show_string(0,19*16,"9.");
                ips200_show_string(100,10*16,"10.");
                ips200_show_string(100,11*16,"11.");
                ips200_show_string(100,12*16,"12.");
                ips200_show_string(100,13*16,"13.");
                ips200_show_string(100,14*16,"14.");
                }

        }
        if(IO_END_Key_Push())   //确认键退出模式选择
        {
            Isp_Mode_Stay = 1;
            Mode_change = 0;
            Mode_Stay = 0;
            Mode = 0;
        }
    }
    if(Isp_Mode==0&&Mode_change==0&&Mode_Stay==0)
    {
        ips200_show_string(8*9,8*16,"                 ");
        ips200_show_string(8*9,8*16,"image_test");
        if(IO_ADD_Key_Push())
        {
            Isp_image03x+=1;
            if(Isp_image03x==2)
            Isp_image03x=0;
        }
        if(IO_SUB_Key_Push())
        {
            Isp_image03x-=1;
            if(Isp_image03x==-1)
                Isp_image03x=1;
        }
        if(IO_END_Key_Push())
        {
            Mode_Stay = 1;
            Isp_Mode_Stay = 0;
        }
    }
    if(Isp_Mode==1&&Mode_change==0&&Mode_Stay==0)
    {
        if(IO_Mode_Key_Push_Short())
        {
            Mode+=1;
            if(Mode==12)
            {
                Mode=0;
            }
            ips200_show_string(40,9*16,"                    ");//清除
            if(Mode==0)
                ips200_show_string(40,9*16,"Speed");
            else if(Mode==1)
                ips200_show_string(40,9*16,"Island_Speed_S");
            else if(Mode==2)
                ips200_show_string(40,9*16,"Island_Speed_M");
            else if(Mode==3)
                ips200_show_string(40,9*16,"Island_Speed_L");
            else if(Mode==4)
                ips200_show_string(40,9*16,"Straight_Speed");
            else if(Mode==5)
                ips200_show_string(40,9*16,"Time");
            else if(Mode==6)
                ips200_show_string(40,9*16,"Speed_Max");
            else if(Mode==7)
                ips200_show_string(40,9*16,"Speed_Min");
            else if(Mode==8)
                ips200_show_string(40,9*16,"radiu_k");
            else if(Mode==9)
                ips200_show_string(40,9*16,"Speed_Change");
            else if(Mode==10)
                ips200_show_string(40,9*16,"FS_pressure");
            else if(Mode==11)
                ips200_show_string(40,9*16,"Handao");
        }

        if(IO_ADD_Key_Push())
        {
            if(Mode_Stay==0)
            {
                if(Mode==0)
                {
                        Isp_Speed+=2;
                }
                else if(Mode==1)
                {
                    Island_Speed_S+=2;
                }
                else if(Mode==2)
                {
                    Island_Speed_M+=2;
                }
                else if(Mode==3)
                {
                    Island_Speed_L+=2;
                }
                else if(Mode==4)
                {
                        Straight_Speed+=2;
                }
                else if(Mode==5)
                {
                        Time_max+=10;
                }
                else if(Mode==6)
                {
                        Speed_Max+=5;
                }
                else if(Mode==7)
                {
                        Speed_Min+=5;
                }
                else if(Mode==8)
                {
                        radiu_k+=10;
                }
                else if(Mode==9)
                {
                    Change_Speed = Change_Speed==1?0:1;
                }
                else if(Mode==10)
                {
                    Ness_pressure+=50;
                }
                else if(Mode==11)
                {
                    Handao+=50;
                }
            }
            }

        if(IO_SUB_Key_Push())
        {
            if(Mode_Stay==0)
            {
                if(Mode==0)
                {
                        Isp_Speed-=2;
                }
                else if(Mode==1)
                {
                    Island_Speed_S-=2;
                }
                else if(Mode==2)
                {
                    Island_Speed_M-=2;
                }
                else if(Mode==3)
                {
                    Island_Speed_L-=2;
                }
                else if(Mode==4)
                {
                    Straight_Speed-=2;
                }
                else if(Mode==5)
                {
                    Time_max-=10;
                }
                else if(Mode==6)
                {
                        Speed_Max-=5;
                }
                else if(Mode==7)
                {
                        Speed_Min-=5;
                }
                else if(Mode==8)
                {
                        radiu_k-=10;
                }
                else if(Mode==9)
                {
                    Change_Speed = Change_Speed==1?0:1;
                }
                else if(Mode==10)
                {
                    Ness_pressure-=50;
                }
                else if(Mode==11)
                {
                    Handao-=50;
                }
            }
        }

        if(IO_END_Key_Push())
        {
            Mode_Stay = 1;
            Isp_Mode_Stay = 0;
        }
    }

    if(Isp_Mode==2&&Mode_change==0&&Mode_Stay==0)
    {
        if(IO_Mode_Key_Push_Short())
        {
            Mode+=1;
            if(Mode==6)
            {
                Mode=0;
            }
            ips200_show_string(40,9*16,"                    ");//清除
            if(Mode==0)
                ips200_show_string(40,9*16,"M_KP");
            else if(Mode==1)
                ips200_show_string(40,9*16,"M_KI");
            else if(Mode==2)
                ips200_show_string(40,9*16,"M_KD");
            else if(Mode==3)
                ips200_show_string(40,9*16,"SP_KP");
            else if(Mode==4)
                ips200_show_string(40,9*16,"SP_KI");
            else if(Mode==5)
                ips200_show_string(40,9*16,"SP_KD");
        }

        if(IO_ADD_Key_Push())
        {
            if(Mode_Stay==0)
            {
                if(Mode==0)
                {
                    Cha_KP+=0.01;
                }
                else if(Mode==1)
                {
                    Cha_KI+=0.01;
                    Cha_KI = 0;
                }
                else if(Mode==2)
                {
                    Cha_KD+=0.05;
                }
                else if(Mode==3)
                {
                    Velocity_KP+=0.05;
                }
                else if(Mode==4)
                {
                    Velocity_KI+=0.01;
                    Velocity_KI = 0;
                }
                else if(Mode==5)
                {
                    Velocity_KD+=1;
                }
            }
            }

        if(IO_SUB_Key_Push())
        {
            if(Mode_Stay==0)
            {
                if(Mode==0)
                {
                    Cha_KP-=0.01;
                }
                else if(Mode==1)
                {
                    Cha_KI-=0.01;
                    Cha_KI = 0;
                }
                else if(Mode==2)
                {
                    Cha_KD-=0.05;
                }
                else if(Mode==3)
                {
                    Velocity_KP-=0.05;
                }
                else if(Mode==4)
                {
                    Velocity_KI-=0.01;
                    Velocity_KI = 0;
                }
                else if(Mode==5)
                {
                    Velocity_KD-=1;
                }
            }
        }

        if(IO_END_Key_Push())
        {
            Mode_Stay = 1;
            Isp_Mode_Stay = 0;
        }
    }
    if(Isp_Mode==3&&Mode_change==0&&Mode_Stay==0)
    {
        if(IO_Mode_Key_Push_Short())
        {
            Mode+=1;
            if(Mode==3)
            {
                Mode=0;
            }
            ips200_show_string(40,9*16,"                    ");//清除
            if(Mode==0)
                ips200_show_string(40,9*16,"EXP_TIME");
            else if(Mode==1)
                ips200_show_string(40,9*16,"Isp_image03x");
            else if(Mode==2)
                ips200_show_string(40,9*16,"Y_MEET");
        }
        if(IO_ADD_Key_Push())
        {
            if(Mode_Stay==0)
            {
                if(Mode==0)
                {
                    if(EXP_TIME>50)
                        EXP_TIME+=10;
                    else if(EXP_TIME>30)
                        EXP_TIME+=5;
                    else if(EXP_TIME>12)
                        EXP_TIME+=3;
                    else
                        EXP_TIME+=2;
                    mt9v03x_set_exposure_time(EXP_TIME);
                }
                else if(Mode==1)
                {
                    Isp_image03x+=1;
                    if(Isp_image03x==2)
                    Isp_image03x=0;
                }
                else if(Mode==2)
                {
                    Y_MEET++;
                }
            }

        }
        if(IO_SUB_Key_Push())
        {
            if(Mode_Stay==0)
            {
                if(Mode==0)
                {
                    if(EXP_TIME>50)
                        EXP_TIME-=10;
                    else if(EXP_TIME>30)
                        EXP_TIME-=5;
                    else if(EXP_TIME>12)
                        EXP_TIME-=3;
                    else
                        EXP_TIME-=2;
                    mt9v03x_set_exposure_time(EXP_TIME);
                }
                else if(Mode==1)
                {
                    Isp_image03x+=1;
                    if(Isp_image03x==2)
                    Isp_image03x=0;
                }
                else if(Mode==2)
                {
                    Y_MEET--;
                }
            }
        }
        if(IO_END_Key_Push())
        {
            Mode_Stay = 1;
            Isp_Mode_Stay = 0;
        }
    }
    if(Isp_Mode==4&&Mode_change==0&&Mode_Stay==0)
    {
        if(IO_Mode_Key_Push_Short())
        {
            Mode+=1;
            if(Mode==15)
            {
                Mode=0;
            }
            ips200_show_string(40,9*16,"  ");//清除
            ips200_show_int(40,9*16,Mode,2);
        }

        if(IO_ADD_Key_Push())
        {
            if(Mode_Stay==0)
            {
                    Cro_Isl[Mode]+=1;
                    if(Cro_Isl[Mode]>6)
                        Cro_Isl[Mode] = 0;
            }
        }
        if(IO_SUB_Key_Push())
        {
            if(Mode_Stay==0)
            {
                Cro_Isl[Mode]-=1;
                if(Cro_Isl[Mode]<0)
                    Cro_Isl[Mode] = 6;
            }
        }
        if(IO_END_Key_Push())
        {
            Mode_Stay = 1;
            Isp_Mode_Stay = 0;
        }
    }
    if(Isp_Mode==5&&Mode_change==0&&Mode_Stay==0)
    {
        if(IO_Mode_Key_Push_Short())
        {
            Mode+=1;
            if(Mode==15)
            {
                Mode=0;
            }
            ips200_show_string(40,9*16,"  ");//清除
            ips200_show_int(40,9*16,Mode,2);
        }

        if(IO_ADD_Key_Push())
        {
            if(Mode_Stay==0)
            {
                CI_Speed[Mode]+=2;
            }
        }
        if(IO_SUB_Key_Push())
        {
            if(Mode_Stay==0)
            {
                CI_Speed[Mode]-=2;
            }
        }
        if(IO_END_Key_Push())
        {
            Mode_Stay = 1;
            Isp_Mode_Stay = 0;
        }
    }
	if (IO_END_Key_Push())  //发车确认
	{
		if (Mode_Stay == 1 && Isp_Mode_Stay == 1)   //发车准备
    {
        Begin();
    }

}
	void Value_isp() //参数显示
{
    if(Value_isp_Flag)
    {
        Value_isp_Flag = 0;
        if(Isp_image03x==0)
        {
            ips200_displayimage03x(image_two_value[35], MT9V03X_W, MT9V03X_H-35);
        }
        else if(Isp_image03x==1)
        {
            ips200_displayimage03x(mt9v03x_image[35], MT9V03X_W, MT9V03X_H-35);
        }
        if(Isp_Mode==0)
        {
            ips200_show_int(48,9*16,Boundry_Start_Left,3);
            ips200_show_int(100+48,9*16,continuity_change_left_flag,3);
            ips200_show_int(200,9*16,monotonicity_change_left_flag,3);
            ips200_show_int(48,10*16,Boundry_Start_Right,3);
            ips200_show_int(100+48,10*16,continuity_change_right_flag,3);
            ips200_show_int(200,10*16,monotonicity_change_right_flag,3);
            ips200_show_int(48,11*16,Search_Stop_Line,3);
            ips200_show_int(164,11*16,Left_Lost_Time,3);
            ips200_show_int(200,11*16,Right_Lost_Time,3);
            ips200_show_int(160+48,12*16,Both_Lost_Time,3);
            ips200_show_int(96,13*16,left_down_guai[0],3);
            ips200_show_int(160,13*16,Left_Up_Guai[0],3);
            ips200_show_int(200,13*16,Left_Up_Guai[1],3);
            ips200_show_int(96,14*16,right_down_guai[0],3);
            ips200_show_int(160,14*16,Right_Up_Guai[0],3);
            ips200_show_int(200,14*16,Right_Up_Guai[1],3);
            ips200_show_int(100,15*16,Left_Up_Find,3);
            ips200_show_int(200,15*16,Left_Down_Find,3);
            ips200_show_int(100,16*16,Right_Up_Find,3);
            ips200_show_int(200,16*16,Right_Down_Find,3);
            ips200_show_int(48,12*16,Cross_state,1);
            ips200_show_int(60,12*16,X_Cross,1);
            ips200_show_int(80+48,12*16,Island_State,1);
            ips200_show_int(80+48+12,12*16,Zebra_Stripes_Flag,1);
            ips200_show_float(0,17*16,abs(FJ_Angle),3,2);
            ips200_show_float(100,17*16,radiu,4,6);
            ips200_show_int(88,18*16,sqrt((float)radiu_k/radiu),3);
        }
        else if(Isp_Mode==1)
        {
            ips200_show_int(48,10*16,Isp_Speed,3);
            ips200_show_int(8*13,11*16,Island_Speed_S,3);
            ips200_show_int(8*18,11*16,Island_Speed_M,3);
            ips200_show_int(8*23,11*16,Island_Speed_L,3);
            ips200_show_int(8*15,12*16,Straight_Speed,3);
            ips200_show_int(40,13*16,Time_max,3);
            ips200_show_int(80,14*16,Speed_Max,3);
            ips200_show_int(80,15*16,Speed_Min,3);
            ips200_show_int(64,16*16,radiu_k,4);
            ips200_show_string(13*8,17*16,"   ");
            if(Change_Speed==1)
                ips200_show_string(13*8,17*16,"ON");
            else
                ips200_show_string(13*8,17*16,"OFF");
            ips200_show_int(12*8,18*16,Ness_pressure,4);
            ips200_show_int(7*8,19*16,Handao,4);

        }
        else if(Isp_Mode==2)
        {
            ips200_show_float(40,10*16,Cha_KP,3,2);
            ips200_show_float(40,11*16,Cha_KI,3,2);
            ips200_show_float(40,12*16,Cha_KD,3,2);
            ips200_show_float(48,13*16,Velocity_KP,3,2);
            ips200_show_float(48,14*16,Velocity_KI,3,2);
            ips200_show_float(48,15*16,Velocity_KD,3,2);
        }
        else if(Isp_Mode==3)
        {
            ips200_show_int(72,10*16,EXP_TIME,4);
            ips200_show_int(56,11*16,Y_MEET,3);
        }
        else if(Isp_Mode==4)
        {
            for(int i=0;i<10;i++)
            {
                if(Cro_Isl[i]==0)
                {
                    ips200_show_string(2*8,(i+10)*16,"NUll");
                }
                if(Cro_Isl[i]==1)
                {
                    ips200_show_string(2*8,(i+10)*16,"Island_S");
                }
                if(Cro_Isl[i]==2)
                {
                    ips200_show_string(2*8,(i+10)*16,"Island_M");
                }
                if(Cro_Isl[i]==3)
                {
                    ips200_show_string(2*8,(i+10)*16,"Island_L");
                }
                if(Cro_Isl[i]==4)
                {
                    ips200_show_string(2*8,(i+10)*16,"Cross_S");
                }
                if(Cro_Isl[i]==5)
                {
                    ips200_show_string(2*8,(i+10)*16,"Cross_R");
                }
                if(Cro_Isl[i]==6)
                {
                    ips200_show_string(2*8,(i+10)*16,"Cross_L");
                }
            }
            for(int i=10;i<15;i++)
            {
                if(Cro_Isl[i]==0)
                {
                    ips200_show_string(124,i*16,"NUll");
                }
                if(Cro_Isl[i]==1)
                {
                    ips200_show_string(124,i*16,"Island_S");
                }
                if(Cro_Isl[i]==2)
                {
                    ips200_show_string(124,i*16,"Island_M");
                }
                if(Cro_Isl[i]==3)
                {
                    ips200_show_string(124,i*16,"Island_L");
                }
                if(Cro_Isl[i]==4)
                {
                    ips200_show_string(124,i*16,"Cross_S");
                }
                if(Cro_Isl[i]==5)
                {
                    ips200_show_string(124,i*16,"Cross_R");
                }
                if(Cro_Isl[i]==6)
                {
                    ips200_show_string(124,i*16,"Cross_L");
                }
            }
        }
        else if(Isp_Mode==5)
        {
            for(int i=0;i<10;i++)
            {
                ips200_show_int(16,(i+10)*16,CI_Speed[i],3);
            }
            for(int i=10;i<15;i++)
            {
                ips200_show_int(124,i*16,CI_Speed[i],3);
            }
        }
    }

}




