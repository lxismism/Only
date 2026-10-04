/**
 * @file pid_degspeed.cpp
 * @author lxlx (1729649497@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-10-04
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "pid_degspeed.h"
#include "topics.hpp"
#include "Motor.hpp"
#include "com_config.h"
#include "pid_controller.h"
#include "topic_pool.h"

osThreadId_t pidDegSpeed_TaskHandle;

extern C620Motor chassis_motor1;

static TypedTopicPublisher<pub_chassis_cmd> chassis_cmd_pub("chassis_cmd");
static pub_chassis_cmd chassis_cmd_msg{};


void pidDegSpeedTask(void *argument)
{
    TickType_t currentTime = xTaskGetTickCount();
    

    for(;;)
    {
        chassis_cmd_msg.chassis_motor1_cmd = PID_Calculate(chassis_motor1.getDegSpeedPID(), 
            chassis_motor1.getDegSpeed(), chassis_motor1.getRefDegSpeed());

        chassis_cmd_pub.Publish(chassis_cmd_msg);
        vTaskDelayUntil(&currentTime, 1);
    }

}