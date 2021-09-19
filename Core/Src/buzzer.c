#include "buzzer.h"

void buzzer__init__(buzzer_class* buzzer, TIM_HandleTypeDef* htim, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	buzzer->htim = htim;
	buzzer->GPIO_Pin = GPIO_Pin;
	buzzer->GPIOx = GPIOx;
	buzzer->alarm = NO_ALARM;
	buzzer->count = 0;
}

void buzzer_set_alarm(buzzer_class* buzzer, enum ALARM_TYPE alarm)
{
	HAL_TIM_Base_Stop_IT(buzzer->htim);
	HAL_GPIO_WritePin(buzzer->GPIOx, buzzer->GPIO_Pin, 0);
	buzzer->alarm = alarm;
	buzzer->count = 0;
	if (alarm != NO_ALARM)
		HAL_TIM_Base_Start_IT(buzzer->htim);
}

void buzzer_beep(buzzer_class* buzzer)
{
	if(buzzer->alarm == NO_ALARM)
		HAL_GPIO_WritePin(buzzer->GPIOx, buzzer->GPIO_Pin, 0);
	else if (buzzer->alarm == DANGER_ALARM)
		HAL_GPIO_WritePin(buzzer->GPIOx, buzzer->GPIO_Pin, 1);
	else
	{
		buzzer->count = (buzzer->count + 1)%(10/buzzer->alarm);
		if (buzzer->count == 0)
			HAL_GPIO_WritePin(buzzer->GPIOx, buzzer->GPIO_Pin, 1);
		else
			HAL_GPIO_WritePin(buzzer->GPIOx, buzzer->GPIO_Pin, 0);

	}
}
