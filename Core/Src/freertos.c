/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "i2c_lcd.h"
#include "rng.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
    STATE_MENU,
    STATE_SHOW_SEQUENCE,
    STATE_PLAYER_INPUT,
    STATE_GAME_OVER
} GameState_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SEQUENCE_MAX_LENGTH 50

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
volatile GameState_t game_state = STATE_MENU;
uint8_t sequence[SEQUENCE_MAX_LENGTH];
uint8_t current_level = 0;
uint8_t sequence_length = 0;
uint8_t player_check_index = 0;
uint32_t random32bit;
extern volatile uint8_t button_pressed_flag;
extern volatile uint8_t last_pressed_button;
extern RNG_HandleTypeDef hrng;
/* USER CODE END Variables */
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
const osThreadAttr_t DisplayTask_attributes = {
  .name = "DisplayTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MainTask */
osThreadId_t MainTaskHandle;
const osThreadAttr_t MainTask_attributes = {
  .name = "MainTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for EXTICallback */
osEventFlagsId_t EXTICallbackHandle;
const osEventFlagsAttr_t EXTICallback_attributes = {
  .name = "EXTICallback"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void all_leds_on(void);
void all_leds_off(void);
void turn_led_on(uint8_t led_num);
void error_indicators_on(void);
void error_indicators_off(void);
void play_error_blink(void);
void generate_next_level(void);
void play_sequence(void);
uint8_t RTOS_Scroll_Interruptible(uint8_t row, const char* str, uint32_t delay_ms);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of DisplayTask */
  DisplayTaskHandle = osThreadNew(StartDefaultTask, NULL, &DisplayTask_attributes);

  /* creation of MainTask */
  MainTaskHandle = osThreadNew(StartTask02, NULL, &MainTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of EXTICallback */
  EXTICallbackHandle = osEventFlagsNew(&EXTICallback_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the MainTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  lcd_init();
  lcd_clear();
  RTOS_Scroll_Interruptible(0, "WELCOME TO SIMON SAYS", 200);
  RTOS_Scroll_Interruptible(1, "Press ANY button to start", 200);

  game_state = STATE_MENU;
  /* Infinite loop */
  for(;;)
  {
      if (button_pressed_flag) {
          uint8_t pressed = last_pressed_button;

          if (game_state == STATE_MENU) {
              button_pressed_flag = 0;
              osEventFlagsClear(EXTICallbackHandle, 0x01);
              
              turn_led_on(pressed);
              osDelay(150);
              all_leds_off();

              srand(HAL_GetTick());
              current_level = 0;
              game_state = STATE_SHOW_SEQUENCE;
          }
          else if (game_state == STATE_GAME_OVER) {
              button_pressed_flag = 0;
              osEventFlagsClear(EXTICallbackHandle, 0x01);

              turn_led_on(pressed);
              osDelay(150);
              all_leds_off();

              lcd_clear();
              lcd_put_cursor(0, 0);
              lcd_send_string("=== MAIN MENU ===");
              lcd_put_cursor(1, 0);
              lcd_send_string("Press ANY button");

              game_state = STATE_MENU;
          }
          else if (game_state == STATE_PLAYER_INPUT) {
              button_pressed_flag = 0;
              osEventFlagsClear(EXTICallbackHandle, 0x01);

              turn_led_on(pressed);
              osDelay(200);
              all_leds_off();

              if (pressed == sequence[player_check_index]) {
                  player_check_index++;

                  if (player_check_index >= sequence_length) {
                      lcd_clear();
                      lcd_put_cursor(0, 0);
                      lcd_send_string(" Correct!");
                      osDelay(500);
                      game_state = STATE_SHOW_SEQUENCE;
                  }
              } else {
                  play_error_blink();
                  char final_score_msg[64];
                  snprintf(final_score_msg, sizeof(final_score_msg), "WRONG! Level %u", current_level);

                  lcd_clear();
                  lcd_put_cursor(0, 0);
                  lcd_send_string(final_score_msg);
                  lcd_put_cursor(1, 0);
                  lcd_send_string("Press button...");

                  game_state = STATE_GAME_OVER;
              }
          }
      }
      else if (game_state == STATE_SHOW_SEQUENCE) {
          generate_next_level();
          play_sequence();
      }

      osDelay(10); // Yield to keep RTOS watchdog happy
  }
  /* USER CODE END StartTask02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
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
    if (led_num == 0)      HAL_GPIO_WritePin(LED_1_GPIO_Output_GPIO_Port, LED_1_GPIO_Output_Pin, GPIO_PIN_SET);
    else if (led_num == 1) HAL_GPIO_WritePin(LED_2_GPIO_Output_GPIO_Port, LED_2_GPIO_Output_Pin, GPIO_PIN_SET);
    else if (led_num == 2) HAL_GPIO_WritePin(LED_3_GPIO_Output_GPIO_Port, LED_3_GPIO_Output_Pin, GPIO_PIN_SET);
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
        osDelay(150);
        all_leds_off();
        error_indicators_off();
        osDelay(150);
    }
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
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "--- LEVEL %u ---", current_level);  
    
    lcd_clear();
    lcd_put_cursor(0, 0);
    lcd_send_string(buffer);
    lcd_put_cursor(1, 0);
    lcd_send_string("Watch LEDs...");
    osDelay(1000);

    uint32_t flash_delay = 800 - (current_level * 50);
    if (flash_delay < 200) flash_delay = 200;

    for (uint8_t i = 0; i < sequence_length; i++) {
        turn_led_on(sequence[i]);
        osDelay(flash_delay);
        all_leds_off();
        osDelay(flash_delay / 2);
    }

    lcd_clear();
    lcd_put_cursor(0, 0);
    lcd_send_string("Your turn!");
    
    player_check_index = 0;
    game_state = STATE_PLAYER_INPUT;
}

// RTOS-Friendly Interruptible Line Scrolling
uint8_t RTOS_Scroll_Interruptible(uint8_t row, const char* str, uint32_t delay_ms) {
    uint16_t len = strlen(str);
    
    if (len <= 16) {
        lcd_put_cursor(row, 0);
        lcd_send_string((char*)str);
        for (uint16_t i = len; i < 16; i++) lcd_send_data(' ');
        return 0;
    }

    char window[17];
    window[16] = '\0';

    for (uint16_t i = 0; i <= len; i++) {
        for (uint8_t col = 0; col < 16; col++) {
            window[col] = ((i + col) < len) ? str[i + col] : ' ';
        }
        
        lcd_put_cursor(row, 0);
        lcd_send_string(window);

        // Non-blocking RTOS delay check
        uint32_t flags = osEventFlagsWait(EXTICallbackHandle, 0x01, osFlagsNoClear, delay_ms);
        if ((flags & 0x01) != 0) {
            return 1; // Interrupted by EXTI button press
        }
    }
    return 0;
}
/* USER CODE END Application */

