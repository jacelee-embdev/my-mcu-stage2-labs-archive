# Understand 分析工程

此目录保留正式分析工程和系统总流程图插件。导出的 10 张图片及阅读说明见仓库根目录的 [中文 README](../README_zh.md#understand-流程与依赖图)。图片编号按阅读优先级排列。

## 保留内容

- `065_AHT21_Handler_Integration_Test.und/`：Understand 工程目录，包含分析配置、数据库和阅读会话。整个目录应一起保留。
- `AHT21_System_Lifecycle.upy`：系统总流程图插件，包含人工核对的中文说明、线程入口和循环回线。

搜索索引、临时问题报告及统计缓存已加入仓库的 `.gitignore`；旧备份、配置试验脚本和临时界面快照已清理。

## 打开与查看

1. 使用 Understand 打开上述 `.und` 工程目录。本工程使用 Understand Offline Build 1206 配置。
2. 将 `AHT21_System_Lifecycle.upy` 复制到 `%APPDATA%\SciTools\plugin\Graph\`，重启 Understand 后即可加载总流程插件。
3. 在 `Graphs → Project Graphs → 01 AHT21 System Lifecycle` 查看总流程；函数调用图和文件包含图可从对应函数或文件的 `Graphical Views` 菜单打开。

总流程图依据源码人工整理并由 Understand 渲染；原生调用图反映静态分析关系。代码变化后需重新分析，并核对总流程插件。工程中保留了本机的头文件与 ARMCC 工具链路径，换到其他位置或电脑时需调整。
