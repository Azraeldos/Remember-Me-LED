# STM32 Simon Says Memory Game

An embedded **Simon Says memory game** developed in **C using STM32 HAL and FreeRTOS**. The system generates an increasingly difficult LED sequence that the player must reproduce using physical push buttons, with real-time feedback through an **I2C LCD, UART terminal, LEDs, and buzzer**.

## Key Features

* **FreeRTOS-based architecture** using multiple tasks (threads) for concurrent game operations
* **RTOS event synchronization** for communication between tasks and hardware events
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

## FreeRTOS Architecture

The original bare-metal implementation was redesigned around **FreeRTOS**, separating application functionality into concurrent tasks and using RTOS events for synchronization.

```text
              ┌────────────────────┐
              │      FreeRTOS      │
              │     Scheduler      │
              └─────────┬──────────┘
                        │
          ┌─────────────┼─────────────┐
          ▼             ▼             ▼
     Game Logic     LED / Display   Input Task
        Task            Task            │
          │             │               │
          └─────────────┼───────────────┘
                        │
                  RTOS Events
                        │
                        ▼
                 Game State Update
```

### RTOS Concepts Implemented

* **Tasks / Threads** — Game functionality is divided into independent execution tasks.
* **Event-Driven Communication** — RTOS events coordinate task execution and game-state changes.
* **Task Scheduling** — FreeRTOS manages concurrent execution instead of a traditional bare-metal superloop.
* **Interrupt Integration** — GPIO EXTI interrupts capture physical button events and pass input into the RTOS-driven application.
* **Timing & Delays** — RTOS timing services are used for game sequencing and progressive difficulty.

## Implementation Highlights

### Random Sequence Generation

```c
uint32_t random32bit;

if (HAL_RNG_GenerateRandomNumber(&hrng, &random32bit) == HAL_OK) {
    sequence[sequence_length - 1] = (uint8_t)(random32bit % 4);
} else {
    sequence[sequence_length - 1] = (uint8_t)(rand() % 4);
}
```

### Interrupt-Driven Button Debouncing

```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    uint32_t current_time = HAL_GetTick();

    if ((current_time - last_interrupt_time) < DEBOUNCE_DELAY_MS)
        return;

    last_interrupt_time = current_time;

    // Signal button event to FreeRTOS
}
```

### Progressive Difficulty

```c
uint32_t flash_delay = 800 - (current_level * 50);

if (flash_delay < 200)
    flash_delay = 200;
```

## Embedded Concepts Demonstrated

`Embedded C` • `FreeRTOS` • `RTOS Tasks/Threads` • `Event Synchronization` • `STM32 HAL` • `GPIO` • `EXTI Interrupts` • `I2C` • `UART` • `Hardware RNG` • `Software Debouncing` • `Concurrent Programming` • `Hardware/Software Integration`




---
