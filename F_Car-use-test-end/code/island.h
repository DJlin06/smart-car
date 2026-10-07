/*
 * island.h
 *
 *  Created on: 2025Äê5ÔÂ7ÈÕ
 *      Author: DJL
 */
#ifndef CODE_ISLAND_H_
#define CODE_ISLAND_H_
void Island_Detect(void);
void Image_Flag_Show(uint8 MT9V03XW,uint8(*InImg)[MT9V03XW],uint8 image_flag);
int Monotonicity_Change_Right(int start,int end);
int Monotonicity_Change_Left(int start,int end);
int Continuity_Change_Right(int start,int end);
int Continuity_Change_Left(int start,int end);
int Find_Left_Down_Point(int start,int end);
int Find_Left_Up_Point(int start,int end);
int Find_Right_Down_Point(int start,int end);
int Find_Right_Up_Point(int start,int end);
int Get_Road_Wide(int start_line,int end_line);
void K_Add_Boundry_Left(float k,int startX,int startY,int endY);
void K_Add_Boundry_Right(float k,int startX,int startY,int endY);
void K_Draw_Line(float k, int startX, int startY,int endY);
float Get_Right_K(int start_line,int end_line);
float Get_Left_K(int start_line,int end_line);
void Draw_Line(int x0, int y0, int x1, int y1);
#endif /* CODE_ISLAND_H_ */
