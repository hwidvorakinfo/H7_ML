/*
 * amp_messaging_cm7.h
 *
 *  Created on: 10. 5. 2021
 *      Author: Petr Dvorak
 */

#ifndef SOURCES_USER_CM7_INC_AMP_MESSAGING_CM7_H_
#define SOURCES_USER_CM7_INC_AMP_MESSAGING_CM7_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx_hal.h"
#include "defs.h"

void new_service_cb(struct rpmsg_device *rdev, const char *name, uint32_t dest);
void amp_set_status(AMP_MESSAGING_STATUS status);
int16_t amp_send_message(uint16_t cmd, void *data, uint16_t length);
void amp_send_alive_message(void);
int16_t amp_receive_message(void);
RETURN_STATUS amp_message_decode(void);
void vprint(const char *fmt, va_list argp);

#ifdef __cplusplus
}
#endif


#endif /* SOURCES_USER_CM7_INC_AMP_MESSAGING_CM7_H_ */
