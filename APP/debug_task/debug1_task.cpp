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
#include "Canbus.hpp"
#include "Motor.hpp"
#include "com_config.h"
#include "pid_controller.h"
#include "topic_pool.h"


osThreadId_t Debug1_TaskHandle;

extern C620Motor chassis_motor1;

static TypedTopicPublisher<pub_chassis_cmd> chassis_cmd_pub("chassis_cmd");
static pub_chassis_cmd chassis_cmd_msg{};

PID_t chassis_motor1_pid{};
float ref_deg_speed = 0.0f;

void debug1Task(void *argument)
{
    TickType_t currentTime = xTaskGetTickCount();

    for(;;)
    {
        chassis_cmd_msg.chassis_motor1_cmd = PID_Calculate(&chassis_motor1_pid, 
            chassis_motor1.getDegSpeed(), ref_deg_speed);

        chassis_cmd_pub.Publish(chassis_cmd_msg);
        vTaskDelayUntil(&currentTime, 1);
    }

}