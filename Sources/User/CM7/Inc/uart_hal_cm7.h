/*
 * uart_hal_cm7.h
 *
 *  Created on: 13. 4. 2021
 *      Author: Petr Dvorak
 */

#ifndef SOURCES_USER_CM7_INC_UART_HAL_CM7_H_
#define SOURCES_USER_CM7_INC_UART_HAL_CM7_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"
#include "main.hpp"

typedef enum {
	READYTOSEND = 0,
	READYTORECEIVE,
	TRANSFERING,
	RECEIVING,
	TRANSFERDONE,
	RECEIVEDONE
} USARTSTATUS;

typedef struct {
	uint32_t length;
	uint8_t *buffer;
	uint16_t index;
	USARTSTATUS status;
} usart_data_tx_t;

typedef struct {
	uint32_t length;
	uint8_t *buffer;
	uint16_t index;
	USARTSTATUS status;
} usart_data_rx_t;

#define RX1BUFFERSIZE					128
#define TX1BUFFERSIZE					128

#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF7_USART1
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF7_USART1

/* Definition for USARTx's NVIC IRQ and IRQ Handlers */
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler




extern UART_HandleTypeDef Uart1Handle;

RETURN_STATUS uart_config(void);
RETURN_STATUS uart_unconfig(void);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle);

#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM7_INC_UART_HAL_CM7_H_ */
