/*
 * dataacq.h
 *
 *  Created on: 18. 5. 2021
 *      Author: Petr Dvorak
 */

#ifndef SOURCES_USER_CM4_INC_DATAACQ_H_
#define SOURCES_USER_CM4_INC_DATAACQ_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"
#include "main.h"
#include "application.h"

#define MAX_NUM_OF_COLUMNS				10

typedef struct {
 	uint16_t stime;
 	uint8_t num_of_columns;
 	uint32_t (*p_getfunc[MAX_NUM_OF_COLUMNS])(void);
 	uint8_t taskid;
 } dataacq_setup_t;

 uint32_t vspfunc(uint16_t index, char *format, ...);

RETURN_STATUS dacq_init(void);
RETURN_STATUS dacq_set_stime(uint16_t time);
RETURN_STATUS daqc_set_colfunc(uint8_t index, void *p_func);
RETURN_STATUS daqc_set_colnum(uint8_t num);
RETURN_STATUS datacq_send_data(void);
RETURN_STATUS dacq_start_acq(void);
RETURN_STATUS dacq_stop_acq(void);

#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM4_INC_DATAACQ_H_ */
