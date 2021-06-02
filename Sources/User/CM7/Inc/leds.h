/*
 * leds.h
 *
 *  Created on: 24. 5. 2021
 *      Author: Petr Dvorak
 */

#ifndef SOURCES_USER_CM4_INC_LEDS_H_
#define SOURCES_USER_CM4_INC_LEDS_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"
#include "main.h"
#include "application.h"
#include "beasth7.h"

RETURN_STATUS leds_config(void);

#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM4_INC_LEDS_H_ */
