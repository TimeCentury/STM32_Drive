/** 
 * @Author: 时间世纪
 * @Date: 2021-11-06 13:34:14
 * @Description: 软件IIC驱动程序
 */
#include "Software_IIC.h"

/** 
 * @brief 微妙级延时
 * @param uint16_t t
 * @retval 
 */
static __inline void Delay(uint32_t t)
{
    //delay_us(t);
	for(uint32_t i = 0; i < t; i++)
	{
		for (uint32_t j = 0; j < 10; j++)
		{
			__nop();
		}
	}
}

/** 
 * @brief 设置端口电平
 * @param IIC_GPIO_TypeDef * pGPIO
 * @retval 
 */
static __inline void IIC_GPIO_High(IIC_GPIO_TypeDef * pGPIO)
{
    pGPIO->GPIO_PORT->BSRRL = 1 << pGPIO->GPIO_PIN;
}

/** 
 * @brief 设置端口电平
 * @param IIC_GPIO_TypeDef * pGPIO
 * @retval 
 */
static __inline void IIC_GPIO_Low(IIC_GPIO_TypeDef * pGPIO)
{
	pGPIO->GPIO_PORT->BSRRH = 1 << pGPIO->GPIO_PIN;
}

/** 
 * @brief 获取端口输入
 * @param IIC_GPIO_TypeDef * pGPIO
 * @retval 
 */
static __inline uint8_t IIC_GPIO_Read(IIC_GPIO_TypeDef * pGPIO)
{
    return ((pGPIO->GPIO_PORT->IDR & (1 << pGPIO->GPIO_PIN)) != 0) ? 1 : 0;
}

/** 
 * @brief 将端口设置为输出 
 * @param IIC_GPIO_TypeDef * pGPIO
 * @retval 
 */
static __inline void IIC_GPIO_SetOutput(IIC_GPIO_TypeDef * pGPIO)
{
    pGPIO->GPIO_PORT->MODER &= ~(0x00000003 << pGPIO->GPIO_PIN * 2);
    pGPIO->GPIO_PORT->MODER |= (0x00000001 << pGPIO->GPIO_PIN * 2);//设置为输出
	pGPIO->GPIO_PORT->OSPEEDR &= ~(0x00000003 << pGPIO->GPIO_PIN * 2);
    pGPIO->GPIO_PORT->OSPEEDR |= (0x00000002 << pGPIO->GPIO_PIN * 2);//设置为高速
    pGPIO->GPIO_PORT->OTYPER &= ~(0x00000001 << pGPIO->GPIO_PIN);//设置为推挽输出
    pGPIO->GPIO_PORT->PUPDR &= ~(0x00000003 << pGPIO->GPIO_PIN * 2);//设置为无上下拉
}

/** 
 * @brief 将端口设置为输入
 * @param IIC_GPIO_TypeDef * pGPIO
 * @retval 
 */
static __inline void IIC_GPIO_SetInput(IIC_GPIO_TypeDef * pGPIO)
{
    pGPIO->GPIO_PORT->MODER &= ~(0x00000003 << pGPIO->GPIO_PIN * 2);//设置为输入
    pGPIO->GPIO_PORT->PUPDR &= ~(0x00000003 << pGPIO->GPIO_PIN * 2);
    pGPIO->GPIO_PORT->PUPDR |= (0x00000001 << pGPIO->GPIO_PIN * 2);//设置为上拉
}

/** 
 * @brief 初始化一个IIC设备
 * @param 
 * @retval 
 */
void IIC_Init(IIC_TypeDef * const pIIC)
{
    RCC->AHB1ENR |= pIIC->SCL.GPIO_CLK | pIIC->SDA.GPIO_CLK; //开启时钟
    IIC_GPIO_SetOutput(&pIIC->SCL);
    IIC_GPIO_Low(&pIIC->SCL);
    IIC_GPIO_SetOutput(&pIIC->SDA);
    IIC_GPIO_Low(&pIIC->SDA);
}

/** 
 * @brief 启动一次传输，在SCL高电平是SDA高变低开始传输
 * @param IIC_TypeDef *pIIC
 * @retval 
 */
void IIC_Start(IIC_TypeDef * const pIIC)
{
    IIC_GPIO_SetOutput(&pIIC->SDA);
    IIC_GPIO_High(&pIIC->SDA);
    Delay(IIC_DELAY_TIME);
    IIC_GPIO_High(&pIIC->SCL);
    Delay(IIC_DELAY_TIME);
    IIC_GPIO_Low(&pIIC->SDA);
    Delay(IIC_DELAY_TIME);
    IIC_GPIO_Low(&pIIC->SCL);
    Delay(IIC_DELAY_TIME);
}

/** 
 * @brief 停止本次传输，在SCL高电平时SDA低变高停止传输
 * @param IIC_TypeDef *pIIC
 * @retval 
 */
void IIC_Stop(IIC_TypeDef *const pIIC)
{
    IIC_GPIO_SetOutput(&pIIC->SDA);
    IIC_GPIO_Low(&pIIC->SDA);
    Delay(IIC_DELAY_TIME);
    IIC_GPIO_High(&pIIC->SCL);
    Delay(IIC_DELAY_TIME);
    IIC_GPIO_High(&pIIC->SDA);
    Delay(IIC_DELAY_TIME);
    IIC_GPIO_Low(&pIIC->SCL);
    Delay(IIC_DELAY_TIME);
}

/** 
 * @brief 等待从机应答，在SCL高时从机会拉低SDA进行应答
 * @param IIC_TypeDef *pIIC
 * @retval 0：有应答，1：无应答
 */
uint8_t IIC_Sack(IIC_TypeDef *const pIIC)
{
    uint8_t result;

    IIC_GPIO_SetInput(&pIIC->SDA);//设置SDA为输入
    Delay(IIC_DELAY_TIME);
    IIC_GPIO_High(&pIIC->SCL);
    Delay(IIC_DELAY_TIME);
    for (result = 0; result < 10; result++)
    {
        if (!IIC_GPIO_Read(&pIIC->SDA))
            break;
        Delay(IIC_DELAY_TIME);
    }
    IIC_GPIO_Low(&pIIC->SCL);
    Delay(IIC_DELAY_TIME);
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
    IIC_GPIO_Low(&pIIC->SCL);
    Delay(IIC_DELAY_TIME);
    IIC_GPIO_SetOutput(&pIIC->SDA);
    if (ack)
        IIC_GPIO_Low(&pIIC->SDA);
    else
        IIC_GPIO_High(&pIIC->SDA);
    Delay(IIC_DELAY_TIME);
    IIC_GPIO_High(&pIIC->SCL);
    Delay(IIC_DELAY_TIME);
    IIC_GPIO_Low(&pIIC->SCL);
    Delay(IIC_DELAY_TIME);
}

/** 
 * @brief 发送一字节数据
 * @param IIC_TypeDef *pIIC
 * @param uint8_t dat
 */
void IIC_SendByte(IIC_TypeDef *const pIIC, uint8_t dat)
{
    IIC_GPIO_SetOutput(&pIIC->SDA);
    for (uint8_t i = 0; i < 8; i++)
    {
        IIC_GPIO_Low(&pIIC->SCL);
        Delay(IIC_DELAY_TIME);
        if (dat & (0x80 >> i))
            IIC_GPIO_High(&pIIC->SDA);
        else
            IIC_GPIO_Low(&pIIC->SDA);
        Delay(IIC_DELAY_TIME);
        IIC_GPIO_High(&pIIC->SCL);
        Delay(IIC_DELAY_TIME);
    }
    IIC_GPIO_Low(&pIIC->SCL);
    Delay(IIC_DELAY_TIME);
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

    IIC_GPIO_SetInput(&pIIC->SDA);
    for (uint8_t i = 0; i < 8; i++)
    {
        IIC_GPIO_Low(&pIIC->SCL);
        Delay(IIC_DELAY_TIME);
        IIC_GPIO_High(&pIIC->SCL);
        Delay(IIC_DELAY_TIME);
        dat = (dat << 1) | IIC_GPIO_Read(&pIIC->SDA);
    }
    IIC_GPIO_Low(&pIIC->SCL);
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

/** 
 * @brief 发送写地址
 * @param IIC_TypeDef *pIIC
 * @retval 0：有应答，1：无应答
 */
__inline uint8_t IIC_SendWriteAddress(IIC_TypeDef *const pIIC)
{
    IIC_SendByte(pIIC, pIIC->ADD);
    return IIC_Sack(pIIC);
}

/** 
 * @brief 发送读地址
 * @param IIC_TypeDef *pIIC
 * @retval 0：有应答，1：无应答
 */
__inline uint8_t IIC_SendReadAddress(IIC_TypeDef *const pIIC)
{
    IIC_SendByte(pIIC, pIIC->ADD | 0x01);
    return IIC_Sack(pIIC);
}

/** 
 * @brief 发送一个字节并检查从机应答
 * @param IIC_TypeDef *pIIC
 * @param uint8_t dat
 * @retval 0：有应答，1：无应答
 */
__inline uint8_t IIC_SendAndSack(IIC_TypeDef *const pIIC, uint8_t dat)
{
    IIC_SendByte(pIIC, dat);
    return IIC_Sack(pIIC);
}
