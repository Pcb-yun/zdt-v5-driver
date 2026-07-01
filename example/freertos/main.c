/**
 * @file main.c
 * @brief 张大头V5步进电机 FreeRTOS 使用示例
 *
 * 架构说明：
 *   Ctrl_Task    — 命令处理任务，从命令队列接收并执行电机命令
 *   Recv_Task    — 接收处理任务，从串口接收队列获取数据并解析
 *   Update_Task  — 状态更新任务，定时轮询所有电机状态
 */

#include "zdt_v5_drv.h"
#if !ONLY_DRIVER
#include "zdt_v5_engine.h"
#endif
#include "cmsis_os2.h"
#include <string.h>

#if !ONLY_DRIVER
/* ======================== 电机状态 ======================== */

static MotorStatus_t motors[MOTOR_NUM];

/* ======================== RTOS 资源 ======================== */

static osMessageQueueId_t cmd_queue;		// 电机命令队列

/* 串口接收数据结构 */
typedef struct {
	uint8_t data[64];
	uint8_t len;
} RxBuf_t;

static osMessageQueueId_t rx_queue;			// 串口接收数据队列

/* ======================== 命令发送接口 ======================== */

static bool Motor_Send_Cmd(MotorCmd_t *cmd) {
	return osMessageQueuePut(cmd_queue, cmd, 0, 50) == osOK;
}

/* ======================== 任务函数 ======================== */

/**
 * @brief 命令处理任务 — 从队列取命令并执行
 */
void Ctrl_Task(void *argument) {
	(void)argument;
	MotorCmd_t cmd;

	for (;;) {
		if (osMessageQueueGet(cmd_queue, &cmd, NULL, osWaitForever) == osOK) {
			ZDT_V5_Process_Cmd(&cmd);
			osDelay(2);		// 防止粘包
		}
	}
}

/**
 * @brief 接收处理任务 — 从串口队列取数据并解析
 */
void Recv_Task(void *argument) {
	(void)argument;
	RxBuf_t rx_buf;

	for (;;) {
		if (osMessageQueueGet(rx_queue, &rx_buf, NULL, osWaitForever) == osOK) {
			ZDT_V5_Receive(rx_buf.data, rx_buf.len);
		}
	}
}

/**
 * @brief 状态更新任务 — 定时轮询所有电机状态
 */
void Update_Task(void *argument) {
	(void)argument;
	MotorCmd_t cmd;
	uint32_t interval = 50;	// 50ms 轮询一次

	osDelay(100);	// 等待系统就绪

	for (;;) {
		for (uint8_t i = 0; i < MOTOR_NUM; i++) {
			if (motors[i].motor_id == 0) continue;	// 跳过未注册的槽位
			cmd.motor_id = motors[i].motor_id;
			cmd.op_type = OP_PARAM_READ;
			cmd.type.read.type = MP_SYS;
			cmd.type.read.p.sys = S_CPOS;	// 读取实时位置
			Motor_Send_Cmd(&cmd);

			cmd.type.read.p.sys = S_VEL;	// 读取实时转速
			Motor_Send_Cmd(&cmd);
		}
		osDelay(interval);
	}
}

/* ======================== 系统初始化 ======================== */

/**
 * @brief 初始化电机系统
 */
static void Motor_Init(void) {
	/* 初始化队列 */
	cmd_queue = osMessageQueueNew(16, sizeof(MotorCmd_t), NULL);
	rx_queue = osMessageQueueNew(8, sizeof(RxBuf_t), NULL);

	/* 注册电机到引擎层（ID 可任意，无需连续） */
	ZDT_V5_Register_Motor(1, &motors[0]);

	/* 初始化串口（HAL 库函数）*/
	MX_USART6_UART_Init();

	/* 启动 DMA 接收 */
	extern uint8_t rx6Buffer[128];
	HAL_UARTEx_ReceiveToIdle_DMA(&huart6, rx6Buffer, 128);
}

/* ======================== 应用层使用示例（引擎层方式）======================== */

/**
 * @brief 应用任务 — 演示电机控制流程
 *
 * 用户可根据需求在此基础上封装更高级的 API。
 */
static void app_demo_enable(uint8_t id, bool enable) {
	MotorCmd_t cmd = {0};
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	cmd.type.ctrl.type = CTRL_ENABLE;
	cmd.type.ctrl.p.en.enable = enable;
	Motor_Send_Cmd(&cmd);
}

static void app_demo_velocity(uint8_t id, uint8_t dir, uint16_t vel, uint16_t acc) {
	MotorCmd_t cmd = {0};
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	cmd.type.ctrl.type = CTRL_VELOCITY;
	cmd.type.ctrl.p.vel.dir = dir;
	cmd.type.ctrl.p.vel.vel = vel;
	cmd.type.ctrl.p.vel.acc = acc;
	Motor_Send_Cmd(&cmd);
}

static void app_demo_stop(uint8_t id) {
	MotorCmd_t cmd = {0};
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	cmd.type.ctrl.type = CTRL_STOP;
	Motor_Send_Cmd(&cmd);
}

static void app_demo_position(uint8_t id, uint8_t dir, uint16_t vel, uint16_t acc, int32_t target, uint8_t mode) {
	MotorCmd_t cmd = {0};
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	cmd.type.ctrl.type = CTRL_POSITION;
	cmd.type.ctrl.p.pos.dir = dir;
	cmd.type.ctrl.p.pos.vel = vel;
	cmd.type.ctrl.p.pos.acc = acc;
	cmd.type.ctrl.p.pos.target = target;
	cmd.type.ctrl.p.pos.mode = mode;
	Motor_Send_Cmd(&cmd);
}

void App_Task(void *argument) {
	(void)argument;

	osDelay(500);	// 等待系统就绪

	/* 使能电机 1 */
	app_demo_enable(1, true);
	osDelay(100);

	/* 速度模式：正转 500 RPM，加速度 200 */
	app_demo_velocity(1, 0, 500, 200);
	osDelay(3000);

	/* 停止 */
	app_demo_stop(1);
	osDelay(500);

	/* 位置模式：相对运动 +2000 脉冲，速度 300 RPM */
	app_demo_position(1, 0, 300, 200, 2000, 0);

	for (;;) {
		osDelay(1000);
	}
}

/* ======================== 系统入口 ======================== */

/**
 * @brief 创建任务并启动调度器
 */
int main(void) {
	HAL_Init();
	SystemClock_Config();
	Motor_Init();

	osKernelInitialize();

	osThreadNew(Ctrl_Task, NULL, NULL);
	osThreadNew(Recv_Task, NULL, NULL);
	osThreadNew(Update_Task, NULL, NULL);
	osThreadNew(App_Task, NULL, NULL);

	osKernelStart();

	for (;;);
}

/* ======================== 移植接口实现 ======================== */

void zdt_v5_port_send(uint8_t *cmd, uint8_t len) {
	extern UART_HandleTypeDef huart6;
	HAL_UART_Transmit(&huart6, cmd, len, 100);
}

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

/* 串口 DMA 接收完成回调 — 由 HAL 中断调用 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	if (huart->Instance == USART6) {
		extern uint8_t rx6Buffer[128];
		RxBuf_t rx_buf;
		memcpy(rx_buf.data, rx6Buffer, Size);
		rx_buf.len = Size;

		/* 将接收数据放入队列（ISR 安全版本）*/
		osMessageQueuePut(rx_queue, &rx_buf, 0, 0);

		/* 重新启动 DMA 接收 */
		HAL_UARTEx_ReceiveToIdle_DMA(&huart6, rx6Buffer, 128);
	}
}

#else

/* ======================== ONLY_DRIVER 模式：直接调用驱动层 API ======================== */

/**
 * @brief 应用任务 — 演示电机控制流程（驱动层方式）
 */
void App_Task(void *argument) {
	(void)argument;

	osDelay(500);	// 等待系统就绪

	/* 使能电机 1 */
	ZDT_V5_En_Control(1, true, false);
	osDelay(100);

	/* 速度模式：正转 500 RPM，加速度 200 */
	ZDT_V5_Vel_Control(1, 0, 500, 200, false);
	osDelay(3000);

	/* 停止 */
	ZDT_V5_Stop(1, false);
	osDelay(500);

	/* 位置模式：相对运动 +2000 脉冲，速度 300 RPM */
	ZDT_V5_Pos_Control(1, 0, 300, 200, 0, 2000, false, false);

	for (;;) {
		osDelay(1000);
	}
}

/* ======================== 系统入口 ======================== */

int main(void) {
	HAL_Init();
	SystemClock_Config();

	/* 初始化串口（HAL 库函数）*/
	MX_USART6_UART_Init();

	/* 启动 DMA 接收 */
	extern uint8_t rx6Buffer[128];
	HAL_UARTEx_ReceiveToIdle_DMA(&huart6, rx6Buffer, 128);

	osKernelInitialize();

	osThreadNew(App_Task, NULL, NULL);

	osKernelStart();

	for (;;);
}

/* ======================== 移植接口实现 ======================== */

void zdt_v5_port_send(uint8_t *cmd, uint8_t len) {
	extern UART_HandleTypeDef huart6;
	HAL_UART_Transmit(&huart6, cmd, len, 100);
}

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

#endif