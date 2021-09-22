#include "motor.h"

float P = 2.3; //112.75;//6.7229;
float I = 0.67;//337;//329.5087;
float D = 0;//52.25;//0;
uint16_t INTEGER_WINDUP = 1000;

void motor__init__(motor_class* motor, TIM_HandleTypeDef* htim, uint32_t timer_channel, GPIO_TypeDef* GPIOx_IN1, GPIO_TypeDef* GPIOx_IN2,
		uint16_t GPIO_Pin_IN1, uint16_t GPIO_Pin_IN2, TIM_HandleTypeDef* htim_encoder, uint32_t htim_encoder_frequency)
{
	motor->htim = htim;
	motor->timer_channel = timer_channel;
	motor->GPIO_Pin_IN1 = GPIO_Pin_IN1;
	motor->GPIO_Pin_IN2 = GPIO_Pin_IN2;
	motor->GPIOx_IN1 = GPIOx_IN1;
	motor->GPIOx_IN2 = GPIOx_IN2;
	HAL_GPIO_WritePin(motor->GPIOx_IN1, motor->GPIO_Pin_IN1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(motor->GPIOx_IN2, motor->GPIO_Pin_IN2, GPIO_PIN_RESET);
	motor->htim_encoder = htim_encoder;
	motor->htim_encoder_frequency = htim_encoder_frequency;
	motor->cnt1 = 0;
	motor->cnt2 = 0;
	motor->current_speed = 0;
	motor->error = 0;
	motor->prev_error = 0;
	motor->historic_error = 0;
	motor->diff = 0;
	motor->pwm = 0;
	motor->stopped = 1;
}

void motor_start(motor_class* motor)
{
	HAL_TIM_PWM_Start(motor->htim, motor->timer_channel);
	HAL_TIM_Encoder_Start(motor->htim_encoder, TIM_CHANNEL_ALL);
	motor->cnt1 = __HAL_TIM_GET_COUNTER(motor->htim_encoder);
	motor->stopped = 0;
}

void motor_set_speed_and_direction(motor_class* motor, enum DIRECTION direction, int desired_speed)
{
	_motor_compute_current_speed(motor);
	if(!motor->stopped){
		// __HAL_TIM_SET_COMPARE(motor->htim, motor->timer_channel, 10*desired_speed);
		
		if (desired_speed != 0)
			__HAL_TIM_SET_COMPARE(motor->htim, motor->timer_channel, _PID(motor, desired_speed, motor->current_speed));
		else {
			motor->error = 0;
			motor->prev_error = 0;
			motor->historic_error = 0;
			__HAL_TIM_SET_COMPARE(motor->htim, motor->timer_channel, 0);
		} 

		if(direction == FORWARD)
		{
			HAL_GPIO_WritePin(motor->GPIOx_IN1, motor->GPIO_Pin_IN1, GPIO_PIN_SET);
			HAL_GPIO_WritePin(motor->GPIOx_IN2, motor->GPIO_Pin_IN2, GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_WritePin(motor->GPIOx_IN1, motor->GPIO_Pin_IN1, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(motor->GPIOx_IN2, motor->GPIO_Pin_IN2, GPIO_PIN_SET);
		}
	}
}

void _motor_compute_current_speed(motor_class* motor)
{
	motor->cnt2 = __HAL_TIM_GET_COUNTER(motor->htim_encoder);
	if (motor->cnt2 >= motor->cnt1)
		motor->diff = motor->cnt2 - motor->cnt1;
	else
		motor->diff = (__HAL_TIM_GET_AUTORELOAD(motor->htim_encoder) - motor->cnt1) + motor->cnt2;


	motor->cnt1 = __HAL_TIM_GET_COUNTER(motor->htim_encoder);
	motor->current_speed = (int)(((motor->htim_encoder_frequency * 1000000) / (motor->htim_encoder->Instance->PSC + 1)) / motor->diff) * 60;
}

int _PID (motor_class* motor, int desired_speed, int current_speed)
{
	motor->prev_error = motor->error;
	motor->error = desired_speed - current_speed;
	motor->historic_error += motor->error;
	if(motor->historic_error > INTEGER_WINDUP)
		motor->historic_error = INTEGER_WINDUP;
	if(motor->historic_error < -1*INTEGER_WINDUP)
			motor->historic_error = -1*INTEGER_WINDUP;

	motor->pwm = ( (motor->error * P) + (motor->historic_error * I) + (motor->error - motor->prev_error) * D);
	if(motor->pwm < 0) motor->pwm = 0;
	return motor->pwm;
}

void motor_stop_and_go(motor_class* motor)
{
	if(!motor->stopped){
		motor->stopped = 1;
		HAL_GPIO_WritePin(motor->GPIOx_IN1, motor->GPIO_Pin_IN1, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(motor->GPIOx_IN2, motor->GPIO_Pin_IN2, GPIO_PIN_RESET);
		HAL_TIM_PWM_Stop(motor->htim, motor->timer_channel);
	}
	else
		motor_start(motor);
}

int motor_is_stopped(motor_class* motor)
{
	return motor->stopped;
}

int motor_get_current_speed(motor_class* motor)
{
	return motor->current_speed;
}

int motor_get_pwm(motor_class* motor){
	return motor->pwm;
}
