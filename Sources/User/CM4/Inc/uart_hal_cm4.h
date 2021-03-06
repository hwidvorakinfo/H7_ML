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
#include "main.h"
#include "application.h"

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
	uint8_t escape;
} usart_data_rx_t;

#define RX1BUFFERSIZE						128
#define TX1BUFFERSIZE						128

#define UART1_BAUDRATE_SLOW					115200
#define UART1_BAUDRATE_FAST					921600

#define USARTUSB                        	USART1
#define USARTUSB_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define USARTUSB_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTUSB_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTUSB_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTUSB_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for USARTx Pins */
#define USARTUSB_TX_PIN                    GPIO_PIN_9
#define USARTUSB_TX_GPIO_PORT              GPIOA
#define USARTUSB_TX_AF                     GPIO_AF7_USART1
#define USARTUSB_RX_PIN                    GPIO_PIN_10
#define USARTUSB_RX_GPIO_PORT              GPIOA
#define USARTUSB_RX_AF                     GPIO_AF7_USART1

/* Definition for USARTx's NVIC IRQ and IRQ Handlers */
#define USARTUSB_IRQn                      USART1_IRQn
#define USARTUSB_IRQHandler                USART1_IRQHandler

extern UART_HandleTypeDef Uart1Handle;
extern volatile usart_data_tx_t Tx2;
extern volatile usart_data_rx_t Rx2;

RETURN_STATUS uart_config(void);
RETURN_STATUS uart_hello(void);
RETURN_STATUS uart_unconfig(void);
RETURN_STATUS uart1_send_message(uint8_t *text, uint16_t len);
RETURN_STATUS uart1_new_line(void);
void usart_wait_until_sent(usart_data_tx_t *buffer);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *UartHandle, uint16_t Size);
void usart_set_receive_mode(UART_HandleTypeDef *UartHandle);
uint8_t *usart_get_rx_buffer(void);
void myprintf(const char *format, ...);
RETURN_STATUS usart_set_baudrate(uint32_t baudrate);

#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM7_INC_UART_HAL_CM7_H_ */
