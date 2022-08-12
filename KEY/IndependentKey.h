#ifndef _INDEPENDENT_KEY_H_
#define _INDEPENDENT_KEY_H_

#include "stm32f4xx.h"

#define USE_EXTENSIONS 1

//优先级 单击>双击>长按
typedef struct 
{
	uint32_t		GPIO_CLK;
	GPIO_TypeDef*	GPIO_PORT; 
	uint16_t		GPIO_PIN;//0~15
	uint8_t Number;//编号，在点击时返回 
} IndependentKey_TypeDef;


#endif
