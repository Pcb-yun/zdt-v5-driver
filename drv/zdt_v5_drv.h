/**
 * @file zdt_v5_drv.h
 * @brief 张大头V5步进电机驱动层接口
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
 */

#ifndef __ZDT_V5_DRV_H__
#define __ZDT_V5_DRV_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "zdt_v5_cmd.h"

#define ZDT_V5_SYS_READ_ENABLE \
	(MOTOR_STATUS_READ_BATCH || MOTOR_STATUS_BUS_VOLTAGE || MOTOR_STATUS_PHASE_CURRENT || MOTOR_STATUS_ENCODER_VALUE || \
	 MOTOR_STATUS_ENCODER_RAW || MOTOR_STATUS_TARGET_POS || MOTOR_STATUS_SET_POS || MOTOR_STATUS_SPEED || \
	 MOTOR_STATUS_REAL_POS || MOTOR_STATUS_POS_ERROR || MOTOR_STATUS_INPUT_PULSES || MOTOR_STATUS_MOTOR_FLAGS || \
	 MOTOR_STATUS_HOME_FLAGS || MOTOR_STATUS_BUS_CURRENT || MOTOR_STATUS_TEMPERATURE || MOTOR_STATUS_FLAGS_COMBINED || \
	 MOTOR_STATUS_BATTERY_VOLTAGE || MOTOR_STATUS_PIN_STATUS)

#define ZDT_V5_DRV_READ_ENABLE \
	(MOTOR_DRIVER_READ_BATCH || MOTOR_DRIVER_POS_WINDOW || MOTOR_DRIVER_HOME)

#define ZDT_V5_CTRL_READ_ENABLE \
	(MOTOR_PID_READ || MOTOR_INTEGRAL_LIMIT_READ || \
	 MOTOR_PROTECT_THRESHOLD_READ || MOTOR_COLLISION_ANGLE_READ || MOTOR_HEARTBEAT_READ)

#define ZDT_V5_INFO_READ_ENABLE \
	(MOTOR_READ_VERSION || MOTOR_READ_PHASE_PARAMS || \
	 MOTOR_READ_OPTION_PARAMS || MOTOR_BROADCAST_READ_ID || MOTOR_DRIVER_DMX512)

#if ZDT_V5_SYS_READ_ENABLE
void ZDT_V5_Read_Sys_Params(uint8_t addr, SysParams_t s);
#endif

#if ZDT_V5_DRV_READ_ENABLE
void ZDT_V5_Read_Driver_Params(uint8_t addr, DriverParams_t d);
#endif

#if ZDT_V5_CTRL_READ_ENABLE
void ZDT_V5_Read_Ctrl_Params(uint8_t addr, CtrlParams_t c);
#endif

#if ZDT_V5_INFO_READ_ENABLE
void ZDT_V5_Read_Device_Info_Params(uint8_t addr, DeviceInfo_t i);
#endif

#if MOTOR_PERIODIC_RETURN
void ZDT_V5_Auto_Return_Sys_Params_Timed(uint8_t addr, SysParams_t s, uint16_t time_ms);
#endif

#if MOTOR_PID_WRITE
#if CURRENT_FIRMWARE == FIRMWARE_EMM
void ZDT_V5_Modify_PID_Params(uint8_t addr, bool svF, uint32_t kp, uint32_t ki, uint32_t kd);
#elif CURRENT_FIRMWARE == FIRMWARE_X
void ZDT_V5_Modify_PID_Params(uint8_t addr, bool svF, uint32_t trapezoidal_kp, uint32_t direct_kp, uint32_t vel_kp, uint32_t vel_ki);
#endif
#endif

#if MOTOR_INTEGRAL_LIMIT_WRITE
void ZDT_V5_Modify_Integral_Limit(uint8_t addr, bool svF, uint32_t il);
#endif

#if MOTOR_POS_WINDOW_WRITE
void ZDT_V5_Modify_Pos_Window(uint8_t addr, bool svF, uint16_t prw);
#endif

#if MOTOR_PROTECT_THRESHOLD_WRITE
void ZDT_V5_Modify_Otocp(uint8_t addr, bool svF, uint16_t otp, uint16_t ocp, uint16_t time_ms);
#endif

#if MOTOR_HEARTBEAT_WRITE
void ZDT_V5_Modify_Heart_Protect(uint8_t addr, bool svF, uint32_t hp);
#endif

#if MOTOR_COLLISION_ANGLE_WRITE
void ZDT_V5_Modify_Collision_Angle(uint8_t addr, bool svF, uint16_t angle);
#endif

/******************** 运动控制命令 *********************/

#if MOTOR_CMD_ENABLE
void ZDT_V5_En_Control(uint8_t addr, bool state, bool snF);
#endif

#if MOTOR_VELOCITY_MODE
void ZDT_V5_Vel_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint16_t acc, bool snF);
#endif

#if MOTOR_VELOCITY_MODE_LIMIT
void ZDT_V5_Vel_Control_With_Limit(uint8_t addr, uint8_t dir, uint16_t vel, uint16_t acc, bool snF, uint16_t max_current);
#endif

#if MOTOR_POS_MODE_DIRECT
void ZDT_V5_Pos_Control_Direct(uint8_t addr, uint8_t dir, uint16_t vel, uint32_t pos, uint8_t mode, bool snF);
#endif

#if MOTOR_POS_MODE_DIRECT_LIMIT
void ZDT_V5_Pos_Control_Direct_With_Limit(uint8_t addr, uint8_t dir, uint16_t vel, uint32_t pos, uint8_t mode, bool snF, uint16_t max_current);
#endif

#if MOTOR_POS_MODE
void ZDT_V5_Pos_Control(uint8_t addr, uint8_t dir, uint16_t vel, uint16_t acc, uint16_t dec, uint32_t clk, bool raF, bool snF);
#endif

#if MOTOR_POS_MODE_LIMIT
void ZDT_V5_Pos_Control_With_Limit(uint8_t addr, uint8_t dir, uint16_t vel, uint16_t acc, uint16_t dec, uint32_t pos, uint8_t mode, bool snF, uint16_t max_current);
#endif

#if MOTOR_TORQUE_MODE
void ZDT_V5_Torque_Control(uint8_t addr, uint8_t dir, uint16_t slope, uint16_t current, bool snF);
#endif

#if MOTOR_TORQUE_MODE_LIMIT
void ZDT_V5_Torque_Control_With_Limit(uint8_t addr, uint8_t dir, uint16_t slope, uint16_t current, uint16_t max_vel, bool snF);
#endif

#if MOTOR_CMD_STOP
void ZDT_V5_Stop(uint8_t addr, bool snF);
#endif

#if MOTOR_SYNC_TRIGGER
void ZDT_V5_Synchronous_Motion(uint8_t addr);
#endif

#if MOTOR_POS_MODE_FAST
void ZDT_V5_Fast_Set_Param(uint8_t addr, uint16_t vel, uint16_t acc, uint16_t dec, uint16_t max_current, uint8_t mode, uint8_t sync);
void ZDT_V5_Fast_Send_Pos(uint8_t addr, int32_t pos);
#endif

/******************** 触发动作命令 *********************/

#if MOTOR_TRIGGER_ENCODER_CALIB
void ZDT_V5_Trig_Encoder_Cal(uint8_t addr);
#endif

#if MOTOR_TRIGGER_RESTART
void ZDT_V5_Reset_Motor(uint8_t addr);
#endif

#if MOTOR_TRIGGER_RESET_POS
void ZDT_V5_Reset_CurPos_To_Zero(uint8_t addr);
#endif

#if MOTOR_TRIGGER_CLEAR_PROTECT
void ZDT_V5_Reset_Clog_Pro(uint8_t addr);
#endif

#if MOTOR_TRIGGER_FACTORY_RESET
void ZDT_V5_Restore_Motor(uint8_t addr);
#endif

/******************** 原点回零命令 *********************/

#if MOTOR_HOME_SET_ZERO
void ZDT_V5_Origin_Set_Zero(uint8_t addr, bool svF);
#endif

#if MOTOR_HOME_TRIGGER
void ZDT_V5_Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, bool snF);
#endif

#if MOTOR_HOME_INTERRUPT
void ZDT_V5_Origin_Interrupt(uint8_t addr);
#endif

#if MOTOR_DRIVER_HOME_WRITE
void ZDT_V5_Origin_Modify_Params(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF);
#endif

/******************** 电机参数设置 *********************/

#if MOTOR_SET_MOTOR_ID
void ZDT_V5_Modify_Motor_ID(uint8_t addr, bool svF, uint8_t id);
#endif

#if MOTOR_SET_MICRO_STEP
void ZDT_V5_Modify_MicroStep(uint8_t addr, bool svF, uint8_t mstep);
#endif

#if MOTOR_SET_POWER_FLAG
void ZDT_V5_Modify_PDFlag(uint8_t addr, bool pdf);
#endif

#if MOTOR_SET_MOTOR_TYPE
void ZDT_V5_Modify_Motor_Type(uint8_t addr, bool svF, bool mottype);
#endif

#if MOTOR_SET_FIRMWARE_TYPE
void ZDT_V5_Modify_Firmware_Type(uint8_t addr, bool svF, bool fwtype);
#endif

#if MOTOR_SET_OPENLOOP_CURRENT
void ZDT_V5_Modify_OM_mA(uint8_t addr, bool svF, uint16_t om_ma);
#endif

#if MOTOR_SET_CLOSEDLOOP_CURRENT
void ZDT_V5_Modify_FOC_mA(uint8_t addr, bool svF, uint16_t foc_mA);
#endif

#if MOTOR_SET_CONTROL_MODE
void ZDT_V5_Modify_Ctrl_Mode(uint8_t addr, bool svF, bool ctrl_mode);
#endif

#if MOTOR_SET_DIRECTION
void ZDT_V5_Modify_Motor_Dir(uint8_t addr, bool svF, bool dir);
#endif

#if MOTOR_SET_LOCK_KEY
void ZDT_V5_Modify_Lock_Btn(uint8_t addr, bool svF, bool lockbtn);
#endif

#if MOTOR_SET_SCALE_INPUT
void ZDT_V5_Modify_S_Vel(uint8_t addr, bool svF, bool s_vel);
#endif

#if MOTOR_SET_DRIVER_CONFIG_BATCH
void ZDT_V5_Modify_Batch_Config(uint8_t addr, bool svF, uint8_t *params);
#endif

#if MOTOR_SET_LOCK_PARAMS
void ZDT_V5_Modify_Lock_Params(uint8_t addr, bool svF, uint8_t lock_level);
#endif

#if MOTOR_DMX512_WRITE
void ZDT_V5_Modify_DMX512_Params(uint8_t addr, bool svF, uint16_t tch, uint8_t nch, uint8_t mode, uint16_t vel, uint16_t acc, uint16_t vel_step, uint32_t pos_step);
#endif

/******************** 多机指令构造 *********************/

#if MOTOR_MULTI_CMD
bool ZDT_V5_Multi_Reset(ZDT_V5_Multi_Cmd_t *cmd);
bool ZDT_V5_Multi_Send(ZDT_V5_Multi_Cmd_t *cmd);

#if MOTOR_VELOCITY_MODE
bool ZDT_V5_Multi_Vel_Ctrl(ZDT_V5_Multi_Cmd_t *cmd, uint8_t addr, uint8_t dir, uint16_t vel, uint16_t acc, bool snF);
#endif

#if MOTOR_VELOCITY_MODE_LIMIT
bool ZDT_V5_Multi_Vel_Ctrl_Limit(ZDT_V5_Multi_Cmd_t *cmd, uint8_t addr, uint8_t dir, uint16_t vel, uint16_t acc, bool snF, uint16_t max_current);
#endif

#if MOTOR_POS_MODE
bool ZDT_V5_Multi_Pos_Ctrl(ZDT_V5_Multi_Cmd_t *cmd, uint8_t addr, uint8_t dir, uint16_t vel, uint16_t acc, uint16_t dec, uint32_t clk, bool raF, bool snF);
#endif

#if MOTOR_POS_MODE_LIMIT
bool ZDT_V5_Multi_Pos_Ctrl_Limit(ZDT_V5_Multi_Cmd_t *cmd, uint8_t addr, uint8_t dir, uint16_t vel, uint16_t acc, uint16_t dec, uint32_t pos, uint8_t mode, bool snF, uint16_t max_current);
#endif

#if MOTOR_POS_MODE_DIRECT
bool ZDT_V5_Multi_Pos_Ctrl_Direct(ZDT_V5_Multi_Cmd_t *cmd, uint8_t addr, uint8_t dir, uint16_t vel, uint32_t pos, uint8_t mode, bool snF);
#endif

#if MOTOR_POS_MODE_DIRECT_LIMIT
bool ZDT_V5_Multi_Pos_Ctrl_Direct_Limit(ZDT_V5_Multi_Cmd_t *cmd, uint8_t addr, uint8_t dir, uint16_t vel, uint32_t pos, uint8_t mode, bool snF, uint16_t max_current);
#endif

#if MOTOR_TORQUE_MODE
bool ZDT_V5_Multi_Torque_Ctrl(ZDT_V5_Multi_Cmd_t *cmd, uint8_t addr, uint8_t dir, uint16_t slope, uint16_t current, bool snF);
#endif

#if MOTOR_TORQUE_MODE_LIMIT
bool ZDT_V5_Multi_Torque_Ctrl_Limit(ZDT_V5_Multi_Cmd_t *cmd, uint8_t addr, uint8_t dir, uint16_t slope, uint16_t current, uint16_t max_vel, bool snF);
#endif
#endif /* MOTOR_MULTI_CMD */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ZDT_V5_DRV_H__ */
