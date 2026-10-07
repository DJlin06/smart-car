/*
 * pid.h
 *
 *  Created on: 2025年1月11日
 *      Author: DJL
 */
#ifndef CODE_PID_H_
#define CODE_PID_H_
int Position_PID(int Position,int target,float KP,float KI,float KD,float *Bias,float *Inteal_Bias,float *Last_Bias,float *Output,float MAX,float MIN);//位置式PID
void PID_send();//无线调参
int image_Position_PID(int Position,int target,float KP,float KI,float KD,float *Bias,float *Inteal_Bias,float *Last_Bias,int *Output,int MAX,int MIN);//位置式PID
void motor_ctrl(int image_error,int Speed_Target,int16 Speed_R,int16 Speed_L);//电机控制
void Speed_Choice();
#endif /* CODE_PID_H_ */
