/*
 * images.c
 *
 *  Created on: 2024年12月21日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
//宏定义
#define IMG_BLACK 0
#define IMG_WHITE 255
uint8 image_two_value[MT9V03X_H][MT9V03X_W] = {0};//二值化后的原数组
uint8 image_two_original[MT9V03X_H][MT9V03X_W] = {0}; //原始灰度图像存放数组
uint8 Value_isp_Flag = 0;  //图像显示标志位，1为显示，0为不显示
uint8 Cross_state = 0; //十字状态位
uint8 Left_Line[MT9V03X_H] = {0}; //左边线数组
uint8 Right_Line[MT9V03X_H] = {0};//右边线数组
uint8 Mid_Line[MT9V03X_H] = {0};  //中线数组
uint8 Road_Wide[MT9V03X_H] = {0}; //赛宽数组
uint8 White_Column[MT9V03X_W] = {0};    //每列白列长度
uint8 Search_Stop_Line = 0;     //搜索截止行,只记录长度，想要坐标需要用视野高度减去该值
uint8 Boundry_Start_Left = 0;   //左右边界起始点
uint8 Boundry_Start_Right = 0;  //第一个非丢线点,常规边界起始点
uint8 Left_Lost_Time = 0;       //边界丢线数
uint8 Right_Lost_Time = 0;
uint8 Both_Lost_Time = 0;//两边同时丢线数
uint8 Longest_White_Column_Left[2] = {0}; //最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
uint8 Longest_White_Column_Right[2]= {0} ;//最长白列,[0]是最长白列的长度，也就是Search_Stop_Line搜索截止行，[1】是第某列
uint8 Left_Lost_Flag[MT9V03X_H]= {0} ; //左丢线数组，丢线置1，没丢线置0
uint8 Right_Lost_Flag[MT9V03X_H]= {0}; //右丢线数组，丢线置1，没丢线置0

//环岛
uint8 Island_State =0;     //环岛状态标志
uint8 Left_Island_Flag =0; //左右环岛标志
uint8 Right_Island_Flag =0;//左右环岛标志
//十字
uint8 Cross_Flag = 0;     //十字标志位
uint8 Left_Down_Find = 0; //十字使用，找到被置行数，没找到就是0
uint8 Left_Up_Find = 0;   //四个拐点标志
uint8 Right_Down_Find = 0;
uint8 Right_Up_Find = 0;
//直道
uint8 Straight_Flag = 0; //直道标志位
//丢图
uint8 Img_Disappear_Flag = 0; //丢图标志位
uint8 Zebra_Stripes_Flag = 0; //斑马线标志位
int32 Y_MEET = 0; //最大前瞻计算点

/*
 * 前瞻固定权重数组，用于计算error
 * 具体权重取值看实际情况，这一部分需要微调
 */
int weight_image[MT9V03X_H]={

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,              //图像最远端00 ——09 行权重
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,              //图像最远端10 ——19 行权重
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 3, 4, 5,
        6, 7, 7, 9, 9,11,11,13,15,15,
        13,11,11,9, 9, 7, 7, 5, 3, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/*
 * 动态权重基础数组，后面会有一个函数专门对这个数组操作，实现权重动态变化
 */

uint8 base_weight[MT9V03X_H]={

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

//像素点坐标结构体
typedef struct
{
    double x;
    double y;
} Point;

// 计算两点间距离
double distance(Point a, Point b)
{
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return sqrt(dx*dx + dy*dy);
}

// 三点法计算曲率（核心算法）
double curvature_3_points(Point p1, Point p2, Point p3)
{
    // 计算三边长度
    double a = distance(p2, p3);
    double b = distance(p1, p3);
    double c = distance(p1, p2);

    // 向量叉乘求面积（判断方向）
    double cross = (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);
    double area = fabs(cross) / 2.0;

    // 处理共线情况（曲率为0）
    if (area < 1e-2/2) return 0.0;

    // 计算外接圆半径 R = abc/(4S)
    double radius = (a * b * c) / (4 * area);

    // 曲率 = 1/半径，保留方向性（左弯正/右弯负）
    return (cross > 0 ? 1.0 : -1.0) / radius;
}
// 滑动窗口滤波（抑制噪声）
void moving_average_filter(double* curvature, int size, int window_size) {
    double window[window_size];
    for (int i = 0; i < window_size; i++) window[i] = curvature[i];

    for (int i = window_size; i < size; i++)
    {
        // 更新窗口
        window[i % window_size] = curvature[i];

        // 计算窗口平均值
        double sum = 0.0;
        for (int j = 0; j < window_size; j++)
        {
            sum += window[j];
        }
        curvature[i] = sum / window_size;
    }
}

// 根据曲率计算安全速度

double radiu = 0.0; //三点法计算出的曲率
double Last_radiu = 0.0; //上一次三点法计算出的曲率

/*
 * 函数作用：曲率计算
 * 参数作用：y为曲率起始计算点的纵坐标，n为选取计算曲率像素点的个数，一般固定为3，m为每个像素点所取纵坐标的差值
 * 示例：Curvature_Pro(60,3,5)
 */
double Curvature_Pro(int y,int n,int m)
{
    Last_radiu = radiu;
    Point point[n];
    int j = 0;
    for(int i=y;i<y+m*n;i+=m)
    {
        point[j].y = (double)Mid_Line[i];
        point[j].x = (double)i;
        j++;
    }
    double curvature[n-2];
    for(int i=0;i<n-2;i++)
    {
        curvature[i] = curvature_3_points(point[i],point[i+1],point[i+2]);
    }
    /*
    这一部分为均值滤波
    double sum = 0.0;
    for(int i = 0;i<n-2;i++)
    {
        sum+= curvature[i];
    }
    radiu = sum/(n-2);
    */
    //这一部分为较为简单的滤波，与上一次计算曲率按权重计算
    //radiu = radiu>0?radiu:-radiu;
    //radiu = 0.7*radiu+0.3*Last_radiu;
    return curvature[0];
}

uint8 Y_Meet_Array[20] = {0};//最长白列数存放数组
//此函数为权重数组动态变化以及曲率计算函数，用作速度决策及路径规划
void Weight_change()
{
    //相遇点滑动均值滤波
    uint8 Y_Meet_Max = 0;
    uint8 Y_Meet_Min = 0;
    uint16 Y_Meet_Sum = 0;
    uint8 Y_Meet_Average = 0;
    uint8 continuity_change_right_flag=0;
    uint8 continuity_change_left_flag=0;
    Y_Meet_Min = Y_Meet_Array[0];
    Y_Meet_Max = Y_Meet_Array[0];
    for(int i = 0; i < 19; i++)
    {
        Y_Meet_Array[i + 1] = Y_Meet_Array[i];
    }
    Y_Meet_Array[0] = Search_Stop_Line;

    for(int i = 0; i < 20; i++)
    {
        Y_Meet_Sum += (uint16)Y_Meet_Array[i];
        if(Y_Meet_Array[i] < Y_Meet_Min)
          {
            Y_Meet_Min = Y_Meet_Array[i];
          }
        if(Y_Meet_Array[i] > Y_Meet_Max)
          {
            Y_Meet_Max = Y_Meet_Array[i];
          }
    }
    Y_Meet_Sum = Y_Meet_Sum - (uint16)Y_Meet_Min - (uint16)Y_Meet_Max;
    Y_Meet_Average = (uint8)(Y_Meet_Sum / 18);
    int Lost = continuity_change_left_flag<continuity_change_right_flag?continuity_change_right_flag:continuity_change_left_flag;
    if(Lost<=45)
    {
        radiu = fabs((Curvature_Pro(45,3,5)+Curvature_Pro(46,3,5)+Curvature_Pro(44,3,5))/3.0);
    }
    else if(Lost>45&&Lost<100)
    {
        radiu = fabs((Curvature_Pro(Lost,3,5)+Curvature_Pro(Lost+1,3,5)+Curvature_Pro(Lost-1,3,5))/3.0);
    }
    else if(Lost>=100)
    {
        radiu = 1;
    }

    if(Y_Meet_Average > Y_MEET)
    {
        Y_Meet_Average = Y_MEET;
    }
    else if(Y_Meet_Average < Y_MEET-10)
    {
        Y_Meet_Average = Y_MEET-10;
    }   //一直到这里都是滑动均值滤波，滤的比较狠，防止数据跳动

    //固定权值
    for(int i = 0;i < MT9V03X_H ; i++)
    {
        weight_image[i] = base_weight[i];
    }
    //局部动态权值，可根据车速自行调整区间和数值
    uint8 Trends_Weight[50] =
    {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 3, 4, 5, 6, 7, 7, 9, 9,
            11,11,13,15,15,13,11,11,9, 9,
            7, 7, 5, 3, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    };
    uint8 Start_Line = Y_Meet_Average;
    //将动态权值赋值给固定权值数组
    for(int i = 0; i < 50; i ++)
    {
        weight_image[MT9V03X_H - 1 - Start_Line + i] = Trends_Weight[i];
    }
}
/*
函数名称：int my_abs(int value)
功能说明：求绝对值
参数说明：
函数返回：绝对值
修改时间：2025年1月18日
备    注：
example：  my_abs( x)；
 */
int my_abs(int value)
{
    if (value >= 0) return value;
    else return -value;
}

int16 limit_a_b(int16 x, int16 a, int16 b)
{
    if (x < a) x = a;
    if (x > b) x = b;
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
/*变量声明*/
uint8 image_thereshold;//图像分割阈值
//------------------------------------------------------------------------------------------------------------------
//  @brief      获得一副灰度图像
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------
void Get_image(uint8(*index)[MT9V03X_W])
{
#define use_num     1   //1就是不压缩，2就是压缩一倍
    uint8 i = 0, j = 0, row = 0, line = 0;
    for (i = 0; i < MT9V03X_H; i += use_num)          //
    {
        for (j = 0; j < MT9V03X_W; j += use_num)     //
        {
            image_two_original[row][line] = index[i][j];//这里的参数填写你的摄像头采集到的图像
            line++;
        }
        line = 0;
        row++;
    }
}

/*-------------------------------------------------------------------------------------------------------------------
      @brief     快速大津求阈值
      @param     image       图像数组
                 col         列 ，宽度
                 row         行，长度
      @return    null
      Sample     threshold=my_adapt_threshold(mt9v03x_image[0],MT9V03X_W, MT9V03X_H);//山威快速大津
      @note      据说比传统大津法快一点，使用效果差不多
    -------------------------------------------------------------------------------------------------------------------
*/
uint8 otsuThreshold(uint8* image, uint16 col, uint16 row)   //注意计算阈值的一定要是原图像
{
#define GrayScale 256
    uint16 width = col;
    uint16 height = row;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j;
    int pixelSum = width * height / 4;
    uint8 threshold = 0;
    uint8* data = image;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
    uint32 gray_sum = 0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i += 2)
    {
        for (j = 0; j < width; j += 2)
        {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum += (int)data[i * width + j];       //灰度值总和
        }
    }

    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++)
    {
        pixelPro[j] = (float)pixelCount[j] / pixelSum;//计算每个像素值的点在整幅图像中的比例
        w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值
        w1 = 1 - w0;
        u1tmp = gray_sum / pixelSum - u0tmp;
        u0 = u0tmp / w0;              //背景平均灰度
        u1 = u1tmp / w1;              //前景平均灰度
        u = u0tmp + u1tmp;            //全局平均灰度
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }
    }
    return threshold;
}




//------------------------------------------------------------------------------------------------------------------
//  @brief      图像二值化，这里用的是大津法二值化。
//  @since      v1.0
//------------------------------------------------------------------------------------------------------------------


void turn_to_bin(void)
{
    uint8 i, j;
    image_thereshold = otsuThreshold(mt9v03x_image[35], MT9V03X_W, MT9V03X_H-35);
    mt9v03x_finish_flag = 0;
    for (i = 35; i < MT9V03X_H; i++)
    {
        for (j = 0; j < MT9V03X_W; j++)
        {
            if (mt9v03x_image[i][j] > image_thereshold)image_two_value[i][j] = IMG_WHITE;
            else image_two_value[i][j] = IMG_BLACK;
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     双最长白列巡线
  @param     null
  @return    null
  Sample     Longest_White_Column_Left();
  @note      最长白列巡线，寻找初始边界，丢线，最长白列等基础元素，后续读取这些变量来进行赛道识别
-------------------------------------------------------------------------------------------------------------------*/
void Longest_White_Column()//最长白列巡线
{
    int i, j;
    int start_column = 20;//最长白列的搜索区间
    int end_column = MT9V03X_W - 20;
    int left_border = 0, right_border = 0;//临时存储赛道位置
    Longest_White_Column_Left[0] = 0;//最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Left[1] = 0;//最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Right[0] = 0;//最长白列,[0]是最长白列的长度，[1】是第某列
    Longest_White_Column_Right[1] = 0;//最长白列,[0]是最长白列的长度，[1】是第某列
    Right_Lost_Time = 0;    //边界丢线数
    Left_Lost_Time = 0;
    Boundry_Start_Left = 0;//第一个非丢线点,常规边界起始点
    Boundry_Start_Right = 0;
    Both_Lost_Time = 0;//两边同时丢线数

    for (i = 35; i <= MT9V03X_H - 1; i++)//数据清零
    {
        Right_Lost_Flag[i] = 0;
        Left_Lost_Flag[i] = 0;
        Left_Line[i] = 0;
        Right_Line[i] = MT9V03X_W - 1;
    }
    //环岛需要对最长白列范围进行限定，环岛3状态找不到上角点，可以修改下述参数
        //环岛3状态需要改变最长白列寻找范围
    if (Island_State == 3)
    {
        if (Right_Island_Flag == 1)//右环
        {
                start_column = 60;
                end_column = MT9V03X_W - 40;
        }
        else if (Left_Island_Flag == 1)//左环
        {
                start_column = 40;
                end_column = MT9V03X_W - 60;
        }
    }

    //从左到右找左边最长白列
    Longest_White_Column_Left[0] = 0;
    for (i = start_column; i <= end_column; i++)
    {
        White_Column[i] = 0; //从左到右，从下往上，遍历全图记录范围内的每一列白点数量
        for (j = MT9V03X_H - 1; j >= 35; j--)
        {
            if (image_two_value[j][i] == IMG_BLACK)
                break;
            else
                White_Column[i]++;
        }
        if (Longest_White_Column_Left[0] < White_Column[i])//找最长的那一列
        {
            Longest_White_Column_Left[0] = White_Column[i];//【0】是白列长度
            Longest_White_Column_Left[1] = i;              //【1】是下标，第j列
        }
    }
    //从右到左找右左边最长白列
    Longest_White_Column_Right[0] = 0;//【0】是白列长度
    for (i = end_column; i >= Longest_White_Column_Left[1]; i--)//从右往左，注意条件，找到左边最长白列位置就可以停了
    {
        if (Longest_White_Column_Right[0] < White_Column[i])//找最长的那一列
        {
            Longest_White_Column_Right[0] = White_Column[i];//【0】是白列长度
            Longest_White_Column_Right[1] = i;              //【1】是下标，第j列
        }
    }

    Search_Stop_Line = Longest_White_Column_Left[0];//搜索截止行选取左或者右区别不大，他们两个理论上是一样的
    for (i = MT9V03X_H - 1; i >= MT9V03X_H - Search_Stop_Line; i--)//常规巡线
    {
        for (j = Longest_White_Column_Right[1]; j <= MT9V03X_W - 1 - 2; j++)
        {
            if (image_two_value[i][j] == IMG_WHITE && image_two_value[i][j + 1] == IMG_BLACK && image_two_value[i][j + 2] == IMG_BLACK)//白黑黑，找到右边界
            {
                right_border = j;
                Right_Lost_Flag[i] = 0; //右丢线数组，丢线置1，不丢线置0
                break;
            }
            else if (j >= MT9V03X_W - 1 - 2)//没找到右边界，把屏幕最右赋值给右边界
            {
                right_border = j;
                Right_Lost_Flag[i] = 1; //右丢线数组，丢线置1，不丢线置0
                break;
            }
        }
        for (j = Longest_White_Column_Left[1]; j >= 0 + 2; j--)//往左边扫描
        {
            if (image_two_value[i][j] == IMG_WHITE && image_two_value[i][j - 1] == IMG_BLACK && image_two_value[i][j - 2] == IMG_BLACK)//黑黑白认为到达左边界
            {
                left_border = j;
                Left_Lost_Flag[i] = 0; //左丢线数组，丢线置1，不丢线置0
                break;
            }
            else if (j <= 0 + 2)
            {
                left_border = j;//找到头都没找到边，就把屏幕最左右当做边界
                Left_Lost_Flag[i] = 1; //左丢线数组，丢线置1，不丢线置0
                break;
            }
        }
        Left_Line[i] = left_border;       //左边线线数组
        Right_Line[i] = right_border;      //右边线线数组
    }

    for (i = MT9V03X_H - 2; i >= 36; i--)//赛道数据初步分析
    {
        if(abs(Left_Line[i]-Left_Line[i-1])>=5&&abs(Left_Line[i]-Left_Line[i+1])>=5)
            Left_Lost_Flag[i] = 1;
        if(abs(Right_Line[i]-Right_Line[i-1])>=5&&abs(Right_Line[i]-Right_Line[i+1])>=5)
            Right_Lost_Flag[i] = 1;
        if (Left_Lost_Flag[i] == 1)//单边丢线数
            Left_Lost_Time++;
        if (Right_Lost_Flag[i] == 1)
            Right_Lost_Time++;
        if (Left_Lost_Flag[i] == 1 && Right_Lost_Flag[i] == 1)//双边丢线数
            Both_Lost_Time++;
        if (Boundry_Start_Left == 0 && Left_Lost_Flag[i] != 1)//记录第一个非丢线点，边界起始点
            Boundry_Start_Left = i;
        if (Boundry_Start_Right == 0 && Right_Lost_Flag[i] != 1)
            Boundry_Start_Right = i;
        //Road_Wide[i] = Right_Line[i] - Left_Line[i];
        //printf("%d,%d\n",Road_Wide[i],i);
    }

}

//图像显示中线，左右边线，单调突变点，丢线点
void Show_Boundry(void)
{
    int16 i;
    for (i = MT9V03X_H - 1; i >= MT9V03X_H - Search_Stop_Line; i--)//从最底下往上扫描
    {
        image_two_value[i][Left_Line[i] + 1] = IMG_BLACK;
        image_two_value[i][Mid_Line[i]] = IMG_BLACK;
        image_two_value[i][Right_Line[i] - 1] = IMG_BLACK;
    }
    for(i=0;i<MT9V03X_W-1;i++)
    {
        image_two_value[monotonicity_change_right_flag][i] = IMG_BLACK;
        image_two_value[monotonicity_change_left_flag][i] = IMG_BLACK;
        image_two_value[continuity_change_left_flag][i] = IMG_BLACK;
        image_two_value[continuity_change_right_flag][i] = IMG_BLACK;
    }
    //在屏幕理论中线处显示红线，用于调整摄像头
    //ips200_draw_line ( MT9V03X_W/2, MT9V03X_H-10, MT9V03X_W/2, MT9V03X_H, RGB565_RED);
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     左补线
  @param     补线的起点，终点
  @return    null
  Sample     Left_Add_Line(int x1,int y1,int x2,int y2);
  @note      补的直接是边界，点最好是可信度高的,不要乱补
-------------------------------------------------------------------------------------------------------------------*/
void Left_Add_Line(int x1, int y1, int x2, int y2)//左补线,补的是边界
{
    int i, max, a1, a2;
    int hx;
    if (x1 >= MT9V03X_W - 1)//起始点位置校正，排除数组越界的可能
        x1 = MT9V03X_W - 1;
    else if (x1 <= 0)
        x1 = 0;
    if (y1 >= MT9V03X_H - 1)
        y1 = MT9V03X_H - 1;
    else if (y1 <= 0)
        y1 = 0;
    if (x2 >= MT9V03X_W - 1)
        x2 = MT9V03X_W - 1;
    else if (x2 <= 0)
        x2 = 0;
    if (y2 >= MT9V03X_H - 1)
        y2 = MT9V03X_H - 1;
    else if (y2 <= 0)
        y2 = 0;
    a1 = y1;
    a2 = y2;
    if (a1 > a2)//坐标互换
    {
        max = a1;
        a1 = a2;
        a2 = max;
    }
    for (i = a1; i <= a2; i++)//根据斜率补线即可
    {
        hx = (i - y1) * (x2 - x1) / (y2 - y1) + x1;
        if (hx >= MT9V03X_W-1)
            hx = MT9V03X_W-1;
        else if (hx <= 0)
            hx = 0;
        Left_Line[i] = hx;
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右补线
  @param     补线的起点，终点
  @return    null
  Sample     Right_Add_Line(int x1,int y1,int x2,int y2);
  @note      补的直接是边界，点最好是可信度高的，不要乱补
-------------------------------------------------------------------------------------------------------------------*/
void Right_Add_Line(int x1, int y1, int x2, int y2)//右补线,补的是边界
{
    int i, max, a1, a2;
    int hx;
    if (x1 >= MT9V03X_W - 1)//起始点位置校正，排除数组越界的可能
        x1 = MT9V03X_W - 1;
    else if (x1 <= 0)
        x1 = 0;
    if (y1 >= MT9V03X_H - 1)
        y1 = MT9V03X_H - 1;
    else if (y1 <= 0)
        y1 = 0;
    if (x2 >= MT9V03X_W - 1)
        x2 = MT9V03X_W - 1;
    else if (x2 <= 0)
        x2 = 0;
    if (y2 >= MT9V03X_H - 1)
        y2 = MT9V03X_H - 1;
    else if (y2 <= 0)
        y2 = 0;
    a1 = y1;
    a2 = y2;
    if (a1 > a2)//坐标互换
    {
        max = a1;
        a1 = a2;
        a2 = max;
    }
    for (i = a1; i <= a2; i++)//根据斜率补线即可
    {
        hx = (i - y1) * (x2 - x1) / (y2 - y1) + x1;
        if (hx >= MT9V03X_W-1)
            hx = MT9V03X_W-1;
        else if (hx <= 0)
            hx = 0;
        Right_Line[i] = hx;
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     找下面的两个拐点，供十字使用
  @param     搜索的范围起点，终点
  @return    修改两个全局变量
             Right_Down_Find=0;
             Left_Down_Find=0;
  Sample     Find_Down_Point(int start,int end)
  @note      运行完之后查看对应的变量，注意，没找到时对应变量将是0
-------------------------------------------------------------------------------------------------------------------*/
void Find_Down_Point(int start, int end)
{
    int i, t;
    Right_Down_Find = 0;
    Left_Down_Find = 0;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5)//下面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if(Left_Up_Find!=0)
        {
            if (Left_Down_Find == 0 &&//只找第一个符合条件的点
            abs(Left_Line[i] - Left_Line[i + 1]) <= 5 &&//角点的阈值可以更改
            abs(Left_Line[i + 1] - Left_Line[i + 2]) <= 5 &&
            abs(Left_Line[i + 2] - Left_Line[i + 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i - 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i - 3]) >= 15 &&
            (Left_Line[i] - Left_Line[i - 4]) >= 15)
        {
            Left_Down_Find = i;//获取行数即可
        }
        }
        if(Right_Up_Find!=0)
        {
        if (Right_Down_Find == 0 &&//只找第一个符合条件的点
            abs(Right_Line[i] - Right_Line[i + 1]) <= 5 &&//角点的阈值可以更改
            abs(Right_Line[i + 1] - Right_Line[i + 2]) <= 5 &&
            abs(Right_Line[i + 2] - Right_Line[i + 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i - 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i - 3]) <= -15 &&
            (Right_Line[i] - Right_Line[i - 4]) <= -15)
        {
            Right_Down_Find = i;
        }
        }
        if (Left_Down_Find != 0 && Right_Down_Find != 0)//两个找到就退出
        {
            if (Left_Down_Find <= Left_Up_Find)
            {
                Left_Down_Find = 0;//下点不可能比上点还靠上
            }
            if (Right_Down_Find <= Right_Up_Find)
            {
                Right_Down_Find = 0;//下点不可能比上点还靠上
            }
            break;
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     找上面的两个拐点，供十字使用
  @param     搜索的范围起点，终点
  @return    修改两个全局变量
             Left_Up_Find=0;
             Right_Up_Find=0;
  Sample     Find_Up_Point(int start,int end)
  @note      运行完之后查看对应的变量，注意，没找到时对应变量将是0
-------------------------------------------------------------------------------------------------------------------*/
void Find_Up_Point(int start, int end)
{
    int i, t;
    Left_Up_Find = 0;
    Right_Up_Find = 0;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)//及时最长白列非常长，也要舍弃部分点，防止数组越界
        end = 5;
    if (start >= MT9V03X_H - 1 - 5)//下面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;
    for (i = start; i >= end; i--)
    {
        if (Left_Up_Find == 0 &&//只找第一个符合条件的点
            abs(Left_Line[i] - Left_Line[i - 1]) <= 5 &&
            abs(Left_Line[i - 1] - Left_Line[i - 2]) <= 5 &&
            abs(Left_Line[i - 2] - Left_Line[i - 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i + 2]) >= 5 &&
            (Left_Line[i] - Left_Line[i + 3]) >= 10 &&
            (Left_Line[i] - Left_Line[i + 4]) >= 10)
        {
            Left_Up_Find = i;//获取行数即可
        }
        if (Right_Up_Find == 0 &&//只找第一个符合条件的点
            abs(Right_Line[i] - Right_Line[i - 1]) <= 5 &&//下面两行位置差不多
            abs(Right_Line[i - 1] - Right_Line[i - 2]) <= 5 &&
            abs(Right_Line[i - 2] - Right_Line[i - 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i + 2]) <= -5 &&
            (Right_Line[i] - Right_Line[i + 3]) <= -10 &&
            (Right_Line[i] - Right_Line[i + 4]) <= -10)
        {
            Right_Up_Find = i;//获取行数即可
        }
        if (Left_Up_Find != 0 && Right_Up_Find != 0)//下面两个找到就出去
        {
            break;
        }
    }
    if (abs(Right_Up_Find - Left_Up_Find) >= 30)//纵向撕裂过大，视为误判
    {
        Right_Up_Find = 0;
        Left_Up_Find = 0;
    }
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     左边界延长
  @param     延长起始行数，延长到某行
  @return    null
  Sample     Stop_Detect(void)
  @note      从起始点向上找5个点，算出斜率，向下延长，直至结束点
-------------------------------------------------------------------------------------------------------------------*/
void Lengthen_Left_Boundry(int start,int end)
{
    int i,t;
    float k=0;
    if(start>=MT9V03X_H-1)//起始点位置校正，排除数组越界的可能
        start=MT9V03X_H-1;
    else if(start<=0)
        start=0;
    if(end>=MT9V03X_H-1)
        end=MT9V03X_H-1;
    else if(end<=0)
        end=0;
    if(end<start)//++访问，坐标互换
    {
        t=end;
        end=start;
        start=t;
    }

    if(start<=5)//因为需要在开始点向上找3个点，对于起始点过于靠上，不能做延长，只能直接连线
    {
         Left_Add_Line(Left_Line[start],start,Left_Line[end],end);
    }

    else
    {
        k=(float)(Left_Line[start]-Left_Line[start-4])/5.0;//这里的k是1/斜率
        for(i=start;i<=end;i++)
        {
            Left_Line[i]=(int)(i-start)*k+Left_Line[start];//(x=(y-y1)*k+x1),点斜式变形
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
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右左边界延长
  @param     延长起始行数，延长到某行
  @return    null
  Sample     Stop_Detect(void)
  @note      从起始点向上找3个点，算出斜率，向下延长，直至结束点
-------------------------------------------------------------------------------------------------------------------*/
void Lengthen_Right_Boundry(int start,int end)
{
    int i,t;
    float k=0;
    if(start>=MT9V03X_H-1)//起始点位置校正，排除数组越界的可能
        start=MT9V03X_H-1;
    else if(start<=0)
        start=0;
    if(end>=MT9V03X_H-1)
        end=MT9V03X_H-1;
    else if(end<=0)
        end=0;
    if(end<start)//++访问，坐标互换
    {
        t=end;
        end=start;
        start=t;
    }

    if(start<=5)//因为需要在开始点向上找3个点，对于起始点过于靠上，不能做延长，只能直接连线
    {
        Right_Add_Line(Right_Line[start],start,Right_Line[end],end);
    }
    else
    {
        k=(float)(Right_Line[start]-Right_Line[start-4])/5.0;//这里的k是1/斜率
        for(i=start;i<=end;i++)
        {
            Right_Line[i]=(int)(i-start)*k+Right_Line[start];//(x=(y-y1)*k+x1),点斜式变形
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
}
uint8 X_Cross=0;//十字类型判断
//这两个函数都用于判断斜入十字，当返回的值大于某个值时即认为不属于斜入十字
int arctan_Line_Left(int n,int p)
{
    float k=((float)(Left_Line[monotonicity_change_left_flag + n]-Left_Line[monotonicity_change_left_flag]))/((float)n);//这里的k是1/斜率
    float m =p*k+Left_Line[monotonicity_change_left_flag];//(x=(y-y1)*k+x1),点斜式变形
    return abs((int)(Left_Line[monotonicity_change_left_flag+p]-m));
}
int arctan_Line_Right(int n,int p)
{
    float k=((float)(Right_Line[monotonicity_change_right_flag + n]-Right_Line[monotonicity_change_right_flag]))/((float)n);//这里的k是1/斜率
    float m =p*k+Right_Line[monotonicity_change_right_flag];//(x=(y-y1)*k+x1),点斜式变形
    return abs((int)(Right_Line[monotonicity_change_right_flag + p]-m));
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     十字检测
  @param     null
  @return    null
  Sample     Cross_Detect(void);
  @note      利用四个拐点判别函数，查找四个角点，根据找到拐点的个数决定是否补线
-------------------------------------------------------------------------------------------------------------------*/
void Cross_Detect()
{
    continuity_change_left_flag=Continuity_Change_Left(MT9V03X_H-1-5,35);//连续性判断
    continuity_change_right_flag=Continuity_Change_Right(MT9V03X_H-1-5,35);
    monotonicity_change_right_flag=Monotonicity_Change_Right(MT9V03X_H-1-10,35);//单调突变点判断
    monotonicity_change_left_flag=Monotonicity_Change_Left(MT9V03X_H-1-10,35);
    int down_search_start = 0;//下点搜索开始行
    //Cross_Flag = 0;
    X_Cross = 0;//十字类型清零
    if (Island_State == 0 && Img_Disappear_Flag == 0)//与环岛互斥开
    {
        Left_Up_Find = 0;
        Right_Up_Find = 0;
        Find_Up_Point(MT9V03X_H - 1, 35);//找上拐点

        if (Left_Up_Find != 0 || Right_Up_Find != 0)//只要没有同时找到两个上点，直接结束
        {
            down_search_start = Left_Up_Find > Right_Up_Find ? Left_Up_Find : Right_Up_Find;//用两个上拐点坐标靠下者作为下点的搜索上限
            Find_Down_Point(MT9V03X_H - 5, down_search_start + 2);//在上拐点下2行作为下点的截止行
        }
        if(Cross_Flag==0)
        {
            //正常正入十字
            //如果存在两个上拐点并且满足一定范围即为正入十字
            if ((Cross_Flag==0&&Cross_state==0&&Left_Up_Find <= 70 && Right_Up_Find <= 70 && Left_Up_Find >= 20 && Right_Up_Find >= 20&&Both_Lost_Time >= 10&&Left_Line[Left_Up_Find]<=118&&Right_Line[Right_Up_Find]>=70&&abs(Left_Up_Find-Right_Up_Find)<=10&&Search_Stop_Line<=85)
                    ||(Cross_Flag==0&&Cross_state==1&&Left_Up_Find <= 100 && Right_Up_Find <= 100 && Left_Up_Find >= 20 && Right_Up_Find >= 20&&Both_Lost_Time >= 10))
            {
                gpio_set_level(P02_5,1);
                Cross_Flag = 1;
                X_Cross=0;
            }
            //稍斜入十字判断
            //这个判断适用于正入分辨不出或者提前存入十字信息可用
            else if(Cross_Flag==0&&Cross_state==1&&Left_Up_Find <= 90 && Right_Up_Find <= 90 && Left_Up_Find >= 10 && Right_Up_Find >= 10&&Both_Lost_Time < 10 && (Left_Lost_Time > 10 || Right_Lost_Time > 10))
            {
                float L_avery = 0;
                float R_avery = 0;
                int L_sum = 0;
                int R_sum = 0;
                int L_Flag = 0;
                int R_Flag = 0;
                //识别到下拐点时判断
                if(Left_Down_Find != 0)
                {
                    for(int i = Left_Up_Find;i<=Left_Down_Find;i++)
                    {
                        L_sum+=Left_Line[i];
                    }
                    L_avery = (float)L_sum/(Left_Down_Find - Left_Up_Find);
                    float LUD_avery = (Left_Line[Left_Up_Find] + Left_Line[Left_Down_Find])/2;
                    if(LUD_avery - L_avery > 10){
                        L_Flag = 1;
                    }
                }
                //未识别到下拐点但存在单调转折点时
                else{
                    if(monotonicity_change_left_flag - Left_Up_Find>=10){
                            Left_Down_Find = monotonicity_change_left_flag;
                            for(int i = Left_Up_Find;i<=Left_Down_Find;i++)
                            {
                                L_sum+=Left_Line[i];
                            }
                            L_avery = (float)L_sum/(Left_Down_Find - Left_Up_Find);
                            float LUD_avery = (Left_Line[Left_Up_Find] + Left_Line[Left_Down_Find])/2;
                            if(LUD_avery - L_avery > 10){
                                L_Flag = 1;
                            }

                    }
                }
                if(Right_Down_Find != 0)
                {
                    for(int i = Right_Up_Find;i<=Right_Down_Find;i++)
                    {
                        R_sum+=Right_Line[i];
                    }
                    R_avery = (float)R_sum/(Right_Down_Find - Right_Up_Find);
                    float RUD_avery = (Right_Line[Right_Up_Find] + Right_Line[Right_Down_Find])/2;
                    if(RUD_avery - R_avery < -10)
                    {
                        R_Flag = 1;
                    }
                }
                else{
                    if(monotonicity_change_right_flag - Right_Up_Find >=10){
                        Right_Down_Find = monotonicity_change_right_flag;
                        for(int i = Right_Up_Find;i<=Right_Down_Find;i++)
                        {
                            R_sum+=Right_Line[i];
                        }
                        R_avery = (float)R_sum/(Right_Down_Find - Right_Up_Find);
                        float RUD_avery = (Right_Line[Right_Up_Find] + Right_Line[Right_Down_Find])/2;
                        if(RUD_avery - R_avery < -10)
                        {
                            R_Flag = 1;
                        }
                    }
                }
                //判断十字类型
                if(L_Flag && R_Flag)
                {
                    gpio_set_level(P02_5,1);
                    Cross_Flag = 1;
                    X_Cross = 0;
                }
            }
            //右斜入十字判断
            if((Cross_Flag==0&&Cross_state==0&&monotonicity_change_left_flag > 70 &&monotonicity_change_left_flag-continuity_change_left_flag>10&&Left_Line[monotonicity_change_left_flag] >= 80&&Right_Lost_Time >= 25//&&continuity_change_right_flag<=45
                    &&arctan_Line_Left(5,20)<=5&&Boundry_Start_Right<=100)
                    ||(Cross_Flag==0&&(Cross_state==1||Cro_Isl[CIF]==5)&&monotonicity_change_left_flag > 50 &&Left_Line[monotonicity_change_left_flag] >= 70&&Right_Lost_Time >= 30&&continuity_change_left_flag < monotonicity_change_left_flag-10&&Right_Lost_Time >= 25 ))
            {
                gpio_set_level(P02_5,1);
                Cross_Flag = 1;
                X_Cross = 1;
            }
            //左斜入十字判断
            else if((Cross_Flag==0&&monotonicity_change_right_flag > 70 &&monotonicity_change_right_flag - continuity_change_right_flag > 10&&Right_Line[monotonicity_change_right_flag] <= 108&&Left_Lost_Time >= 25//&&continuity_change_left_flag<=45
                    &&arctan_Line_Right(5,20)<=5&&Boundry_Start_Left<=100)
                    ||(Cross_Flag==0&&(Cross_state==1||Cro_Isl[CIF]==6)&&monotonicity_change_right_flag > 50 &&Right_Line[monotonicity_change_right_flag] <= 118&&continuity_change_right_flag < monotonicity_change_right_flag-10 &&Left_Lost_Time >= 25))
                {
                gpio_set_level(P02_5,1);
                    Cross_Flag = 1;
                    X_Cross = 2;
                }
            }
        //正入十字补线
        if(Cross_Flag==1&&X_Cross==0)
                {

                    if (Left_Up_Find != 0 && Right_Up_Find != 0)
                    {

                        if (Left_Down_Find != 0 && Right_Down_Find != 0)
                        {//四个点都在，无脑连线，这种情况显然很少
                           Left_Add_Line(Left_Line[Left_Up_Find], Left_Up_Find, Left_Line[Left_Down_Find], Left_Down_Find);
                           Right_Add_Line(Right_Line[Right_Up_Find], Right_Up_Find, Right_Line[Right_Down_Find], Right_Down_Find);
                        }
                        else if (Left_Down_Find == 0 && Right_Down_Find != 0)//11//这里使用的都是斜率补线
                        {//三个点                                     //01
                           Lengthen_Left_Boundry(Left_Up_Find - 1, MT9V03X_H - 1);
                           Right_Add_Line(Right_Line[Right_Up_Find], Right_Up_Find, Right_Line[Right_Down_Find], Right_Down_Find);
                        }
                        else if (Left_Down_Find != 0 && Right_Down_Find == 0)//11
                        {//三个点                                     //10
                           Left_Add_Line(Left_Line[Left_Up_Find], Left_Up_Find, Left_Line[Left_Down_Find], Left_Down_Find);
                           Lengthen_Right_Boundry(Right_Up_Find - 1, MT9V03X_H - 1);
                        }
                        else if (Left_Down_Find == 0 && Right_Down_Find == 0)//11
                        {//就俩上点                                   //00
                           Lengthen_Left_Boundry(Left_Up_Find - 1, MT9V03X_H - 1);
                           Lengthen_Right_Boundry(Right_Up_Find - 1, MT9V03X_H - 1);
                        }
                        Cross_Flag = 0;
                        Cross_state = 1;
                    }
                }
        //斜入十字补线
        else if(Cross_Flag==1&&X_Cross==1)
        {
            float k=(float)(Left_Line[monotonicity_change_left_flag + 6]-Left_Line[monotonicity_change_left_flag+1])/5.0;//这里的k是1/斜率
            for(int i=monotonicity_change_left_flag;i>=MT9V03X_H-Search_Stop_Line;i--)
            {
                Left_Line[i]=(int)(i - monotonicity_change_left_flag)*k+Left_Line[monotonicity_change_left_flag];//(x=(y-y1)*k+x1),点斜式变形
                if(Left_Line[i]>=MT9V03X_W-1)
                {
                    Left_Line[i]=MT9V03X_W-1;
                }
                else if(Left_Line[i]<=0)
                {
                    Left_Line[i]=0;
                }
        }
            if(monotonicity_change_right_flag>=85&&monotonicity_change_right_flag<=110)
            {
                float k1=(float)(Right_Line[monotonicity_change_right_flag + 6]-Right_Line[monotonicity_change_right_flag+1])/5.0;//这里的k是1/斜率
                for(int i=monotonicity_change_right_flag;i>=MT9V03X_H-Search_Stop_Line;i--)
                {
                    Right_Line[i]=(int)(i - monotonicity_change_right_flag)*k1+Right_Line[monotonicity_change_right_flag];//(x=(y-y1)*k+x1),点斜式变形
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
            Cross_Flag = 0;
            Cross_state = 1;
            //X_Cross = 0;
        }
        else if(Cross_Flag==1&&X_Cross==2)
        {
            float k=(float)(Right_Line[monotonicity_change_right_flag + 6]-Right_Line[monotonicity_change_right_flag+1])/5.0;//这里的k是1/斜率
            for(int i=monotonicity_change_right_flag;i>=MT9V03X_H-Search_Stop_Line;i--)
            {
                Right_Line[i]=(int)(i - monotonicity_change_right_flag)*k+Right_Line[monotonicity_change_right_flag];//(x=(y-y1)*k+x1),点斜式变形
                if(Right_Line[i]>=MT9V03X_W-1)
                {
                    Right_Line[i]=MT9V03X_W-1;
                }
                else if(Right_Line[i]<=0)
                {
                    Right_Line[i]=0;
                }

        }
            if(monotonicity_change_left_flag>=85&&monotonicity_change_left_flag<=110)
            {
                float k1=(float)(Left_Line[monotonicity_change_left_flag + 6]-Left_Line[monotonicity_change_left_flag+1])/5.0;//这里的k是1/斜率
                for(int i=monotonicity_change_left_flag;i>=MT9V03X_H-Search_Stop_Line;i--)
                {
                    Left_Line[i]=(int)(i - monotonicity_change_left_flag)*k1+Left_Line[monotonicity_change_left_flag];//(x=(y-y1)*k+x1),点斜式变形
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
            Cross_Flag = 0;
            Cross_state = 1;
        }
        //十字结束或者强制跳出防止影响后续元素判断
        if((abs(FJ_Angle)>=60||(Left_Up_Find > 100 && Right_Up_Find > 100)||interrupt>=100)&&Cross_state==1)
        {
            Cross_Flag = 0;
            gpio_set_level(P02_5,0);
            Cross_state = 0;
            X_Cross = 0;
            FJ_Angle = 0; //陀螺仪角度积分清零
            interrupt=0; //十字时间清零
            if(Cro_Isl[CIF]>=4&&Cro_Isl[CIF]<=6)
                CIF++;//元素数组往后延
            //printf("Cross_Start!\n");

        }

}
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     直道检测
  @param     null
  @return    null
  Sample     Straight_Detect()；
  @note      利用最长白列，边界起始点，中线起始点，
-------------------------------------------------------------------------------------------------------------------*/
void Straight_Detect(void)
{
    if(Straight_Flag==0&&Search_Stop_Line>=80&&Img_Disappear_Flag == 0 &&Island_State == 0 &&Cross_state==0&&Left_Lost_Time<=10&&Right_Lost_Time<=10&&continuity_change_left_flag<=45&&continuity_change_right_flag<=45)//截止行很远
    {
        if(Boundry_Start_Left>=110&&Boundry_Start_Right>=110&&radiu<(1e-2)/2)//曲率很小并且基本不丢线
        {
            if(-10<=error&&error<=10)//误差很小
            {
                gpio_set_level(P02_5,1);
                Straight_Flag=1;//认为是直道
            }
        }
    }
    //直道结束进弯
    else if(Straight_Flag==1&&abs(error)>15&&
            (Img_Disappear_Flag == 1 ||Island_State >= 1 ||Cross_state==1
            ||Search_Stop_Line<70))
    {
        gpio_set_level(P02_5,0);
        Straight_Flag=0;
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     丢图检测
  @param     null
  @return    null
  Sample     Stop_Detect(void)
  @note      最长白列很短，边界不连续，停车
-------------------------------------------------------------------------------------------------------------------*/
void Img_Disappear_Detect(void)
{
    int i=0;
    uint8 black_line_count=0;
    uint8 continuity_change_right_flag=0;
    uint8 continuity_change_left_flag=0;
    continuity_change_left_flag= Continuity_Change_Left (MT9V03X_H-1,MT9V03X_H-10);//连续性判断
    continuity_change_right_flag=Continuity_Change_Right(MT9V03X_H-1,MT9V03X_H-10);
    //Img_Disappear_Flag = 0;
//相关参量，debug使用
//    ips200_show_int(0,16*10,Search_Stop_Line,3);
//    ips200_show_int(0,16*11,(Longest_White_Column_Left[1]-Longest_White_Column_Right[1]),3);
//    ips200_show_int(0,16*12,Road_Wide[(MT9V03X_H-Search_Stop_Line)+1],3);
//    ips200_show_int(0,16*13,Road_Wide[(MT9V03X_H-Search_Stop_Line)+2],3);
//    ips200_show_int(0,16*14,(Standard_Road_Wide[(MT9V03X_H-Search_Stop_Line)+2]-Road_Wide[(MT9V03X_H-Search_Stop_Line)+2]),3);
//    ips200_show_int(0,16*15,(Standard_Road_Wide[(MT9V03X_H-Search_Stop_Line)+1]-Road_Wide[(MT9V03X_H-Search_Stop_Line)+1]),3);
//    ips200_show_int(0,16*16,Boundry_Start_Left+Left_Lost_Time,3);
//    ips200_show_int(0,16*17,Boundry_Start_Right+Right_Lost_Time,3);

    if(continuity_change_left_flag!=0&&continuity_change_right_flag!=0&&Search_Stop_Line<=20)
    {//不连续，且搜索截止行非常短
        Img_Disappear_Flag=1;//认为丢图
        return;
    }
    else
    {
        for(i= MT9V03X_H-1; i>MT9V03X_H-1-10; i--)//选定区域全黑，认为丢图了
        {
            if(image_two_value[i][4]==IMG_BLACK&&image_two_value[i][5]==IMG_BLACK&&
                    image_two_value[i][10]==IMG_BLACK&&image_two_value[i][15]==IMG_BLACK&&
                    image_two_value[i][20]==IMG_BLACK&&image_two_value[i][25]==IMG_BLACK&&
                    image_two_value[i][64]==IMG_BLACK&&image_two_value[i][65]==IMG_BLACK&&
                    image_two_value[i][66]==IMG_BLACK&&image_two_value[i][67]==IMG_BLACK&&
                    image_two_value[i][68]==IMG_BLACK&&image_two_value[i][69]==IMG_BLACK&&
                    image_two_value[i][70]==IMG_BLACK&&image_two_value[i][75]==IMG_BLACK&&
                    image_two_value[i][66]==IMG_BLACK&&image_two_value[i][67]==IMG_BLACK&&
                    image_two_value[i][80]==IMG_BLACK&&image_two_value[i][90]==IMG_BLACK&&
                    image_two_value[i][100]==IMG_BLACK&&image_two_value[i][110]==IMG_BLACK&&
                    image_two_value[i][115]==IMG_BLACK&&image_two_value[i][120]==IMG_BLACK&&
                    image_two_value[i][125]==IMG_BLACK&&image_two_value[i][130]==IMG_BLACK&&
                    image_two_value[i][135]==IMG_BLACK&&image_two_value[i][136]==IMG_BLACK&&
                    image_two_value[i][137]==IMG_BLACK&&image_two_value[i][138]==IMG_BLACK
            )
                black_line_count++;
        }
        if(black_line_count>=5)
        {
            Img_Disappear_Flag=1;
        }
    }
}

void Zebra_Stripes_Detect()
{
    int i=0,j=0;
    int change_count=0;//跳变计数
    int start_line=0;
    int endl_ine=0;
    int narrow_road_count=0;
    if(Cross_Flag!=0||Zebra_Stripes_Flag!=0||Stop_Flag!=0||Island_State!=0||Img_Disappear_Flag!=0||Time<40)//元素互斥，不是十字，不是，不是坡道，不是停车
    {
        return;
    }
    ////赛宽变化判斑马线
    if(Search_Stop_Line>=60&&
       30<=Longest_White_Column_Left[1]&&Longest_White_Column_Left[1]<=MT9V03X_W-30&&
       30<=Longest_White_Column_Right[1]&&Longest_White_Column_Right[1]<=MT9V03X_W-30&&
       Boundry_Start_Left>=MT9V03X_H-15&&Boundry_Start_Right>=MT9V03X_H-15)
    {//截止行长，.最长白列的位置在中心附近，边界起始点靠下
        for(i=MT9V03X_H-1;i>=MT9V03X_H-30;i--)//在靠下的区域进行寻找赛道宽度过窄的地方
        {
            if(abs(Left_Line[i]-Right_Line[i])<30)
            {
                narrow_road_count++;//多组赛宽变窄，才认为是斑马线
                if(narrow_road_count>=5)
                {
                    start_line=i;//记录赛道宽度很窄的位置
                    break;
                }
            }
        }
    }
    if(start_line!=0)//多组赛宽变窄，，以赛道过窄的位置为中心，划定一个范围，进行跳变计数
    {
        start_line=start_line+8;
        endl_ine=start_line-15;
        if(start_line>=MT9V03X_H-1)//限幅保护，防止数组越界
        {
            start_line=MT9V03X_H-1;
        }
        if(endl_ine<=0)//限幅保护，防止数组越界
        {
            endl_ine=0;
        }
        for(i=start_line;i>=endl_ine;i--)//区域内跳变计数
        {
            for(j=MT9V03X_W/4;j<=MT9V03X_W*0.75;j++)
            {
                if(image_two_value[i][j+1]-image_two_value[i][j]!=0)
                {
                    change_count++;
                }
            }
        }
//        ips200_show_int(0*16,100,change_count,5);//debug使用，查看跳变数，便于适应赛道
    }
    if(change_count>45)//跳变大于某一阈值，认为找到了斑马线
    {
        Zebra_Stripes_Flag=1;
    }
//相关参数显示。debug使用
//    ips200_show_uint(0*16,50,narrow_road_count,5);
//    ips200_show_uint(1*16,50,change_count,5);
}


/*-------------------------------------------------------------------------------------------------------------------
  @brief     计算偏差
  @param     null
  @return    偏差
  Sample
  @note
-------------------------------------------------------------------------------------------------------------------*/
//图像调试用的函数

float Point_real = 0;
int32 Cal_ERR(void)
{
    uint8 i;
    float Sum = 0;
    float Weight_count = 0;
    float ERR_ceshi = 0;
    float Point_aim = ((float)(MT9V03X_W - 1)) / 2;
    int32 ERR = 0;
    Weight_change(); //动态权重
    //error计算
    for (i = MT9V03X_H-1; i > MT9V03X_H-Search_Stop_Line; i--) //此处需要修改
    {
        Mid_Line[i] = (Left_Line[i] + Right_Line[i]) >> 1;
        Sum += Mid_Line[i] * weight_image[i];
        Weight_count += weight_image[i];
    }
    Point_real = Sum / Weight_count;

    ERR_ceshi = Point_real - Point_aim;
    ERR = ERR_ceshi * 2;//这里是为了增大分辨率

    return ERR;
}

int32 error = 0;

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
    //system_start();
    turn_to_bin();
    //printf("%.2f\n",(float)system_getval()/100000);
    Longest_White_Column();
    Img_Disappear_Detect();
    Zebra_Stripes_Detect();
    //元素判断既可以选择存档读取判断也可以选择直接判断，看个人喜好不过如果后者需要稍微修改代码
    if(Cro_Isl[CIF]>=1&&Cro_Isl[CIF]<=3)
        Island_Detect();
    if(Cro_Isl[CIF]>=4&&Cro_Isl[CIF]<=6)
    Cross_Detect();
    error = Cal_ERR();
    Straight_Detect();
    Show_Boundry();
    Value_isp_Flag = 1;
}
