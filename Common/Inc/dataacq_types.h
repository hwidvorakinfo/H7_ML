/*
 * dataacq_types.h
 *
 *  Created on: 25. 5. 2021
 *      Author: Petr Dvorak
 */

#ifndef COMMON_INC_DATAACQ_TYPES_H_
#define COMMON_INC_DATAACQ_TYPES_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"
#include "main.h"
#include "application.h"

#define MAX_NUM_OF_COLUMNS				10

typedef enum {
 	NONE = 0,
 	AINFIRST,
 	AIN1,
 	AIN2,
 	AIN3,
 	AINLAST,
 	SERIALFIRST,
 	ACCX,
 	ACCY,
 	ACCZ,
	GYROX,
	GYROY,
	GYROZ,
	TEMP,
	MAGX,
	MAGY,
	MAGZ,
 	SERIALLAST,
	TIME,
} dataacq_quantity_t;

typedef enum {
	SERIAL_ACQ_DISABLED = 0,
	SERIAL_ACQ_ENABLED,
} dataacq_serialsetup_t;

typedef struct {
 	uint32_t freq;		// vzorkovaci frekvence Hz od 1 do 32000
 	uint32_t period;	// delka zaznamu v milisekundach
 	uint8_t num_of_columns;
 	uint32_t (*p_getfunc[MAX_NUM_OF_COLUMNS])(void);
 	dataacq_quantity_t quantity_in_column[MAX_NUM_OF_COLUMNS];
 	uint32_t file_for_column[MAX_NUM_OF_COLUMNS];
 	uint8_t taskid;
 	dataacq_serialsetup_t serialsetup;
} dataacq_setup_t;

#define ADCRES_16B					ADC_RESOLUTION_16B
//define ADCRES_14B					ADC_RESOLUTION_14B
//#define ADCRES_12B				ADC_RESOLUTION_12B
//#define ADCRES_10B				ADC_RESOLUTION_10B
#define ADCRANGEV					3.3f

#define ACC_RANGE_2G
//#define ACC_RANGE_4G
//#define ACC_RANGE_8G
//#define ACC_RANGE_16G

#define GYRO_RANGE_250
//#define GYRO_RANGE_500
//#define GYRO_RANGE_1000
//#define GYRO_RANGE_2000


#ifdef __cplusplus
}
#endif

#endif /* COMMON_INC_DATAACQ_TYPES_H_ */
