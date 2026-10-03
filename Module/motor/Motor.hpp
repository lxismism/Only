/**
 * @file Motor.hpp
 * @author lxlx (1729649497@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-09-30
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#pragma once

#include "Canbus.hpp"

#define RAD_2_DEG            57.2957795f
#define DEG_2_RAD            0.01745329252f
#define RPM_2_DEG_PER_SEC    6.0f
#define RPM_2_RAD_PER_SEC    0.104719755f

class MotorBase{
public:
    MotorBase() = default;

    void setMotorCmd(float cmd){
        if(cmd > max_cmd_){
            cmd = max_cmd_;
        }
        if(cmd < -max_cmd_){
            cmd = -max_cmd_;
        }
        cmd_ = cmd;
    }
    void setMotorRadSpeed(float speed);
    void setMotorDeg(float deg);

    float getSinglePos(void) const { return single_deg_; }
    float getSumPos(void) const { return sum_deg_; }
    float getDegSpeed(void) const { return deg_speed_; }
    float getTorque(void) const {return torque_; }
    float getTemperature(void) const { return temperature_; }

protected:
    float cmd_{0.0f};
    
    float max_cmd_{99999.0f};
    float reduction_ratio_{1.0f};

    float single_deg_{0.0f};
    float sum_deg_{0.0f};
    float deg_speed_{0.0f};
    float torque_{0.0f};
    float temperature_{0.0f};

};

class C620Motor : public CanDevice , public MotorBase {
public:
    C620Motor(CanBus *manager, uint32_t id, bool is_extid,
              uint32_t tx_id, bool tx_is_extid)
              : CanDevice(manager, id, is_extid, tx_id, tx_is_extid){}

    void init(float reduction = 3591.0f / 187.0f, float max_cmd = 20000.0f){
        reduction_ratio_ = reduction;
        max_cmd_ = max_cmd;
        is_motor_init_ = true;
    }


    void onRx(const uint8_t data[8], const uint8_t len) override{
        if(len < 8) return;     
        if(!is_motor_init_) return;      
        
        /*data[0]~data[1]: encoder data[0]为单圈内转子位置的高8位，data[1]为单圈内转子位置的低八位*/
        encoder_ = (uint16_t)((data[0] << 8) | data[1] );

        int16_t encoder_delta = encoder_ - last_encoder_;
        
        if(is_encoder_init){
        
            if(encoder_delta < -kCountPerHalfRound ) round_cnt_++;
            else if(encoder_delta > kCountPerHalfRound) round_cnt_--;
        
        } else {
            encoder_offset_ = encoder_;
            is_encoder_init = true;
        }
        last_encoder_ = encoder_;

        single_deg_ = (encoder_ * 360.0f / kCountPerRound) / reduction_ratio_;
        sum_deg_ = (static_cast<float>(round_cnt_) - 
                        static_cast<float>(encoder_offset_) / kCountPerRound) * 360.0f / reduction_ratio_
                         + single_deg_;

        /*data[2]~data[3]: data[2]转子RPM高8位，data[3]转子RPM低8位*/
        int16_t raw_rpm = static_cast<int16_t>((data[2] << 8) | data[3]);
        deg_speed_ = static_cast<float>(raw_rpm) * RPM_2_DEG_PER_SEC / reduction_ratio_;

        /*data[4]~data[5]: data[4]实际转矩电流高8位，data[5]实际按转矩电流低8位*/
        int16_t raw_tor_cur = static_cast<int16_t>((data[4] << 8) | data[5]);
        torque_ = static_cast<float>(raw_tor_cur) *20.0f * kCurToTorque / 16384.0f ;       //torque = current * k

        /*data[6]: 电机温度*/
        temperature_ = static_cast<float>(data[6]);

    }

    float cmdTrans() { return cmd_ * 16384.0f / 20000.0f;}

    bool buildTx(uint8_t data[8], uint8_t &len) override {
        len = 0;
        return false;
    }

    

private:
    bool is_motor_init_ = false;
    bool is_encoder_init = false;

    static constexpr float kCountPerRound = 8192.0f;
    static constexpr float kCountPerHalfRound = 4096.0f;
    static constexpr float kCurToTorque = 0.3f;  //转矩常数，单位N*m/A，这里是以DJI3508电机为准填的0.3，但是这玩意不同电机是不一样的，按理说应该作为一个初始化参数传进来，但是由于我们只用3508电机，所以就先这样吧。
    uint16_t encoder_offset_{0U};
    uint16_t encoder_{0U};
    uint16_t last_encoder_{0U};

    int32_t round_cnt_{0};


};

void packDJIMotorCanMsg(const uint32_t tx_id, const uint32_t *motor_ids,
                        const int16_t *commands,const uint8_t motor_count, 
                        uint8_t *data, uint8_t &len);