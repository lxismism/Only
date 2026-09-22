/**
 * @file debug_task.cpp
 * @author lxlx (1729649497@qq.com)
 * @brief APP调试层，专门用来测试程序
 * @version 0.1
 * @date 2026-09-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "debug_task.h"
#include "cmsis_os2.h"
#include "memory_map.h"
#include "double_buffer.hpp"
#include "lockfree_queue.hpp"
#include "UartPort.hpp"
#include "topics.hpp"

osThreadId_t Debug_TaskHandle;

void debugTask(void *argument)
{
    TickType_t currentTime;
    currentTime = xTaskGetTickCount();
    
    for(;;)
    {
        vTaskDelayUntil(&currentTime, 1);
    }

}