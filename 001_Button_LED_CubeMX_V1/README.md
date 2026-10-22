**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Button LED Control (CubeMX V1)

This project implements the first approach to the first assignment. Built from an STM32CubeMX-generated HAL project, it introduces GPIO input/output, UART printing, and polling in the main loop. HSI and the PLL configure the system clock to 100 MHz. The CubeMX configuration is retained for comparing graphical settings with the initialization code.

## Experiment

PA0 is a pull-up input, with the button connecting it to ground when pressed. PC13 is a push-pull output connected to the LED. The main loop toggles the LED whenever the button reads low, then delays for approximately one second. Holding the button therefore toggles the LED repeatedly. A short press during the delay may be missed; separate debounce logic and single-press event detection are not implemented.

USART1 uses PA9/TX and PA10/RX at 115200 baud, 8N1. Each loop prints `TEST 1142!` through the retargeted `printf`, providing a periodic indication that the program is running.

## Build and Run

1. Open [STM32F411CEU6.uvprojx](MDK-ARM/STM32F411CEU6.uvprojx) in Keil MDK. The project is configured for ARMCC 5.06 update 7 and STM32F4xx_DFP 2.17.1.
2. Rebuild the project and download it through SWD. Observe the LED transitions according to its wiring, and check the periodic output in a serial terminal. Connect the serial adapter's RX to PA9 and share ground with the board. The LED's on/off level depends on its wiring.
3. The main logic is in `Core/Src/main.c`; GPIO and UART initialization are in `gpio.c` and `usart.c`. When regenerating CubeMX code, retain the button logic and print retargeting in the user code sections.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
