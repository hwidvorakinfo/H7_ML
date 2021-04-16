/*
 * uart_hal_cm7.c
 *
 *  Created on: 13. 4. 2021
 *      Author: Petr Dvorak
 */

#include <main.hpp>
#include "uart_hal_cm7.h"
#include "defs.h"
#include "error_handler.h"

UART_HandleTypeDef Uart1Handle;
volatile usart_data_tx_t Tx1;
volatile usart_data_rx_t Rx1;
static volatile uint8_t Tx1_buffer[TX1BUFFERSIZE];
static volatile uint8_t Rx1_buffer[RX1BUFFERSIZE];

uint8_t hello_message[] = "\n\r Hello world!";

/* UART configured as follows:
      - Word Length = 8 Bits (7 data bit + 1 parity bit) :
                      BE CAREFUL : Program 7 data bits + 1 parity bit in PC HyperTerminal
      - Stop Bit    = One Stop bit
      - Parity      = ODD parity
      - BaudRate    = 9600 baud
      - Hardware flow control disabled (RTS and CTS signals) */
RETURN_STATUS uart_config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

	/* Enable GPIO TX/RX clock */
	USARTx_TX_GPIO_CLK_ENABLE();
	USARTx_RX_GPIO_CLK_ENABLE();

	/* Select SysClk as source of USART1 clocks */
	RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART16;
	RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

	/* Enable USARTx clock */
	USARTx_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = USARTx_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = USARTx_TX_AF;

	HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

	/* UART RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = USARTx_RX_PIN;
	GPIO_InitStruct.Alternate = USARTx_RX_AF;

	HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

	/* NVIC for USARTx */
	HAL_NVIC_SetPriority(USARTx_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USARTx_IRQn);

	Uart1Handle.Instance        = USARTx;
	Uart1Handle.Init.BaudRate   = 115200;
	Uart1Handle.Init.WordLength = UART_WORDLENGTH_8B;
	Uart1Handle.Init.StopBits   = UART_STOPBITS_1;
	Uart1Handle.Init.Parity     = UART_PARITY_NONE; //UART_PARITY_ODD;
	Uart1Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	Uart1Handle.Init.Mode       = UART_MODE_TX_RX;
	Uart1Handle.Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&Uart1Handle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	// inicializuj data prijmu a vysilani
	Tx1.buffer = (uint8_t *)&Tx1_buffer;
	Tx1.index = 0;
	Tx1.status = READYTOSEND;
	Rx1.buffer = (uint8_t *)&Rx1_buffer;
	Rx1.index = 0;
	Rx1.status = READYTORECEIVE;

	memcpy(Tx1.buffer, hello_message, strlen((const char *)hello_message));

	if(HAL_UART_Transmit_IT(&Uart1Handle, (uint8_t*)Tx1.buffer, strlen((const char *)Tx1.buffer)) != HAL_OK)
	{
		/* Transfer error in transmission process */
		Error_Handler();
	}

	while (HAL_UART_GetState(&Uart1Handle) != HAL_UART_STATE_READY)
	{}

	return RETURN_OK;
}

RETURN_STATUS uart_unconfig(void)
{
	/*##-1- Reset peripherals ##################################################*/
	USARTx_FORCE_RESET();
	USARTx_RELEASE_RESET();

	/*##-2- Disable peripherals and GPIO Clocks ################################*/
	/* Configure UART Tx as alternate function  */
	HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
	/* Configure UART Rx as alternate function  */
	HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

	/*##-3- Disable the NVIC for UART ##########################################*/
	HAL_NVIC_DisableIRQ(USARTx_IRQn);

	return RETURN_OK;
}

/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle.
  * @note   This example shows a simple way to report end of IT Tx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Transfer in transmission process is correct */
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and
  *         you can add your own implementation
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Transfer in reception process is correct */
}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  /* Transfer error in reception/transmission process */
}

