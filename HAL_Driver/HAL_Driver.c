#include "HAL_Driver.h"


static uint32_t TickUs = 0;
static uint32_t TickMs = 0;
static uint64_t RunTickMs = 0;

void  SysTick_Init (uint32_t ticks)
{
	TickMs = ticks / 8 / 1000;
	TickUs = TickMs / 1000;
    SysTick->CTRL &= SysTick_CLKSource_HCLK_Div8;
    SysTick->LOAD = TickMs - 1;
    SysTick->VAL  = 0;
    NVIC_SetPriority(SysTick_IRQn, 0xff);
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
    
}

/** 
 * @brief: 
 * @param uint32_t us
 * @retval: 
 */
void Delay_us(uint32_t us)
{
    uint32_t ticksNeed = us * TickUs;
	uint32_t ticksCount = 0;
	uint32_t oldValue = SysTick->VAL;
	uint32_t newValue;
	while(1) 
	{
		newValue = SysTick->VAL;
		if(newValue != oldValue)
		{
			if(newValue < oldValue) {
				ticksCount += (oldValue-newValue);
			}
			else {
				ticksCount += (TickMs-newValue+oldValue);
			}
			oldValue = newValue;
			if(ticksCount >= ticksNeed) {
				break;
			}
		}
	}
}

/** 
 * @brief: 
 * @param uint32_t ms
 * @retval: 
 */
void Delay_ms(uint32_t ms)
{
	for(uint32_t i = 0; i < ms; i++)
		Delay_us(1000);
}

uint8_t TimeBaseFlag(TimeBaseFlag_TypeDef * pTimeBaseFlag)
{
	if(pTimeBaseFlag->TriggerTime < RunTickMs)
	{
		if(++pTimeBaseFlag->TriggerCnt >= pTimeBaseFlag->TriggerCntMax)
		{
			pTimeBaseFlag->TriggerCnt = 0;
			pTimeBaseFlag->TriggerTime = RunTickMs + pTimeBaseFlag->DelayMs;
			return 1;
		}
		return 1;
	}
	return 0;
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	RunTickMs++;
  	lv_tick_inc(1);
}
