/**
 * @file zdt_v5_port.h
 * @brief CI 编译检查用移植接口
 *
 * zdt-v5-driver - 张大头V5步进电机通用驱动
 * Copyright (C) 2024-2026  Pcb-yun
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * 此文件仅用于 GitHub Actions CI 编译验证。
 */

#ifndef __ZDT_V5_PORT_H__
#define __ZDT_V5_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void zdt_v5_port_send(uint8_t *cmd, uint8_t len);

#define ZDT_V5_LOG(fmt, ...)

#ifdef __cplusplus
}
#endif

#endif /* __ZDT_V5_PORT_H__ */
