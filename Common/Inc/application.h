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
#define UART2_ENABLED

// UART1
#ifdef UART1_ENABLED
#endif // UART1_ENABLED

// UART2
#ifdef UART2_ENABLED
#endif // UART2_ENABLED

 #endif // UART_ENABLED


#define I2C_ENABLED
#ifdef I2C_ENABLED

//#define I2C1_ENABLED
//#define I2C2_ENABLED
//#define I2C3_ENABLED
#define I2C4_ENABLED

// I2C1
#ifdef I2C1_ENABLED
#endif // I2C1_ENABLED

 // I2C2
 #ifdef I2C2_ENABLED
 #endif // I2C2_ENABLED

 // I2C3
 #ifdef I2C3_ENABLED
 #endif // I2C3_ENABLED

 // I2C4
 #ifdef I2C4_ENABLED
 #endif // I2C4_ENABLED

#endif // I2C_ENABLED

#define SPI_ENABLED
#ifdef SPI_ENABLED
#define SPI1_ENABLED

#ifdef SPI1_ENABLED
#endif // SPI1_ENABLED

#endif // SPI_ENABLED

// vstupy ADC
#define ADC_INIT
#ifdef ADC_INIT

#define ADC                    		  ADC1
#define ADC_CLK_ENABLE()              __HAL_RCC_ADC12_CLK_ENABLE()
#define ADC_CHANNEL_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADC_CHANNEL_PIN_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()

#define TIM_ADC_SYNC				  TIM3
#define TIM_ADC_SYNC_CLK_ENABLE()     __HAL_RCC_TIM3_CLK_ENABLE()
#define TIM_ADC_SYNC_IRQn             TIM3_IRQn
#define TIM_ADC_SYNC_IRQHandler       TIM3_IRQHandler
#define TIM_ADC_SYNC_TRIGGER		  ADC_EXTERNALTRIG_T3_TRGO

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
