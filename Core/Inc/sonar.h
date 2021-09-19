#ifndef sonar_h_
#define sonar_h_

#include "stm32f4xx_hal.h"

typedef struct
{
	uint8_t reading_edge; //0 if rising edge, 1 if falling
	uint32_t T1; //Starting time
	uint32_t T2; //Ending time
	uint32_t TIMER_OVERFLOW; // counter of the timer overflow
	float distance; //last distance from an obstacle computed
	TIM_HandleTypeDef* htim; //timer used for the sonar
	TIM_HandleTypeDef* htim_delay;
	GPIO_TypeDef* GPIOx; //GPIO channel of the trigger
	uint16_t GPIO_Pin; //GPIO number of the trigger
	uint32_t timer_channel;
} sonar_class;

void sonar__init__(sonar_class* sonar, TIM_HandleTypeDef* htim, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t timer_channel, TIM_HandleTypeDef* htim_delay);
void sonar_timer_overflow(sonar_class* sonar, TIM_HandleTypeDef* htim);
float sonar_get_distance(sonar_class* sonar);
void sonar_start(sonar_class* sonar);
float sonar_compute_distance(sonar_class* sonar, TIM_HandleTypeDef* htim);
void usDelay(sonar_class* sonar, uint32_t uSec);
uint8_t sonar_get_reading_edge(sonar_class* sonar);

#endif
