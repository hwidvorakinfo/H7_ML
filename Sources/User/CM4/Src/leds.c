/*
 * leds.c
 *
 *  Created on: 24. 5. 2021
 *      Author: Petr Dvorak
 */

#include "main.h"
#include "application.h"
#include "defs.h"
#include "error_handler.h"
#include "leds.h"

RETURN_STATUS leds_config(void)
{
	LEDC_GPIO_CLK_ENABLE();
	LEDD_GPIO_CLK_ENABLE();

	GPIO_InitTypeDef gpio_init_structure;
	gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull = GPIO_PULLUP;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_structure.Pin = LEDC_PIN;
	HAL_GPIO_Init(LEDC_GPIO_PORT, &gpio_init_structure);

	gpio_init_structure.Pin = LEDD_PIN;
	HAL_GPIO_Init(LEDD_GPIO_PORT, &gpio_init_structure);

	HAL_GPIO_WritePin(LEDC_GPIO_PORT, LEDC_PIN, GPIO_PIN_SET);		// rozsvit LEDC

	HAL_GPIO_WritePin(LEDD_GPIO_PORT, LEDD_PIN, GPIO_PIN_SET);		// rozsvit LEDD

  return RETURN_OK;
}
