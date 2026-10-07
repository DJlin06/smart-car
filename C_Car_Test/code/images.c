/*
 * images.c
 *
 *  Created on: 2024年12月21日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
//宏定义
#define white_pixel 255
#define black_pixel 0
/*变量声明*/
// 二值图像数据 (0-背景, 255-前景)
int current = 0;
uint16 original_image[MT9V03X_H/3][MT9V03X_W] = {0};
uint8 image_thereshold;//图像分割阈值
uint8 bin_image[MT9V03X_H/3][MT9V03X_W] = {0};//图像数组

#define MIN_AREA 5  // 最小区域面积
#define CIRCLE_THRESH 0.85  // 圆形度阈值
#define pi 3.1415926

//------------------------------------------------------------------------------------------------------------------

/*
函数名称：int my_abs(int value)
功能说明：求绝对值
参数说明：
函数返回：绝对值
修改时间：2025年1月18日
备    注：
example：  the_abs( x)；
 */
int my_abs(int value)
{
if(value>=0) return value;
else return -value;
}
float my_abs_float(float value)
{
    if(value>=0) return value;
    else return -value;
}
int16 limit_a_b(int16 x, int16 a, int16 b)
{
    if(x<a) x = a;
    if(x>b) x = b;
    return x;
}

/*
函数名称：int16 limit(int16 x, int16 y)
功能说明：求x,y中的最小值
参数说明：
函数返回：返回两值中的最小值
修改时间：2025年1月18日
备    注：
example：  limit( x,  y)
 */
int16 limit1(int16 x, int16 y)
{
    if (x > y)             return y;
    else if (x < -y)       return -y;
    else                return x;
}

//------------------------------------------------------------------------------------------------------------------
//  @brief      获得一副灰度图像
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------
void Get_image(uint8(*index)[MT9V03X_W])
{
#define use_num     1   //1就是不压缩，2就是压缩一倍
    uint8 i = 0, j = 0, row = 0, line = 0;
    for (i = MT9V03X_H/3-20; i < MT9V03X_H*2/3-20; i += use_num)          //
    {
        for (j = 0; j <MT9V03X_W; j += use_num)     //
        {
            original_image[row][line] = index[i][j];//这里的参数填写你的摄像头采集到的图像
            line++;
        }
        line = 0;
        row++;
    }
}

//------------------------------------------------------------------------------------------------------------------
//  @brief     固定阈值
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------
uint8 otsuThreshold(uint16 *image, uint16 col, uint16 row)
{
   return 240;
}

//------------------------------------------------------------------------------------------------------------------
//  @brief      图像二值化，这里用的是固定阈值二值化。
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------

void turn_to_bin(void)
{
  uint8 i,j;
  image_thereshold = otsuThreshold(original_image, MT9V03X_W, MT9V03X_H/3);
  for(i = 0;i<MT9V03X_H/3;i++)
  {
      for(j = 0;j<MT9V03X_W;j++)
      {
          if(original_image[i][j]>image_thereshold)bin_image[i][j] = white_pixel;
          else bin_image[i][j] = black_pixel;
      }
  }
}

//定义膨胀和腐蚀的阈值区间
#define threshold_max   255*5//此参数可根据自己的需求调节
#define threshold_min   255*2//此参数可根据自己的需求调节
void image_filter(uint8(*bin_image)[MT9V03X_W])//形态学滤波，简单来说就是膨胀和腐蚀的思想
{
    uint16 i, j;
    uint32 num = 0;


    for (i = 1; i < MT9V03X_H/3 - 1; i++)
    {
        for (j = 1; j < (MT9V03X_W - 1); j++)
        {
            //统计八个方向的像素值
            num =
                bin_image[i - 1][j - 1] + bin_image[i - 1][j] + bin_image[i - 1][j + 1]
                + bin_image[i][j - 1] + bin_image[i][j + 1]
                + bin_image[i + 1][j - 1] + bin_image[i + 1][j] + bin_image[i + 1][j + 1];


            if (num >= threshold_max && bin_image[i][j] == 0)
            {

                bin_image[i][j] = 255;//白  可以搞成宏定义，方便更改

            }
            if (num <= threshold_min && bin_image[i][j] == 255)
            {

                bin_image[i][j] = 0;//黑

            }

        }
    }

}

#define MAX_LABELS 800  //最大标签数
RegionInfo regions[MAX_LABELS]; // 区域存储数组
int region_count = 0;

// 8邻域方向
const int dx8[] = {-1, 0, 1, -1, 1, -1, 0, 1};
const int dy8[] = {-1, -1, -1, 0, 0, 1, 1, 1};
//4邻域
const int dx4[] = {-1, 0,0,1};
const int dy4[] = {0, -1,1,0};
// 连通域标记函数（两遍扫描法）
void connected_component_labeling() {

    for(int i = 0;i<MT9V03X_H/3;i++)
      {
          for(int j = 0;j<MT9V03X_W;j++)
          {
              original_image[i][j]=0;
          }
      }
    uint8 current_label = 1;
    uint8 union_table[MAX_LABELS+1] = { 0 };

    // 第一遍扫描：临时标记

    for (int y = 0; y < MT9V03X_H/3; y++)
    {
        for (int x = 0; x < MT9V03X_W; x++)
        {

            if (bin_image[y][x] != 255) continue;

            int neighbor_labels[8] = { 0 };
            int neighbor_count = 0;

            // 检查4邻域
            for (int d = 0; d < 4; d++)
            {
                int nx = x + dx4[d];
                int ny = y + dy4[d];
                if (nx >= 0 && ny >= 0 && nx < MT9V03X_W && ny < MT9V03X_H/3 && bin_image[ny][nx] == 255 && original_image[ny][nx] !=0)
                {
                    neighbor_labels[neighbor_count++] = original_image[ny][nx];
                }
            }


            if (neighbor_count == 0)
            {
                // 新区域

                original_image[y][x] = current_label;
                union_table[current_label] = current_label;
                current_label++;

            }
            else
            {
                // 找到最小标签
                uint16 min_label = neighbor_labels[0];
                for (int i = 1; i < neighbor_count; i++)
                {
                    if (neighbor_labels[i] < min_label)
                    {
                        min_label = neighbor_labels[i];
                    }
                }
                original_image[y][x] = min_label;
                // 合并等价标签
                for (int i = 0; i < neighbor_count; i++)
                {
                    if (neighbor_labels[i] != min_label)
                    {
                        union_table[neighbor_labels[i]] = min_label;
                    }
                }
            }
        }
    }
    current = current_label;
    // 第二遍扫描：统一标签
    for (int y = 0; y < MT9V03X_H/3; y++)
    {
        for (int x = 0; x < MT9V03X_W; x++)
        {

            int label = original_image[y][x];
            if (label == 0) continue;

            while (label != union_table[label])
            {
                label = union_table[label];
            }
            original_image[y][x] = label;
        }
    }

    int area[MAX_LABELS] = {0};
    int perim[MAX_LABELS] = {0};

    float sum_x[MAX_LABELS] = {0};
    float sum_y[MAX_LABELS] = {0};

    // 区域特征统计

    // 初始化边界值

    // 计算区域
    for (int y = 0; y < MT9V03X_H/3; y++) {
        for (int x = 0; x < MT9V03X_W; x++) {
            int label = original_image[y][x];
            if (label == 0) continue;

            area[label]++;
            sum_x[label] += x;
            sum_y[label] += y;

            // 更新边界


            // 计算周长（边界像素）
            int is_boundary = 0;
            for (int d = 0; d < 4; d++)
            {
                int nx = x + dx4[d];
                int ny = y + dy4[d];
                if (nx < 0 || ny < 0 || nx >= MT9V03X_W || ny >= MT9V03X_H/3 ||
                        bin_image[ny][nx] != 255)
                {
                    is_boundary = 1;
                }
                }
              if (is_boundary) perim[label]++;
        }
    }

    // 存储区域信息
    region_count = 0;
    for (int l = 1; l < current_label; l++)
    {
        //if (area[l] < MIN_AREA) continue;

        regions[region_count].label = l;
        regions[region_count].area = area[l];
        regions[region_count].perimeter = perim[l];

        regions[region_count].centroid_x = sum_x[l] / area[l];
        regions[region_count].centroid_y = sum_y[l] / area[l];

        // 计算圆形度4π*A/P*P
        if (perim[l] > 0)
        {
            float circular = (float)((4 * pi * area[l]) / (perim[l] * perim[l]));
            regions[region_count].circularity = circular;
        }

        region_count++;
    }
    //printf("%d\n",current_label);
}
int valid_count = 0;
// 圆形区域筛选函数
// 先过滤再按面积排序
void find_circles(RegionInfo*circles, int max_circles)
{
    RegionInfo temp[region_count];
    valid_count = 0;

    // 过滤有效区域
    for (int i = 0; i < region_count; i++)
    {
        if (regions[i].circularity > CIRCLE_THRESH && regions[i].area > MIN_AREA)
        {
            temp[valid_count++] = regions[i];
        }
    }

    // 按面积降序排序
    for (int i = 0; i < valid_count; i++)
    {
        for (int j = i + 1; j < valid_count; j++)
        {
            if (temp[j].circularity > temp[i].circularity)
            {
                RegionInfo swap = temp[i];
                temp[i] = temp[j];
                temp[j] = swap;
            }
        }
    }

    // 复制结果
    int found = 0;
    for (int i = 0; i < valid_count && found < max_circles; i++)
    {
        circles[found++] = temp[i];
    }
}
uint8 Img_disappear = 0; //丢图标志位
uint8 Two_disappear = 0; //前车丢灯标志位
uint8 Start_Flag = 0; //发车标志位
void Protect(RegionInfo*circles) //后车保护函数
{
    Img_disappear = 0;
    Two_disappear = 0;
    if(my_abs_float(circles[0].centroid_x - circles[1].centroid_x)>15||valid_count<2) //前车灯误判保护
    {
        Two_disappear = 1;
        Img_dissapear();
    }
}
void Begin()    //发车启动判断函数
{
    if(valid_count>=2&&Start_Flag==0)
    {
        system_delay_ms(3150); //这个时间与前车亮灯后时间对应
        Start_Flag = 1;
    }
}
void Img_dissapear()  //丢灯判断
{
    float variance = 0;
    float variance_sum = 0;
    uint32 sum_y = 0;
    uint32 sum_x = 0;
    uint16_t  point_y_record[800] = {0}; //存储所有白色光斑点的横向坐标
    uint16_t  point_x_record[800] = {0};
    //uint16_t  point_y_record[800] = {0}; //存储所有白色光斑点的横向坐标
    uint16 point_count = 0;
    for(int i= 0 ;i < MT9V03X_H/3;i++)
     {
         for(j= 0 ;j < MT9V03X_W;j++)
         {
                if (bin_image[i][j] == 255)
                {
                    point_x_record[point_count] = j;
                    point_y_record[point_count] = i;
                    sum_y+= i;
                    sum_x+= j;
                    point_count++;
                   // point_y_record[point_count] = i;
                    //point_count++;
                }
          }
     }
         if(point_count <  3)   //如果白点数太少，默认丢图
         {
             Img_disappear= 1;
             Start_Flag = 0;
         }
         else
         {
             float y_avg = ((float)sum_y)/(float)point_count; //计算车灯所有白光点平均纵坐标
             error = 2*(((float)sum_x)/(float)point_count) - MT9V03X_W; //计算error，并且增大分辨率
             for (int k = 0 ;k < point_count;k++)
          {
             variance_sum += cauclate_variance(y_avg,point_y_record[k]);
          }
          variance = variance_sum/ point_count; //计算所有白光点纵坐标方差
          float rad = error*0.375; //根据error推算车灯与后车摄像头所成角度
           float k = 0;
          float y = 408.75*powf(variance,-0.541);   //这是拟合后的函数，基于物理模型下推算的由前车灯方差计算车灯与后车摄像头距离
          if(error==0)
              k=1; //防止0越位
          else
              k = (rad*pi/180)/sinf(rad*pi/180);//物理模型公式
          error_instance = y*k; //最终计算出的实际距离
         }
}
int32 error = 0;
int32 error_instance = Instance_target;
int32 Last_error = 0;
int32 Last_error_instance = Instance_target;
float ins_temp = 0;
float error_Y = 0;
int Filter_error = 0;
void Get_err(RegionInfo*circles) //获取error以及前后车距离
{
    float position = circles[0].centroid_x + circles[1].centroid_x;
    if(Img_disappear==1)
    {
        error = Last_error;
        error_instance = Last_error_instance;
        //Speed = 0;
    }
    else if(Two_disappear == 0)
    {
        Filter_error = error;
        error = (position - MT9V03X_W);
        error_Y = my_abs_float(circles[1].centroid_y - circles[0].centroid_y);
        //printf("%f\n",x);
        //这一部分与上面一样，不过上面的是为了防止距离太远两灯重叠
        float rad = error*0.375;
         float k = 0;
        int x = LQ_Deal_Image(circles);
        float y = 408.75*powf(x,-0.541);
        if(error==0)
            k=1;
        else
            k = (rad*pi/180)/sinf(rad*pi/180);
        error_instance = y*k;
    }
    //记录这一次的数据
    Last_error = error;
    Last_error_instance = error_instance;
}
/* 方差计算
 * 距离环备案
 */
float cauclate_variance(float average,float num)
{
    float out = 0;
    out = (num - average)*(num - average);
    return out;
}
int32 LQ_Deal_Image(RegionInfo*circles)
{
    float variance = 0;
    int pro_flag = 1;
    if(my_abs_float(circles[1].centroid_x - circles[0].centroid_x)>=15)
    {
        pro_flag = 0;
    }
    uint16_t  point_x_record[800] = {0}; //存储所有白色光斑点的横向坐标
    uint16_t i = 0,j = 0,point_count = 0;
    float x_avg = (circles[0].centroid_y*circles[0].area + circles[1].centroid_y*circles[1].area)/(circles[0].area+circles[1].area);
    for(i= 0 ;i < MT9V03X_H/3;i++)
    {
        for(j= 0 ;j < MT9V03X_W;j++)
        {
               if (original_image[i][j] == circles[0].label || original_image[i][j] == circles[1].label)
               {
                   point_x_record[point_count] = i;
                   point_count ++;
                   if(point_count >= 800)
                   {
                       ips200_show_string(100,120,"ERROR");
                   }
               }
        }
    }
    float variance_sum = 0;
    if(point_count != 0)        //图像中残存的有亮点
    {
        if( pro_flag  == 1) //保护状态机
        {
            for (uint8_t k = 0 ;k < point_count;k++)
         {
            variance_sum += cauclate_variance(x_avg,point_x_record[k]);
         }
         variance = variance_sum/ point_count;
        }

    }
    else if(point_count == 0)    //图像光电丢失
    {
        variance = error_instance;
    }
    return variance;
}
float centroid_x0 = 0;
float centroid_y0 = 0;
float centroid_x1 = 0;
float centroid_y1 = 0;
/*
函数名称：void image_process(void)
功能说明：最终处理函数
参数说明：无
函数返回：无
修改时间：2025年1月18日
备    注：
example： image_process();
 */
void image_process()
{
//uint16 i;
/*这是离线调试用的*/

Get_image(mt9v03x_image);
Isp_flag = 1;
turn_to_bin();
//image_filter(bin_image);//滤波

connected_component_labeling();

RegionInfo detected_circles[2];

find_circles(detected_circles, 2);

Protect(detected_circles);

Get_err(detected_circles);
if(Img_disappear==0)
    Begin(); //发车检测
centroid_x0 = detected_circles[0].centroid_x;
centroid_x1 = detected_circles[1].centroid_x;
centroid_y0 = detected_circles[0].centroid_y;
centroid_y1 = detected_circles[1].centroid_y;


//清零

//显示图像
//

}

/*

这里是起点（0.0）***************——>*************x值最大
************************************************************
************************************************************
************************************************************
************************************************************
******************假如这是一副图像*************************
***********************************************************
***********************************************************
***********************************************************
***********************************************************
***********************************************************
***********************************************************
y值最大*******************************************(188.120)

*/


