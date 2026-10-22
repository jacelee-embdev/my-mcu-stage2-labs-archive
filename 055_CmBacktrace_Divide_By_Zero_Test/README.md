**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411CEU6 CmBacktrace Divide-by-Zero Test

This project uses FreeRTOS, CmBacktrace, SEGGER RTT, and EasyLogger to demonstrate fault triggering, diagnostic output, and source location. TIM1 provides the HAL time base. Output uses RTT channel 0 with a 2 KB upstream buffer.

## Test Behavior

- `OS_Init()` creates tasks through native FreeRTOS APIs. The status task tests the LED and logging.
- `fault_test_task` runs at priority 1 with a stack depth of 128 elements (512 bytes). It enables `DIV_0_TRP` and executes `10 / 0`.
- CmBacktrace reports the thread name, stack, registers, and divide-by-zero cause. Without recovery from the exception, `Test over.` is not printed.

## Run and Locate

1. Open `MDK-ARM/STM32F411CEU6.uvprojx` in Keil MDK-ARM with ARMCC 5, then build and flash.
2. View channel 0 in J-Link RTT Viewer to capture the fault log.

![RTT divide-by-zero fault log](Z03_Images/divide_by_zero_fault_log.png)

Open PowerShell at the project root and run this example from the screenshots. Add `addr2line` to PATH first:

```powershell
Set-Location .\MDK-ARM\STM32F411CEU6
addr2line -e .\STM32F411CEU6.axf -afpiC 08003f2c 080008c8
```

The results identify `fault_test:76` and its caller, `Fault_Test_TaskEntry:110`. The latter corresponds to the return address.

![addr2line call stack resolution](Z03_Images/addr2line_backtrace.png)

Addresses and line numbers change between builds. Use addresses from the current log and the `.axf` matching the flashed firmware.

## CubeMX Maintenance

After regeneration, preserve the `OS_Init()` call, the `cmb_fault.S` fault entry point, and the FreeRTOS stack-depth adaptations.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
