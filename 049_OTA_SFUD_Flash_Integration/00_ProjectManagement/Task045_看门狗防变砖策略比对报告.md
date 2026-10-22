# Task045 看门狗防变砖策略比对报告

审查日期：2026-10-07。结论：**当前工程没有完整实现教师版的首次启动看门狗检测流程；教师版自身也没有完成“检测失败后自动恢复旧 APP”的闭环。**

本次只审查代码并编写报告，没有修改固件源代码、工程配置或烧录设备。

## 1. 比对范围与版本口径

| 项目 | 目录 |
|---|---|
| 当前 Bootloader | `03_Firmware/01_Bootloader` |
| 当前 APP | `03_Firmware/02_APP1` |
| 教师 Bootloader | `C:/Users/Administrator/Desktop/教师版/30_Watchdog_Detection_Step6_Boot/Bootloader` |
| 教师 APP | `C:/Users/Administrator/Desktop/教师版/30_Watchdog_Detection_Step6_App/04_Software` |

当前 Git 只有基线提交 `e9aab1e`，提交说明为 `chore:establish initial project baseline`。本次新增逻辑位于未提交的工作区修改中，不能称为已经提交的最终版本。新增 `iwdg.c`、`iwdg.h` 仍为未跟踪文件。

教师版的状态机位于 `Tasks/Boot_Manager/Boot_Manager.c`，对应当前工程的 `Tasks/Bootloader/bootloader.c`。APP2 不在本次审查范围内；本文关于被安装 APP 启动行为的推导，以它包含当前 APP1 的启动代码为条件。

“窗口看门狗”需要澄清：两份工程实际调用的是 **IWDG 独立看门狗**，没有启用 WWDG 窗口看门狗业务。这不是当前工程遗漏 WWDG，而是教师版原本就采用 IWDG 给首次启动设置超时时间。

## 2. 教师版实际做了什么

EEPROM 地址 `0x00` 保存 OTA 状态：

| 状态 | 教师版实际行为 |
|---|---|
| `0x00` | 正常启动；按键下载后安装新 APP；安装路径写 `0x33` 并软件复位 |
| `0x11` | 下载异常；先尝试启动当前 APP，无法跳转时重新下载；成功安装后写 `0x33` 并复位 |
| `0x22` | 使用 APP 已下载的包；解密、备份、安装后写 `0x33` 并复位 |
| `0x33` | 写 `0x44`，开启 IWDG，然后跳转已经安装的新 APP |
| `0x44` | Bootloader 再次读到该状态，视为首次启动没有完成确认；写 `0x00` 并软件复位 |

教师 APP 在 GPIO、DMA、USART1、SPI1 初始化之后读取状态：读到 `0x44` 就写 `0x00`，随后停在死循环中，等待 IWDG 复位；读到其他非零状态就软件复位；读到 `0x00` 才继续启动调度器。

因此，教师版的成功链路是：

```mermaid
flowchart LR
    A[新 APP 安装完成] --> B[写 0x33 并软件复位]
    B --> C[Boot 写 0x44 并开启 IWDG]
    C --> D[跳转新 APP]
    D --> E[APP 写 0x00 并等待看门狗复位]
    E --> F[Boot 按 0x00 正常启动 APP]
```

最后一次正常启动时，教师 Bootloader 没有再次开启 IWDG，所以 APP 不需要持续喂狗。教师版 `main.c:71` 的全局看门狗初始化是注释状态；实际开启位置是 `Boot_Manager.c:207`。

证据：[教师状态机][tb-state] 的安装路径在 109、154、186 行设置 `0x33`；203 行开始处理首次启动；[教师 APP 启动][ta-main] 在 104 行开始确认 `0x44`。

## 3. 相对教师版的遗漏与偏差

### B1：安装成功后没有提交 `0x33` 并复位（P1）

教师版的各个成功安装路径均执行“写 `APP_FIRST_CHECK_START` → 软件复位”。当前工程的公共安装函数在安装成功、保存新 APP 长度后，直接调用 `bootloader_jump_to_app()`，没有设置 `APP_FIRST_CHECK`，也没有在此处复位。

当前工程虽然定义了 `APP_FIRST_CHECK = 0x33`，也增加了对应 `case`，但检索 Bootloader 与 APP1 的业务源码，没有任何有效代码将 EEPROM 状态设置成 `0x33`。正常升级路径无法进入这个新状态。

影响：

- 首次从 Bootloader 下载且状态原为 `0x00` 时，新 APP 直接以正常状态启动，跳过教师版的 `0x33 → 0x44` 试运行握手。
- 从 APP 发起 OTA 时，状态原为 `0x22`。安装后状态仍为 `0x22`；如果新镜像使用本次 APP1 的启动逻辑，它会在 `main()` 中立即软件复位，无法启动到 OTA 任务。
- 基线中 OTA 任务启动时会清状态，但新增的 `main()` 非零状态检查发生在调度器启动之前，原来的清状态代码已不能为这条路径收尾。

证据：[当前安装成功路径][b-install]，尤其第 487～491 行；[当前 APP 非零状态处理][a-main]，第 108～111 行；[OTA 任务启动清状态][a-task-clear]。

整改方向：在公共安装成功路径中接入“可靠保存 `0x33` → 系统复位”，覆盖直接下载、`0x11` 恢复下载和 `0x22` 已下载包安装，不要只补某一个 `case`。

### B2：`0x33` 分支调用了重新下载，而非跳转新 APP（P1）

当前 `case APP_FIRST_CHECK` 写 `0x44` 并启动 IWDG 后调用的是 `bootloader_download_app()`。这个函数会进入 `Ymodem_Receive()`，接收新的加密包并重新执行安装；教师版在相同位置调用 `JumpToApp()`。

影响：即使手工设置 `0x33`，也不会试运行已经安装的镜像，而会等待串口下载；该下载过程没有周期喂狗，还会开始覆盖 A 区。若 A 区保存旧 APP 备份，这条错误路径可能破坏备份。

证据：[当前 `0x33` 分支][b-first] 第 640 行；[下载函数][b-download]；[YMODEM 开始新文件][b-ymodem] 第 265 行重置 A 区写入管理状态；教师版 `Boot_Manager.c:209` 调用 `JumpToApp()`。

整改方向：首次检测分支应跳转已经安装的新 APP；无效向量表或跳转返回要进入明确的失败处理路径。

### B3：每次启动都开启 IWDG，改变了教师版的生命周期（P1）

当前 Bootloader 在 `main.c:57` 无条件调用 `IWDG_Init(IWDG_Prescaler_64, 3000)`，教师版只在 `0x33` 首次检测阶段开启。

当前唯一的周期喂狗在 Bootloader 的按键等待循环 `main.c:78`。启动时的 10 次 500 ms 延时、YMODEM 接收、解密、备份、安装流程及 APP1 正常业务中，都没有周期喂狗。

IWDG 开启后会持续运行；跳转 APP 和 `RCC_DeInit()` 不等于系统复位，不能结束这次看门狗计时。按 LSI 32 kHz 估算，64 分频、重装载 3000 对应约 6 秒超时，实际时间随 LSI 频率变化。启动日志延时本身已经消耗约 5 秒。上述硬件行为可核对 [ST RM0383 第 15 章](https://www.st.com/resource/zh/reference_manual/rm0383-stm32f411xce-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)。

影响：

- `0x00` 正常启动 APP 后，只要没有其他硬件调试冻结等因素，APP 不持续喂狗就会再次复位；后续启动又重新开启 IWDG，可能形成重复重启。
- 等待用户发送文件、传输或安装耗时超过剩余计时时间时，正常升级也可能被看门狗打断。
- 即使 APP 正确完成 `0x44 → 0x00` 并等待复位，下一次 Bootloader 仍会开启 IWDG，无法得到教师版那样的正常运行阶段。

证据：[当前 Bootloader 入口][b-main] 第 57、60、78 行；[当前跳转实现][b-jump] 第 119 行；本地 ST 驱动 `stm32f4xx_iwdg.c:18～28` 也说明启动后持续计时且需周期重装载。

整改方向：若目标是跟随教师版，移除正常启动阶段的无条件开启，只在试运行阶段开启；若另外要做常驻看门狗，则需要独立设计各阶段的喂狗和故障处理，不能只在 Bootloader 等待按键时喂狗。

### B4：APP 确认时机比教师版更早（P2）

当前 APP1 对 `0x44` 的清零与等待复位逻辑，在内容上与教师版一致；`SoftReset()` 的实现已经存在，本次也增加了头文件声明。

差异是当前确认代码位于 `USER CODE BEGIN SysInit`，在 `MX_GPIO_Init()`、`MX_DMA_Init()`、`MX_USART1_UART_Init()`、`MX_SPI1_Init()` 之前；教师版放在这些初始化之后的 `USER CODE BEGIN 2`。

影响：当前即使写入 `0x00` 成功，也不能证明这些外设初始化已经执行成功。软件 I2C 会自行配置 EEPROM 引脚，因此不能简单认定“放在 GPIO 初始化之前就一定不能访问 EEPROM”；这里的问题是确认范围更窄。

证据：[当前 APP 启动][a-main] 第 95～121 行；教师版 `main.c:93～109`。

整改方向：至少与教师版对齐到必要外设初始化之后；若需要证明完整业务健康，则进一步制定调度器和关键任务的成功判据。

## 4. 已经跟上的内容

| 内容 | 核对结果 |
|---|---|
| `0x33`、`0x44` 常量 | 值与教师版一致；`APP_FIRST_CHECK` 与教师 `APP_FIRST_CHECK_START` 的命名差异不影响业务 |
| `IWDG_Init()` | 写访问使能、分频、重装载、重装载计数器、使能顺序一致；`u8/u16` 与 `uint8_t/uint16_t` 类型等价 |
| Bootloader 工程集成 | `main.h` 包含 `iwdg.h`；Keil 工程包含 `Core/Src/iwdg.c` 与 `stm32f4xx_iwdg.c`；标准库配置包含 IWDG 头文件 |
| APP1 对 `0x44` 的确认 | 清零并死循环等 IWDG 复位，内容与教师版一致，但确认位置更早 |
| APP1 其他非零状态处理 | 调用 `SoftReset()`，与教师版一致 |
| Bootloader 对残留 `0x44` 的处理 | 清零并软件复位，与教师版有效代码一致；但两边都没有在这里执行回滚 |
| 原有接收、解密、备份、安装能力 | 当前基线已经具有长度、向量表、写入回读检查和同一次执行内的失败恢复；这些能力不能替代本轮首次启动检测 |

APP1 的 OTA 完成状态 `0x22`、包长度保存和更新命令后的软件复位也已存在。当前采用“先确认长度，再提交 `0x22`”的顺序，不需要为了逐行相同改成教师版先写状态的顺序。

## 5. 教师版自身与当前工程共有的防变砖缺口

以下内容不是“漏抄教师版已实现的功能”，但决定了是否真的具备自动恢复能力。

### C1：检测失败后没有恢复旧 APP

教师版 `case APP_FIRST_CHECKING` 只写 `0x00` 并软件复位；`ExA_To_App()` 和再次跳转的语句在第 219、221 行被注释，而且放在不返回的软件复位之后。不能把这些注释当作已经实现的回滚。

当前 `0x44` 分支也是清零并复位，没有调用 `exti_A_to_internal_APP()`。如果新 APP 向量表合法、运行时却卡死在确认之前，看门狗复位后仍会保留同一份坏镜像。教师版下一次按 `0x00` 无看门狗启动它，仍可能卡死；当前版则可能因无条件开启看门狗反复重启。两边都没有自动返回旧固件。

当前确实有旧版本恢复函数，并且在 `bootloader_install_app()` 中处理安装报错或 APP 跳转调用返回。但 CPU 卡死、HardFault 后被看门狗复位，不会返回到原调用栈，这段恢复代码无法接续执行。

证据：[教师状态机][tb-state] 第 212～221 行；[当前残留 `0x44` 处理][b-failure]；[当前同次执行回滚][b-rollback]。

### C2：跨复位恢复所需的备份元数据没有完整保存

当前 `backup_valid`、旧 APP 的 `app_size` 都是安装函数里的局部变量，复位后丢失。安装新 APP 又将 EEPROM `0x05～0x08` 更新为新 APP 长度，不能再把该字段直接视为旧备份长度。

每次 `W25Q64_Init()` 都会清空 RAM 中 A/B 区的管理长度。外部 Flash 中备份内容还可能存在，但恢复函数依赖 `Read_BlockSize(BLOCK_1)`，没有恢复正确长度就会拒绝恢复。

真正补全跨复位回滚时，需要独立持久化旧备份有效标志、旧镜像长度及必要校验信息；在失败启动分支中恢复 A 区管理参数，确认备份有效，再恢复内部 APP 和旧长度记录。不能只把一个恢复函数调用加进 `0x44` 分支就认为完成。

证据：[当前安装函数][b-install] 第 453～454 行；[保存 APP 长度][b-size]；[外部分区初始化][b-flash-init]；[恢复长度检查][b-copy]。

### C3：启动确认只证明执行到了早期代码

两边都在调度器启动前清除 `0x44`，没有实际的外设返回状态汇总、关键任务健康检查或稳定运行观察期。即使严格照抄教师版，也只能称为早期启动确认，不能保证后续 FreeRTOS 任务和 OTA 业务正常。

### C4：新增状态写入缺少可靠性确认

当前新增的 `0x44` 写入、`0x00` 清零以及 APP 启动状态读取，都没有处理读写返回值，也没有等待写完成后回读确认；教师版这些新增位置同样没有完整确认。

底层 `ee_write_bytes()` 返回成功表示数据已经发送，最后一页内部写周期可能仍在进行。当前原有 `bootloader_save_app_size()` 和 `swc_ota_save_state()` 已经有等待、回读校验，可作为补全思路；启动阶段不能直接调用包含 `vTaskDelay()` 的任务阶段帮助函数。

影响是写入失败可能使系统仍跳转、等待复位或进入错误状态，而无法依据 EEPROM 作出可靠的启动判断。

### C5：`0x11` 失败下载后尝试启动旧 APP，会与 APP 的非零复位判断冲突

两份 Bootloader 在 `0x11` 分支中都先跳转当前 APP，没有在跳转前将失败下载状态妥善收尾。如果该 APP 采用对应版本中“除 `0x44` 外，其他非零状态都软件复位”的启动代码，APP 会因读到 `0x11` 再次复位；Boot 又读到 `0x11` 并跳转，形成循环。

当前基线原先没有 APP `main()` 中的非零状态检查，能够到达 OTA 任务清状态；本轮加上检查后，该恢复路径也需要一起调整。教师版同样存在这一冲突。修复时应区分“丢弃未完成下载、允许旧 APP 正常运行”和“新 APP 首次检测失败、需要恢复备份”两类业务。

证据：当前 `bootloader.c:606～610`、`APP1/main.c:108～111`；教师 `Boot_Manager.c:137～143` 与教师 `APP/main.c:111～114`。

## 6. 当前代码的典型执行结果

| 场景 | 静态推导结果 |
|---|---|
| `0x00` 正常开机 | Boot 无条件开 IWDG，约 5 秒后尝试启动 APP；APP 不持续喂狗，存在重复重启问题 |
| Boot 首次下载安装 APP1 | 安装后直接跳转，未进入 `0x33/0x44` 自检；传输过程还可能被提前开启的 IWDG 打断 |
| `0x22` 安装具有本次 APP1 启动逻辑的新镜像 | 状态仍为 `0x22`，APP 立即软件复位，不能到达 OTA 任务的清状态语句 |
| 上述 `0x22` 安装时已将旧 APP 备份覆盖 A 区 | 下次开机仍把 A 区当作待解密包；A 区实际已是旧 APP 明文，长度/解密检查可能失败，回到新 APP 后又因 `0x22` 复位 |
| 人工写 `0x33` 再开机 | 写 `0x44` 后进入重新下载，而不是启动已安装镜像 |
| Boot 读到 `0x44` | 清零并复位，不恢复旧 APP |
| `0x11`，且当前有效 APP 采用本次启动状态检查 | Boot 跳转 APP，APP 因 `0x11` 软件复位，之后重复该路径 |

这些是对源码控制流的推导，不是已完成的板上测试结果。

## 7. 建议整改与验收顺序

1. 先修复 B1、B2、B3，使教师版成功路径完整接通：安装完成提交 `0x33` 并复位；`0x33` 写 `0x44` 后开启 IWDG 并跳转；`0x00` 正常启动不再次开启本次试运行看门狗。
2. 调整 APP1 的确认时机，补充启动阶段 EEPROM 读写失败处理、写完成等待和回读验证。
3. 为 `0x44` 失败路径设计真正的跨复位恢复流程，持久化旧备份元数据；恢复失败或没有有效备份时进入可下载的恢复模式。不要先清成正常状态再启动同一份失败镜像。
4. 将新增 `iwdg.c/.h` 纳入后续提交，保证提交后其他环境能完整构建。

板上验收建议：

| 用例 | 预期结果 |
|---|---|
| EEPROM `0x00`，正常启动 APP1 | 连续运行超过多个看门狗超时周期，不重复重启 |
| Boot 直接下载并首次安装 | 可观察到 `0x33 → 0x44 → 0x00`，复位后正常运行 |
| APP OTA 下载完成后复位安装 | 不残留 `0x22`，完成首次确认并正常运行 |
| `0x33` 试运行入口 | 不要求重新传文件，直接启动已安装的 APP |
| 新 APP 在清除 `0x44` 前故意卡死或进入 HardFault | IWDG 复位，Boot 检测残留 `0x44`，恢复有效旧镜像并运行 |
| 首次安装没有旧备份且新 APP 失败 | 留在可重新下载的恢复模式，不伪装成正常启动成功 |
| EEPROM 写状态失败 | 不继续提交成功或盲目等待复位，进入明确故障处理 |
| APP 下载失败后留下 `0x11` 并复位 | 旧 APP 能恢复正常运行，不能因非零状态判断循环复位 |
| 下载、备份、安装期间中断或断电 | 单独验证恢复路径；不能用首次启动看门狗测试替代安装断电恢复测试 |

本次完成了源码、工程文件、Git 基线差异及状态写入/调用点审查，没有重新编译或运行硬件测试。已有 Bootloader 构建日志记录 `0 Error(s), 0 Warning(s)`，但不代表业务已通过验证；APP1 现有构建日志指向旧 `task044_PerfOpt_Integrity` 目录，不能用作本轮 APP 修改的编译通过证据。

## 8. 可用于汇报的结论

本轮已加入 IWDG 驱动、`0x33/0x44` 状态定义以及 APP1 早期启动确认代码，但升级成功后进入自检的状态转换没有接通，`0x33` 分支误调用下载函数，且看门狗被扩大为每次启动都开启。因此当前版本尚未完整复现教师版的首次启动检测流程。教师版提供的参考代码本身也没有启用检测失败后的旧版本恢复；当前已有回滚仅能处理同一次执行中的安装错误或跳转返回，不能处理看门狗复位后的恢复。修复上述控制流并补全跨复位备份元数据及失败恢复、完成板上故障注入验收后，才能认定 OTA 防变砖策略实现完整。

[tb-state]: C:/Users/Administrator/Desktop/教师版/30_Watchdog_Detection_Step6_Boot/Bootloader/Tasks/Boot_Manager/Boot_Manager.c:93
[ta-main]: C:/Users/Administrator/Desktop/教师版/30_Watchdog_Detection_Step6_App/04_Software/Core/Src/main.c:93
[b-install]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/01_Bootloader/Tasks/Bootloader/bootloader.c:451
[b-first]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/01_Bootloader/Tasks/Bootloader/bootloader.c:635
[b-download]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/01_Bootloader/Tasks/Bootloader/bootloader.c:559
[b-ymodem]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/01_Bootloader/Middlewares_User/Ymodem/ymodem.c:265
[b-main]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/01_Bootloader/Core/Src/main.c:57
[b-jump]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/01_Bootloader/Tasks/Bootloader/bootloader.c:99
[b-failure]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/01_Bootloader/Tasks/Bootloader/bootloader.c:643
[b-rollback]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/01_Bootloader/Tasks/Bootloader/bootloader.c:505
[b-size]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/01_Bootloader/Tasks/Bootloader/bootloader.c:421
[b-flash-init]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/01_Bootloader/BSP/W25QXX/w25qxx_Handler.c:86
[b-copy]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/01_Bootloader/Tasks/Bootloader/bootloader.c:308
[a-main]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/02_APP1/Core/Src/main.c:95
[a-task-clear]: C:/Users/Administrator/Desktop/task045_PerfOpt_AntiBrick/03_Firmware/02_APP1/Tasks/SWC_OTA/swc_ota.c:443
