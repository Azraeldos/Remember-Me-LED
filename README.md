STM32 Simon Says Memory GameAn embedded memory game implemented in C using the STM32 HAL library. The application demonstrates core embedded systems concepts by generating an increasingly difficult LED sequence that players must replicate using physical push buttons, with real-time feedback delivered via an I2C LCD screen, UART terminal, and audio-visual error indicators.System Architecture & Features4-Channel Game Interface: 4 LEDs and 4 push buttons paired with an I2C LCD screen for real-time status and score tracking.Hardware & Pseudo-Random Generation: Utilizes the STM32 hardware RNG for sequence generation, with a rand() fallback mechanism.Interrupt-Driven I/O: Button inputs are processed using GPIO External Interrupts (EXTI) combined with software non-blocking debouncing.Finite State Machine (FSM): Structured application logic for robust state transitions (Menu, Show Sequence, Player Input, Game Over).Progressive Difficulty: Dynamic LED/LCD timing that speeds up sequence playback as player levels increase.Multi-Channel Feedback: Serial status updates via USART2, physical I2C display updates, and visual/audible error indicators.Hardware ConfigurationComponentInterface / PeripheralPurposeSTM32 MCU—Core MicrocontrollerLEDs (x4)GPIO OutputSequence display channels 1–4Push Buttons (x4)GPIO EXTI (Interrupt)Player inputs for channels 1–4LCD ScreenI2C (I2C1 / I2C2)Real-time level, game state, and score displayError LEDGPIO OutputVisual game-over indicatorBuzzerGPIO OutputAudible game-over feedbackUSART2Serial (115200 8N1)Debugging and terminal status logRNGHardware PeripheralTrue random sequence generationGame State Machine               ┌─────────────┐
               │  STATE_MENU │
               └──────┬──────┘
                      │ Any Button Press
                      ▼
         ┌─────────────────────────┐
         │   STATE_SHOW_SEQUENCE   │◄────────┐
         └────────────┬────────────┘         │
                      │ Sequence Complete    │
                      ▼                      │ Correct Pattern
         ┌─────────────────────────┐         │ (Level Up)
         │   STATE_PLAYER_INPUT    ├─────────┘
         └────────────┬────────────┘
                      │
                      │ Incorrect Input
                      ▼
         ┌─────────────────────────┐
         │     STATE_GAME_OVER     │
         └────────────┬────────────┘
                      │ Any Button Press
                      └──────────────────────► [Return to STATE_MENU]
State BehaviorsSTATE_MENU: Prompts the user via the I2C LCD and UART to press any button to begin.STATE_SHOW_SEQUENCE: Adds a new random channel ($0..3$) to the sequence array and plays back the full pattern via the corresponding LEDs.STATE_PLAYER_INPUT: Waits for EXTI button triggers and compares inputs sequentially against the stored array.STATE_GAME_OVER: Triggers the error sequence (buzzer + flashing LEDs), displays final scores on the LCD/UART, and waits for a button reset.Core Implementation Details1. 4-Channel Random Sequence GenerationThe MCU attempts to poll the hardware RNG and maps the result to one of four game channels:Cuint32_t random32bit;
if (HAL_RNG_GenerateRandomNumber(&hrng, &random32bit) == HAL_OK) {
    sequence[sequence_length - 1] = (uint8_t)(random32bit % 4);
} else {
    sequence[sequence_length - 1] = (uint8_t)(rand() % 4); // Fallback
}
2. EXTI Button DebouncingButton inputs are configured as external interrupt lines. Debouncing is handled in software by checking elapsed system ticks (HAL_GetTick()) inside the callback:Cvoid HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    uint32_t current_time = HAL_GetTick();
    if ((current_time - last_interrupt_time) < DEBOUNCE_DELAY_MS) {
        return; // Ignore bounce
    }
    last_interrupt_time = current_time;
    
    // Process input event flag
}
3. Progressive Difficulty ScalingPlayback delay decreases dynamically per level until reaching a floor limit of 200 ms:Cuint32_t flash_delay = 800 - (current_level * 50);
if (flash_delay < 200) {
    flash_delay = 200;
}
Embedded Concepts DemonstratedGPIO Output & EXTI ConfigurationI2C Protocol Implementation (LCD Driver)Interrupt Handling & Software DebouncingHardware Random Number Generator (RNG)Finite State Machine ArchitectureUSART Serial DebuggingHardware/Software Integration & Memory Management

---
