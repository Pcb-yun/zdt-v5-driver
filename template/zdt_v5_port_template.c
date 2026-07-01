/**
 * @file zdt_v5_port_template.c
 * @brief 张大头V5步进电机移植接口模板
 *
 * zdt-v5-driver - 张大头V5步进电机通用驱动
 * Copyright (C) 2024-2026  Pcb-yun
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * 将本文件复制到工程中，并改名为 zdt_v5_port.c
 */

/* ==================== 1. 实现发送函数（必须） ==================== */
/*
#include "zdt_v5_port.h"

/**
 * @brief 串口发送函数（必须）
 * @param cmd 命令指针
 * @param len 命令长度
 */
/*
void zdt_v5_port_send(uint8_t *cmd, uint8_t len) {
    // STM32 HAL 阻塞发送:
    //     HAL_UART_Transmit(&huart6, cmd, len, 1000);

    // STM32 HAL DMA 发送（需配合 TX 完成中断）:
    //     HAL_UART_Transmit_DMA(&huart6, cmd, len);

    // ESP-IDF:
    //     uart_write_bytes(UART_NUM, cmd, len);

    // Linux:
    //     write(uart_fd, cmd, len);
}
*/

/* ==================== 2. 可选：实现日志输出函数 ==================== */
/*
 * 引擎层通过 ZDT_V5_LOG 宏输出警告信息，默认为空（不输出）。
 * 如需启用，在 zdt_v5_port.h 中将宏指向用户实现的日志函数，例如:
 *
 *   void zdt_v5_port_log(const char *fmt, ...);
 *
 *   #define ZDT_V5_LOG(fmt, ...)  zdt_v5_port_log("[STEP] " fmt "\r\n", ##__VA_ARGS__)
 *
 * 然后在 zdt_v5_port.c 中实现该函数:
 *
 *   #include <stdio.h>
 *   #include <stdarg.h>
 *
 *   void zdt_v5_port_log(const char *fmt, ...) {
 *       va_list args;
 *       va_start(args, fmt);
 *       vprintf(fmt, args);
 *       va_end(args);
 *   }
 */
