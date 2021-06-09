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

#define AUTO_CLASSIFIER_LEADTIME					5

typedef enum {
	AUTO_STOPPED = 0,
	AUTO_STARTED,
	AUTO_ERASED,
	AUTO_STORED,
	AUTO_WAITING,
	AUTO_CLASSIFIED,
} classifier_automat_states_t;

typedef struct {
	uint8_t enabled;
	classifier_automat_states_t state;
 	uint8_t taskid;
} classifier_automat_t;

RETURN_STATUS class_init(void);
RETURN_STATUS class_start(void);
RETURN_STATUS class_stop(void);
RETURN_STATUS class_send_enable(void);
RETURN_STATUS class_automat(void);
RETURN_STATUS class_set_state(classifier_automat_states_t state);
classifier_automat_states_t class_get_state(void);
RETURN_STATUS class_received_finished(uint8_t *data, uint16_t len);


#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM4_INC_CLASSIFIER_H_ */
