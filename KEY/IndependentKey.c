#include "IndependentKey.h"

static IndependentKey_TypeDef Keys[10];
static uint8_t KeyCount = 0;


static __inline uint8_t Key_GPIO_Read(IndependentKey_TypeDef * const pKey)
{
	return ((pKey->GPIO_PORT->IDR & (1 << pKey->GPIO_PIN)) != 0) ? 1 : 0;
}

/** 
 * @brief 添加按键 
 * @param uint32_t GPIO_CLK GPIO端口时钟
 * @param GPIO_TypeDef* GPIO_PORT GPIO端口
 * @param uint16_t GPIO_PIN GPIO端口引脚
 * @param uint8_t Number 编号，在点击时返回
 */
void IndependentKey_Add(uint32_t GPIO_CLK, GPIO_TypeDef* GPIO_PORT; uint16_t GPIO_PIN, uint8_t Number)
{	
	RCC->AHB1ENR |= GPIO_CLK; //开启时钟
	GPIO_PORT->MODER &= ~(0x00000003 << GPIO_PIN * 2);//设置为输入
    GPIO_PORT->PUPDR &= ~(0x00000003 << GPIO_PIN * 2);
    GPIO_PORT->PUPDR |= (0x00000001 << GPIO_PIN * 2);//设置为上拉
	Keys[KeyCount].GPIO_CLK = GPIO_CLK;
	Keys[KeyCount].GPIO_PORT = GPIO_PORT;
	Keys[KeyCount].GPIO_PIN = GPIO_PIN;
	Keys[KeyCount].Number = Number;
	KeyCount++;
}

/** 
 * @brief 扫描按键
 * @param 
 * @retval 约定返回的低16位为按键按下的编号，高16位bit0为长按标志
 */
uint32_t IndependentKey_Scan() 
{
	static uint8_t state = 0;
	static uint16_t clickTime = 0;
	static uint16_t keyValue;//按键值
	uint16_t tempKeyValue = 0;//临时按键
	uint16_t reValue = 0;//返回值

	for(uint8_t i = 0; i < KeyCount; i++)
	{
		// if (Key_GPIO_Read(&Keys[i]))
		// {
		// 	KeyValue |= (1 << Keys[i].Number);//支持组合键
		// }
		if(Key_GPIO_Read(&Keys[i]))
		{
			tempKeyValue = Keys[i].Number;
		}
	}

	switch (state)
	{
		case 0://等待按键按下
			if (tempKeyValue != 0)//检测按键是否按下
			{
				state = 1;
				keyValue = tempKeyValue;
			}
			break;
		case 1://消抖
			if (tempKeyValue == keyValue)//检测按键是否改变
			{
				clickTime = 0;
				state = 2;
				
			}
			else
				state = 0;
		break;
		case 2://确认按下，判断是否是长按
			if (tempKeyValue == keyValue)
			{
				if (++clickTime > LONG_CLICK_TIME_1)//还在按下，计时判断是否是长按
				{
					state = 3;
					clickTime = 0;
					reValue = keyValue | 0x0100;//长按
				}
			}
			else
			{
				state = 0;
				reValue = keyValue;
			}
		break;
		case 3://长按中
			if (tempKeyValue == keyValue)
			{
				if (++clickTime > LONG_CLICK_TIME_2)//还在按下，计时判断是否是长按
				{
					clickTime = 0;
					reValue = keyValue | 0x0100;//长按
				}
			}
			else
			{
				state = 0;
			}
		break;
	}
	return reValue;
}
