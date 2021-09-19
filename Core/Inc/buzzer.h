#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "stm32f4xx_hal.h"

enum ALARM_TYPE
{
	NO_ALARM,
	ALARM_1,
	ALARM_2,
	ALARM_3,
	ALARM_4,
	DANGER_ALARM
};

typedef struct
{
	TIM_HandleTypeDef* htim;
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	uint16_t count;
	enum ALARM_TYPE alarm;
} buzzer_class;

void buzzer__init__(buzzer_class* buzzer, TIM_HandleTypeDef* htim, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void buzzer_set_alarm(buzzer_class* buzzer, enum ALARM_TYPE type);
void buzzer_beep(buzzer_class* buzzer);

#endif /* INC_BUZZER_H_ */
