#include "sonar.h"

void sonar__init__(sonar_class* sonar, TIM_HandleTypeDef* htim, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t timer_channel, TIM_HandleTypeDef* htim_delay) //receives in input the instance of the sonar and the timer used
{
	sonar->reading_edge = 0;
	sonar->T1 = 0;
	sonar->T2 = 0;
	sonar->TIMER_OVERFLOW = 0;
	sonar->distance = 0.0;
	sonar->htim = htim;
	sonar->GPIOx = GPIOx;
	sonar->GPIO_Pin = GPIO_Pin;
	sonar->timer_channel = timer_channel;
	sonar->htim_delay = htim_delay;
}

void sonar_timer_overflow(sonar_class* sonar, TIM_HandleTypeDef* htim)
{
	if (htim->Instance == sonar->htim->Instance)
	{
		sonar->TIMER_OVERFLOW++;
	}
}

float sonar_get_distance(sonar_class* sonar)
{
	return sonar->distance;
}

void sonar_start(sonar_class* sonar)
{
	HAL_GPIO_WritePin(sonar->GPIOx, sonar->GPIO_Pin, GPIO_PIN_RESET);
	usDelay(sonar, 3);
	HAL_GPIO_WritePin(sonar->GPIOx, sonar->GPIO_Pin, GPIO_PIN_SET);
	usDelay(sonar, 10);
	HAL_GPIO_WritePin(sonar->GPIOx, sonar->GPIO_Pin, GPIO_PIN_RESET);
	HAL_TIM_IC_Start_IT(sonar->htim, sonar->timer_channel);
}

float sonar_compute_distance(sonar_class* sonar, TIM_HandleTypeDef* htim)
{
	if (htim->Instance == sonar->htim->Instance)
	{
		if(sonar->reading_edge == 0)
		{
			sonar->T1 = HAL_TIM_ReadCapturedValue(htim, sonar->timer_channel);
			sonar->reading_edge = 1;
			sonar->TIMER_OVERFLOW = 0;
			return -1;
		}
		else
		{
			sonar->T2 = HAL_TIM_ReadCapturedValue(htim, sonar->timer_channel);
			sonar->T2 += (sonar->TIMER_OVERFLOW * htim->Instance->ARR);
			if (((sonar->T2 - sonar->T1) + 0.0f) * (0.0343/2) < 400.0)
				sonar->distance = ((sonar->T2 - sonar->T1) + 0.0f) * (0.0343/2);
			sonar->reading_edge = 0;
			HAL_TIM_IC_Stop_IT(sonar->htim, sonar->timer_channel);
			return sonar->distance;
		}
	}
	return -1;
}

void usDelay(sonar_class* sonar, uint32_t uSec)
{
	if(uSec < 2) uSec = 2;
	sonar->htim_delay->Instance->ARR = uSec - 1; 	/*sets the value in the auto-reload register*/
	sonar->htim_delay->Instance->EGR = 1; 			/*Re-initialises the timer*/
	sonar->htim_delay->Instance->SR &= ~1; 		//Resets the flag
	sonar->htim_delay->Instance->CR1 |= 1; 		//Enables the counter
	while((sonar->htim_delay->Instance->SR&0x0001) != 1);
	sonar->htim_delay->Instance->SR &= ~(0x0001);
}

uint8_t sonar_get_reading_edge(sonar_class* sonar){
	return sonar->reading_edge;
}
