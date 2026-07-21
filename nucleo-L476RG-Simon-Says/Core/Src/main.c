/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
RNG_HandleTypeDef hrng;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

#define SEQUENCE_MAX_LENGTH 50
#define DEBOUNCE_DELAY_MS   150

typedef enum {
    STATE_MENU,
    STATE_SHOW_SEQUENCE,
    STATE_PLAYER_INPUT,
    STATE_GAME_OVER
} GameState_t;

volatile GameState_t game_state = STATE_MENU;

uint8_t sequence[SEQUENCE_MAX_LENGTH];
uint8_t current_level = 0;
uint8_t sequence_length = 0;
uint8_t player_check_index = 0;
uint32_t random32bit;
volatile uint8_t button_pressed_flag = 0;
volatile uint8_t last_pressed_button = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RNG_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void all_leds_on(void){
	HAL_GPIO_WritePin(LED_1_GPIO_Output_GPIO_Port, LED_1_GPIO_Output_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_2_GPIO_Output_GPIO_Port, LED_2_GPIO_Output_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_3_GPIO_Output_GPIO_Port, LED_3_GPIO_Output_Pin, GPIO_PIN_SET);
}
void all_leds_off(void){
	HAL_GPIO_WritePin(LED_1_GPIO_Output_GPIO_Port, LED_1_GPIO_Output_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_2_GPIO_Output_GPIO_Port, LED_2_GPIO_Output_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_3_GPIO_Output_GPIO_Port, LED_3_GPIO_Output_Pin, GPIO_PIN_RESET);
}
void turn_led_on(uint8_t led_num) {
    if (led_num == 0)      	HAL_GPIO_WritePin(LED_1_GPIO_Output_GPIO_Port, LED_1_GPIO_Output_Pin, GPIO_PIN_SET);
    else if (led_num == 1) 	HAL_GPIO_WritePin(LED_2_GPIO_Output_GPIO_Port, LED_2_GPIO_Output_Pin, GPIO_PIN_SET);
    else if (led_num == 2) 	HAL_GPIO_WritePin(LED_3_GPIO_Output_GPIO_Port, LED_3_GPIO_Output_Pin, GPIO_PIN_SET);

}
void play_error_blink(void) {
    for (int i = 0; i < 4; i++) {
        all_leds_on();
        HAL_Delay(150);
        all_leds_off();
        HAL_Delay(150);
    }
}

void UART_Print(const char *str) {
    HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}
void generate_next_level(void) {
    current_level++;
    sequence_length = current_level + 1;

    // Grab a hardware-generated true random 32-bit integer
    if (HAL_RNG_GenerateRandomNumber(&hrng, &random32bit) == HAL_OK) {
        sequence[sequence_length - 1] = (uint8_t)(random32bit % 3);
    } else {
        // Fallback to basic math if peripheral encounters a clock/seed error
        sequence[sequence_length - 1] = rand() % 3;
    }
}
void play_sequence(void) {
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "\r\n--- LEVEL %u ---\r\nWatch the LEDs closely...\r\n", current_level);
    UART_Print(buffer);
    HAL_Delay(1000);

    // Calculate flashing speed
    uint32_t flash_delay = 800 - (current_level * 50);
    if (flash_delay < 200) flash_delay = 200; // Speed floor so it's humanly possible

    for (uint8_t i = 0; i < sequence_length; i++) {
        turn_led_on(sequence[i]);
        HAL_Delay(flash_delay);
        all_leds_off();
        HAL_Delay(flash_delay / 2); // Small gap between consecutive identical flashes
    }
    UART_Print("Your turn! Repeat the pattern...\r\n");
    player_check_index = 0;
    game_state = STATE_PLAYER_INPUT;
}
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
  MX_USART2_UART_Init();
  MX_RNG_Init();
  /* USER CODE BEGIN 2 */
    UART_Print("\r\n=========================\r\n");
    UART_Print("   WELCOME TO SIMON SAYS   \r\n");
    UART_Print("=========================\r\n");
    UART_Print("Press ANY button to start playing.\r\n");

    game_state = STATE_MENU;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // --- STATE: SHOW SEQUENCE ---
	        if (game_state == STATE_SHOW_SEQUENCE) {
	            generate_next_level();
	            play_sequence();
	        }

	        // --- STATE: PROCESS PLAYER INPUT ---
	        else if (game_state == STATE_PLAYER_INPUT && button_pressed_flag) {
	            // Clear the event flag immediately
	            button_pressed_flag = 0;
	            uint8_t pressed_button = last_pressed_button;

	            // Provide instant visual feedback safely on the main thread
	            turn_led_on(pressed_button);
	            HAL_Delay(150);
	            all_leds_off();

	            // Evaluate the player's entry against the sequence
	            if (pressed_button == sequence[player_check_index]) {
	                player_check_index++;

	                // Did they successfully replicate the entire level pattern?
	                if (player_check_index >= sequence_length) {
	                    UART_Print("✨ Correct!\r\n");
	                    HAL_Delay(500);
	                    game_state = STATE_SHOW_SEQUENCE;
	                }
	            } else {
	                // Wrong button sequence picked
	                char final_score_msg[128];
	                snprintf(final_score_msg, sizeof(final_score_msg),
	                         "\r\n❌ WRONG BUTTON! Game Over.\r\nYou reached Level %u.\r\nPress ANY button to return to menu.\r\n",
	                         current_level);
	                UART_Print(final_score_msg);

	                play_error_blink();
	                game_state = STATE_GAME_OVER;
	            }
	        }

	        // Small poll rate yield to keep power stable
	        HAL_Delay(10);
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED_1_GPIO_Output_Pin|LED_2_GPIO_Output_Pin|LED_3_GPIO_Output_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN_3_GPIO_EXTI6_Pin BTN_2_GPIO_EXTI8_Pin BTN_1_GPIO_EXTI9_Pin */
  GPIO_InitStruct.Pin = BTN_3_GPIO_EXTI6_Pin|BTN_2_GPIO_EXTI8_Pin|BTN_1_GPIO_EXTI9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_1_GPIO_Output_Pin LED_2_GPIO_Output_Pin LED_3_GPIO_Output_Pin */
  GPIO_InitStruct.Pin = LED_1_GPIO_Output_Pin|LED_2_GPIO_Output_Pin|LED_3_GPIO_Output_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    // Software Debounce: ignore accidental noisy contact triggers
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = HAL_GetTick();
    if ((current_time - last_interrupt_time) < DEBOUNCE_DELAY_MS) {
        return;
    }
    last_interrupt_time = current_time;

    // --- MENU CONTROLS ---
    if (game_state == STATE_MENU) {
        if (GPIO_Pin == BTN_1_GPIO_EXTI9_Pin || GPIO_Pin == BTN_2_GPIO_EXTI8_Pin || GPIO_Pin == BTN_3_GPIO_EXTI6_Pin) {
            // Seed our random number generator using system up-time clock ticks
            srand(HAL_GetTick());
            current_level = 0;
            game_state = STATE_SHOW_SEQUENCE;
        }
    }

    // --- STANDBY/GAME OVER OVER CONTROLS ---
    else if (game_state == STATE_GAME_OVER) {
        if (GPIO_Pin == BTN_1_GPIO_EXTI9_Pin || GPIO_Pin == BTN_2_GPIO_EXTI8_Pin || GPIO_Pin == BTN_3_GPIO_EXTI6_Pin) {
            UART_Print("\r\n=== MAIN MENU ===\r\nPress ANY button to start a new game.\r\n");
            game_state = STATE_MENU;
        }
    }

    // --- LIVE ACTIVE GAMEPLAY CONTROLS ---
    else if (game_state == STATE_PLAYER_INPUT) {
        uint8_t pressed_button = 99;
        // Identify which hardware trigger dropped low
        if (GPIO_Pin == BTN_1_GPIO_EXTI9_Pin)      pressed_button = 0;
        else if (GPIO_Pin == BTN_2_GPIO_EXTI8_Pin) pressed_button = 1;
        else if (GPIO_Pin == BTN_3_GPIO_EXTI6_Pin) pressed_button = 2;

        if (pressed_button != 99) {
            // Provide instant visual feedback by illuminating the user's pressed selector
            turn_led_on(pressed_button);
            all_leds_off();

            // Evaluate against the correct historical code step
            if (pressed_button == sequence[player_check_index]) {
                player_check_index++;

                // Did they clear the entire sequence pattern?
                if (player_check_index >= sequence_length) {
                    UART_Print("✨ Correct!\r\n");
                    HAL_Delay(500);
                    game_state = STATE_SHOW_SEQUENCE; // Set flags back to display engine loop
                }
            } else {
                // Wrong step picked!
                char final_score_msg[128];
                snprintf(final_score_msg, sizeof(final_score_msg), "\r\n❌ WRONG BUTTON! Game Over.\r\nYou reached Level %u.\r\nPress ANY button to return to menu.\r\n", current_level);
                UART_Print(final_score_msg);

                play_error_blink();
                game_state = STATE_GAME_OVER;
            }
        }
    }
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
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
