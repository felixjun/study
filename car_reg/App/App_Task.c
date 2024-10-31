#include "App_Task.h"

#define START_STACK_DEPTH 256
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle;
void Start_Task(void *pvParameters);

/* OLED��ʾ�������� */
#define DISPLAY_STACK_DEPTH 256
#define DISPLAY_TASK_PRIORITY 4
TaskHandle_t display_task_handle;
void App_Task_Display(void *pvParameters);

/* ��ȡ�Ƕȡ�����ֵ�������� */
#define ANGLE_STACK_DEPTH 256
#define ANGLE_TASK_PRIORITY 4
TaskHandle_t angle_task_handle;
void App_Task_GetAngle(void *pvParameters);

/* PID�������� */
#define PID_STACK_DEPTH 256
#define PID_TASK_PRIORITY 4
TaskHandle_t pid_task_handle;
void App_Task_PID(void *pvParameters);

/**
 * @description: ��ں���������������������������
 * @return {*}
 */
void App_Task_Init(void)
{
    /* 1. ������������ */
    xTaskCreate((TaskFunction_t)Start_Task,
                (char *)"Start_Task",
                (configSTACK_DEPTH_TYPE)START_STACK_DEPTH,
                (void *)NULL,
                (UBaseType_t)START_TASK_PRIORITY,
                (TaskHandle_t *)&start_task_handle);

    /* 2. ���������� */
    vTaskStartScheduler();
}

/**
 * @description: �������񣬴�����������
 * @param {void *} pvParameters
 * @return {*}
 */
void Start_Task(void *pvParameters)
{
    taskENTER_CRITICAL();
    /* ������ʾ���� */
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
 * @description: OLED��ʾ�����ٶȡ��Ƕȡ���ص�ѹ
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
        vTaskDelayUntil(&pxPreviousWakeTime, 50); /* ÿ50ms��ʾһ�νǶȡ������� */
    }
}

/**
 * @description: ÿ10ms��ͬMPU����Ƶ�ʣ���ȡ������ ��б�Ƕ� �� ��������ֵ
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
 * @description: ����PID����
 * @param {void} *pvParameters
 * @return {*}
 */
void App_Task_PID(void *pvParameters)
{
    while (1)
    {
        /* һ����ȡ��Ƕȣ��Ϳ�ʼ����pid���� */
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        App_Car_PID();
    }
    
}

