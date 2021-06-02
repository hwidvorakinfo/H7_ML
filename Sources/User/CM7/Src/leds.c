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
	LEDA_GPIO_CLK_ENABLE();
	LEDB_GPIO_CLK_ENABLE();

	GPIO_InitTypeDef gpio_init_structure;
	gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull = GPIO_PULLUP;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_structure.Pin = LEDA_PIN;						// pin
	HAL_GPIO_Init(LEDA_GPIO_PORT, &gpio_init_structure);	// init portu

	gpio_init_structure.Pin = LEDB_PIN;						// pin
	HAL_GPIO_Init(LEDB_GPIO_PORT, &gpio_init_structure);	// init portu

	HAL_GPIO_WritePin(LEDA_GPIO_PORT, LEDA_PIN, GPIO_PIN_SET);		// zhasni LEDA
	HAL_GPIO_WritePin(LEDB_GPIO_PORT, LEDB_PIN, GPIO_PIN_SET);		// zhasni LEDB

  return RETURN_OK;
}
