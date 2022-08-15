/** 
 * @Author: 时间世纪
 * @Date: 2022-08-11 13:47:34
 * @Description: EEPROM驱动程序
 */
#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "Software_IIC.h"
#include "HAL_Driver.h"

#define EEPROM_BUSY_CNT 10 //EEPROM忙等待次数

typedef enum
{
	AT24C02 = 0,
	AT24C04,
	AT24C08,
	AT24C16,
	AT24C32 = 4,
	AT24C64,
	AT24C128,
	AT24C256 = 7,
} EEPROM_Type;

typedef struct
{
	IIC_TypeDef IIC;//IIC接口
	EEPROM_Type Type;//EEPROM类型
	uint16_t PageSize;//页大小
	uint16_t PageCount;//页数
	uint16_t TotalCapacity;//总容量
	//因为EEPROM器件型号不同启动传输的操作也有所不同，所以使用的启动方式也有所不同，如AT24C04在启动一次传输时需要占用一个bit的器件地址做寻址地址
	uint8_t (*EEPROM_StartTransmission)(void* self, uint16_t address);
} EEPROM_TypeDef;

extern EEPROM_TypeDef EEPROM_AT24C02;
extern EEPROM_TypeDef EEPROM_AT24C256;

/** 
 * @brief 初始化EEPROM
 * @param EEPROM_TypeDef *pEEPROM
 * @retval 0：成功，1：失败
 */
extern uint8_t EEPROM_Init(EEPROM_TypeDef * const pEEPROM, EEPROM_Type type);
/** 
 * @brief 
 * @param EEPROM_TypeDef *pEEPROM
 * @retval 0：成功，其他失败
 */
extern uint8_t EEPROM_Check(EEPROM_TypeDef * const pEEPROM);
/** 
 * @brief 写入一个字节
 * @param EEPROM_TypeDef* pEEPROM
 * @param uint16_t address
 * @param uint8_t dat
 * @retval 0：成功，其他失败
 */
extern uint8_t EEPROM_WriteByte(EEPROM_TypeDef * const pEEPROM, uint16_t address, uint8_t dat);
/** 
 * @brief 写入多个数据
 * @param EEPROM_TypeDef* pEEPROM
 * @param uint16_t address
 * @param uint8_t * memory 数据源内存地址
 * @param uint16_t size
 * @retval 0：成功，其他失败
 */
extern uint8_t EEPROM_Write(EEPROM_TypeDef * const pEEPROM, uint16_t address, uint8_t * memory, uint16_t size);
/** 
 * @brief 读一个字节
 * @param EEPROM_TypeDef* pEEPROM
 * @param uint16_t address
 * @param uint8_t * dat 读取的数据
 * @retval 0：成功，其他失败
 */
extern uint8_t EEPROM_ReadByte(EEPROM_TypeDef * const pEEPROM, uint16_t address, uint8_t * dat);
/** 
 * @brief 读取多个数据
 * @param EEPROM_TypeDef* pEEPROM
 * @param uint16_t address
 * @param uint8_t * memory 读取到的目标内存地址
 * @param uint16_t size
 * @retval 0：成功，其他失败
 */
extern uint8_t EEPROM_Read(EEPROM_TypeDef * const pEEPROM, uint16_t address, uint8_t * memory, uint16_t size);

#endif
