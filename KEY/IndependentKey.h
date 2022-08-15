/** 
 * @Author: 时间世纪
 * @Date: 2022-08-14 20:34:43
 * @Description: 
 */
#ifndef _INDEPENDENT_KEY_H_
#define _INDEPENDENT_KEY_H_

#include "stm32f4xx.h"
#include "HAL_Driver.h"
#include "ITM.h"

#define USE_EXTENSIONS 1
#define INDEPENDENT_KEY_COUNT 10
#define LONG_CLICK_TIME_1 100 //第一次触发长按，为IndependentKey_Scan函数调用的间隔时间*LONG_CLICK_TIME_1
#define LONG_CLICK_TIME_2 50//触发长按后的连续触发时间

//优先级 单击>双击>长按
typedef struct 
{
	HAL_GPIO_TypeDef Key;
	uint8_t Number;//编号，在点击时返回 
} IndependentKey_TypeDef;

/** 
 * @brief 添加按键 
 * @param uint32_t GPIO_CLK GPIO端口时钟
 * @param GPIO_TypeDef* GPIO_PORT GPIO端口
 * @param uint16_t GPIO_PIN GPIO端口引脚
 * @param uint8_t Number 编号，在点击时返回
 */
extern void IndependentKey_Add(uint32_t GPIO_CLK, GPIO_TypeDef* GPIO_PORT, uint16_t GPIO_PIN, uint8_t Number);
/** 
 * @brief 扫描按键,应每10ms调用一次
 * @param 
 * @retval 约定返回的低8位为按键按下的编号，高8位bit0为长按标志
 */
extern uint16_t IndependentKey_Scan();

#endif
