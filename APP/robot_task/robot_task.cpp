/**
 * @file robot_task.cpp
 * @author lxlx (1729649497@qq.com)
 * @brief 任务管理，创建所有任务
 * @version 0.1
 * @date 2026-09-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "robot_task.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "debug_task.h"

extern osThreadId_t Debug_TaskHandle;

void osTaskInit(void)
{
    const osThreadAttr_t DebugTaskHandle_attributes = {
        .name = "Debug_TaskHandle",
        .stack_size = 256 * 4,
        .priority = (osPriority_t)osPriorityNormal,
    };
    Debug_TaskHandle = osThreadNew(debugTask, NULL, &DebugTaskHandle_attributes);

}
