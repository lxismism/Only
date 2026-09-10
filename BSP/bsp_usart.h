/**
 * @file bsp_usart.h
 * @author lxlx (1729649497@qq.com)
 * @brief 串口设备抽象层，提供串口发送功能
 * @version 0.1
 * @date 2026-09-06
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef BSP_USART_H
#define BSP_USART_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include "usart.h"

#define RX_BUFFER_SIZE              256
#define DEVICE_UART_CNT               8

/**
 * @brief 对于不同的模块，收到的数据处理方式不同，所以回调函数的函数体不一样，所以在此定义一个回调函数类型，
 * 这样就可以把不同函数体的函数像传参一样传进callback了。
 * 
 */
typedef uint8_t (*uart_callback_t)(void *uart_device, uint16_t rx_buf_num);

/**
 * @brief 串口包结构体
 * 
 */
typedef struct 
{
    UART_HandleTypeDef *uart_handle;        /*串口句柄*/
  
    uint8_t *rx_buffer;                     /*接收缓存区*/
    uint16_t rx_buffer_size;                /*接收缓存区大小*/
    uint8_t use_static_length_data;         /*接收数据的长度是否固定*/

    uint8_t IT_CHOOSE;                      /*0:DWA空闲接收中断 1:关闭DWA，普通中断*/
    uart_callback_t uart_callback;          /*处理数据的回调函数*/

}uart_package_t;

/**
 * @brief 串口设备实例
 * 
 */
typedef struct 
{
    uart_package_t uart_package;    /*串口配置包*/
    void* device;                   /*父指针，存上层设备。void*是万能指针*/
    uint8_t (*Uart_Deinit)(void*);  /*注销函数指针*/
}Uart_Instance_t;


/**
 * @brief 串口发送数据包结构体
 * 
 */
typedef struct 
{
    UART_HandleTypeDef *uart_handle;        /*串口句柄*/
    uint8_t *tx_buffer;                     /*要发送的缓存*/
    uint8_t tx_buffer_size;                /*发送缓存的大小*/
}Uart_Tx_Package_t;




uint8_t Uart_Tx_By_Blocking(Uart_Tx_Package_t tx_package);
uint8_t Uart_Tx_By_It(Uart_Tx_Package_t tx_package);
uint8_t Uart_Tx_By_DMA(Uart_Tx_Package_t tx_package);


void Uart_Receive_By_Blocking(uart_package_t *uart_config);
Uart_Instance_t* Uart_Register(uart_package_t *uart_config);


#ifdef __cplusplus
}
#endif

#endif  /*BSP_USART_H*/