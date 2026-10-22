/**
 * @file pickhand_task.cpp
 * @author lxlx (1729649497@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-10-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "pickhand_task.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"


#include "topic_pool.h"

osThreadId_t pickHand_TaskHandle;

void pickHandTask(void *argument){
    TickType_t current = xTaskGetTickCount();

    for(;;){

        vTaskDelayUntil(&current, 5);
    }
}