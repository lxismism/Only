/**
 * @file com_config.cpp
 * @author lxlx (1729649497@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-09-20
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "com_config.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "usart.h"
#include "fdcan.h"

#include "topics.hpp"
#include "memory_map.h"
#include "UartPort.hpp"
#include "Canbus.hpp"
#include "Motor.hpp"
#include "topic_pool.h"

/*-------------------------------------fdcan----------------------------------------*/


osThreadId_t can3Send_TaskHandle;

CanBus fdcan1_bus(hfdcan1);
CanBus fdcan2_bus(hfdcan2);
CanBus fdcan3_bus(hfdcan3);

C620Motor chassis_motor1(&fdcan3_bus, 0x202, false, 0x200, false);

static TypedTopicSubscriber<pub_chassis_cmd> chassis_cmd_sub("chassis_cmd", 8);
static pub_chassis_cmd chassis_cmd{};


/*-------------------------------------fdcan----------------------------------------*/



/*-------------------------------------usart----------------------------------------*/
osThreadId_t uart3Process_TaskHandle;

void onUart3RxCb(const uint8_t *data, size_t len, void *user);

DMA_BUFFER_ATTR static uint8_t uart3_rx_dma[64];
DMA_BUFFER_ATTR static uint8_t uart3_tx_dma[64];


UartPort uart3_port(&huart3, uart3_rx_dma, sizeof(uart3_rx_dma),
                    uart3_tx_dma, sizeof(uart3_tx_dma), onUart3RxCb, nullptr);

osSemaphoreId_t uart3_rx_semaphore = NULL;

/*-------------------------------------usart----------------------------------------*/

uint8_t comServiceInit(){

    canFilterInit(&hfdcan1, FDCAN_STANDARD_ID, 0, 0, FDCAN_FILTER_TO_RXFIFO0);
    canFilterInit(&hfdcan1, FDCAN_STANDARD_ID, 0, 0, FDCAN_FILTER_TO_RXFIFO1);
    bspCanInit(&hfdcan1);

    canFilterInit(&hfdcan2, FDCAN_STANDARD_ID, 0, 0, FDCAN_FILTER_TO_RXFIFO0);
    canFilterInit(&hfdcan2, FDCAN_STANDARD_ID, 0, 0, FDCAN_FILTER_TO_RXFIFO1);
    bspCanInit(&hfdcan2);

    canFilterInit(&hfdcan3, FDCAN_STANDARD_ID, 0, 0, FDCAN_FILTER_TO_RXFIFO0);
    canFilterInit(&hfdcan3, FDCAN_STANDARD_ID, 0, 0, FDCAN_FILTER_TO_RXFIFO1);
    bspCanInit(&hfdcan3);

    fdcan1_bus.init();
    fdcan2_bus.init();
    fdcan3_bus.init();

    chassis_motor1.init();
    fdcan3_bus.registerDevice(&chassis_motor1);


    uart3_rx_semaphore = osSemaphoreNew(1, 0, NULL);
    uart3_port.startRxDmaIdle();


    return 0;
}


/*-------------------------------------usart----------------------------------------*/
void onUart3RxCb(const uint8_t *data, size_t len, void *user){
    (void)user;
    if(data != nullptr && len > 0 && uart3_rx_semaphore != nullptr){
        osSemaphoreRelease(uart3_rx_semaphore);
    }
}

void uart3RxProcessTask(void *argument){

    (void)argument;
    for(;;){
        osSemaphoreAcquire(uart3_rx_semaphore, osWaitForever);
        
        UartPort::Packet packet{};
        while(uart3_port.Read(packet)){
            uart3_port.write(packet.data, packet.len);
        }
    }
}

/*-------------------------------------usart----------------------------------------*/


/*-------------------------------------fdcan----------------------------------------*/

void can3SendTask(void *argument){
    TickType_t current = xTaskGetTickCount();

    for(;;){
        
            chassis_cmd_sub.TryGet(&chassis_cmd);
            chassis_motor1.setMotorCmd(chassis_cmd.chassis_motor1_cmd);

            uint8_t data[8] = {0};
            CanBus::ClassicPack pack = {};
            uint8_t len = 0U;
            uint32_t motor_ids[4] = {0, 0x202, 0 ,0};
            int16_t commands[4] = {0, static_cast<int16_t>(chassis_motor1.cmdTrans()), 0, 0};

            packDJIMotorCanMsg(0x200, motor_ids, commands, 4U, data, len);

            pack.id = 0x200;
            pack.type = CanBus::Type::STANDARD;
            for(uint8_t i = 0; i < 8; ++i) pack.data[i] = data[i];

            fdcan3_bus.addCanMsg(pack);

        

        vTaskDelayUntil(&current, 1);

    }
}
/*-------------------------------------fdcan----------------------------------------*/
