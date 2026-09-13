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
#include "stm32h7xx_hal_uart.h"
#include <stdint.h>
#include <string.h>

static Uart_Instance_t *Uart_Device[DEVICE_UART_CNT] = {NULL};
static uint8_t idx = 0;

static uint8_t Uart_Rx_Idle_Callback(Uart_Instance_t *uart_instance);

/**
 * @brief 阻塞式发送函数，发完前阻塞cpu，不建议用
 * 
 * @param tx_package 
 */     
uint8_t Uart_Tx_By_Blocking(Uart_Tx_Package_t tx_package)
{
    HAL_UART_Transmit(tx_package.uart_handle, tx_package.tx_buffer, tx_package.tx_buffer_size, 20);
    return 1;
}

/**
 * @brief 中断发送
 * 
 * @param tx_package 
 * @return uint8_t 
 */
uint8_t Uart_Tx_By_It(Uart_Tx_Package_t tx_package)
{
    HAL_UART_Transmit_IT(tx_package.uart_handle, tx_package.tx_buffer, tx_package.tx_buffer_size);
    return 1;
}

/**
 * @brief DMA发送
 * 
 * @param tx_package 
 * @return uint8_t 
 */
uint8_t Uart_Tx_By_DMA(Uart_Tx_Package_t tx_package)
{
    HAL_UART_Transmit_DMA(tx_package.uart_handle, tx_package.tx_buffer, tx_package.tx_buffer_size);
    return 1;
}

/**
 * @brief 阻塞式发送
 * 
 * @param uart_config 
 */
void Uart_Receive_By_Blocking(uart_package_t *uart_config)
{
    HAL_UART_Receive(uart_config->uart_handle, uart_config->rx_buffer, uart_config->rx_buffer_size, HAL_MAX_DELAY);
}



/**
 * @brief 串口注册
 * 
 * @param uart_config 
 * @return Uart_Instance_t* 
 */
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

    if(uart_config->IT_CHOOSE == 0)
    {
        __HAL_UART_CLEAR_IDLEFLAG(uart_instance->uart_package.uart_handle);
        __HAL_UART_ENABLE_IT(uart_instance->uart_package.uart_handle, UART_IT_IDLE);
        HAL_UART_Receive_DMA(uart_instance->uart_package.uart_handle,
                             uart_instance->uart_package.rx_buffer,
                             uart_instance->uart_package.rx_buffer_size);
    }
    else if(uart_config->IT_CHOOSE == 1)
    {
        HAL_UART_Receive_IT(uart_instance->uart_package.uart_handle,
                              uart_instance->uart_package.rx_buffer,
                              uart_instance->uart_package.rx_buffer_size);
    }

    Uart_Device[idx++] = uart_instance;
    return uart_instance;
    

}

uint8_t Uart_Receive_Handler(Uart_Instance_t *uart_instance)
{
    if(uart_instance == NULL) return 0;

    if(__HAL_UART_GET_FLAG(uart_instance->uart_package.uart_handle, UART_FLAG_IDLE) != RESET)
    {
        Uart_Rx_Idle_Callback(uart_instance);
        return 1;
    }
    else
    {
        return 0;
    }
}

static uint8_t Uart_Rx_Idle_Callback(Uart_Instance_t *uart_instance)
{
    /*指针非空 */
    if(uart_instance == NULL) return 0;

    /*数据长度，清除标志位，停一下DMA方便读取NDTR*/
    static uint16_t uart_rx_num;
    __HAL_UART_CLEAR_IDLEFLAG(uart_instance->uart_package.uart_handle);
    HAL_UART_DMAStop(uart_instance->uart_package.uart_handle);

    uart_rx_num = uart_instance->uart_package.rx_buffer_size
                - ((DMA_Stream_TypeDef*)uart_instance->uart_package.uart_handle->hdmarx->Instance)->NDTR;
    if(uart_instance->uart_package.uart_callback != NULL)
    {
        uart_instance->uart_package.uart_callback(uart_instance, uart_rx_num);
    }    
    else 
    {
        return 0;
    }

    HAL_UART_Receive_DMA(uart_instance->uart_package.uart_handle,
                         uart_instance->uart_package.rx_buffer,
                         uart_instance->uart_package.rx_buffer_size);
    return 1;
}

Uart_Instance_t* Uart_Find_Device(UART_HandleTypeDef* huart)
{
    for(int i = 0; i < idx ; i++)
    {
        if(Uart_Device[i]->uart_package.uart_handle == huart) return Uart_Device[i];
    }
    return NULL;
}

// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
// {
//     Uart_Receive_Handler( Uart_Find_Device(huart));
// }