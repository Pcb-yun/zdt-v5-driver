# zdt-v5-driver

[![Build](https://github.com/Pcb-yun/zdt-v5-driver/actions/workflows/build.yml/badge.svg)](https://github.com/Pcb-yun/zdt-v5-driver/actions/workflows/build.yml)

张大头 V5 系列步进电机通用驱动库。

## 关于此仓库

一切的起因只是因为我不喜欢官方提供的 API。

本仓库基于张大头官方通讯手册重新设计了驱动架构，主要有以下特点：

- **统一的命令模型** — 使用 `MotorCmd_t` 结构体封装所有操作类型（控制/触发/参数读写），参数清晰，不易出错
- **平台无关** — 核心层零平台依赖，仅需实现一个串口发送函数即可移植
- **条件编译裁剪** — 所有功能通过配置文件开关控制，未启用的功能不会被编译，零额外开销
- **原生 RTOS 友好** — 命令通过队列传递，天然适配任务间通信；接收解析与命令执行分离，符合 RTOS 任务分工
- **双层架构可选** — 引擎层提供完整的命令封装和状态解析，驱动层提供轻量的函数式 API

## 特性

| 特性      | 说明                                   |
| ------- | ------------------------------------ |
| 固件支持    | Emm 固件 + X 固件，同一 API 兼容两种协议          |
| 型号支持    | X42、X42S、Y42 全系列                     |
| 功能覆盖    | 使能/速度/位置/力矩/停止/同步/回零/参数读写/编码器校准等全部功能 |
| 条件编译    | 约 100 个配置开关，精确控制每个功能的编译              |
| 资源占用    | 配置开关关闭的功能完全不占用代码空间                   |
| 多电机     | 通过 `MOTOR_NUM` 配置，API 天然支持任意数量电机     |
| RTOS 适配 | 命令队列 + 接收队列 + 状态轮询任务架构，开箱即用          |
| 无平台依赖   | 仅需标准 C 库，任何 MCU 架构均可移植               |

## 支持的电机

| 型号 | X42 | X42S | Y42 |
| :--: | :-: | :--: | :-: |
| 固件 | Emm / X | Emm / X | Emm / X |

## 文件架构

```
├── core/                    # 协议定义层（纯 C 类型 + 宏，零平台依赖）
│   ├── zdt_v5_proto.h      # 协议命令码、固件类型定义
│   └── zdt_v5_cmd.h        # 命令结构体、协议枚举定义
│
├── drv/                     # 驱动协议层
│   ├── zdt_v5_drv.h/.c     # 驱动 API（命令打包，条件编译裁剪）
│   └── zdt_v5_engine.h/.c  # 引擎层（响应解析、命令分发，可通过 ONLY_DRIVER 关闭）
│
├── template/                # 移植模板（用户复制到工程中修改）
│   ├── zdt_v5_cfg_template.h   # 配置文件模板
│   ├── zdt_v5_port_template.h  # 移植接口模板
│   └── zdt_v5_port_template.c  # 移植实现模板
│
├── example/                 # 使用示例（支持 ONLY_DRIVER 模式）
│   ├── bare_metal/          # 裸机示例
│   └── freertos/            # FreeRTOS 示例
│
├── docs/                    # 官方文档与通讯协议手册
│   ├── ZDT_X42S_第二代闭环步进电机用户手册V1.0.4_260401.pdf  # 官方原始 PDF 文档
│   ├── ZDT_X42S_通讯指令手册.md       # AI 辅助整理的通讯指令参考
│   └── ZDT_X42S_指令分类与包含关系.md  # AI 辅助整理的指令分类与关系图
└── LICENSE                  # GPL-3.0-or-later
```

**架构选择：**

| 模式   | `ONLY_DRIVER` | 包含层                 | 适用场景                  |
| ---- | ------------- | ------------------- | --------------------- |
| 完整模式 | `0`（默认）       | core + drv + engine | RTOS 环境、需要状态解析、复杂命令控制 |
| 轻量模式 | `1`           | core + drv          | 裸机环境、资源受限、简单控制逻辑      |

## 移植方法

### 文件组织

将 `core/` 和 `drv/` 两个目录添加到工程的头文件搜索路径和编译列表中。

此仓库不提供默认的 `zdt_v5_cfg.h` 和 `zdt_v5_port.h`，因为这两个文件必须由用户根据实际硬件平台创建。`template/` 目录中提供了模板文件，复制到工程中修改即可。

### 移植步骤

#### 1. 创建配置文件

复制 `template/zdt_v5_cfg_template.h` 到工程中，改名为 `zdt_v5_cfg.h`。

根据硬件配置：

- 选择固件版本（`CURRENT_FIRMWARE`）
- 选择电机型号（`CURRENT_MOTOR_MODEL`）
- 设置电机数量（`MOTOR_NUM`）
- 按需开启功能开关

```c
#define CURRENT_FIRMWARE         FIRMWARE_EMM
#define CURRENT_MOTOR_MODEL      MOTOR_MODEL_X42S
#define MOTOR_NUM                4
#define ONLY_DRIVER              0   // 仅使用驱动层，不使用引擎层（见下文）

// 按需开启功能（0=关闭，1=开启）
#define MOTOR_CMD_ENABLE         1   // 使能控制
#define MOTOR_VELOCITY_MODE      1   // 速度模式
#define MOTOR_POS_MODE_TRAPEZOIDAL 1 // 位置模式
#define MOTOR_TRIGGER_RESET_POS  1   // 位置清零
```

#### 1.1 仅驱动层模式（ONLY\_DRIVER）

设置 `ONLY_DRIVER = 1` 可启用轻量级模式：

- **不编译引擎层** — `zdt_v5_engine.c/.h` 完全不参与编译，减小代码体积
- **直接调用驱动层 API** — 使用 `ZDT_V5_En_Control()`、`ZDT_V5_Vel_Control()` 等函数
- **无命令结构体** — 无需 `MotorCmd_t`，参数直接传入函数
- **适合场景** — 裸机环境、资源受限的 MCU、简单的电机控制逻辑

对比两种使用方式：

| 方式  | 接口                                    | 示例                      |
| --- | ------------------------------------- | ----------------------- |
| 引擎层 | `ZDT_V5_Process_Cmd(cmd)`             | 通过 `MotorCmd_t` 结构体传递参数 |
| 驱动层 | `ZDT_V5_En_Control(id, enable, sync)` | 直接调用函数，参数清晰             |

#### 2. 创建移植接口头文件

复制 `template/zdt_v5_port_template.h` 到工程中，改名为 `zdt_v5_port.h`。

此文件声明了用户需要实现的串口发送函数 `zdt_v5_port_send()`。

#### 3. 实现串口发送函数

复制 `template/zdt_v5_port_template.c` 到工程中，改名为 `zdt_v5_port.c`。

根据实际硬件平台实现发送函数：

```c
#include "zdt_v5_port.h"

void zdt_v5_port_send(uint8_t *cmd, uint8_t len) {
    /* STM32 HAL 示例 */
    HAL_UART_Transmit(&huart6, cmd, len, 1000);
}
```

函数内部可以使用阻塞发送、DMA 发送或中断发送，仓库对此无限制。

#### 4. 处理串口接收

串口接收由用户管理（中断或 DMA），收到完整的数据帧后调用 `ZDT_V5_Receive()` 即可：

```c
#include "zdt_v5_engine.h"

/* 串口接收中断回调 */
void on_uart_rx(uint8_t *data, uint8_t len) {
    ZDT_V5_Receive(data, len, motors);
}
```

### 注意事项

- 移植接口仅需实现串口发送，**接收由中断/DMA 处理**
- 日志输出可选，不需要时无需任何配置
- 仓库中所有 `.h` 文件均包含 `extern "C"` 声明，C++ 工程可直接使用

## API 概览

### 驱动层 (`zdt_v5_drv.h`)

| 分类    | 函数                             | 说明             |
| ----- | ------------------------------ | -------------- |
| 使能控制  | `ZDT_V5_En_Control`            | 电机使能/失能        |
| 速度模式  | `ZDT_V5_Vel_Control`           | 速度控制           |
| 位置模式  | `ZDT_V5_Pos_Control`           | 梯形曲线位置控制       |
| 力矩模式  | `ZDT_V5_Torque_Control`        | 力矩控制           |
| 立即停止  | `ZDT_V5_Stop`                  | 紧急停止           |
| 同步运动  | `ZDT_V5_Synchronous_Motion`    | 多机同步触发         |
| 编码器校准 | `ZDT_V5_Trig_Encoder_Cal`      | 触发编码器校准        |
| 回零    | `ZDT_V5_Origin_Trigger_Return` | 触发回零           |
| 参数设置  | `ZDT_V5_Modify_*`              | PID、电流、细分等参数设置 |

所有函数均通过条件编译控制，仅在配置中启用的功能才会被编译。

### 引擎层 (`zdt_v5_engine.h`)

| 函数                   | 说明                                  |
| -------------------- | ----------------------------------- |
| `ZDT_V5_Receive`     | 处理串口接收数据，更新电机状态到 `MotorStatus_t` 数组 |
| `ZDT_V5_Process_Cmd` | 执行电机命令（同步调用，内部调用发送函数）               |

### 命令结构体 (`zdt_v5_cmd.h`)

核心数据结构 `MotorCmd_t` 覆盖所有电机操作：

```c
MotorCmd_t cmd;
cmd.op_type = OP_CONTROL;         // 操作类型
cmd.motor_id = 1;                 // 目标电机 ID
cmd.type.ctrl.type = CTRL_VELOCITY; // 控制类型
cmd.type.ctrl.p.vel.dir = 0;      // 方向
cmd.type.ctrl.p.vel.vel = 500;    // 速度
cmd.type.ctrl.p.vel.acc = 200;    // 加速度
cmd.type.ctrl.p.vel.sync = false; // 同步标志
```

### 移植接口 (`zdt_v5_port.h`)

| 接口                   | 说明             |
| -------------------- | -------------- |
| `zdt_v5_port_send()` | 串口发送函数（用户必须实现） |
| `ZDT_V5_LOG`         | 日志输出宏（可选，默认空）  |

## RTOS 使用建议

本仓库的架构天然适配 RTOS 系统，推荐的任务分工：

- **Ctrl\_Task** — 阻塞等待命令队列，收到命令后调用 `ZDT_V5_Process_Cmd` 执行
- **Recv\_Task** — 阻塞等待接收队列，收到串口数据后调用 `ZDT_V5_Receive` 解析
- **Update\_Task** — 定时循环，发送参数读取命令更新电机状态
- **App\_Task** — 应用逻辑，构造命令并通过队列发送

详细实现参考 `example/freertos/main.c`。

## 许可证

GNU General Public License v3.0 or later (GPL-3.0-or-later)

Copyright (C) 2024-2026 Pcb-yun
