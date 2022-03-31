/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 * Copyright (c) 2022 Muchirijohn
 * License: MIT
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t address;
uint8_t cTemp;
uint8_t fTemp;
uint8_t humidity;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void uart_puts(char ch);
void uart_print(char *ch);
uint8_t sht30x_get();
void print_SHT30_values();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
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
	MX_I2C1_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */
	uart_print("SHT30 Sensor Demo\n");
	//init sht30x temp&hum sensor
	address = 0x44 << 1;
	uart_print("Init..\n");
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		print_SHT30_values();
		HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
		HAL_Delay(500);
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1
			| RCC_PERIPHCLK_I2C1;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void uart_puts(char ch) {
	HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, 100);
}
void uart_print(char *ch) {
	while (*ch) {
		uart_puts(*ch++);
	}
}

/**
 * get sensor values
 */
uint8_t sht30x_get() {
	uint8_t data[6] = { 0 };
	uint8_t config[2] = { 0 };
	uint8_t state = 0;

	int temp = 0;

	//check if sht30 is ready
	state = HAL_I2C_IsDeviceReady(&hi2c1, address, 2, 2000);

	if (state != HAL_OK) {
		//uart_print("SHT30 DEVICE NOT READY\n");
		return 0;
	}
	//commands to send to sht30 sensor via i2c
	config[0] = 0x2c;
	config[1] = 0x06;
	//send config data
	state = HAL_I2C_Master_Transmit(&hi2c1, address, &config[0], 2, 1500);
	//if transmit failed return
	if (state != HAL_OK) {
		//uart_print("SHT30 DEVICE TRANSMIT ERROR\n");
		return 0;
	}
	//delay 5ms
	HAL_Delay(5);
	//receive bytes
	state = HAL_I2C_Master_Receive(&hi2c1, address, &data[0], 6, 1500);
	//if receive data failed return
	if (state != HAL_OK) {
		//uart_print("SHT30 DEVICE RECEIVE ERROR\n");
		return 0;
	}
	//delay 50ms
	HAL_Delay(50);

	temp = (data[0] * 256 + data[1]);
	cTemp = (uint8_t) (-45 + (float) (175.0 * (temp / 65535.0)));
	fTemp = (uint8_t) (-49 + (float) (315.0 * (temp / 65535.0)));
	humidity = (uint8_t) (100 * (float) ((data[3] * 256 + data[4]) / 65535.0));

	return 1;
}

/**
 * print sensor values
 */
void print_SHT30_values() {
	char buffer[4] = { 0 };
	uint8_t state = 0;
	//read sensor
	state = sht30x_get();
	//show error message if reading failed
	if (state == 0) {
		uart_print("Failed to Read.\n");
		return;
	}
	//print the reading
	itoa(cTemp, buffer, 10);
	uart_print("SHT30 >> C: ");
	uart_print(buffer);
	memset(buffer, 0, 4);
	//humidity
	itoa(humidity, buffer, 10);
	uart_print(", H: ");
	uart_print(buffer);
	uart_puts('\n');
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
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
