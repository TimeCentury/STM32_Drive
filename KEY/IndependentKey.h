#ifndef _INDEPENDENT_KEY_H_
#define _INDEPENDENT_KEY_H_

#include "stm32f4xx.h"

#define USE_EXTENSIONS 1
#define LONG_CLICK_TIME_1 100 //第一次触发长按，为IndependentKey_Scan函数调用的间隔时间*LONG_CLICK_TIME_1
#define LONG_CLICK_TIME_2 50//触发长按后的连续触发时间

//优先级 单击>双击>长按
typedef struct 
{
	uint32_t		GPIO_CLK;
	GPIO_TypeDef*	GPIO_PORT; 
	uint16_t		GPIO_PIN;//0~15
	uint8_t Number;//编号，在点击时返回 
} IndependentKey_TypeDef;

/** 
 * @brief 添加按键 
 * @param uint32_t GPIO_CLK GPIO端口时钟
 * @param GPIO_TypeDef* GPIO_PORT GPIO端口
 * @param uint16_t GPIO_PIN GPIO端口引脚
 * @param uint8_t Number 编号，在点击时返回
 */
void IndependentKey_Add(uint32_t GPIO_CLK, GPIO_TypeDef* GPIO_PORT, uint16_t GPIO_PIN, uint8_t Number);
/** 
 * @brief 扫描按键,应没10ms调用一次
 * @param 
 * @retval 约定返回的低16位为按键按下的编号，高16位bit0为长按标志
 */
uint32_t IndependentKey_Scan();

#endif
