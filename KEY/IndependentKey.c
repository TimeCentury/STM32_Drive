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
	keyvalue = Key_Value();//读取按键
	switch(status) {
		case 0: {
			if(keyvalue != 0)//按键按下
				status = 1;
		}break;
		case 1: {//消抖
			if(keyvalue != 0) {//如果还在按下
				re_key = keyvalue;
				status = 2;
			}
		}break;
		case 2: {//等待释放
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
	
	uint8_t Temp_CONFIGURE[2] = {0xff, 0xff};//临时的配置寄存器
	uint8_t Temp_OUT[2] = {0xff, 0xff};//临时输出寄存器
	uint8_t Temp_POLARITY[2] = {0x00, 0x00};//临时输入翻转寄存器
	KEY_FLAG_GRP = OSFlagCreate(0, &OS_ERR);//初始化事件标志组
	while(1) {
		if(Temp_CONFIGURE[0] != EIO_PCA9539X_DEV.pINFO->CONFIGURE[0] && Temp_CONFIGURE[0] != EIO_PCA9539X_DEV.pINFO->CONFIGURE[0]) {//如果数据有变化更新,端口配置
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
		if(Temp_OUT[0] != EIO_PCA9539X_DEV.pINFO->OUT[0] && Temp_OUT[1] != EIO_PCA9539X_DEV.pINFO->OUT[1]) {//端口输出寄存器
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
		
		if(Temp_POLARITY[0] != EIO_PCA9539X_DEV.pINFO->POLARITY[0] && Temp_POLARITY[1] != EIO_PCA9539X_DEV.pINFO->POLARITY[1]) {//输入极性
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
		
		if(++cnt10ms >= 10) {//10ms读取一次端口
			cnt10ms = 0;
			if((EIO_PCA9539X_DEV.pINFO->CONFIGURE[0] != 0) && (EIO_PCA9539X_DEV.pINFO->CONFIGURE[1] != 0)) {//检测端口是否被配置为输入，如果被配置为输入就读取端口
				PCA9539X_RD(&EIO_PCA9539X_DEV, PCA9539_REG_INPUT, EIO_PCA9539X_DEV.pINFO->IN, 2);
			} else if(EIO_PCA9539X_DEV.pINFO->CONFIGURE[0] != 0) {
				PCA9539X_RD(&EIO_PCA9539X_DEV, PCA9539_REG_INPUT_L, &EIO_PCA9539X_DEV.pINFO->IN[0], 1);
			} else if(EIO_PCA9539X_DEV.pINFO->CONFIGURE[1] != 0) {
				PCA9539X_RD(&EIO_PCA9539X_DEV, PCA9539_REG_INPUT_H, &EIO_PCA9539X_DEV.pINFO->IN[1], 1);
			}
			if(++cnt1000ms >= 100) {//每一秒检测一次芯片是否正常
				cnt1000ms = 0;
				if((EIO_PCA9539X_DEV.pINFO->IN[0] & 0x40) || EIO_PCA9539X_DEV.pINFO->ERROR != PCA9539X_ERR_NONE) {//芯片被某些原因复位或在读写中出现错误就将芯片重新初始化
					PCA9539X_INIT(&EIO_PCA9539X_DEV);//初始化IO扩展芯片
					PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_CONFIGURE, EIO_PCA9539X_DEV.pINFO->CONFIGURE, 2);//写入输入输出配置
					PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_OUTPUT, EIO_PCA9539X_DEV.pINFO->OUT, 2);//写入输出配置
					PCA9539X_WR(&EIO_PCA9539X_DEV, PCA9539_REG_POLARITY, EIO_PCA9539X_DEV.pINFO->POLARITY, 2);//写入端口输入极性配置
				}
			}
			if(KeyValue == 0)//没有绑定按键
				keyvalue = KEY_NONE;
			else 
				keyvalue = Key_Value();//读取按键
			switch(status) {
				case 0: {
					if(keyvalue != KEY_NONE)//按键按下
						status = 1;
				}break;
				case 1: {//消抖
					if(keyvalue != KEY_NONE){//按键还在按下
						key = keyvalue;
						status = 2;
					} else
						status = 0;
				}break;
				case 2: {//等待释放
					BacklightTime = 0;
					if(BacklightFlag != 0) {//在低背光状态状态
						if(keyvalue == 0) { 
							BacklightFlag = 0;
							if(PowerDownFlag == 0)
								Backlight(0x40);//恢复背光
							else
								Backlight(0x05);//在低电压状态下
							status = 0;
						}
						break;//本次操作只点亮屏幕，不进行功能操作
					}
					if(keyvalue == 0) {
						OSFlagPost(KEY_FLAG_GRP,//按键就绪
									KEY_VALUE_FLAG,
									OS_FLAG_SET,
									&OS_ERR);
						*KeyValue = key;
						ContinuousCnt = 0;
						status = 3;
					} else if(++ContinuousCnt >= CONTINUOUSTIME) {//触发长按
						ContinuousCnt = 0;
						OSFlagPost(KEY_FLAG_GRP,//按键就绪
									KEY_VALUE_FLAG,
									OS_FLAG_SET,
									&OS_ERR);
						*KeyValue = (KEY_VALUE_TypeDef)(keyvalue | 0x80);//长按按键
						status = 3;
					}
				}break;
				case 3: {
					OSFlagAccept(KEY_FLAG_GRP,//消除按键就位
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
			if(!CHARGING_IN) {//充电检测，电池电量检测
				POWER_KEY_SET;//MP1584使能
				PowerFlag = 1;	
			}
			
			switch(Power_Status) {//开关检测
				case 0: {
					if(!POWER_IN) //按键按下
						Power_Status = 1;
				}break;
				case 1: {
					if(!POWER_IN) //消抖
						Power_Status = 2;
					else
						Power_Status = 0;
				}break;
				case 2: {
					if(POWER_IN) {//确认按下等待松开
						Power_Status = 0;
						Power_Cnt = 0;
					} else if(++Power_Cnt >= 100) {//1秒
						Power_Cnt = 0;
						if(PowerFlag == 0) {//开机
							
							POWER_KEY_SET;//MP1584使能
							PowerFlag = 1;	
						}
						else if(PowerFlag == 1) {//关机
							PowerFlag = 0;
							Backlight(0x00);
							OSTaskSuspend(TASK_SensorSlow_PRIO);//停止任务
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
					if(POWER_IN) {//确认按下等待松开
						Power_Status = 0;
						if(PowerFlag == 0) {
							POWER_KEY_RESET;//MP1584失能
						}
					}
				}break;
			}
		}
		OSTimeDly(1);
	}
}
