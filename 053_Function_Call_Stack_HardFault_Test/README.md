**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 Function Call Stack and HardFault Experiments

## Project Description

Two independent STM32F411CEU6 experiments cover function calls, argument
passing, local-variable stack layout, and HardFault inspection after an
invalid memory access.

##### Experiments

| Subproject | Experiment | Usage Guide |
|---|---|---|
| `01_FunctionCalls` | Three nested calls: inspect arguments, local variables, SP, and LR | [English README](01_FunctionCalls/README.md) |
| `02_Hardfaulthandler` | Write to a reserved address: inspect HardFault and fault registers | [English README](02_Hardfaulthandler/README.md) |

##### Repository Structure

```text
01_FunctionCalls/     Function-call and stack-inspection project
02_Hardfaulthandler/  HardFault invalid-write project
LICENSE              Repository license
README_zh.md         Chinese documentation
README.md            English documentation
```

Each subproject has its own `Business`, `Core`, `Drivers`, `MDK-ARM`, and
`Middlewares_User` directories and can be built, flashed, and debugged
independently.

## Usage Guide

##### Environment and Connections

- STM32F411CEU6 development board with a 100 MHz system clock.
- Keil MDK 5 with ARMCC 5.06 update 7; HAL, CMSIS, SEGGER RTT, and EasyLogger are included.
- Connect a debugger through SWD, including target reference voltage and ground.
- Optional UART: PA9 → USB-to-UART RX, PA10 → TX, GND → GND; 3.3 V TTL, 115200 baud, 8N1.

##### Build, Run, and Observe

1. Choose an experiment and open `MDK-ARM/STM32F411CEU6.uvprojx` inside its directory.
2. Select `-O0`, enable debug information, then build and flash the board.
3. Set breakpoints as described in the subproject README, reset, and run. Each experiment executes once at startup.

##### Porting Notes

Experiment logic resides in `Business` and is called by `main` after
initialization. Adapt clock, peripheral, and debugger settings when changing
chips; also recheck the fault address for the HardFault experiment. Keep
application logic inside `USER CODE` regions in CubeMX-generated files.

## Author's Notes

None.

Regenerate the firmware samples and build outputs mentioned here locally, following the corresponding project's instructions.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
