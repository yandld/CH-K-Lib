/**
  ******************************************************************************
  * @file    common.c
  * @author  YANDLD
  * @version V2.5
  * @date    2013.12.25
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "common.h"
#include <string.h>
#include "systick.h"

 /**
 * @brief  编码快速初始化结构 用户一般不需调用
 *
 * @param  type: I2C快速初始化结构
 * @retval 32位快速初始化编码
 */
uint32_t QuickInitEncode(QuickInit_Type type)
{
    uint32_t value = 0;
    value = type.ip_instance<<0;
    value|= type.io_instance<<3;
    value|= type.mux<<6;
    value|= type.io_base<<9;
    value|= type.io_offset<<14;
    value|= type.channel<<19;
    return value;
}

 /**
 * @brief  解码快速初始化结构 用户一般不需调用
 *
 * @param  map: 32位快速初始化编码
 * @param  type: I2C快速初始化结构指针
 * @retval None
 */
void QuickInitDecode(uint32_t map, QuickInit_Type* type)
{
    QuickInit_Type * pMap = (QuickInit_Type*)&(map);
    memcpy(type, pMap, sizeof(QuickInit_Type));  
}

#pragma weak DelayMs
void DelayMs(uint32_t ms)
{
    SYSTICK_DelayMs(ms);
}

#pragma weak DelayUs
void DelayUs(uint32_t ms)
{
    SYSTICK_DelayUs(ms);
}

#pragma weak DelayInit
void DelayInit(void)
{
    SYSTICK_DelayInit();
}





