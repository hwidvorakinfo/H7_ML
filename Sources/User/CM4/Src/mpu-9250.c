/*
 * mpu-9250.c
 *
 *  Created on: 27. 5. 2021
 *      Author: Petr Dvorak
 */

#include "main.h"
#include "application.h"
#include "defs.h"
#include "error_handler.h"
#include "beasth7.h"
#include "mpu-9250.h"
#include "mikrobus.h"

static volatile uint8_t i2c_buffer[MPU_DATA_LEN];

//
volatile accgyro_data_t accgyro_data;

RETURN_STATUS mpu_whoami(void)
{
	i2c_buffer[0] = (MPU_WHOAMI_REG); 								// WHOAMI registr

	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 1);
	mikrobus_i2c_read(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 1);

	if (i2c_buffer[0] == MPU9250WHOAMIRESPONSE)
	{
		mpu_config();
		mpu_set_acc_range(MPU_ACC_RANGE_2G);
		mpu_set_gyro_range(MPU_GYRO_RANGE_250DPS);

		return RETURN_OK;
	}

	return RETURN_ERROR;
}

RETURN_STATUS mpu_init(void)
{
	if (mpu_whoami() == RETURN_OK)
	{
		return RETURN_OK;
	}

	return RETURN_ERROR;
}

RETURN_STATUS mpu_config(void)
{
	// MPU_SMPLRTDIV registr
	i2c_buffer[0] = (MPU_SMPLRTDIV);
	i2c_buffer[1] = 0;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	// MPU_GYROCONF registr
	i2c_buffer[0] = (MPU_GYROCONF);
	i2c_buffer[1] = 0;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	// MPU_ACCCONF registr
	i2c_buffer[0] = (MPU_ACCCONF);
	i2c_buffer[1] = 0;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	// MPU_ACCCONF2 registr
	i2c_buffer[0] = (MPU_ACCCONF2);
	i2c_buffer[1] = 0;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	// MPU_PWMMNGT1 registr
	i2c_buffer[0] = (MPU_PWMMNGT1);
	i2c_buffer[1] = 0;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	// MPU_PWMMNGT2 registr
	i2c_buffer[0] = (MPU_PWMMNGT2);
	i2c_buffer[1] = 0;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	return RETURN_ERROR;
}

RETURN_STATUS mpu_set_acc_range(uint8_t range)
{
	// MPU_ACCCONF registr
	i2c_buffer[0] = (MPU_ACCCONF);
	i2c_buffer[1] = (range << 3) & 0x18;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	return RETURN_ERROR;
}

RETURN_STATUS mpu_set_gyro_range(uint8_t range)
{
	// MPU_GYROCONF registr
	i2c_buffer[0] = (MPU_GYROCONF);
	i2c_buffer[1] = (range << 3) & 0x18;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	return RETURN_ERROR;
}

RETURN_STATUS mpu_read_accgyro(void)
{
	i2c_buffer[0] = (MPU_ACCXH); 								// MPU_ACCXH registr
	if (mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 1) != RETURN_OK)
	{
		return RETURN_ERROR;
	}

	// cti data
	if (mikrobus_i2c_read(MPU9250I2CADDR, (uint8_t *)i2c_buffer, MPU_DATA_LEN) != RETURN_OK)
	{
		return RETURN_ERROR;
	}

	accgyro_data.accx = (i2c_buffer[MPU_ACC_OFFSET] << 8) + i2c_buffer[MPU_ACC_OFFSET+1];
	accgyro_data.accy = (i2c_buffer[MPU_ACC_OFFSET+2] << 8) + i2c_buffer[MPU_ACC_OFFSET+3];
	accgyro_data.accz = (i2c_buffer[MPU_ACC_OFFSET+4] << 8) + i2c_buffer[MPU_ACC_OFFSET+5];

	accgyro_data.gyrox = (i2c_buffer[MPU_GYRO_OFFSET] << 8) + i2c_buffer[MPU_GYRO_OFFSET+1];
	accgyro_data.gyroy = (i2c_buffer[MPU_GYRO_OFFSET+2] << 8) + i2c_buffer[MPU_GYRO_OFFSET+3];
	accgyro_data.gyroz = (i2c_buffer[MPU_GYRO_OFFSET+4] << 8) + i2c_buffer[MPU_GYRO_OFFSET+5];

	accgyro_data.temp = (i2c_buffer[MPU_TEMP_OFFSET] << 8) + i2c_buffer[MPU_TEMP_OFFSET+1];

	return RETURN_OK;
}

accgyro_data_t *mpu_get_data(void)
{
	return (accgyro_data_t *)&accgyro_data;
}

