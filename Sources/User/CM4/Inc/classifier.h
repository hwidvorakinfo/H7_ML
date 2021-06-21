/*
 * classifier.h
 *
 *  Created on: 2. 6. 2021
 *      Author: Petr Dvorak
 */

#ifndef SOURCES_USER_CM4_INC_CLASSIFIER_H_
#define SOURCES_USER_CM4_INC_CLASSIFIER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"
#include "main.h"
#include "application.h"
#include "msg_types.h"
#include "sdramfs.h"

#define AUTO_CLASSIFIER_LEADTIME					5
#define AUTO_FILE_ADC_0								0
#define AUTO_FILE_ADC_1								2
#define AUTO_FILE_SERIAL_0							1
#define AUTO_FILE_SERIAL_1							3


typedef enum {
	AUTO_STOPPED = 0,
	AUTO_STARTED,
	AUTO_STORED,
	AUTO_WAITING,
	AUTO_CLASSIFIED,
} classifier_automat_states_t;

typedef struct {
	uint8_t enabled;
	classifier_automat_states_t state;
 	uint8_t taskid;
 	sdramfs_record_t files[4];
 	uint8_t file_active;
 	uint8_t buffer_active;
} classifier_automat_t;

RETURN_STATUS class_init(void);
RETURN_STATUS class_start(void);
RETURN_STATUS class_stop(void);
RETURN_STATUS class_send_enable(void);
RETURN_STATUS class_automat(void);
RETURN_STATUS class_set_state(classifier_automat_states_t state);
classifier_automat_states_t class_get_state(void);
RETURN_STATUS class_received_finished(uint8_t *data, uint16_t len);
RETURN_STATUS class_data_length_ok(uint16_t freq, uint16_t period);
RETURN_STATUS class_auto_init(void);
RETURN_STATUS class_change_files(void);


#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM4_INC_CLASSIFIER_H_ */
