#include "App_Task.h"

#define START_STACK_DEPTH 256
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle;
void Start_Task(void *pvParameters);

/* OLED显示任务配置 */
#define DISPLAY_STACK_DEPTH 256
#define DISPLAY_TASK_PRIORITY 4
TaskHandle_t display_task_handle;
void App_Task_Display(void *pvParameters);

/* 获取角度、编码值任务配置 */
#define ANGLE_STACK_DEPTH 256
#define ANGLE_TASK_PRIORITY 4
TaskHandle_t angle_task_handle;
void App_Task_GetAngle(void *pvParameters);

/* PID控制任务 */
#define PID_STACK_DEPTH 256
#define PID_TASK_PRIORITY 4
TaskHandle_t pid_task_handle;
void App_Task_PID(void *pvParameters);

/**
 * @description: 入口函数：创建启动任务、启动调度器
 * @return {*}
 */
void App_Task_Init(void)
{
    /* 1. 创建启动任务 */
    xTaskCreate((TaskFunction_t)Start_Task,
                (char *)"Start_Task",
                (configSTACK_DEPTH_TYPE)START_STACK_DEPTH,
                (void *)NULL,
                (UBaseType_t)START_TASK_PRIORITY,
                (TaskHandle_t *)&start_task_handle);

    /* 2. 启动调度器 */
    vTaskStartScheduler();
}

/**
 * @description: 启动任务，创建其他任务
 * @param {void *} pvParameters
 * @return {*}
 */
void Start_Task(void *pvParameters)
{
    taskENTER_CRITICAL();
    /* 创建显示任务 */
    xTaskCreate((TaskFunction_t)App_Task_Display,
                (char *)"App_Task_Display",
                (configSTACK_DEPTH_TYPE)DISPLAY_STACK_DEPTH,
                (void *)NULL,
                (UBaseType_t)DISPLAY_TASK_PRIORITY,
                (TaskHandle_t *)&display_task_handle);
    xTaskCreate((TaskFunction_t)App_Task_GetAngle,
                (char *)"App_Task_GetAngle",
                (configSTACK_DEPTH_TYPE)ANGLE_STACK_DEPTH,
                (void *)NULL,
                (UBaseType_t)ANGLE_TASK_PRIORITY,
                (TaskHandle_t *)&angle_task_handle);
    xTaskCreate((TaskFunction_t)App_Task_PID,
                (char *)"App_Task_PID",
                (configSTACK_DEPTH_TYPE)PID_STACK_DEPTH,
                (void *)NULL,
                (UBaseType_t)PID_TASK_PRIORITY,
                (TaskHandle_t *)&pid_task_handle);
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();
}

/**
 * @description: OLED显示任务：速度、角度、电池电压
 * @param {void *} pvParameters
 * @return {*}
 */
void App_Task_Display(void *pvParameters)
{

    TickType_t pxPreviousWakeTime;
    pxPreviousWakeTime = xTaskGetTickCount();

    while (1)
    {
        App_Car_Display();
        vTaskDelayUntil(&pxPreviousWakeTime, 50); /* 每50ms显示一次角度、编码器 */
    }
}

/**
 * @description: 每10ms（同MPU采样频率）获取并计算 倾斜角度 和 编码器的值
 * @param {void *} pvParameters
 * @return {*}
 */
void App_Task_GetAngle(void *pvParameters)
{
    TickType_t pxPreviousWakeTime;
    pxPreviousWakeTime = xTaskGetTickCount();

    while (1)
    {
        App_Car_GetAngle();
        xTaskNotifyGive(pid_task_handle);
        vTaskDelayUntil(&pxPreviousWakeTime, 10);
    }
}

/**
 * @description: 进行PID控制
 * @param {void} *pvParameters
 * @return {*}
 */
void App_Task_PID(void *pvParameters)
{
    while (1)
    {
        /* 一旦获取完角度，就开始进行pid控制 */
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        App_Car_PID();
    }
    
}

