/*
 * Flash.c
 *
 *  Created on: 2025年5月14日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
#define FLASH_SECTION_INDEX       (0)                                 // 存储数据用的扇区
#define FLASH_PAGE_INDEX          (127)                               // 存储数据用的页码 倒数第一个页码
void Flash_write()
{
    if(flash_check(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX))                      // 判断是否有数据
        flash_erase_page(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);                // 擦除这一页
    flash_buffer_clear();
    flash_union_buffer[0].float_type = Velocity_KP;                              // 向缓冲区第 0 个位置写入 float  数据
    flash_union_buffer[1].float_type = Velocity_KI;                             // 向缓冲区第 1 个位置写入 float 数据
    flash_union_buffer[2].float_type = Velocity_KD;                            // 向缓冲区第 2 个位置写入 float  数据
    flash_union_buffer[3].int32_type = Speed;                                  // 向缓冲区第 3 个位置写入 int32 数据
    flash_union_buffer[4].float_type = Cha_KP;                                 // 向缓冲区第 4 个位置写入 float  数据
    flash_union_buffer[5].float_type = Cha_KI;                                    // 向缓冲区第 5 个位置写入 float  数据
    flash_union_buffer[6].float_type = Cha_KD;
    flash_union_buffer[7].int32_type = Isp_Speed;
    flash_union_buffer[8].int32_type = Island_Speed_S;
    flash_union_buffer[9].int32_type = Straight_Speed;
    flash_union_buffer[10].int32_type = Time_max;
    flash_union_buffer[11].int32_type = Speed_Max;
    flash_union_buffer[12].int32_type = Speed_Min;
    flash_union_buffer[13].int32_type = radiu_k;
    flash_union_buffer[14].int32_type = EXP_TIME;
    flash_union_buffer[15].int32_type = Change_Speed;
    flash_union_buffer[16].int32_type = Ness_pressure;
    flash_union_buffer[17].int32_type = Handao;
    flash_union_buffer[18].int32_type = Island_Speed_M;
    flash_union_buffer[19].int32_type = Island_Speed_L;
    //向相应缓冲区内存下对应元素
    for(int i=20;i<35;i++)
    {
        flash_union_buffer[i].int32_type = Cro_Isl[i-20];
    }
    //向相应缓冲区内存下对应元素前速度
    for(int i=36;i<36+15;i++)
    {
        flash_union_buffer[i].int32_type = CI_Speed[i-36];
    }
    flash_union_buffer[35].int32_type = Y_MEET;     //最大截止行
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);    //写入数据
}
/*
 *  函数作用：初始化时读取FLASH内容
 *  变量存储位置与FLASH_Write()对应
 */
void Flash_read()
{
    if(flash_check(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX))                      // 判断是否有数据
        {flash_buffer_clear();
        flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);
        Velocity_KP = flash_union_buffer[0].float_type;
        Velocity_KI = flash_union_buffer[1].float_type;
        Velocity_KD = flash_union_buffer[2].float_type;
        Speed = 0;//flash_union_buffer[3].int32_type;
        Cha_KP = flash_union_buffer[4].float_type;
        Cha_KI = flash_union_buffer[5].float_type;
        Cha_KD = flash_union_buffer[6].float_type;
        Isp_Speed   = flash_union_buffer[7].int32_type;
        Island_Speed_S = flash_union_buffer[8].int32_type;
        Straight_Speed = flash_union_buffer[9].int32_type;
        Time_max =flash_union_buffer[10].int32_type;
        Speed_Max = flash_union_buffer[11].int32_type;
        Speed_Min = flash_union_buffer[12].int32_type;
        radiu_k = flash_union_buffer[13].int32_type;
        EXP_TIME = flash_union_buffer[14].int32_type;
        Change_Speed = flash_union_buffer[15].int32_type;
        Ness_pressure = flash_union_buffer[16].int32_type;
        Handao = flash_union_buffer[17].int32_type;
        Island_Speed_M = flash_union_buffer[18].int32_type;
        Island_Speed_L = flash_union_buffer[19].int32_type;
        for(int i=20;i<35;i++)
        {
            Cro_Isl[i-20] = flash_union_buffer[i].int32_type;
        }
        for(int i=36;i<36+15;i++)
        {
            CI_Speed[i-36] = flash_union_buffer[i].int32_type;
        }
        Y_MEET = flash_union_buffer[35].int32_type;
        mt9v03x_set_exposure_time(EXP_TIME);
        }
    else printf("Flash NULL!"); //读取不成功时出现输出
}
