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
#include "dataacq_types.h"

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

		// ACC range config
#ifdef ACC_RANGE_2G
		mpu_set_acc_range(MPU_ACC_RANGE_2G);
#endif // ACC_RANGE_2G
#ifdef ACC_RANGE_4G
		mpu_set_acc_range(MPU_ACC_RANGE_4G);
#endif // ACC_RANGE_4G
#ifdef ACC_RANGE_8G
		mpu_set_acc_range(MPU_ACC_RANGE_8G);
#endif // ACC_RANGE_8G
#ifdef ACC_RANGE_16G
		mpu_set_acc_range(MPU_ACC_RANGE_16G);
#endif // ACC_RANGE_16G

		// GYRO range config
#ifdef GYRO_RANGE_250
		mpu_set_gyro_range(MPU_GYRO_RANGE_250DPS);
#endif // GYRO_RANGE_250
#ifdef GYRO_RANGE_500
		mpu_set_gyro_range(MPU_GYRO_RANGE_500DPS);
#endif // GYRO_RANGE_500
#ifdef GYRO_RANGE_1000
		mpu_set_gyro_range(MPU_GYRO_RANGE_1000DPS);
#endif // GYRO_RANGE_1000
#ifdef GYRO_RANGE_2000
		mpu_set_gyro_range(MPU_GYRO_RANGE_2000DPS);
#endif // GYRO_RANGE_2000

		return RETURN_OK;
	}

	return RETURN_ERROR;
}

RETURN_STATUS mpu_mag_whoami(void)
{
	i2c_buffer[0] = (MPU_MAG_WHOAMI); 								// WHOAMI registr

	mikrobus_i2c_write(MPUMAGNETOADDR, (uint8_t *)i2c_buffer, 1);
	mikrobus_i2c_read(MPUMAGNETOADDR, (uint8_t *)i2c_buffer, 1);

	if (i2c_buffer[0] == MPU_MAG_WHOAMIRESPONSE)
	{
		mpu_mag_config();

		return RETURN_OK;
	}

	return RETURN_ERROR;
}

RETURN_STATUS mpu_init(void)
{
	volatile uint8_t ret = RETURN_ERROR;

	// ACC+GYRO
	if (mpu_whoami() == RETURN_OK)
	{
		ret = RETURN_OK;
	}

	// MAGNETOMETER
	if (mpu_mag_whoami() == RETURN_OK)
	{
		if (ret == RETURN_OK)
		{
			return RETURN_OK;
		}
	}

	return RETURN_ERROR;
}

RETURN_STATUS mpu_config(void)
{
	// MPU_SMPLRTDIV registr
	i2c_buffer[0] = (MPU_SMPLRTDIV);
	i2c_buffer[1] = 0;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	// MPU_CONFIG registr
	i2c_buffer[0] = (MPU_CONFIG);
	i2c_buffer[1] = MPU_GYRO_BW_10HZ;
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
	i2c_buffer[1] = MPU_ACC_BW_5HZ;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	// MPU_PWMMNGT1 registr
	i2c_buffer[0] = (MPU_PWMMNGT1);
	i2c_buffer[1] = 0;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	// MPU_PWMMNGT2 registr
	i2c_buffer[0] = (MPU_PWMMNGT2);
	i2c_buffer[1] = 0;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	// MPU_INTBYPASS registr
	i2c_buffer[0] = (MPU_INTBYPASS);
	i2c_buffer[1] = MPU_BYPASSEN;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 2);

	// tara pro gyroskop
	mpu_gyro_tara();

	return RETURN_ERROR;
}

RETURN_STATUS mpu_gyro_tara(void)
{
	// tara pro gyroskop
	volatile int32_t offsx = 0;
	volatile int32_t offsy = 0;
	volatile int32_t offsz = 0;
	accgyro_data_t *p_values = (accgyro_data_t *)mpu_get_data();
	uint8_t i;
	volatile uint8_t num = 0;

	// vynuluj offsety v senzoru
	i2c_buffer[0] = MPU_GYRO_OFS_XH;
	i2c_buffer[1] = 0;
	i2c_buffer[2] = 0;
	i2c_buffer[3] = 0;
	i2c_buffer[4] = 0;
	i2c_buffer[5] = 0;
	i2c_buffer[6] = 0;
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 7);

	// prumeruj nactene hodnoty MPU_AVG_NUMBER-krat
	for (i = 0; i < MPU_AVG_NUMBER; i++)
	{
		if (mpu_read_accgyro() == RETURN_OK)
		{
			offsx += p_values->gyrox;
			offsy += p_values->gyroy;
			offsz += p_values->gyroz;
			num++;
		}
	}
	// prepocet offsetu na korekcni hodnotu
	offsx /= (-4*num);
	offsy /= (-4*num);
	offsz /= (-4*num);

	// zapis nove hodnoty do offsetu
	i2c_buffer[0] = MPU_GYRO_OFS_XH;
	i2c_buffer[1] = ((offsx >> 8) & 0xFF);
	i2c_buffer[2] = (offsx & 0xFF);
	i2c_buffer[3] = ((offsy >> 8) & 0xFF);
	i2c_buffer[4] = (offsy & 0xFF);
	i2c_buffer[5] = ((offsz >> 8) & 0xFF);
	i2c_buffer[6] = (offsz & 0xFF);
	mikrobus_i2c_write(MPU9250I2CADDR, (uint8_t *)i2c_buffer, 7);

	return RETURN_ERROR;
}
RETURN_STATUS mpu_mag_config(void)
{
	// MPU_MAG_CNTL1 registr
	i2c_buffer[0] = (MPU_MAG_CNTL1);
	i2c_buffer[1] = MPU_MAG_16B_CONT2;		// 14-bitove rozliseni
	mikrobus_i2c_write(MPUMAGNETOADDR, (uint8_t *)i2c_buffer, 2);

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
	// accelerometer a gyroscope
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

	// magnetometer
	i2c_buffer[0] = (MPU_MAGXL); 								// MPU_MAGXL registr
	if (mikrobus_i2c_write(MPUMAGNETOADDR, (uint8_t *)i2c_buffer, 1) != RETURN_OK)
	{
		return RETURN_ERROR;
	}

	// cti data, ve skutecnosti 7 bytu, data a ST2 jako data stop pro kontinualni mod
	if (mikrobus_i2c_read(MPUMAGNETOADDR, (uint8_t *)i2c_buffer, MPU_MAG_DATA_LEN) != RETURN_OK)
	{
		return RETURN_ERROR;
	}

	accgyro_data.magx = i2c_buffer[MPU_MAG_OFFSET] + (i2c_buffer[MPU_MAG_OFFSET+1] << 8);
	accgyro_data.magy = i2c_buffer[MPU_MAG_OFFSET+2] + (i2c_buffer[MPU_MAG_OFFSET+3] << 8);
	accgyro_data.magz = i2c_buffer[MPU_MAG_OFFSET+4] + (i2c_buffer[MPU_MAG_OFFSET+5] << 8);

	return RETURN_OK;
}

accgyro_data_t *mpu_get_data(void)
{
	return (accgyro_data_t *)&accgyro_data;
}

