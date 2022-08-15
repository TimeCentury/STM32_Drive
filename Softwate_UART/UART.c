#include "UART.h"

/**
 * @brief 初始化一个模拟串口
 * @param UART_S_TypeDef *pUART
 * @param uint8_t *TxBuf 发送缓冲区
 * @param uint8_t TxBufLen 发送缓冲区长度
 * @retval:
 */
void UART_S_Init(UART_S_TypeDef *const pUART, uint8_t *TxBuf, uint8_t TxBufLen)
{
    if(pUART->TX != 0)
    {
        HAL_GPIO_Init(&pUART->TX);
        HAL_GPIO_SetOutput(&pUART->TX);
    }
    if(pUART->RX != 0)
    {
        HAL_GPIO_Init(&pUART->RX);
        HAL_GPIO_SetInput(&pUART->RX);
    }
    pUART->RxState = 0;
    pUART->RxTestCnt = 0;
    pUART->RxHighCnt = 0;
    pUART->RxBitCnt = 0;
    pUART->RxData = 0;
    pUART->TxState = 0;
    pUART->TxBitCnt = 0;
    pUART->TxBusy = 0;
    pUART->TxHeadIndex = 0;
    pUART->TxTailIndex = 0;
    pUART->TxBufDataCnt = 0;
    pUART->TxBuf = TxBuf;
    pUART->TxBufLen = TxBufLen;
}

/** 
 * @brief 换相变换RX、TX，可能导致未知风险，使用场景为一根线通信时将其中一个设置为0，另一个设置为IO口
 * @param UART_S_TypeDef *pUART
 * @retval
 */
static void UART_S_ChangeIO(UART_S_TypeDef * const pUART)
{
    HAL_GPIO_TypeDef temp;
    temp = pUART->RX;
    pUART->RX = pUART->TX;
    pUART->TX = temp;
    if(pUART->TX != 0)
        HAL_GPIO_SetOutput(&pUART->TX);
    if(pUART->RX != 0)
        HAL_GPIO_SetInput(&pUART->RX);
    pUART->RxState = 0;//重置状态
    pUART->RxTestCnt = 0;
    pUART->RxHighCnt = 0;
    pUART->RxBitCnt = 0;
    pUART->RxData = 0;
    pUART->TxState = 0;
    pUART->TxBitCnt = 0;
    pUART->TxBusy = 0;
    pUART->TxHeadIndex = 0;
    pUART->TxTailIndex = 0;
    pUART->TxBufDataCnt = 0;
}

/**
 * @brief 发送一个字节
 * @param UART_S_TypeDef *pUART
 * @param uint8_t dat
 * @retval 0:成功 1:失败
 */
uint8_t UART_S_SendByte(UART_S_TypeDef *const pUART, uint8_t dat)
{
    uint8_t cnt = 0;
    do
    {
        if (pUART->TxBusy) //如果在忙状态，就填充进缓存中
        {
            if (pUART->TxBufDataCnt < pUART->TxBufLen)
            {
                pUART->TxBuf[pUART->TxHeadIndex] = dat;
                if (++pUART->TxHeadIndex == pUART->TxBufLen)
                {
                    pUART->TxHeadIndex = 0;
                }
                pUART->TxBufDataCnt++;
                return 0;
            }
        }
        else
        {
            pUART->TxData = dat;
            pUART->TxBusy = 1;
            return 0;
        }
        Delay_ms(1);
    } while (++cnt < 10);
    return 1;
}

/**
 * @brief 发送数据
 * @param UART_S_TypeDef *pUART
 * @param uint8_t *dat
 * @param uint8_t len
 * @retval 发送的字节数
 */
uint8_t UART_S_SendBytes(UART_S_TypeDef *const pUART, uint8_t *dat, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        if (UART_S_SendByte(pUART, dat[i]))
        {
            return i;
        }
    }
    return len;
}

/**
 * @brief 发送字符串
 * @param UART_S_TypeDef *pUART
 * @param uint8_t *dat
 * @retval 发送的字节数
 */
uint8_t UART_S_SendString(UART_S_TypeDef *const pUART, uint8_t *dat)
{
    uint8_t len = 0;
    while (*dat)
    {
        if (UART_S_SendByte(pUART, *dat++))
        {
            return len;
        }
        len++;
    }
    return len;
}

/**
 * @brief 格式化发送字符串
 * @param UART_S_TypeDef *pUART
 * @param char *fmt
 * @retval 发送的字节数
 */
uint8_t UART_S_Printf(UART_S_TypeDef *const pUART, const char *fmt, ...)
{
    uint8_t buf[50];
    va_list args;
    va_start(args, fmt);
    uint8_t len = vsnprintf((char *)buf, 50, fmt, args);
    va_end(args);
    return UART_S_SendBytes(pUART, pUART->TxBuf, len);
}

/**
 * @brief 模拟串口接收驱动
 * @param UART_S_TypeDef *pUART
 * @retval
 */
void UART_S_ReadDrive(UART_S_TypeDef *const pUART)
{
    switch (pUART->RxState)
    {
    case 0:
        if (pUART->RX != 0 && HAL_GPIO_Read(&pUART->RX) == 0) //检测到开始位
        {
            pUART->RxState = 1;
        }
        break;
    case 1:
        if (HAL_GPIO_Read(&pUART->RX) == 0) //第二次检测到开始位，转入接收数据
        {
            pUART->RxBitCnt = 0;
            pUART->RxHighCnt = 0;
            pUART->RxData = 0;
            pUART->RxTestCnt = 0;
            pUART->RxState = 1;
        }
        else
        {
            pUART->RxState = 0;
        }
        break;
    case 2: //读取数据位
        if (HAL_GPIO_Read(&pUART->RX) != 0)
        {
            pUART->RxHighCnt++;
        }
        if (++pUART->RxTestCnt >= 3) //检测一个Bit位三次
        {
            pUART->RxTestCnt = 0;
            pUART->RxData >>= 1;
            if (pUART->RxHighCnt >= 2) //如果有两次位高就为高电平
            {
                pUART->RxData |= 0x80;
            }
            pUART->RxHighCnt = 0;
            if (++pUART->RxBitCnt >= 8)
            {
                pUART->RxBitCnt = 0;
                pUART->RxState = 3;
            }
        }
        break;
    case 3: //检测通停止位
        if (HAL_GPIO_Read(&pUART->RX) != 0)
        {
            pUART->RxHighCnt++;
        }
        pUART->RxTestCnt++;
        if (pUART->RxTestCnt == 2) //在第二次检测停止位时如果高电平检测到的次数为0次，那么就丢弃这次数据
        {
            if (pUART->RxHighCnt == 0)
            {
                if (pUART->ReadErrorCallback)
                {
                    pUART->ReadErrorCallback(pUART, 1);
                }
                pUART->RxState = 0;
            }
        }
        else if (pUART->RxTestCnt >= 3)
        {
            if (pUART->RxHighCnt >= 2) //大于两次认为接收成功
            {
                if (pUART->ReadCallback)
                {
                    pUART->ReadCallback(pUART, pUART->RxData);
                }
                pUART->RxState = 0;
            }
        }
        break;
    }
}

/**
 * @brief 模拟串口发送驱动
 * @param UART_S_TypeDef *pUART
 * @retval:
 */
void UART_S_SendDrive(UART_S_TypeDef *const pUART)
{
    switch (pUART->TxState)
    {
    case 0: //发送开始位
        if (pUART->TxBusy == 1)
        {
            HAL_GPIO_Low(&pUART->TX);
            pUART->TxState = 1;
        }
        break;
    case 1: //发送数据位
        if (pUART->RxData & 0x01)
        {
            HAL_GPIO_High(&pUART->TX);
        }
        else
        {
            HAL_GPIO_Low(&pUART->TX);
        }
        pUART->RxData >>= 1;
        if (++pUART->TxBitCnt >= 8)
        {
            pUART->TxBitCnt = 0;
            pUART->TxState = 2;
        }
        break;
    case 2: //发送停止位
        HAL_GPIO_High(&pUART->TX);
        if (pUART->TxBufDataCnt > 0)
        {
            if (++pUART->TxTailIndex >= pUART->TxBufLen)
            {
                pUART->TxTailIndex = 0;
            }
            pUART->TxBufDataCnt--;
            if (pUART->TxHeadIndex == pUART->TxTailIndex)
            {
                pUART->TxBusy = 0;
            }
            else
            {
                pUART->TxData = pUART->TxBuf[pUART->TxTailIndex];
            }
        }
        else
        {
            pUART->TxBusy = 0;
        }
        pUART->TxState = 0;
        break;
    }
}
