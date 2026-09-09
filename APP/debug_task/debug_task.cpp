/**
 * @file debug_task.cpp
 * @author lxlx (1729649497@qq.com)
 * @brief APP调试层，专门用来测试程序
 * @version 0.1
 * @date 2026-09-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "debug_task.h"
#include "bsp_usart.h"
#include "cmsis_os2.h"

osThreadId_t Debug_TaskHandle;

static uint8_t my_rx_buffer[RX_BUFFER_SIZE];


static uint8_t MyEcho(void *uart_device, uint16_t rx_buf_num)
{
  Uart_Instance_t *inst = (Uart_Instance_t*)uart_device;
  Uart_Tx_Package_t tx_package = {inst->uart_package.uart_handle,
                                    inst->uart_package.rx_buffer,
                               (uint8_t)rx_buf_num};
  Uart_Tx_By_Blocking(tx_package);

  return 1;
}


static uart_package_t my_uart = {
  .uart_handle    = &huart3,
  .rx_buffer      = my_rx_buffer,
  .rx_buffer_size = RX_BUFFER_SIZE,
  .uart_callback  = MyEcho
};

void debugTask(void *argument)
{
    TickType_t currentTime;
    currentTime = xTaskGetTickCount();
    Uart_Register(&my_uart);
    
    for(;;)
    {
        vTaskDelayUntil(&currentTime, 1);
    }

}