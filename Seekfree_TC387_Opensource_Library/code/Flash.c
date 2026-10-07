/*
 * Flash.c
 *
 *  Created on: 2025年5月14日
 *      Author: DJL
 */
#include "zf_common_headfile.h"
#define FLASH_SECTION_INDEX       (0)                                 // 存储数据用的扇区
#define FLASH_PAGE_INDEX          (127)                               // 存储数据用的页码 倒数第一个页码
void Flash_write(){
    if(flash_check(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX))                      // 判断是否有数据
        flash_erase_page(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);                // 擦除这一页
    flash_buffer_clear();
    flash_union_buffer[0].float_type  = sp_KP;                              // 向缓冲区第 0 个位置写入 float  数据
    flash_union_buffer[1].float_type = sp_KI;                             // 向缓冲区第 1 个位置写入 float 数据
    flash_union_buffer[2].float_type  = sp_KD;                            // 向缓冲区第 2 个位置写入 float  数据
    flash_union_buffer[3].int32_type = base_speed;                                  // 向缓冲区第 3 个位置写入 int32 数据
    flash_union_buffer[4].float_type  = Cha_KP;                                 // 向缓冲区第 4 个位置写入 float  数据
    flash_union_buffer[5].float_type  = Cha_KI;                                    // 向缓冲区第 5 个位置写入 float  数据
    flash_union_buffer[6].float_type   = Cha_KD;
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);
}
void Flash_read(){
    if(flash_check(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX))                      // 判断是否有数据
        {flash_buffer_clear();
        flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);
        sp_KP = flash_union_buffer[0].float_type;
        sp_KI = flash_union_buffer[1].float_type;
        sp_KD = flash_union_buffer[2].float_type;
        base_speed = flash_union_buffer[3].int32_type;
        Cha_KP = flash_union_buffer[4].float_type;
        Cha_KI = flash_union_buffer[5].float_type;
        Cha_KD = flash_union_buffer[6].float_type;
        }
    else printf("Flash NULL!");
}
