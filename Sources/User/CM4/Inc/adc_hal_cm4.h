/*
 * adc_hal_cm4.h
 *
 *  Created on: 17. 5. 2021
 *      Author: Petr Dvorak
 */

#ifndef SOURCES_USER_CM4_INC_ADC_HAL_CM4_H_
#define SOURCES_USER_CM4_INC_ADC_HAL_CM4_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"
#include "main.h"
#include "application.h"

#define DMAx_CHANNELx_CLK_ENABLE()      __HAL_RCC_DMA1_CLK_ENABLE()

#define ADCx_FORCE_RESET()              __HAL_RCC_ADC12_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __HAL_RCC_ADC12_RELEASE_RESET()

extern ADC_HandleTypeDef AdcHandle;
extern TIM_HandleTypeDef TimHandle_adc_sync;

// prescalery a periody pro spousteni casovace taktujiciho ADC
#define PRESC_100HZ						2000
#define PERIO_100HZ						1000
//
#define PRESC_1HZ						20000
#define PERIO_1HZ						10000

#define NUMBER_OF_ADC_CHANNELS			3

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc);
RETURN_STATUS adc_dma_config(uint32_t file_address);
RETURN_STATUS adc_dma_start(void);
RETURN_STATUS adc_dma_unconfig(void);
RETURN_STATUS adc_sync_timer_init(uint32_t freq);
RETURN_STATUS adc_sync_timer_deinit(void);

uint32_t adc_get_ain1(void);
uint32_t adc_get_ain2(void);
uint32_t adc_get_ain3(void);

#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM4_INC_ADC_HAL_CM4_H_ */
