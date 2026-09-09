/**
 * @file bsp_usart.c
 * @author lxlx (1729649497@qq.com)
 * @brief 串口设备抽象层，提供串口发送功能
 * @version 0.1
 * @date 2026-09-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */


#include "bsp_usart.h"
#include "FreeRTOS.h"
#include <string.h>

static Uart_Instance_t *Uart_Device[DEVICE_UART_CNT] = {NULL};
static uint8_t idx = 0;

/**
 * @brief 阻塞式发送函数，发完前阻塞cpu，不建议用
 * 
 * @param tx_package 
 */     
uint8_t Uart_Tx_By_Blocking(Uart_Tx_Package_t tx_package)
{
    HAL_UART_Transmit(tx_package.uart_handle, tx_package.tx_buffer, tx_package.tx_buffer_size, HAL_MAX_DELAY);
    return 1;
}

/**
 * @brief 阻塞式接收函数，收完len个字节之后才会把数据存进buffer，调用此函数后，收完前cpu会一直在这行代码这里阻塞
 * 
 * @param uart_config 
 */
void Uart_Receive_By_Blocking(uart_package_t *uart_config)
{
    HAL_UART_Receive(uart_config->uart_handle, uart_config->rx_buffer, uart_config->rx_buffer_size, HAL_MAX_DELAY);
}

Uart_Instance_t* Uart_Register(uart_package_t *uart_config)
{
    if(uart_config == NULL)
    {
        return NULL;
    }   /*传空指针进来，注册失败*/
    if(idx >= DEVICE_UART_CNT)
    {
        return NULL;
    }   /*户口超过限额，注册失败*/
    for(uint8_t i = 0; i < idx; i++)
    {
        if(uart_config->uart_handle == Uart_Device[i]->uart_package.uart_handle)
        {
            return NULL;
        }   /*该uart口已被原有设备注册，注册失败*/
    }   /*检查该uart口是否已经被注册过*/

    Uart_Instance_t *uart_instance = (Uart_Instance_t*)pvPortMalloc(sizeof(Uart_Instance_t));   /*用FreeRtos的安全动态分配*/
    memset(uart_instance, 0, sizeof(Uart_Instance_t));      /*把动态分配的内存全部设置为空指针*/
    
    memset(uart_config->rx_buffer, 0,uart_config->rx_buffer_size);      /*把包里rx缓冲区的内容全部初始化为0*/
    uart_instance->uart_package = *uart_config;

    HAL_UARTEx_ReceiveToIdle_IT(uart_instance->uart_package.uart_handle,
                                uart_instance->uart_package.rx_buffer,
                                uart_instance->uart_package.rx_buffer_size
    );

    Uart_Device[idx++] = uart_instance;
    return uart_instance;
    

}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)   /*此处的Size是在进到这个回调函数时，rx_buffer收到的字节数*/
{
    /*进这个函数的方式不一定是装满，10ms后没消息也会进这个回调函数*/              
    for(uint8_t i = 0; i<idx ; i++)
    {                                                             
        if(huart == Uart_Device[i]->uart_package.uart_handle)
        {
            if(Uart_Device[i]->uart_package.uart_callback != NULL)
            {
                Uart_Device[i]->uart_package.uart_callback(Uart_Device[i], Size);
            }
            HAL_UARTEx_ReceiveToIdle_IT(Uart_Device[i]->uart_package.uart_handle,
                                        Uart_Device[i]->uart_package.rx_buffer,    
                                        Uart_Device[i]->uart_package.rx_buffer_size);   /*收满之后会跳进这个回调函数，而且收满一次之后中断接收就不再工作了，所以要在这里重新开启*/
            break;                            
        }
    }
}

