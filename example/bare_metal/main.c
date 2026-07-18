/**
 * @file main.c
 * @brief 张大头V5步进电机裸机使用示例
 *
 * 示例功能：
 *   1. 初始化电机（串口、状态数组）
 *   2. 使能电机
 *   3. 发送速度控制命令
 *   4. 发送位置控制命令
 *   5. 主循环中处理串口接收数据
 *   6. 多机命令发送示例
 *
 * 注意：此示例为最小演示，实际使用时需根据平台实现
 * zdt_v5_port_send() 函数和串口接收中断/DMA。
 */

#include "zdt_v5_drv.h"
#if !ZDT_ONLY_DRIVER
#include "zdt_v5_engine.h"

/* 电机状态数组（由引擎层使用） */
static MotorStatus_t motors[ZDT_STEP_NUM];
#endif

/* 串口接收缓冲区 */
#define RX_BUF_SIZE 128
static uint8_t rx_buf[RX_BUF_SIZE];
static uint8_t rx_len = 0;

#if !ZDT_ONLY_DRIVER
/* 模拟串口接收回调 — 由中断/DMA 调用 */
void uart_rx_callback(uint8_t *data, uint8_t len) {
	ZDT_V5_Receive(data, len);
}

/* 发送电机命令的辅助函数 */
static void motor_send_cmd(MotorCmd_t *cmd) {
	ZDT_V5_Process_Cmd(cmd);
}

/* 使能指定电机 */
static void motor_enable(uint8_t id, bool enable) {
	MotorCmd_t cmd;
	MotorCtrl_t ctrl;
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	ctrl.type = CTRL_ENABLE;
	ctrl.p.en.enable = enable;
	ctrl.p.en.sync = false;
	cmd.type.ctrl = ctrl;
	motor_send_cmd(&cmd);
}

/* 速度模式 */
static void motor_velocity(uint8_t id, uint8_t dir, uint16_t vel, uint16_t acc) {
	MotorCmd_t cmd;
	MotorCtrl_t ctrl;
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	ctrl.type = CTRL_VEL;
	ctrl.p.vel.dir = dir;
	ctrl.p.vel.vel = vel;
	ctrl.p.vel.acc = acc;
	ctrl.p.vel.sync = false;
	cmd.type.ctrl = ctrl;
	motor_send_cmd(&cmd);
}

/* 位置模式（梯形曲线） */
static void motor_position(uint8_t id, uint8_t dir, uint16_t vel, uint16_t acc, int32_t target, uint8_t mode) {
	MotorCmd_t cmd;
	MotorCtrl_t ctrl;
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	ctrl.type = CTRL_POS;
	ctrl.p.pos.dir = dir;
	ctrl.p.pos.vel = vel;
	ctrl.p.pos.acc = acc;
	ctrl.p.pos.target = target;
	ctrl.p.pos.mode = mode;
	ctrl.p.pos.sync = false;
	cmd.type.ctrl = ctrl;
	motor_send_cmd(&cmd);
}

/* 立即停止电机 */
static void motor_stop(uint8_t id) {
	MotorCmd_t cmd;
	MotorCtrl_t ctrl;
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	ctrl.type = CTRL_STOP;
	ctrl.p.stop.sync = false;
	cmd.type.ctrl = ctrl;
	motor_send_cmd(&cmd);
}

/* 触发编码器校准 */
static void motor_calibrate(uint8_t id) {
	MotorCmd_t cmd;
	MotorTrigger_t trigger;
	cmd.op_type = OP_TRIGGER;
	cmd.motor_id = id;
	trigger.type = TRIG_ENCODER_CALIB;
	cmd.type.trigger = trigger;
	motor_send_cmd(&cmd);
}

#if MOTOR_MULTI_CMD
/* 多机命令示例：同时控制多个电机 */
static void motor_multi_demo(void) {
#if MOTOR_MULTI_PTR_BUF
	uint8_t *multi_buf = (uint8_t *)pvPortMalloc(256);
	if (!multi_buf) return;
	ZDT_V5_Multi_Cmd_t cmd = {
		.data = multi_buf,
		.used_len = 0,
		.buf_size = 256
	};
#else
	uint8_t multi_buf[256];
	ZDT_V5_Multi_Cmd_t cmd = {
		.data = multi_buf,
		.used_len = 0,
		.buf_size = sizeof(multi_buf)
	};
#endif

	ZDT_V5_Multi_Reset(&cmd);

	MotorMulti_t multi;

	/* 为电机 1 添加速度控制指令 */
	multi.type = MULTI_VEL;
	multi.p.vel.dir = 0;
	multi.p.vel.vel = 500;
	multi.p.vel.acc = 200;
	multi.p.vel.snF = false;
	ZDT_V5_Process_Multi_Cmd(1, &multi, &cmd);

	/* 为电机 2 添加速度控制指令 */
	multi.type = MULTI_VEL;
	multi.p.vel.dir = 1;
	multi.p.vel.vel = 500;
	multi.p.vel.acc = 200;
	multi.p.vel.snF = false;
	ZDT_V5_Process_Multi_Cmd(2, &multi, &cmd);

	/* 发送多机指令 */
	ZDT_V5_Multi_Send(&cmd);

#if MOTOR_MULTI_PTR_BUF
	vPortFree(multi_buf);
#endif
}
#endif
#else
/* ZDT_ONLY_DRIVER 模式：直接调用驱动层 API */

static void motor_enable(uint8_t id, bool enable) {
	ZDT_V5_En_Control(id, enable, false);
}

static void motor_velocity(uint8_t id, uint8_t dir, uint16_t vel, uint16_t acc) {
	ZDT_V5_Vel_Control(id, dir, vel, acc, false);
}

static void motor_position(uint8_t id, uint8_t dir, uint16_t vel, uint16_t acc, int32_t target, uint8_t mode) {
	ZDT_V5_Pos_Control(id, dir, vel, acc, 0, (uint32_t)target, (mode == 1), false);
}

static void motor_stop(uint8_t id) {
	ZDT_V5_Stop(id, false);
}

static void motor_calibrate(uint8_t id) {
	ZDT_V5_Trig_Encoder_Cal(id);
}

#if MOTOR_MULTI_CMD
/* 多机命令示例：直接调用驱动层 API */
static void motor_multi_demo(void) {
#if MOTOR_MULTI_PTR_BUF
	uint8_t *multi_buf = (uint8_t *)pvPortMalloc(256);
	if (!multi_buf) return;
	ZDT_V5_Multi_Cmd_t cmd = {
		.data = multi_buf,
		.used_len = 0,
		.buf_size = 256
	};
#else
	uint8_t multi_buf[256];
	ZDT_V5_Multi_Cmd_t cmd = {
		.data = multi_buf,
		.used_len = 0,
		.buf_size = sizeof(multi_buf)
	};
#endif

	ZDT_V5_Multi_Reset(&cmd);

	/* 为电机 1 添加速度控制指令 */
	ZDT_V5_Multi_Vel_Ctrl(&cmd, 1, 0, 500, 200, false);

	/* 为电机 2 添加速度控制指令 */
	ZDT_V5_Multi_Vel_Ctrl(&cmd, 2, 1, 500, 200, false);

	/* 发送多机指令 */
	ZDT_V5_Multi_Send(&cmd);

#if MOTOR_MULTI_PTR_BUF
	vPortFree(multi_buf);
#endif
}
#endif
#endif

/* 简单的延时函数（阻塞，可根据平台替换） */
static void delay_ms(uint32_t ms) {
	for (volatile uint32_t i = 0; i < ms * 1000; i++);
}

/* 移植接口实现（用户根据平台实现 zdt_v5_port_send）*/
/*
void zdt_v5_port_send(uint8_t *cmd, uint8_t len) {
	HAL_UART_Transmit(&huart6, cmd, len, 100);
}
*/

/* 日志输出函数（可选，启用时在 zdt_v5_port.h 中定义 ZDT_V5_LOG 指向此函数）*/
/*
#include <stdio.h>
#include <stdarg.h>

void zdt_v5_port_log(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}
*/

/**
 * @brief 主函数
 *
 * 使用流程：
 *   1. 初始化硬件
 *   2. 使能电机
 *   3. 执行运动控制
 *   4. 主循环处理串口接收
 */
int main(void) {
	/***** 硬件初始化（用户根据平台实现） *****/
	// uart_init(115200);          // 初始化串口
	// uart_set_rx_callback(uart_rx_callback); // 注册接收回调

#if !ZDT_ONLY_DRIVER
	/* 注册电机到引擎层（ID 可任意，无需连续） */
	ZDT_V5_Register_Motor(1, &motors[0]);
	ZDT_V5_Register_Motor(2, &motors[1]);
#endif

	/***** 系统启动 *****/

	/* 使能电机 1 */
	motor_enable(1, true);
	delay_ms(100);

	/* 速度模式：电机 1 正转 500 RPM，加速度 200 */
	motor_velocity(1, 0, 500, 200);

	/* 运行 3 秒 */
	delay_ms(3000);

	/* 停止 */
	motor_stop(1);
	delay_ms(500);

	/* 位置模式：电机 1 正转 + 2000 脉冲，速度 300 RPM */
	motor_position(1, 0, 300, 200, 2000, 0);

	/* 等待位置到达 */
	delay_ms(2000);

#if MOTOR_MULTI_CMD
	/* 多机命令示例：同时控制电机 1 和电机 2 */
	motor_multi_demo();
	delay_ms(3000);
#endif

	/***** 主循环 *****/
	while (1) {
#if !ZDT_ONLY_DRIVER
		/* 处理串口接收数据（由中断填充 rx_buf/rx_len）*/
		if (rx_len > 0) {
			ZDT_V5_Receive(rx_buf, rx_len);
			rx_len = 0;
		}
#endif

		/* 用户的其他逻辑 */
	}
}
