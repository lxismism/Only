/**
 * @file photo_gate.hpp
 * @author lxlx (1729649497@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-10-16
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include "stm32h7xx_hal.h"
#include <cstdint>

class PhotoGate{
public:
    PhotoGate() = default;

    void init(GPIO_TypeDef *port, uint16_t pin, bool active_high){
        port_ = port;
        pin_ = pin;
        active_high_ = active_high;

        raw_high_ = false;
        blocked_ = false;
        stable_cnt_ = 0U;

        /*防御性编程：没给端口就当作"永远没被遮挡"，避免空指针解引用*/
        if(port_ == nullptr){
            return;
        }

        /*先读一次，把当前电平当作初始稳定状态，
          否则上电时若恰好被遮挡，会凭空产生一个假边沿*/
        raw_high_ = (HAL_GPIO_ReadPin(port_, pin_) == GPIO_PIN_SET);
        blocked_ = active_high_ ? raw_high_ : !raw_high_;
    }

    void update(){

        /*防御性编程：init()还没调用时直接返回*/
        if(port_ == nullptr){
            return;
        }

        raw_high_ = (HAL_GPIO_ReadPin(port_, pin_) == GPIO_PIN_SET);

        /*极性换算：先算出"逻辑上是否被遮挡"，
          之后的代码只跟blocked_打交道，不再关心高低电平*/
        bool sample_blocked = active_high_ ? raw_high_ : !raw_high_;

        if(sample_blocked == blocked_){
            /*和已确认的状态一致，去抖计数清零*/
            stable_cnt_ = 0U;
            return;
        }

        /*和已确认的状态不一致：累计连续拍数，够了才认账*/
        stable_cnt_++;
        if(stable_cnt_ >= kDebounceCount){
            blocked_ = sample_blocked;
            stable_cnt_ = 0U;
        }
    }

    /* 去抖之后的稳定状态：true = 光路被挡片遮挡 */
    bool isBlocked() const { return blocked_; }

    /* 去抖之前的原始电平，只给调试观察用 */
    bool isRawHigh() const { return raw_high_; }

private:
    static constexpr uint8_t kDebounceCount = 3U;   //连续几拍一致才认账

    GPIO_TypeDef *port_{nullptr};
    uint16_t pin_{0U};
    bool active_high_{false};

    bool raw_high_{false};      //本拍读到的原始电平
    bool blocked_{false};       //去抖后的稳定状态
    uint8_t stable_cnt_{0U};    //与blocked_不同的连续拍数
};