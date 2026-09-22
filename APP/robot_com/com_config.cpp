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

#include "memory_map.h"
#include "UartPort.hpp"

osThreadId_t uart3Process_TaskHandle;

void onUart3RxCb(const uint8_t *data, size_t len, void *user);

DMA_BUFFER_ATTR static uint8_t uart3_rx_dma[64];
DMA_BUFFER_ATTR static uint8_t uart3_tx_dma[64];


UartPort uart3_port(&huart3, uart3_rx_dma, sizeof(uart3_rx_dma),
                    uart3_tx_dma, sizeof(uart3_tx_dma), onUart3RxCb, nullptr);

osSemaphoreId_t uart3_rx_semaphore = NULL;

uint8_t comServiceInit(){
    uart3_rx_semaphore = osSemaphoreNew(1, 0, NULL);
    uart3_port.startRxDmaIdle();
    return 0;
}



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

