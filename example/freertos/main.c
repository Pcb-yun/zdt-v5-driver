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
#if !ZDT_ONLY_DRIVER
#include "zdt_v5_engine.h"
#endif
#include "cmsis_os2.h"
#include <string.h>

#if !ZDT_ONLY_DRIVER
/* ======================== 电机状态 ======================== */

static MotorStatus_t motors[ZDT_STEP_NUM];

/* ======================== RTOS 资源 ======================== */

static osMessageQueueId_t cmd_queue;

typedef struct {
	uint8_t data[64];
	uint8_t len;
} RxBuf_t;

static osMessageQueueId_t rx_queue;

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
			osDelay(2);
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
	MotorParamRead_t read;
	uint32_t interval = 50;

	osDelay(100);

	for (;;) {
		for (uint8_t i = 0; i < ZDT_STEP_NUM; i++) {
			if (motors[i].motor_id == 0) continue;

			read.type = MP_SYS;
			read.p.sys = S_CPOS;

			cmd.motor_id = motors[i].motor_id;
			cmd.op_type = OP_PARAM_READ;
			cmd.type.read = read;
			Motor_Send_Cmd(&cmd);

			read.p.sys = S_VEL;
			cmd.type.read = read;
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
	cmd_queue = osMessageQueueNew(16, sizeof(MotorCmd_t), NULL);
	rx_queue = osMessageQueueNew(8, sizeof(RxBuf_t), NULL);

	ZDT_V5_Register_Motor(1, &motors[0]);
	ZDT_V5_Register_Motor(2, &motors[1]);

	MX_USART6_UART_Init();

	extern uint8_t rx6Buffer[128];
	HAL_UARTEx_ReceiveToIdle_DMA(&huart6, rx6Buffer, 128);
}

/* ======================== 应用层使用示例（引擎层方式）======================== */

static void app_demo_enable(uint8_t id, bool enable) {
	MotorCmd_t cmd;
	MotorCtrl_t ctrl;
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	ctrl.type = CTRL_ENABLE;
	ctrl.p.en.enable = enable;
	ctrl.p.en.sync = false;
	cmd.type.ctrl = ctrl;
	Motor_Send_Cmd(&cmd);
}

static void app_demo_velocity(uint8_t id, uint8_t dir, uint16_t vel, uint16_t acc) {
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
	Motor_Send_Cmd(&cmd);
}

static void app_demo_stop(uint8_t id) {
	MotorCmd_t cmd;
	MotorCtrl_t ctrl;
	cmd.op_type = OP_CONTROL;
	cmd.motor_id = id;
	ctrl.type = CTRL_STOP;
	ctrl.p.stop.sync = false;
	cmd.type.ctrl = ctrl;
	Motor_Send_Cmd(&cmd);
}

static void app_demo_position(uint8_t id, uint8_t dir, uint16_t vel, uint16_t acc, int32_t target, uint8_t mode) {
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
	Motor_Send_Cmd(&cmd);
}

#if MOTOR_MULTI_CMD
static void app_demo_multi(void) {
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

	multi.type = MULTI_VEL;
	multi.p.vel.dir = 0;
	multi.p.vel.vel = 500;
	multi.p.vel.acc = 200;
	multi.p.vel.snF = false;
	ZDT_V5_Process_Multi_Cmd(1, &multi, &cmd);

	multi.type = MULTI_VEL;
	multi.p.vel.dir = 1;
	multi.p.vel.vel = 500;
	multi.p.vel.acc = 200;
	multi.p.vel.snF = false;
	ZDT_V5_Process_Multi_Cmd(2, &multi, &cmd);

	ZDT_V5_Multi_Send(&cmd);

#if MOTOR_MULTI_PTR_BUF
	vPortFree(multi_buf);
#endif
}
#endif

void App_Task(void *argument) {
	(void)argument;

	osDelay(500);

	app_demo_enable(1, true);
	osDelay(100);

	app_demo_velocity(1, 0, 500, 200);
	osDelay(3000);

	app_demo_stop(1);
	osDelay(500);

	app_demo_position(1, 0, 300, 200, 2000, 0);
	osDelay(2000);

#if MOTOR_MULTI_CMD
	app_demo_multi();
	osDelay(3000);
#endif

	for (;;) {
		osDelay(1000);
	}
}

/* ======================== 系统入口 ======================== */

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

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	if (huart->Instance == USART6) {
		extern uint8_t rx6Buffer[128];
		RxBuf_t rx_buf;
		memcpy(rx_buf.data, rx6Buffer, Size);
		rx_buf.len = Size;

		osMessageQueuePut(rx_queue, &rx_buf, 0, 0);

		HAL_UARTEx_ReceiveToIdle_DMA(&huart6, rx6Buffer, 128);
	}
}

#else

/* ======================== ZDT_ONLY_DRIVER 模式：直接调用驱动层 API ======================== */

#if MOTOR_MULTI_CMD
static void app_demo_multi(void) {
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

	ZDT_V5_Multi_Vel_Ctrl(&cmd, 1, 0, 500, 200, false);
	ZDT_V5_Multi_Vel_Ctrl(&cmd, 2, 1, 500, 200, false);

	ZDT_V5_Multi_Send(&cmd);

#if MOTOR_MULTI_PTR_BUF
	vPortFree(multi_buf);
#endif
}
#endif

void App_Task(void *argument) {
	(void)argument;

	osDelay(500);

	ZDT_V5_En_Control(1, true, false);
	osDelay(100);

	ZDT_V5_Vel_Control(1, 0, 500, 200, false);
	osDelay(3000);

	ZDT_V5_Stop(1, false);
	osDelay(500);

	ZDT_V5_Pos_Control(1, 0, 300, 200, 0, 2000, false, false);
	osDelay(2000);

#if MOTOR_MULTI_CMD
	app_demo_multi();
	osDelay(3000);
#endif

	for (;;) {
		osDelay(1000);
	}
}

/* ======================== 系统入口 ======================== */

int main(void) {
	HAL_Init();
	SystemClock_Config();

	MX_USART6_UART_Init();

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
