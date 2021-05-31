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

 typedef struct {
  	int16_t accx __attribute__((__packed__));
  	int16_t accy __attribute__((__packed__));
  	int16_t accz __attribute__((__packed__));
  	int16_t temp __attribute__((__packed__));
  	int16_t gyrox __attribute__((__packed__));
  	int16_t gyroy __attribute__((__packed__));
  	int16_t gyroz __attribute__((__packed__));
 } accgyro_data_t;

#define MPU9250ENABLED

#define MPU9250I2CADDR							(0x68 << 1)

// registry
#define MPU_WHOAMI_REG							(0x75)
#define MPU_SMPLRTDIV							(0x19)
#define MPU_GYROCONF							(0x1B)
#define MPU_ACCCONF								(0x1C)
#define MPU_ACCCONF2							(0x1D)
#define MPU_PWMMNGT1							(0x6B)
#define MPU_PWMMNGT2							(0x6C)

#define MPU_ACCXH								(0x3B)
#define MPU_ACCYH								(0x3D)
#define MPU_ACCZH								(0x3F)
#define MPU_GYRXH								(0x43)
#define MPU_GYRYH								(0x45)
#define MPU_GYRZH								(0x47)
#define MPU_TEMPH								(0x41)

// obsah registru
#define MPU9250WHOAMIRESPONSE					(0x71)
#define MPU_ACC_RANGE_2G						(0)
#define MPU_ACC_RANGE_4G						(1)
#define MPU_ACC_RANGE_8G						(2)
#define MPU_ACC_RANGE_16G						(3)
#define MPU_GYRO_RANGE_250DPS					(0)
#define MPU_GYRO_RANGE_500DPS					(1)
#define MPU_GYRO_RANGE_1000DPS					(2)
#define MPU_GYRO_RANGE_2000DPS					(3)

#define MPU_ACC_DATA_LEN						(6)
#define MPU_GYRO_DATA_LEN						(6)
#define MPU_TEMP_DATA_LEN						(2)
#define MPU_ACC_OFFSET							(0)
#define MPU_TEMP_OFFSET							(3)
#define MPU_GYRO_OFFSET							(4)
#define MPU_DATA_LEN							(MPU_ACC_DATA_LEN+MPU_TEMP_DATA_LEN+MPU_GYRO_DATA_LEN)

RETURN_STATUS mpu_whoami(void);
RETURN_STATUS mpu_init(void);
RETURN_STATUS mpu_config(void);
RETURN_STATUS mpu_set_acc_range(uint8_t range);
RETURN_STATUS mpu_set_gyro_range(uint8_t range);
RETURN_STATUS mpu_read_accgyro(void);
accgyro_data_t *mpu_get_data(void);

#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM4_INC_MPU_9250_H_ */
