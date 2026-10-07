/*
 * images.h
 *
 *  Created on: 2024年12月21日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
#ifndef CODE_IMAGES_H_
#define CODE_IMAGES_H_
int my_abs(int value);
int16 limit1(int16 x, int16 y);
void Get_image(uint8(*index)[MT9V03X_W]);
uint8 otsuThreshold(uint8 *image, uint16 col, uint16 row);
void turn_to_bin(void);
void Longest_White_Column();//最长白列巡线
void Show_Boundry(void);
void image_process(void); //直接在中断或循环里调用此程序就可以循环执行了
void Left_Add_Line(int x1, int y1, int x2, int y2);//左补线,补的是边界
void Right_Add_Line(int x1, int y1, int x2, int y2);//右补线,补的是边界
void Find_Down_Point(int start, int end);
void Find_Up_Point(int start, int end);
void Lengthen_Left_Boundry(int start,int end);
void Lengthen_Right_Boundry(int start,int end);
void Cross_Detect();
int32 Cal_ERR(void);
#endif /* CODE_IMAGES_H_ */
