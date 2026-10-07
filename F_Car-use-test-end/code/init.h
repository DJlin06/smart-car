/*
 * init.h
 *
 *  Created on: 2025年1月16日
 *      Author: DJL
 */
#ifndef CODE_INIT_H_
#define CODE_INIT_H_
void All_init();//初始化函数
void DIR_PULSE(int16 *DIR_DIR_R,int16 *DIR_DIR_L);  //编码器
void Print(int *m);//输出电机速度
void Isp_init();
#endif /* CODE_INIT_H_ */
