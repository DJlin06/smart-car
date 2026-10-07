/*
 * UI.c
 *
 *  Created on: 2025年7月31日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
int Mode = 0; //模式选择
int Isp_Mode = 0;
int Mode_change = 0;
int Mode_Stay = 0;
int Isp_Mode_Stay = 0;
int32 Fuya = 0;
int IO_Mode_Key_Push_Short()
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
int IO_Mode_Key_Push_Long()
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
int IO_ADD_Key_Push()
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
int IO_SUB_Key_Push()
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
int IO_END_Key_Push()
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
void Key_Control()
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
            if(Isp_Mode==2)
                Isp_Mode = 0;
            ips200_show_string(8*9,8*16,"                    ");//清除
            if(Isp_Mode==0)
            {
                ips200_clear();
                ips200_show_string(0,8*16,"Isp_Mode:");
                ips200_show_string(8*9,8*16,"                    ");//清除
                ips200_show_string(72,8*16,"original");
                ips200_show_string(0,9*16,"Cen1_X&Y:");//j*8
                ips200_show_string(0,10*16,"Cen2_X&Y:");//j*8
                ips200_show_string(0,11*16,"error_Y:");
                ips200_show_string(0,12*16,"Error:");
                ips200_show_string(0,13*16,"Instance:");
                ips200_show_string(0,14*16,"Base&True_Speed:");
            }
            else if(Isp_Mode==1)
            {
                ips200_clear();
                ips200_show_string(0,8*16,"Isp_Mode:");
                ips200_show_string(8*9,8*16,"                    ");//清除
                ips200_show_string(72,8*16,"PID_select");
                ips200_show_string(40,9*16,"sp_kp");
                ips200_show_string(0,9*16,"Mode:");//清除
                ips200_show_string(0,10*16,"sp_kp:");//清除
                ips200_show_string(0,11*16,"sp_ki:");//清除
                ips200_show_string(0,12*16,"sp_kd:");//清除
                ips200_show_string(0,13*16,"Base_speed:");//清除
                ips200_show_string(0,14*16,"M_KP:");//清除
                ips200_show_string(0,15*16,"M_KI:");//清除
                ips200_show_string(0,16*16,"M_KD:");//清除
                ips200_show_string(0,17*16,"Fuya:");//清除
            }
        }
        if(IO_END_Key_Push())
        {
            Isp_Mode_Stay = 1;
            Mode_change = 0;
            Mode_Stay = 0;
            Mode = 0;
        }
    }
    if(Isp_Mode==1&&Mode_change==0&&Mode_Stay==0)
    {
        if(IO_Mode_Key_Push_Short())
        {
            Mode+=1;
            if(Mode==8)
            {
                Mode=0;
            }
            ips200_show_string(40,9*16,"                    ");//清除
            if(Mode==0)
                ips200_show_string(40,9*16,"sp_kp");
            else if(Mode==1)
                ips200_show_string(40,9*16,"sp_ki");
            else if(Mode==2)
                ips200_show_string(40,9*16,"sp_kd");
            else if(Mode==3)
                ips200_show_string(40,9*16,"Base_speed");
            else if(Mode==4)
                ips200_show_string(40,9*16,"M_KP");
            else if(Mode==5)
                ips200_show_string(40,9*16,"M_KI");
            else if(Mode==6)
                ips200_show_string(40,9*16,"M_KD");
            else if(Mode==7)
                ips200_show_string(40,9*16,"Fuya");
        }

        if(IO_ADD_Key_Push())
        {
            if(Mode_Stay==0)
            {
                if(Mode==0)
                {
                    sp_KP+=0.01;
                }
                else if(Mode==1)
                {
                    sp_KI+=0.01;
                }
                else if(Mode==2)
                {
                    sp_KD+=0.01;
                }
                else if(Mode==3)
                {
                    base_speed+=5;
                }
                else if(Mode==4)
                {
                    Cha_KP+=0.05;
                }
                else if(Mode==5)
                {
                    Cha_KI+=0.01;
                }
                else if(Mode==6)
                {
                    Cha_KD+=0.05;
                }
                else if(Mode==7)
                {
                    Fuya+=10;
                }
            }
        }

        if(IO_SUB_Key_Push())
        {
            if(Mode_Stay==0)
            {
                if(Mode==0)
                {
                    sp_KP-=0.01;
                }
                else if(Mode==1)
                {
                    sp_KI-=0.01;
                }
                else if(Mode==2)
                {
                    sp_KD-=0.01;
                }
                else if(Mode==3)
                {
                    base_speed-=5;
                }
                else if(Mode==4)
                {
                    Cha_KP-=0.05;
                }
                else if(Mode==5)
                {
                    Cha_KI-=0.01;
                }
                else if(Mode==6)
                {
                    Cha_KD-=0.05;
                }
                else if(Mode==7)
                {
                    Fuya-=10;
                }
            }
        }

        if(IO_END_Key_Push())
        {
            Mode_Stay = 1;
            Isp_Mode_Stay = 0;
        }
    }
    if(IO_END_Key_Push())
    {
        Mode_Stay = 1;
        Isp_Mode_Stay = 1;
        Flash_write();
        Flash_read();
        ness_pressure = Fuya;
    }

}
void Value_isp()
{
    if(Isp_flag)
    {

        Isp_flag = 0;
        if(Isp_Mode==0)
        {
            //ips200_displayimage03x(mt9v03x_image[0], MT9V03X_W, MT9V03X_H); //原图像
            ips200_draw_line(centroid_x0,centroid_y0,centroid_x1,centroid_y1,RGB565_RED);
            ips200_show_float(72,9*16,centroid_x0,3,2);
            ips200_show_float(150,9*16,centroid_y0,3,2);
            ips200_show_float(72,10*16,centroid_x1,3,2);
            ips200_show_float(150,10*16,centroid_y1,3,2);
            ips200_show_float(64,11*16,error_Y,3,2);
            ips200_show_int(48,12*16,error,3);
            ips200_show_float(72,13*16,error_instance,3,2);
            ips200_show_int(128,14*16,base_speed,3);
            ips200_show_int(200,14*16,Speed,3);
        }
        else if(Isp_Mode==1)
        {
            ips200_show_float(6*8,10*16,sp_KP,3,2);
            ips200_show_float(6*8,11*16,sp_KI,3,2);
            ips200_show_float(6*8,12*16,sp_KD,3,2);
            ips200_show_int(11*8,13*16,base_speed,3);
            ips200_show_float(7*8,14*16,Cha_KP,3,2);
            ips200_show_float(7*8,15*16,Cha_KI,3,2);
            ips200_show_float(7*8,16*16,Cha_KD,3,2);
            ips200_show_int(5*8,17*16,Fuya,4);
        }
        ips200_displayimage03x(bin_image[0], MT9V03X_W, MT9V03X_H/3);   //二值化图像

    }

}




