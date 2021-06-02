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

RETURN_STATUS class_send_enable(void);


#ifdef __cplusplus
}
#endif

#endif /* SOURCES_USER_CM4_INC_CLASSIFIER_H_ */
