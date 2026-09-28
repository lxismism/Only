/**
 * @file CanBus.hpp
 * @author lxlx (1729649497@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-09-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "stm32h7xx_hal.h"

void canFilterInit(FDCAN_HandleTypeDef *hcan, uint32_t idType, 
                       uint32_t id, uint32_t maskId, uint32_t fifo);

void bspCanInit(FDCAN_HandleTypeDef *hcan);

class CanBus;

class CanDevice{
public:
    CanDevice(uint32_t id, bool is_extid, uint32_t tx_id,
              bool tx_is_extid,Canbus *manager)
              : id_(id), is_extid_(is_extid), tx_id_(tx_id),
                tx_is_extid_(tx_is_extid), manager_(manager){}
    
    virtual ~CanDevice() = default;
    uint32_t id() const { return id_; }
    
    
    

private:
    uint32_t id_;
    uint32_t tx_id_;
    bool is_extid_;
    bool tx_is_extid_;
    CanBus *manager_{nullptr};

}