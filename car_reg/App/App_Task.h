#ifndef __APP_TASK_H
#define __APP_TASK_H
#include "FreeRTOS.h"
#include "task.h"
#include "App_Car.h"

void App_Task_Init(void);
void Start_Task(void *pvParameters);
void App_Task_Display(void *pvParameters);
void App_Task_GetAngle(void *pvParameters);
void App_Task_PID(void *pvParameters);
#endif
