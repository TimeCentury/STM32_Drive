#include "IndependentKey.h"

static IndependentKey_TypeDef Keys[10];
static uint8_t KeyCount = 0;


static __inline uint8_t Key_GPIO_Read(IndependentKey_TypeDef * const pKey)
{
	return ((pKey->GPIO_PORT->IDR & (1 << pKey->GPIO_PIN)) != 0) ? 1 : 0;
}

/** 
 * @brief ��Ӱ��� 
 * @param uint32_t GPIO_CLK GPIO�˿�ʱ��
 * @param GPIO_TypeDef* GPIO_PORT GPIO�˿�
 * @param uint16_t GPIO_PIN GPIO�˿�����
 * @param uint8_t Number ��ţ��ڵ��ʱ����
 */
void IndependentKey_Add(uint32_t GPIO_CLK, GPIO_TypeDef* GPIO_PORT; uint16_t GPIO_PIN, uint8_t Number)
{	
	RCC->AHB1ENR |= GPIO_CLK; //����ʱ��
	GPIO_PORT->MODER &= ~(0x00000003 << GPIO_PIN * 2);//����Ϊ����
    GPIO_PORT->PUPDR &= ~(0x00000003 << GPIO_PIN * 2);
    GPIO_PORT->PUPDR |= (0x00000001 << GPIO_PIN * 2);//����Ϊ����
	Keys[KeyCount].GPIO_CLK = GPIO_CLK;
	Keys[KeyCount].GPIO_PORT = GPIO_PORT;
	Keys[KeyCount].GPIO_PIN = GPIO_PIN;
	Keys[KeyCount].Number = Number;
	KeyCount++;
}

/** 
 * @brief ɨ�谴��
 * @param 
 * @retval Լ�����صĵ�16λΪ�������µı�ţ���16λbit0Ϊ������־
 */
uint32_t IndependentKey_Scan() 
{
	static uint8_t state = 0;
	static uint16_t clickTime = 0;
	static uint16_t keyValue;//����ֵ
	uint16_t tempKeyValue = 0;//��ʱ����
	uint16_t reValue = 0;//����ֵ

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
		case 0://�ȴ���������
			if (tempKeyValue != 0)//��ⰴ���Ƿ���
			{
				state = 1;
				keyValue = tempKeyValue;
			}
			break;
		case 1://����
			if (tempKeyValue == keyValue)//��ⰴ���Ƿ�ı�
			{
				clickTime = 0;
				state = 2;
				
			}
			else
				state = 0;
		break;
		case 2://ȷ�ϰ��£��ж��Ƿ��ǳ���
			if (tempKeyValue == keyValue)
			{
				if (++clickTime > LONG_CLICK_TIME_1)//���ڰ��£���ʱ�ж��Ƿ��ǳ���
				{
					state = 3;
					clickTime = 0;
					reValue = keyValue | 0x0100;//����
				}
			}
			else
			{
				state = 0;
				reValue = keyValue;
			}
		break;
		case 3://������
			if (tempKeyValue == keyValue)
			{
				if (++clickTime > LONG_CLICK_TIME_2)//���ڰ��£���ʱ�ж��Ƿ��ǳ���
				{
					clickTime = 0;
					reValue = keyValue | 0x0100;//����
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
