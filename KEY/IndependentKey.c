#include "IndependentKey.h"

IndependentKey_TypeDef Keys[10];

void IndependentKey_Add(uint32_t GPIO_CLK, GPIO_TypeDef* GPIO_PORT; uint16_t GPIO_PIN, uint8_t Number)
{	
	Keys[Number].GPIO_CLK = GPIO_CLK;
	Keys[Number].GPIO_PORT = GPIO_PORT;
	Keys[Number].GPIO_PIN = GPIO_PIN;
	Keys[Number].Number = Number;
}

#if USE_EXTENSIONS
void IndependentKey_AddExpand(uint32_t GPIO_CLK, GPIO_TypeDef* GPIO_PORT; uint16_t GPIO_PIN, uint8_t Number, uint8_t UseLongClick, uint8_t UseDoubleClick, uint8_t LongTime)
{	
	Keys[Number].GPIO_CLK = GPIO_CLK;
	Keys[Number].GPIO_PORT = GPIO_PORT;
	Keys[Number].GPIO_PIN = GPIO_PIN;
	Keys[Number].Number = Number;
	Keys[Number].UseLongClick = UseLongClick;
	Keys[Number].UseDoubleClick = UseDoubleClick;
	Keys[Number].LongTime = LongTime;
}

#endif

static KEY_VALUE_TypeDef Key_Value(void) {

}

uint8_t IndependentKey_Scan() {
	uint8_t Key_Value = KEY_NONE;
	if (IndependentKey->KeyFlag == 0) {
		if (IndependentKey->KeyFlagCount >= IndependentKey->KeyFlagTime) {
			IndependentKey->KeyFlag = 1;
			IndependentKey->KeyFlagCount = 0;
		} else {
			IndependentKey->KeyFlagCount++;
		}
	} else {
		if (IndependentKey->LongTimeFlag == 0) {
			if (IndependentKey->LongTimeCount >= IndependentKey->LongTime) {
				IndependentKey->LongTimeFlag = 1;
				IndependentKey->LongTimeCount = 0;
				Key_Value = IndependentKey->LongValue;
			} else {
				IndependentKey->LongTimeCount++;
			}
		} else {
			if (IndependentKey->KeyFlagCount >= IndependentKey->KeyFlagTime) {
				IndependentKey->KeyFlagCount = 0;
				Key_Value = IndependentKey->LongValue;
			} else {
				IndependentKey->KeyFlagCount++;
			}
		}
	}
	return Key_Value;
}

uint8_t Get_Key(void) {
	static uint8_t status = 0;
	uint8_t re_key = 0;
	uint8_t keyvalue = 0;
	keyvalue = Key_Value();//��ȡ����
	switch(status) {
		case 0: {
			if(keyvalue != 0)//��������
				status = 1;
		}break;
		case 1: {//����
			if(keyvalue != 0) {//������ڰ���
				re_key = keyvalue;
				status = 2;
			}
		}break;
		case 2: {//�ȴ��ͷ�
			if(keyvalue == 0) {
				status = 0;
			} 
		}break;
	}
	return re_key;
}
void AddKey(KEY_VALUE_TypeDef *Key) {
	KeyValue = Key;
	*KeyValue = KEY_NONE;
}
void DelKey(void) {
	if(KeyValue == 0)
		return;
	
	*KeyValue = KEY_NONE;
	KeyValue = 0;
}

void Task_Key(void *p_arg) {
	uint8_t OS_ERR;
	uint16_t ContinuousCnt = 0;
	uint8_t status = 0;
	KEY_VALUE_TypeDef keyvalue;
	KEY_VALUE_TypeDef key;
	uint8_t cnt10ms = 0;
	uint8_t cnt1000ms;
	uint8_t Power_Status = 0;
	uint16_t Power_Cnt = 0;
	
	uint8_t Temp_CONFIGURE[2] = {0xff, 0xff};//��ʱ�����üĴ���
	uint8_t Temp_OUT[2] = {0xff, 0xff};//��ʱ����Ĵ���
	uint8_t Temp_POLARITY[2] = {0x00, 0x00};//��ʱ���뷭ת�Ĵ���
	KEY_FLAG_GRP = OSFlagCreate(0, &OS_ERR);//��ʼ���¼���־��
	while(1) {
		if(Temp_CONFIGURE[0] != EIO_PCA9539X_DEV.pINFO->CONFIGURE[0] && Temp_CONFIGURE[0] != EIO_PCA9539X_DEV.pINFO->CONFIGURE[0]) {//��������б仯����,�˿�����
			Temp_CONFIGURE[0] = EIO_PCA9539X_DEV.pINFO->CONFIGURE[0];
			Temp_CONFIGURE[1] = EIO_PCA9539X_DEV.pINFO->CONFIGURE[1];
			PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_CONFIGURE, EIO_PCA9539X_DEV.pINFO->CONFIGURE, 2);
		} else if(Temp_CONFIGURE[0] != EIO_PCA9539X_DEV.pINFO->CONFIGURE[0]) {
			Temp_CONFIGURE[0] = EIO_PCA9539X_DEV.pINFO->CONFIGURE[0];
			PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_CONFIGURE_L, &EIO_PCA9539X_DEV.pINFO->CONFIGURE[0], 1);
		} else if(Temp_CONFIGURE[1] != EIO_PCA9539X_DEV.pINFO->CONFIGURE[1]) {
			Temp_CONFIGURE[1] = EIO_PCA9539X_DEV.pINFO->CONFIGURE[1];
			PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_CONFIGURE_L, &EIO_PCA9539X_DEV.pINFO->CONFIGURE[1], 1);
		}
		if(Temp_OUT[0] != EIO_PCA9539X_DEV.pINFO->OUT[0] && Temp_OUT[1] != EIO_PCA9539X_DEV.pINFO->OUT[1]) {//�˿�����Ĵ���
			Temp_OUT[0] = EIO_PCA9539X_DEV.pINFO->OUT[0];
			Temp_OUT[1] = EIO_PCA9539X_DEV.pINFO->OUT[1];
			PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_OUTPUT, EIO_PCA9539X_DEV.pINFO->OUT, 2);
		} else if(Temp_OUT[0] != EIO_PCA9539X_DEV.pINFO->OUT[0]) {
			Temp_OUT[0] = EIO_PCA9539X_DEV.pINFO->OUT[0];
			PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_OUTPUT_L, &EIO_PCA9539X_DEV.pINFO->OUT[0], 1);
		} else if(Temp_OUT[1] != EIO_PCA9539X_DEV.pINFO->OUT[1]) {
			Temp_OUT[1] = EIO_PCA9539X_DEV.pINFO->OUT[1];
			PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_OUTPUT_H, &EIO_PCA9539X_DEV.pINFO->OUT[1], 1);
		}
		
		if(Temp_POLARITY[0] != EIO_PCA9539X_DEV.pINFO->POLARITY[0] && Temp_POLARITY[1] != EIO_PCA9539X_DEV.pINFO->POLARITY[1]) {//���뼫��
			Temp_POLARITY[0] = EIO_PCA9539X_DEV.pINFO->POLARITY[0];
			Temp_POLARITY[1] = EIO_PCA9539X_DEV.pINFO->POLARITY[1];
			PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_POLARITY, EIO_PCA9539X_DEV.pINFO->POLARITY, 2);
		} else if(Temp_POLARITY[0] != EIO_PCA9539X_DEV.pINFO->POLARITY[0]) {
			Temp_POLARITY[0] = EIO_PCA9539X_DEV.pINFO->POLARITY[0];
			PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_POLARITY_L, &EIO_PCA9539X_DEV.pINFO->POLARITY[0], 1);
		} else if(Temp_POLARITY[1] != EIO_PCA9539X_DEV.pINFO->POLARITY[1]) {
			Temp_POLARITY[1] = EIO_PCA9539X_DEV.pINFO->POLARITY[1];
			PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_POLARITY_H, &EIO_PCA9539X_DEV.pINFO->POLARITY[1], 1);
		}
		
		if(++cnt10ms >= 10) {//10ms��ȡһ�ζ˿�
			cnt10ms = 0;
			if((EIO_PCA9539X_DEV.pINFO->CONFIGURE[0] != 0) && (EIO_PCA9539X_DEV.pINFO->CONFIGURE[1] != 0)) {//���˿��Ƿ�����Ϊ���룬���������Ϊ����Ͷ�ȡ�˿�
				PCA9539X_RD(&EIO_PCA9539X_DEV, PCA9539_REG_INPUT, EIO_PCA9539X_DEV.pINFO->IN, 2);
			} else if(EIO_PCA9539X_DEV.pINFO->CONFIGURE[0] != 0) {
				PCA9539X_RD(&EIO_PCA9539X_DEV, PCA9539_REG_INPUT_L, &EIO_PCA9539X_DEV.pINFO->IN[0], 1);
			} else if(EIO_PCA9539X_DEV.pINFO->CONFIGURE[1] != 0) {
				PCA9539X_RD(&EIO_PCA9539X_DEV, PCA9539_REG_INPUT_H, &EIO_PCA9539X_DEV.pINFO->IN[1], 1);
			}
			if(++cnt1000ms >= 100) {//ÿһ����һ��оƬ�Ƿ�����
				cnt1000ms = 0;
				if((EIO_PCA9539X_DEV.pINFO->IN[0] & 0x40) || EIO_PCA9539X_DEV.pINFO->ERROR != PCA9539X_ERR_NONE) {//оƬ��ĳЩԭ��λ���ڶ�д�г��ִ���ͽ�оƬ���³�ʼ��
					PCA9539X_INIT(&EIO_PCA9539X_DEV);//��ʼ��IO��չоƬ
					PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_CONFIGURE, EIO_PCA9539X_DEV.pINFO->CONFIGURE, 2);//д�������������
					PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_OUTPUT, EIO_PCA9539X_DEV.pINFO->OUT, 2);//д���������
					PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_POLARITY, EIO_PCA9539X_DEV.pINFO->POLARITY, 2);//д��˿����뼫������
				}
			}
			if(KeyValue == 0)//û�а󶨰���
				keyvalue = KEY_NONE;
			else 
				keyvalue = Key_Value();//��ȡ����
			switch(status) {
				case 0: {
					if(keyvalue != KEY_NONE)//��������
						status = 1;
				}break;
				case 1: {//����
					if(keyvalue != KEY_NONE){//�������ڰ���
						key = keyvalue;
						status = 2;
					} else
						status = 0;
				}break;
				case 2: {//�ȴ��ͷ�
					BacklightTime = 0;
					if(BacklightFlag != 0) {//�ڵͱ���״̬״̬
						if(keyvalue == 0) { 
							BacklightFlag = 0;
							if(PowerDownFlag == 0)
								Backlight(0x40);//�ָ�����
							else
								Backlight(0x05);//�ڵ͵�ѹ״̬��
							status = 0;
						}
						break;//���β���ֻ������Ļ�������й��ܲ���
					}
					if(keyvalue == 0) {
						OSFlagPost(KEY_FLAG_GRP,//��������
									KEY_VALUE_FLAG,
									OS_FLAG_SET,
									&OS_ERR);
						*KeyValue = key;
						ContinuousCnt = 0;
						status = 3;
					} else if(++ContinuousCnt >= CONTINUOUSTIME) {//��������
						ContinuousCnt = 0;
						OSFlagPost(KEY_FLAG_GRP,//��������
									KEY_VALUE_FLAG,
									OS_FLAG_SET,
									&OS_ERR);
						*KeyValue = (KEY_VALUE_TypeDef)(keyvalue | 0x80);//��������
						status = 3;
					}
				}break;
				case 3: {
					OSFlagAccept(KEY_FLAG_GRP,//����������λ
						KEY_VALUE_FLAG,
						OS_FLAG_WAIT_SET_ANY | OS_FLAG_CONSUME,
						&OS_ERR);
					status = 4;
				}break;
				case 4: {
					if(keyvalue == 0) {
						status = 0;
					}
				}break;
			}
			if(!CHARGING_IN) {//����⣬��ص������
				POWER_KEY_SET;//MP1584ʹ��
				PowerFlag = 1;	
			}
			
			switch(Power_Status) {//���ؼ��
				case 0: {
					if(!POWER_IN) //��������
						Power_Status = 1;
				}break;
				case 1: {
					if(!POWER_IN) //����
						Power_Status = 2;
					else
						Power_Status = 0;
				}break;
				case 2: {
					if(POWER_IN) {//ȷ�ϰ��µȴ��ɿ�
						Power_Status = 0;
						Power_Cnt = 0;
					} else if(++Power_Cnt >= 100) {//1��
						Power_Cnt = 0;
						if(PowerFlag == 0) {//����
							
							POWER_KEY_SET;//MP1584ʹ��
							PowerFlag = 1;	
						}
						else if(PowerFlag == 1) {//�ػ�
							PowerFlag = 0;
							Backlight(0x00);
							OSTaskSuspend(TASK_SensorSlow_PRIO);//ֹͣ����
							OSTaskSuspend(TASK_SensorFast_PRIO);
							OSTaskSuspend(TASK_RefreshGUI_PRIO);
							OSTaskSuspend(TASK_GUI_PRIO);
							TIM_SetCompare1(TIM3, 0);
							TIM_SetCompare2(TIM3, 0);
							TIM_SetCompare3(TIM3, 0);
							TIM_SetCompare4(TIM3, 0);
						}
						Power_Status = 3;
					}
				}break;
				case 3: {
					if(POWER_IN) {//ȷ�ϰ��µȴ��ɿ�
						Power_Status = 0;
						if(PowerFlag == 0) {
							POWER_KEY_RESET;//MP1584ʧ��
						}
					}
				}break;
			}
		}
		OSTimeDly(1);
	}
}
