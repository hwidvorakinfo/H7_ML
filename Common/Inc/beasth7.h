/*
 * beasth7.h
 *
 *  Created on: 23. 3. 2021
 *      Author: Petr Dvorak
 */

#ifndef EXAMPLE_USER_BEASTH7_H_
#define EXAMPLE_USER_BEASTH7_H_

#define BUTTON1_PIN						GPIO_PIN_2
#define BUTTON1_GPIO_PORT				GPIOD
#define BUTTON1_GPIO_CLK_ENABLE()      	__HAL_RCC_GPIOD_CLK_ENABLE()
#define BUTTON1_GPIO_CLK_DISABLE()     	__HAL_RCC_GPIOD_CLK_DISABLE()
#define BUTTON1_EXTI_IRQn              	EXTI2_IRQn
#define BUTTON1_EXTI_LINE              	EXTI_LINE_2

#define BUTTON2_PIN						GPIO_PIN_3
#define BUTTON2_GPIO_PORT				GPIOD
#define BUTTON2_GPIO_CLK_ENABLE()      	__HAL_RCC_GPIOD_CLK_ENABLE()
#define BUTTON2_GPIO_CLK_DISABLE()     	__HAL_RCC_GPIOD_CLK_DISABLE()
#define BUTTON2_EXTI_IRQn              	EXTI2_IRQn
#define BUTTON2_EXTI_LINE              	EXTI_LINE_3

#define LEDA_GPIO_PORT                   GPIOE
#define LEDA_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOE_CLK_ENABLE()
#define LEDA_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOE_CLK_DISABLE()
#define LEDA_PIN                         GPIO_PIN_5

#define LEDB_GPIO_PORT                   GPIOI
#define LEDB_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOI_CLK_ENABLE()
#define LEDB_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOI_CLK_DISABLE()
#define LEDB_PIN                         GPIO_PIN_14

#define LEDC_GPIO_PORT                   GPIOI
#define LEDC_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOI_CLK_ENABLE()
#define LEDC_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOI_CLK_DISABLE()
#define LEDC_PIN                         GPIO_PIN_13

#define LEDD_GPIO_PORT                   GPIOI
#define LEDD_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOI_CLK_ENABLE()
#define LEDD_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOI_CLK_DISABLE()
#define LEDD_PIN                         GPIO_PIN_12

#endif /* EXAMPLE_USER_BEASTH7_H_ */
