#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "stm32f4xx_hal.h"

enum DIRECTION
{
	FORWARD,
	BACKWARD
};

typedef struct
{
	TIM_HandleTypeDef* htim;
	uint32_t timer_channel;
	GPIO_TypeDef* GPIOx_IN1;
	GPIO_TypeDef* GPIOx_IN2;
	uint16_t GPIO_Pin_IN1;
	uint16_t GPIO_Pin_IN2;
	uint16_t stopped;
	TIM_HandleTypeDef* htim_encoder;
	uint32_t htim_encoder_frequency;
	uint16_t cnt1;
	uint16_t cnt2;
	uint32_t diff;
	uint32_t current_speed;
	int error;
	int prev_error;
	int historic_error;
	int pwm;
} motor_class;

void motor__init__(motor_class* motor, TIM_HandleTypeDef* htim, uint32_t timer_channel, GPIO_TypeDef* GPIOx_IN1, GPIO_TypeDef* GPIOx_IN2,
		uint16_t GPIO_Pin_IN1, uint16_t GPIO_Pin_IN2, TIM_HandleTypeDef* htim_encoder, uint32_t htim_encoder_frequency);
void motor_start(motor_class* motor);
void motor_set_speed_and_direction(motor_class* motor, enum DIRECTION direction, int desired_speed);
void motor_stop_and_go(motor_class* motor);
int motor_is_stopped(motor_class* motor);
void _motor_compute_current_speed(motor_class* motor);
int _PID (motor_class* motor, int desired_speed, int current_speed);
int motor_get_current_speed(motor_class* motor);
int motor_get_pwm(motor_class* motor);

#endif /* INC_MOTOR_H_ */
