/**
 * @file zdt_v5_cmd.h
 * @brief 电机命令结构体定义
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

#ifndef __ZDT_V5_CMD_H__
#define __ZDT_V5_CMD_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>
#include "zdt_v5_proto.h"
#include "zdt_v5_cfg.h"

/**
 * @brief 电机命令操作类型枚举
 */
typedef enum {
	OP_NONE = 0,      // 无操作，空命令
	OP_CONTROL,       // 运动控制（使能/速度/位置/力矩/停止/同步）
	OP_TRIGGER,       // 触发动作（编码器校准/重启/清零/解除保护/重置/回零动作）
	OP_PARAM_READ,    // 参数读取（使用 MotorParamRead_t 选择四大分类枚举值）
	OP_PARAM_WRITE    // 参数写入（使用 MotorParamWrite_t 选择具体写入类型）
} MotorOpType_t;

/**
 * @brief 电机参数分类枚举
 */
typedef enum {
	MP_NONE = 0,		// 无参数分类
	MP_SYS,				// 系统状态参数
	MP_DEV,				// 驱动配置参数
	MP_CTRL,			// 控制参数
	MP_INFO				// 设备信息与特殊功能
} MotorParam_t;

/**
 * @brief 运动控制命令类型枚举
 */
typedef enum {
	CTRL_NONE = 0,       // 无命令
	CTRL_ENABLE,         // 使能控制
	CTRL_VELOCITY,       // 速度模式控制
#if MOTOR_VELOCITY_MODE_LIMIT
	CTRL_VELOCITY_LIMIT, // 速度模式限电流控制
#endif
	CTRL_POSITION,       // 位置模式控制
#if MOTOR_POS_MODE_DIRECT
	CTRL_POS_DIRECT,     // 直通限速位置模式
#endif
#if MOTOR_POS_MODE_DIRECT_LIMIT
	CTRL_POS_DIRECT_LIMIT, // 直通限速位置模式限电流控制
#endif
#if MOTOR_POS_MODE_TRAPEZOIDAL
	CTRL_POS_TRAPEZOIDAL, // 梯形曲线位置模式
#endif
#if MOTOR_POS_MODE_TRAPEZOIDAL_LIMIT
	CTRL_POS_TRAPEZOIDAL_LIMIT, // 梯形曲线位置模式限电流控制
#endif
#if MOTOR_TORQUE_MODE
	CTRL_TORQUE,         // 力矩模式
#endif
#if MOTOR_TORQUE_MODE_LIMIT
	CTRL_TORQUE_LIMIT,   // 力矩模式限速控制
#endif
	CTRL_STOP,           // 立即停止电机运动
#if MOTOR_SYNC_TRIGGER
	CTRL_SYNC,           // 触发多机同步运动
#endif
#if MOTOR_MULTI_CMD
	CTRL_MULTI,          // 多电机命令（通过一条命令控制多个电机）
#endif
#if MOTOR_POS_MODE_FAST
	CTRL_FAST_SET,       // 快速位置模式-设定参数（第一步）
	CTRL_FAST_SEND      // 快速位置模式-发送位置（第二步）
#endif
} MotorCtrlType_t;

/**
 * @brief 运动控制命令结构体
 */
typedef struct {
	MotorCtrlType_t type;  // 控制命令类型

	union {
#if MOTOR_CMD_ENABLE
		struct {
			bool enable;  // 使能状态: true=使能(锁轴), false=不使能(松轴)
			bool sync;    // 同步标志: true=先缓存等待同步触发, false=立即执行
		} en;
#endif

#if MOTOR_VELOCITY_MODE
		struct {
			uint8_t dir;            // 方向: 0=CW(正转), 其他=CCW(反转)
			uint16_t vel;           // 速度, 单位:RPM, X固件范围:0-3000.0(值*10), Emm范围:0-3000
			uint16_t acc;           // 加速度, X固件单位:RPM/S(0-65535), Emm固件:档位(0-255)
#if MOTOR_VELOCITY_MODE_LIMIT
			uint16_t max_current;   // 最大电流限制, 单位:mA, 范围:0-5000
#endif
			bool sync;              // 同步标志: true=先缓存等待同步触发, false=立即执行
		} vel;
#endif

#if MOTOR_POS_MODE_TRAPEZOIDAL
		struct {
			uint8_t dir;            // 方向: 0=CW(正转), 其他=CCW(反转)
			uint16_t vel;           // 速度, X固件单位:0.1RPM(0-3000.0), Emm单位:RPM(0-3000)
			uint16_t acc;           // 加速度, X固件单位:RPM/S(0-65535), Emm固件:档位(0-255)
			int32_t target;         // 目标位置, Emm:脉冲数, X:角度(单位0.1°, 范围0-429496729.5)
			uint8_t mode;           // 运动模式: 0=相对上一目标, 1=绝对位置, 2=相对当前位置

#if CURRENT_FIRMWARE == FIRMWARE_X
			uint16_t dec;           // 减速度, 单位:RPM/S, 范围:0-65535
#endif

#if MOTOR_POS_MODE_TRAPEZOIDAL_LIMIT
			uint16_t max_current;   // 最大电流限制, 单位:mA, 范围:0-5000
#endif

			bool sync;              // 同步标志: true=先缓存等待同步触发, false=立即执行
		} pos;
#endif

#if MOTOR_POS_MODE_DIRECT
		struct {
			uint8_t dir;            // 方向: 0=CW(正转), 其他=CCW(反转)
			uint16_t vel;           // 速度, 单位:0.1RPM, 范围:0-3000.0
			int32_t target;         // 目标位置角度, 单位:0.1°, 范围:0-429496729.5
			uint8_t mode;           // 运动模式: 0=相对上一目标, 1=绝对位置, 2=相对当前位置
			uint16_t max_current;   // 最大电流限制, 单位:mA, 范围:0-5000
			bool snF;               // 同步标志: true=先缓存等待同步触发, false=立即执行
		} pos_dir;
#endif

#if MOTOR_TORQUE_MODE
		struct {
			uint8_t dir;            // 方向: 0=CW(正转), 其他=CCW(反转)
			uint16_t slope;         // 电流斜率(加速度), 单位:mA/S, 范围:0-65535
			uint16_t current;       // 目标电流, 单位:mA, 范围:0-5000
			uint16_t max_vel;       // 最大速度限制, 单位:0.1RPM, 范围:0-3000.0
			bool sync;              // 同步标志: true=先缓存等待同步触发, false=立即执行
		} torque;
#endif

#if MOTOR_CMD_STOP
		struct {
			bool sync;              // 同步标志: true=先缓存等待同步触发, false=立即执行
		} stop;
#endif

#if MOTOR_POS_MODE_FAST
		struct {
#if CURRENT_FIRMWARE == FIRMWARE_EMM
			uint16_t vel;           // 速度, 单位:RPM, 范围:0-3000
			uint8_t acc;            // 加速度, 档位:0-255
#elif CURRENT_FIRMWARE == FIRMWARE_X
			uint16_t acc;           // 加速加速度, 单位:RPM/S, 范围:0-65535
			uint16_t dec;           // 减速加速度, 单位:RPM/S, 范围:0-65535
			uint16_t vel;           // 最大速度, 单位:0.1RPM, 范围:0-3000.0
			uint16_t max_current;   // 最大电流, 单位:mA, 范围:0-5000
#endif
			uint8_t mode;           // 运动模式: 0=相对上一目标, 1=绝对位置, 2=相对当前位置
			bool sync;              // 同步标志: true=立即执行, false=等待同步触发
		} fast_set;

		struct {
			int32_t pos;            // 目标位置, Emm:脉冲数, X:角度(单位0.1°)
		} fast_send;
#endif

#if MOTOR_MULTI_CMD
		struct {
			uint8_t len;            // 命令数据总字节数
			uint8_t *data;          // 命令数据指针（包含多个电机的命令）
		} multi;
#endif

		struct {
			bool dummy;  // 空成员保证编译
		} dummy;
	} p;
} MotorCtrl_t;

/**
 * @brief 触发动作命令类型枚举
 */
typedef enum {
	TRIG_NONE = 0,

#if MOTOR_TRIGGER_ENCODER_CALIB
	TRIG_ENCODER_CALIB,    // 触发编码器校准
#endif

#if MOTOR_TRIGGER_RESTART
	TRIG_RESTART,          // 重启电机
#endif

#if MOTOR_TRIGGER_RESET_POS
	TRIG_RESET_POS,        // 当前位置角度清零
#endif

#if MOTOR_TRIGGER_CLEAR_PROTECT
	TRIG_CLEAR_PROTECT,    // 解除堵转/过热/过流保护
#endif

#if MOTOR_TRIGGER_FACTORY_RESET
	TRIG_FACTORY_RESET,    // 恢复出厂设置
#endif

#if MOTOR_HOME_SET_ZERO
	TRIG_HOME_SET_ZERO,    // 设置单圈回零零点位置
#endif

#if MOTOR_HOME_TRIGGER
	TRIG_HOME_RETURN,      // 触发回零
#endif

#if MOTOR_HOME_INTERRUPT
	TRIG_HOME_INTERRUPT   // 强制中断并退出回零操作
#endif
} MotorTriggerType_t;

/**
 * @brief 触发动作命令结构体
 */
typedef struct {
	MotorTriggerType_t type;  // 触发类型

	union {
#if MOTOR_HOME_SET_ZERO
		struct {
			bool save;      // 是否存储到内存: true=存储, false=不存储
		} set_zero;
#endif

#if MOTOR_HOME_TRIGGER
		struct {
			uint8_t mode;   // 回零模式: 0=单圈就近, 1=单圈方向, 2=无限位碰撞,
			                //           3=限位回零, 4=回到绝对坐标零点, 5=回到上次掉电位置
			bool sync;      // 同步标志: true=先缓存等待同步触发, false=立即执行
		} home;
#endif

		struct {
			bool dummy;  // 空成员保证编译
		} dummy;
	} p;
} MotorTrigger_t;

/**
 * @brief 电机参数类型枚举
 */
typedef enum {
	PARAM_NONE = 0,
	PARAM_BATCH,                // 批量读取参数

#if MOTOR_BROADCAST_READ_ID
	PARAM_MOTOR_ID,             // 电机ID/地址
#endif

#if MOTOR_SET_MICRO_STEP
	PARAM_MICRO_STEP,           // 细分值
#endif

#if MOTOR_SET_POWER_FLAG
	PARAM_POWER_FLAG,           // 掉电存储位置标志
#endif

#if MOTOR_SET_MOTOR_TYPE
	PARAM_MOTOR_TYPE,           // 电机类型
#endif

#if MOTOR_SET_FIRMWARE_TYPE
	PARAM_FIRMWARE_TYPE,        // 固件类型
#endif

#if MOTOR_SET_OPENLOOP_CURRENT
	PARAM_OPENLOOP_CURRENT,     // 开环模式工作电流
#endif

#if MOTOR_SET_CLOSEDLOOP_CURRENT
	PARAM_CLOSEDLOOP_CURRENT,   // 闭环模式最大电流
#endif

#if MOTOR_SET_CONTROL_MODE
	PARAM_CONTROL_MODE,         // 开环/闭环控制模式
#endif

#if MOTOR_SET_DIRECTION
	PARAM_DIRECTION,            // 电机运动正方向
#endif

#if MOTOR_SET_LOCK_KEY
	PARAM_LOCK_KEY,             // 锁定按键功能
#endif

#if MOTOR_SET_SCALE_INPUT
	PARAM_SCALE_INPUT,          // 缩小倍数输入
#endif

#if MOTOR_SET_DRIVER_CONFIG_BATCH
	PARAM_DRIVER_CONFIG_ALL,    // 修改驱动配置参数
#endif

#if MOTOR_SET_LOCK_PARAMS
	PARAM_LOCK_PARAMS,          // 锁定修改参数功能等级
#endif

#if MOTOR_POS_WINDOW_WRITE
	PARAM_POS_WINDOW,           // 位置到达窗口
#endif

#if MOTOR_PID_WRITE
	PARAM_PID,                  // PID参数
#endif

#if MOTOR_INTEGRAL_LIMIT_WRITE
	PARAM_INTEGRAL_LIMIT,       // 积分限幅/刚性系数
#endif

#if MOTOR_PROTECT_THRESHOLD_WRITE
	PARAM_PROTECT_THRESHOLD,    // 过热过流保护检测阈值
#endif

#if MOTOR_HEARTBEAT_WRITE
	PARAM_HEARTBEAT,            // 心跳保护功能时间
#endif

#if MOTOR_COLLISION_ANGLE_WRITE
	PARAM_COLLISION_ANGLE,      // 碰撞回零返回角度
#endif

#if MOTOR_DMX512_WRITE
	PARAM_DMX512,               // DMX512协议参数
#endif

#if MOTOR_DRIVER_HOME_WRITE
	PARAM_HOME_PARAMS,          // 回零参数
#endif

#if MOTOR_PERIODIC_RETURN
	PARAM_PERIODIC_RETURN      // 定时返回信息
#endif
} MotorParamType_t;

/**
 * @brief 系统状态参数枚举
 */
typedef enum {
	S_NULL = 0, // 无指令
#if MOTOR_STATUS_READ_BATCH
	S_BATCH,	// 批量读取
#endif
#if MOTOR_STATUS_BUS_VOLTAGE
	S_VBUS,    // 读取总线电压
#endif
#if MOTOR_STATUS_PHASE_CURRENT
	S_CPHA,   // 读取相电流
#endif
#if MOTOR_STATUS_ENCODER_VALUE
	S_ENCL,  // 读取经过线性化校准后的编码器值
#endif
#if MOTOR_STATUS_TARGET_POS
	S_TPOS,   // 读取电机目标位置
#endif
#if MOTOR_STATUS_SET_POS
	S_SPOS,   // 读取电机实时设定的目标位置
#endif
#if MOTOR_STATUS_SPEED
	S_VEL,   // 读取电机实时转速
#endif
#if MOTOR_STATUS_REAL_POS
	S_CPOS,   // 读取电机实时位置
#endif
#if MOTOR_STATUS_POS_ERROR
	S_PERR,   // 读取电机位置误差
#endif
#if MOTOR_STATUS_MOTOR_FLAGS
	S_FLAG,   // 读取电机状态标志位
#endif
#if MOTOR_STATUS_HOME_FLAGS
	S_OFLAG,   // 读取回零状态标志位
#endif
#if MOTOR_STATUS_BUS_CURRENT
	S_CBUS,    // 读取总线电流
#endif
#if MOTOR_STATUS_TEMPERATURE
	S_TEMP,   // 读取电机实时温度
#endif
#if MOTOR_STATUS_FLAGS_COMBINED
	S_OAF,   // 读取电机状态标志位 + 回零状态标志位（组合）
#endif
#if MOTOR_STATUS_BATTERY_VOLTAGE
	S_VBAT,   // 读取多圈编码器电池电压
#endif
#if MOTOR_STATUS_INPUT_PULSES
	S_CLKI,   // 读取输入脉冲数
#endif
#if MOTOR_STATUS_PIN_STATUS
	S_PIN,   // 读取引脚IO电平状态
#endif
} SysParams_t;

/**
 * @brief 电机参数设置命令结构体
 */
typedef struct {
	MotorParamType_t type;  // 设置类型

	union {
#if MOTOR_SET_MOTOR_ID
		struct {
			uint8_t id;     // 电机ID, 范围:1-255(0为广播地址)
			bool save;      // 是否存储到内存: true=存储, false=不存储
		} motor_id;
#endif

#if MOTOR_SET_MICRO_STEP
		struct {
			uint8_t step;   // 细分值: 1/2/4/8/16/32/64/128/256
		} micro_step;
#endif

#if MOTOR_SET_POWER_FLAG
		struct {
			bool enable;    // 上电默认为true，修改为false可知是否掉电
		} power_flag;
#endif

#if MOTOR_SET_MOTOR_TYPE
		struct {
			bool type;      // false=1.8°步进电机, true=0.9°步进电机
			bool save;      // 是否存储到内存: true=存储, false=不存储
		} motor_type;
#endif

#if MOTOR_SET_FIRMWARE_TYPE
		struct {
			bool type;      // false=X固件, true=Emm固件
			bool save;      // 是否存储到内存: true=存储, false=不存储
		} firmware_type;
#endif

#if MOTOR_SET_OPENLOOP_CURRENT
		struct {
			uint16_t current;   // 开环工作电流, 单位:mA, 范围:0-5000
			bool save;          // 是否存储到内存: true=存储, false=不存储
		} openloop_current;
#endif

#if MOTOR_SET_CLOSEDLOOP_CURRENT
		struct {
			uint16_t current;   // 闭环最大电流, 单位:mA, 范围:0-5000, 默认3000
			bool save;          // 是否存储到内存: true=存储, false=不存储
		} closedloop_current;
#endif

#if MOTOR_SET_CONTROL_MODE
		struct {
			bool mode;      // false=开环控制, true=闭环FOC控制
			bool save;      // 是否存储到内存: true=存储, false=不存储
		} control_mode;
#endif

#if MOTOR_SET_DIRECTION
		struct {
			bool dir;       // false=正转(CW), true=反转(CCW)
			bool save;      // 是否存储到内存: true=存储, false=不存储
		} direction;
#endif

#if MOTOR_SET_LOCK_KEY
		struct {
			bool lock;      // true=锁定按键, false=解锁按键
			bool save;      // 是否存储到内存: true=存储, false=不存储
		} lock_key;
#endif

#if MOTOR_SET_SCALE_INPUT
		struct {
			bool enable;    // true=启用缩小倍数, false=禁用
			bool save;      // 是否存储到内存: true=存储, false=不存储
		} scale_input;
#endif

#if MOTOR_SET_LOCK_PARAMS
		struct {
			uint8_t level;  // 锁定等级: 0=解锁, 1=禁止修改通讯参数,
			                //           2=禁止修改所有参数+触发校准, 3=同2级
			bool save;      // 是否存储到内存: true=存储, false=不存储
		} lock_params;
#endif

#if MOTOR_POS_WINDOW_WRITE
		struct {
			uint16_t window;    // 位置到达窗口值, 单位:0.1°
			bool save;          // 是否存储到内存: true=存储, false=不存储
		} pos_window;
#endif

#if MOTOR_PID_WRITE
#if CURRENT_FIRMWARE == FIRMWARE_EMM
		struct {
			uint32_t kp;    // 比例系数Kp
			uint32_t ki;    // 积分系数Ki
			uint32_t kd;    // 微分系数Kd
			bool save;
		} pid;
#elif CURRENT_FIRMWARE == FIRMWARE_X
		struct {
			uint32_t trapezoidal_kp;    // 梯形曲线位置环Kp
			uint32_t direct_kp;        // 直通限速位置环Kp
			uint32_t vel_kp;           // 速度环Kp
			uint32_t vel_ki;           // 速度环Ki
			bool save;
		} pid;
#endif
#endif

#if MOTOR_INTEGRAL_LIMIT_WRITE
		struct {
			uint32_t value; 	// 积分限幅/刚性系数值
			bool save;      	// 是否存储到内存: true=存储, false=不存储
		} integral_limit;
#endif

#if MOTOR_PROTECT_THRESHOLD_WRITE
		struct {
			uint16_t temp;      // 过热保护阈值, 单位:℃
			uint16_t current;   // 过流保护阈值, 单位:mA
			uint16_t time_ms;   // 保护检测时间, 单位:ms
			bool save;          // 是否存储到内存: true=存储, false=不存储
		} protect_threshold;
#endif

#if MOTOR_HEARTBEAT_WRITE
		struct {
			uint32_t time_ms;   // 心跳保护时间, 单位:ms, 默认值:0(关闭)
			bool save;          // 是否存储到内存: true=存储, false=不存储
		} heartbeat;
#endif

#if MOTOR_COLLISION_ANGLE_WRITE
		struct {
			uint16_t angle;     // 碰撞回零返回角度, 单位:0.1° 0自适应
			bool save;          // 是否存储到内存: true=存储, false=不存储
		} collision_angle;
#endif

#if MOTOR_DMX512_WRITE
		struct {
			uint16_t tch;       // 目标通道/每个电机占用通道数
			uint8_t nch;        // 总通道数
			uint8_t mode;       // 控制模式: 0=位置, 1=速度, 2=力矩
			uint16_t vel;       // 单通道模式运动速度, 单位:RPM
			uint16_t acc;       // 加速度
			uint16_t vel_step;  // 双通道模式速度步长
			uint32_t pos_step;  // 双通道模式运动步长
			bool save;          // 是否存储到内存: true=存储, false=不存储
		} dmx512;
#endif

#if MOTOR_DRIVER_HOME_WRITE
		struct {
			uint8_t mode;       // 回零模式: 0=单圈就近, 1=单圈方向, 2=无限位碰撞,
			                    //           3=限位回零, 4=回到绝对坐标零点, 5=回到上次掉电位置
			uint8_t dir;        // 回零方向: 0=CW(正转), 其他=CCW(反转)
			uint16_t vel;       // 回零速度, 单位:RPM
			uint32_t timeout;   // 回零超时时间, 单位:ms
			uint16_t sl_vel;    // 无限位碰撞回零检测转速, 单位:RPM
			uint16_t sl_current;// 无限位碰撞回零检测电流, 单位:mA
			uint16_t sl_time;   // 无限位碰撞回零检测时间, 单位:ms
			bool auto_home;     // 上电自动触发回零: true=使能, false=不使能
			bool save;          // 是否存储到内存: true=存储, false=不存储
		} home_params;
#endif

#if MOTOR_PERIODIC_RETURN
		struct {
			SysParams_t param;  // 定时返回的参数类型
			uint16_t time_ms;   // 定时时间, 单位:ms, 0=关闭定时返回
		} periodic_return;
#endif

		struct {
			bool dummy;  		// 空成员保证编译
		} dummy;
	} p;
} MotorParamWrite_t;

/**
 * @brief 驱动配置参数枚举
 */
typedef enum {
	D_NULL = 0, // 无指令
#if MOTOR_DRIVER_READ_BATCH
	D_BATCH,	// 批量读取
#endif
#if MOTOR_DRIVER_POS_WINDOW
	D_POS_WINDOW,     // 位置到达窗口
#endif
#if MOTOR_DRIVER_HOME
	D_HOME,           // 读取回零参数
#endif
} DriverParams_t;

/**
 * @brief 电机控制参数枚举
 */
typedef enum {
	C_NULL = 0, // 无指令
#if MOTOR_PID_READ
	C_PID,            // 读取PID参数
#endif
#if MOTOR_INTEGRAL_LIMIT_READ
	C_INTEGRAL_LIMIT, // 读取积分限幅/刚性系数
#endif
#if MOTOR_PROTECT_THRESHOLD_READ
	C_PROTECT_THRESHOLD, // 读取过热过流保护阈值
#endif
#if MOTOR_COLLISION_ANGLE_READ
	C_COLLISION_ANGLE, // 读取碰撞回零返回角度
#endif
#if MOTOR_HEARTBEAT_READ
	C_HEARTBEAT,      // 读取心跳保护时间
#endif
} CtrlParams_t;

/**
 * @brief 设备信息与特殊功能枚举
 */
typedef enum {
	I_NULL = 0, // 无指令
#if MOTOR_READ_VERSION
	I_VERSION,        // 读取固件版本和硬件版本
#endif
#if MOTOR_READ_PHASE_PARAMS
	I_PHASE_PARAMS,   // 读取相电阻和相电感
#endif
#if MOTOR_READ_OPTION_PARAMS
	I_OPTION,         // 读取选项参数状态
#endif
#if MOTOR_BROADCAST_READ_ID
	I_ID,             // 广播读取ID地址
#endif
#if MOTOR_DRIVER_DMX512
	I_DMX512,         // DMX512协议参数
#endif
} DeviceInfo_t;

/**
 * @brief 参数读取命令结构体
 */
typedef struct {
	MotorParam_t type;        	// 参数类型

	union {
		SysParams_t sys;        // 系统状态参数
		DriverParams_t drv;     // 驱动配置参数
		CtrlParams_t ctrl;      // 控制参数（PID/保护/回零）
		DeviceInfo_t info;      // 设备信息与特殊功能
	} p;
} MotorParamRead_t;

/**
 * @brief 电机命令结构体
 */
typedef struct {
	MotorOpType_t op_type;      // 操作类型
	uint8_t motor_id;           // 电机ID, 范围:1-255(0为广播地址)

	union {
		MotorCtrl_t ctrl;       // 运动控制命令
		MotorTrigger_t trigger; // 触发动作命令
		MotorParamWrite_t write;// 电机参数设置
		MotorParamRead_t read;  // 参数读取
	} type;
} MotorCmd_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __ZDT_V5_CMD_H__ */
