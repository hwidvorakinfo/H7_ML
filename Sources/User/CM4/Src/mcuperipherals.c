/*
 * mcuperipherals.c
 *
 *  Created on: Dec 29, 2014
 *      Author: daymoon
 */

#include "mcuperipherals.h"
#include "main.h"

//#include "gpio.h"
//#include "timer.h"
#include "uart_hal_cm4.h"
#include "adc_hal_cm4.h"
#include "leds.h"
//#include "i2c.h"
//#include "spi.h"
//#include "application.h"

// inicializuje vsechny mozne vystupy na vsech portech a vstupy do ADC
void mcuperipherals_init(void)
{
	//gpio_init();
	//adc_init();
	//timer_timebase_init();
	//spi_init();
	//usart_config();

	if (leds_config() != RETURN_OK)
	{
		Error_Handler();
	}

	// konfigurace UART
	if (uart_config() != RETURN_OK)
	{
	    Error_Handler();
	}
	else
	{
		uart_hello();
	}
	//i2c_init();
}
