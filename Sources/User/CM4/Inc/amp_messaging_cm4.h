/*
 * amp_messaging_cm4.h
 *
 *  Created on: 17. 5. 2021
 *      Author: Petr Dvorak
 */

#ifndef SOURCES_USER_CM4_INC_AMP_MESSAGING_CM4_H_
#define SOURCES_USER_CM4_INC_AMP_MESSAGING_CM4_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"

void amp_set_status(AMP_MESSAGING_STATUS status);
void amp_set_timeout(void);

#ifdef __cplusplus
}
#endif


#endif /* SOURCES_USER_CM4_INC_AMP_MESSAGING_CM4_H_ */
