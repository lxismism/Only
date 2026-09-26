/**
 * @file debug_task1.cpp
 * @author lxlx (1729649497@qq.com)
 * @brief APP调试层，专门用来测试程序
 * @version 0.1
 * @date 2026-09-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "debug1_task.h"
#include "cmsis_os2.h"
#include "memory_map.h"
#include "double_buffer.hpp"
#include "lockfree_queue.hpp"
#include "UartPort.hpp"
#include "topics.hpp"

osThreadId_t Debug1_TaskHandle;


static TypedTopicPublisher<debug_data_t> debug_data("debug");
static debug_data_t debug_d{}; 


void debug1Task(void *argument)
{
    TickType_t currentTime;
    currentTime = xTaskGetTickCount();
    
    for(;;)
    {
        debug_d.a +=2;
        debug_d.b +=4;
        debug_d.c +=8;
        debug_data.Publish(debug_d);
        vTaskDelayUntil(&currentTime, 1000);
    }

}