/*
 * application.h
 *
 *  Created on: 22. 4. 2021
 *      Author: Petr Dvorak
 */

#ifndef COMMON_INC_APPLICATION_H_
#define COMMON_INC_APPLICATION_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define UART_ENABLED
#ifdef UART_ENABLED

#define UART1_ENABLED
//#define UART2_ENABLED

// UART1
#ifdef UART1_ENABLED

#define UARTUSB_HANDLE		Uart1Handle

#endif // UART1_ENABLED

// UART2
#ifdef UART2_ENABLED
#define DEBUG_UART			USART2
#define USART2_TX_CLOCKS	RCC_AHBPeriph_GPIOA
#define USART2_TX_PORT		GPIOA
#define USART2_TX_PIN		GPIO_Pin_2
#define USART2_TX_SOURCE	GPIO_PinSource2
#define USART2_TX_AF		GPIO_AF_1

#define USART2_RX_CLOCKS	RCC_AHBPeriph_GPIOA
#define USART2_RX_PORT		GPIOA
#define USART2_RX_PIN		GPIO_Pin_3
#define USART2_RX_SOURCE	GPIO_PinSource3
#define USART2_RX_AF		GPIO_AF_1

//#define USART2_DE_CLOCKS	RCC_AHBPeriph_GPIOA
//#define USART2_DE_PORT	GPIOA
//#define USART2_DE_PIN		GPIO_Pin_1
//#define USART2_DE_SOURCE	GPIO_PinSource1
//#define USART2_DE_AF		GPIO_AF_1

// prototypy ovladacich funkci
#define DEBUG_SEND_TEXT		usart2_send_text
#define DEBUG_NEW_LINE		usart2_newline
#define DEBUG_GET_DATA		get_usart2_Rx_buffer
#define DEBUG_RELEASE_RX	usart2_release_Rx_buffer
#define DEBUG_SET_UART_MODE	usart2_set_mode
#define DEBUG_GET_UART_MODE	usart2_get_mode
#endif // UART2_ENABLED

#endif // UART_ENABLED

// vstupy ADC
#define ADC_INIT
#ifdef ADC_INIT

#define ADC                    		  ADC1
#define ADC_CLK_ENABLE()              __HAL_RCC_ADC12_CLK_ENABLE()
#define ADC_CHANNEL_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADC_CHANNEL_PIN_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()

// port PAx


// port PBx

// port PCx
#define INPUT1_ADC_PIN                GPIO_PIN_0
#define INPUT2_ADC_PIN                GPIO_PIN_1
#define INPUT3_ADC_PIN                GPIO_PIN_2
#define ADC_CHANNEL_PINS			  INPUT1_ADC_PIN | INPUT2_ADC_PIN | INPUT3_ADC_PIN
#define ADC_GPIO_PORT       		  GPIOC

#define INPUT1_CHANNEL                ADC_CHANNEL_10
#define INPUT2_CHANNEL                ADC_CHANNEL_11
#define INPUT3_CHANNEL                ADC_CHANNEL_12

// DMA+ADC

#endif // ADC_INIT

#ifdef __cplusplus
}
#endif

#endif /* COMMON_INC_APPLICATION_H_ */
