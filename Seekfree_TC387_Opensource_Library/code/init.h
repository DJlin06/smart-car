/*
 * init.h
 *
 *  Created on: 2025年1月16日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
#ifndef CODE_INIT_H_
#define CODE_INIT_H_
void All_init();//初始化函数
void DIR_PULSE(int16 *DIR_DIR_R,int16 *DIR_DIR_L);  //编码器
void Print();//输出电机速度
#endif /* CODE_INIT_H_ */
