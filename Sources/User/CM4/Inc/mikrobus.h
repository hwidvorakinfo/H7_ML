/*
 * mikrobus.h
 *
 *  Created on: 19. 5. 2021
 *      Author: Petr Dvorak
 */

#ifndef SOURCES_USER_CM4_INC_MIKROBUS_H_
#define SOURCES_USER_CM4_INC_MIKROBUS_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"
#include "main.h"
#include "application.h"
#include "uart_hal_cm4.h"

// UART
#define RX2BUFFERSIZE						128
#define TX2BUFFERSIZE						128

#define USARTMIKROE     	                USART2
#define USARTMIKROE_CLK_ENABLE()            __HAL_RCC_USART2_CLK_ENABLE()
#define USARTMIKROE_RX_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTMIKROE_TX_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTMIKROE_FORCE_RESET()           __HAL_RCC_USART2_FORCE_RESET()
#define USARTMIKROE_RELEASE_RESET()         __HAL_RCC_USART2_RELEASE_RESET()

/* Definition for USARTMIKROE Pins */
#define USARTMIKROE_TX_PIN                  GPIO_PIN_2
#define USARTMIKROE_TX_GPIO_PORT            GPIOA
#define USARTMIKROE_TX_AF                   GPIO_AF7_USART2
#define USARTMIKROE_RX_PIN                  GPIO_PIN_3
#define USARTMIKROE_RX_GPIO_PORT            GPIOA
#define USARTMIKROE_RX_AF                   GPIO_AF7_USART2

/* Definition for USARTMIKROE NVIC IRQ and IRQ Handlers */
#define USARTMIKROE_IRQn                    USART2_IRQn
#define USARTMIKROE_IRQHandler              USART2_IRQHandler

extern UART_HandleTypeDef Uart2Handle;

RETURN_STATUS mikrobus_uart_config(void);
RETURN_STATUS mikrobus_uart_send_message(uint8_t *text, uint16_t len);
void mikrobus_uart_set_receive_mode(void);
uint8_t *mikrobus_uart_get_rx_buffer(void);

// I2C
#define I2CBUFFERSIZE						10

/* Definition for I2Cx clock resources */
#define I2CMIKROE                           I2C4
#define I2CMIKROE_CLK_ENABLE()              __HAL_RCC_I2C4_CLK_ENABLE()
#define I2CMIKROE_SDA_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2CMIKROE_SCL_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2CMIKROE_FORCE_RESET()             __HAL_RCC_I2C4_FORCE_RESET()
#define I2CMIKROE_RELEASE_RESET()           __HAL_RCC_I2C4_RELEASE_RESET()

/* Definition for I2CMIKROE Pins */
#define I2CMIKROE_SCL_PIN                   GPIO_PIN_8
#define I2CMIKROE_SCL_GPIO_PORT             GPIOB
#define I2CMIKROE_SDA_PIN                   GPIO_PIN_7
#define I2CMIKROE_SDA_GPIO_PORT             GPIOB
#define I2CMIKROE_SCL_SDA_AF                GPIO_AF6_I2C4

/* Definition for I2CMIKROE's NVIC */
#define I2CMIKROE_EV_IRQn                   I2C4_EV_IRQn
#define I2CMIKROE_ER_IRQn                   I2C4_ER_IRQn
#define I2CMIKROE_EV_IRQHandler             I2C4_EV_IRQHandler
#define I2CMIKROE_ER_IRQHandler             I2C4_ER_IRQHandler

/* Definition for usage of internal pull up */
/* Enable this define, for an integration of this example inside
   an ecosystem board with external pull-up */
#define EXTERNAL_PULL_UP_AVAILABLE      	0

#define BUS_I2C_MIKROE_FREQUENCY   			400000U
#define MCU_I2C_ADDRESS        				0x01

extern I2C_HandleTypeDef I2c4Handle;

RETURN_STATUS mikrobus_i2c_config(void);
RETURN_STATUS mikrobus_i2c_unconfig(void);
RETURN_STATUS mikrobus_i2c_write(uint16_t addr, uint8_t *buffer, uint16_t size);
RETURN_STATUS mikrobus_i2c_read(uint16_t addr, uint8_t *buffer, uint16_t size);
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle);
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle);

// SPI
#define SPIBUFFERSIZE						10
/* Definition for SPIx clock resources */
#define SPIMIKROE							SPI1
#define SPIMIKROE_CLK_ENABLE()              __HAL_RCC_SPI1_CLK_ENABLE()
#define SPIMIKROE_SCK_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIMIKROE_MISO_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIMIKROE_MOSI_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIMIKROE_CS_GPIO_CLK_ENABLE()    	__HAL_RCC_GPIOA_CLK_ENABLE()

#define SPIMIKROE_FORCE_RESET()             __HAL_RCC_SPI1_FORCE_RESET()
#define SPIMIKROE_RELEASE_RESET()           __HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIMIKROE_SCK_PIN                   GPIO_PIN_5
#define SPIMIKROE_SCK_GPIO_PORT             GPIOA
#define SPIMIKROE_SCK_AF                    GPIO_AF5_SPI1
#define SPIMIKROE_MISO_PIN                  GPIO_PIN_6
#define SPIMIKROE_MISO_GPIO_PORT            GPIOA
#define SPIMIKROE_MISO_AF                   GPIO_AF5_SPI1
#define SPIMIKROE_MOSI_PIN                  GPIO_PIN_7
#define SPIMIKROE_MOSI_GPIO_PORT            GPIOA
#define SPIMIKROE_MOSI_AF                   GPIO_AF5_SPI1
#define SPIMIKROE_CS_PIN                  	GPIO_PIN_4
#define SPIMIKROE_CS_GPIO_PORT            	GPIOA

/* Definition for SPIx's NVIC */
#define SPIMIKROE_IRQn                      SPI1_IRQn
#define SPIMIKROE_IRQHandler                SPI1_IRQHandler

enum {
	TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};

extern SPI_HandleTypeDef Spi1Handle;

RETURN_STATUS mikrobus_spi_config(void);
RETURN_STATUS mikrobus_spi_unconfig(void);
RETURN_STATUS mikrobus_spi_transmitreceive(uint8_t *tx_buffer, uint8_t *rx_buffer, uint32_t size);
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi);

// Reset
#define MIKROE_RESET_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define MIKROE_RESET_PIN                   GPIO_PIN_0
#define MIKROE_RESET_GPIO_PORT             GPIOA

RETURN_STATUS mikrobus_reset_config(void);
RETURN_STATUS mikrobus_reset_low(void);
RETURN_STATUS mikrobus_reset_high(void);

#define MIKROE_INT_GPIO_CLK_ENABLE()     	__HAL_RCC_GPIOA_CLK_ENABLE()
#define MIKROE_INT_PIN                   	GPIO_PIN_1
#define MIKROE_INT_GPIO_PORT             	GPIOA
#define MIKROE_INT_EXTILINE				 	EXTI_LINE_1
#define MIKROE_INT_SLOPE					GPIO_MODE_IT_FALLING

RETURN_STATUS mikrobus_int_config(void);
void mikrobus_int_handler(void);
void mikrobus_int_cb(void);

// PWM
/* Definition for TIMx clock resources */
#define TIMPWMMIKROE                        TIM8
#define TIMPWMMIKROE_CLK_ENABLE()           __HAL_RCC_TIM8_CLK_ENABLE()

/* Definition for TIMx Channel Pins */
#define TIMPWMMIKROE_CHANNEL_GPIO_PORT()    __HAL_RCC_GPIOB_CLK_ENABLE();
#define TIMPWMMIKROE_GPIO_PORT_CHANNEL2     GPIOB
#define TIMPWMMIKROE_GPIO_PIN_CHANNEL2      GPIO_PIN_0
#define TIMPWMMIKROE_GPIO_AF_CHANNEL2       GPIO_AF3_TIM8

#define MIKROEPWMPERIOD_VALUE       		(uint32_t)(1000 - 1)    /* Period Value  */

RETURN_STATUS mikrobus_pwm_config(void);
RETURN_STATUS mikrobus_pwm_set_duty(uint16_t duty);

#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM4_INC_MIKROBUS_H_ */
