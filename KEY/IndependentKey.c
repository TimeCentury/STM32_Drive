/** 
 * @Author: ʱ������
 * @Date: 2022-08-14 20:34:43
 * @Description: ����������������
 */
#include "IndependentKey.h"

static IndependentKey_TypeDef Keys[INDEPENDENT_KEY_COUNT];
static uint8_t KeyCount = 0;

/** 
 * @brief ��Ӱ��� 
 * @param uint32_t GPIO_CLK GPIO�˿�ʱ��
 * @param GPIO_TypeDef* GPIO_PORT GPIO�˿�
 * @param uint16_t GPIO_PIN GPIO�˿�����
 * @param uint8_t Number ��ţ��ڵ��ʱ����
 */
void IndependentKey_Add(uint32_t GPIO_CLK, GPIO_TypeDef* GPIO_PORT, uint16_t GPIO_PIN, uint8_t Number)
{	
	if(KeyCount == INDEPENDENT_KEY_COUNT)
	{
		LOG_ERROR("IndependentKey_Add: KeyCount > %d", INDEPENDENT_KEY_COUNT);
		return;
	}
	RCC->AHB1ENR |= GPIO_CLK; //����ʱ��
	Keys[KeyCount].Key.GPIO_CLK = GPIO_CLK;
	Keys[KeyCount].Key.GPIO_PORT = GPIO_PORT;
	Keys[KeyCount].Key.GPIO_PIN = GPIO_PIN;
	Keys[KeyCount].Number = Number;
	HAL_GPIO_SetInput(&(Keys[KeyCount].Key));
	KeyCount++;
}

/** 
 * @brief ɨ�谴��
 * @param 
 * @retval Լ�����صĵ�8λΪ�������µı�ţ���8λbit0Ϊ������־
 */
uint16_t IndependentKey_Scan() 
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
		if(HAL_GPIO_Read(&Keys[i].Key))
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
