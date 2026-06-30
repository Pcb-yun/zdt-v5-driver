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
 *
 * 注意：此示例为最小演示，实际使用时需根据平台实现
 * zdt_v5_port_send() 函数和串口接收中断/DMA。
 */

#include "zdt_v5_drv.h"
#include "zdt_v5_engine.h"

/* 电机状态数组（由引擎层使用） */
static MotorStatus_t motors[MOTOR_NUM];

/* 串口接收缓冲区 */
#define RX_BUF_SIZE 128
static uint8_t rx_buf[RX_BUF_SIZE];
static uint8_t rx_len = 0;

/* 模拟串口接收回调 — 由中断/DMA 调用 */
void uart_rx_callback(uint8_t *data, uint8_t len) {
	ZDT_V5_Receive(data, len, motors);
}

/* 简单的延时函数（阻塞，可根据平台替换） */
static void delay_ms(uint32_t ms) {
	for (volatile uint32_t i = 0; i < ms * 1000; i++);
}

/* 发送电机命令的辅助函数 */
static void motor_send_cmd(MotorCmd_t *cmd) {
	ZDT_V5_Process_Cmd(cmd);
}

/* 使能指定电机 */
static void motor_enable(uint8_t id, bool enable) {
	MotorCmd_t cmd;
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	cmd.type.ctrl.type = CTRL_ENABLE;
	cmd.type.ctrl.p.en.enable = enable;
	cmd.type.ctrl.p.en.sync = false;
	motor_send_cmd(&cmd);
}

/* 速度模式 */
static void motor_velocity(uint8_t id, uint8_t dir, uint16_t vel, uint16_t acc) {
	MotorCmd_t cmd;
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	cmd.type.ctrl.type = CTRL_VELOCITY;
	cmd.type.ctrl.p.vel.dir = dir;
	cmd.type.ctrl.p.vel.vel = vel;
	cmd.type.ctrl.p.vel.acc = acc;
	cmd.type.ctrl.p.vel.sync = false;
	motor_send_cmd(&cmd);
}

/* 位置模式（梯形曲线） */
static void motor_position(uint8_t id, uint8_t dir, uint16_t vel, uint16_t acc, int32_t target, uint8_t mode) {
	MotorCmd_t cmd;
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	cmd.type.ctrl.type = CTRL_POSITION;
	cmd.type.ctrl.p.pos.dir = dir;
	cmd.type.ctrl.p.pos.vel = vel;
	cmd.type.ctrl.p.pos.acc = acc;
	cmd.type.ctrl.p.pos.target = target;
	cmd.type.ctrl.p.pos.mode = mode;
	cmd.type.ctrl.p.pos.sync = false;
	motor_send_cmd(&cmd);
}

/* 立即停止电机 */
static void motor_stop(uint8_t id) {
	MotorCmd_t cmd;
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	cmd.type.ctrl.type = CTRL_STOP;
	cmd.type.ctrl.p.stop.sync = false;
	motor_send_cmd(&cmd);
}

/* 触发编码器校准 */
static void motor_calibrate(uint8_t id) {
	MotorCmd_t cmd;
	cmd.op_type = OP_TRIGGER;
	cmd.motor_id = id;
	cmd.type.trigger.type = TRIG_ENCODER_CALIB;
	motor_send_cmd(&cmd);
}

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

	/***** 主循环 *****/
	while (1) {
		/* 处理串口接收数据（由中断填充 rx_buf/rx_len）*/
		if (rx_len > 0) {
			ZDT_V5_Receive(rx_buf, rx_len, motors);
			rx_len = 0;
		}

		/* 用户的其他逻辑 */
	}
}
