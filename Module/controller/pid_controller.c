/**
 * @file pid_controller.c
 * @author lxlx (1729649497@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-10-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "pid_controller.h"
#include "bsp_dwt.h"

#define MAX_DT 0.02f

static void f_Derivate_On_Measurement(PID_t *pid);
static void f_Output_Filter(PID_t *pid);


float PID_Calculate(PID_t *pid, float measure, float ref)
{
    pid->Measure = measure;
    pid->Ref = ref;
    pid->Err = pid->Ref - pid->Measure;
    
    pid->dt = DWT_GetDeltaT(&pid->DWT_CNT);

    if(pid->dt > MAX_DT){
        pid->Last_Err = pid->Err;
        pid->Last_Measure = pid->Measure;
        return pid->Output;
    }
    if(pid->dt <= 1e-6f) pid->dt = 1e-6f;


    const uint8_t in_deadband = (pid->Err < pid->DeadBand && pid->Err > -pid->DeadBand) ? 1 : 0;
    if(in_deadband){
        pid->Err = 0.0f;
    }

    //比例项
    pid->Pout = pid->Kp * pid->Err;

    //积分项
    pid->ITerm += pid->Ki * pid->Err * pid->dt;
    if(pid->ITerm >= pid->IntegralLimit) pid->ITerm = pid->IntegralLimit;
    else if(pid->ITerm <= -pid->IntegralLimit) pid->ITerm = -pid->IntegralLimit;
    pid->Iout = pid->ITerm;

    //微分项
    if(in_deadband){
        pid->Dout = 0.0f;
    } else {
        pid->Dout = pid->Kd * (pid->Err - pid->Last_Err) / pid->dt;
    }

    if(pid->Improve & Derivative_On_Measurement)
        f_Derivate_On_Measurement(pid);

    pid->Output = pid->Pout + pid->Iout + pid->Dout;


    if(pid->Improve & OutputFilter)
        f_Output_Filter(pid);
    


    // 输出限幅
    if (pid->Output > pid->Maxout)  pid->Output = pid->Maxout;
    if (pid->Output < -pid->Maxout) pid->Output = -pid->Maxout;

    pid->Last_Err = pid->Err;
    pid->Last_Output = pid->Output;
    pid->Last_Measure = pid->Measure;

     
    return pid->Output;
}


static void f_Derivate_On_Measurement(PID_t *pid){
    pid->Dout = pid->Kd * (pid->Last_Measure - pid->Measure) / pid->dt;
}

static void f_Output_Filter(PID_t *pid){
    pid->Output = (pid->Output * pid->dt + pid->Output_LPF_RC * pid->Last_Output) /
                 (pid->Output_LPF_RC + pid->dt);
}

void PID_Reset(PID_t *pid){
    pid->Ref = 0.0f;
    pid->Measure = 0.0f;
    pid->Last_Measure = 0.0f;
    pid->Err = 0.0f;
    pid->Last_Err = 0.0f;

    pid->ITerm = 0.0f;
    pid->Pout = 0.0f;
    pid->Iout = 0.0f;
    pid->Dout = 0.0f;
    pid->Output = 0.0f;
    pid->Last_Output = 0.0f;

    pid->DWT_CNT = 0;
    pid->dt = 0.0f;
}