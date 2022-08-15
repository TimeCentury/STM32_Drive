/** 
 * @Author: 时间世纪
 * @Date: 2021-11-06 13:34:14
 * @Description: 软件IIC驱动程序
 */
#include "Software_IIC.h"

/** 
 * @brief 初始化一个IIC设备
 * @param 
 * @retval 
 */
void IIC_Init(IIC_TypeDef * const pIIC)
{
    HAL_GPIO_Init(&pIIC->SCL);
    HAL_GPIO_Init(&pIIC->SDA);
    HAL_GPIO_SetOutput(&pIIC->SCL);
    HAL_GPIO_Low(&pIIC->SCL);
    HAL_GPIO_SetOutput(&pIIC->SDA);
    HAL_GPIO_Low(&pIIC->SDA);
}

/** 
 * @brief 启动一次传输，在SCL高电平时SDA高变低开始传输
 * @param IIC_TypeDef *pIIC
 * @retval 
 */
void IIC_Start(IIC_TypeDef * const pIIC)
{
    HAL_GPIO_SetOutput(&pIIC->SDA);
    HAL_GPIO_High(&pIIC->SDA);
    HAL_GPIO_High(&pIIC->SCL);
    Delay_us(IIC_DELAY_TIME);
    HAL_GPIO_Low(&pIIC->SDA);
    Delay_us(IIC_DELAY_TIME);
    HAL_GPIO_Low(&pIIC->SCL);
    Delay_us(IIC_DELAY_TIME);
}

/** 
 * @brief 停止本次传输，在SCL高电平时SDA低变高停止传输
 * @param IIC_TypeDef *pIIC
 * @retval 
 */
void IIC_Stop(IIC_TypeDef *const pIIC)
{
    HAL_GPIO_SetOutput(&pIIC->SDA);
    HAL_GPIO_Low(&pIIC->SDA);
    Delay_us(IIC_DELAY_TIME);
    HAL_GPIO_High(&pIIC->SCL);
    Delay_us(IIC_DELAY_TIME);
    HAL_GPIO_High(&pIIC->SDA);
    Delay_us(IIC_DELAY_TIME);
    HAL_GPIO_Low(&pIIC->SCL);
    Delay_us(IIC_DELAY_TIME);
}

/** 
 * @brief 等待从机应答，在SCL高时从机会拉低SDA进行应答
 * @param IIC_TypeDef *pIIC
 * @retval 0：有应答，1：无应答
 */
uint8_t IIC_Sack(IIC_TypeDef *const pIIC)
{
    uint8_t result;

    HAL_GPIO_SetInput(&pIIC->SDA);//设置SDA为输入
    Delay_us(IIC_DELAY_TIME);
    HAL_GPIO_High(&pIIC->SCL);
    Delay_us(IIC_DELAY_TIME);
    for (result = 0; result <= 10; result++)
    {
        if (!HAL_GPIO_Read(&pIIC->SDA))
            break;
        Delay_us(IIC_DELAY_TIME);
    }
    HAL_GPIO_Low(&pIIC->SCL);
    Delay_us(IIC_DELAY_TIME);
    return result >= 10 ? 1 : 0;
}

/** 
 * @brief 产生一个应答信号对从机应答
 * @param IIC_TypeDef *pIIC
 * @param uint8_t ack 0: 结束不应答，1：应答
 * @retval: 
 */
static void IIC_Ack(IIC_TypeDef *const pIIC, uint8_t ack)
{
    HAL_GPIO_Low(&pIIC->SCL);
    Delay_us(IIC_DELAY_TIME);
    HAL_GPIO_SetOutput(&pIIC->SDA);
    if (ack)
        HAL_GPIO_Low(&pIIC->SDA);
    else
        HAL_GPIO_High(&pIIC->SDA);
    Delay_us(IIC_DELAY_TIME);
    HAL_GPIO_High(&pIIC->SCL);
    Delay_us(IIC_DELAY_TIME);
    HAL_GPIO_Low(&pIIC->SCL);
    Delay_us(IIC_DELAY_TIME);
}

/** 
 * @brief 发送一字节数据
 * @param IIC_TypeDef *pIIC
 * @param uint8_t dat
 */
void IIC_SendByte(IIC_TypeDef *const pIIC, uint8_t dat)
{
    HAL_GPIO_SetOutput(&pIIC->SDA);
    for (uint8_t i = 0; i < 8; i++)
    {
        HAL_GPIO_Low(&pIIC->SCL);
        Delay_us(IIC_DELAY_TIME);
        if (dat & (0x80 >> i))
            HAL_GPIO_High(&pIIC->SDA);
        else
            HAL_GPIO_Low(&pIIC->SDA);
        Delay_us(IIC_DELAY_TIME);
        HAL_GPIO_High(&pIIC->SCL);
        Delay_us(IIC_DELAY_TIME);
    }
    HAL_GPIO_Low(&pIIC->SCL);
    Delay_us(IIC_DELAY_TIME);
}

/** 
 * @brief 发送多个数据
 * @param IIC_TypeDef *pIIC
 * @param uint8_t * dat
 * @param uint8_t len 长度
 * @retval 0：发送成功，1: 失败
 */
uint8_t IIC_Send(IIC_TypeDef *const pIIC, uint8_t * dat, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        IIC_SendByte(pIIC, dat[i]);
        if(!IIC_Sack(pIIC))
        {
            return 1;
        }
    }
    return 0;
}

/** 
 * @brief 接收一字节数据
 * @param IIC_TypeDef *pIIC
 * @param uint8_t ack 对从机的应答信号指示是否继续读取
 * @retval 
 */
uint8_t IIC_ReadByte(IIC_TypeDef *const pIIC, uint8_t ack)
{
    uint8_t dat = 0;

    HAL_GPIO_SetInput(&pIIC->SDA);
    for (uint8_t i = 0; i < 8; i++)
    {
        HAL_GPIO_Low(&pIIC->SCL);
        Delay_us(IIC_DELAY_TIME);
        HAL_GPIO_High(&pIIC->SCL);
        Delay_us(IIC_DELAY_TIME);
        dat = (dat << 1) | HAL_GPIO_Read(&pIIC->SDA);
    }
    HAL_GPIO_Low(&pIIC->SCL);
    IIC_Ack(pIIC, ack);
    return dat;
}

/** 
 * @brief 读取多个数据
 * @param IIC_TypeDef *pIIC
 * @param uint8_t * dat
 * @param uint8_t len 长度
 */
void IIC_Read(IIC_TypeDef *const pIIC, uint8_t * dat, uint8_t len)
{
    for (uint8_t i = 0; i < len - 1; i++)
    {
        dat[i] = IIC_ReadByte(pIIC, 1);
    }
    dat[len - 1] = IIC_ReadByte(pIIC, 0);
}
