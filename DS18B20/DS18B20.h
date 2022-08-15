#ifndef _DS18B20_H_
#define _DS18B20_H_

#include "HAL_Driver.h"

typedef struct
{
    HAL_GPIO_TypeDef DIO;//数据输入输出端口
} OneWire_TypeDef;


typedef	struct {
	OneWire_TypeDef DQ;//数据输入输出端口
} DS18B20_TypeDef;

/** 
 * @brief 初始化一个DS18B20设备
 * @param DS18B20_TypeDef *pDS18B20
 * @retval: 
 */
extern void DS18B20_INIT(DS18B20_TypeDef * const pDS18B20);
/** 
 * @brief 启动一个温度转换
 * @param DS18B20_TypeDef *pDS18B20
 * @retval 0:成功 1:失败
 */
extern uint8_t DS18B20_Convert(DS18B20_TypeDef * const pDS18B20);
/** 
 * @brief 读取温度值
 * @param DS18B20_TypeDef *pDS18B20
 * @param float *temperature 温度值
 * @retval 0:成功 1:失败
 */
extern uint8_t DS18B20_ReadTemperature(DS18B20_TypeDef * const pDS18B20, float *temperature);

#endif
