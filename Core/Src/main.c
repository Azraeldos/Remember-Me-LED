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
#include "dac.h"
#include "i2c.h"
#include "rng.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "i2c_lcd.h"
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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void all_leds_on(void){
	HAL_GPIO_WritePin(LED_1_GPIO_Output_GPIO_Port, LED_1_GPIO_Output_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_2_GPIO_Output_GPIO_Port, LED_2_GPIO_Output_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_3_GPIO_Output_GPIO_Port, LED_3_GPIO_Output_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_4_GPIO_Output_GPIO_Port, LED_4_GPIO_Output_Pin, GPIO_PIN_SET);
}
void all_leds_off(void){
	HAL_GPIO_WritePin(LED_1_GPIO_Output_GPIO_Port, LED_1_GPIO_Output_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_2_GPIO_Output_GPIO_Port, LED_2_GPIO_Output_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED_3_GPIO_Output_GPIO_Port, LED_3_GPIO_Output_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_4_GPIO_Output_GPIO_Port, LED_4_GPIO_Output_Pin, GPIO_PIN_RESET);
}
void turn_led_on(uint8_t led_num) {
    if (led_num == 0)      	HAL_GPIO_WritePin(LED_1_GPIO_Output_GPIO_Port, LED_1_GPIO_Output_Pin, GPIO_PIN_SET);
    else if (led_num == 1) 	HAL_GPIO_WritePin(LED_2_GPIO_Output_GPIO_Port, LED_2_GPIO_Output_Pin, GPIO_PIN_SET);
    else if (led_num == 2) 	HAL_GPIO_WritePin(LED_3_GPIO_Output_GPIO_Port, LED_3_GPIO_Output_Pin, GPIO_PIN_SET);
    else if (led_num == 3) HAL_GPIO_WritePin(LED_4_GPIO_Output_GPIO_Port, LED_4_GPIO_Output_Pin, GPIO_PIN_SET);
}
void error_indicators_on(void) {
    HAL_GPIO_WritePin(GPIOB, ERR_LED_GPIO_Output_Pin | ERR_BUZZER_GPIO_Output_Pin, GPIO_PIN_SET);
}

void error_indicators_off(void) {
    HAL_GPIO_WritePin(GPIOB, ERR_LED_GPIO_Output_Pin | ERR_BUZZER_GPIO_Output_Pin, GPIO_PIN_RESET);
}
void play_error_blink(void) {
    for (int i = 0; i < 4; i++) {
        all_leds_on();
        error_indicators_on();
        HAL_Delay(150);
        all_leds_off();
        error_indicators_off();
        HAL_Delay(150);
    }
}

void UART_Print(const char *str) {
    HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}
void generate_next_level(void) {
    current_level++;
    sequence_length = current_level + 1;

    if (HAL_RNG_GenerateRandomNumber(&hrng, &random32bit) == HAL_OK) {
        sequence[sequence_length - 1] = (uint8_t)(random32bit % 4);
    } else {
        sequence[sequence_length - 1] = rand() % 4;
    }
}
void play_sequence(void) {
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "\r\n--- LEVEL %u ---\r\nWatch the LEDs closely...\r\n", current_level);
    UART_Print(buffer);
    HAL_Delay(1000);


    uint32_t flash_delay = 800 - (current_level * 50);
    if (flash_delay < 200) flash_delay = 200; // Speed floor

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
  MX_I2C1_Init();
  MX_DAC1_Init();
  /* USER CODE BEGIN 2 */
    UART_Print("\r\n=========================\r\n");
    UART_Print("   WELCOME TO SIMON SAYS   \r\n");
    UART_Print("=========================\r\n");
    UART_Print("Press ANY button to start playing.\r\n");
lcd_init();
    game_state = STATE_MENU;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 while (1)
  {
    // lcd_put_cursor(0, 0);
    // lcd_send_string("Hello World!");
    // lcd_put_cursor(1, 0);
    // lcd_send_string("From STM32Nucleo!");
    // HAL_Delay(50);

      if (button_pressed_flag) {
          uint8_t pressed = last_pressed_button;

          
          if (game_state == STATE_MENU) {
              button_pressed_flag = 0;
              turn_led_on(pressed);
              HAL_Delay(150);
              all_leds_off();

              srand(HAL_GetTick());
              current_level = 0;
              game_state = STATE_SHOW_SEQUENCE;
          }

         
          else if (game_state == STATE_GAME_OVER) {
              button_pressed_flag = 0;
              turn_led_on(pressed);
              HAL_Delay(150);
              all_leds_off();
              UART_Print("\r\n=== MAIN MENU ===\r\nPress ANY button to start a new game.\r\n");
              game_state = STATE_MENU;
          }

          
          else if (game_state == STATE_PLAYER_INPUT) {
              button_pressed_flag = 0;
              turn_led_on(pressed);
              HAL_Delay(200);
              all_leds_off();

              
              if (pressed == sequence[player_check_index]) {
                  player_check_index++;

                  if (player_check_index >= sequence_length) {
                      UART_Print("✨ Correct!\r\n");
                      HAL_Delay(500);
                      game_state = STATE_SHOW_SEQUENCE;
                  }
              } else {
                  char final_score_msg[128];
                  snprintf(final_score_msg, sizeof(final_score_msg),
                           "\r\n❌ WRONG BUTTON! Game Over.\r\nYou reached Level %u.\r\nPress ANY button to return to menu.\r\n",
                           current_level);
                  UART_Print(final_score_msg);

                  play_error_blink();
                  game_state = STATE_GAME_OVER;
              }
          }
      }

      else if (game_state == STATE_SHOW_SEQUENCE) {
          generate_next_level();
          play_sequence();
      }

      HAL_Delay(10);
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  
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
  RCC_OscInitStruct.PLL.PLLN = 8;
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = HAL_GetTick();

    // Debounce check
    if ((current_time - last_interrupt_time) < DEBOUNCE_DELAY_MS) {
        return;
    }
    last_interrupt_time = current_time;

    // Identify which pin was pressed and set the flag
    if (GPIO_Pin == BTN_1_GPIO_EXTI9_Pin) {
        last_pressed_button = 0;
        button_pressed_flag = 1;
    } else if (GPIO_Pin == BTN_2_GPIO_EXTI8_Pin) {
        last_pressed_button = 1;
        button_pressed_flag = 1;
    } else if (GPIO_Pin == BTN_3_GPIO_EXTI6_Pin) {
        last_pressed_button = 2;
        button_pressed_flag = 1;
    }   else if (GPIO_Pin == BTN_4_GPIO_EXTI7_Pin) {
        last_pressed_button = 3;
        button_pressed_flag = 1;
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
