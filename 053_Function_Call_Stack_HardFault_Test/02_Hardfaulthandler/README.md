**English** | [简体中文](README_zh.md)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

# STM32F411 HardFault Invalid-Write Experiment

## Project Description

This STM32F411CEU6 experiment adds `1` and `2`, then writes the result to the
reserved address `0x60000000` to observe a HardFault caused by an invalid access.

The call path is `main → HardFaultDemo_Run → HardFaultDemo_TriggerInvalidWrite`.
Input pointers reference valid variables; the deliberate write to the target
address causes the fault.

##### Project Structure

```text
Business/          HardFault test module
Core/              main entry point, peripheral setup, and exception handlers
Drivers/           HAL and CMSIS
MDK-ARM/           Keil project and startup files
Middlewares_User/  SEGGER RTT and EasyLogger
```

## Usage Guide

##### Environment and Connections

- STM32F411CEU6 development board with a 100 MHz system clock.
- Keil MDK 5 with ARMCC 5.06 update 7; HAL and CMSIS sources are included.
- Connect a debugger through SWD, including target reference voltage and ground.
- Optional UART: PA9 → USB-to-UART RX, PA10 → TX, GND → GND;
  use 3.3 V TTL levels, 115200 baud, 8N1.

##### Build, Run, and Observe

1. Open the [Keil project](MDK-ARM/STM32F411CEU6.uvprojx), select `-O0`, enable debug information, then build, flash, and start debugging.
2. Set breakpoints on `*p_fault_target = sum_value;` and inside `HardFault_Handler()`, then reset and run.
3. Before the write, confirm that `sum_value` is `3` and the target address is `0x60000000`, then continue.
4. In the exception handler, inspect `SCB->HFSR`, `SCB->CFSR`, SP, and LR. Treat `SCB->BFAR` as a valid fault address only when `BFARVALID` is 1.

BusFault is disabled by default, so the access error escalates to HardFault
and the program stops in the existing exception-handler loop. Write buffering
retains its default configuration: fault reporting may be delayed, and the PC
in the exception frame may not identify the write instruction. Reset to repeat
the test.

##### Porting Notes

Add `Business/hardfault_handler_demo.c` to the target project, configure the
header search path, and call `HardFaultDemo_Run()` after initialization.
Recheck the reserved address and fault settings when changing chips. Keep
application logic inside `USER CODE` regions in CubeMX-generated files.

## Author's Notes

##### 1. Halt and Record the Context

Run at full speed without preset breakpoints. After halting and finding the
program in `HardFault_Handler()`, record these three registers:

| Register | Value in This Run | Meaning |
|---|---|---|
| SP | `0x20000E00` | Current stack top; the exception frame starts here in this run |
| LR | `0xFFFFFFF9` | Exception-return marker: context is on MSP in a basic frame without floating-point extension |
| PC | `0x0800146E` | Current position in the HardFault infinite loop |

![HardFault handler and SP after halting](Image/01_hardfault_handler_and_stack_pointer.png)

##### 2. Decode the Exception Frame

Enter `0x20000E00` in Memory. The handler adds no stack entries in this build,
so read eight consecutive 4-byte groups from this address:
`R0, R1, R2, R3, R12, LR, PC, xPSR`.

The saved PC is at `SP + 0x18 = 0x20000E18`. Decode `58 14 00 08` as
`0x08001458` in little-endian order; do not reverse the digits within a byte.

![Basic exception frame and saved PC decoding](Image/02_exception_stack_frame_pc_decoding.png)

The decoded R12 value in the diagram should be `0x00000000`.

##### 3. Inspect the Nearby Source

View `0x08001458` in Disassembly and inspect the nearby source alongside the
assembly. Check CFSR to determine whether the saved PC identifies the faulting
instruction precisely. In this example, inspect the `STRB` at `0x08001466`,
which corresponds to `*p_fault_target = sum_value;`.

![Disassembly and nearby source located using the saved PC](Image/03_stacked_pc_disassembly_and_source.png)

##### 4. Fix and Retest

Correct the suspected code, rebuild, and flash again. Repeat the test under
the same conditions, then assess the fix using both the fault mechanism and
the test results.

## License

This project is licensed under the **MIT License**. Copyright (c) 2026 JesMicro. See the [LICENSE](LICENSE) file in this project's root for the full terms. Third-party components are subject to their respective licenses.
