/** 
 * @Author: 时间世纪
 * @Date: 2021-11-06 13:35:00
 * @Description: 软件IIC驱动程序
 */
#ifndef _SOFTWARE_IIC_H_
#define _SOFTWARE_IIC_H_

#include "stm32f4xx.h"

#define IIC_DELAY_TIME 1 //延时时间

//包含操作IO口的一些必要信息
typedef	struct
{
	uint32_t		GPIO_CLK;
	GPIO_TypeDef*	GPIO_PORT; 
	uint16_t		GPIO_PIN;//0~15
} IIC_GPIO_TypeDef;

//一个软件IIC设备
typedef	struct
{
	IIC_GPIO_TypeDef	SCL;
	IIC_GPIO_TypeDef	SDA;
	uint8_t ADD;//设备地址，内部使用时没有移位，只设置了读写位
} IIC_TypeDef;

/** 
 * @brief 初始化一个IIC设备
 * @param IIC_TypeDef *pIIC
 * @retval: 
 */
extern void IIC_Init(IIC_TypeDef * const pIIC);
/** 
 * @brief 开始一次IIC通信
 * @param IIC_TypeDef *pIIC
 * @retval 
 */
extern void IIC_Start(IIC_TypeDef *const pIIC);
/** 
 * @brief 停止一次IIC通信
 * @param IIC_TypeDef *pIIC
 * @retval 
 */
extern void IIC_Stop(IIC_TypeDef *const pIIC);
/** 
 * @brief 等待从机应答
 * @param IIC_TypeDef *pIIC
 * @retval 0：有应答， 1：无应答
 */
extern uint8_t IIC_Sack(IIC_TypeDef *const pIIC);
/** 
 * @brief 发送一字节数据
 * @param IIC_TypeDef *pIIC
 * @param uint8_t dat
 * @retval 
 */
extern void IIC_SendByte(IIC_TypeDef *const pIIC, uint8_t dat);
/** 
 * @brief 发送多个数据
 * @param IIC_TypeDef *pIIC
 * @param uint8_t * dat
 * @param uint8_t len 长度
 * @retval 0：发送成功，1: 失败
 */
extern uint8_t IIC_Send(IIC_TypeDef *const pIIC, uint8_t *dat, uint8_t len);
/** 
 * @brief 读取一字节数据
 * @param IIC_TypeDef *pIIC
 * @param uint8_t ack 是否对从机进行应答继续读取，0：否停止读取，1：继续读取
 * @retval 
 */
extern uint8_t IIC_ReadByte(IIC_TypeDef *const pIIC, uint8_t ack);
/** 
 * @brief 读取多个数据
 * @param IIC_TypeDef *pIIC
 * @param uint8_t * dat
 * @param uint8_t len 长度
 */
extern void IIC_Read(IIC_TypeDef *const pIIC, uint8_t *dat, uint8_t len);
/** 
 * @brief 发送写地址并等待应答
 * @param IIC_TypeDef *pIIC
 * @retval 0：有应答，1：无应答
 */
extern __inline uint8_t IIC_SendWriteAddress(IIC_TypeDef *const pIIC);
/** 
 * @brief 发送读地址并等待应答
 * @param IIC_TypeDef *pIIC
 * @retval 0：有应答，1：无应答
 */
extern __inline uint8_t IIC_SendReadAddress(IIC_TypeDef *const pIIC);
/** 
 * @brief 发送一个字节并检查从机应答
 * @param IIC_TypeDef *pIIC
 * @param uint8_t dat
 * @retval 0：有应答，1：无应答
 */
extern __inline uint8_t IIC_SendAndSack(IIC_TypeDef *const pIIC, uint8_t dat);

#endif
