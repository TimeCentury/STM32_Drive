/** 
 * @Author: ʱ������
 * @Date: 2021-08-11 13:46:24
 * @Description: EEPROM��������
 */
#include "EEPROM.h"

EEPROM_TypeDef EEPROM_AT24C02 = {
	.IIC = {
		{RCC_AHB1Periph_GPIOH, GPIOH, 4},
		{RCC_AHB1Periph_GPIOH, GPIOH, 5},
		0xA0
	},
};

EEPROM_TypeDef EEPROM_AT24C16 = {
	.IIC = {
		{RCC_AHB1Periph_GPIOH, GPIOH, 4},
		{RCC_AHB1Periph_GPIOH, GPIOH, 6},
		0xA0
	},
};

EEPROM_TypeDef EEPROM_AT24C256 = {
	.IIC = {
		{RCC_AHB1Periph_GPIOH, GPIOH, 4},
		{RCC_AHB1Periph_GPIOH, GPIOH, 5},
		0xA8
	},
};

/** 
 * @brief �ж�EEPROM�Ƿ���æ״̬������һ����ʼ�ͷ����豸��ַ�Ĺ���
 * @param EEPROM_TypeDef *pEEPROM
 * @param uint8_t devAdd �豸��ַ
 * @retval 0����æ���Լ���������1��æ��ֹ����
 */
static uint8_t EEPROM_IsBusy(EEPROM_TypeDef *const pEEPROM, uint8_t devAdd)
{
	for (uint8_t i = 0; i < EEPROM_BUSY_CNT; i++)
	{
		IIC_Start(&pEEPROM->IIC);
		if(IIC_SendAndSack(&pEEPROM->IIC, devAdd) == 0)
		{
			return 0;
		}
		if(i < EEPROM_BUSY_CNT - 1)
		{
			IIC_Stop(&pEEPROM->IIC);
			Delay_ms(1000);
		}
	}
	return 1;
}

/** 
 * @brief Ѱַ��������֧��AT24C02,AT24C04,AT24C08,AT24C16
 * @param void* self
 * @param uint16_t address
 * @retval: 
 */
uint8_t AT24C0X_StartTransmission(void* self, uint16_t address)
{
	EEPROM_TypeDef* pEEPROM = self;
	uint8_t add = pEEPROM->IIC.ADD | ((address>>8) & ~(0xff << (uint8_t)pEEPROM->Type));
	if(EEPROM_IsBusy(pEEPROM, add))
		return 1;
	if (IIC_SendAndSack(&pEEPROM->IIC, address & 0xFF))
		return 2;
	return 0;
}

/** 
 * @brief Ѱַ��������֧��AT24C32,AT24C64,AT24C128,AT24C256
 * @param void* self
 * @param uint16_t address
 * @retval: 
 */
uint8_t AT24CXX_StartTransmission(void* self, uint16_t address)
{
	IIC_TypeDef* IIC = self;
	if (EEPROM_IsBusy(self, IIC->ADD))
		return 1;
	if (IIC_SendAndSack(IIC, address >> 8))
		return 2;
	if (IIC_SendAndSack(IIC, address & 0xFF))
		return 3;
	return 0;
}

/**
 * @brief ��ʼ��EEPROM
 * @param EEPROM_TypeDef *pEEPROM
 * @param EEPROM_Type type EEPROM����
 * @retval 0���ɹ�������ʧ��
 */
uint8_t EEPROM_Init(EEPROM_TypeDef *const pEEPROM, EEPROM_Type type)
{
	IIC_Init(&pEEPROM->IIC);
	switch(type)
	{
		case AT24C02:
		case AT24C04:
		case AT24C08:
		case AT24C16:
			pEEPROM->Type = type;
			if (type == AT24C02) pEEPROM->PageSize = 8;
			else pEEPROM->PageSize = 16;
			pEEPROM->EEPROM_StartTransmission = &AT24C0X_StartTransmission;
		break;
		case AT24C32:
		case AT24C64:
		case AT24C128:
		case AT24C256:
			pEEPROM->Type = type;
			if (type == AT24C256) pEEPROM->PageSize = 64;
			else pEEPROM->PageSize = 32;
			pEEPROM->EEPROM_StartTransmission = &AT24CXX_StartTransmission;
		break;
		default:
			return 2;
	}
	pEEPROM->TotalCapacity = 256;
	for (uint8_t i = 0; i < (uint8_t)type; i++)
	{
		pEEPROM->TotalCapacity *= 2;
	}
	pEEPROM->PageCount = pEEPROM->TotalCapacity / pEEPROM->PageSize;
	return EEPROM_Check(pEEPROM);
}

/**
 * @brief ���EEPROM�Ƿ�����
 * @param EEPROM_TypeDef *pEEPROM
 * @retval 0: �������������쳣
 */
uint8_t EEPROM_Check(EEPROM_TypeDef *const pEEPROM)
{
	uint8_t result = 0;
	// IIC_Start(&pEEPROM->IIC);
	// result = IIC_SendWriteAddress(&pEEPROM->IIC);
	// IIC_Stop(&pEEPROM->IIC);
	return result;
}

/**
 * @brief д��һ���ֽ�
 * @param EEPROM_TypeDef *pEEPROM
 * @param uint16_t address д���ַ
 * @param uint8_t dat ����
 * @retval 0���ɹ�������ʧ��
 */
uint8_t EEPROM_WriteByte(EEPROM_TypeDef *const pEEPROM, uint16_t address, uint8_t dat)
{
	uint8_t result = 0;

	if(pEEPROM->EEPROM_StartTransmission(pEEPROM, address))
	{
		result = 1;
		goto err;
	}
	// ��������
	if (IIC_SendAndSack(&pEEPROM->IIC, dat))
	{
		result = 2;
	}
err:
	IIC_Stop(&pEEPROM->IIC);
	return result;
}

/** 
 * @brief: 
 * @param EEPROM_TypeDef *pEEPROM
 * @param uint16_t address
 * @param uint8_t *memory
 * @param uint16_t size
 * @retval: 
 */
uint8_t EEPROM_Write(EEPROM_TypeDef *const pEEPROM, uint16_t address, uint8_t *memory, uint16_t size)
{
	uint8_t result = 0;

	//��ʼһ��IIC����
	if(pEEPROM->EEPROM_StartTransmission(pEEPROM, address))
	{
		result = 1;
		goto err;
	}
	// д������
	for (uint16_t i = 0; i < size; i++)
	{
		if (IIC_SendAndSack(&pEEPROM->IIC, memory[i]))
		{
			result = 2 + i;
			break;
		}
		// ����
		if (i + 1 == size)
			break;
		//�����һ����ַ����ҳ�Ŀ�ͷ�������δ���
		if ((address + i + 1) % pEEPROM->PageSize == 0)
		{
			IIC_Stop(&pEEPROM->IIC);//������ҳ����
			return EEPROM_Write(pEEPROM, address + i + 1, memory + i + 1, size - i - 1);
		}
	}

err:
	IIC_Stop(&pEEPROM->IIC);
	return result;
}

/** 
 * @brief ��ȡһ���ֽ�
 * @param EEPROM_TypeDef *pEEPROM
 * @param uint16_t address
 * @param uint8_t* dat
 * @retval 0���ɹ�������ʧ��
 */
uint8_t EEPROM_ReadByte(EEPROM_TypeDef *const pEEPROM, uint16_t address, uint8_t* dat)
{
	uint8_t result = 0;

	// ����д�豸��ַ
	if(pEEPROM->EEPROM_StartTransmission(pEEPROM, address))
	{
		result = 1;
		goto err;
	}
	// ����IIC
	IIC_Start(&pEEPROM->IIC);
	// ���Ͷ��豸��ַ
	if (IIC_SendReadAddress(&pEEPROM->IIC))
	{
		result = 2;
		goto err;
	}
	*dat = IIC_ReadByte(&pEEPROM->IIC, 0);

err:
	IIC_Stop(&pEEPROM->IIC);
	return result;
}

/** 
 * @brief ��ȡ����
 * @param EEPROM_TypeDef *pEEPROM
 * @param uint16_t address
 * @param uint8_t *memory
 * @param uint16_t size
 * @retval 0���ɹ�������ʧ��
 */
uint8_t EEPROM_Read(EEPROM_TypeDef *const pEEPROM, uint16_t address, uint8_t *memory, uint16_t size)
{
	uint8_t result = 0;

	// ����д�豸��ַ
	if(pEEPROM->EEPROM_StartTransmission(pEEPROM, address))
	{
		result = 1;
		goto err;
	}
	// ����IIC
	IIC_Start(&pEEPROM->IIC);
	// ���Ͷ��豸��ַ
	if (IIC_SendReadAddress(&pEEPROM->IIC))
	{
		result = 2;
		goto err;
	}

	for (uint16_t i = 0; i < size - 1; i++)
	{
		memory[i] = IIC_ReadByte(&pEEPROM->IIC, 1);
	}
	memory[size - 1] = IIC_ReadByte(&pEEPROM->IIC, 0);
err:
	IIC_Stop(&pEEPROM->IIC);
	return result;
}
