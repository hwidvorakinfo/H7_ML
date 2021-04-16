/*
 * crc_hal_cm7.c
 *
 *  Created on: 15. 4. 2021
 *      Author: Petr Dvorak
 */

#ifndef SOURCES_USER_CM7_INC_CRC_HAL_CM7_C_
#define SOURCES_USER_CM7_INC_CRC_HAL_CM7_C_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"


extern CRC_HandleTypeDef hcrc;

RETURN_STATUS crc_config(void);

#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM7_INC_CRC_HAL_CM7_C_ */
