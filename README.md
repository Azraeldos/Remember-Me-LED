# STM32 Simon Says Memory Game

An embedded **Simon Says memory game** developed in **C using the STM32 HAL library**. The system generates an increasingly difficult LED sequence that the player must reproduce using physical push buttons, with real-time feedback through an **I2C LCD, UART terminal, LEDs, and buzzer**.

## Key Features

* **Finite State Machine (FSM)** for Menu, Sequence Playback, Player Input, and Game Over states
* **GPIO EXTI interrupts** for responsive button input with software debouncing
* **STM32 hardware RNG** for randomized game sequences with `rand()` fallback
* **Progressive difficulty** by decreasing LED playback delay as levels increase
* **I2C LCD** for game status, level, and score display
* **USART2 (115200 8N1)** for debugging and terminal output
* **Buzzer and error LED** for audio/visual game-over feedback

## Hardware

| Component       | Interface           | Purpose                    |
| --------------- | ------------------- | -------------------------- |
| STM32 MCU       | —                   | Main controller            |
| 4x LEDs         | GPIO Output         | Sequence display           |
| 4x Push Buttons | GPIO EXTI           | Player input               |
| LCD             | I2C                 | Game status and score      |
| Buzzer          | GPIO Output         | Audio feedback             |
| Error LED       | GPIO Output         | Game-over indicator        |
| USART2          | UART                | Serial debugging           |
| RNG             | Hardware Peripheral | Random sequence generation |

## Game Architecture

```text id="9xk4m2"
STATE_MENU
    │
    │ Button Press
    ▼
STATE_SHOW_SEQUENCE
    │
    │ Sequence Complete
    ▼
STATE_PLAYER_INPUT ───── Correct ─────► Level Up
    │                                     │
    │                                     └──► STATE_SHOW_SEQUENCE
    │
    │ Incorrect
    ▼
STATE_GAME_OVER
    │
    │ Button Press
    ▼
STATE_MENU
```

## Implementation Highlights

### Random Sequence Generation

```c id="j8p2w5"
uint32_t random32bit;

if (HAL_RNG_GenerateRandomNumber(&hrng, &random32bit) == HAL_OK) {
    sequence[sequence_length - 1] = (uint8_t)(random32bit % 4);
} else {
    sequence[sequence_length - 1] = (uint8_t)(rand() % 4);
}
```

### Interrupt-Driven Button Debouncing

```c id="k7n3r1"
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    uint32_t current_time = HAL_GetTick();

    if ((current_time - last_interrupt_time) < DEBOUNCE_DELAY_MS)
        return;

    last_interrupt_time = current_time;

    // Process button input
}
```

### Progressive Difficulty

```c id="m4q8t6"
uint32_t flash_delay = 800 - (current_level * 50);

if (flash_delay < 200)
    flash_delay = 200;
```

## Embedded Concepts Demonstrated

`Embedded C` • `STM32 HAL` • `GPIO` • `EXTI Interrupts` • `I2C` • `UART` • `Hardware RNG` • `Software Debouncing` • `Finite State Machines` • `Hardware/Software Integration`



---
