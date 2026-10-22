**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 Fault Tests

This project uses FreeRTOS, SEGGER RTT, and EasyLogger to reproduce three fault scenarios for debugging practice with Keil/Ozone. CMSIS V2 starts the kernel; TIM1 provides the HAL time base.

![Ozone HardFault debugging interface](Z03_Images/PixPin_2026-10-20_18-07-24.png)

## Tasks and Cases

`OS_Init()` creates the application tasks. `Status_Task` toggles the LED and logs every 10 seconds. `Fault_Test_Task` logs activity before selecting a test, then delays 1 second after each normal loop.

| Value | Enumeration | Operation |
| --- | --- | --- |
| 0 | `FAULT_TEST_NONE` | Do not trigger a fault |
| 1 | `FAULT_TEST_INVALID_WRITE` | Write 8-bit data to `0x60000000` |
| 2 | `FAULT_TEST_DIVIDE_BY_ZERO` | Enable the divide-by-zero trap and execute `10 / 0` |
| 3 | `FAULT_TEST_INVALID_READ` | Read 32-bit data from `0xFFFFFFFF` |

The selector, `g_fault_test_case`, is a module-private `static volatile` variable in `Z02_Business/Fault_Test_Task/fault_test_task.c`. Its current initializer selects the invalid write. Select `FAULT_TEST_NONE` to observe periodic logs. Change the initializer and rebuild, or modify the variable in the debugger.

## Usage and Validation

Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil MDK-ARM with ARMCC 5. Build and flash, then view channel 0 in J-Link RTT Viewer. Set a breakpoint in `HardFault_Handler()`, select one case per run, and reset after a fault before testing another case.

All three cases have been verified on hardware to enter HardFault. The invalid write originates as BusFault; division by zero originates as UsageFault. Inspect `HFSR` and `CFSR` for the cause. The invalid read also involves misalignment and an address-space boundary crossing, so use the fault registers to identify its cause.

ARMCC V5.06 update 7 (build 960) reports **0 errors and 0 warnings**. After CubeMX regeneration, check the `OS_Init()` call, disabled default task, TIM1 time base, logging initialization, RTT redirection, and application file references in Keil.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
