#ifndef _INDEPENDENT_KEY_H_
#define _INDEPENDENT_KEY_H_

#include "stm32f4xx.h"

#define KEY_RIGHI	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)
#define KEY_ESC		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9)
#define KEY_OK		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)
#define KEY_UP		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)
#define KEY_DOWN	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)
#define KEY_LEFT	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)

#define USE_EXTENSIONS 1

//优先级 单击>双击>长按
typedef struct 
{
	uint32_t		GPIO_CLK;
	GPIO_TypeDef*	GPIO_PORT; 
	uint16_t		GPIO_PIN;//0~15
	uint8_t Number;//编号，在点击时返回 
#if USE_EXTENSIONS
	uint8_t UseLongClick : 1;//是否使用长按
	uint8_t UseDoubleClick : 1;//是否使用双击
	uint8_t LongTime;//长按时间
	uint8_t LongTimeFlag;//长按标志
	uint8_t KeyFlag;//按键标志
	uint8_t KeyFlagCount;//按键标志计数
#endif
} IndependentKey_TypeDef;


#define	CombinationKeyFlag  0//�Ƿ�֧����ϼ�?
#define CONTINUOUSTIME 50//����ʱ�䣬500ms��һ�γ���

#define POWER_IN 		((EIO_PCA9539X_DEV.pINFO->IN[0] & 0x80) >> 7)//��Դ�����?
#define CHARGING_IN		((EIO_PCA9539X_DEV.pINFO->IN[1] & 0x02) >> 1)//�����?
#define POWER_KEY_RESET	GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#define POWER_KEY_SET	GPIO_SetBits(GPIOA, GPIO_Pin_1)


extern OS_FLAG_GRP *KEY_FLAG_GRP;//�����¼���־��
#define KEY_VALUE_FLAG 1U

extern void KeyInit(void);
extern void Task_Key(void *p_arg);
extern void AddKey(KEY_VALUE_TypeDef *Key);//���Ӱ���
extern void DelKey(void);
extern void PowerKey_Init(void);
#endif
