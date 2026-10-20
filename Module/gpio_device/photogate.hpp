/**
 * @file photogate.hpp
 * @author lxlx (1729649497@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-10-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#pragma once
#include "gpio.h"

class PhotoGate{
public:
    PhotoGate(GPIO_TypeDef* GPIO_Port, uint16_t GPIO_Pin)
        : gpio_port_(GPIO_Port), gpio_pin_(GPIO_Pin){}

    bool is_blocked(){ return state_; }
    
    void update(){
        state_ = 
        (HAL_GPIO_ReadPin(gpio_port_, gpio_pin_) == GPIO_PIN_RESET)
        ? false
        : true;
    }


private:
    bool state_{false};
    GPIO_TypeDef* gpio_port_;
    uint16_t gpio_pin_;

};