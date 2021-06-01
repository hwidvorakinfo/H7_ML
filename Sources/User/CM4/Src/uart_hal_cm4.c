/*
 * uart_hal_cm7.c
 *
 *  Created on: 13. 4. 2021
 *      Author: Petr Dvorak
 */

#include "main.h"
#include "application.h"
#include "uart_hal_cm4.h"
#include "defs.h"
#include "error_handler.h"
#include "services.h"
#include "mikrobus.h"

UART_HandleTypeDef Uart1Handle;
volatile usart_data_tx_t Tx1;
volatile usart_data_rx_t Rx1;
static volatile uint8_t Tx1_buffer[TX1BUFFERSIZE];
static volatile uint8_t Rx1_buffer[RX1BUFFERSIZE];

const uint8_t hello_message[] = "\n\r-= BeastH7! =-\r\n";

/* UART configured as follows:
      - Word Length = 8 Bits (7 data bit + 1 parity bit) :
                      BE CAREFUL : Program 7 data bits + 1 parity bit in PC HyperTerminal
      - Stop Bit    = One Stop bit
      - Parity      = ODD parity
      - BaudRate    = 115200 baud
      - Hardware flow control disabled (RTS and CTS signals) */
RETURN_STATUS uart_config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

	/* Enable GPIO TX/RX clock */
	USARTUSB_TX_GPIO_CLK_ENABLE();
	USARTUSB_RX_GPIO_CLK_ENABLE();

	/* Select SysClk as source of USART1 clocks */
	RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART16;
	RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

	/* Enable USARTUSB clock */
	USARTUSB_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = USARTUSB_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = USARTUSB_TX_AF;

	HAL_GPIO_Init(USARTUSB_TX_GPIO_PORT, &GPIO_InitStruct);

	/* UART RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = USARTUSB_RX_PIN;
	GPIO_InitStruct.Alternate = USARTUSB_RX_AF;

	HAL_GPIO_Init(USARTUSB_RX_GPIO_PORT, &GPIO_InitStruct);

	/* NVIC for USARTUSB */
	HAL_NVIC_SetPriority(USARTUSB_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USARTUSB_IRQn);

	Uart1Handle.Instance        = USARTUSB;
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
	Rx1.escape = FALSE;

	uart1_send_message((uint8_t *)hello_message, strlen((const char *)hello_message));

	if(HAL_UART_Receive_IT(&Uart1Handle, (uint8_t *)Rx1.buffer, 1) != HAL_OK)
	{
		Error_Handler();
	}

	return RETURN_OK;
}

#ifdef UART1_ENABLED
RETURN_STATUS uart1_send_message(uint8_t *text, uint16_t len)
{
	usart_wait_until_sent((usart_data_tx_t *)&Tx1);

	Tx1.index = 0;
	Tx1.length = len;
	Tx1.status = TRANSFERING;

	memcpy(Tx1.buffer, (const char *)text, len);

	if(HAL_UART_Transmit_IT(&Uart1Handle, (uint8_t*)Tx1.buffer, len) != HAL_OK)
	{
		/* Transfer error in transmission process */
		Error_Handler();
	}
	return RETURN_OK;
}
#endif // UART1_ENABLED

#ifdef UART1_ENABLED
RETURN_STATUS uart1_new_line(void)
{
	usart_wait_until_sent((usart_data_tx_t *)&Tx1);

	Tx1.buffer[0] = '\r';
	Tx1.buffer[1] = '\n';
	Tx1.index = 0;
	Tx1.length = 2;
	Tx1.status = TRANSFERING;

	if(HAL_UART_Transmit_IT(&Uart1Handle, (uint8_t*)Tx1.buffer, 2) != HAL_OK)
	{
		/* Transfer error in transmission process */
		Error_Handler();
	}
	return RETURN_OK;
}
#endif // UART1_ENABLED

void usart_wait_until_sent(usart_data_tx_t *buffer)
{
	volatile uint16_t timeout = 0xffff;

	while (buffer->status == TRANSFERING)
	{
		if (timeout-- == 0)
		{
			break;
		}
	}
}

RETURN_STATUS uart_unconfig(void)
{
	/*##-1- Reset peripherals ##################################################*/
	USARTUSB_FORCE_RESET();
	USARTUSB_RELEASE_RESET();

	/*##-2- Disable peripherals and GPIO Clocks ################################*/
	/* Configure UART Tx as alternate function  */
	HAL_GPIO_DeInit(USARTUSB_TX_GPIO_PORT, USARTUSB_TX_PIN);
	/* Configure UART Rx as alternate function  */
	HAL_GPIO_DeInit(USARTUSB_RX_GPIO_PORT, USARTUSB_RX_PIN);

	/*##-3- Disable the NVIC for UART ##########################################*/
	HAL_NVIC_DisableIRQ(USARTUSB_IRQn);

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
	if (UartHandle == &Uart1Handle)
	{
		Tx1.status = TRANSFERDONE;
	}

	/* Transfer in transmission process is correct */
	if (UartHandle == &Uart2Handle)
	{
		Tx2.status = TRANSFERDONE;
	}
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
	if (UartHandle == &Uart1Handle)
	{
		Rx1.status = RECEIVING;

		// textovy mod
		if ((Rx1.buffer[Rx1.length] != '\n') && (Rx1.buffer[Rx1.length] != '\r'))
		{
			Rx1.length++;

			// posli prijaty znak jako echo, pokud jde o text
			volatile uint8_t reply = Rx1.buffer[Rx1.length-1];
			if ((reply >= ' ') && (reply <= '~'))
			{
				if (Rx1.escape != TRUE)
				{
					Tx1.index = 0;
					Tx1.length = 1;
					Tx1.status = TRANSFERING;
					if(HAL_UART_Transmit_IT(&Uart1Handle, (uint8_t *)&reply, 1) != HAL_OK)
					{
						// chyby osetreny jeste pred vysilanim
						//Error_Handler();
					}
				}
				else
				{
					Rx1.escape = FALSE;
				}
			}
			else if (reply == '\e')
			{
				// escape znak - priste nic nevypisuj
				Rx1.escape = TRUE;
			}

			// nastav prijem dalsiho znaku
			if(HAL_UART_Receive_IT(&Uart1Handle, (uint8_t *)&Rx1.buffer[Rx1.length], 1) != HAL_OK)
			{
				Error_Handler();
			}
		}
		else
		{
			// konec prenosu
			Rx1.status = RECEIVEDONE;
			Rx1.buffer[Rx1.length] = 0;
			if(Scheduler_Add_Task(Cmd_decode_service, 0, CMD_DECODE_SERVICE_PERIOD) == SCH_MAX_TASKS)
			{
				// chyba pri zalozeni service
			}
		}
	}

	/* Transfer in reception process is correct */
	if (UartHandle == &Uart2Handle)
	{
		Rx2.status = RECEIVING;

		// textovy mod
		if ((Rx2.buffer[Rx2.length] != '\n') && (Rx2.buffer[Rx2.length] != '\r'))
		{
			Rx2.length++;

			// posli prijaty znak jako echo
			Tx2.index = 0;
			Tx2.length = 1;
			Tx2.status = TRANSFERING;
			if(HAL_UART_Transmit_IT(&Uart2Handle, (uint8_t *)&Rx2.buffer[Rx2.length-1], 1) != HAL_OK)
			{
				// TODO recovery z teto chyby je nutne, muze se stat pri prijmu ceskych znaku
				Error_Handler();
			}

			// nastav prijem dalsiho znaku
			if(HAL_UART_Receive_IT(&Uart2Handle, (uint8_t *)&Rx2.buffer[Rx2.length], 1) != HAL_OK)
			{
				Error_Handler();
			}
		}
		else
		{
			// konec prenosu
			Rx2.status = RECEIVEDONE;
			Rx2.buffer[Rx2.length] = 0;
			if(Scheduler_Add_Task(Mikrobus_uart_decode_service, 0, MIKROBUS_DECODE_SERVICE_PERIOD) == SCH_MAX_TASKS)
			{
				// chyba pri zalozeni service
			}
		}
	}
}

/**
  * @brief  Reception Event Callback (Rx event notification called after use of advanced reception service).
  * @param  huart UART handle
  * @param  Size  Number of data available in application reception buffer (indicates a position in
  *               reception buffer until which, data are available)
  * @retval None
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *UartHandle, uint16_t Size)
{
	if (UartHandle == &Uart1Handle)
	{
		Rx1.status = RECEIVING;
	}

	if (UartHandle == &Uart2Handle)
	{
		Rx2.status = RECEIVING;
	}
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

// nastav mod UART na prijem - pro HAL to znamena nastavit prijem na ocekavani 1 znaku
void usart_set_receive_mode(UART_HandleTypeDef *UartHandle)
{
	if (UartHandle == &Uart1Handle)
	{
		Rx1.index = 0;
		Rx1.length = 0;
		Rx1.status = READYTORECEIVE;

		// nastav prijem dalsiho znaku
		if(HAL_UART_Receive_IT(&Uart1Handle, (uint8_t *)&Rx1.buffer[0], 1) != HAL_OK)
		{
			Error_Handler();
		}
	}

	if (UartHandle == &Uart2Handle)
	{
		Rx2.index = 0;
		Rx2.length = 0;
		Rx2.status = READYTORECEIVE;

		// nastav prijem dalsiho znaku
		if(HAL_UART_Receive_IT(&Uart2Handle, (uint8_t *)&Rx2.buffer[0], 1) != HAL_OK)
		{
			Error_Handler();
		}
	}

}

uint8_t *usart_get_rx_buffer(void)
{
	return (uint8_t *)&Rx1_buffer;
}
