**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# My MCU Stage 2 Labs Archive

An archive of projects from the second stage of my embedded systems learning, covering FreeRTOS, DMA, bootloaders, OTA updates, embedded debugging, and more.

## About This Archive

This repository preserves the projects and notes accumulated during the second stage of my embedded systems learning. It serves as a personal learning archive and code backup for revisiting experiments and reproducing their results.

The learning topics include task scheduling and synchronization, data transfer and buffering, firmware upgrades, encryption, fault diagnosis, linker scripts, and more. Projects are numbered in learning order and retain the implementation and experiment state from that point in the learning process.

Each lab is opened and built separately. Some labs contain related firmware projects, such as a bootloader and its applications. Refer to the lab's documentation for its hardware, toolchain, build sequence, and expected behavior.

## Project Index

Select a project name to open its English documentation.

| No. | Project | Description |
|---:|---|---|
| 001 | [Button_LED_CubeMX_V1](001_Button_LED_CubeMX_V1/README.md) | Poll a button to toggle an LED in a CubeMX project, with periodic UART test output. |
| 002 | [Button_LED_Template_V2](002_Button_LED_Template_V2/README.md) | Poll a button to toggle an LED using a custom HAL template with separate BSP and system drivers. |
| 003 | [FreeRTOS_UART_Printf](003_FreeRTOS_UART_Printf/README.md) | Poll a button, toggle an LED, and print UART messages from a FreeRTOS thread. |
| 004 | [FreeRTOS_Printf_Thread_Safety](004_FreeRTOS_Printf_Thread_Safety/README.md) | Explore concurrent printf calls and the limits of character-level scheduler protection. |
| 005 | [Keil_Debug_Basics](005_Keil_Debug_Basics/README.md) | Practice Keil breakpoints, stepping, and inspection with periodic UART output. |
| 006 | [FreeRTOS_Queue_LED_Control](006_FreeRTOS_Queue_LED_Control/README.md) | Decouple button input and LED control through queues and a relay thread. |
| 007 | [FreeRTOS_Button_Short_Long_Press](007_FreeRTOS_Button_Short_Long_Press/README.md) | Handle short and long button presses through FreeRTOS threads and queues. |
| 008 | [FreeRTOS_Software_Interrupt](008_FreeRTOS_Software_Interrupt/README.md) | Pend a software interrupt through NVIC and report the result from thread context. |
| 009 | [FreeRTOS_Button_EXTI_V1](009_FreeRTOS_Button_EXTI_V1/README.md) | Classify short and long presses using EXTI edge events and a thread state machine. |
| 010 | [FreeRTOS_Button_EXTI_V2](010_FreeRTOS_Button_EXTI_V2/README.md) | Control an LED with independent EXTI state machines for two buttons. |
| 011 | [FreeRTOS_Button_Timer_LED_Control](011_FreeRTOS_Button_Timer_LED_Control/README.md) | Map button events to LED commands and drive toggling with TIM2 interrupts. |
| 012 | [FreeRTOS_Interrupt_Response_Comparison](012_FreeRTOS_Interrupt_Response_Comparison/README.md) | Compare value and pointer queue transfers in an interrupt response path. |
| 013 | [SEGGER_RTT_Demo](013_SEGGER_RTT_Demo/README.md) | Integrate SEGGER RTT for direct RTT and standard printf debug output. |
| 014 | [EasyLogger_UART_Port](014_EasyLogger_UART_Port/README.md) | Port EasyLogger for leveled, colored UART logging. |
| 015 | [RTT_EasyLogger_Template_V1](015_RTT_EasyLogger_Template_V1/README.md) | Combine RTT and EasyLogger in a bare-metal logging template. |
| 016 | [FreeRTOS_RTT_EasyLogger_Template_V2](016_FreeRTOS_RTT_EasyLogger_Template_V2/README.md) | Integrate FreeRTOS, RTT, and EasyLogger with a common thread initialization entry. |
| 017 | [UART_Interrupt_DMA_RX_Comparison](017_UART_Interrupt_DMA_RX_Comparison/README.md) | Compare CPU overhead and workload timing for interrupt-driven and DMA UART reception. |
| 018 | [DMA_Memory_To_GPIO](018_DMA_Memory_To_GPIO/README.md) | Transfer memory data to GPIO with DMA and inspect the resulting waveform. |
| 019 | [UART_DMA_RX_To_SRAM](019_UART_DMA_RX_To_SRAM/README.md) | Receive UART data into SRAM through DMA and restart reception from its callback. |
| 020 | [UART_DMA_FIFO_Burst](020_UART_DMA_FIFO_Burst/README.md) | Explore FIFO, burst, and buffer behavior in UART DMA reception. |
| 021 | [PWM_DMA_WT588F02B_Control](021_PWM_DMA_WT588F02B_Control/README.md) | Generate WT588F02B voice-module control waveforms using PWM and DMA. |
| 022 | [FreeRTOS_ADC_DMA_Double_Buffer](022_FreeRTOS_ADC_DMA_Double_Buffer/README.md) | Acquire and process ADC data with DMA double buffering under FreeRTOS. |
| 023 | [FreeRTOS_ADC_DMA_Double_Buffer_Mutex](023_FreeRTOS_ADC_DMA_Double_Buffer_Mutex/README.md) | Coordinate ADC DMA double-buffer access with a mutex. |
| 024 | [UART_Interrupt_Double_Buffer_Parser](024_UART_Interrupt_Double_Buffer_Parser/README.md) | Receive fixed-length UART data with interrupts and double buffers for frame parsing. |
| 025 | [UART_DMA_Fixed_Length_Double_Buffer_Parser](025_UART_DMA_Fixed_Length_Double_Buffer_Parser/README.md) | Parse frames from fixed-length UART DMA reception using double buffers. |
| 026 | [UART_DMA_HT_TC_Idle_Test](026_UART_DMA_HT_TC_Idle_Test/README.md) | Test UART DMA half-transfer, transfer-complete, and idle events. |
| 027 | [UART_DMA_Idle_Frame_Parser](027_UART_DMA_Idle_Frame_Parser/README.md) | Parse variable-length frames using circular UART DMA reception and idle detection. |
| 028 | [HAL_Bootloader_APP_Jump](028_HAL_Bootloader_APP_Jump/README.md) | Verify application jumps from a HAL-based bootloader. |
| 029 | [SPL_RTT_EasyLogger_Template](029_SPL_RTT_EasyLogger_Template/README.md) | Build a base project using SPL, RTT, and EasyLogger. |
| 030 | [SPL_Bootloader_APP_Jump](030_SPL_Bootloader_APP_Jump/README.md) | Verify basic jumps from an SPL bootloader to a standalone application. |
| 031 | [Bootloader_Peripheral_Drivers](031_Bootloader_Peripheral_Drivers/README.md) | Integrate UART, button, and status drivers for a bootloader. |
| 032 | [Bootloader_YMODEM_Test](032_Bootloader_YMODEM_Test/README.md) | Test YMODEM file reception in a bootloader. |
| 033 | [Bootloader_YMODEM_APP_Install](033_Bootloader_YMODEM_APP_Install/README.md) | Receive an application over YMODEM, install it in internal Flash, and boot it. |
| 034 | [Bootloader_Internal_Flash_Staging](034_Bootloader_Internal_Flash_Staging/README.md) | Stage new firmware in internal Flash, copy it to the execution partition, and boot it. |
| 035 | [AES_Encryption_Decryption_Test](035_AES_Encryption_Decryption_Test/README.md) | Verify AES encryption, decryption, and round-trip data consistency. |
| 036 | [Bootloader_AES_Firmware_Upgrade](036_Bootloader_AES_Firmware_Upgrade/README.md) | Receive, decrypt, and install encrypted firmware in a bootloader. |

## Usage

1. Clone or download this repository, then select a lab from the project index.
2. Read the lab's `README.md` and check its MCU, hardware connections, required components, and development environment.
3. Open the corresponding `.uvprojx` file in Keil MDK and install the compiler and device packs required by that project.
4. Rebuild the project to generate its firmware. If the lab contains related firmware projects, follow the documented build and flashing sequence.
5. Flash the firmware with the configured debugger, then check the serial output, LEDs, or other documented results.

Reproducible build outputs are excluded from version control. When using a lab independently, keep its source files, project configuration, `LICENSE`, and `.gitignore` together. Hardware and toolchain details may vary between labs; use the corresponding documentation and source as the reference.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in the repository root for the full terms. Third-party components are subject to their respective licenses.
