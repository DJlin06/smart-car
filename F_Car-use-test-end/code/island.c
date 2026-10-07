/*
 * island.c
 *
 *  Created on: 2025年5月7日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
float FJ_Angle = 0;
uint8 Left_Up_Guai[2]={0};    //四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
uint8 Right_Up_Guai[2]={0};   //四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
uint8 Stardard_Road[MT9V03X_H]={
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        8, 8, 8, 9, 9, 9, 10,10,10,10,
        10,10,11,12,13,14,14,15,15,16,
        17,18,18,19,19,20,21,22,22,23,
        24,25,25,26,27,28,29,29,30,31,
        32,33,34,34,35,36,37,37,38,39,
        40,40,41,42,43,43,44,45,46,47,
        47,48,49,49,50,51,51,52,53,54,
        54,55,56,56,57,58,58,59,60,61,
        61,61,62,63,64,65,66,67,67,68,
        69,69,70,71,71,72,73,74,74,75
};
/*-------------------------------------------------------------------------------------------------------------------
  @brief     环岛检测
  @param     null
  @return    null
  Sample     Island_Detect(void);
  @note      利用四个拐点判别函数，单调性改变函，连续性数撕裂点，分为8步
-------------------------------------------------------------------------------------------------------------------*/
int island_state_5_down[2]={0};//状态5时即将离开环岛，左右边界边最低点，[0]存y，第某行，{1}存x，第某列
int island_state_3_up[2]={0};//状态3时即将进入环岛用，左右上面角点[0]存y，第某行，{1}存x，第某列
int left_down_guai[2]={0};//四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
int right_down_guai[2]={0};//四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
int monotonicity_change_line[2];//单调性改变点坐标，[0]寸某行，[1]寸某列
int monotonicity_change_left_flag=0;//不转折是0
int monotonicity_change_right_flag=0;//不转折是0
int continuity_change_right_flag=0; //连续是0
int continuity_change_left_flag=0;  //连续是0
void Island_Detect()
{
    static float k=0;//3和5状态的k
    monotonicity_change_line[2];//单调性改变点坐标，[0]寸某行，[1]寸某列
    monotonicity_change_left_flag=0;//不转折是0
    monotonicity_change_right_flag=0;//不转折是0
    continuity_change_right_flag=0; //连续是0
    continuity_change_left_flag=0;  //连续是0
    //以下是常规判断法
    continuity_change_left_flag=Continuity_Change_Left(MT9V03X_H-1-5,35);//连续性判断
    continuity_change_right_flag=Continuity_Change_Right(MT9V03X_H-1-5,35);
    monotonicity_change_right_flag=Monotonicity_Change_Right(MT9V03X_H-1-10,35);
    monotonicity_change_left_flag=Monotonicity_Change_Left(MT9V03X_H-1-10,35);
    if(Cross_state==0&&Island_State==0)
    {
        //continuity_change_left_flag=Continuity_Change_Left(MT9V03X_H-1-5,10);//连续性判断
        //continuity_change_right_flag=Continuity_Change_Right(MT9V03X_H-1-5,10);

        if(Left_Island_Flag==0)//左环
        {
            //正入环岛
            if((monotonicity_change_right_flag<=45&& //右边是单调的
               continuity_change_left_flag>=60&& //左边是不连续的
               continuity_change_left_flag>monotonicity_change_left_flag&&
               continuity_change_right_flag<=45&& //左环岛右边是连续的
               Left_Lost_Time>10&& //左边丢线很多
               Left_Lost_Time<=70&& //也不能全丢了
               Right_Lost_Time<=10&&//右边丢线较少
               Search_Stop_Line>=75&& //搜索截止行看到很远
               Boundry_Start_Left>=MT9V03X_H-20&&Boundry_Start_Right>=MT9V03X_H-20&& //边界起始点靠下
               Both_Lost_Time<=15)//双边丢线少
               )
            {
                left_down_guai[0]=Find_Left_Down_Point(MT9V03X_H-1,35);//找左下角点
                if(left_down_guai[0]>=60)//条件1很松，在这里判断拐点，位置不对，则是误判，跳出
                {
                    Island_State=1;
                    Left_Island_Flag=1;
                }
                else//误判，归零
                {
                    Island_State=0;
                    Left_Island_Flag=0;
                }
            }
            //弯内接环岛
            else if(
                    (monotonicity_change_right_flag<=45&& //右边是单调的
                     continuity_change_left_flag>60&& //左边是不连续的
                     continuity_change_left_flag>monotonicity_change_left_flag+15&&
                     continuity_change_right_flag<=45&& //左环岛右边是连续的
                     Left_Lost_Time<15&& //左边丢线很多
                     Left_Lost_Time<=70&& //也不能全丢了
                     Right_Lost_Time<=10&&//右边丢线较少
                     Search_Stop_Line>=80&& //搜索截止行看到很远
                     Boundry_Start_Left>=MT9V03X_H-10&&Boundry_Start_Right>=MT9V03X_H-10&& //边界起始点靠下
                     Both_Lost_Time<=10))
            {
                //环岛补直线
                float k=(float)(Left_Line[continuity_change_left_flag + 6]-Left_Line[continuity_change_left_flag+1])/5.0;//这里的k是1/斜率
                int j=(int)(monotonicity_change_left_flag - continuity_change_left_flag)*k+Left_Line[continuity_change_left_flag];
                if(abs(j-Left_Line[monotonicity_change_left_flag])<=10)
                {
                    Island_State=1;
                    Left_Island_Flag=1;
                }
                k=(float)(Left_Line[monotonicity_change_left_flag + 8]-Left_Line[monotonicity_change_left_flag+3])/5.0;//这里的k是1/斜率
                j=(int)(continuity_change_left_flag - monotonicity_change_left_flag - 3)*k+Left_Line[monotonicity_change_left_flag+3];
                int m = Monotonicity_Change_Left(continuity_change_left_flag,continuity_change_left_flag-15);
                if(m>=continuity_change_left_flag-15&&abs(j-Left_Line[m])<=10)
                {
                    Island_State=1;
                    Left_Island_Flag=1;
                }
            }
            //弯外接环岛
            else if(
                    (monotonicity_change_right_flag<=45&& //右边是单调的
                     continuity_change_left_flag>45&& //左边是不连续的
                     continuity_change_left_flag<monotonicity_change_left_flag-5&&
                     continuity_change_right_flag<=45&& //左环岛右边是连续的
                     Left_Lost_Time<15&& //左边丢线很多
                     Left_Lost_Time<=70&& //也不能全丢了
                     Right_Lost_Time>=45&&//右边丢线较少
                     Search_Stop_Line>=80&& //搜索截止行看到很远
                     Boundry_Start_Left>=MT9V03X_H-10 //边界起始点靠下
                     ))
            {
                float k=(float)(Left_Line[monotonicity_change_left_flag + 8]-Left_Line[monotonicity_change_left_flag+3])/5.0;//这里的k是1/斜率
                int j=(int)(continuity_change_left_flag - monotonicity_change_left_flag - 3)*k+Left_Line[monotonicity_change_left_flag+3];
                int m = Monotonicity_Change_Left(continuity_change_left_flag,continuity_change_left_flag-15);
                if(m>=continuity_change_left_flag-15&&abs(j-Left_Line[m])<=10)
                {
                    Island_State=1;
                    Left_Island_Flag=1;
                }
            }
            else if(monotonicity_change_right_flag<=45&& //右边是单调的
                    //continuity_change_left_flag<monotonicity_change_left_flag-1&& //左边是不连续的
                    continuity_change_left_flag >= 45 &&
                    monotonicity_change_left_flag>=55&&
                    Left_Line[monotonicity_change_left_flag]>Left_Line[continuity_change_left_flag]&&
                    continuity_change_right_flag<=45&& //左环岛右边是连续的
                    Left_Lost_Time>=20&& //左边丢线很多
                    Left_Lost_Time<=70&& //也不能全丢了
                    Right_Lost_Time<=20&&//右边丢线较少
                    Search_Stop_Line>=80&& //搜索截止行看到很远
                    Boundry_Start_Left < MT9V03X_H-10&&Boundry_Start_Right>=MT9V03X_H-15&& //边界起始点靠下
                    Both_Lost_Time<=20)//双边丢线少)
            {
                gpio_set_level(P02_5,1);
                Island_State=2;
                Left_Island_Flag=1;
            }
        }
        if(Right_Island_Flag==0&&Left_Island_Flag==0)//右环
        {
            //正入环岛
            if(monotonicity_change_left_flag<=45&&
               continuity_change_left_flag<=45&& //右环岛左边是连续的
               continuity_change_right_flag>=60&& //右边是不连续的
               continuity_change_right_flag>monotonicity_change_right_flag&&
               Right_Lost_Time>10&&           //右丢线多
               Right_Lost_Time<=70&&           //右丢线不能太多
               Left_Lost_Time<=10&&            //左丢线少
               Search_Stop_Line>=75&& //搜索截止行看到很远
               Boundry_Start_Left>=MT9V03X_H-20&&Boundry_Start_Right>=MT9V03X_H-20&& //边界起始点靠下
               Both_Lost_Time<=15)
            {
                right_down_guai[0]=Find_Right_Down_Point(MT9V03X_H-1,35);//右下点
                if(right_down_guai[0]>=60)//条件1很松，在这里加判拐点，位置不对，则是误判，跳出
                {
                    Island_State=1;
                    Right_Island_Flag=1;
                }
                else
                {
                    Island_State=0;
                    Right_Island_Flag=0;
                }
            }
            //弯内接环岛
            else if(
                    (monotonicity_change_left_flag<=45&& //右边是单调的
                     continuity_change_right_flag>60&& //左边是不连续的
                     continuity_change_right_flag>monotonicity_change_right_flag+15&&
                     continuity_change_left_flag<=45&& //左环岛右边是连续的
                     Right_Lost_Time<15&& //左边丢线很多
                     Right_Lost_Time<=70&& //也不能全丢了
                     Left_Lost_Time<=10&&//右边丢线较少
                     Search_Stop_Line>=80&& //搜索截止行看到很远
                     Boundry_Start_Right>=MT9V03X_H-10&&Boundry_Start_Left>=MT9V03X_H-10&& //边界起始点靠下
                     Both_Lost_Time<=10))
            {
                //环岛内补直线
                float k=(float)(Right_Line[continuity_change_right_flag + 6]-Right_Line[continuity_change_right_flag+1])/5.0;//这里的k是1/斜率
                int j=(int)(monotonicity_change_right_flag - continuity_change_right_flag)*k+Right_Line[continuity_change_right_flag];
                if(abs(j-Right_Line[monotonicity_change_right_flag])<=5)
                {
                    Island_State=1;
                    Right_Island_Flag=1;
                }
                k=(float)(Right_Line[monotonicity_change_right_flag + 8]-Right_Line[monotonicity_change_right_flag+3])/5.0;//这里的k是1/斜率
                j=(int)(continuity_change_right_flag - monotonicity_change_right_flag - 3)*k+Right_Line[monotonicity_change_right_flag+3];
                int m = Monotonicity_Change_Right(continuity_change_right_flag,continuity_change_right_flag-15);
                if(m>=continuity_change_right_flag-15&&abs(j-Right_Line[m])<=10)
                {
                    Island_State=1;
                    Right_Island_Flag=1;
                }
            }
            //弯外接环岛
            else if(
                    (monotonicity_change_left_flag<=45&& //右边是单调的
                     continuity_change_right_flag>45&& //左边是不连续的
                     continuity_change_right_flag<monotonicity_change_right_flag-5&&
                     continuity_change_left_flag<=45&& //左环岛右边是连续的
                     Right_Lost_Time<15&& //左边丢线很多
                     Right_Lost_Time<=70&& //也不能全丢了
                     Left_Lost_Time>=45&&//右边丢线较少
                     Search_Stop_Line>=80&& //搜索截止行看到很远
                     Boundry_Start_Right>=MT9V03X_H-10 //边界起始点靠下
                     ))
            {
                float k=(float)(Right_Line[monotonicity_change_right_flag + 8]-Right_Line[monotonicity_change_right_flag+3])/5.0;//这里的k是1/斜率
                int j=(int)(continuity_change_right_flag - monotonicity_change_right_flag - 3)*k+Right_Line[monotonicity_change_right_flag+3];
                int m = Monotonicity_Change_Right(continuity_change_right_flag,continuity_change_right_flag-15);
                if(m>=continuity_change_right_flag-15&&abs(j-Right_Line[m])<=10)
                {
                    Island_State=1;
                    Right_Island_Flag=1;
                }
            }
            else if(monotonicity_change_left_flag<=45&&
                    continuity_change_left_flag<=45&& //右环岛左边是连续的
                    //continuity_change_right_flag<monotonicity_change_right_flag-1&&
                    monotonicity_change_right_flag>=55&&
                    Right_Line[monotonicity_change_right_flag]<Right_Line[continuity_change_right_flag]&&
                    continuity_change_right_flag>=35&& //右边是不连续的
                    Right_Lost_Time>=20&&           //右丢线多
                    Right_Lost_Time<=70&&           //右丢线不能太多
                    Left_Lost_Time<=20&&            //左丢线少
                    Search_Stop_Line>=80&& //搜索截止行看到很远
                    Boundry_Start_Left>=MT9V03X_H-15&&Boundry_Start_Right<=MT9V03X_H-10&& //边界起始点靠下
                    Both_Lost_Time<=20)
            {
                gpio_set_level(P02_5,1);
                Island_State=2;
                Right_Island_Flag=1;
            }
        }
    }



    if(Left_Island_Flag==1)//1状态下拐点还在，没丢线
    {
        if(Island_State==1)
        {
            gpio_set_level(P02_5,1);
            monotonicity_change_line[0]=Monotonicity_Change_Left(90,40);//寻找单调性改变点
            monotonicity_change_line[1]=Left_Line[monotonicity_change_line[0]];
            Left_Add_Line((int)(monotonicity_change_line[1]*0.25),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);//补左线
            float k2=(float)(Left_Line[monotonicity_change_line[0] + 6]-Left_Line[monotonicity_change_line[0]+1])/5.0;//这里的k是1/斜率
            for(int i=monotonicity_change_line[0];i>=MT9V03X_H-Search_Stop_Line;i--)
            {
                Left_Line[i]=(int)(i - monotonicity_change_line[0])*k2+Left_Line[monotonicity_change_line[0]];//(x=(y-y1)*k+x1),点斜式变形
                if(Left_Line[i]>=MT9V03X_W-1)
                {
                    Left_Line[i]=MT9V03X_W-1;
                }
                else if(Left_Line[i]<=0)
                {
                    Left_Line[i]=0;
                }
            }
            if(Boundry_Start_Left<110&&Boundry_Start_Left>=60&&monotonicity_change_line[0]>=50)//下方当丢线时候进2
            {
                Island_State=2;
            }
        }


        else if(Island_State==2)//下方角点消失，2状态时下方应该是丢线，上面是弧线
        {
            gpio_toggle_level(P02_5);
            monotonicity_change_line[0]=Monotonicity_Change_Left(90,40);//寻找单调性改变点
            monotonicity_change_line[1]=Left_Line[monotonicity_change_line[0]];
            Left_Add_Line((int)(monotonicity_change_line[1]*0.25),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);//补左线
            float k2=(float)(Left_Line[monotonicity_change_line[0] + 6]-Left_Line[monotonicity_change_line[0]+1])/5.0;//这里的k是1/斜率
            for(int i=monotonicity_change_line[0];i>=MT9V03X_H-Search_Stop_Line;i--)
            {
                Left_Line[i]=(int)(i - monotonicity_change_line[0])*k2+Left_Line[monotonicity_change_line[0]];//(x=(y-y1)*k+x1),点斜式变形
                if(Left_Line[i]>=MT9V03X_W-1)
                {
                    Left_Line[i]=MT9V03X_W-1;
                }
                else if(Left_Line[i]<=0)
                {
                    Left_Line[i]=0;
                }
            }
            if(Boundry_Start_Left>=MT9V03X_H-25&&monotonicity_change_line[0]>60)//当圆弧靠下时候，进3
            {
                Island_State=3;//最长白列寻找范围也要改，见camera.c
                Left_Island_Flag=1;
            }
        }
        else if(Island_State==3)//3状态准备进环，寻找上拐点，连线
        {
            if(k!=0)
            {
                gpio_set_level(P02_5,0);
                K_Draw_Line(k,MT9V03X_W-1,MT9V03X_H-10,35);//k是刚刚算出来的，静态变量存着
                Longest_White_Column();//刷新赛道数据
            }
            else
            {
                monotonicity_change_line[0]=Monotonicity_Change_Left(90,40);//寻找单调性改变点
                monotonicity_change_line[1]=Left_Line[monotonicity_change_line[0]];
                Left_Up_Guai[0]=Find_Left_Up_Point(80,40);//找左上拐点
                Left_Up_Guai[1]=Left_Line[Left_Up_Guai[0]];
                Left_Add_Line((int)(monotonicity_change_line[1]*0.25),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);//补左线
                float k2=(float)(Left_Line[monotonicity_change_line[0] + 6]-Left_Line[monotonicity_change_line[0]+1])/5.0;//这里的k是1/斜率
                for(int i=monotonicity_change_line[0];i>=MT9V03X_H-Search_Stop_Line;i--)
                {
                    Left_Line[i]=(int)(i - monotonicity_change_line[0])*k2+Left_Line[monotonicity_change_line[0]];//(x=(y-y1)*k+x1),点斜式变形
                    if(Left_Line[i]>=MT9V03X_W-1)
                    {
                        Left_Line[i]=MT9V03X_W-1;
                    }
                    else if(Left_Line[i]<=0)
                    {
                        Left_Line[i]=0;
                    }
                }

                /*if (Left_Up_Guai[0]<5)//此处为了防止误判，如果经常从3状态归零，建议修改此处判断条件
                {
                    Island_State=0;
                    Left_Island_Flag=0;
                }
                */
                if(k==0&&50<=Left_Up_Guai[0]&&Left_Up_Guai[0]<=75&&(18<Left_Up_Guai[1]&&Left_Up_Guai[1]<130))//拐点出现在一定范围内，认为是拐点出现
                {
                    island_state_3_up[0]= Left_Up_Guai[0];
                    island_state_3_up[1]= Left_Up_Guai[1];
                    k=(float)((float)(MT9V03X_H-10-island_state_3_up[0])/(float)(MT9V03X_W-1-island_state_3_up[1]));
                    if(k<0)
                        {
                            k=0;//防止误判
                        }
                    else
                        {
                        K_Draw_Line(k,MT9V03X_W-1,MT9V03X_H-10,35);//记录下第一次上点出现时位置，针对这个环岛拉一条死线，入环
                        Longest_White_Column();//刷新赛道数据
                        }
                }

            }
            /*Left_Up_Guai[0]=Find_Left_Up_Point(60,5);//找左上拐点
            Left_Up_Guai[1]=Left_Line[Left_Up_Guai[0]];
           for(int i = MT9V03X_H - 1; i >= Left_Up_Guai[0]; i--)
           {
               image_two_value[i][Left_Line[i]+(Stardard_Road[i]*2)+1]=0;
               image_two_value[i][Left_Line[i]+(Stardard_Road[i]*2)]=0;
               image_two_value[i][Left_Line[i]+(Stardard_Road[i]*2)-1]=0;
           }
           Longest_White_Column();//刷新边界数据
           */
            if((Island_State==3)&&(abs(FJ_Angle)>=150))//纯靠陀螺仪积分入环
            {
                //k=0;//斜率清零
                Longest_White_Column();//刷新赛道数据
                Island_State=4;//这一步时候顺便调整了最长白列的搜索范围
            }
        }
        else if(Island_State==4)//状态4已经在里面
        {
            if(abs(FJ_Angle)>200)//积分200度以后在打开出环判断
            {
                monotonicity_change_line[0]=Monotonicity_Change_Right(MT9V03X_H-10,35);//单调性改变
                monotonicity_change_line[1]=Right_Line[monotonicity_change_line[0]];
                if((Island_State==4)&&(50<=monotonicity_change_line[0]&&monotonicity_change_line[0]<=80&&monotonicity_change_line[1]>=55))//单调点靠下，进去5
                {//monotonicity_change_line[1]>=90&&
                    island_state_5_down[0]=MT9V03X_H-1;
                    island_state_5_down[1]=Right_Line[MT9V03X_H-1];
                    if(Cro_Isl[CIF]==3)
                    {
                        island_state_5_down[0]=MT9V03X_H-1;
                        island_state_5_down[1]=Right_Line[MT9V03X_H-1];
                    }
                    k=(float)((float)(island_state_5_down[0]-monotonicity_change_line[0])/(float)(island_state_5_down[1]-monotonicity_change_line[1]));
                    K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],35);//和状态3一样，记住斜率
                    Island_State=5;
                }
            }
            else if(abs(FJ_Angle)>=360)//陀螺仪积分足够强制出环
            {
                Island_State=8;//基本上找到拐点就去8
            }
        }
        else if(Island_State==5)//出环
        {
            gpio_set_level(P02_5,1);
            K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],35);
            if((Island_State==5)&&(Boundry_Start_Right<=MT9V03X_H-20))//右边先丢线
            {
                Island_State=6;
            }
            else if(abs(FJ_Angle)>=360)//陀螺仪积分足够强制出环
            {
                Island_State=8;//基本上找到拐点就去8
            }
        }
        else if(Island_State==6)//还在出
        {
            K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],35);
            if((Island_State==6)&&(Boundry_Start_Right>MT9V03X_H-20||abs(FJ_Angle)>=320))//右边不丢线
            {//
                k=0;
                Island_State=7;
            }
            else if(abs(FJ_Angle)>=360)//陀螺仪积分足够强制出环
            {
                Island_State=8;//基本上找到拐点就去8
            }
        }
        else if(Island_State==7)//基本出去了，在寻找拐点，准备离开环岛状态
        {
            gpio_set_level(P02_5,0);
            Left_Up_Guai[0]=Find_Left_Up_Point(MT9V03X_H-10,35);//获取左上点坐标，坐标点合理去8
            Left_Up_Guai[1]=Left_Line[Left_Up_Guai[0]];
            if((Island_State==7)&&(Left_Up_Guai[1]<=MT9V03X_W/2)&&(5<=Left_Up_Guai[0]&&Left_Up_Guai[0]<=MT9V03X_H-20)||abs(FJ_Angle)>=360)//注意这里，对横纵坐标都有要求
            {
                Island_State=8;//基本上找到拐点就去8
            }
        }
        else if(Island_State==8)//连线，出环最后一步
        {
            gpio_set_level(P02_5,1);
            Left_Up_Guai[0]=Find_Left_Up_Point(MT9V03X_H-1,35);//获取左上点坐标
            Left_Up_Guai[1]=Left_Line[Left_Up_Guai[0]];
            Lengthen_Left_Boundry(Left_Up_Guai[0]-1,MT9V03X_H-1);
            if((Island_State==8)&&(Left_Up_Guai[0]>=MT9V03X_H-20||(Left_Up_Guai[0]<10&&Boundry_Start_Left>=MT9V03X_H-20)))//当拐点靠下时候，认为出环了，环岛结束
            {//要么拐点靠下，要么拐点丢了，切下方不丢线，认为环岛结束了
                Island_State=9;//8时候环岛基本结束了，为了防止连续判环，8后会进9，大概几十毫秒后归零，

            }
        }
        else if(Island_State==9){//延时500ms清零
            if(interrupt>=50){
                gpio_set_level(P02_5,0);
                Island_State=0;
                Left_Island_Flag=0;
                interrupt=0;
                FJ_Angle=0;//数据清零
                CIF++;

            }

        }
    }
    else if(Right_Island_Flag==1)
    {
        if(Island_State==1)//1状态下拐点还在，没丢线
        {
            gpio_set_level(P02_5,1);
            monotonicity_change_line[0]=Monotonicity_Change_Right(90,40);//单调性改变
            monotonicity_change_line[1]=Right_Line[monotonicity_change_line[0]];
            Right_Add_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*0.25)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
            float k1=(float)(Right_Line[monotonicity_change_line[0] + 6]-Right_Line[monotonicity_change_line[0]+1])/5.0;//这里的k是1/斜率
            for(int i=monotonicity_change_line[0];i>=MT9V03X_H-Search_Stop_Line;i--)
            {
                Right_Line[i]=(int)(i - monotonicity_change_line[0])*k1+Right_Line[monotonicity_change_line[0]];//(x=(y-y1)*k+x1),点斜式变形
                if(Right_Line[i]>=MT9V03X_W-1)
                {
                    Right_Line[i]=MT9V03X_W-1;
                }
                else if(Right_Line[i]<=0)
                {
                    Right_Line[i]=0;
                }
        }
            if(Boundry_Start_Right<=110&&Boundry_Start_Right >= 60&&monotonicity_change_line[0]>=50)//右下角先丢线
            {
                Island_State=2;
            }
        }
        else if(Island_State==2)//2状态下方丢线，上方即将出现大弧线
        {
            gpio_toggle_level(P02_5);
            monotonicity_change_line[0]=Monotonicity_Change_Right(90,40);//单调性改变
            monotonicity_change_line[1]=Right_Line[monotonicity_change_line[0]];
            Right_Add_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*0.25)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
            float k1=(float)(Right_Line[monotonicity_change_line[0] + 6]-Right_Line[monotonicity_change_line[0]+1])/5.0;//这里的k是1/斜率
            for(int i=monotonicity_change_line[0];i>=MT9V03X_H-Search_Stop_Line;i--)
            {
                Right_Line[i]=(int)(i - monotonicity_change_line[0])*k1+Right_Line[monotonicity_change_line[0]];//(x=(y-y1)*k+x1),点斜式变形
                if(Right_Line[i]>=MT9V03X_W-1)
                {
                    Right_Line[i]=MT9V03X_W-1;
                }
                else if(Right_Line[i]<=0)
                {
                    Right_Line[i]=0;
                }
        }
//            if(Island_State==2&&(Boundry_Start_Right>=MT9V03X_H-10))//右下角再不丢线进3
            if(Boundry_Start_Right>=MT9V03X_H-25&&monotonicity_change_line[0]>60)//右下角再不丢线进3
            {
                Island_State=3;//下方丢线，说明大弧线已经下来了
                Right_Island_Flag=1;
            }
        }
        else if(Island_State==3)//下面已经出现大弧线，且上方出现角点
        {
            if(k!=0)//将角点与下方连接，画一条死线
            {
                gpio_set_level(P02_5,0);
                K_Draw_Line(k,0,MT9V03X_H-10,35);
                Longest_White_Column();//刷新赛道数据
            }
            else
            {
                monotonicity_change_line[0]=Monotonicity_Change_Right(90,40);//单调性改变
                monotonicity_change_line[1]=Right_Line[monotonicity_change_line[0]];
                if(monotonicity_change_line[0]>50)
                    Right_Up_Guai[0]=Find_Right_Up_Point(monotonicity_change_line[0],40);//找右上拐点
                else
                    Right_Up_Guai[0]=Find_Right_Up_Point(80,40);//找右上拐点
                Right_Up_Guai[1]=Right_Line[Right_Up_Guai[0]];
                Right_Add_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*0.25)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
                float k1=(float)(Right_Line[monotonicity_change_line[0] + 6]-Right_Line[monotonicity_change_line[0]+1])/5.0;//这里的k是1/斜率
                for(int i=monotonicity_change_line[0];i>=MT9V03X_H-Search_Stop_Line;i--)
                {
                    Right_Line[i]=(int)(i - monotonicity_change_line[0])*k1+Right_Line[monotonicity_change_line[0]];//(x=(y-y1)*k+x1),点斜式变形
                    if(Right_Line[i]>=MT9V03X_W-1)
                    {
                        Right_Line[i]=MT9V03X_W-1;
                    }
                    else if(Right_Line[i]<=0)
                    {
                        Right_Line[i]=0;
                    }
            }
                /*
                if(Right_Up_Guai[0]<10)//这里改过，此处为了防止环岛误判，如果经常出现环岛3归零，请修改此处判断条件
                {
                    Island_State=0;
                    Right_Island_Flag=0;
                }
                */
                if(k==0&&(45<=Right_Up_Guai[0]&&Right_Up_Guai[0]<=75)&&(68<Right_Up_Guai[1]&&Right_Up_Guai[1]<160))//找第一个符合条件的角点，连线
                {
                    island_state_3_up[0]= Right_Up_Guai[0];
                    island_state_3_up[1]= Right_Up_Guai[1];
                    k=(float)((float)(MT9V03X_H-10-island_state_3_up[0])/(float)(0-island_state_3_up[1]));
                    if(k>0)
                    {
                        k=0;
                    }
                    else
                    {
                        K_Draw_Line(k,0,MT9V03X_H-10,35);
                        Longest_White_Column();//刷新赛道数据
                    }
                }
            }
            if((Island_State==3)&&(abs(FJ_Angle)>=150))//只依靠陀螺仪积分
            {
                //k=0;//斜率清零
                Longest_White_Column();//刷新赛道数据
                Island_State=4;
            }//记得去最长白列那边改一下，区分下左右环岛
        }
        else if(Island_State==4)//4状态完全进去环岛了
        {
            if(abs(FJ_Angle)>200)//环岛积分200度后再打开单调转折判断
            {
                monotonicity_change_line[0]=Monotonicity_Change_Left(MT9V03X_H-10,35);//单调性改变
                monotonicity_change_line[1]=Left_Line[monotonicity_change_line[0]];
                if((Island_State==4)&&(40<=monotonicity_change_line[0]&&monotonicity_change_line[0]<=80&&monotonicity_change_line[1]<=MT9V03X_W-55))//单调点靠下，进去5
                {//monotonicity_change_line[1]<=120&&
                   island_state_5_down[0]=MT9V03X_H-1;
                   island_state_5_down[1]=Left_Line[MT9V03X_H-1];//抓住第一次出现的斜率，定死
                   if(Cro_Isl[CIF]==3)
                   {
                       island_state_5_down[0]=MT9V03X_H-1;
                       island_state_5_down[1]=Left_Line[MT9V03X_H-1];
                   }
                   k=(float)((float)(island_state_5_down[0]-monotonicity_change_line[0])/(float)(island_state_5_down[1]-monotonicity_change_line[1]));
                   K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],35);
                   Island_State=5;
                }
            }
            else if(abs(FJ_Angle)>=360)
            {
                Island_State=8;//基本上找到拐点就去8
            }
        }
        else if(Island_State==5)//准备出环岛
        {
            gpio_set_level(P02_5,1);
            K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],35);
            if(Island_State==5&&Boundry_Start_Left<=MT9V03X_H-20)//左边先丢线
            {
                Island_State=6;
            }
            else if(abs(FJ_Angle)>=360)
            {
                Island_State=8;//基本上找到拐点就去8
            }
        }
        else if(Island_State==6)//继续出
        {
            K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],35);
            if((Island_State==6)&&(Boundry_Start_Left>MT9V03X_H-20||abs(FJ_Angle)>=320))
            {//
                k=0;
                Island_State=7;
            }
            else if(abs(FJ_Angle)>=360)
            {
                Island_State=8;//基本上找到拐点就去8
            }
        }
        else if(Island_State==7)//基本出环岛，找角点
        {
            gpio_set_level(P02_5,0);
            Right_Up_Guai[0]=Find_Right_Up_Point(MT9V03X_H-10,35);//获取左上点坐标，找到了去8
            Right_Up_Guai[1]=Right_Line[Right_Up_Guai[0]];
            if((Island_State==7)&&((Right_Up_Guai[1]>=MT9V03X_W/2&&(5<=Right_Up_Guai[0]&&Right_Up_Guai[0]<=MT9V03X_H-20)))||abs(FJ_Angle)>=360)//注意这里，对横纵坐标都有要求，因为赛道不一样，会意外出现拐点
            {//当角点位置合理时，进8
                Island_State=8;
            }
        }
        else if(Island_State==8)//环岛8
        {
            gpio_set_level(P02_5,1);
            Right_Up_Guai[0]=Find_Right_Up_Point(MT9V03X_H-1,35);//获取右上点坐标
            Right_Up_Guai[1]=Right_Line[Right_Up_Guai[0]];
            Lengthen_Right_Boundry(Right_Up_Guai[0]-1,MT9V03X_H-1);
            if((Island_State==8)&&(Right_Up_Guai[0]>=MT9V03X_H-20||(Right_Up_Guai[0]<10&&Boundry_Start_Left>=MT9V03X_H-20)))//当拐点靠下时候，认为出环了，环岛结束
            {//角点靠下，或者下端不丢线，认为出环了
                Island_State=9;
            }
        }
        else if(Island_State==9)
        {
            if(interrupt>=50)
            {
                gpio_set_level(P02_5,0);
                FJ_Angle=0;
                Right_Island_Flag=0;
                interrupt = 0;
                Island_State=0;
                CIF++;
            }
        }
    }

}


/*-------------------------------------------------------------------------------------------------------------------
  @brief     左赛道连续性检测
  @param     起始点，终止点
  @return    连续返回0，不连续返回断线出行数
  Sample     Continuity_Change_Left(int start,int end);
  @note      连续性的阈值设置为5，可更改
-------------------------------------------------------------------------------------------------------------------*/
int Continuity_Change_Left(int start,int end)//连续性阈值设置为5
{
    int i;
    int t;
    int continuity_change_flag=0;
    if(Left_Lost_Time>=0.7*MT9V03X_H||Search_Stop_Line<=5)//大部分都丢线，没必要判断了
       return 1;
    if(start>=MT9V03X_H-1-5)//数组越界保护
        start=MT9V03X_H-1-5;
    if(end<=5)
       end=5;
    if(start<end)//都是从下往上计算的，反了就互换一下
    {
       t=start;
       start=end;
       end=t;
    }

    for(i=start;i>=end;i--)
    {
       if(abs(Left_Line[i]-Left_Line[i-1])>=10)//连续判断阈值是5,可更改
       {
            continuity_change_flag=i;
            break;
       }
    }
    return continuity_change_flag;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右赛道连续性检测
  @param     起始点，终止点
  @return    连续返回0，不连续返回断线出行数
  Sample     continuity_change_flag=Continuity_Change_Right(int start,int end)
  @note      连续性的阈值设置为5，可更改
-------------------------------------------------------------------------------------------------------------------*/
int Continuity_Change_Right(int start,int end)
{
    int i;
    int t;
    int continuity_change_flag=0;
    if(Right_Lost_Time>=0.7*MT9V03X_H)//大部分都丢线，没必要判断了
       return 1;
    if(start>=MT9V03X_H-5)//数组越界保护
        start=MT9V03X_H-5;
    if(end<=5)
       end=5;
    if(start<end)//都是从下往上计算的，反了就互换一下
    {
       t=start;
       start=end;
       end=t;
    }

    for(i=start;i>=end;i--)
    {
        if(abs(Right_Line[i]-Right_Line[i-1])>=10)//连续性阈值是5，可更改
       {
            continuity_change_flag=i;
            break;
       }
    }
    return continuity_change_flag;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     左下角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Left_Down_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Left_Down_Point(int start,int end)//找四个角点，返回值是角点所在的行数
{
    int i,t;
    int left_down_line=0;
    if(Left_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有拐点判断的意义
       return left_down_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(start>=MT9V03X_H-1-5)//下面5行数据不稳定，不能作为边界点来判断，舍弃
        start=MT9V03X_H-1-5;
    if(end<=MT9V03X_H-Search_Stop_Line)
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)
       end=5;
    for(i=start;i>=end;i--)
    {
        if(left_down_line==0&&//只找第一个符合条件的点
           abs(Left_Line[i]-Left_Line[i+1])<=5&&//角点的阈值可以更改
           abs(Left_Line[i+1]-Left_Line[i+2])<=5&&
           abs(Left_Line[i+2]-Left_Line[i+3])<=5&&
              (Left_Line[i]-Left_Line[i-2])>=5&&
              (Left_Line[i]-Left_Line[i-3])>=10&&
              (Left_Line[i]-Left_Line[i-4])>=10)
        {
            left_down_line=i;//获取行数即可
            break;
        }
    }
    return left_down_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     左上角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Left_Up_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Left_Up_Point(int start,int end)//找四个角点，返回值是角点所在的行数
{
    int i,t;
    int left_up_line=0;
    if(Left_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有拐点判断的意义
       return left_up_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(end<=MT9V03X_H-Search_Stop_Line)//搜索截止行往上的全都不判
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)//及时最长白列非常长，也要舍弃部分点，防止数组越界
        end=5;
    if(start>=MT9V03X_H-1-5)
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(left_up_line==0&&//只找第一个符合条件的点
           abs(Left_Line[i]-Left_Line[i-1])<=5&&
           abs(Left_Line[i-1]-Left_Line[i-2])<=6&&
           abs(Left_Line[i-2]-Left_Line[i-3])<=7&&
              (Left_Line[i]-Left_Line[i+2])>=5&&
              (Left_Line[i]-Left_Line[i+3])>=10&&
              (Left_Line[i]-Left_Line[i+4])>=10)
        {
            left_up_line=i;//获取行数即可
            break;
        }
    }
    return left_up_line;//如果是MT9V03X_H-1，说明没有这么个拐点
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右下角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Right_Down_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Down_Point(int start,int end)//找四个角点，返回值是角点所在的行数
{
    int i,t;
    int right_down_line=0;
    if(Right_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有拐点判断的意义
        return right_down_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(start>=MT9V03X_H-1-5)//下面5行数据不稳定，不能作为边界点来判断，舍弃
        start=MT9V03X_H-1-5;
    if(end<=MT9V03X_H-Search_Stop_Line)
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)
       end=5;
    for(i=start;i>=end;i--)
    {
        if(right_down_line==0&&//只找第一个符合条件的点
           abs(Right_Line[i]-Right_Line[i+1])<=5&&//角点的阈值可以更改
           abs(Right_Line[i+1]-Right_Line[i+2])<=5&&
           abs(Right_Line[i+2]-Right_Line[i+3])<=5&&
              (Right_Line[i]-Right_Line[i-2])<=-5&&
              (Right_Line[i]-Right_Line[i-3])<=-10&&
              (Right_Line[i]-Right_Line[i-4])<=-10)
        {
            right_down_line=i;//获取行数即可
            break;
        }
    }
    return right_down_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右上角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Up_Point(int start,int end)//找四个角点，返回值是角点所在的行数
{
    int i,t;
    int right_up_line=0;
    if(Right_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有拐点判断的意义
        return right_up_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(end<=MT9V03X_H-Search_Stop_Line)//搜索截止行往上的全都不判
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)//及时最长白列非常长，也要舍弃部分点，防止数组越界
        end=5;
    if(start>=MT9V03X_H-1-5)
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(right_up_line==0&&//只找第一个符合条件的点
           abs(Right_Line[i]-Right_Line[i-1])<=5&&//下面两行位置差不多
           abs(Right_Line[i-1]-Right_Line[i-2])<=6&&
           abs(Right_Line[i-2]-Right_Line[i-3])<=7&&
              (Right_Line[i]-Right_Line[i+2])<=-5&&
              (Right_Line[i]-Right_Line[i+3])<=-10&&
              (Right_Line[i]-Right_Line[i+4])<=-10)
        {
            right_up_line=i;//获取行数即可
            break;
        }
    }
    return right_up_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     单调性突变检测
  @param     起始点，终止行
  @return    点所在的行数，找不到返回0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      前5后5它最大（最小），那他就是角点
-------------------------------------------------------------------------------------------------------------------*/
int Monotonicity_Change_Left(int start,int end)//单调性改变，返回值是单调性改变点所在的行数
{
    int i;
    int monotonicity_change_line=0;
    if(Left_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有单调性判断的意义
       return monotonicity_change_line;
    if(start>=MT9V03X_H-1-5)//数组越界保护，在判断第i个点时
       start=MT9V03X_H-1-5; //要访问它前后5个点，数组两头的点要不能作为起点终点
    if(end<=5)
        end=5;
    if(start<=end)//递减计算，入口反了，直接返回0
      return monotonicity_change_line;
    for(i=start;i>=end;i--)//会读取前5后5数据，所以前面对输入范围有要求
    {
        if(Left_Line[i]==Left_Line[i+5]&&Left_Line[i]==Left_Line[i-5]&&
        Left_Line[i]==Left_Line[i+4]&&Left_Line[i]==Left_Line[i-4]&&
        Left_Line[i]==Left_Line[i+3]&&Left_Line[i]==Left_Line[i-3]&&
        Left_Line[i]==Left_Line[i+2]&&Left_Line[i]==Left_Line[i-2]&&
        Left_Line[i]==Left_Line[i+1]&&Left_Line[i]==Left_Line[i-1])
        {//一堆数据一样，显然不能作为单调转折点
            continue;
        }
        else if(Left_Line[i]>=Left_Line[i+5]&&Left_Line[i]>=Left_Line[i-5]&&
        Left_Line[i]>=Left_Line[i+4]&&Left_Line[i]>=Left_Line[i-4]&&
        Left_Line[i]>=Left_Line[i+3]&&Left_Line[i]>=Left_Line[i-3]&&
        Left_Line[i]>=Left_Line[i+2]&&Left_Line[i]>=Left_Line[i-2]&&
        Left_Line[i]>=Left_Line[i+1]&&Left_Line[i]>=Left_Line[i-1])
        {//就很暴力，这个数据是在前5，后5中最大的（可以取等），那就是单调突变点
            monotonicity_change_line=i;
            int x=0;
            for(int j=-4;j<=4;j++)
            {
                if(abs(Left_Line[i] - Left_Line[i+j]) > 20||Left_Lost_Flag[i+j]==1)
                    x=1;
            }
            if(x==0)
                break;
            else
                continue;        }
    }
    return monotonicity_change_line;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     单调性突变检测
  @param     起始点，终止行
  @return    点所在的行数，找不到返回0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      前5后5它最大（最小），那他就是角点
-------------------------------------------------------------------------------------------------------------------*/
int Monotonicity_Change_Right(int start,int end)//单调性改变，返回值是单调性改变点所在的行数
{
    int i;
    int monotonicity_change_line=0;

    if(Right_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有单调性判断的意义
        return monotonicity_change_line;
    if(start>=MT9V03X_H-1-5)//数组越界保护
        start=MT9V03X_H-1-5;
     if(end<=5)
        end=5;
    if(start<=end)
        return monotonicity_change_line;
    for(i=start;i>=end;i--)//会读取前5后5数据，所以前面对输入范围有要求
    {
        if(Right_Line[i]==Right_Line[i+5]&&Right_Line[i]==Right_Line[i-5]&&
        Right_Line[i]==Right_Line[i+4]&&Right_Line[i]==Right_Line[i-4]&&
        Right_Line[i]==Right_Line[i+3]&&Right_Line[i]==Right_Line[i-3]&&
        Right_Line[i]==Right_Line[i+2]&&Right_Line[i]==Right_Line[i-2]&&
        Right_Line[i]==Right_Line[i+1]&&Right_Line[i]==Right_Line[i-1])
        {//一堆数据一样，显然不能作为单调转折点
            continue;
        }
        else if(Right_Line[i]<=Right_Line[i+5]&&Right_Line[i]<=Right_Line[i-5]&&
        Right_Line[i]<=Right_Line[i+4]&&Right_Line[i]<=Right_Line[i-4]&&
        Right_Line[i]<=Right_Line[i+3]&&Right_Line[i]<=Right_Line[i-3]&&
        Right_Line[i]<=Right_Line[i+2]&&Right_Line[i]<=Right_Line[i-2]&&
        Right_Line[i]<=Right_Line[i+1]&&Right_Line[i]<=Right_Line[i-1])
        {//就很暴力，这个数据是在前5，后5中最大的，那就是单调突变点
            monotonicity_change_line=i;
            int x=0;
            for(int j=-4;j<=4;j++)
            {
                if(abs(Right_Line[i] - Right_Line[i+j]) > 20||Right_Lost_Flag[i+j]==1)
                    x=1;
            }
            if(x==0)
                break;
            else
                continue;
        }
    }
    return monotonicity_change_line;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     通过斜率，定点补线--
  @param     k       输入斜率
             startY  输入起始点纵坐标
             endY    结束点纵坐标
  @return    null
  Sample     K_Add_Boundry_Left(float k,int startY,int endY);
  @note      补得线直接贴在边线上
-------------------------------------------------------------------------------------------------------------------*/
void K_Add_Boundry_Left(float k,int startX,int startY,int endY)
{
    int i,t;
    if(startY>=MT9V03X_H-1)
        startY=MT9V03X_H-1;
    else if(startY<=0)
        startY=0;
    if(endY>=MT9V03X_H-1)
        endY=MT9V03X_H-1;
    else if(endY<=0)
        endY=0;
    if(startY<endY)//--操作，start需要大
    {
        t=startY;
        startY=endY;
        endY=t;
    }
    for(i=startY;i>=endY;i--)
    {
        Left_Line[i]=(int)((i-startY)/k+startX);//(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
        if(Left_Line[i]>=MT9V03X_W-1)
        {
            Left_Line[i]=MT9V03X_W-1;
        }
        else if(Left_Line[i]<=0)
        {
            Left_Line[i]=0;
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     通过斜率，定点补线
  @param     k       输入斜率
             startY  输入起始点纵坐标
             endY    结束点纵坐标
  @return    null    直接补边线
  Sample     K_Add_Boundry_Right(float k,int startY,int endY);
  @note      补得线直接贴在边线上
-------------------------------------------------------------------------------------------------------------------*/
void K_Add_Boundry_Right(float k,int startX,int startY,int endY)
{
    int i,t;
    if(startY>=MT9V03X_H-1)
        startY=MT9V03X_H-1;
    else if(startY<=0)
        startY=0;
    if(endY>=MT9V03X_H-1)
        endY=MT9V03X_H-1;
    else if(endY<=0)
        endY=0;
    if(startY<endY)
    {
        t=startY;
        startY=endY;
        endY=t;
    }
    for(i=startY;i>=endY;i--)
    {
        Right_Line[i]=(int)((i-startY)/k+startX);//(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
        if(Right_Line[i]>=MT9V03X_W-1)
        {
            Right_Line[i]=MT9V03X_W-1;
        }
        else if(Right_Line[i]<=0)
        {
            Right_Line[i]=0;
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     根据斜率划线
  @param     输入斜率，定点，画一条黑线
  @return    null
  Sample     K_Draw_Line(k, 20,MT9V03X_H-1 ,0)
  @note      补的就是一条线，需要重新扫线
-------------------------------------------------------------------------------------------------------------------*/
void K_Draw_Line(float k, int startX, int startY,int endY)
{
    int endX=0;

    if(startX>=MT9V03X_W-1)//限幅处理
        startX=MT9V03X_W-1;
    else if(startX<=0)
        startX=0;
    if(startY>=MT9V03X_H-1)
        startY=MT9V03X_H-1;
    else if(startY<=0)
        startY=0;
    if(endY>=MT9V03X_H-1)
        endY=MT9V03X_H-1;
    else if(endY<=0)
        endY=0;
    endX=(int)((endY-startY)/k+startX);//(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
    if(endX>=MT9V03X_W-1)
    {
        endX = MT9V03X_W-1;
        endY = (int)(k*(endX-startX)+startY);
        if(endY<0)
            endY=0;
    }
    else if(endX<=0)
    {
        endX = 0;
        endY = (int)(k*(endX-startX)+startY);
        if(endY<0)
            endY=0;
    }
    Draw_Line(startX,startY,endX,endY);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     获取平均赛宽
  @param     int start_line,int end_line，起始行，中止行
  @return    这几行赛宽平均值
  Sample     road_wide=Get_Road_Wide(68,69);
  @note      ++运算，向下寻找，算出平均赛宽
-------------------------------------------------------------------------------------------------------------------*/
int Get_Road_Wide(int start_line,int end_line)
{
    if(start_line>=MT9V03X_H-1)
        start_line=MT9V03X_H-1;
    else if(start_line<=0)
        start_line=0;
    if(end_line>=MT9V03X_H-1)
        end_line=MT9V03X_H-1;
    else if(end_line<=0)
        end_line=0;
    int i=0,t=0;
    int road_wide=0;
    if(start_line>end_line)//++访问，坐标反了互换
    {
        t=start_line;
        start_line=end_line;
        end_line=t;
    }
    for(i=start_line;i<=end_line;i++)
    {
        road_wide+=Right_Line[i]-Left_Line[i];
    }
    road_wide=road_wide/(end_line-start_line+1);//平均赛宽
    return road_wide;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     获取左赛道边界斜率
  @param     int start_line,int end_line，起始行，中止行
  @return    两点之间的斜率
  Sample     k=Get_Left_K(68,69);
  @note      两点之间得出斜率，默认第一个参数小，第二个参数大
-------------------------------------------------------------------------------------------------------------------*/
float Get_Left_K(int start_line,int end_line)
{
    if(start_line>=MT9V03X_H-1)
        start_line=MT9V03X_H-1;
    else if(start_line<=0)
        start_line=0;
    if(end_line>=MT9V03X_H-1)
        end_line=MT9V03X_H-1;
    else if(end_line<=0)
        end_line=0;
    float k=0;
    int t=0;
    if(start_line>end_line)//++访问，坐标反了互换
    {
        t=start_line;
        start_line=end_line;
        end_line=t;
    }
    k=(float)(((float)Left_Line[start_line]-(float)Left_Line[end_line])/(end_line-start_line+1));
    return k;
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     获取右赛道边界斜率
  @param     int start_line,int end_line，起始行，中止行
  @return    两点之间的斜率
  Sample     k=Get_Right_K(68,69);
  @note      两点之间得出斜率，默认第一个参数小，第二个参数大
-------------------------------------------------------------------------------------------------------------------*/
float Get_Right_K(int start_line,int end_line)
{
    if(start_line>=MT9V03X_H-1)
        start_line=MT9V03X_H-1;
    else if(start_line<=0)
        start_line=0;
    if(end_line>=MT9V03X_H-1)
        end_line=MT9V03X_H-1;
    else if(end_line<=0)
        end_line=0;
    float k=0;
    int t=0;
    if(start_line>end_line)//++访问，坐标反了互换
    {
        t=start_line;
        start_line=end_line;
        end_line=t;
    }
    k=(float)(((float)Right_Line[start_line]-(float)Right_Line[end_line])/(end_line-start_line+1));
    return k;
}
//基于两点画线，为入环做准备
void Draw_Line(int x0, int y0, int x1, int y1)
{
    // 参数校验
    if (x0 < 0 || x0 >= MT9V03X_W || y0 < 0 || y0 >= MT9V03X_H ||
        x1 < 0 || x1 >= MT9V03X_W || y1 < 0 || y1 >= MT9V03X_H) {
        return; // 坐标越界直接返回[1,4]
    }

    // 保证从左到右绘制（交换起点终点）
    if (x0 > x1) {
        int temp = x0; x0 = x1; x1 = temp;
        temp = y0; y0 = y1; y1 = temp;
    }

    int dx = x1 - x0;
    int dy = abs(y1 - y0);
    int stepY = (y1 > y0) ? 1 : -1; // 步进方向控制[4](@ref)

    int err = dx - dy; // Bresenham误差项[2,3](@ref)

    // 主循环绘制直线
    while (x0 <= x1) {
        // 绘制当前点（二维数组直接访问）
        image_two_value[y0][x0] = 0;
        if(x0<=MT9V03X_W-2||x0>=1)
        {
            image_two_value[y0][x0+1] = 0;
            image_two_value[y0][x0-1] = 0;
        }
        // 误差判断与坐标更新
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0++;
        }
        if (e2 < dx) {
            err += dx;
            y0 += stepY;
            // 垂直方向越界保护
            if (y0 < 0 || y0 >= MT9V03X_H) break;
        }
    }
}
/*void Draw_Line(int x0, int y0, int x1, int y1)
{
                float k=(float)(x0-x1)/(y0-y1);//这里的k是1/斜率
                for(int i=y0;i>=y1;i--)
                {
                    j=(int)(i - y0)*k+x0;//(x=(y-y1)*k+x1),点斜式变形
                    if(j>=MT9V03X_W-1)
                    {
                        j=MT9V03X_W-1;
                    }
                    else if(j<=0)
                    {
                        j=0;
                    }
                    image_two_value[i][j]=0;
                }
}
*/
