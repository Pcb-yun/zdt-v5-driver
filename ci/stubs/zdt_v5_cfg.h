/**
 * @file zdt_v5_cfg.h
 * @brief CI 编译检查用配置（开启全功能以最大化代码覆盖率）
 *
 * zdt-v5-driver - 张大头V5步进电机通用驱动
 * Copyright (C) 2024-2026  Pcb-yun
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * 此文件仅用于 GitHub Actions CI 编译验证。
 */

#ifndef __ZDT_V5_CFG_H__
#define __ZDT_V5_CFG_H__

#include "zdt_v5_proto.h"

#define CURRENT_FIRMWARE                   FIRMWARE_EMM
#define CURRENT_MOTOR_MODEL                MOTOR_MODEL_X42S
#define USE_HEARTBEAT                      1
#define ZDT_STEP_NUM                          4
#define ZDT_ONLY_DRIVER                        0

/******************** 设备信息与特殊功能 *********************/
#define MOTOR_READ_VERSION                 1
#define MOTOR_READ_PHASE_PARAMS            1
#define MOTOR_BROADCAST_READ_ID            1
#define MOTOR_READ_OPTION_PARAMS           1
#define MOTOR_DRIVER_MOTOR_TYPE            1
#define MOTOR_DIRECTION                    1
#define MOTOR_FIRMWARE_TYPE                1
#define MOTOR_LOCK_PARAMS                  1
#define MOTOR_DRIVER_CONTROL_MODE          1
#define MOTOR_DRIVER_LOCK_KEY              1
#define MOTOR_DRIVER_SCALE_10X             1

/******************** 系统状态参数 *********************/
#define MOTOR_STATUS_READ_BATCH            1
#define MOTOR_STATUS_ENCODER_RAW           1
#define MOTOR_STATUS_PIN_STATUS            1
#define MOTOR_STATUS_TEMPERATURE           1
#define MOTOR_STATUS_FLAGS_COMBINED        1
#define MOTOR_STATUS_MOTOR_FLAGS           1
#define MOTOR_STATUS_HOME_FLAGS            1
#define MOTOR_STATUS_BUS_CURRENT           1
#define MOTOR_STATUS_BATTERY_VOLTAGE       1
#define MOTOR_STATUS_BUS_VOLTAGE           1
#define MOTOR_STATUS_PHASE_CURRENT         1
#define MOTOR_STATUS_ENCODER_VALUE         1
#define MOTOR_STATUS_TARGET_POS            1
#define MOTOR_STATUS_SPEED                 1
#define MOTOR_STATUS_REAL_POS              1
#define MOTOR_STATUS_POS_ERROR             1
#define MOTOR_STATUS_INPUT_PULSES          1
#define MOTOR_STATUS_SET_POS               1

/******************** 驱动配置参数 *********************/
#define MOTOR_DRIVER_READ_BATCH            1
#define MOTOR_DRIVER_MICRO_STEP            1
#define MOTOR_DRIVER_FIRMWARE_TYPE         1
#define MOTOR_DRIVER_OPENLOOP_CURRENT      1
#define MOTOR_DRIVER_CLOSEDLOOP_CURRENT    1
#define MOTOR_DRIVER_BAUDRATE              1
#define MOTOR_DRIVER_CAN_RATE              1
#define MOTOR_DRIVER_STALL_PROTECT         1
#define MOTOR_DRIVER_STALL_SPEED           1
#define MOTOR_DRIVER_STALL_CURRENT         1
#define MOTOR_DRIVER_STALL_TIME            1
#define MOTOR_DRIVER_PULSE_PORT_MODE       1
#define MOTOR_DRIVER_COMM_PORT_MODE        1
#define MOTOR_DRIVER_EN_PIN_LEVEL          1
#define MOTOR_DRIVER_DIR_PIN_LEVEL         1
#define MOTOR_DRIVER_AUTO_SCREEN_OFF       1
#define MOTOR_DRIVER_COMM_CHECK_MODE       1
#define MOTOR_DRIVER_CMD_RESPONSE_MODE     1
#define MOTOR_DRIVER_CURRENT_LOOP_BANDWIDTH 1
#define MOTOR_DRIVER_CLOSEDLOOP_MAX_SPEED  1
#define MOTOR_DRIVER_CLOSEDLOOP_MAX_VOLTAGE 1
#define MOTOR_DRIVER_POS_WINDOW            1
#define MOTOR_DRIVER_HOME                  1
#define MOTOR_DRIVER_HOME_WRITE            1
#define MOTOR_DRIVER_HOME_MODE             1
#define MOTOR_DRIVER_HOME_DIR              1
#define MOTOR_DRIVER_HOME_SPEED            1
#define MOTOR_DRIVER_HOME_TIMEOUT          1
#define MOTOR_DRIVER_AUTO_HOME             1
#define MOTOR_DRIVER_HOME_COLLISION_SPEED  1
#define MOTOR_DRIVER_HOME_COLLISION_CURRENT 1
#define MOTOR_DRIVER_HOME_COLLISION_TIME   1

/******************** 控制参数读写 *********************/
#define MOTOR_PID_READ                     1
#define MOTOR_PID_WRITE                    1
#define MOTOR_INTEGRAL_LIMIT_READ          1
#define MOTOR_INTEGRAL_LIMIT_WRITE         1
#define MOTOR_PROTECT_THRESHOLD_READ       1
#define MOTOR_PROTECT_THRESHOLD_WRITE      1
#define MOTOR_COLLISION_ANGLE_READ         1
#define MOTOR_COLLISION_ANGLE_WRITE        1
#define MOTOR_HEARTBEAT_READ               1
#define MOTOR_HEARTBEAT_WRITE              1

/******************** 运动控制命令 *********************/
#define MOTOR_CMD_ENABLE                   1
#define MOTOR_CMD_STOP                     1
#define MOTOR_POS_MODE_FAST                1
#define MOTOR_POS_MODE                     1
#define MOTOR_VELOCITY_MODE                1
#define MOTOR_POS_MODE_DIRECT              1
#define MOTOR_TORQUE_MODE                  1
#define MOTOR_POS_MODE_DIRECT_LIMIT        1
#define MOTOR_TORQUE_MODE_LIMIT            1
#define MOTOR_POS_MODE_LIMIT               1
#define MOTOR_VELOCITY_MODE_LIMIT          1
#define MOTOR_SYNC_TRIGGER                 1
#define MOTOR_MULTI_CMD                    1
#if MOTOR_MULTI_CMD
#define MOTOR_MULTI_PTR_BUF                0
#define MOTOR_MULTI_BUF_SIZE               96
#define MOTOR_MULTI_BUF_FREE(ptr)          
#endif /* MOTOR_MULTI_CMD */

/******************** 触发动作命令 *********************/
#define MOTOR_TRIGGER_ENCODER_CALIB        1
#define MOTOR_TRIGGER_RESET_POS            1
#define MOTOR_TRIGGER_CLEAR_PROTECT        1
#define MOTOR_TRIGGER_FACTORY_RESET        1
#define MOTOR_HOME_SET_ZERO                1
#define MOTOR_HOME_TRIGGER                 1
#define MOTOR_HOME_INTERRUPT               1
#define MOTOR_TRIGGER_RESTART              1

/******************** 电机参数设置 *********************/
#define MOTOR_SET_MOTOR_ID                 1
#define MOTOR_SET_MICRO_STEP               1
#define MOTOR_SET_POWER_FLAG               1
#define MOTOR_SET_MOTOR_TYPE               1
#define MOTOR_SET_FIRMWARE_TYPE            1
#define MOTOR_SET_OPENLOOP_CURRENT         1
#define MOTOR_SET_CLOSEDLOOP_CURRENT       1
#define MOTOR_SET_CONTROL_MODE             1
#define MOTOR_SET_DIRECTION                1
#define MOTOR_SET_LOCK_KEY                 1
#define MOTOR_SET_SCALE_INPUT              1
#define MOTOR_SET_DRIVER_CONFIG_BATCH      1
#define MOTOR_SET_LOCK_PARAMS              1
#define MOTOR_POS_WINDOW_WRITE             1
#define MOTOR_DMX512_WRITE                 1
#define MOTOR_PERIODIC_RETURN              1

#endif /* __ZDT_V5_CFG_H__ */
