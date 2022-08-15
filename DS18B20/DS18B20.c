#include "DS18B20.h"


/** 
 * @brief 初始化一个DS18B20设备
 * @param DS18B20_TypeDef *pDS18B20
 * @retval: 
 */
void DS18B20_INIT(DS18B20_TypeDef * const pDS18B20)
{
	HAL_GPIO_Init((HAL_GPIO_TypeDef *)pDS18B20);
    HAL_GPIO_SetInput((HAL_GPIO_TypeDef *)pDS18B20);
}

/** 
 * @brief 开始时序
 * @param OneWire_TypeDef *pOneWire
 * @retval: 
 */
static uint8_t OneWire_StartPulse(OneWire_TypeDef * const pOneWire)
{
    HAL_GPIO_SetOutput(&pOneWire->DIO);//产生复位时序 >480us
    HAL_GPIO_Low(&pOneWire->DIO);
    Delay_us(500);
    HAL_GPIO_SetInput(&pOneWire->DIO);//等待读取应答信号 等待时间15us~60us
    for (uint8_t i = 0; i <= 60; i++)
    {
        if (HAL_GPIO_Read(&pOneWire->DIO) == 0)
        {
            break;
        }
        if(i == 60)
        {
            return 1;
        }
        Delay_us(1);
    }
    for (uint8_t i = 0; i <= 240; i++)//等待DS18B20释放总线 应答时间60us~240us
    {
        if (HAL_GPIO_Read(&pOneWire->DIO) == 1)
        {
            break;
        }
        if(i == 240)
        {
            return 2;
        }
        Delay_us(1);
    }
    return 0;
}

/** 
 * @brief 发送一个字节数据
 * @param OneWire_TypeDef *pOneWire
 * @param uint8_t dat
 * @retval: 
 */
static void OneWire_SendByte(OneWire_TypeDef * const pOneWire, uint8_t dat)
{
    HAL_GPIO_SetOutput(&pOneWire->DIO);
    for (uint8_t i = 0; i < 8; i++)
    {
        HAL_GPIO_Low(&pOneWire->DIO);
        Delay_us(10);
        if (dat & 0x01)//发送1 时序为拉低1us~15us 然后释放总线，时序大于60us
        {
            HAL_GPIO_High(&pOneWire->DIO);
            Delay_us(60);
        }
        else//发送0 时序是拉低60us~120us 然后释放总线要大于1us，时序大于60us
        {
            Delay_us(60);
            HAL_GPIO_High(&pOneWire->DIO);
            Delay_us(5);
        }
        dat >>= 1;
    }    
}

/** 
 * @brief 从OneWire读取一个字节 
 * @param OneWire_TypeDef *pOneWire
 * @retval: 
 */
static uint8_t OneWire_ReadByte(OneWire_TypeDef * const pOneWire)
{
    uint8_t dat = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        dat >>= 1;
        HAL_GPIO_SetOutput(&pOneWire->DIO);//读取时序 主机将总线拉低1~15us，然后释放总线，在15us后读取数据，时序大于60us
        HAL_GPIO_Low(&pOneWire->DIO);
        Delay_us(5);
        HAL_GPIO_SetInput(&pOneWire->DIO);
        Delay_us(10);
        if (HAL_GPIO_Read(&pOneWire->DIO) == 1)
        {
            dat |= 0x80;
        }
        Delay_us(60);
    }
    return dat;
}

/** 
 * @brief 启动一个温度转换
 * @param DS18B20_TypeDef *pDS18B20
 * @retval 0:成功 1:失败
 */
uint8_t DS18B20_Convert(DS18B20_TypeDef * const pDS18B20)
{
    uint8_t result = 0;
    result = OneWire_StartPulse(&pDS18B20->DQ);
    if (result)
        return result;
    OneWire_SendByte(&pDS18B20->DQ, 0xCC);//发送跳过ROM命令
    OneWire_SendByte(&pDS18B20->DQ, 0x44);//发送转换命令
    return 0;
}

/** 
 * @brief 读取温度值
 * @param DS18B20_TypeDef *pDS18B20
 * @param float *temperature 温度值
 * @retval 0:成功 1:失败
 */
uint8_t DS18B20_ReadTemperature(DS18B20_TypeDef * const pDS18B20, float *temperature)
{
    uint8_t result = 0;
    uint8_t TL,TM;
    result = OneWire_StartPulse(&pDS18B20->DQ);
    if (result)
        return result;
    OneWire_SendByte(&pDS18B20->DQ, 0xCC);//发送跳过ROM命令
    OneWire_SendByte(&pDS18B20->DQ, 0xBE);//发送读取命令
    TL = OneWire_ReadByte(&pDS18B20->DQ);//读取温度高位
    TM = OneWire_ReadByte(&pDS18B20->DQ);//读取温度低位
    if ((TM & 0xF8) == 0)//正数
    {
        *temperature = ((int16_t)((TM << 8) | TL)) * 0.0625f;
    }
    else//负数
    {
        *temperature = -((~(int16_t)((TM << 8) | TL)) + 1) * 0.0625f;
    }
    return 0;
}

