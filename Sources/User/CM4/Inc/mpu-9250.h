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
  	int16_t magx __attribute__((__packed__));
  	int16_t magy __attribute__((__packed__));
  	int16_t magz __attribute__((__packed__));
 } accgyro_data_t;

#define MPU9250ENABLED

#define MPU9250I2CADDR							(0x68 << 1)
#define MPUMAGNETOADDR							(0x0C << 1)

#define MPU_AVG_NUMBER							10

// registry
#define MPU_WHOAMI_REG							(0x75)
#define MPU_SMPLRTDIV							(0x19)
#define MPU_CONFIG								(0x1A)
#define MPU_GYROCONF							(0x1B)
#define MPU_ACCCONF								(0x1C)
#define MPU_ACCCONF2							(0x1D)
#define MPU_PWMMNGT1							(0x6B)
#define MPU_PWMMNGT2							(0x6C)
#define MPU_INTBYPASS							(0x37)

#define MPU_ACCXH								(0x3B)
#define MPU_ACCYH								(0x3D)
#define MPU_ACCZH								(0x3F)
#define MPU_GYRXH								(0x43)
#define MPU_GYRYH								(0x45)
#define MPU_GYRZH								(0x47)
#define MPU_TEMPH								(0x41)

#define MPU_MAG_WHOAMI							(0x00)
#define MPU_MAG_CNTL1							(0x0A)

#define MPU_GYRO_OFS_XH							(0x13)
#define MPU_GYRO_OFS_XL							(0x14)
#define MPU_GYRO_OFS_YH							(0x15)
#define MPU_GYRO_OFS_YL							(0x16)
#define MPU_GYRO_OFS_ZH							(0x17)
#define MPU_GYRO_OFS_ZL							(0x18)

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
#define MPU_GYRO_BW_8K8HZ						(7)
#define MPU_GYRO_BW_3K6HZ						(7)
#define MPU_GYRO_BW_250HZ						(0)
#define MPU_GYRO_BW_184HZ						(1)
#define MPU_GYRO_BW_92HZ						(2)
#define MPU_GYRO_BW_41HZ						(3)
#define MPU_GYRO_BW_20HZ						(4)
#define MPU_GYRO_BW_10HZ						(5)
#define MPU_GYRO_BW_5HZ							(6)
#define MPU_ACC_HIGHSPEED						(8)
#define MPU_ACC_BW_1000HZ						(0+MPU_ACC_HIGHSPEED)
#define MPU_ACC_BW_218HZ						(0)
#define MPU_ACC_BW_99HZ							(2)
#define MPU_ACC_BW_45HZ							(3)
#define MPU_ACC_BW_21HZ							(4)
#define MPU_ACC_BW_10HZ							(5)
#define MPU_ACC_BW_5HZ							(6)
#define MPU_ACC_BW_420HZ						(7)
#define MPU_BYPASSEN							(2)

#define MPU_MAG_WHOAMIRESPONSE					(0x48)
#define MPU_MAG_16B_CONT1						(0x12)
#define MPU_MAG_16B_CONT2						(0x16)
#define MPU_MAG_14B_SELF						(0x08)
#define MPU_MAG_14B_CONT2						(0x06)
#define MPU_MAGXL								(0x03)
#define MPU_MAGYL								(0x05)
#define MPU_MAGZL								(0x07)

// parametry prenosu
#define MPU_ACC_DATA_LEN						(6)
#define MPU_GYRO_DATA_LEN						(6)
#define MPU_TEMP_DATA_LEN						(2)
#define MPU_MAG_DATA_LEN						(7)
#define MPU_ACC_OFFSET							(0)
#define MPU_TEMP_OFFSET							(6)
#define MPU_GYRO_OFFSET							(8)
#define MPU_MAG_OFFSET							(0)
#define MPU_DATA_LEN							(MPU_ACC_DATA_LEN+MPU_TEMP_DATA_LEN+MPU_GYRO_DATA_LEN)

RETURN_STATUS mpu_whoami(void);
RETURN_STATUS mpu_init(void);
RETURN_STATUS mpu_config(void);
RETURN_STATUS mpu_set_acc_range(uint8_t range);
RETURN_STATUS mpu_set_gyro_range(uint8_t range);
RETURN_STATUS mpu_read_accgyro(void);
accgyro_data_t *mpu_get_data(void);
RETURN_STATUS mpu_gyro_tara(void);

RETURN_STATUS mpu_mag_whoami(void);
RETURN_STATUS mpu_mag_config(void);

#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM4_INC_MPU_9250_H_ */
