/*
 * mpu-9250.h
 *
 *  Created on: 27. 5. 2021
 *      Author: Petr Dvorak
 */

#ifndef SOURCES_USER_CM4_INC_MPU_9250_H_
#define SOURCES_USER_CM4_INC_MPU_9250_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"
#include "main.h"
#include "application.h"

//#define MPU9250ENABLED

#define MPU9250I2CADDR							(0x68 << 1)

// registry
#define MPU_WHOAMI_REG							(0x75)

// obsah registru
#define MPU9250WHOAMIRESPONSE					(0x71)

RETURN_STATUS mpu_whoami(void);
RETURN_STATUS mpu_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM4_INC_MPU_9250_H_ */
