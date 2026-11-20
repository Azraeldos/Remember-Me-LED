# STM32 Simon Says Memory Game

An embedded **Simon Says memory game** in **C (STM32 HAL + FreeRTOS)** on a **custom STM32L476RG PCB**. The game plays an LED sequence the player must repeat on push buttons, with feedback through an **I2C OLED, UART, addressable RGB LEDs, DAC LED voltmeter, buzzers, and a vibration motor**.

> **Firmware status:** Application code still targets the earlier Nucleo / breadboard prototype (4 GPIO LEDs, 4 buttons, character LCD). Hardware and manufacturing docs below match the **V1 custom PCB** (BOM + gerbers in `manufacturing/`). Firmware will be updated when the assembled board arrives.

## Key Features

* **FreeRTOS-based architecture** with concurrent game, display, and input tasks
* **RTOS event synchronization** between tasks and hardware events
* **GPIO EXTI interrupts** for button input with software debouncing
* **STM32 hardware RNG** for randomized sequences (`rand()` fallback)
* **Progressive difficulty** by shortening LED playback delay as levels increase
* **USART2 (115200 8N1)** for debug / terminal output
* **Custom PCB** with USB-C power, LiPo charge/load sharing, and buck-boost 3.3 V rail

## Hardware (Custom PCB V1)

Source of truth: [`manufacturing/Remember_Me_Led_BOM.xlsx`](manufacturing/Remember_Me_Led_BOM.xlsx), pick-and-place, and gerbers in [`manufacturing/`](manufacturing/).

### Core & UI

| Ref | Qty | Part | Role |
| --- | --- | --- | --- |
| U2 | 1 | STM32L476RGT6 (LQFP-64) | Main MCU |
| LEDS1–LEDS10 | 10 | Inolux IN-PI33TBTPRPGPB | Addressable RGB sequence LEDs |
| BTN1–BTN7 | 7 | Adafruit 3983 (6×6 mm SMD) | Player / UI buttons |
| LCD1 | 1 | EastRising ER-OLEDM0.96-1B-I2C | 0.96″ 128×64 I2C OLED |
| BZ1–BZ2 | 2 | Same Sky CPT-9019S-SMT-TR | Audio feedback |
| J1 | 1 | SparkFun 14313 LED Voltmeter | DAC-driven visual countdown |
| M1 | 1 | Pololu 1636 / 1201 | Shaftless vibration motor |

### Power & charging

| Ref | Qty | Part | Role |
| --- | --- | --- | --- |
| USB1 | 1 | Amphenol GSB1C41110SSHR | USB-C connector |
| B1 | 1 | Microchip MCP73871T-4CCI/ML | LiPo/Li-ion charger + load sharing |
| BTY1 | 1 | TinyCircuits YDL503450 (3.7 V 1 Ah) | Battery (JST) |
| U3 | 1 | TI TPS63020DSJT | Buck-boost → system rail |
| L1 | 1 | Würth 744874001 (1.5 µH) | Buck-boost inductor |
| NTC1 | 1 | Vishay NTCS0805E3103JMT (10 k) | Charge thermistor |
| P1 | 1 | C&K JS202011SCQN | DPDT slide switch |
| U1 | 1 | AO3400A | N-channel MOSFET (motor / load drive) |
| D1 | 1 | 1N4148W-7-F | Signal diode |

### Debug & interconnect

| Ref | Qty | Part | Role |
| --- | --- | --- | --- |
| J3 | 1 | Samtec FTSH-107-01-L-DV-K-P | SWD / programming header |



### Manufacturing package

| File | Purpose |
| ---- | ------- |
| `manufacturing/Remember_Me_Led_BOM.xlsx` | Assembly BOM (PCBWay format) |
| `manufacturing/RememberMeLED-all-pos.csv` | Pick-and-place |
| `manufacturing/*.gbr`, `*.drl` | Gerbers + drill |
| `manufacturing/.step` | 3D STEP export (2026-11-02) |

## Firmware vs board (until PCB bring-up)

| Capability | Custom PCB (BOM) | Current firmware |
| ---------- | ---------------- | ---------------- |
| LEDs | 10× addressable RGB (IN-PI33…) | 4× GPIO LEDs |
| Buttons | 7× SMD | 4× EXTI buttons |
| Display | 0.96″ I2C OLED | Character I2C LCD driver |
| Haptics | Vibration motor + MOSFET | Not driven |
| Power | USB-C + MCP73871 + TPS63020 | Nucleo / external 3.3 V |
| Target | Custom board (U2) | Nucleo-L476RG (`.ioc`) |

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

<img width="1385" height="973" alt="Screenshot 2026-11-19 085514" src="https://github.com/user-attachments/assets/0af92864-4b74-4fcf-9a77-490927b5a697" />


*Prototype — custom PCB is in fab.*

### RTOS Concepts Implemented

* **Tasks / Threads** — Game functionality is divided into independent execution tasks.
* **Event-Driven Communication** — RTOS events coordinate task execution and game-state changes.
* **Task Scheduling** — FreeRTOS manages concurrent execution instead of a traditional bare-metal superloop.
* **Interrupt Integration** — GPIO EXTI interrupts capture physical button events and pass input into the RTOS-driven application.
* **Timing & Delays** — RTOS timing services are used for game sequencing and progressive difficulty.



## Embedded Concepts Demonstrated

`Embedded C` • `FreeRTOS` • `RTOS Tasks/Threads` • `Event Synchronization` • `STM32 HAL` • `GPIO` • `EXTI Interrupts` • `I2C` • `UART` • `Hardware RNG` • `Software Debouncing` • `Concurrent Programming` • `Hardware/Software Integration` • `Custom PCB` • ` Power Path`
