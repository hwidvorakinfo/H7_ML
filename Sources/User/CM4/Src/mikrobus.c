/*
 * mikrobus.c
 *
 *  Created on: 19. 5. 2021
 *      Author: Petr Dvorak
 */

#include "main.h"
#include "application.h"
#include "defs.h"
#include "error_handler.h"
#include "mikrobus.h"
#include "i2c_timing_utility.h"

UART_HandleTypeDef Uart2Handle;
volatile usart_data_tx_t Tx2;
volatile usart_data_rx_t Rx2;
static volatile uint8_t Tx2_buffer[TX2BUFFERSIZE];
static volatile uint8_t Rx2_buffer[RX2BUFFERSIZE];

I2C_HandleTypeDef I2c4Handle;
static volatile uint8_t i2c_buffer[I2CBUFFERSIZE];

SPI_HandleTypeDef Spi1Handle;
static volatile uint8_t spitx_buffer[SPIBUFFERSIZE];
static volatile uint8_t spirx_buffer[SPIBUFFERSIZE];
__IO uint32_t wTransferState = TRANSFER_WAIT;


const uint8_t mikrobus_message[] = "\n\rmikrobus\r\n";

RETURN_STATUS mikrobus_init(void)
{
#ifdef UART2_ENABLED
	if (mikrobus_uart_config() != RETURN_OK)
	{
		Error_Handler();
	}
#endif // UART2_ENABLED

#ifdef I2C4_ENABLED
	if (mikrobus_i2c_config() != RETURN_OK)
	{
		Error_Handler();
	}
#endif // I2C4_ENABLED

#ifdef SPI1_ENABLED
	if (mikrobus_spi_config() != RETURN_OK)
	{
		Error_Handler();
	}
#endif // SPI1_ENABLED

	return RETURN_OK;
}

#ifdef UART2_ENABLED
/* UART configured as follows:
      - Word Length = 8 Bits (7 data bit + 1 parity bit) :
                      BE CAREFUL : Program 7 data bits + 1 parity bit in PC HyperTerminal
      - Stop Bit    = One Stop bit
      - Parity      = ODD parity
      - BaudRate    = 115200 baud
      - Hardware flow control disabled (RTS and CTS signals) */
RETURN_STATUS mikrobus_uart_config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

	/* Enable GPIO TX/RX clock */
	USARTMIKROE_TX_GPIO_CLK_ENABLE();
	USARTMIKROE_RX_GPIO_CLK_ENABLE();

	/* Select SysClk as source of USART2 clocks */
	RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART16;
	RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

	/* Enable USARTUSB clock */
	USARTMIKROE_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = USARTMIKROE_TX_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = USARTMIKROE_TX_AF;

	HAL_GPIO_Init(USARTMIKROE_TX_GPIO_PORT, &GPIO_InitStruct);

	/* UART RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = USARTMIKROE_RX_PIN;
	GPIO_InitStruct.Alternate = USARTMIKROE_RX_AF;

	HAL_GPIO_Init(USARTMIKROE_RX_GPIO_PORT, &GPIO_InitStruct);

	/* NVIC for USARTUSB */
	HAL_NVIC_SetPriority(USARTMIKROE_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(USARTMIKROE_IRQn);

	Uart2Handle.Instance        = USARTMIKROE;
	Uart2Handle.Init.BaudRate   = 115200;
	Uart2Handle.Init.WordLength = UART_WORDLENGTH_8B;
	Uart2Handle.Init.StopBits   = UART_STOPBITS_1;
	Uart2Handle.Init.Parity     = UART_PARITY_NONE; //UART_PARITY_ODD;
	Uart2Handle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	Uart2Handle.Init.Mode       = UART_MODE_TX_RX;
	Uart2Handle.Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&Uart2Handle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	// inicializuj data prijmu a vysilani
	Tx2.buffer = (uint8_t *)&Tx2_buffer;
	Tx2.index = 0;
	Tx2.status = READYTOSEND;
	Rx2.buffer = (uint8_t *)&Rx2_buffer;
	Rx2.index = 0;
	Rx2.status = READYTORECEIVE;

	mikrobus_uart_send_message((uint8_t *)mikrobus_message, strlen((const char *)mikrobus_message));

	if(HAL_UART_Receive_IT(&Uart2Handle, (uint8_t *)Rx2.buffer, 1) != HAL_OK)
	{
		Error_Handler();
	}

	return RETURN_OK;
}

RETURN_STATUS mikrobus_uart_unconfig(void)
{
	/*##-1- Reset peripherals ##################################################*/
	USARTMIKROE_FORCE_RESET();
	USARTMIKROE_RELEASE_RESET();

	/*##-2- Disable peripherals and GPIO Clocks ################################*/
	/* Configure UART Tx as alternate function  */
	HAL_GPIO_DeInit(USARTMIKROE_TX_GPIO_PORT, USARTMIKROE_TX_PIN);
	/* Configure UART Rx as alternate function  */
	HAL_GPIO_DeInit(USARTMIKROE_RX_GPIO_PORT, USARTMIKROE_RX_PIN);

	/*##-3- Disable the NVIC for UART ##########################################*/
	HAL_NVIC_DisableIRQ(USARTMIKROE_IRQn);

	return RETURN_OK;
}

RETURN_STATUS mikrobus_uart_send_message(uint8_t *text, uint16_t len)
{
	usart_wait_until_sent((usart_data_tx_t *)&Tx2);

	Tx2.index = 0;
	Tx2.length = len;
	Tx2.status = TRANSFERING;

	memcpy(Tx2.buffer, (const char *)text, len);

	if(HAL_UART_Transmit_IT(&Uart2Handle, (uint8_t*)Tx2.buffer, len) != HAL_OK)
	{
		/* Transfer error in transmission process */
		Error_Handler();
	}
	return RETURN_OK;
}

// nastav mod UART na prijem - pro HAL to znamena nastavit prijem na ocekavani 1 znaku
void mikrobus_uart_set_receive_mode(void)
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

uint8_t *mikrobus_uart_get_rx_buffer(void)
{
	return (uint8_t *)&Rx2_buffer;
}
#endif // UART2_ENABLED

#ifdef I2C4_ENABLED
RETURN_STATUS mikrobus_i2c_config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	I2CMIKROE_SCL_GPIO_CLK_ENABLE();
	I2CMIKROE_SDA_GPIO_CLK_ENABLE();
	/* Enable I2Cx clock */
	I2CMIKROE_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* I2C TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = I2CMIKROE_SCL_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
	#if (EXTERNAL_PULL_UP_AVAILABLE == 0)
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	#else
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	#endif
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = I2CMIKROE_SCL_SDA_AF;
	HAL_GPIO_Init(I2CMIKROE_SCL_GPIO_PORT, &GPIO_InitStruct);

	/* I2C RX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = I2CMIKROE_SDA_PIN;
	GPIO_InitStruct.Alternate = I2CMIKROE_SCL_SDA_AF;
	HAL_GPIO_Init(I2CMIKROE_SDA_GPIO_PORT, &GPIO_InitStruct);

	/*##-3- Configure the NVIC for I2C #########################################*/
	/* NVIC for I2Cx */
	HAL_NVIC_SetPriority(I2CMIKROE_ER_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(I2CMIKROE_ER_IRQn);
	HAL_NVIC_SetPriority(I2CMIKROE_EV_IRQn, 0, 2);
	HAL_NVIC_EnableIRQ(I2CMIKROE_EV_IRQn);

	/*##-1- Configure the I2C peripheral ######################################*/
	I2c4Handle.Instance              = I2CMIKROE;
	I2c4Handle.Init.Timing           = I2C_GetTiming(HAL_RCC_GetPCLK2Freq(), BUS_I2C_MIKROE_FREQUENCY);
	I2c4Handle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
	I2c4Handle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
	I2c4Handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	I2c4Handle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
	I2c4Handle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
	I2c4Handle.Init.OwnAddress1      = MCU_I2C_ADDRESS;
	I2c4Handle.Init.OwnAddress2      = 0xFF;

	if(HAL_I2C_Init(&I2c4Handle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	/* Enable the Analog I2C Filter */
	HAL_I2CEx_ConfigAnalogFilter(&I2c4Handle, I2C_ANALOGFILTER_ENABLE);

//	i2c_buffer[0] = 0x40;
//	i2c_buffer[1] = 0x01;
//	uint8_t i;
//
//	for (i = 0; i < 10; i++)
//	{
//		mikrobus_i2c_write((0x44 << 1), (uint8_t *)i2c_buffer, 2);
//	}

	return RETURN_OK;
}

RETURN_STATUS mikrobus_i2c_unconfig(void)
{
	/*##-1- Reset peripherals ##################################################*/
	I2CMIKROE_FORCE_RESET();
	I2CMIKROE_RELEASE_RESET();

	/*##-2- Disable peripherals and GPIO Clocks #################################*/
	/* Configure I2C Tx as alternate function  */
	HAL_GPIO_DeInit(I2CMIKROE_SCL_GPIO_PORT, I2CMIKROE_SCL_PIN);
	/* Configure I2C Rx as alternate function  */
	HAL_GPIO_DeInit(I2CMIKROE_SDA_GPIO_PORT, I2CMIKROE_SDA_PIN);

	/*##-3- Disable the NVIC for I2C ##########################################*/
	HAL_NVIC_DisableIRQ(I2CMIKROE_ER_IRQn);
	HAL_NVIC_DisableIRQ(I2CMIKROE_EV_IRQn);

	return RETURN_OK;
}

// addr je nutne posunout o 1 bit doleva z puvodni 7bit adresy z datasheetu, posunem je nutne udelat misto pro R/W bit
// size je pocet bytu mimo adresu
RETURN_STATUS mikrobus_i2c_write(uint16_t addr, uint8_t *buffer, uint16_t size)
{
	uint16_t timeout = 0xffff;

	do
	{
		if(HAL_I2C_Master_Transmit_IT(&I2c4Handle, (uint16_t)addr, (uint8_t *)buffer, size) != HAL_OK)
		{
			/* Error_Handler() function is called when error occurs. */
			Error_Handler();
		}

		/*##-3- Wait for the end of the transfer #################################*/
		/*  Before starting a new communication transfer, you need to check the current
			state of the peripheral; if it's busy you need to wait for the end of current
			transfer before starting a new one.
			For simplicity reasons, this example is just waiting till the end of the
			transfer, but application may perform other tasks while transfer operation
			is ongoing. */
		while (HAL_I2C_GetState(&I2c4Handle) != HAL_I2C_STATE_READY)
		{
		}

		/* When Acknowledge failure occurs (Slave don't acknowledge it's address) Master restarts communication */

		if (!timeout--)
		{
			return RETURN_ERROR;
		}
	}
	while(HAL_I2C_GetError(&I2c4Handle) == HAL_I2C_ERROR_AF);

	return RETURN_OK;
}

// addr je nutne posunout o 1 bit doleva z puvodni 7bit adresy z datasheetu, posunem je nutne udelat misto pro R/W bit
// size je pocet bytu mimo adresu
RETURN_STATUS mikrobus_i2c_read(uint16_t addr, uint8_t *buffer, uint16_t size)
{
	uint16_t timeout = 0xffff;

	do
	{
		if(HAL_I2C_Master_Receive_IT(&I2c4Handle, (uint16_t)addr, (uint8_t *)buffer, size) != HAL_OK)
		{
			/* Error_Handler() function is called when error occurs. */
			Error_Handler();
		}

		/*##-5- Wait for the end of the transfer #################################*/
		/*  Before starting a new communication transfer, you need to check the current
			state of the peripheral; if it�s busy you need to wait for the end of current
			transfer before starting a new one.
			For simplicity reasons, this example is just waiting till the end of the
			transfer, but application may perform other tasks while transfer operation
			is ongoing. */
		while (HAL_I2C_GetState(&I2c4Handle) != HAL_I2C_STATE_READY)
		{
		}

		/* When Acknowledge failure occurs (Slave don't acknowledge it's address)
		   Master restarts communication */

		if (!timeout--)
		{
			return RETURN_ERROR;
		}
	}
	while(HAL_I2C_GetError(&I2c4Handle) == HAL_I2C_ERROR_AF);

	return RETURN_OK;
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  /* Transfer in transmission process is correct */
}


/**
  * @brief  Rx Transfer completed callback.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  /* Transfer in reception process is correct */
}


/**
  * @brief  I2C error callbacks.
  * @param  I2cHandle: I2C handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
	/** Error_Handler() function is called when error occurs.
	* 1- When Slave doesn't acknowledge its address, Master restarts communication.
	* 2- When Master doesn't acknowledge the last data transferred, Slave doesn't care in this example.
	*/
	if (HAL_I2C_GetError(I2cHandle) != HAL_I2C_ERROR_AF)
	{

	}
}
#endif // I2C4_ENABLED

#ifdef SPI1_ENABLED
RETURN_STATUS mikrobus_spi_config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	SPIMIKROE_SCK_GPIO_CLK_ENABLE();
	SPIMIKROE_MISO_GPIO_CLK_ENABLE();
	SPIMIKROE_MOSI_GPIO_CLK_ENABLE();
	SPIMIKROE_CS_GPIO_CLK_ENABLE();

	/* Enable SPI1 clock */
	SPIMIKROE_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* SPI SCK GPIO pin configuration  */
	GPIO_InitStruct.Pin       = SPIMIKROE_SCK_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = SPIMIKROE_SCK_AF;
	HAL_GPIO_Init(SPIMIKROE_SCK_GPIO_PORT, &GPIO_InitStruct);

	/* SPI MISO GPIO pin configuration  */
	GPIO_InitStruct.Pin = SPIMIKROE_MISO_PIN;
	GPIO_InitStruct.Alternate = SPIMIKROE_MISO_AF;
	HAL_GPIO_Init(SPIMIKROE_MISO_GPIO_PORT, &GPIO_InitStruct);

	/* SPI MOSI GPIO pin configuration  */
	GPIO_InitStruct.Pin = SPIMIKROE_MOSI_PIN;
	GPIO_InitStruct.Alternate = SPIMIKROE_MOSI_AF;
	HAL_GPIO_Init(SPIMIKROE_MOSI_GPIO_PORT, &GPIO_InitStruct);

	/* SPI /CS GPIO pin configuration  */
	GPIO_InitStruct.Pin = SPIMIKROE_CS_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SPIMIKROE_CS_GPIO_PORT, &GPIO_InitStruct);
	HAL_GPIO_WritePin(SPIMIKROE_CS_GPIO_PORT, SPIMIKROE_CS_PIN, GPIO_PIN_SET);		// nastav CS na HIGH


	/*##-3- Configure the NVIC for SPI #########################################*/
	/* NVIC for SPI */
	HAL_NVIC_SetPriority(SPIMIKROE_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(SPIMIKROE_IRQn);

	Spi1Handle.Instance               = SPIMIKROE;

	// zakladni hodiny jsou 200 MHz, delenim BaudRatePrescaler je nastavena perioda SCLK, pro 256 je to 781 kHz
	Spi1Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;

	Spi1Handle.Init.Direction         = SPI_DIRECTION_2LINES;
	Spi1Handle.Init.CLKPhase          = SPI_PHASE_1EDGE;
	Spi1Handle.Init.CLKPolarity       = SPI_POLARITY_LOW;
	Spi1Handle.Init.DataSize          = SPI_DATASIZE_8BIT;
	Spi1Handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	Spi1Handle.Init.TIMode            = SPI_TIMODE_DISABLE;
	Spi1Handle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	Spi1Handle.Init.CRCPolynomial     = 7;
	Spi1Handle.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;
	Spi1Handle.Init.NSS               = SPI_NSS_SOFT;
	Spi1Handle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
	Spi1Handle.Init.Mode = SPI_MODE_MASTER;

	if(HAL_SPI_Init(&Spi1Handle) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler();
	}

	spitx_buffer[0] = 0x45;
	spitx_buffer[1] = 0x23;
	uint8_t i;

	for (i = 0; i < 10; i++)
	{
		mikrobus_spi_transmitreceive((uint8_t *)spitx_buffer, (uint8_t *)spitx_buffer, 2);
	}

	return RETURN_OK;
}

RETURN_STATUS mikrobus_spi_unconfig(void)
{
	/*##-1- Reset peripherals ##################################################*/
	SPIMIKROE_FORCE_RESET();
	SPIMIKROE_RELEASE_RESET();

	/*##-2- Disable peripherals and GPIO Clocks ################################*/
	/* Deconfigure SPI SCK */
	HAL_GPIO_DeInit(SPIMIKROE_SCK_GPIO_PORT, SPIMIKROE_SCK_PIN);
	/* Deconfigure SPI MISO */
	HAL_GPIO_DeInit(SPIMIKROE_MISO_GPIO_PORT, SPIMIKROE_MISO_PIN);
	/* Deconfigure SPI MOSI */
	HAL_GPIO_DeInit(SPIMIKROE_MOSI_GPIO_PORT, SPIMIKROE_MOSI_PIN);
	/* Deconfigure SPI CS */
	HAL_GPIO_DeInit(SPIMIKROE_CS_GPIO_PORT, SPIMIKROE_CS_PIN);

	/*##-3- Disable the NVIC for SPI ###########################################*/
	HAL_NVIC_DisableIRQ(SPIMIKROE_IRQn);

	return RETURN_OK;
}

RETURN_STATUS mikrobus_spi_transmitreceive(uint8_t *tx_buffer, uint8_t *rx_buffer, uint32_t size)
{
	uint16_t timeout = 0xffff;

	HAL_GPIO_WritePin(SPIMIKROE_CS_GPIO_PORT, SPIMIKROE_CS_PIN, GPIO_PIN_RESET);		// nastav CS na LOW

	/*##-2- Start the Full Duplex Communication process ########################*/
	/* While the SPI in TransmitReceive process, user can transmit data through "Tx Buffer" buffer & receive data through "Rx Buffer" */
	if(HAL_SPI_TransmitReceive_IT(&Spi1Handle, (uint8_t*)tx_buffer, (uint8_t *)rx_buffer, size) != HAL_OK)
	{
		/* Transfer error in transmission process */
		Error_Handler();
	}

	/*##-3- Wait for the end of the transfer ###################################*/
	/*  Before starting a new communication transfer, you must wait the callback call
	  to get the transfer complete confirmation or an error detection.
	  For simplicity reasons, this example is just waiting till the end of the
	  transfer, but application may perform other tasks while transfer operation
	  is ongoing. */
	while (wTransferState == TRANSFER_WAIT)
	{
		if (!timeout--)
		{
			HAL_GPIO_WritePin(SPIMIKROE_CS_GPIO_PORT, SPIMIKROE_CS_PIN, GPIO_PIN_SET);	// nastav CS na HIGH
			return RETURN_ERROR;
		}
	}

	HAL_GPIO_WritePin(SPIMIKROE_CS_GPIO_PORT, SPIMIKROE_CS_PIN, GPIO_PIN_SET);		// nastav CS na HIGH

	return RETURN_OK;
}

/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of Interrupt TxRx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &Spi1Handle)
	{
		wTransferState = TRANSFER_COMPLETE;
	}
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &Spi1Handle)
	{
		wTransferState = TRANSFER_ERROR;
	}
}

#endif // SPI1_ENABLED


