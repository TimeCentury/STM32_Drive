/** 
 * @Author: 时间世纪
 * @Date: 2021-08-05 09:31:54
 * @Description: 硬件驱动层
 */
#ifndef _HAL_DRIVER_H_
#define _HAL_DRIVER_H_

#include "stm32f4xx.h"
#include "ITM.h"
#include "lvgl.h"

//包含控制IO口的必要条件
typedef struct 
{
    uint32_t		GPIO_CLK;
	GPIO_TypeDef*	GPIO_PORT; 
	uint16_t		GPIO_PIN;//0~15
}HAL_GPIO_TypeDef;


typedef struct
{
    uint32_t DelayMs;
    uint8_t TriggerCntMax;
    uint64_t TriggerTime;
    uint8_t TriggerCnt;
}TimeBaseFlag_TypeDef;

void  SysTick_Init (uint32_t cnts);

/** 
 * @brief 微妙级延时
 * @param uint32_t t
 * @retval 
 */
extern void Delay_us(uint32_t us);

/** 
 * @brief 毫妙级延时
 * @param uint32_t t
 * @retval 
 */
extern void Delay_ms(uint32_t ms);

__STATIC_FORCEINLINE void HAL_GPIO_Init(HAL_GPIO_TypeDef * pGPIO)
{
    RCC->AHB1ENR |= pGPIO->GPIO_CLK;
}

uint8_t TimeBaseFlag(TimeBaseFlag_TypeDef * pTimeBaseFlag);

/** 
 * @brief 设置端口电平
 * @param HAL_GPIO_TypeDef * pGPIO
 * @retval 
 */
__STATIC_FORCEINLINE void HAL_GPIO_High(HAL_GPIO_TypeDef * pGPIO)
{
    pGPIO->GPIO_PORT->BSRRL = 1 << pGPIO->GPIO_PIN;
}

/** 
 * @brief 设置端口电平
 * @param HAL_GPIO_TypeDef * pGPIO
 * @retval 
 */
__STATIC_FORCEINLINE void HAL_GPIO_Low(HAL_GPIO_TypeDef * pGPIO)
{
	pGPIO->GPIO_PORT->BSRRH = 1 << pGPIO->GPIO_PIN;
}

/** 
 * @brief 获取端口输入
 * @param HAL_GPIO_TypeDef * pGPIO
 * @retval 
 */
__STATIC_FORCEINLINE uint8_t HAL_GPIO_Read(HAL_GPIO_TypeDef * pGPIO)
{
    return ((pGPIO->GPIO_PORT->IDR & (1 << pGPIO->GPIO_PIN)) != 0) ? 1 : 0;
}

/** 
 * @brief 将端口设置为输出 
 * @param HAL_GPIO_TypeDef * pGPIO
 * @retval 
 */
__STATIC_FORCEINLINE void HAL_GPIO_SetOutput(HAL_GPIO_TypeDef * pGPIO)
{
    uint16_t temp = pGPIO->GPIO_PIN * 2;
    pGPIO->GPIO_PORT->MODER &= ~(0x00000003 << temp);
    pGPIO->GPIO_PORT->MODER |= (0x00000001 << temp);//设置为输出
	pGPIO->GPIO_PORT->OSPEEDR &= ~(0x00000003 << temp);
    pGPIO->GPIO_PORT->OSPEEDR |= (0x00000002 << temp);//设置为高速
    pGPIO->GPIO_PORT->OTYPER &= ~(0x00000001 << pGPIO->GPIO_PIN);//设置为推挽输出
    pGPIO->GPIO_PORT->PUPDR &= ~(0x00000003 << temp);//设置为无上下拉
}

/** 
 * @brief 将端口设置为输入
 * @param HAL_GPIO_TypeDef * pGPIO
 * @retval 
 */
__STATIC_FORCEINLINE void HAL_GPIO_SetInput(HAL_GPIO_TypeDef * pGPIO)
{
    uint16_t temp = pGPIO->GPIO_PIN * 2;
    pGPIO->GPIO_PORT->MODER &= ~(0x00000003 << temp);//设置为输入
    pGPIO->GPIO_PORT->PUPDR &= ~(0x00000003 << temp);
    pGPIO->GPIO_PORT->PUPDR |= (0x00000001 << temp);//设置为上拉
}

#endif
