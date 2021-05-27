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

static volatile uint8_t i2c_buffer[I2CBUFFERSIZE];

RETURN_STATUS mpu_whoami(void)
{
	i2c_buffer[0] = (MPU_WHOAMI_REG); 								// WHOAMI registr

	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 1);
	mikrobus_i2c_read(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 1);

	if (i2c_buffer[0] == MPU9250WHOAMIRESPONSE)
	{
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
