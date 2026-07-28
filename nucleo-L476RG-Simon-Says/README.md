# STM32 Simon Says Memory Game

An embedded **Simon Says memory game** implemented in C using the STM32 HAL library. The game generates an increasingly difficult sequence of LED flashes that the player must reproduce using three physical push buttons.

The project demonstrates embedded-system concepts including **GPIO control, external interrupts, hardware random-number generation, UART communication, button debouncing, and finite-state-machine design**.

---

## Overview

The objective is simple: memorize the LED pattern and repeat it correctly.

At the beginning of each round, the STM32 adds a new randomly selected LED to the existing sequence. The complete sequence is then displayed to the player. The player reproduces the sequence using the corresponding push buttons.

Each successful round increases the level and makes the LED sequence progressively faster. An incorrect button press ends the game and activates the visual and audible error indicators.

---

## Features

* Three-button / three-LED Simon Says interface
* Increasing sequence length after every successful round
* STM32 hardware RNG for sequence generation
* Pseudo-random fallback if hardware RNG generation fails
* Interrupt-driven button detection
* Software button debouncing
* Finite-state-machine game architecture
* Progressive difficulty through faster LED timing
* UART-based game status and debugging output
* Error LED and buzzer feedback
* Game-over score reporting
* Maximum sequence storage of 50 entries

---

## Hardware

The application uses the following peripherals:

| Component    | Purpose                      |
| ------------ | ---------------------------- |
| STM32 MCU    | Main game controller         |
| LED 1        | Sequence indicator 1         |
| LED 2        | Sequence indicator 2         |
| LED 3        | Sequence indicator 3         |
| Button 1     | Player input for LED 1       |
| Button 2     | Player input for LED 2       |
| Button 3     | Player input for LED 3       |
| Error LED    | Visual game-over indication  |
| Buzzer       | Audible game-over indication |
| USART2       | Serial terminal output       |
| Hardware RNG | Random sequence generation   |

The three buttons are configured as GPIO external-interrupt inputs, while the LEDs, error LED, and buzzer operate as GPIO outputs.

---

## Game State Machine

The application is organized around four states:

```text
STATE_MENU
    │
    │ Button Press
    ▼
STATE_SHOW_SEQUENCE
    │
    │ Sequence Displayed
    ▼
STATE_PLAYER_INPUT
    │
    ├── Correct Sequence ──────► STATE_SHOW_SEQUENCE
    │
    └── Incorrect Input ───────► STATE_GAME_OVER
                                      │
                                      │ Button Press
                                      ▼
                                  STATE_MENU
```

### `STATE_MENU`

The system waits for the player to press any button. Once a button is detected, the game variables are initialized and gameplay begins.

### `STATE_SHOW_SEQUENCE`

A new random entry is added to the sequence and the complete pattern is displayed using the LEDs.

### `STATE_PLAYER_INPUT`

The STM32 compares each button press against the expected position in the generated sequence.

Completing the entire sequence correctly advances the player to the next level.

### `STATE_GAME_OVER`

An incorrect input triggers the error LED/buzzer sequence and reports the player's final level over UART. Pressing any button returns the system to the main menu.

---

## Random Sequence Generation

Each new level adds one value to the existing sequence.

The firmware first attempts to generate the value using the STM32 hardware random-number generator:

```c
HAL_RNG_GenerateRandomNumber(&hrng, &random32bit);
```

The generated value is reduced to one of the three available LED/button channels.

```c
sequence[sequence_length - 1] = (uint8_t)(random32bit % 3);
```

If hardware RNG generation fails, the application falls back to `rand()`.

---

## Progressive Difficulty

The game becomes faster as the player advances:

```c
uint32_t flash_delay = 800 - (current_level * 50);

if (flash_delay < 200)
    flash_delay = 200;
```

Each level decreases the LED display time by approximately **50 ms**, with a minimum delay of **200 ms**.

This increases difficulty without allowing the sequence to become impractically fast.

---

## Button Input and Debouncing

Buttons are handled using GPIO external interrupts rather than continuous polling.

The interrupt callback determines which button was pressed and stores the result for processing by the main game loop.

A **150 ms debounce interval** prevents mechanical button bounce from being interpreted as multiple presses.

```c
if ((current_time - last_interrupt_time) < DEBOUNCE_DELAY_MS) {
    return;
}
```

The interrupt handler performs minimal work and uses a flag to communicate the event to the main application.

---

## UART Interface

USART2 is configured for:

```text
Baud Rate:    115200
Data Bits:    8
Stop Bits:    1
Parity:       None
Flow Control: None
```

A serial terminal can therefore be used to follow game progress.

Example output:

```text
=========================
   WELCOME TO SIMON SAYS
=========================
Press ANY button to start playing.

--- LEVEL 1 ---
Watch the LEDs closely...

Your turn! Repeat the pattern...

Correct!

--- LEVEL 2 ---
Watch the LEDs closely...
```

When the player enters an incorrect sequence:

```text
WRONG BUTTON! Game Over.
You reached Level 4.
Press ANY button to return to menu.
```

---

## Error Feedback

An incorrect input activates both the error indicator and buzzer while flashing all three game LEDs.

The error sequence repeats four times:

```text
LEDs ON  + Error LED/Buzzer ON
        ↓
      150 ms
        ↓
LEDs OFF + Error LED/Buzzer OFF
        ↓
      150 ms
```

This provides immediate visual and audible feedback that the game has ended.

---

## Software Structure

The main application is organized around several helper functions:

| Function                   | Description                              |
| -------------------------- | ---------------------------------------- |
| `all_leds_on()`            | Turns on all game LEDs                   |
| `all_leds_off()`           | Turns off all game LEDs                  |
| `turn_led_on()`            | Activates a selected game LED            |
| `error_indicators_on()`    | Activates the error LED and buzzer       |
| `error_indicators_off()`   | Disables the error indicators            |
| `play_error_blink()`       | Runs the game-over indication sequence   |
| `UART_Print()`             | Sends text through USART2                |
| `generate_next_level()`    | Extends the random game sequence         |
| `play_sequence()`          | Displays the current sequence            |
| `HAL_GPIO_EXTI_Callback()` | Handles button interrupts and debouncing |

---

## Building and Running

### Requirements

* STM32 development board with the required GPIO/peripheral support
* STM32CubeIDE
* STM32 HAL drivers
* Three LEDs
* Three push buttons
* Error indicator LED
* Buzzer
* USB/UART serial connection

### Setup

1. Open the STM32 project in **STM32CubeIDE**.
2. Verify the GPIO assignments for the LEDs, buttons, error LED, and buzzer.
3. Verify that **USART2** and the **RNG peripheral** are enabled.
4. Build the project.
5. Flash the firmware to the STM32.
6. Open a serial terminal at **115200 baud**.
7. Press any game button to begin.

---

## Program Flow

```text
Power On
   │
   ▼
Initialize HAL
   │
   ├── Configure System Clock
   ├── Initialize GPIO
   ├── Initialize USART2
   └── Initialize RNG
   │
   ▼
Display Main Menu
   │
   ▼
Wait for Button
   │
   ▼
Generate Sequence
   │
   ▼
Flash LEDs
   │
   ▼
Wait for Player Input
   │
   ├── Correct ──► Next Level
   │
   └── Wrong ────► Error Feedback
                         │
                         ▼
                     Game Over
                         │
                         ▼
                     Main Menu
```

---

## Embedded Concepts Demonstrated

This project provides practical experience with:

* STM32 HAL programming
* GPIO inputs and outputs
* External interrupts (EXTI)
* Interrupt-safe event handling
* Button debouncing
* UART serial communication
* Hardware random-number generation
* Finite-state machines
* Timing with `HAL_Delay()`
* Embedded game logic
* Hardware/software integration

---

## Possible Improvements

Future versions could include:

* Non-blocking timing instead of `HAL_Delay()`
* PWM-generated buzzer tones
* Different tones for each game button
* LCD/OLED level and score display
* High-score storage using Flash or EEPROM
* Adjustable difficulty modes
* Start/restart button
* More LEDs and input buttons
* Timer-based sequence playback
* RTOS-based task organization

---

## License

The STM32-generated portions of the project are subject to the licensing terms included with the STM32 project.

Any additional application code should be distributed according to the license selected for the project.

---

## Author

**Michael Espino**

Embedded Systems Project — STM32 Simon Says Memory Game
