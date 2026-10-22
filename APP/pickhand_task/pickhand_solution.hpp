/**
 * @file pickhand_solution.hpp
 * @author lxlx (1729649497@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-10-22
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include "math_utils.hpp"

#include "djiMotor.hpp"
#include "dmMotor.hpp"

#include "topic_pool.h"

class PickHand{
public:
    PickHand(C620Motor* lift_motor, C620Motor* tripod_motor,
        C610Motor* extend_motor, DM4310Motor* wrist_motor) :
        lift_motor_(lift_motor), tripod_motor_(tripod_motor),
        extend_motor_(extend_motor), wrist_motor_(wrist_motor){}

    void run(const pub_pickhand_cmd &cmd){

        lift_motor_->setMotorDeg(cmd.lift_deg);
        tripod_motor_->setMotorDeg(cmd.tripod_deg);
        extend_motor_->setMotorDeg(cmd.extend_deg);
        wrist_motor_->setMotorDeg(cmd.wrist_deg);
        
    }

private:
    C620Motor* lift_motor_{};
    C620Motor* tripod_motor_{};
    C610Motor* extend_motor_{};
    DM4310Motor* wrist_motor_{};


};