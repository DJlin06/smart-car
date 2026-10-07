/*
 * images.h
 *
 *  Created on: 2024年12月21日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
#ifndef CODE_IMAGES_H_
#define CODE_IMAGES_H_
typedef struct {
    int label;
    int area;
    int perimeter;
    float centroid_x;
    float centroid_y;
    float circularity;
} RegionInfo;
int my_abs(int value);
float my_abs_float(float value);
int16 limit1(int16 x, int16 y);
void Get_image(uint8(*index)[MT9V03X_W]);
uint8 otsuThreshold(uint16 *image, uint16 col, uint16 row);
void turn_to_bin(void);
void connected_component_labeling() ;
void find_circles(RegionInfo* circles, int max_circles);
//int32 Cal_ERR(void);
void Get_err(RegionInfo*circles);
void Img_dissapear();
void image_filter(uint8(*bin_image)[MT9V03X_W]);//形态学滤波，简单来说就是膨胀和腐蚀的思想
void image_draw_rectan(uint8(*image)[MT9V03X_W]);
void image_process(void); //直接在中断或循环里调用此程序就可以循环执行了
float cauclate_variance(float average,float num);
int32 LQ_Deal_Image(RegionInfo*circles);
#endif /* CODE_IMAGES_H_ */
