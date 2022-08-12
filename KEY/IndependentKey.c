#include "IndependentKey.h"

static IndependentKey_TypeDef Keys[10];
static uint8_t KeyCount = 0;


static __inline uint8_t Key_GPIO_Read(IndependentKey_TypeDef * const pKey)
{
	return ((pKey->GPIO_PORT->IDR & (1 << pKey->GPIO_PIN)) != 0) ? 1 : 0;
}


void IndependentKey_Add(uint32_t GPIO_CLK, GPIO_TypeDef* GPIO_PORT; uint16_t GPIO_PIN, uint8_t Number)
{	
	Keys[Number].GPIO_CLK = GPIO_CLK;
	Keys[Number].GPIO_PORT = GPIO_PORT;
	Keys[Number].GPIO_PIN = GPIO_PIN;
	Keys[Number].Number = Number;
}

/** 
 * @brief ɨ�谴��
 * @param 
 * @retval Լ�����صĵ�16λΪ�������µı�ţ���16λbit0Ϊ˫����־��bit1Ϊ������־
 */
uint32_t IndependentKey_Scan() {
	static uint8_t state = 0;
	static uint8_t clickTime = 0;
	static uint16_t keyValue;//����ֵ
	uint16_t tempKeyValue = 0;//��ʱ����
	for(uint8_t i = 0; i < KeyCount; i++)
	{
		// if (Key_GPIO_Read(&Keys[i]))
		// {
		// 	KeyValue |= (1 << Keys[i].Number);//֧����ϼ�
		// }
		if(Key_GPIO_Read(&Keys[i]))
		{
			tempKeyValue = Keys[i].Number;
		}
	}

	switch (state)
	{
	case 0:
		if (tempKeyValue != 0)//��ⰴ���Ƿ���
		{
			state = 1;
			keyValue = tempKeyValue;
		}
		break;
	case 1:
		if (tempKeyValue != keyValue)//��ⰴ���Ƿ�ı�
		{
			state = 0;
			keyValue = 0;
		}
		else
		{
			state = 2;
		}
	break;
	case 2://�ж��Ƿ��ǳ���
		if (tempKeyValue == keyValue)
		{
			if (++clickTime > LONG_CLICK_TIME)
			{
				state = 0;
				clickTime = 0;
			}
		}
		else//
		{
			state = 0;
			clickTime = 0;
		}
		
	break;
	default:
		break;
	}
}
