/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"
#include "buzzer.h"
#include "sonar.h"
#include "i2c-lcd.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_LENGTH_DISPLAY 16
#define MAX_JOY_VALUE 40
#define TIM_ENCODER_FREQUENCY 84
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
motor_class motor1;
motor_class motor2;
uint32_t joy_pos[2];
int speed1 = 0;
int speed2 = 0;
float distance = 0.0;
float new_distance = 0.0;
sonar_class sonar;
buzzer_class buzzer;
enum ALARM_TYPE previous_alarm = NO_ALARM;
enum ALARM_TYPE current_alarm = NO_ALARM;
char diplay_string[MAX_LENGTH_DISPLAY];
uint8_t calibration = 0;
uint32_t standard_x_pos = 0;
uint32_t standard_y_pos = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM9_Init();
  MX_TIM3_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */

	motor__init__(&motor1, &htim2, TIM_CHANNEL_1, GPIOA, GPIOB, GPIO_PIN_10,
			GPIO_PIN_3,&htim4,TIM_ENCODER_FREQUENCY);
	motor__init__(&motor2, &htim2, TIM_CHANNEL_2, GPIOB, GPIOB, GPIO_PIN_5,
			GPIO_PIN_4,&htim1,TIM_ENCODER_FREQUENCY);
	sonar__init__(&sonar, &htim3, GPIOA, GPIO_PIN_12, TIM_CHANNEL_1, &htim11);
	buzzer__init__(&buzzer, &htim10, GPIOA, GPIO_PIN_11);
	motor_start(&motor1);
	motor_start(&motor2);
	lcd_init();
	HAL_ADC_Start_DMA(&hadc1, joy_pos, 2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		sonar_start(&sonar);
		HAL_Delay(250);
		lcd_clear();
		lcd_put_cur(0, 0);
		distance = sonar_get_distance(&sonar);
		sprintf(diplay_string, "Distance: %d", (int)distance);
		lcd_send_string(diplay_string);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
enum ALARM_TYPE GetAlarmType(int distance) {
	if (distance > 0 && distance <= 10)
		return DANGER_ALARM;
	else if (distance > 10 && distance <= 20)
		return ALARM_4;
	else if (distance > 20 && distance <= 40)
		return ALARM_2;
	else if (distance > 40 && distance <= 60)
		return ALARM_1;
	return NO_ALARM;
}

void CaptureCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM3) {
		new_distance = sonar_compute_distance(&sonar, htim);
		if (new_distance != -1) {
			distance = new_distance;
			current_alarm = GetAlarmType((int) distance);
			if (current_alarm != previous_alarm) {
				previous_alarm = current_alarm;
				buzzer_set_alarm(&buzzer, current_alarm);
			}
		}
	}
}

void PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM3)
		sonar_timer_overflow(&sonar, htim);
	else if (htim->Instance == TIM10)
		buzzer_beep(&buzzer);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	if (!motor_is_stopped(&motor1)) {
		speed1 = -100 + ((100 - (-100)) / MAX_JOY_VALUE) * (joy_pos[0] / 100);
		speed2 = -100 + ((100 - (-100)) / MAX_JOY_VALUE) * (joy_pos[1] / 100);
		if (calibration) {
			standard_x_pos = speed1;
			standard_y_pos = speed2;
			calibration = 0;
		}
		if (speed1 == standard_x_pos)
			speed1 = 0;
		if (speed2 == standard_y_pos)
			speed2 = 0;
		if (speed1 >= 0 && speed2 >= 0) //avanti
				{
			motor_set_speed_and_direction(&motor1, FORWARD, speed1);
			motor_set_speed_and_direction(&motor2, FORWARD, speed1);
		} else if (speed1 < 0 && speed2 < 0) //indietro
				{
			motor_set_speed_and_direction(&motor1, BACKWARD, -1 * speed1);
			motor_set_speed_and_direction(&motor2, BACKWARD, -1 * speed1);
		} else if (speed1 < 0 && speed2 >= 0) //destra
				{
			motor_set_speed_and_direction(&motor1, BACKWARD, 0);
			motor_set_speed_and_direction(&motor2, BACKWARD, speed2);
		} else if (speed1 >= 0 && speed2 < 0) //sinistra
				{
			motor_set_speed_and_direction(&motor1, BACKWARD, -1 * speed2);
			motor_set_speed_and_direction(&motor2, BACKWARD, 0);
		}
	}

	HAL_ADC_Start_DMA(&hadc1, joy_pos, 2);
}

void Emergency_Stop_And_Go(void) {
	motor_stop_and_go(&motor1);
	motor_stop_and_go(&motor2);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
