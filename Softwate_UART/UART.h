/** 
 * @Author: 时间世纪
 * @Date: 2022-08-15 15:53:26
 * @Description: 模拟串口,应定时调用UART_S_ReadDrive，UART_S_SendDrive函数，
 * UART_S_SendDrive定时时间为 1秒 / 想要的波特率
 * UART_S_ReadDrive定时时间为 1秒 / 想要的波特率 / 3，除3是因为对1bit要进行三次采样以次数多的为准，如三次采样中高电平次数为2次，则为高电平
 * 否则为低电平，进行三次采样的原因为防止采样点落在bit的边缘上，而进行三次采样即使有一次落在边缘上也可以保证数据的准确性，不建议超过9600波特率
 * 如想要9600波特率，则定时时间为1秒 / 9600 / 3 = 34.7us，所以UART_S_ReadDrive为每34us调用一次，UART_S_SendDrive为每102us调用一次
 * 如果想要4800波特率，则定时时间为1秒 / 4800 / 3 = 69.4us，所以UART_S_ReadDrive为每69us调用一次，UART_S_SendDrive为每207us调用一次
 * 如果想要1200波特率，则定时时间为1秒 / 1200 / 3 = 277.7us，所以UART_S_ReadDrive为每277us调用一次，UART_S_SendDrive为每831us调用一次
 * 
 */

#ifndef _UART_H_
#define _UART_H_

#include "HAL_Driver.h"
#include <stdio.h>
#include <stdarg.h>

typedef struct
{
    HAL_GPIO_TypeDef TX;
    HAL_GPIO_TypeDef RX;
    uint16_t RxState : 3;//发送状态
    uint16_t RxTestCnt : 3;//读取IO计数
    uint16_t RxHighCnt : 3;//高电平计数
    uint16_t RxBitCnt : 4;//接收位计数
    uint16_t RxReserve : 3;//保留
    uint8_t RxData;//接收数据
    uint8_t TxState : 3;//发送状态
    uint8_t TxBitCnt : 4;//发送位计数
    uint8_t TxBusy : 1;//发送忙标志
    uint8_t TxData;//发送数据
    uint8_t *TxBuf;//发送缓冲区
    uint8_t TxBufLen;//发送缓冲区长度
    uint8_t TxHeadIndex;//发送缓冲区头指针
    uint8_t TxTailIndex;//发送缓冲区尾指针
    uint8_t TxBufDataCnt;//发送缓冲区数据个数
    void (*ReadCallback)(void *self, uint8_t dat);//读取回调函数
    void (*ReadErrorCallback)(void *self, uint8_t errcode);//读取错误回调函数
} UART_S_TypeDef;


/** 
 * @brief 初始化一个模拟串口
 * @param UART_S_TypeDef *pUART
 * @param uint8_t *TxBuf
 * @param uint8_t TxBufLen
 * @retval 
 */
void UART_S_Init(UART_S_TypeDef * const pUART, uint8_t *TxBuf, uint8_t TxBufLen);
/** 
 * @brief 发送一个字节
 * @param UART_S_TypeDef *pUART
 * @param uint8_t dat
 * @retval 0:成功 1:失败
 */
uint8_t UART_S_SendByte(UART_S_TypeDef * const pUART, uint8_t dat);
/** 
 * @brief 发送数据
 * @param UART_S_TypeDef *pUART
 * @param uint8_t *dat
 * @param uint8_t len
 * @retval 发送的字节数
 */
uint8_t UART_S_SendBytes(UART_S_TypeDef * const pUART, uint8_t *dat, uint8_t len);
/** 
 * @brief 发送字符串
 * @param UART_S_TypeDef *pUART
 * @param uint8_t *dat
 * @retval 发送的字节数
 */
uint8_t UART_S_SendString(UART_S_TypeDef * const pUART, uint8_t *dat);
/** 
 * @brief 格式化发送字符串
 * @param UART_S_TypeDef *pUART
 * @param char *fmt
 * @retval: 
 */
uint8_t UART_S_Printf(UART_S_TypeDef * const pUART, const char *fmt, ...);
/** 
 * @brief 模拟串口接收驱动
 * @param UART_S_TypeDef *pUART
 * @retval 发送的字节数
 */
void UART_S_ReadDrive(UART_S_TypeDef * const pUART);
/** 
 * @brief 模拟串口发送驱动
 * @param UART_S_TypeDef *pUART
 * @retval: 
 */
void UART_S_SendDrive(UART_S_TypeDef * const pUART);

#endif
