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

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc);
RETURN_STATUS adc_dma_config(void);

uint32_t adc_get_ain1(void);
uint32_t adc_get_ain2(void);
uint32_t adc_get_ain3(void);

#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM4_INC_ADC_HAL_CM4_H_ */
