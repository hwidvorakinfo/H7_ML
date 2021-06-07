/*
 * classifier.h
 *
 *  Created on: 2. 6. 2021
 *      Author: Petr Dvorak
 */

#ifndef SOURCES_USER_CM7_INC_CLASSIFIER_H_
#define SOURCES_USER_CM7_INC_CLASSIFIER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"
#include "main.h"
#include "application.h"
#include "beasth7.h"

typedef enum {
  	STOPPED = 0,
  	ENABLED,
  	RUNNING,
 } classifier_state_t;

typedef struct {
	classifier_state_t state;

 } classifier_t;

RETURN_STATUS classifier_config(void);
RETURN_STATUS classifier_prepare(msg_t *msg);
RETURN_STATUS classifier_set_state(classifier_state_t state);
classifier_state_t classifier_get_state(void);
uint32_t classifier_get_data_address(void);

#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM7_INC_CLASSIFIER_H_ */
