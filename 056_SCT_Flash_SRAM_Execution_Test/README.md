**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 SCT Scatter-Loading Experiment

This experiment places functions in separate Flash regions and SRAM on an STM32F411CEU6, compares their execution times using identical logic, and verifies the results through the MAP file and on-board logs. The device has 512 KB of Flash and 128 KB of SRAM.

## Implementation

The application code is in `Z02_Business/SCT_Test_Task`. `OS_Init()` in `Z02_Business/Init/os_init.c` creates the task with the native `xTaskCreate()` API. Each test calls two functions in sequence through `volatile` function pointers for ten million iterations. `HAL_GetTick()` measures the elapsed time in milliseconds, `elog_i` logs the result, and the task waits one second after each pair of tests. The status task toggles the LED and logs its status approximately every fifty seconds.

CMSIS V2 initializes and starts the FreeRTOS kernel, while application tasks use native APIs. TIM1 provides the HAL time base, and EasyLogger sends logs through RTT.

## Build and Run

1. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil MDK with ARMCC 5.06 update 7 (build 960).
2. Under Linker, clear `Use Memory Layout from Target Dialog` and set Scatter File to `.\STM32F411CEU6\User_sct\user_sct.sct`, relative to the MDK project directory.
3. Rebuild, flash the firmware, and view channel 0 in J-Link RTT Viewer.

The custom SCT places ordinary code in the region starting at `0x08000000`, `myflash` in the final 64 KB of Flash starting at `0x08070000`, and the execution region for `myram` in SRAM starting at `0x20000000`. The SRAM code is still stored in Flash and copied into SRAM by the runtime during startup.

After regenerating code with CubeMX, retain the custom SCT path, task creation entry, and section attributes, then check the MAP layout again.

## Verification and Results

A full rebuild completes with **0 errors and 0 warnings**. In the newly generated `MDK-ARM/STM32F411CEU6/STM32F411CEU6.map`, check the function execution addresses: Flash functions start with `0x080`, and SRAM functions start with `0x200`. A successful build alone does not prove that scatter-loading is configured correctly. Distinguish load addresses from execution addresses: a Flash load address does not mean the code executes from Flash.

On-board measurements are approximately **2.61 seconds for Flash** and **3.41 seconds for SRAM**. These timings include loop, call, interrupt, and scheduling overhead and apply only to the current test configuration.

![RTT timing test logs](Z03_Images/sct_flash_sram_timing_rtt.png)

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
